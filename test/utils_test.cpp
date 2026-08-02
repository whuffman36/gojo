#include <fstream>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "src/utils.hpp"


namespace {

// This code is run from <PROJECT_ROOT>/build/Debug/test
// The assets path is <PROJECT_ROOT>/test/assets
const std::filesystem::path ASSETS_PATH {
  "../../../test/assets"
};

const std::filesystem::path TEST_FILE {
  ASSETS_PATH / "test_utils_example.txt"
};

}  // namespace


TEST(UtilsTest, ExecuteCommand) {
  const std::string cmd { "ls" };

  const auto result1 { utils::execute_command(cmd) };
  EXPECT_TRUE(result1.output.empty());
  EXPECT_TRUE(result1.success);

  const auto result2 { utils::execute_command(cmd, true) };
  EXPECT_FALSE(result2.output.empty());
  EXPECT_TRUE(result2.success);

  const std::string bad_cmd { "bad-cmd" };

  const auto result3 { utils::execute_command(bad_cmd) };
  EXPECT_TRUE(result3.output.empty());
  EXPECT_FALSE(result3.success);

  const auto result4 { utils::execute_command(bad_cmd, true) };
  EXPECT_FALSE(result4.output.empty());
  EXPECT_FALSE(result4.success);

  const auto result5 { utils::execute_command(bad_cmd, true) };
  EXPECT_FALSE(result5.output.empty());
  EXPECT_FALSE(result5.success);
}


TEST(UtilsTest, ReadFileIntoMemory) {
  const auto result1 { utils::read_file_into_memory(TEST_FILE) };
  EXPECT_TRUE(result1.has_value());
  EXPECT_TRUE(result1.value().starts_with("This is example text for a file to be read."));
  EXPECT_TRUE(result1->ends_with("Last Line"));
  EXPECT_TRUE(result1->contains("Line 3 example"));
  EXPECT_TRUE(result1->contains("Line 5 example"));

  const auto result2 { utils::read_file_into_memory("example.txt") };
  EXPECT_FALSE(result2.has_value());
}


TEST(UtilsTest, AppendFileAtLine) {
  const auto result1 { utils::read_file_into_memory(TEST_FILE) };
  ASSERT_TRUE(result1.has_value());
  const std::string original_file_contents { std::move(result1.value()) };

  constexpr std::string_view line_to_match { "Line 3 example" };
  constexpr std::string_view contents { "Line 4 example" };

  const auto result2 { utils::append_file_at_line(TEST_FILE, line_to_match, contents) };
  EXPECT_FALSE(result2.has_value());

  const auto result3 { utils::read_file_into_memory(TEST_FILE) };
  ASSERT_TRUE(result3.has_value());
  EXPECT_TRUE(result3.value().starts_with("This is example text for a file to be read."));
  EXPECT_TRUE(result3->ends_with("Last Line"));
  EXPECT_TRUE(result3->contains("Line 3 example"));
  EXPECT_TRUE(result3->contains("Line 4 example"));
  EXPECT_TRUE(result3->contains("Line 5 example"));

  constexpr std::string_view line_to_match2 { "Line 4 example" };
  constexpr std::string_view contents2 { "\nLine 4.5 example" };

  const auto result4 { utils::append_file_at_line(TEST_FILE, line_to_match2, contents2) };
  EXPECT_FALSE(result4.has_value());

  const auto result5 { utils::read_file_into_memory(TEST_FILE) };
  ASSERT_TRUE(result5.has_value());
  EXPECT_TRUE(result5.value().starts_with("This is example text for a file to be read."));
  EXPECT_TRUE(result5->ends_with("Last Line"));
  EXPECT_TRUE(result5->contains("Line 3 example"));
  EXPECT_TRUE(result5->contains("Line 4 example"));
  EXPECT_TRUE(result5->contains("Line 4.5 example"));
  EXPECT_TRUE(result5->contains("Line 5 example"));

  const auto result6 { utils::append_file_at_line(TEST_FILE, "does not exist", contents) };
  EXPECT_TRUE(result6.has_value());

  // Rewrite test file to have original contents.
  std::ofstream test_file { TEST_FILE, std::ios::trunc };
  ASSERT_TRUE(test_file.is_open());
  test_file << original_file_contents << std::flush;
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