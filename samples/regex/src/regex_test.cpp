//----------------------------------------------------------------------------
// Copyright (c) 2004-2021 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file
 * \brief    An example of yat::Regex usage
 * \author   S. Poirier - Synchrotron SOLEIL
 */

#include <iostream>
#include <yat/utils/Logging.h>
#include <yat/Exception.h>
#include <yat/regex/Regex.h>

typedef yat::Format _strf;

//-----------------------------------------------------------------------------
void match_pattern(const yat::String& in, const yat::String& pattern)
{
  yat::Regex re(pattern);
  yat::Format sfmt("'{}' is an exact match of '{}': {}");
  sfmt.arg(in).arg(re.pattern());
  std::cout << sfmt.arg(re.match(in, NULL)) << std::endl;
}

//-----------------------------------------------------------------------------
void coordinates_parser(const yat::String& in)
{
  std::cout << "Parsing coordinates from " << in << std::endl;
  yat::Regex re(R"(Rect\s*\(([0-9]+),([0-9]+)\),-\[([0-9]+),([0-9]+)\])");
  yat::Regex::Match m;
  yat::String str("Rect(0,0),-[659,494]");
  if( re.search(in, &m) )
  {
    for( std::size_t i = 1; i < 5; ++i )
    {
      std::cout << m[i] << ' ';
    }
    std::cout << std::endl;
  }
}

//-----------------------------------------------------------------------------
void extract_submatches_with_iterator(const yat::String& in)
{
  std::cout << "Extract submatches from " << in << std::endl;
  yat::Regex re(R"((www)\.([-_[:alnum:]]+)\.([[:lower:]]{2,4}))");
  yat::Regex::Match m;
  re.search(in, &m);
  for( yat::Regex::Match::iterator it = m.begin(); it != m.end(); ++it )
  {
    std::cout << *it << std::endl;
  }
}

//-----------------------------------------------------------------------------
void extract_submatches(const yat::String& in, const yat::String& pattern)
{
  yat::Regex re(pattern);

  // perform a simple match
  std::cout << _strf("Is '{}' contains pattern '{}': {}")
                    .arg(in).arg(pattern).arg(re.search(in)) << std::endl;

   // show contents of marked subexpressions within each match
  yat::Regex::Match m;
  if( re.search(in, &m) )
  {
      std::cout << m.size() << " submatches for '" << in << "'\n";
      std::cout << "Prefix: '" << m.prefix() << "'\n";
      for (size_t j = 0; j < m.size(); ++j)
          std::cout << j << ": " << m.str(j) << '\n';
      std::cout << "Suffix: '" << m.suffix() << "\'\n\n";
  }
}

//-----------------------------------------------------------------------------
void repeated_searches(const yat::String& in, const yat::String& pattern)
{
  yat::Regex re(pattern);
  yat::Regex::Match m;
  while( re.search(in, &m) )
  {
    std::cout << m.str() << " >> " << m.suffix() << std::endl;
    for (size_t j = 0; j < m.size(); ++j)
        std::cout << j << ": '" << m.str(j) << "'" << std::endl;
  }
}

//-----------------------------------------------------------------------------
void replace_test(const yat::String& in, const yat::String& from,
                  const yat::String& to, yat::Regex::MatchFlags mflags = yat::Regex::match_default)
{
  yat::Regex re(from);
  yat::String out = re.replace(in, to, mflags);
  std::cout << _strf("Input string: {}. Search for: {}. Replace by: {}. Output: {}")
               .arg(in).arg(from).arg(to).arg(out) << std::endl;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  std::cout << std::endl;
  try
  {
/*
    coordinates_parser("Rect(0,0),-[659,494]");
    std::cout << std::endl;

    yat::String http_addr_pattern(R"(www\.[-_[:alnum:]]+\.[[:lower:]]{2,4})");
    match_pattern("ww.cppreference.com", http_addr_pattern);
    match_pattern("www.cppreference.info", http_addr_pattern);
    match_pattern("www.cppreference.Com", http_addr_pattern);
    match_pattern("www.cppreference.com", http_addr_pattern);
    match_pattern("Two words", R"(\w+ \w+)");
    std::cout << std::endl;

    extract_submatches_with_iterator("http://www.cppreference.com/w/cpp");
    std::cout << std::endl;

    const yat::String color_pattern("#([a-f0-9]{2})([a-f0-9]{2})([a-f0-9]{2})");
    extract_submatches("Roses are #ff0000.", color_pattern);
    extract_submatches("Violets are #0000ff.", color_pattern);
    extract_submatches("All of my base are belong to you.", color_pattern);
    std::cout << std::endl;

    // repeated searches
    repeated_searches("Two words", R"(\w+)");
    std::cout << std::endl;

    repeated_searches("Speed: 366, Mass: 35.Speed: 378; Mass: 32! \n   Speed: 400,  Mass: 30", "Speed: [0-9]+");
    std::cout << std::endl;
*/
    std::cout << "A few replace examples...\n" << std::endl;
    replace_test("ga bu zo meu", "zo", "zozo");
    replace_test("ga bu zo meu", "\\w+", "[$0]");
    replace_test("ga bu zo meu", "bu", "$&!!");
    replace_test("ga bu zo meu", "(ga)(.*)(meu)", "$3$2$1");
    replace_test("ga bu zo meu", "(ga) (bu) (zo) (meu)", "$4 $3 $2 $1");
    replace_test("ga bu zo meu", "bu zo", "$'$0$`");
    std::cout << "In the next example, non matching strings are not copied into the output" << std::endl;
    replace_test("ga bu zo meu", "zo", "zozo", yat::Regex::format_no_copy);
    replace_test("ga 1000€", "ga [0-9]+€", "bu 1100$$ > $0 !");
    std::cout << std::endl;

  }
  catch(const yat::Exception& e)
  {
    YAT_LOG_EXCEPTION(e);
  }
  return 0;
}
