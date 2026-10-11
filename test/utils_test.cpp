#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "src/utils.hpp"
#include "test/utils.hpp"


namespace {

const std::filesystem::path PATH {
  get_assets_path() / "utils"
};

const std::filesystem::path TEST_FILE {
  PATH / "example.txt"
};

}  // namespace


TEST(UtilsTest, ExecuteCommand) {
  const char* cmd { "ls" };

  const auto result1 { utils::execute_command(cmd) };
  EXPECT_TRUE(result1.output.empty());
  EXPECT_TRUE(result1.success);

  const auto result2 { utils::execute_command(cmd, true) };
  EXPECT_FALSE(result2.output.empty());
  EXPECT_TRUE(result2.success);

  const char* bad_cmd { "bad-cmd" };

  const auto result3 { utils::execute_command(bad_cmd) };
  EXPECT_TRUE(result3.output.empty());
  EXPECT_FALSE(result3.success);

  const auto result4 { utils::execute_command(bad_cmd, true) };
  EXPECT_FALSE(result4.output.empty());
  EXPECT_FALSE(result4.success);

  const auto result5 { utils::execute_command(bad_cmd, true) };
  EXPECT_FALSE(result5.output.empty());
  EXPECT_FALSE(result5.success);

  const std::filesystem::path new_file { PATH / "execute_command.txt" };
  const std::string touch_cmd { std::format("touch {}", new_file.string()) };
  const auto result6 { utils::execute_command(touch_cmd.c_str()) };
  EXPECT_TRUE(result6.success);
  EXPECT_TRUE(std::filesystem::exists(new_file));
  EXPECT_TRUE(std::filesystem::remove(new_file));
}


TEST(UtilsTest, GetFileContents) {
  std::string contents {};
  EXPECT_NO_THROW(contents = utils::get_file_contents(TEST_FILE));

  EXPECT_TRUE(contents.starts_with("This is example text for a file to be read."));
  EXPECT_TRUE(contents.ends_with("Last Line"));
  EXPECT_EQ(std::filesystem::file_size(TEST_FILE), contents.size());
}


TEST(UtilsTest, AppendFileAtLine) {
  std::string original_file_contents {};
  EXPECT_NO_THROW(original_file_contents = utils::get_file_contents(TEST_FILE));

  constexpr std::string_view line_to_match { "Line 3 example" };
  constexpr std::string_view contents { "Line 4 example" };
  utils::append_file_at_line(TEST_FILE, line_to_match, contents);

  std::string new_contents {};
  EXPECT_NO_THROW(new_contents = utils::get_file_contents(TEST_FILE));
  EXPECT_TRUE(new_contents.starts_with("This is example text for a file to be read.\n"));
  EXPECT_TRUE(new_contents.ends_with("Last Line"));
  EXPECT_TRUE(new_contents.contains("Line 3 example\n"));
  EXPECT_TRUE(new_contents.contains("Line 4 example\n"));
  EXPECT_TRUE(new_contents.contains("Line 5 example\n"));

  constexpr std::string_view line_to_match2 { "Line 4 example" };
  constexpr std::string_view contents2 { "Line 4.5 example" };
  utils::append_file_at_line(TEST_FILE, line_to_match2, contents2);

  std::string new_contents2 {};
  EXPECT_NO_THROW(new_contents2 = utils::get_file_contents(TEST_FILE));
  EXPECT_TRUE(new_contents2.starts_with("This is example text for a file to be read."));
  EXPECT_TRUE(new_contents2.ends_with("Last Line"));
  EXPECT_TRUE(new_contents2.contains("Line 3 example\n"));
  EXPECT_TRUE(new_contents2.contains("Line 4 example\n"));
  EXPECT_TRUE(new_contents2.contains("Line 4.5 example\n"));
  EXPECT_TRUE(new_contents2.contains("Line 5 example\n"));

  EXPECT_THROW(utils::append_file_at_line(TEST_FILE, "does not exist", contents), std::runtime_error);

  // Rewrite test file to have original contents.
  if (!original_file_contents.empty()) {
    std::ofstream test_file { TEST_FILE, std::ios::trunc };
    ASSERT_TRUE(test_file.is_open());
    test_file << original_file_contents << std::flush;
  }
}


TEST(UtilsTest, ToUpper) {
  constexpr std::string_view str1 {
    "all lowercase"
  };

  EXPECT_EQ(utils::to_upper(str1), "ALL LOWERCASE");

  constexpr std::string_view str2 {
    "AlTeRnAtInG"
  };

  EXPECT_EQ(utils::to_upper(str2), "ALTERNATING");

  // Not really sure this is necessary, project names should only consist
  // of ASCII characters without numbers or alternate characters.
  constexpr std::string_view str3 {
    "other chars..._-69"
  };

  EXPECT_EQ(utils::to_upper(str3), "OTHER CHARS..._-69");
}


TEST(UtilsTest, ToLower) {
  constexpr std::string_view str1 {
    "ALL UPPERCASE"
  };

  EXPECT_EQ(utils::to_lower(str1), "all uppercase");

  constexpr std::string_view str2 {
    "AlTeRnAtInG"
  };

  EXPECT_EQ(utils::to_lower(str2), "alternating");

  constexpr std::string_view str3 {
    "other chars..._-69"
  };

  // No change.
  EXPECT_EQ(utils::to_lower(str3), str3);
}