#include "src/utils.hpp"

#include <array>
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#if defined(__APPLE__)
#include <sys/_types/_pid_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#if defined(__linux__)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {

std::vector<char*> split_args(const char* const args_str) {
    std::vector<char*> args_vec {};
    const std::size_t args_len { std::strlen(args_str) };
    // Because this is only ever called in a child process, the memory
    // does not need to be freed manually since the OS will clean up
    // the memory when exiting the process.
    // Maybe not the best practice but if it works it works.
    // Allocate room for and copy over the null byte.
    // NOLINTNEXTLINE(cppcoreguidelines-no-malloc)
    char* args_str_copy { static_cast<char*>(std::malloc(args_len + 1)) };
    std::strncpy(args_str_copy, args_str, args_len + 1);
    std::size_t current_arg_start { 0 };
    bool in_quotes { false };

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (std::size_t i { 0 }; i < args_len; ++i) {
        if (args_str_copy[i] == '"') {
            in_quotes = !in_quotes;
        }
        if (args_str_copy[i] == ' ' && !in_quotes) {
            // Remove quotes from any arguments.
            if (args_str_copy[current_arg_start] == '"' &&
                args_str_copy[i - 1] == '"') {
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

    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // The last element of the array must be a nullptr so exec() knows when to
    // stop.
    args_vec.push_back(nullptr);

    return args_vec;
}

}  // namespace

namespace utils {

CommandResult execute_command(
    const char* const cmd, bool capture_output, bool verbose,
    const std::optional<std::filesystem::path>& logfile
) {
    if (cmd == nullptr) {
        return CommandResult { .output {
                                   "nullptr passed to 'execute_command()'" },
                               .success = false };
    }

    // Open pipe to pass data between parent and child process.
    std::array<int, 2> pipefd {};
    if (pipe(pipefd.data()) == -1) {
        return CommandResult { .output { "failed to open inter-process pipe" },
                               .success = false };
    }

    std::fflush(nullptr);
    // Fork a new child process to run the command.
    const pid_t pid { fork() };
    if (pid == -1) {
        return CommandResult { .output { "failed to fork process" },
                               .success = false };
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
    } else {
        // Parent process
        // Close write end of the pipe since this process only reads from the
        // child.
        close(pipefd.at(1));

        gojo_log(logfile.value_or("/dev/null"), cmd, std::strlen(cmd));

        constexpr std::size_t BUFFER_SIZE { 4096 };
        std::array<char, BUFFER_SIZE> buffer {};
        std::string output {};
        ssize_t bytes_read { 0 };

        if (verbose) {
            while ((bytes_read =
                        read(pipefd.at(0), buffer.data(), BUFFER_SIZE)) > 0) {
                gojo_print(LogLevel::Info, "{}", buffer.data());
            }
        } else {
            if (!logfile.has_value() && !capture_output) {
                gojo_print(LogLevel::Warning, "no logfile to write to");
            }
            // Kind of awkward, but I can't think of another way to avoid
            // appending to output without having an if statement in the while
            // loop.
            if (capture_output) {
                while ((bytes_read = read(pipefd.at(0), buffer.data(),
                                          BUFFER_SIZE)) > 0) {
                    gojo_log(logfile.value_or("/dev/null"), buffer.data(),
                             static_cast<std::size_t>(bytes_read));
                    output.append(buffer.data(),
                                  static_cast<std::size_t>(bytes_read));
                }
            } else {
                while ((bytes_read = read(pipefd.at(0), buffer.data(),
                                          BUFFER_SIZE)) > 0) {
                    gojo_log(logfile.value_or("/dev/null"), buffer.data(),
                             static_cast<std::size_t>(bytes_read));
                }
            }
        }

        // Close read end of the pipe after (possibly) reading from the child
        // process.
        close(pipefd.at(0));

        // Wait for child process to terminate.
        int status {};
        if (waitpid(pid, &status, 0) == -1) {
            return CommandResult { .output = output, .success = false };
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_SUCCESS) {
                // process exited in failure state
                return CommandResult { .output = output, .success = true };
            } else {
                // Unknown exit code...
                return CommandResult { .output = output, .success = false };
            }
        } else {
            // Process was killed or stopped
            return CommandResult { .output = output, .success = false };
        }
    }

    std::unreachable();
}

std::string get_file_contents(const std::filesystem::path& file) {
    if (!std::filesystem::exists(file)) {
        throw std::runtime_error { std::format("file not found: {}",
                                               file.string()) };
    }

    const std::ifstream filestream { file.string() };
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

void append_file_at_line(const std::filesystem::path& file,
                         std::string_view line_to_match,
                         std::string_view contents) {
    std::string current_contents { get_file_contents(file) };

    const std::size_t idx { current_contents.find(line_to_match) };
    if (idx == std::string::npos) {
        throw std::runtime_error { std::format("'{}' not found in '{}'",
                                               line_to_match, file.string()) };
    }

    const std::string new_contents { std::format("{}\n", contents) };

    // Insert at newline after the line is found.
    current_contents.insert(idx + line_to_match.size(), new_contents);

    std::ofstream fstream { file };
    fstream << current_contents << std::flush;
}

std::string to_upper(std::string_view str) {
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

std::string to_lower(std::string_view str) {
    std::string lower {};
    for (const char c : str) {
        lower.push_back(static_cast<char>(std::tolower(static_cast<int>(c))));
    }
    return lower;
}

void gojo_log(const std::filesystem::path& destination, const char* content,
              std::size_t content_size) {
    const std::filesystem::path parent { destination.parent_path() };
    if (!parent.empty()) {
        std::error_code error;
        std::filesystem::create_directories(parent, error);
        if (error) {
            gojo_print(LogLevel::Warning, "failed to create log directory: {}",
                       parent.string());
            return;
        }
    }
    std::ofstream destination_file { destination,
                                     std::ios::app | std::ios::binary };
    if (!destination_file.is_open()) {
        gojo_print(LogLevel::Warning, "failed to open log file: {}",
                   destination.string());
        return;
    }

    destination_file.write(content, static_cast<std::streamsize>(content_size));
    if (!destination_file) {
        gojo_print(LogLevel::Warning, "failed to write log file: {}",
                   destination.string());
    }
}

const std::filesystem::path get_root_dir() {
    static const std::filesystem::path root_dir {
        std::filesystem::absolute(std::filesystem::current_path())
    };
    return root_dir;
}

const std::filesystem::path get_home_dir() {
    static const std::filesystem::path HOME {
        std::filesystem::absolute(std::getenv("HOME"))
    };
    return HOME;
}

const std::filesystem::path get_profiles_dir() {
    static const std::filesystem::path profiles_dir {
        std::filesystem::absolute(get_home_dir() / ".gojo" / "profiles")
    };
    return profiles_dir;
}

const std::filesystem::path get_config_path() {
    static const std::filesystem::path config_path {
        std::filesystem::absolute(get_root_dir() / "gojo.toml")
    };
    return config_path;
}

}  // namespace utils
