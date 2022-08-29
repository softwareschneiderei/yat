#include "catch.hpp"
#include <yat/utils/String.h>

TEST_CASE("basics", "[String]")
{
  REQUIRE(yat::String("toto").is_equal("toto"));
}

TEST_CASE("split_std::string_1", "[String]")
{
  std::vector<std::string> vResultStdValues;
  std::string strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");

  yat::StringUtil::split(strTestPhraseToSplit, '_', &vResultStdValues);

  REQUIRE(5 == vResultStdValues.size());
  CHECK(vResultStdValues[0] == " This");
  CHECK(vResultStdValues[1] == "is");
  CHECK(vResultStdValues[2] == "a");
  CHECK(vResultStdValues[3] == "split");
  CHECK(vResultStdValues[4] == "test,obviously.  ");
}

TEST_CASE("split_std::string_2", "[String]")
{
  std::vector<std::string> vResultStdValues;
  vResultStdValues.push_back(std::string("Test line not to be cleared"));
  vResultStdValues.push_back(std::string("Second Test line not to be cleared"));
  std::string strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");

  yat::StringUtil::split(strTestPhraseToSplit, '_', &vResultStdValues, false);

  REQUIRE(7 == vResultStdValues.size());
  CHECK(vResultStdValues[0] == "Test line not to be cleared");
  CHECK(vResultStdValues[1] == "Second Test line not to be cleared");
  CHECK(vResultStdValues[0+2] == " This");
  CHECK(vResultStdValues[1+2] == "is");
  CHECK(vResultStdValues[2+2] == "a");
  CHECK(vResultStdValues[3+2] == "split");
  CHECK(vResultStdValues[4+2] == "test,obviously.  ");
}

TEST_CASE("split_yat::String_1", "[String]")
{
  std::vector<yat::String> vResultStdValues;
  yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");

  strTestPhraseToSplit.split('_', &vResultStdValues);

  REQUIRE(5 == vResultStdValues.size());
  CHECK(vResultStdValues[0] == " This");
  CHECK(vResultStdValues[1] == "is");
  CHECK(vResultStdValues[2] == "a");
  CHECK(vResultStdValues[3] == "split");
  CHECK(vResultStdValues[4] == "test,obviously.  ");
}

TEST_CASE("split_yat::String_2", "[String]")
{
  std::vector<yat::String> vResultStdValues;
  vResultStdValues.push_back(std::string("Test line not to be cleared"));
  vResultStdValues.push_back(std::string("Second Test line not to be cleared"));
  yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
  strTestPhraseToSplit.split('_', &vResultStdValues, false);

  REQUIRE(7 == vResultStdValues.size());
  CHECK(vResultStdValues[0] == "Test line not to be cleared");
  CHECK(vResultStdValues[1] == "Second Test line not to be cleared");
  CHECK(vResultStdValues[0+2] == " This");
  CHECK(vResultStdValues[1+2] == "is");
  CHECK(vResultStdValues[2+2] == "a");
  CHECK(vResultStdValues[3+2] == "split");
  CHECK(vResultStdValues[4+2] == "test,obviously.  ");
}

TEST_CASE("split_std::string_3", "[String]")
{
  std::string left, right;
  yat::String to_split("left,right");

  to_split.split(',', &left, &right);

  REQUIRE(left == "left");
  REQUIRE(right == "right");
}

TEST_CASE("split_yat::String_3", "[String]")
{
  yat::String left, right;
  yat::String to_split("left,right");

  to_split.split(',', &left, &right);

  REQUIRE(left.is_equal("left"));
  REQUIRE(right.is_equal("right"));
}

TEST_CASE("split_yat::String_4", "[String]")
{
  yat::String left, right;
  yat::String to_split(",right");

  to_split.split(',', &left, &right);

  REQUIRE(left.is_equal(""));
  REQUIRE(right.is_equal("right"));
}

TEST_CASE("split_yat::String_5", "[String]")
{
  yat::String left, right;
  yat::String to_split("left,");

  to_split.split(',', &left, &right);

  CHECK(left.is_equal("left"));
  CHECK(right.is_equal(""));
}

TEST_CASE("split_yat::String_6", "[String]")
{
  yat::String left, right;
  yat::String to_split("text");

  to_split.split(',', &left, &right);

  CHECK(left.is_equal(""));
  CHECK(right.is_equal(""));
}

TEST_CASE("split_yat::String_7", "[String]")
{
  yat::String left, right;
  yat::String to_split("left\\,right");

  to_split.split(',', &left, &right);

  CHECK(left.is_equal(""));
  CHECK(right.is_equal(""));
}

TEST_CASE("split_yat::String_8", "[String]")
{
  yat::String left, right;
  yat::String to_split("a\\,b,c");

  to_split.split(',', &left, &right);
  INFO("left: " << left);
  INFO("right: " << right);
  CHECK(left.is_equal("a\\,b"));
  CHECK(right.is_equal("c"));
}

TEST_CASE("split_yat::String_9", "[String]")
{
  yat::String left, right;
  yat::String to_split("a,b\\,c");

  to_split.split(',', &left, &right);
  INFO("left: " << left);
  INFO("right: " << right);
  CHECK(left.is_equal("a"));
  CHECK(right.is_equal("b,c"));
}

TEST_CASE("extract_left_1", "[String]")
{
  yat::String to;
  yat::String from("1,2,3");

  from.extract_token(',', &to);
  CHECK(to.is_equal("1"));
  CHECK(from.is_equal("2,3"));

  from.extract_token(',', &to);
  CHECK(to.is_equal("2"));
  CHECK(from.is_equal("3"));

  from.extract_token(',', &to);
  CHECK(to.is_equal("3"));
  CHECK(from.is_equal(""));
}

TEST_CASE("extract_right_1", "[String]")
{
  yat::String to;
  yat::String from("1,2,3");

  from.extract_token_right(',', &to);
  CHECK(from.is_equal("1,2"));
  CHECK(to.is_equal("3"));

  from.extract_token_right(',', &to);
  CHECK(from.is_equal("1"));
  CHECK(to.is_equal("2"));

  from.extract_token_right(',', &to);
  CHECK(from.is_equal(""));
  CHECK(to.is_equal("1"));
}

TEST_CASE("extract_left_esc", "[String]")
{
  yat::String left;
  yat::String from("1\\,2,3");

  from.extract_token(',', &left, '\\');
  CHECK(left.is_equal("1,2"));
  CHECK(from.is_equal("3"));
}

TEST_CASE("extract_right_esc", "[String]")
{
  yat::String right;
  yat::String from("1,2\\,3");

  from.extract_token_right(',', &right, '\\');
  CHECK(right.is_equal("2,3"));
  CHECK(from.is_equal("1"));
}

TEST_CASE("format_str", "[Format]")
{
  yat::String res = yat::Format("a string: {}").arg("hello!");
  CHECK(res.is_equal("a string: hello!"));
}

TEST_CASE("format_float", "[Format]")
{
  yat::String res = yat::Format("a number: {4.1e}").arg(12.0);
  INFO("result is " << res);
  #ifdef YAT_WIN32
  CHECK(res.is_equal("a number: 1.2e+001"));
  #else
  CHECK(res.is_equal("a number: 1.2e+01"));
  #endif
}

TEST_CASE("format_bool", "[Format]")
{
  yat::String res = yat::Format("a boolean: {b}").arg(true);
  INFO("result is " << res);
  CHECK(res.is_equal("a boolean: true"));
}

TEST_CASE("format_binary", "[Format]")
{
  yat::String res = yat::Format("a binary value: {B}").arg((yat::uint8)42);
  INFO("result is " << res);
  CHECK(res.is_equal("a binary value: 101010"));
}

TEST_CASE("format_hex", "[Format]")
{
  yat::String res = yat::Format("a hex: {x}").arg(255);
  INFO("result is " << res);
  CHECK(res.is_equal("a hex: ff"));
}

TEST_CASE("format_octal", "[Format]")
{
  yat::String res = yat::Format("an octal number: {o}").arg(255);
  INFO("result is " << res);
  CHECK(res.is_equal("an octal number: 377"));
}

TEST_CASE("format_all", "[Format]")
{
  yat::String res = yat::Format("a string: {}; a float number: {4.1e}; an octal number: {o}; a boolean: {b}; a hex: {x}")
                    .arg("hello!").arg(12.0).arg(255).arg(true).arg(255);
  INFO("result is " << res);
  #ifdef YAT_WIN32
  CHECK(res.is_equal("a string: hello!; a float number: 1.2e+001; an octal number: 377; a boolean: true; a hex: ff"));
  #else
  CHECK(res.is_equal("a string: hello!; a float number: 1.2e+01; an octal number: 377; a boolean: true; a hex: ff"));
  #endif
}

TEST_CASE("format_field_size", "[Format]")
{
  yat::String res = yat::Format("fixed field size arg: [{20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("fixed field size arg: [hello!              ]"));
}

TEST_CASE("format_right_field_size_fill", "[Format]")
{
  yat::String res = yat::Format("fixed field size arg: [{_>20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("fixed field size arg: [______________hello!]"));
}

TEST_CASE("format_left_field_size_fill", "[Format]")
{
  yat::String res = yat::Format("fixed field size arg: [{_<20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("fixed field size arg: [hello!______________]"));
}

TEST_CASE("format_left_field_size", "[Format]")
{
  yat::String res = yat::Format("left arg: [{<20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("left arg: [hello!              ]"));
}

TEST_CASE("format_right_field_size", "[Format]")
{
  yat::String res = yat::Format("right arg: [{>20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("right arg: [              hello!]"));
}

#define CHECK_FORMAT(v, fmt, res) \
  WHEN("Format with " fmt) \
  { \
    yat::Format format(fmt); \
    CHECK(format.arg(v).get() == res); \
  }

SCENARIO( "Some Format operations tests", "[Format]" )
{
  WHEN("Testing the arg method")
  {
    GIVEN("A string")
    {
      std::string s = "gabu";

      CHECK_FORMAT(s, "{<5}", "gabu ")
      CHECK_FORMAT(s, "{>5}", " gabu")
    }
    GIVEN("A char value")
    {
      char c = 'W';

      CHECK_FORMAT(c, "{}", "W")
      CHECK_FORMAT(c, "{d}", "87")
    }
    GIVEN("A yat::int8 positive value")
    {
      yat::int8 i8 = 12;

      CHECK_FORMAT(i8, "{d}", "12")
      CHECK_FORMAT(i8, "{+d}", "+12")

      CHECK_FORMAT(i8, "{B}", "1100")
      CHECK_FORMAT(i8, "{o}", "14")
      CHECK_FORMAT(i8, "{x}", "c")
      CHECK_FORMAT(i8, "{X}", "C")

      CHECK_FORMAT(i8, "{#B}", "0b1100")
      CHECK_FORMAT(i8, "{#o}", "0o14")
      CHECK_FORMAT(i8, "{#x}", "0xc")
      CHECK_FORMAT(i8, "{#X}", "0XC")

      CHECK_FORMAT(i8, "{0B}", "00001100")
      CHECK_FORMAT(i8, "{0o}", "014")
      CHECK_FORMAT(i8, "{0x}", "0c")
      CHECK_FORMAT(i8, "{0X}", "0C")

      CHECK_FORMAT(i8, "{#0B}", "0b00001100")
      CHECK_FORMAT(i8, "{#0o}", "0o014")
      CHECK_FORMAT(i8, "{#0x}", "0x0c")
      CHECK_FORMAT(i8, "{#0X}", "0X0C")
    }

    GIVEN("A yat::int8 negative value")
    {
      yat::int8 i8 = -42;

      CHECK_FORMAT(i8, "{d}", "-42")

      CHECK_FORMAT(i8, "{B}", "11010110")
      CHECK_FORMAT(i8, "{o}", "326")
      CHECK_FORMAT(i8, "{x}", "d6")
      CHECK_FORMAT(i8, "{X}", "D6")

      CHECK_FORMAT(i8, "{#B}", "0b11010110")
      CHECK_FORMAT(i8, "{#o}", "0o326")
      CHECK_FORMAT(i8, "{#x}", "0xd6")
      CHECK_FORMAT(i8, "{#X}", "0XD6")

      CHECK_FORMAT(i8, "{010B}", "0011010110")
      CHECK_FORMAT(i8, "{05o}", "00326")
      CHECK_FORMAT(i8, "{03x}", "0d6")
      CHECK_FORMAT(i8, "{03X}", "0D6")

      CHECK_FORMAT(i8, "{#012B}", "0b0011010110")
      CHECK_FORMAT(i8, "{#07o}", "0o00326")
      CHECK_FORMAT(i8, "{#05x}", "0x0d6")
      CHECK_FORMAT(i8, "{#05X}", "0X0D6")
    }

    GIVEN("A yat::uint8 value")
    {
      yat::uint8 ui8 = 42;

      CHECK_FORMAT(ui8, "{d}", "42")

      CHECK_FORMAT(ui8, "{B}", "101010")
      CHECK_FORMAT(ui8, "{o}", "52")
      CHECK_FORMAT(ui8, "{x}", "2a")
      CHECK_FORMAT(ui8, "{X}", "2A")

      CHECK_FORMAT(ui8, "{#B}", "0b101010")
      CHECK_FORMAT(ui8, "{#o}", "0o52")
      CHECK_FORMAT(ui8, "{#x}", "0x2a")
      CHECK_FORMAT(ui8, "{#X}", "0X2A")

      CHECK_FORMAT(ui8, "{#0B}", "0b00101010")
      CHECK_FORMAT(ui8, "{#0o}", "0o052")
      CHECK_FORMAT(ui8, "{#0x}", "0x2a")
      CHECK_FORMAT(ui8, "{#0X}", "0X2A")
    }

    GIVEN("A yat::uint16 value")
    {
      yat::uint16 ui16 = 65280;

      CHECK_FORMAT(ui16, "{}", "65280")
      CHECK_FORMAT(ui16, "{d}", "65280")

      CHECK_FORMAT(ui16, "{B}", "1111111100000000")
      CHECK_FORMAT(ui16, "{o}", "177400")
      CHECK_FORMAT(ui16, "{x}", "ff00")
      CHECK_FORMAT(ui16, "{X}", "FF00")

      CHECK_FORMAT(ui16, "{#B}", "0b1111111100000000")
      CHECK_FORMAT(ui16, "{#o}", "0o177400")
      CHECK_FORMAT(ui16, "{#x}", "0xff00")
      CHECK_FORMAT(ui16, "{#X}", "0XFF00")
    }

    GIVEN("A yat::int16 positive value")
    {
      yat::int16 i16 = 3000;

      CHECK_FORMAT(i16, "{}", "3000")
      CHECK_FORMAT(i16, "{+}", "+3000")
      CHECK_FORMAT(i16, "{d}", "3000")
      CHECK_FORMAT(i16, "{+d}", "+3000")

      CHECK_FORMAT(i16, "{B}", "111010100110000")
      CHECK_FORMAT(i16, "{o}", "72460")
      CHECK_FORMAT(i16, "{x}", "7530")
      CHECK_FORMAT(i16, "{X}", "7530")

      CHECK_FORMAT(i16, "{#B}", "0b111010100110000")
      CHECK_FORMAT(i16, "{#o}", "0o72460")
      CHECK_FORMAT(i16, "{#x}", "0x7530")
      CHECK_FORMAT(i16, "{#X}", "0X7530")

      CHECK_FORMAT(i16, "{#0B}", "0b0111010100110000")
      CHECK_FORMAT(i16, "{#0o}", "0o072460")
      CHECK_FORMAT(i16, "{#0x}", "0x7530")
      CHECK_FORMAT(i16, "{#0X}", "0X7530")
    }
  }
}
