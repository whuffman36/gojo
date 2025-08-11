mod commands;
mod plaintext;
mod templates;

// TODO
// * update command
//    - maybe keep a copy of the repo in .gojo file? git pull it?
// * download some dependencies upon init
//    - cppcheck, cpplint, git, gcc, clang, cmake
// * command to install specific libs that i like
// * how to edit CMakeLists.txt file from rust?
// * add a command to create a new PR


fn main() {
  let args: std::vec::Vec<String> = std::env::args().collect();
  if args.len() == 1 {
    println!("{}", plaintext::WIN);
    commands::help();
    return;
  }

  let command = args[1].as_str();
  let mut command_args: &[String] = &[];
  if args.len() > 2 {
    command_args = args[2..].iter().as_slice();
  }

  let mut result: std::io::Result<()> = std::io::Result::Ok(());

  match command {
    "init" => {
      result = commands::init(command_args);
    }
    "build" => {
      result = commands::build(command_args);
    }
    "run" => {
      result = commands::run(command_args);
    }
    "test" => {
      result = commands::test();
    }
    "clean" => {
      result = commands::clean();
    }
    "fmt" => {
      result = commands::fmt(command_args);
    }
    "lint" => {
      println!("\x1b[31mincorrect usage:\x1b[0m \n\tcommand not recognized: {command}");
    }
    "check" => {
      result = commands::check();
    }
    "new-pr" => {
      println!("\x1b[31mincorrect usage:\x1b[0m \n\tcommand not recognized: {command}");
      // result = handlers::new_pr(command_args);
    }
    "install" => {
      println!("\x1b[31mincorrect usage:\x1b[0m \n\tcommand not recognized: {command}");
      // result = handlers::install(command_args);
    }
    "help" | "--help" | "-h" => {
      commands::help();
    }
    _ => {
      println!("\x1b[31mincorrect usage:\x1b[0m \n\tcommand not recognized: {command}");
    }
  }
  if result.is_err() {
    eprintln!("{}", result.unwrap_err())
  }
}
