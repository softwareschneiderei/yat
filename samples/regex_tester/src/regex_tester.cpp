//----------------------------------------------------------------------------
// Copyright (c) 2004-2015 Synchrotron SOLEIL
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
#include <yat/utils/Json.h>
#include <yat/utils/Logging.h>
#include <yat/file/FileName.h>
#include <yat/utils/CommandLine.h>
#include <yat/Exception.h>
#include <yat/regex/Regex.h>

typedef yat::StringFormat _strf;

typedef enum ShowOpt
{
  none = 0, prefix = 1, suffix = 2, empty = 4, var = 8, sub = 16
} ShowOpt;

yat::String substitute(const yat::CfgFile::Parameters& kvg,
                       const yat::CfgFile::Parameters& kvl, const yat::String& in)
{
  // Allow variable like $var or $(var), where var is made of any
  // characters in [0-9a-zA-Z_]
  yat::Regex re(R"(\$(([_[:alnum:]]+)|(\(([_[:alnum:]]+)\))|\$))");
  yat::String v, to;
  yat::Regex::Match m;
  std::size_t non_match_start = 0, non_match_end = 0;
  while( re.search(in, &m) )
  {
    non_match_end = m.position(0);
    if( non_match_end - non_match_start > 0 )
      to.append(in.substr(non_match_start, non_match_end - non_match_start));

    v = m.str(4).empty() ? m.str(1): m.str(4);
    if( v == "$" )
      to.append(v);
    else
    {
      yat::CfgFile::Parameters::const_iterator cit1 = kvg.find(yat::String("$") + v);
      yat::CfgFile::Parameters::const_iterator cit2 = kvl.find(yat::String("$") + v);
      if( cit2 != kvl.end() )
        to.append(substitute(kvg, kvl, cit2->second));
      else if( cit1 != kvg.end() )
        to.append(substitute(kvg, kvg, cit1->second));
      else
      {
        std::cout << "error: unknown variable " << v << std::endl;
        return "";
      }
    }
    non_match_start = non_match_end + m.length(0);
  }
  if( non_match_start < in.size() )
    to.append(in.substr(non_match_start));

  return to;
}

//-----------------------------------------------------------------------------
void extract_submatches(const yat::Regex::Match& m, int opt)
{
  if( m.size() > 1 )
    std::cout << m.size() - 1 << " Submatches:\n";
  if( opt & prefix )
    std::cout << "  Prefix: '" << m.prefix() << "'\n";

  for (size_t j = 1; j < m.size(); ++j)
  {
    if( !m.str(j).empty() || (m.str(j).empty() && (opt & empty)) )
      std::cout << _strf("{%3d}: '{}'\n").format(j).format(m.str(j));
  }

  if( opt & suffix )
    std::cout << "  Suffix: '" << m.suffix() << "'\n";
  std::cout << '-' << std::endl;
}

//-----------------------------------------------------------------------------
void match(yat::RegexPtr re_ptr, const yat::String& str, int opt)
{
  yat::Regex::Match m;
  bool is_match = re_ptr->match(str, &m);
  std::cout << _strf("'{}' match -> {}").format(str).format(is_match) << std::endl;

   // show contents of marked subexpressions within each match
  if( is_match && (opt & sub) )
  {
    extract_submatches(m, opt);
  }
}

//-----------------------------------------------------------------------------
void search(yat::RegexPtr re_ptr, const yat::String& str, int opt)
{
  yat::Regex::Match m;
  std::cout << _strf("Search matches in '{}': ").format(str) << std::endl;
  while( re_ptr->search(str, &m) )
  {
    std::cout << "Match found: '" << m.str() << "'" << std::endl;
    if( opt & sub )
      extract_submatches(m, opt);
  }
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  yat::CommandLine cl;

  yat::String def_cfg_file("regex_tester.cfg");

  cl.add_opt('c', "cfg-file", "regex file", "Configuration file");
  cl.add_opt('p', "show-prefix", "", "Show prefix");
  cl.add_opt('s', "show-suffix", "", "Show suffix");
  cl.add_opt('v', "show-variables", "", "Show variables");
  cl.add_opt('m', "show-submatches", "", "Show sub-matches");
  cl.add_opt('e', "show-empty-submatch", "", "Show empty submatch");
  cl.add_opt('x', "test-to-be-executed", "test name", "Execute a specific test by its section name");

  int opt = 0;
  yat::String test_exec;

  try
  {
    bool b = cl.read(argc, argv);
    if( b )
    {
      yat::String file_name = "regex_tester.cfg";
      if( cl.is_option("cfg-file") )
        file_name = cl.option_value("cfg-file");

      if( cl.is_option("show-prefix") )
        opt |= prefix;
      if( cl.is_option("show-suffix") )
        opt |= suffix;
      if( cl.is_option("show-empty-submatch") )
        opt |= empty;
      if( cl.is_option("show-submatches") )
        opt |= sub;
      if( cl.is_option("show-variables") )
        opt |= var;
      if( cl.is_option("test-to-be-executed") )
        test_exec = cl.option_value("test-to-be-executed");

      yat::Regex re_test(test_exec);

      yat::CfgFile f(file_name);
      f.load();
      f.set_section(CFG_FILE_DEFAULT_SECTION);
      // pre-defined pattern
      yat::CfgFile::Parameters pg = f.get_parameters();
      for( yat::CfgFile::Parameters::const_iterator cit = pg.begin(); opt & var && cit != pg.end(); ++cit )
        std::cout << cit->first << " -> " << substitute(pg, yat::CfgFile::Parameters(), cit->second) << std::endl;

      std::list<std::string> sections;
      f.get_sections(&sections);
      for( std::list<std::string>::const_iterator cit = sections.begin(); cit != sections.end(); ++cit )
      {
        if( CFG_FILE_DEFAULT_SECTION == *cit || (!test_exec.empty() && !re_test.search(*cit)) )
          continue;

        std::cout << "----\ntest name: " << *cit << std::endl;
        f.set_section(*cit);
        yat::CfgFile::Parameters pl = f.get_parameters();
        for( yat::CfgFile::Parameters::const_iterator cit = pl.begin(); opt & var && cit != pl.end(); ++cit )
        {
          if( yat::StringUtil::start_with(cit->first, '$') )
            std::cout << cit->first << " -> " << substitute(pg, pl, cit->second) << std::endl;
        }

        yat::RegexPtr re_ptr = new yat::Regex(substitute(pg, pl, f.get_param_value("pattern")));
        std::cout << "Pattern: " << re_ptr->pattern() << "\n-" << std::endl;

        yat::String op = f.get_param_value("function");
        yat::CfgFile::Values test_strings = f.get_values();
        for( std::size_t i = 0; i < test_strings.size(); ++i )
        {
          if( op.is_equal_no_case("match") )
            match(re_ptr, test_strings[i], opt);
          else if( op.is_equal_no_case("search") )
            search(re_ptr, test_strings[i], opt);
        }
      }
    }
  }
  catch(const yat::Exception& e)
  {
    YAT_LOG_EXCEPTION(e);
  }
  return 0;
}
