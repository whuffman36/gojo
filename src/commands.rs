use crate::plaintext;
use crate::templates;

use std::collections::hash_map::HashMap;
use std::fs;
use std::io::Error;
use std::io::ErrorKind;
use std::io::Result;
use std::vec::Vec;
use std::path::Path;
use std::path::PathBuf;
use std::process::Command;
use std::process::Stdio;
use std::time;
#[allow(unused_imports)]
use std::os::unix::fs::PermissionsExt;

extern crate num_cpus;

struct GojoConfig {
  pub project_root: String,
  pub build_dir: String,
  pub name: String,
  pub std: String,
  pub cpp: String,
  pub hpp: String,
  pub fmt_style: String,
  pub fmt_args: String,
  pub clang_tidy: bool,
  pub cpplint: bool,
  pub cpplint_args: String,
  pub cppcheck: bool,
  pub cppcheck_args: String,
  pub quiet: bool,
}

const DEFAULT_BUILD_DIR: &'static str = "build";
const CONFIG_FILE: &'static str = ".gojo";

pub fn init(args: &[String]) -> Result<()> {
  if args.is_empty() {
    return Err(Error::new(
      ErrorKind::Other,
      "\x1b[31mincorrect usage:\x1b[0m\n\tgojo init <name> [options]\n\tsee 'gojo help init'\n",
    ));
  }

  let name = args[0].as_str();
  let project_root = std::env::current_dir()?.to_str().unwrap().to_string() + "/" + name;
  let mut std: Option<&str> = None;
  let mut src_extension: Option<&str> = None;
  let mut hdr_extension: Option<&str> = None;
  let mut build_dir: Option<String> = None;
  let mut create_tests = true;
  let mut quiet = false;

  const CXX_STDS: &[&str] = &["11", "14", "17", "20", "23"];
  const CXX_SRC_EXTENSIONS: &[&str] = &["cc", "cpp", "cxx", "c++"];
  const CXX_HDR_EXTENSIONS: &[&str] = &["h", "hpp", "hxx", "h++"];
  const CMAKE_DEFAULT_VERSION: &str = "3.28";

  let arg_map = parse_arguments(&args[1..]);
  for (flag, val) in arg_map {
    match flag {
      "--std" => {
        if val.is_none() || !(CXX_STDS.contains(&val.unwrap())) {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m\n\tunrecognized value for --std flag\n\tsee 'gojo init --help'\n"));
        }
        std = Some(val.unwrap());
      }
      "--src-extension" | "-s" => {
        if val.is_none() || !(CXX_SRC_EXTENSIONS.contains(&val.unwrap())) {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m\n\tunrecognized value for --src-extension flag\n\tsee 'gojo init --help'\n",
          ));
        }
        src_extension = Some(val.unwrap());
      }
      "--hdr-extension" | "-h" => {
        if val.is_none() || !(CXX_HDR_EXTENSIONS.contains(&val.unwrap())) {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m\n\tunrecognized value for --header-extension flag\n\tsee 'gojo init --help'\n",
          ));
        }
        hdr_extension = Some(val.unwrap());
      }
      "--build-dir" | "-b" => {
        if val.is_none() {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m\n\tunrecognized value for --build-dr flag\n\tsee 'gojo init --help'\n",
          ));
        }
        build_dir = Some(format!("{}/{}", project_root.as_str(), val.unwrap()));
      }
      "--no-test" => {
        create_tests = false;
      }
      "--quiet" | "-q" => {
        quiet = true;
      }
      "--help" => {
        println!("Oops! This command hasn't been implemented yet...");
      }
      _ => {
        return Err(Error::new(
          ErrorKind::Other,
          format!(
            "\x1b[31mincorrect usage:\x1b[0m\n\tinvalid option '{}'\n\tsee 'gojo init --help'\n",
            flag
          ),
        ));
      }
    }
  }

  let std_final = std.unwrap_or(std_default());
  let cpp = src_extension.unwrap_or(src_exension_default());
  let hpp = hdr_extension.unwrap_or(hdr_extension_default());

  fs::create_dir(name)?;
  std::env::set_current_dir(Path::new(&format!("./{name}")))?;

  if build_dir.is_none() {
    build_dir = Some(format!("{}/{}", project_root.as_str(), DEFAULT_BUILD_DIR));
  }
  let build = build_dir.unwrap();

  fs::create_dir("src")?;
  fs::create_dir("src/lib")?;
  fs::create_dir(build.as_str())?;

  fs::write(
    "CMakeLists.txt",
    templates::root_cmake_lists_txt(name, std_final, cpp, CMAKE_DEFAULT_VERSION),
  )?;
  fs::write(
    format!("src/main.{cpp}"),
    templates::main_src(hpp).as_bytes(),
  )?;
  fs::write(
    format!("src/lib/hello_world.{cpp}"),
    templates::lib_hello_world_src(hpp).as_bytes(),
  )?;
  fs::write(
    format!("src/lib/hello_world.{hpp}"),
    templates::lib_hello_world_hdr(hpp).as_bytes(),
  )?;
  fs::write(
    "src/lib/CMakeLists.txt",
    templates::lib_cmake_lists_txt(cpp).as_bytes(),
  )?;
  fs::write("README.md", templates::readme(name).as_bytes())?;
  fs::write(".clang-tidy", plaintext::CLANG_TIDY.as_bytes())?;
  fs::write(".gitignore", plaintext::GIT_IGNORE.as_bytes())?;

  if quiet {
    Command::new("git")
      .args(["init"])
      .stdout(Stdio::null())
      .output()?;
  } else {
    print!("\n\x1b[0;35mInitializing Git repository...\x1b[0m\n");
    Command::new("git")
      .args(["init"])
      .stdout(Stdio::inherit())
      .output()?;
  }

  if create_tests {
    fs::create_dir("test")?;
    fs::write(
      format!("test/hello_world_test.{cpp}"),
      templates::test_hello_world_src(hpp).as_bytes(),
    )?;
    fs::write(
      "test/CMakeLists.txt",
      templates::test_cmake_lists_txt(cpp).as_bytes(),
    )?;
  }

  if quiet {
    let config: GojoConfig = GojoConfig {
      project_root: project_root,
      build_dir: build,
      name: name.to_string(),
      std: std_final.to_string(),
      cpp: cpp.to_string(),
      hpp: hpp.to_string(),
      fmt_style: String::from("google"),
      fmt_args: String::new(),
      clang_tidy: true,
      cpplint: false,
      cpplint_args: String::new(),
      cppcheck: true,
      cppcheck_args: String::new(),
      quiet: false
    };

    config_write(config)?;
  } else {
    let config: GojoConfig = GojoConfig {
      project_root: project_root.clone(),
      build_dir: build.clone(),
      name: name.to_string(),
      std: std_final.to_string(),
      cpp: cpp.to_string(),
      hpp: hpp.to_string(),
      fmt_style: String::from("google"),
      fmt_args: String::new(),
      clang_tidy: true,
      cpplint: false,
      cpplint_args: String::new(),
      cppcheck: true,
      cppcheck_args: String::new(),
      quiet: false
    };

    config_write(config)?;
    print!("\n\x1b[1;32mCreated gojo project:\x1b[0m {}\n\t\x1b[1;35mroot:\x1b[0m {}\n\t\x1b[1;35mconfig:\x1b[0m {}/.gojo\n", name, project_root.as_str(), project_root.as_str());
  }
  Ok(())
}

pub fn build(args: &[String]) -> Result<()> {
  let mut build_type = "-DCMAKE_BUILD_TYPE=Debug";
  let mut test = "-DBUILD_TESTING=OFF";
  let mut clean_build = false;
  let mut quiet = false;

  let arg_map = parse_arguments(&args);
  for (flag, _) in arg_map {
    match flag {
      "--release" | "-r" => {
        build_type = "-DCMAKE_BUILD_TYPE=Release";
      }
      "--tests" | "-t" => {
        test = "-DBUILD_TESTING=ON";
      }
      "--clean" | "-c" => {
        clean_build = true;
      }
      "--quiet | -q" => {
        quiet = true;
      }
      "--help" => {
        println!("Oops! This command hasn't been implemented yet...");
      }
      _ => {
        return Err(Error::new(
          ErrorKind::Other,
          format!(
            "\x1b[31mincorrect usage:\x1b[0m\n\tinvalid option '{}'\n\tsee 'gojo build --help'\n",
            flag
          ),
        ));
      }
    }
  }

  if clean_build {
    clean()?;
  }

  let config_result = config_read();
  let mut build_dir = String::from(DEFAULT_BUILD_DIR);
  let mut name = String::from("project");
  if config_result.is_some() {
    let config = config_result.unwrap();
    build_dir = config.build_dir;
    name = config.name;
  }

  if !fs::exists(build_dir.as_str())? {
    fs::create_dir(build_dir.as_str())?;
  }

  let num_cores = num_cpus::get().to_string();

  if quiet {
    Command::new("cmake")
    .args([test, build_type, "-S", ".", "-B", build_dir.as_str()])
    .stdout(Stdio::null())
    .stderr(Stdio::inherit())
    .output()?;
    Command::new("cmake")
      .args(["--build", "build", "-j", num_cores.as_str()])
      .stdout(Stdio::null())
      .stderr(Stdio::inherit())
      .output()?;
    return Ok(())
  }

  print!("\n\x1b[0;35mInitliazing CMake in\x1b[0m {}\n", build_dir.as_str());
  Command::new("cmake")
    .args([test, build_type, "-S", ".", "-B", build_dir.as_str()])
    .stdout(Stdio::null())
    .stderr(Stdio::inherit())
    .output()?;

  let mode: Vec<&str> = build_type.split("=").collect();
  print!("\x1b[1;35mCompiling\x1b[0m {} \x1b[1;35min\x1b[0m \x1b[1;36m{}\x1b[0m \x1b[1;35mmode\x1b[0m\n\n", name, mode[1]);
  let start = time::Instant::now();
  Command::new("cmake")
    .args(["--build", "build", "-j", num_cores.as_str()])
    .stdout(Stdio::inherit())
    .stderr(Stdio::inherit())
    .output()?;

  let total_time = start.elapsed();
  print!("\n\x1b[1;32mBuild successful\x1b[0m ({}s)\n\n", total_time.as_secs());
  Ok(())
}

pub fn run(args: &[String]) -> Result<()> {
  let config_result = config_read();
  let mut build_dir = String::from(DEFAULT_BUILD_DIR);
  let mut name = String::new();
  if config_result.is_some() {
    let config = config_result.unwrap();
    build_dir = config.build_dir;
    name = config.name;
  }

  if !fs::exists(build_dir.as_str())? {
    return Err(Error::new(
      ErrorKind::Other,
      "\x1b[31mfile not found:\x1b[0m\n\tno build directory discovered for this project\n",
    ));
  }

  if !args.is_empty() && args[0] == "--help" {
    println!("Oops! This command hasn't been implemented yet...");
  }

  // Check if name was supplied in argument list
  if !args.is_empty() && !args[0].starts_with("--") {
    let name = &args[0];
    Command::new(name)
      .args(&args[1..])
      .stdout(Stdio::inherit())
      .stdin(Stdio::inherit())
      .stderr(Stdio::inherit())
      .output()?;
    return Ok(());
  }

  // Check gojo config file for executable name
  if !name.is_empty() {
    if fs::exists(format!("{}/{}", build_dir.as_str(), name.as_str()))? {
      Command::new(format!("{}/{}", build_dir, name))
        .args(args)
        .stdout(Stdio::inherit())
        .stdin(Stdio::inherit())
        .stderr(Stdio::inherit())
        .output()?;
      return Ok(());
    }
  }

  return Err(Error::new(
    ErrorKind::Other,
    "\x1b[31mfile not found:\x1b[0m\n\tno executable target found\n",
  ));
}

pub fn test() -> Result<()> {
  let config_result = config_read();
  let mut build_dir = String::from(DEFAULT_BUILD_DIR);
  if config_result.is_some() {
    build_dir = config_result.unwrap().build_dir;
  }

  std::env::set_var("GTEST_COLOR", "1");
  Command::new("ctest")
    .args(["-V"])
    .current_dir(build_dir)
    .stdout(Stdio::inherit())
    .output()?;

  println!();
  Ok(())
}

pub fn clean() -> Result<()> {
  let config_result = config_read();
  let mut build_dir = String::from(DEFAULT_BUILD_DIR);
  if config_result.is_some() {
    build_dir = config_result.unwrap().build_dir;
  }

  if !fs::exists(build_dir.as_str())? {
    fs::create_dir(build_dir.as_str())?;
  }

  let deps = format!("{}/_deps", build_dir.as_str());
  if fs::exists(deps.as_str())? {
    fs::rename(deps.as_str(), "_deps")?;
    fs::remove_dir_all(build_dir.as_str())?;
    fs::create_dir(build_dir.as_str())?;
    fs::rename("_deps", deps.as_str())?;
    return Ok(());
  }
  fs::remove_dir_all(build_dir.as_str())?;
  fs::create_dir(build_dir.as_str())?;
  Ok(())
}

pub fn fmt(args: &[String]) -> Result<()> {
  let styles = [
    "llvm",
    "google",
    "chromium",
    "mozilla",
    "webkit",
    "microsoft",
    "gnu",
  ];
  let mut style = "google";
  let mut file = false;
  let mut in_place = true;

  let config_result = config_read();
  let mut config = config_default();
  if config_result.is_some() {
    config = config_result.unwrap();
  }

  let project_root = config.project_root.as_str();
  let fmt_style = config.fmt_style.as_str();

  if !fmt_style.is_empty() {
    style = fmt_style;
  }

  let arg_map = parse_arguments(&args);
  for (flag, val) in arg_map {
    match flag {
      "--style" => {
        if val.is_none() || !styles.contains(&val.unwrap()) {
          return Err(Error::new(
            ErrorKind::Other,
            format!(
              "\x1b[31mincorrect usage:\x1b[0m\n\tstyle not found: {}\n\tsee 'gojo fmt --help'\n",
              &val.unwrap()
            ),
          ));
        }
        style = val.unwrap();
      }
      "--file" => {
        file = true;
      }
      "--in-place" | "-i" => {
        in_place = true;
      }
      "--help" => {
        println!("Oops! This command hasn't been implemented yet...");
      }
      _ => {
        return Err(Error::new(
          ErrorKind::Other,
          format!(
            "\x1b[31mincorrect usage:\x1b[0m\n\tinvalid option '{}'\n\tsee 'gojo fmt --help'\n",
            args[0].as_str()
          ),
        ));
      }
    }
  }

  if file {
    if !fs::exists(format!("{}/.clang-format", project_root))? {
      return Err(Error::new(
        ErrorKind::Other,
        "\x1b[31mfile not found:\x1b[0m\n\tno .clang-format file found\n\tsee 'gojo fmt --help'\n",
      ));
    }
    style = "file";
  }


  config_write(GojoConfig {
    project_root: config.project_root.as_str().to_string(),
    build_dir: config.build_dir,
    name: config.name,
    std: config.std,
    cpp: config.cpp,
    hpp: config.hpp,
    fmt_style: style.to_string(),
    fmt_args: config.fmt_args,
    clang_tidy: config.clang_tidy,
    cpplint: config.cpplint,
    cpplint_args: config.cpplint_args,
    cppcheck: config.cppcheck,
    cppcheck_args: config.cppcheck_args,
    quiet: config.quiet
  })?;

  let mut src_files: Vec<String> =
    vec![String::from(format!("-style={style}"))];
  if in_place {
    src_files.push(String::from("-i"));
  }
  collect_src_files(PathBuf::from(format!("{}/src", project_root)), &mut src_files)?;
  collect_src_files(PathBuf::from(format!("{}/test", project_root)), &mut src_files)?;

  Command::new("clang-format")
    .args(src_files.as_slice())
    .stdin(Stdio::null())
    .stdout(Stdio::inherit())
    .stderr(Stdio::inherit())
    .output()?;

  Ok(())
}

pub fn check() -> Result<()> {
  let mut config = config_default();
  let config_result = config_read();
  if config_result.is_some() {
    config = config_result.unwrap();
  }

  if !fs::exists(config.build_dir.as_str())? {
    fs::create_dir(config.build_dir.as_str())?;
  }

  print!("\x1b[1;35mRunning checks...\x1b[0m\n");
  let start = time::Instant::now();

  if config.cpplint {
    print!("\x1b[0;35mRunning cpplint...\x1b[0m\n\n");

    let cpplint_start = time::Instant::now();
    // TODO: Run cpplint
    let cpplint_time = cpplint_start.elapsed();

    print!("\n\x1b[1;32mcpplint passed\x1b[0m ({}s)\n\n", cpplint_time.as_secs());
  }

  if config.cppcheck {
    let mut src_files: Vec<String> = vec![
      String::from("--enable=warning,performance,portability"),
      String::from("--force"),
      String::from("--language=c++"),
      format!("--std=c++{}", config.std.as_str()),
    ];
    collect_src_files(PathBuf::from(format!("{}/src", config.project_root.as_str())), &mut src_files)?;
    collect_src_files(PathBuf::from(format!("{}/test", config.project_root.as_str())), &mut src_files)?;

    print!("\x1b[0;35mRunning cppcheck...\x1b[0m\n\n");

    let cppcheck_start = time::Instant::now();
    Command::new("cppcheck")
      .args(src_files.as_slice())
      .stdout(Stdio::inherit())
      .stderr(Stdio::inherit())
      .output()?;

    let cppcheck_time = cppcheck_start.elapsed();
    print!("\n\x1b[1;32mcppcheck passed\x1b[0m ({}s)\n\n", cppcheck_time.as_secs());
  }

  let num_cores = num_cpus::get().to_string();

  if config.clang_tidy {
    print!("\x1b[0;35mRunning clang-tidy...\x1b[0m\n\n");
    print!("\x1b[0;35mInitliazing CMake in\x1b[0m {}\n", config.build_dir.as_str());
    let compile_start = time::Instant::now();
    Command::new("cmake")
      .args([
        "-DCMAKE_BUILD_TYPE=Release",
        "-DBUILD_TESTING=ON",
        "-DSTATIC_CHECK=ON",
        "-S",
        ".",
        "-B",
        config.build_dir.as_str(),
      ])
      //.stdout(Stdio::inherit())
      .stdout(Stdio::null())
      .stderr(Stdio::inherit())
      .output()?;

    print!("\x1b[1;35mCompiling\x1b[0m {} \x1b[1;35min\x1b[0m \x1b[1;36mRelease\x1b[0m \x1b[1;35mmode\x1b[0m\n\n", config.name);
    Command::new("cmake")
      .args(["--build", config.build_dir.as_str(), "-j", num_cores.as_str()])
      .stdout(Stdio::null())
      .stderr(Stdio::inherit())
      .output()?;

    let compile_time = compile_start.elapsed();
    print!("\x1b[1;32mBuild successful\x1b[0m ({}s)\n\n", compile_time.as_secs());
    
    print!("\x1b[1;32mclang-tidy passed\x1b[0m ({}s)\n", compile_time.as_secs());
    }

    let total_time = start.elapsed();
    print!("\x1b[1;32mAll checks passed\x1b[0m ({}s)\n\n", total_time.as_secs());
  Ok(())
}

pub fn help() {
  println!("{}", plaintext::HELP);
}


fn parse_arguments(args: &[String]) -> HashMap<&str, Option<&str>> {
  let mut parsed_args = HashMap::new();

  let mut iter = args.iter();
  let mut arg = iter.next();
  while arg.is_some() {
    let arg_str = arg.unwrap().as_str();

    if arg_str.starts_with("--") || arg_str.starts_with("-") {
      if arg_str.contains("=") {
        let split: Vec<&str> = arg_str.split("=").collect();
        parsed_args.insert(split[0], Some(split[1]));
      } else {
        let value = iter.next();
        if value.is_some() {
          parsed_args.insert(arg_str, Some(value.unwrap().as_str()));
        } else {
          parsed_args.insert(arg_str, None);
        }
      }
    } else {
      // Non flag argument. Should produce error.
      parsed_args.insert(arg_str, None);
    }
    arg = iter.next();
  }
  parsed_args
}

fn collect_src_files(
  path: PathBuf,
  src_files: &mut Vec<String>,
) -> Result<()> {
  let config_result = config_read();
  let mut config = config_default();
  if config_result.is_some() {
    config = config_result.unwrap();
  }

  for entry in fs::read_dir(path)? {
    let entry = entry?;
    if entry.file_name().to_str().unwrap().ends_with(config.cpp.as_str()) {
      src_files.push(String::from(entry.path().to_str().unwrap()));
    } else if entry.file_name().to_str().unwrap().ends_with(config.hpp.as_str()) {
      src_files.push(String::from(entry.path().to_str().unwrap()));
    } else if entry.file_type().unwrap().is_dir() {
      collect_src_files(entry.path(), src_files)?;
    }
  }
  Ok(())
}

fn config_write(config: GojoConfig) -> Result<()> {
  fs::write(CONFIG_FILE,
    format!(
"project_root: {}
build_dir: {}
name: {}
std: {}
src: {}
hdr: {}
fmt_style: {}
fmt_args: {}
clang-tidy: {}
cpplint: {}
cpplint_args: {}
cppcheck: {}
cppcheck_args: {}
quiet: {}",
      config.project_root,
      config.build_dir,
      config.name,
      config.std,
      config.cpp,
      config.hpp,
      config.fmt_style,
      config.fmt_args,
      config.clang_tidy,
      config.cpplint,
      config.cpplint_args,
      config.cppcheck,
      config.cppcheck_args,
      config.quiet
    )
  )?;
  Ok(())
}

fn config_read() -> Option<GojoConfig> {
  let config_result = fs::read_to_string(CONFIG_FILE);
  if config_result.is_err() {
    eprintln!("\x1b[31mfile not found:\x1b[0m \n\tno gojo config file found");
    return None;
  }

  let file_contents = config_result.unwrap();
  let parsed_config: Vec<&str> = file_contents.split("\n").collect();
  let parsed_project_root: Vec<&str> = parsed_config[0].split(":").collect();
  let parsed_build_dir: Vec<&str> = parsed_config[1].split(":").collect();
  let parsed_name: Vec<&str> = parsed_config[2].split(":").collect();
  let parsed_std: Vec<&str> = parsed_config[3].split(":").collect();
  let parsed_cpp: Vec<&str> = parsed_config[4].split(":").collect();
  let parsed_hpp: Vec<&str> = parsed_config[5].split(":").collect();
  let parsed_fmt_style: Vec<&str> = parsed_config[6].split(":").collect();
  let parsed_fmt_args: Vec<&str> = parsed_config[7].split(":").collect();
  let parsed_clang_tidy: Vec<&str> = parsed_config[8].split(":").collect();
  let clang_tidy: bool = match parsed_clang_tidy[1].trim() {
    "true" => {
      true
    }
    _ => {
      false
    }
  };
  let parsed_cpplint: Vec<&str> = parsed_config[9].split(":").collect();
  let cpplint: bool = match parsed_cpplint[1].trim() {
    "true" => {
      true
    }
    _ => {
      false
    }
  };
  let parsed_cpplint_args: Vec<&str> = parsed_config[10].split(":").collect();
  let parsed_cppcheck: Vec<&str> = parsed_config[11].split(":").collect();
  let cppcheck: bool = match parsed_cppcheck[1].trim() {
    "true" => {
      true
    }
    _ => {
      false
    }
  };
  let parsed_cppcheck_args: Vec<&str> = parsed_config[12].split(":").collect();
  let parsed_quiet: Vec<&str> = parsed_config[13].split(":").collect();
  let quiet: bool = match parsed_quiet[1].trim() {
    "true" => {
      true
    }
    _ => {
      false
    }
  };

  Some(GojoConfig { 
    project_root: String::from(parsed_project_root[1].trim()),
    build_dir: String::from(parsed_build_dir[1].trim()),
    name: String::from(parsed_name[1].trim()),
    std: String::from(parsed_std[1].trim()),
    cpp: String::from(parsed_cpp[1].trim()),
    hpp: String::from(parsed_hpp[1].trim()),
    fmt_style: String::from(parsed_fmt_style[1].trim()),
    fmt_args: String::from(parsed_fmt_args[1].trim()),
    clang_tidy: clang_tidy,
    cpplint: cpplint,
    cpplint_args: String::from(parsed_cpplint_args[1].trim()),
    cppcheck: cppcheck,
    cppcheck_args: String::from(parsed_cppcheck_args[1].trim()),
    quiet: quiet
  })
}

fn config_default() -> GojoConfig {
  let project_root = String::from(std::env::current_dir().unwrap().to_str().unwrap());
  let build_dir = format!("{}/{}", project_root.as_str(), DEFAULT_BUILD_DIR);

  GojoConfig {
    project_root: project_root,
    build_dir: build_dir,
    name: String::from("project"),
    std: String::from(std_default()),
    cpp: String::from(src_exension_default()),
    hpp: String::from(hdr_extension_default()),
    fmt_style: String::from("google"),
    fmt_args: String::from("-i,"),
    clang_tidy: true,
    cpplint: false,
    cpplint_args: String::new(),
    cppcheck: false,
    cppcheck_args: String::new(),
    quiet: false
  }
}

fn std_default() -> &'static str {
  "20"
}

fn src_exension_default() -> &'static str {
  #[cfg(target_os = "linux")]
  {
    "cc"
  }
  #[cfg(target_os = "macos")]
  {
    "cc"
  }
  #[cfg(target_os = "windows")]
  {
    "cpp"
  }
}

fn hdr_extension_default() -> &'static str {
  #[cfg(target_os = "linux")]
  {
    "h"
  }
  #[cfg(target_os = "macos")]
  {
    "h"
  }
  #[cfg(target_os = "windows")]
  {
    "hpp"
  }
}
