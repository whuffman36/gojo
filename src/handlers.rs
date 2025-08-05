use crate::file_contents;

use std::fs;
use std::io::Error;
use std::io::ErrorKind;
use std::io::Result;
use std::os::unix::fs::PermissionsExt;


struct GojoConfig {
  name: String,
  src_extension: String,
  header_extension: String
}


pub fn init(args: &[String]) -> Result<()> {
  if args.len() < 3 {
    return Err(Error::new(
      ErrorKind::Other,
      "\x1b[31mincorrect usage:\x1b[0m \n\tgojo init <name> [options]\n\tsee 'gojo help init'\n",
    ));
  }

  let name = args[2].as_str();
  let mut compiler: Option<&String> = None;
  let mut cpp_version: Option<&String> = None;
  let mut description: Option<&String> = None;
  let mut src_extension: Option<&String> = None;
  let mut header_extension: Option<&String> = None;
  let mut create_tests = true;
  let mut create_readme = true;
  let mut create_git = true;

  let mut options = args.iter();
  options.next(); // gojo
  options.next(); // init
  options.next(); // <name>
  loop {
    let option = options.next();
    if option.is_none() {
      break;
    }
    match option.unwrap().as_str() {
      "--compiler" => {
        let value = options.next();
        if value.is_none() || (value.unwrap() != "g++" && value.unwrap() != "clang++") {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m \n\tcompiler option must be \"g++\" or \"clang++\"\n",
          ));
        }
        compiler = value;
      }
      "--cpp-version" => {
        let value = options.next();
        if value.is_none() || !(["11", "14", "17", "20", "23"].contains(&value.unwrap().as_str())) {
          return Err(Error::new(ErrorKind::Other, "\x1b[31mincorrect usage:\x1b[0m \n\tcpp-version option must be one of the following: 11, 14, 17, 20, 23 \n\tsee 'gojo help init'\n"));
        }
        cpp_version = value;
      }
      "--description" => {
        let value = options.next();
        if value.is_none() {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m \n\tdescription argument specified with no value supplied \n\tsee 'gojo help init'\n",
          ));
        }
        description = value;
      }
      "--src-extension" => {
        let value = options.next();
        if value.is_none() {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m \n\tsrc-extension argument specified with no value supplied \n\tsee 'gojo help init'\n",
          ));
        }
        src_extension = value;
      }
      "--h-extension" => {
        let value = options.next();
        if value.is_none() {
          return Err(Error::new(
            ErrorKind::Other,
            "\x1b[31mincorrect usage:\x1b[0m \n\th-extension argument specified with no value supplied \n\tsee 'gojo help init'\n",
          ));
        }
        header_extension = value;
      }
      "--no-tests" => {
        create_tests = false;
      }
      "--no-readme" => {
        create_readme = false;
      }
      "--no-git" => {
        create_git = false;
      }
      _ => {
        return Err(Error::new(
          ErrorKind::Other,
          format!(
            "\x1b[31mincorrect usage:\x1b[0m \n\tinvalid option {}\n",
            option.unwrap().as_str()
          ),
        ));
      }
    };
  }

  let src_ext = src_extension.unwrap_or(&String::from("cc")).clone();
  let h_ext = header_extension.unwrap_or(&String::from("h")).clone();

  fs::create_dir(name)?;
  std::env::set_current_dir(std::path::Path::new(&format!("./{name}")))?;
  fs::create_dir("src")?;
  fs::create_dir("src/lib")?;
  fs::create_dir("build")?;
  fs::write(format!("src/main.{src_ext}"), file_contents::main_src(h_ext.as_str()).as_bytes())?;
  fs::write(
    "CMakeLists.txt",
    file_contents::root_cmake_lists_txt(name, compiler, cpp_version, description, src_ext.as_str()),
  )?;
  fs::write(
    format!("src/lib/hello_world.{h_ext}"),
    file_contents::hello_world_header(h_ext.as_str()).as_bytes(),
  )?;
  fs::write(
    format!("src/lib/hello_world.{src_ext}"),
    file_contents::hello_world_src(h_ext.as_str()).as_bytes(),
  )?;
  fs::write(
    "src/lib/CMakeLists.txt",
    file_contents::lib_cmake_lists_txt(src_ext.as_str()).as_bytes(),
  )?;
  fs::write(".clang-tidy", file_contents::CLANG_TIDY.as_bytes())?;

  if create_tests {
    fs::create_dir("test")?;
    fs::write(
      format!("test/test_hello_world.{src_ext}"),
      file_contents::test_hello_world_src(h_ext.as_str()).as_bytes(),
    )?;
    fs::write(
      "test/CMakeLists.txt",
      file_contents::test_cmake_lists_txt(src_ext.as_str()).as_bytes(),
    )?;
  }

  if create_readme {
    fs::write("README.md", file_contents::readme(name).as_bytes())?;
  }

  if create_git {
    std::process::Command::new("git").arg("init").output()?;
    fs::write(".gitignore", file_contents::GIT_IGNORE.as_bytes())?;
  }

  fs::write("build/.gojo", format!(
"name: {name}
src_exension: {src_ext}
header_extension: {h_ext}"
  ))?;

  Ok(())
}


pub fn build(args: &[String]) -> Result<()> {
  let mut options = args.iter();
  let mut build_mode = "-DCMAKE_BUILD_TYPE=Debug";
  options.next(); // gojo
  options.next(); // build

  loop {
    let option = options.next();
    if option.is_none() {
      break;
    }
    match option.unwrap().as_str() {
      "--release" => {
        build_mode = "-DCMAKE_BUILD_TYPE=Release";
      }
      _ => {
        return Err(Error::new(
          ErrorKind::Other,
          format!(
            "\x1b[31mincorrect usage:\x1b[0m \n\tinvalid option {}\n\tsee 'gojo help build'\n",
            option.unwrap().as_str()
          ),
        ));
      }
    };
  }
  std::process::Command::new("cmake")
    .args(["-DBUILD_TESTING=OFF", build_mode, "-S", ".", "-B", "build"])
    .output()?;
  std::process::Command::new("cmake")
    .args(["--build", "build", "-j", "8"])
    .stdout(std::process::Stdio::inherit())
    .output()?;
  Ok(())
}


pub fn run(args: &[String]) -> Result<()> {
  if !fs::exists("build")? {
    return Err(Error::new(
      ErrorKind::Other,
      "\x1b[31mfile not found:\x1b[0m \n\tno build directory discovered for this project\n\ttry gojo init <name>",
    ));
  }

  // Check if name was supplied in argument list
  if args.len() > 2 && !args[2].starts_with("--") {
    let name = &args[2];
    std::process::Command::new(format!("build/{name}"))
      .args(&args[3..])
      .stdout(std::process::Stdio::inherit())
      .stdin(std::process::Stdio::inherit())
      .stderr(std::process::Stdio::inherit())
      .output()?;
    return Ok(());
  }

  // Check gojo config file for executable name
  let config = gojo_config();
  if config.is_some() {
    if fs::exists(format!("build/{}", config.as_ref().unwrap().name))? {
      std::process::Command::new(format!("build/{}", config.unwrap().name))
        .args(&args[2..])
        .stdout(std::process::Stdio::inherit())
        .stdin(std::process::Stdio::inherit())
        .stderr(std::process::Stdio::inherit())
        .output()?;
      return Ok(());
    }
  }

  // Search build directory for executable
  let mut executable_found = false;
  let mut executable = String::new();
  for entry in fs::read_dir("build")? {
    let entry = entry?;
    if entry.file_type()?.is_file() {
      let permissions = entry.metadata()?.permissions().mode();
      // Permissions code for file with read and execute permissions
      if permissions == 0o0100755 {
        if executable_found {
          return Err(Error::new(ErrorKind::Other, "\x1b[31mmultiple executables:\x1b[0m \n\tplease specify which executable to run\n\tsee 'gojo help run'\n"));
        }
        executable_found = true;
        executable = entry.file_name().into_string().unwrap();
      }
    }
  }
  if !executable_found {
    return Err(Error::new(
      ErrorKind::Other,
      "\x1b[31mfile not found:\x1b[0m \n\tno executable discovered for this project\n\ttry gojo build\n",
    ));
  }

  std::process::Command::new(format!("build/{executable}"))
    .args(&args[2..])
    .stdout(std::process::Stdio::inherit())
    .stdin(std::process::Stdio::inherit())
    .stderr(std::process::Stdio::inherit())
    .output()?;
  Ok(())
}


pub fn test() -> Result<()> {
  std::process::Command::new("cmake")
    .args(["-DBUILD_TESTING=ON", "-S", ".", "-B", "build"])
    .output()?;
  std::process::Command::new("cmake")
    .args(["--build", "build", "-j", "8"])
    .stdout(std::process::Stdio::inherit())
    .output()?;
  std::env::set_var("GTEST_COLOR", "1");
  std::process::Command::new("ctest")
    .args(["-V"])
    .current_dir("build")
    .stdout(std::process::Stdio::inherit())
    .output()?;
  Ok(())
}


pub fn clean() -> Result<()> {
  fs::rename("build/_deps", "_deps")?;
  fs::rename("build/.gojo", ".gojo")?;
  fs::remove_dir_all("build")?;
  fs::create_dir("build")?;
  fs::rename("_deps", "build/_deps")?;
  fs::rename(".gojo", "build/.gojo")?;
  Ok(())
}


pub fn fmt(args: &[String]) -> Result<()> {
  let mut style = "google";
  if args.len() > 2 {
    if args[2] != "--style" {
      return Err(Error::new(ErrorKind::Other, "\x1b[31mincorrect usage:\x1b[0m \n\tgojo fmt --style <style>\n\tsee 'gojo help fmt'\n"));
    }
    if !["llvm", "google", "chromium", "mozilla", "webkit", "microsoft", "gnu"].contains(&args[3].as_str()) {
      return Err(Error::new(ErrorKind::Other, format!("\x1b[31mincorrect usage:\x1b[0m \n\tstyle not found: {}\n\tsee 'gojo help fmt'\n", args[3])));
    }
    style = args[3].as_str();
  }
  let mut src_files: std::vec::Vec<String> = vec![String::from(format!("-style={style}")), String::from("-i")];
  collect_src_files(std::path::PathBuf::from("src"), &mut src_files);
  collect_src_files(std::path::PathBuf::from("test"), &mut src_files);

  std::process::Command::new("clang-format")
    .args(src_files.as_slice())
    .stdout(std::process::Stdio::inherit())
    .stderr(std::process::Stdio::inherit())
    .output()?;

  Ok(())
}


pub fn check() -> Result<()> {
  std::process::Command::new("cmake")
    .args(["-DBUILD_TESTING=ON", "-DSTATIC_CHECK=ON", "-S", ".", "-B", "build"])
    .stdout(std::process::Stdio::inherit())
    .stderr(std::process::Stdio::inherit())
    .output()?;
  std::process::Command::new("cmake")
    .args(["--build", "build", "-j", "8"])
    .stdout(std::process::Stdio::inherit())
    .stderr(std::process::Stdio::inherit())
    .output()?;
  Ok(())
}

/*
pub fn new_class(args: &[String]) -> Result<()> {
  let mut options = args.iter();
  options.next(); // gojo
  options.next(); // new-class
  let name = options.next();
  if name.is_none() {
    return Err(Error::new(
          ErrorKind::Other,
          "\x1b[31mincorrect usage:\x1b[0m \n\tclass name not specified \n\tgojo new-class <NAME> [OPTIONS]"),
    );
  }

  let mut path = "src/";
  loop {
    let option = options.next();
    if option.is_none() {
      break;
    }
    match option.unwrap().as_str() {
      "--path" => {
        let value = options.next();
        if value.is_none() {
          return Err(Error::new(
          ErrorKind::Other,
          "\x1b[31mincorrect usage:\x1b[0m \n\tpath argument specified with no value supplied\n\tsee 'gojo help new-class'\n"),
          );
        }
        path = value.unwrap();
      }
      _ => {
        return Err(Error::new(
          ErrorKind::Other,
          format!(
            "\x1b[31mincorrect usage:\x1b[0m \n\tinvalid option {} \n\tsee 'gojo help new-class'\n",
            option.unwrap().as_str()
          ),
        ));
      }
    };
  }


  Ok(())
}
*/

pub fn help() -> Result<()> {
  println!("{}", file_contents::HELP);
  gojo_config();
  Ok(())
}


fn collect_src_files(path: std::path::PathBuf, src_files: &mut std::vec::Vec<String>) {
  for entry in fs::read_dir(path).unwrap() {
    let entry = entry.unwrap();
    if entry.file_name().to_str().unwrap().ends_with(".cpp") {
      src_files.push(String::from(entry.path().to_str().unwrap()));
    }
    else if entry.file_name().to_str().unwrap().ends_with(".hpp") {
      src_files.push(String::from(entry.path().to_str().unwrap()));
    }
    else if entry.file_type().unwrap().is_dir() {
      collect_src_files(entry.path(), src_files);
    }
  }
}


fn gojo_config() -> Option<GojoConfig> {
  let config_result = fs::read_to_string("build/.gojo");
  if config_result.is_err() {
    eprintln!("\x1b[31mfile not found:\x1b[0m \n\tno gojo config file found");
    //return None;
  }
  let file_contents = config_result.unwrap();
  let parsed_config: std::vec::Vec<&str> = file_contents.split("\n").collect();
  let parsed_name: std::vec::Vec<&str> = parsed_config[0].split(":").collect();
  let parsed_src_ext: std::vec::Vec<&str> = parsed_config[1].split(":").collect();
  let parsed_h_ext: std::vec::Vec<&str> = parsed_config[2].split(":").collect();
  let name = parsed_name[1].trim_start().trim_end();
  let src_ext = parsed_src_ext[1].trim_start().trim_end();
  let h_ext = parsed_h_ext[1].trim_start().trim_end();

  Some(
    GojoConfig {
      name: String::from(name),
      src_extension: String::from(src_ext),
      header_extension: String::from(h_ext),
    }
  )
}