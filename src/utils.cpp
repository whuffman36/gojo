#include "src/utils.hpp"

#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <format>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#if defined(__APPLE__)
  #include <sys/fcntl.h>
  #include <sys/_types/_pid_t.h>
  #include <sys/_types/_ssize_t.h>
  #include <sys/wait.h>
  #include <unistd.h>
#endif

#if defined(__linux__)
  #include <sys/fcntl.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <unistd.h>
#endif


namespace {

std::vector<char*> split_args(const char* const args_str) {
  std::vector<char*> args_vec {};
  std::size_t args_len { std::strlen(args_str) };
  // Because this is only ever called in a child process, the memory
  // does not need to be freed manually since the OS will clean up
  // the memory when exiting the process.
  // Maybe not the best practice but if it works it works.
  // Allocate room for and copy over the null byte.
  char* args_str_copy { static_cast<char*>(std::malloc(args_len + 1)) };
  std::strncpy(args_str_copy, args_str, args_len + 1);
  std::size_t current_arg_start { 0 };
  bool in_quotes { false };
  for (std::size_t i { 0 }; i < args_len; ++i) {
    if (args_str_copy[i] == '"') {
      in_quotes = !in_quotes;
    }
    if (args_str_copy[i] == ' ' && !in_quotes) {
      // Remove quotes from any arguments.
      if (args_str_copy[current_arg_start] == '"' && args_str_copy[i - 1] == '"') {
        current_arg_start += 1;
        args_str_copy[i - 1] = '\0';
      }
      args_str_copy[i] = '\0';
      args_vec.push_back(&args_str_copy[current_arg_start]);
      current_arg_start = i + 1;
    }
  }

  // The last string must be added since there will be no space character
  // found after.
  args_vec.push_back(&args_str_copy[current_arg_start]);

  // The last element of the array must be a nullptr so exec() knows when to
  // stop.
  args_vec.push_back(nullptr);

  return args_vec;
}

}  // namespace

namespace utils {

CommandResult execute_command(const char* const cmd, bool capture_output) noexcept {
  // Open pipe to pass data between parent and child process.
  std::array<int, 2> pipefd {};
  if (pipe(pipefd.data()) == -1) {
    return CommandResult { 
      .output="failed to open inter-process pipe",
      .success=false
    };
  }

  std::fflush(nullptr);
  // Fork a new child process to run the command.
  const pid_t pid { fork() };
  if (pid == -1) {
    return CommandResult {
      .output="failed to fork process",
      .success=false
    };
  }

  if (pid == 0) {
    // Child process
    // Connect stdout of the child process to the write end of the pipe.
    dup2(pipefd.at(1), STDOUT_FILENO);

    // Connect stderr of the child process to the write end of the pipe.
    dup2(pipefd.at(1), STDERR_FILENO);

    // Close both ends of the pipe to avoid leaking them when over-writing
    // the child process.
    close(pipefd.at(0));
    close(pipefd.at(1));

    // Write over this process' memory and execute the command in this
    // process.
    std::vector<char*> args { split_args(cmd) };
    execvp(args.at(0), args.data());
    std::perror("execvp");

    // If this code runs, it means execvp failed and this child process
    // should be killed.
    std::exit(EXIT_FAILURE);
  }
  else {
    // Parent process
    // Close write end of the pipe since this process only reads from the
    // child.
    close(pipefd.at(1));

    constexpr int BUFFER_SIZE { 4096 };
    std::array<char, BUFFER_SIZE> buffer {};
    std::string output {};
    ssize_t bytes_read { 0 };

    if (capture_output) {
      while ((bytes_read = read(pipefd.at(0), buffer.data(), BUFFER_SIZE)) > 0) {
        output.append(buffer.data(), static_cast<std::size_t>(bytes_read));
      }
    }
    else {
      while ((bytes_read = read(pipefd.at(0), buffer.data(), BUFFER_SIZE)) > 0) {
        std::print("{}", buffer.data());
      }
    }

    // Close read end of the pipe after (possibly) reading from the child process.
    close(pipefd.at(0));

    // Wait for child process to terminate.
    int status {};
    if (waitpid(pid, &status, 0) == -1) {
      return CommandResult { .output=output, .success=false };
    }

    if (WIFEXITED(status)) {
      if (WEXITSTATUS(status) == EXIT_FAILURE) {
        // process exited in failure state
        return CommandResult { .output=output, .success=false };
      }
      else if (WEXITSTATUS(status) == EXIT_SUCCESS) {
        return CommandResult { .output=output, .success=true };
      }
      else {
        // Unknown exit code...
        return CommandResult { .output=output, .success=false };
      }
    }
    else {
      // Process was killed or stopped
      return CommandResult { .output=output, .success=false };
    }
  }

  std::unreachable();
}


std::string get_file_contents(const std::filesystem::path& file) {
  if (!std::filesystem::exists(file)) {
    throw std::runtime_error { std::format("file not found: {}", file.string()) };
  }

  std::ifstream filestream { file.string() };
  std::stringstream stringstream {};
  stringstream << filestream.rdbuf();
  return stringstream.str();

  // TODO: Benchmark
  /*
  std::ifstream filestream { file.string() };
  auto size { std::filesystem::file_size(file) };
  std::string contents {};
  contents.reserve(size);
  filestream.read(contents.data(), size);
  return contents;
  */
}


void append_file_at_line(const std::filesystem::path& file, std::string_view line_to_match, std::string_view contents) {
  std::string current_contents { get_file_contents(file) };

  std::size_t idx { current_contents.find(line_to_match) };
  if (idx == std::string::npos) {
    throw std::runtime_error { std::format("'{}' not found in '{}'", line_to_match, file.string()) };
  }

  std::string new_contents { std::format("\n{}\n", contents) };

  // Insert at newline after the line is found.
  current_contents.insert(idx + line_to_match.size(), new_contents);

  std::ofstream fstream { file };
  fstream << current_contents << std::flush;
}


std::string to_upper(std::string_view str) noexcept {
  std::string upper {};
  for (const char c : str) {
    upper.push_back(
      // Sometimes C++ is dumb as fuck...why does toupper
      // take in an int?
      static_cast<char>(std::toupper(static_cast<int>(c)))
    );
  }
  return upper;
}


void print_status(std::string_view str) noexcept {
  std::println(MAGENTA("{}"), str);
}

void print_success(std::string_view str) noexcept {
  std::println(GREEN("{}"), str);
}

void print_error(std::string_view str) noexcept {
  std::println(stderr, RED("[Error]: {}"), str);
}

void print_warning(std::string_view str) noexcept {
  std::println(stderr, YELLOW("[Warning]: {}"), str);
}

void print_info(std::string_view str) noexcept {
  std::println("{}", std::format("{}", str, utils::CLEAR,
                   utils::MAGENTA_BOLD,
                   utils::MAGENTA,
                   utils::YELLOW,
                   utils::CYAN));
}


std::string to_lower(std::string_view str) noexcept {
  std::string lower {};
  for (const char c : str) {
    lower.push_back(
      static_cast<char>(std::tolower(static_cast<int>(c)))
    );
  }
  return lower;
}


const std::filesystem::path get_root_dir() noexcept {
  const std::filesystem::path root_dir {
    std::filesystem::current_path()
  };
  return std::filesystem::absolute(root_dir);
}


const std::filesystem::path get_home_dir() noexcept {
  const static std::filesystem::path HOME { std::getenv("HOME") };
  return std::filesystem::absolute(HOME);
}


const std::filesystem::path get_profiles_dir() noexcept {
  const static std::filesystem::path profiles_dir {
    get_home_dir() / ".gojo" / "profiles"
  };
  return std::filesystem::absolute(profiles_dir);
}


const std::filesystem::path get_config_path() noexcept {
  const static std::filesystem::path config_path {
    get_root_dir() / "gojo.toml"
  };
  return std::filesystem::absolute(config_path);
}

}  // namespace utils
