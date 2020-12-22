//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the GNU Lesser Public License v3
// which accompanies this distribution, and is available at
// http://www.gnu.org/licenses/lgpl.html
//----------------------------------------------------------------------------
/*!
 * \file
 * \brief    An example of yat::Timeout usage
 * \author   N. Leclercq, J. Malik - Synchrotron SOLEIL
 */

#include <iostream>
#include <cstring>
#include <yat/time/Time.h>
#include <yat/utils/Logging.h>
#include <yat/file/FileName.h>
#include <yat/utils/Singleton.h>
#include <yat/utils/StringTemplate.h>
#include <yat/Exception.h>
#include <yat/regex/Regex.h>



typedef yat::StringFormat _strf;

//-----------------------------------------------------------------------------
void match_web_addr(const yat::String& in)
{
  // Regex test
  yat::StringFormat sfmt("'{}' is a regular web address: {}");
  sfmt.format(in);

  yat::Regex re(R"(www\.[-_[:alnum:]]+\.[[:lower:]]{2,4})", yat::Regex::extended);
  std::cout << sfmt.format(re.match(in)) << std::endl;
}

//-----------------------------------------------------------------------------
void coordinates_parser(const yat::String& in)
{
  std::cout << "Parsing coordinates from " << in << std::endl;
  yat::Regex re(R"(Rect\(([0-9]+),([0-9]+)\),-\[([0-9]+),([0-9]+)\])");
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
void extract_submatches(const yat::String& in)
{
  // hex color codes pattern
  yat::Regex re("#([a-f0-9]{2})([a-f0-9]{2})([a-f0-9]{2})");

  // perform a simple match
  std::cout << _strf("Is '{}' contains a hexadecimal color code: {}")
                    .format(in).format(re.search(in)) << std::endl;

   // show contents of marked subexpressions within each match
  yat::Regex::Match m;
  if( re.search(in, &m) )
  {
      std::cout << "matches for '" << in << "'\n";
      std::cout << "Prefix: '" << m.prefix() << "'\n";
      for (size_t j = 0; j < m.size(); ++j)
          std::cout << j << ": " << m.str(j) << '\n';
      std::cout << "Suffix: '" << m.suffix() << "\'\n\n";
  }
}

//-----------------------------------------------------------------------------
void repeated_searches(const yat::String& in)
{
  yat::Regex re("Speed: [0-9]+");
  yat::Regex::Match m;
  while( re.search(in, &m) )
  {
    std::cout << m.str() << '\n';
  }
}

//-----------------------------------------------------------------------------
void replace_test(const yat::String& in, const yat::String& from,
                  const yat::String& to, yat::Regex::MatchFlags mflags = yat::Regex::match_default)
{
  yat::Regex re(from);
  yat::String out = re.replace(in, to, mflags);
  std::cout << _strf("Input string: {}. Search for: {}. Replace by: {}. Result: {}")
               .format(in).format(from).format(to).format(out) << std::endl;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  std::cout << std::endl;
  try
  {
    coordinates_parser("Rect(0,0),-[659,494]");
    std::cout << std::endl;

    match_web_addr("ww.cppreference.com");
    match_web_addr("www.cppreference.info");
    match_web_addr("www.cppreference.Com");
    match_web_addr("www.cppreference.com");
    std::cout << std::endl;

    extract_submatches_with_iterator("http://www.cppreference.com/w/cpp");
    std::cout << std::endl;

    extract_submatches("Roses are #ff0000.");
    extract_submatches("Violets are #0000ff.");
    extract_submatches("All of my base are belong to you.");
    std::cout << std::endl;

    // repeated search
    repeated_searches("Speed: 366, Mass: 35.Speed: 378; Mass: 32! \n   Speed: 400,  Mass: 30");
    std::cout << std::endl;

    replace_test("ga bu zo meu", "zo", "zozo");
    replace_test("ga bu zo meu", "bu", "$&!!");
    replace_test("ga bu zo meu", "(ga) (bu) (zo) (meu)", "$4 $3 $2 $1");
    replace_test("ga bu zo meu", "bu zo", "$'$0$`");
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
