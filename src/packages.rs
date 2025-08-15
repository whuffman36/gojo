use std::process::Command;
use std::process::Stdio;
use std::fs;
use std::io::Result;
use std::env;
use std::io::Error;
use std::io::ErrorKind;
use std::path::PathBuf;
use std::vec::Vec;

fn collect_files(path: PathBuf) -> Vec<String> {
  let mut files = Vec::new();
  for entry in fs::read_dir(path).unwrap() {
    let entry = entry.unwrap();
    if !entry.file_type().unwrap().is_dir() {
      files.push(String::from(entry.file_name().to_str().unwrap()));
    }
  }
  files
}

pub fn install_gtest() -> Result<()> {
  let tmp = env::home_dir().unwrap();
  let home = tmp.to_str().unwrap();
  let result = Command::new("git").args(["clone", "https://github.com/google/googletest.git"]).current_dir(format!("{home}/.gojo/repos")).stdout(Stdio::inherit()).stderr(Stdio::inherit()).output()?;
  let status = result.status;
    if !status.success() {
      return Err(Error::new(
          ErrorKind::Other,
          "\x1b[31mfailed to clone git repo\x1b[0m\n\n"
      ));
    }

  fs::create_dir_all(format!("{home}/.gojo/repos/googletest/build"))?;
  Command::new("cmake").args([".."]).current_dir(format!("{home}/.gojo/repos/googletest/build")).stdout(Stdio::inherit()).stderr(Stdio::inherit()).output()?;
  Command::new("cmake").args(["--build", "."]).current_dir(format!("{home}/.gojo/repos/googletest/build")).stdout(Stdio::inherit()).stderr(Stdio::inherit()).output()?;

  fs::create_dir_all(format!("{home}/.gojo/lib/gtest"))?;
  fs::create_dir_all(format!("{home}/.gojo/lib/gmock"))?;
  fs::create_dir_all(format!("{home}/.gojo/include/gtest"))?;
  fs::create_dir_all(format!("{home}/.gojo/include/gmock"))?;
  
  let lib_files = collect_files(PathBuf::from(format!("{home}/.gojo/repos/googletest/build/lib")));
  for file in lib_files {
    fs::copy(format!("{home}/.gojo/repos/googletest/build/lib/{file}"), format!("{home}/.gojo/lib/gtest/{file}"))?;
  }
  let test_include_files = collect_files(PathBuf::from(format!("{home}/.gojo/repos/googletest/googletest/include/gtest")));
  for file in test_include_files {
    fs::copy(format!("{home}/.gojo/repos/googletest/googletest/include/gtest/{file}"), format!("{home}/.gojo/include/gtest/{file}"))?;
  }
  
  let mock_include_files = collect_files(PathBuf::from(format!("{home}/.gojo/repos/googletest/googlemock/include/gmock")));
  for file in mock_include_files {
    fs::copy(format!("{home}/.gojo/repos/googletest/googlemock/include/gmock/{file}"), format!("{home}/.gojo/include/gmock/{file}"))?;
  }
  println!("gtest successfully installed");
  Ok(())
}