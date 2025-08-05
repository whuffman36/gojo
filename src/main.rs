mod file_contents;
mod handlers;

use std::env;

fn main() {
  let args: Vec<String> = env::args().collect();
  if args.len() == 1 {
    println!("{}", file_contents::WIN);
    handlers::help();
    return;
  }
  let command = args[1].as_str();
  let mut result: std::io::Result<()> = std::io::Result::Ok(());

  match command {
    "init" => {
      result = handlers::init(args.as_slice());
    }
    "build" => {
      result = handlers::build(args.as_slice());
    }
    "run" => {
      result = handlers::run(args.as_slice());
    }
    "test" => {
      result = handlers::test();
    }
    "clean" => {
      result = handlers::clean();
    }
    "fmt" => {
      result = handlers::fmt(args.as_slice());
    }
    "check" => {
      result = handlers::check();
    }
    "help" => {
      handlers::help();
    }
    _ => {
      println!("\x1b[31mincorrect usage:\x1b[0m \n\tcommand not recognized: {command}");
    }
  }
  if result.is_err() {
    eprintln!("{}", result.unwrap_err())
  }
}
