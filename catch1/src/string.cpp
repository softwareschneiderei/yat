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

TEST_CASE("format_str", "[String]")
{
  yat::String res = yat::Format("a string: {}").arg("hello!");
  CHECK(res.is_equal("a string: hello!"));
}

TEST_CASE("format_float", "[String]")
{
  yat::String res = yat::Format("a number: {4.1e}").arg(12.0);
  INFO("result is " << res);
  #ifdef YAT_WIN32
  CHECK(res.is_equal("a number: 1.2e+001"));
  #else
  CHECK(res.is_equal("a number: 1.2e+01"));
  #endif
}

TEST_CASE("format_bool", "[String]")
{
  yat::String res = yat::Format("a boolean: {b}").arg(true);
  INFO("result is " << res);
  CHECK(res.is_equal("a boolean: true"));
}

TEST_CASE("format_binary", "[String]")
{
  yat::String res = yat::Format("a binary value: {B}").arg((yat::uint8)42);
  INFO("result is " << res);
  CHECK(res.is_equal("a binary value: 00101010"));
}

TEST_CASE("format_hex", "[String]")
{
  yat::String res = yat::Format("a hex: {x}").arg(255);
  INFO("result is " << res);
  CHECK(res.is_equal("a hex: ff"));
}

TEST_CASE("format_octal", "[String]")
{
  yat::String res = yat::Format("an octal number: {o}").arg(255);
  INFO("result is " << res);
  CHECK(res.is_equal("an octal number: 377"));
}

TEST_CASE("format_all", "[String]")
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

TEST_CASE("format_field_size", "[String]")
{
  yat::String res = yat::Format("fixed field size arg: [{20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("fixed field size arg: [              hello!]"));
}

TEST_CASE("format_right_field_size_fill", "[String]")
{
  yat::String res = yat::Format("fixed field size arg: [{_>20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("fixed field size arg: [______________hello!]"));
}

TEST_CASE("format_left_field_size_fill", "[String]")
{
  yat::String res = yat::Format("fixed field size arg: [{_<20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("fixed field size arg: [hello!______________]"));
}

TEST_CASE("format_left_field_size", "[String]")
{
  yat::String res = yat::Format("left arg: [{<20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("left arg: [hello!              ]"));
}

TEST_CASE("format_right_field_size", "[String]")
{
  yat::String res = yat::Format("right arg: [{>20s}]").arg("hello!");
  INFO("result is " << res);
  CHECK(res.is_equal("right arg: [              hello!]"));
}

