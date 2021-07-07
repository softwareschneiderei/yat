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
  REQUIRE(vResultStdValues[0] == " This");
  REQUIRE(vResultStdValues[1] == "is");
  REQUIRE(vResultStdValues[2] == "a");
  REQUIRE(vResultStdValues[3] == "split");
  REQUIRE(vResultStdValues[4] == "test,obviously.  ");
}

TEST_CASE("split_std::string_2", "[String]")
{
  std::vector<std::string> vResultStdValues;
  vResultStdValues.push_back(std::string("Test line not to be cleared"));
  vResultStdValues.push_back(std::string("Second Test line not to be cleared"));
  std::string strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");

  yat::StringUtil::split(strTestPhraseToSplit, '_', &vResultStdValues, false);

  REQUIRE(7 == vResultStdValues.size());
  REQUIRE(vResultStdValues[0] == "Test line not to be cleared");
  REQUIRE(vResultStdValues[1] == "Second Test line not to be cleared");
  REQUIRE(vResultStdValues[0+2] == " This");
  REQUIRE(vResultStdValues[1+2] == "is");
  REQUIRE(vResultStdValues[2+2] == "a");
  REQUIRE(vResultStdValues[3+2] == "split");
  REQUIRE(vResultStdValues[4+2] == "test,obviously.  ");
}

TEST_CASE("split_yat::String_1", "[String]")
{
  std::vector<yat::String> vResultStdValues;
  yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");

  strTestPhraseToSplit.split('_', &vResultStdValues);

  REQUIRE(5 == vResultStdValues.size());
  REQUIRE(vResultStdValues[0] == " This");
  REQUIRE(vResultStdValues[1] == "is");
  REQUIRE(vResultStdValues[2] == "a");
  REQUIRE(vResultStdValues[3] == "split");
  REQUIRE(vResultStdValues[4] == "test,obviously.  ");
}

TEST_CASE("split_yat::String_2", "[String]")
{
  std::vector<yat::String> vResultStdValues;
  vResultStdValues.push_back(std::string("Test line not to be cleared"));
  vResultStdValues.push_back(std::string("Second Test line not to be cleared"));
  yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
  strTestPhraseToSplit.split('_', &vResultStdValues, false);

  REQUIRE(7 == vResultStdValues.size());
  REQUIRE(vResultStdValues[0] == "Test line not to be cleared");
  REQUIRE(vResultStdValues[1] == "Second Test line not to be cleared");
  REQUIRE(vResultStdValues[0+2] == " This");
  REQUIRE(vResultStdValues[1+2] == "is");
  REQUIRE(vResultStdValues[2+2] == "a");
  REQUIRE(vResultStdValues[3+2] == "split");
  REQUIRE(vResultStdValues[4+2] == "test,obviously.  ");
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

  REQUIRE(left.is_equal("left"));
  REQUIRE(right.is_equal(""));
}

TEST_CASE("split_yat::String_6", "[String]")
{
  yat::String left, right;
  yat::String to_split("text");

  to_split.split(',', &left, &right);

  REQUIRE(left.is_equal(""));
  REQUIRE(right.is_equal(""));
}

TEST_CASE("split_yat::String_7", "[String]")
{
  yat::String left, right;
  yat::String to_split("left\\,right");

  to_split.split(',', &left, &right);

  REQUIRE(left.is_equal(""));
  REQUIRE(right.is_equal(""));
}

TEST_CASE("split_yat::String_8", "[String]")
{
  yat::String left, right;
  yat::String to_split("a\\,b,c");

  to_split.split(',', &left, &right);
  INFO("left: " << left);
  INFO("right: " << right);
  REQUIRE(left.is_equal("a,b"));
  REQUIRE(right.is_equal("c"));
}

TEST_CASE("split_yat::String_9", "[String]")
{
  yat::String left, right;
  yat::String to_split("a,b\\,c");

  to_split.split(',', &left, &right);
  INFO("left: " << left);
  INFO("right: " << right);
  REQUIRE(left.is_equal("a"));
  REQUIRE(right.is_equal("b,c"));
}

TEST_CASE("extract_left_1", "[String]")
{
  yat::String to;
  yat::String from("1,2,3");

  from.extract_token(',', &to);
  REQUIRE(to.is_equal("1"));
  REQUIRE(from.is_equal("2,3"));

  from.extract_token(',', &to);
  REQUIRE(to.is_equal("2"));
  REQUIRE(from.is_equal("3"));

  from.extract_token(',', &to);
  REQUIRE(to.is_equal("3"));
  REQUIRE(from.is_equal(""));
}

TEST_CASE("extract_right_1", "[String]")
{
  yat::String to;
  yat::String from("1,2,3");

  from.extract_token_right(',', &to);
  REQUIRE(from.is_equal("1,2"));
  REQUIRE(to.is_equal("3"));

  from.extract_token_right(',', &to);
  REQUIRE(from.is_equal("1"));
  REQUIRE(to.is_equal("2"));

  from.extract_token_right(',', &to);
  REQUIRE(from.is_equal(""));
  REQUIRE(to.is_equal("1"));
}

TEST_CASE("format_str", "[String]")
{
  yat::String res = yat::StringFormat("a string: {}").format("hello!");
  REQUIRE(res.is_equal("a string: hello!"));
}

TEST_CASE("format_float", "[String]")
{
  yat::String res = yat::StringFormat("a number: {4.1e}").format(12.0);
  INFO("result is " << res);
  REQUIRE(res.is_equal("a number: 1.2e+01"));
}

TEST_CASE("format_bool", "[String]")
{
  yat::String res = yat::StringFormat("a boolean: {b}").format(true);
  INFO("result is " << res);
  REQUIRE(res.is_equal("a boolean: true"));
}

TEST_CASE("format_binary", "[String]")
{
  yat::String res = yat::StringFormat("a binary value: {B}").format((yat::uint8)42);
  INFO("result is " << res);
  REQUIRE(res.is_equal("a binary value: 00101010"));
}

TEST_CASE("format_hex", "[String]")
{
  yat::String res = yat::StringFormat("a hex: {x}").format(255);
  INFO("result is " << res);
  REQUIRE(res.is_equal("a hex: ff"));
}

TEST_CASE("format_octal", "[String]")
{
  yat::String res = yat::StringFormat("an octal number: {o}").format(255);
  INFO("result is " << res);
  REQUIRE(res.is_equal("an octal number: 377"));
}

TEST_CASE("format_all", "[String]")
{
  yat::String res = yat::StringFormat("a string: {}; a float number: {4.1e}; an octal number: {o}; a boolean: {b}; a hex: {x}")
                    .format("hello!").format(12.0).format(255).format(true).format(255);
  INFO("result is " << res);
  REQUIRE(res.is_equal("a string: hello!; a float number: 1.2e+01; an octal number: 377; a boolean: true; a hex: ff"));
}

TEST_CASE("format_field_size", "[String]")
{
  yat::String res = yat::StringFormat("fixed field size format: [{20s}]").format("hello!");
  INFO("result is " << res);
  REQUIRE(res.is_equal("fixed field size format: [              hello!]"));
}

TEST_CASE("format_field_size_fill", "[String]")
{
  yat::String res = yat::StringFormat("fixed field size format: [{_20s}]").format("hello!");
  INFO("result is " << res);
  REQUIRE(res.is_equal("fixed field size format: [______________hello!]"));
}

TEST_CASE("format_left_field_size", "[String]")
{
  yat::String res = yat::StringFormat("left format: [{<20s}]").format("hello!");
  INFO("result is " << res);
  REQUIRE(res.is_equal("left format: [hello!              ]"));
}

TEST_CASE("format_right_field_size", "[String]")
{
  yat::String res = yat::StringFormat("right format: [{>20s}]").format("hello!");
  INFO("result is " << res);
  REQUIRE(res.is_equal("right format: [              hello!]"));
}

