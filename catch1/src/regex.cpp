#include "catch.hpp"
#include <yat/regex/Regex.h>

TEST_CASE("regex_basic_match", "[Regex]")
{
  { yat::Regex re("ab");
    REQUIRE(re.match("ab"));
  }
  { yat::Regex re("[a-z]+_[0-9]+");
    REQUIRE(re.match("abc_01"));
    REQUIRE(!re.match("01_abc"));
  }
}
TEST_CASE("regex_search", "[Regex]")
{
  yat::Regex re("a|b");
  std::vector<yat::Regex::Match> matchs;
  yat::Regex::Match m;
  while( re.search("abcdabcdabcd", &m) )
    matchs.push_back(m);
  INFO("expected 6 matchs: 'a', 'b', 'a', 'b', 'a', 'b'");

  REQUIRE(6 == matchs.size());
  bool b1 = matchs[0].str() == matchs[2].str() && matchs[0].str() == matchs[4].str() && matchs[0].str() == "a";
  REQUIRE(b1);
  bool b2 = matchs[1].str() == matchs[3].str() && matchs[1].str() == matchs[5].str() && matchs[1].str() == "b";
  REQUIRE(b2);
}

TEST_CASE("regex_search_vec", "[Regex]")
{
  yat::Regex re("a|b");
  std::vector<yat::Regex::Match> matchs;
  re.search("abcdabcdabcd", &matchs);
  INFO("expected 6 matchs: 'a', 'b', 'a', 'b', 'a', 'b'");

  REQUIRE(6 == matchs.size());
  bool b1 = matchs[0].str() == matchs[2].str() && matchs[0].str() == matchs[4].str() && matchs[0].str() == "a";
  REQUIRE(b1);
  bool b2 = matchs[1].str() == matchs[3].str() && matchs[1].str() == matchs[5].str() && matchs[1].str() == "b";
  REQUIRE(b2);
}

TEST_CASE("regex_replace_1", "[Regex]")
{
  yat::Regex from("zo");
  yat::String out = re.replace(in, to, mflags);

  std::vector<yat::Regex::Match> matchs;
  re.search("abcdabcdabcd", &matchs);
  INFO("expected 6 matchs: 'a', 'b', 'a', 'b', 'a', 'b'");

  REQUIRE(6 == matchs.size());
  bool b1 = matchs[0].str() == matchs[2].str() && matchs[0].str() == matchs[4].str() && matchs[0].str() == "a";
  REQUIRE(b1);
  bool b2 = matchs[1].str() == matchs[3].str() && matchs[1].str() == matchs[5].str() && matchs[1].str() == "b";
  REQUIRE(b2);
}

