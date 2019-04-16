// The next line is used for the Windows Seven 32bits compilation.
// It resolved an external link problem for the use of OS-specific functions described in "FileName.h" (for FSBytes tests).
#pragma comment (lib, "Shell32")

#include "cppunit_YAT.h"

CPPUNIT_TEST_SUITE_REGISTRATION (cppunit_YAT);

// Override setUp() to initialize the variables
void cppunit_YAT :: setUp (void)
{
	// set up test environment (initializing objects)
}

// Override tearDown() to release any permanent resources you allocated in setUp()
void cppunit_YAT :: tearDown (void)
{
	// finally delete objects
}

//--------------------------------------------------------------------------
// Test functions. The name is "test_" + <Class name> + "_" + <function name> + "_" + <ID of function template> .
//--------------------------------------------------------------------------

//******************************************************************************************************************
// Function "split"
//******************************************************************************************************************
void cppunit_YAT :: test_String_split_A (void)
{
	yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
	std::vector<std::string> vExpectedStdValues;
	std::size_t oExpectedSize;
	std::vector<std::string> vResultStdValues;
	std::size_t oResultSize;

	// Prepare expected values
	vExpectedStdValues.push_back(std::string(" This"));
	vExpectedStdValues.push_back(std::string("is"));
	vExpectedStdValues.push_back(std::string("a"));
	vExpectedStdValues.push_back(std::string("split"));
	vExpectedStdValues.push_back(std::string("test,obviously.  "));
	oExpectedSize = vExpectedStdValues.size();

	//******************** Test "Multi-split" ********************
	// Split the phrase
	strTestPhraseToSplit.split('_', &vResultStdValues);

	// Check the number of substrings
	oResultSize = vResultStdValues.size();
	CPPUNIT_ASSERT_MESSAGE("String::split() : Unexpected number of substrings : "  + yat::StringUtil::to_string(oResultSize)
	+ " instead of " + yat::StringUtil::to_string(oExpectedSize) + "."
	, oResultSize == oExpectedSize);

	// Check every element
	for (std::size_t i=0; i< oExpectedSize; i++)
	{
		CPPUNIT_ASSERT_MESSAGE("String::split() : Bad substring #" + yat::StringUtil::to_string(i) + ": \""
		+ vResultStdValues[i] + "\" instead of \"" + vExpectedStdValues[i] + "\"."
		, vResultStdValues[i] == vExpectedStdValues[i]);
	}

}
//............................................................................
// Same test than "test_String_split_A", with no previous clearing of the output vector.
//............................................................................
void cppunit_YAT :: test_String_split_B (void)
{
	yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
	std::vector<std::string> vExpectedStdValues;
	std::size_t oExpectedSize;
	std::vector<std::string> vResultStdValues;
	std::size_t oResultSize;
	std::size_t oResultSizeBase;

	// Prepare output vector
	vResultStdValues.push_back(std::string("Test line not to be cleared"));
	vResultStdValues.push_back(std::string("Second Test line not to be cleared"));
	oResultSizeBase = vResultStdValues.size();

	// Prepare expected values
	vExpectedStdValues.push_back(std::string(" This"));
	vExpectedStdValues.push_back(std::string("is"));
	vExpectedStdValues.push_back(std::string("a"));
	vExpectedStdValues.push_back(std::string("split"));
	vExpectedStdValues.push_back(std::string("test,obviously.  "));
	oExpectedSize = vExpectedStdValues.size();

	// Code left to illustrate direct output, for debugging	in further developpements.
	// std::cout << "\n>> Before:\nvExpectedStdValues = " << vExpectedStdValues.size() << "\nvResultStdValues = " << vResultStdValues.size() << ".";
	//******************** Test "Multi-split" ********************
	// Split the phrase
	strTestPhraseToSplit.split('_', &vResultStdValues, false);

	// Check the number of substrings
	oResultSize = vResultStdValues.size();
	CPPUNIT_ASSERT_MESSAGE("String::split() : Unexpected number of substrings : "  + yat::StringUtil::to_string(oResultSize)
	+ " instead of " + yat::StringUtil::to_string(oExpectedSize + oResultSizeBase) + "."
	, oResultSize == (oExpectedSize + oResultSizeBase));

	// Check every new element
	for (std::size_t i=0; i< oExpectedSize; i++)
	{
		CPPUNIT_ASSERT_MESSAGE("String::split() : Bad substring #" + yat::StringUtil::to_string(i) + ": \""
		+ vResultStdValues[i+oResultSizeBase] + "\" instead of \"" + vExpectedStdValues[i] + "\"."
		, vResultStdValues[i+oResultSizeBase] == vExpectedStdValues[i]);
	}

}

void cppunit_YAT :: test_String_split_C (void)
{
	yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
	std::vector<yat::String> vExpectedYatValues;
	std::size_t oExpectedSize;
	std::vector<yat::String> vResultYatValues;
	std::size_t oResultSize;

	// Prepare expected values
	vExpectedYatValues.push_back(yat::String(" This"));
	vExpectedYatValues.push_back(yat::String("is"));
	vExpectedYatValues.push_back(yat::String("a"));
	vExpectedYatValues.push_back(yat::String("split"));
	vExpectedYatValues.push_back(yat::String("test,obviously.  "));
	oExpectedSize = vExpectedYatValues.size();

	//******************** Test "Multi-split" ********************
	// Split the phrase
	strTestPhraseToSplit.split('_', &vResultYatValues);

	// Check the number of substrings
	oResultSize = vResultYatValues.size();
	CPPUNIT_ASSERT_MESSAGE("String::split() : Unexpected number of substrings : "  + yat::StringUtil::to_string(oResultSize)
	+ " instead of " + yat::StringUtil::to_string(oExpectedSize) + "."
	, oResultSize == oExpectedSize);

	// Check every element
	for (std::size_t i=0; i< oExpectedSize; i++)
	{
		CPPUNIT_ASSERT_MESSAGE("String::split() : Bad substring #" + yat::StringUtil::to_string(i) + ": \""
		+ vResultYatValues[i] + "\" instead of \"" + vExpectedYatValues[i] + "\"."
		, vResultYatValues[i] == vExpectedYatValues[i]);
	}

}
//............................................................................
// Same test than "test_String_split_C", with no previous clearing of the output vector.
//............................................................................
void cppunit_YAT :: test_String_split_D (void)
{
	yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
	std::vector<yat::String> vExpectedYatValues;
	std::size_t oExpectedSize;
	std::vector<yat::String> vResultYatValues;
	std::size_t oResultSize;
	std::size_t oResultSizeBase;

	// Prepare output vector
	vResultYatValues.push_back(yat::String("Test line not to be cleared"));
	vResultYatValues.push_back(yat::String("Second Test line not to be cleared"));
	oResultSizeBase = vResultYatValues.size();

	// Prepare expected values
	vExpectedYatValues.push_back(yat::String(" This"));
	vExpectedYatValues.push_back(yat::String("is"));
	vExpectedYatValues.push_back(yat::String("a"));
	vExpectedYatValues.push_back(yat::String("split"));
	vExpectedYatValues.push_back(yat::String("test,obviously.  "));
	oExpectedSize = vExpectedYatValues.size();

	//******************** Test "Multi-split" ********************
	// Split the phrase
	strTestPhraseToSplit.split('_', &vResultYatValues, false);

	// Check the number of substrings
	oResultSize = vResultYatValues.size();
	CPPUNIT_ASSERT_MESSAGE("String::split() : Unexpected number of substrings : "  + yat::StringUtil::to_string(oResultSize)
	+ " instead of " + yat::StringUtil::to_string(oExpectedSize + oResultSizeBase) + "."
	, oResultSize == (oExpectedSize + oResultSizeBase));

	// Check every new element
	for (std::size_t i=0; i< oExpectedSize; i++)
	{
		CPPUNIT_ASSERT_MESSAGE("String::split() : Bad substring #" + yat::StringUtil::to_string(i) + ": \""
		+ vResultYatValues[i+oResultSizeBase] + "\" instead of \"" + vExpectedYatValues[i] + "\"."
		, vResultYatValues[i+oResultSizeBase] == vExpectedYatValues[i]);
	}

}

void cppunit_YAT :: test_String_split_E (void)
{
	//******************** Test "Simple-split" ********************
	yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
	std::string strStdLeft("");
	std::string strStdRight("");
	std::string strStdLeftExpected(" This_is_a_split_test");
	std::string strStdRightExpected("obviously.  ");

	// Split the phrase	in two parts
	strTestPhraseToSplit.split(',', &strStdLeft, &strStdRight);

	// Check each part
	CPPUNIT_ASSERT_MESSAGE("String::split() : Bad left substring : \"" + strStdLeft + "\" instead of \"" + strStdLeftExpected + "\"."
	, strStdLeft == strStdLeftExpected);
	CPPUNIT_ASSERT_MESSAGE("String::split() : Bad right substring : \"" + strStdRight + "\" instead of \"" + strStdRightExpected + "\"."
	, strStdRight == strStdRightExpected);
}

void cppunit_YAT :: test_String_split_F (void)
{
	//******************** Test "Simple-split" ********************
	yat::String strTestPhraseToSplit(" This_is_a_split_test,obviously.  ");
	yat::String strYatLeft("");
	yat::String strYatRight("");
	yat::String strYatLeftExpected(" This_is_a_split_test");
	yat::String strYatRightExpected("obviously.  ");

	// Split the phrase	in two parts
	strTestPhraseToSplit.split(',', &strYatLeft, &strYatRight);

	// Check each part
	CPPUNIT_ASSERT_MESSAGE("String::split() : Bad left substring : \"" + strYatLeft + "\" instead of \"" + strYatLeftExpected + "\"."
	, strYatLeft == strYatLeftExpected);
	CPPUNIT_ASSERT_MESSAGE("String::split() : Bad right substring : \"" + strYatRight + "\" instead of \"" + strYatRightExpected + "\"."
	, strYatRight == strYatRightExpected);
}

//******************************************************************************************************************
// Function "extract_token"
//******************************************************************************************************************
void cppunit_YAT :: test_String_extract_token_A (void)
{
	//******************** Test std-type token extraction (beginning) ********************
	yat::String strPhraseToSplit("First;Second;Third;Fourth");
	std::string strStdExtracted("");
	yat::String strPhraseToSplitExpected("Second;Third;Fourth");
	std::string strStdExtractedExpected("First");

	strPhraseToSplit.extract_token(';', &strStdExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad main std substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad token std substring : \""
	+ strStdExtracted + "\" instead of \"" + strStdExtractedExpected + "\"."
	, strStdExtracted == strStdExtractedExpected);
}

void cppunit_YAT :: test_String_extract_token_B (void)
{
	//******************** Test yat-type token extraction (beginning) ********************
	yat::String strPhraseToSplit("First;Second;Third;Fourth");
	yat::String strExtracted("");
	yat::String strPhraseToSplitExpected("Second;Third;Fourth");
	yat::String strExtractedExpected("First");

	strPhraseToSplit.extract_token(';', &strExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad main yat substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad token yat substring : \""
	+ strExtracted + "\" instead of \"" + strExtractedExpected + "\"."
	, strExtracted == strExtractedExpected);
}

void cppunit_YAT :: test_String_extract_token_C (void)
{
	//******************** Test std-type token extraction (flanked) ********************
	yat::String strPhraseToSplit = "One,Two;Three:Four.Five";
	std::string strStdExtracted = "";
	yat::String strPhraseToSplitExpected = "One,TwoFive";
	std::string strStdExtractedExpected = "Three:Four";

	strPhraseToSplit.extract_token(';', '.', &strStdExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad main std substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad token std substring : \""
	+ strStdExtracted + "\" instead of \"" + strStdExtractedExpected + "\"."
	, strStdExtracted == strStdExtractedExpected);
}

void cppunit_YAT :: test_String_extract_token_D (void)
{
	//******************** Test yat-type token extraction (flanked) ********************
	yat::String strPhraseToSplit = "One,Two;Three:Four.Five";
	yat::String strExtracted = "";
	yat::String strPhraseToSplitExpected = "One,TwoFive";
	yat::String strExtractedExpected = "Three:Four";

	strPhraseToSplit.extract_token(';', '.', &strExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad main yat substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad token yat substring : \""
	+ strExtracted + "\" instead of \"" + strExtractedExpected + "\"."
	, strExtracted == strExtractedExpected);
}

//******************************************************************************************************************
// Function "extract_token_right"
//******************************************************************************************************************
void cppunit_YAT :: test_String_extract_token_right_A (void)
{
	//******************** Test std-type token extraction (right-beginning) ********************
	yat::String strPhraseToSplit("First;Second;Third;Fourth");
	std::string strStdExtracted("");
	yat::String strPhraseToSplitExpected("First;Second;Third");
	std::string strStdExtractedExpected("Fourth");

	strPhraseToSplit.extract_token_right(';', &strStdExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token_right() : Bad main std substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token_right() : Bad token std substring : \""
	+ strStdExtracted + "\" instead of \"" + strStdExtractedExpected + "\"."
	, strStdExtracted == strStdExtractedExpected);
}

void cppunit_YAT :: test_String_extract_token_right_B (void)
{
	//******************** Test yat-type token extraction (right-beginning) ********************
	yat::String strPhraseToSplit("First;Second;Third;Fourth");
	yat::String strExtracted("");
	yat::String strPhraseToSplitExpected("First;Second;Third");
	yat::String strExtractedExpected("Fourth");

	strPhraseToSplit.extract_token_right(';', &strExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token_right() : Bad main yat substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token_right() : Bad token yat substring : \""
	+ strExtracted + "\" instead of \"" + strExtractedExpected + "\"."
	, strExtracted == strExtractedExpected);
}

void cppunit_YAT :: test_String_extract_token_right_C (void)
{
	//******************** Test std-type token extraction (flanked) ********************
	yat::String strPhraseToSplit = "One,Two;Three:Four.Five";
	std::string strStdExtracted = "";
	yat::String strPhraseToSplitExpected = "One,TwoFive";
	std::string strStdExtractedExpected = "Three:Four";

	strPhraseToSplit.extract_token(';', '.', &strStdExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad main std substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad token std substring : \""
	+ strStdExtracted + "\" instead of \"" + strStdExtractedExpected + "\"."
	, strStdExtracted == strStdExtractedExpected);
}

void cppunit_YAT :: test_String_extract_token_right_D (void)
{
	// //******************** Test yat-type token extraction (flanked) ********************
	yat::String strPhraseToSplit = "One,Two;Three:Four.Five";
	yat::String strExtracted = "";
	yat::String strPhraseToSplitExpected = "One,TwoFive";
	yat::String strExtractedExpected = "Three:Four";

	strPhraseToSplit.extract_token(';', '.', &strExtracted);

	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad main yat substring : \""
	+ strPhraseToSplit + "\" instead of \"" + strPhraseToSplitExpected + "\"."
	, strPhraseToSplit == strPhraseToSplitExpected);
	CPPUNIT_ASSERT_MESSAGE("String::extract_token() : Bad token yat substring : \""
	+ strExtracted + "\" instead of \"" + strExtractedExpected + "\"."
	, strExtracted == strExtractedExpected);
}

void cppunit_YAT :: test_StringTemplate_value_A (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialValue = "CPP_UNIT_YAT_TEST_2";
	std::string strExpectedValue = "alpha";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.value(&strInitialValue);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template value : \""
	+ strInitialValue + "\" instead of \"" + strExpectedValue + "\"."
	, strInitialValue == strExpectedValue);
}

void cppunit_YAT :: test_StringTemplate_value_B (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialValue = "THIS_PARAMETER_DOES_NOT_EXISTS|CPP_UNIT_YAT_TEST_2";
	std::string strExpectedValue = "alpha";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.value(&strInitialValue);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template value : \""
	+ strInitialValue + "\" instead of \"" + strExpectedValue + "\"."
	, strInitialValue == strExpectedValue);
}

void cppunit_YAT :: test_StringTemplate_value_C (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialValue = "THIS_PARAMETER_DOES_NOT_EXISTS|'beta'";
	std::string strExpectedValue = "beta";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.value(&strInitialValue);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template value : \""
	+ strInitialValue + "\" instead of \"" + strExpectedValue + "\"."
	, strInitialValue == strExpectedValue);
}

void cppunit_YAT :: test_StringTemplate_substitute_A (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Terms to substitute : $(CPP_UNIT_YAT_TEST_1) and '$(CPP_UNIT_YAT_TEST_2)'";
	std::string strExpectedPhrase = "Terms to substitute : Test of the function StringTemplate::substitute() and 'alpha'";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute(&strInitialPhrase);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitution : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);
}

void cppunit_YAT :: test_StringTemplate_substitute_B (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Term to substitute : $(THIS_PARAMETER_DOES_NOT_EXISTS|CPP_UNIT_YAT_TEST_1)";
	std::string strExpectedPhrase = "Term to substitute : Test of the function StringTemplate::substitute()";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute(&strInitialPhrase);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitution : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);
}

void cppunit_YAT :: test_StringTemplate_substitute_C (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Term to substitute : $(THIS_PARAMETER_DOES_NOT_EXISTS|'gamma')";
	std::string strExpectedPhrase = "Term to substitute : gamma";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute(&strInitialPhrase);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitution : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);
}

void cppunit_YAT :: test_StringTemplate_substitute_D (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Term to substitute : $(uc:CPP_UNIT_YAT_TEST_1)";
	std::string strExpectedPhrase = "Term to substitute : TEST OF THE FUNCTION STRINGTEMPLATE::SUBSTITUTE()";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute(&strInitialPhrase);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitution : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);
}

void cppunit_YAT :: test_StringTemplate_substitute_E (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Term to substitute : $(lc:CPP_UNIT_YAT_TEST_1)";
	std::string strExpectedPhrase = "Term to substitute : test of the function stringtemplate::substitute()";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute(&strInitialPhrase);

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitution : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);
}

void cppunit_YAT :: test_StringTemplate_substitute_ex_A (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Terms to substitute : $(CPP_UNIT_YAT_TEST_1), '$(THIS_PARAMETER_DOES_NOT_EXISTS)' and '$(CPP_UNIT_YAT_TEST_2)'";
	std::string strExpectedPhrase = "Terms to substitute : Test of the function StringTemplate::substitute(), 'THIS_PARAMETER_DOES_NOT_EXISTS' and 'alpha'";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	std::vector<std::string> vstrNotFound;
	size_t oNumberOfBadElements = vstrNotFound.size();
	size_t oExpectedNumberOfBadElements = 1;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute_ex(&strInitialPhrase, &vstrNotFound);
	oNumberOfBadElements = vstrNotFound.size();

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	// Check that we obtain the expected phrase
	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitutions : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);

	// Check that we obtain the expected number of unrecognized tags.
	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad number of template substitutions : \""
	+ yat::StringUtil::to_string(oNumberOfBadElements) + "\" instead of \"" + yat::StringUtil::to_string(oExpectedNumberOfBadElements) + "\"."
	, ((int)oNumberOfBadElements) == ((int)oExpectedNumberOfBadElements));
}

void cppunit_YAT :: test_StringTemplate_substitute_ex_B (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Terms to substitute : $(uc:CPP_UNIT_YAT_TEST_1), '$(THIS_PARAMETER_DOES_NOT_EXISTS)' and '$(CPP_UNIT_YAT_TEST_2)'";
	std::string strExpectedPhrase = "Terms to substitute : TEST OF THE FUNCTION STRINGTEMPLATE::SUBSTITUTE(), 'THIS_PARAMETER_DOES_NOT_EXISTS' and 'alpha'";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	std::vector<std::string> vstrNotFound;
	size_t oNumberOfBadElements = vstrNotFound.size();
	size_t oExpectedNumberOfBadElements = 1;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute_ex(&strInitialPhrase, &vstrNotFound);
	oNumberOfBadElements = vstrNotFound.size();

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	// Check that we obtain the expected phrase
	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitutions : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);

	// Check that we obtain the expected number of unrecognized tags.
	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad number of template substitutions : \""
	+ yat::StringUtil::to_string(oNumberOfBadElements) + "\" instead of \"" + yat::StringUtil::to_string(oExpectedNumberOfBadElements) + "\"."
	, ((int)oNumberOfBadElements) == ((int)oExpectedNumberOfBadElements));
}

void cppunit_YAT :: test_StringTemplate_substitute_ex_C (void)
{
	//******************** Test StringTemplate ********************
	std::string strInitialPhrase = "Terms to substitute : $(lc:CPP_UNIT_YAT_TEST_1), '$(THIS_PARAMETER_DOES_NOT_EXISTS)' and '$(CPP_UNIT_YAT_TEST_2)'";
	std::string strExpectedPhrase = "Terms to substitute : test of the function stringtemplate::substitute(), 'THIS_PARAMETER_DOES_NOT_EXISTS' and 'alpha'";

	yat::EnvVariableInterpreter* poVarInterpreter = new yat::EnvVariableInterpreter();
	yat::StringTemplate oStrTemplate;

	std::vector<std::string> vstrNotFound;
	size_t oNumberOfBadElements = vstrNotFound.size();
	size_t oExpectedNumberOfBadElements = 1;

	oStrTemplate.add_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));

	oStrTemplate.substitute_ex(&strInitialPhrase, &vstrNotFound);
	oNumberOfBadElements = vstrNotFound.size();

	oStrTemplate.remove_symbol_interpreter((yat::ISymbolInterpreter *)(poVarInterpreter));
	delete poVarInterpreter;

	// Check that we obtain the expected phrase
	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad template substitutions : \""
	+ strInitialPhrase + "\" instead of \"" + strExpectedPhrase + "\"."
	, strInitialPhrase == strExpectedPhrase);

	// Check that we obtain the expected number of unrecognized tags.
	CPPUNIT_ASSERT_MESSAGE("StringTemplate::substitute() : Bad number of template substitutions : \""
	+ yat::StringUtil::to_string(oNumberOfBadElements) + "\" instead of \"" + yat::StringUtil::to_string(oExpectedNumberOfBadElements) + "\"."
	, ((int)oNumberOfBadElements) == ((int)oExpectedNumberOfBadElements));
}

void cppunit_YAT :: test_FSBytes_to (void)
{
	//******************** Test FSBytes ********************
	yat::uint64 uiInitialValueInBytes = 123456789;
	double dExpectedValueInMBytes = 123.456789;
	double dConvertedValueInMBytes = 0.0;

	// Define the element to be tested
	yat::FileName::FSBytes oInitialValueInBytes(uiInitialValueInBytes);

	// Test the conversion
	dConvertedValueInMBytes = oInitialValueInBytes.to(yat::FileName::FSBytes::MBYTES);

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad converted value : \""
	+ yat::StringUtil::to_string(dConvertedValueInMBytes) + "\" instead of \"" + yat::StringUtil::to_string(dExpectedValueInMBytes) + "\"."
	, dConvertedValueInMBytes == dExpectedValueInMBytes);
}

void cppunit_YAT :: test_FSBytes_from_A (void)
{
	//******************** Test FSBytes ********************
	yat::uint64 uiExpectedValueInBytes = 1248163264;
	double dInitialValueInGBytes = 1.248163264;

	// Define the element to be tested
	yat::FileName::FSBytes oTestedValueInBytes;
	oTestedValueInBytes.from(dInitialValueInGBytes, yat::FileName::FSBytes::GBYTES);

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad converted value : \""
	+ yat::StringUtil::to_string(oTestedValueInBytes) + "\" instead of \"" + yat::StringUtil::to_string(uiExpectedValueInBytes) + "\"."
	, oTestedValueInBytes == uiExpectedValueInBytes);
}

void cppunit_YAT :: test_FSBytes_from_B (void)
{
	//******************** Test FSBytes ********************
	yat::uint64 uiExpectedValueInBytes = 1248163264;
	std::string strInitialValueInKBytes("1248163.264kbytes");

	// Define the element to be tested
	yat::FileName::FSBytes oTestedValueInBytes;
	oTestedValueInBytes.from(strInitialValueInKBytes);

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad converted value : \""
	+ yat::StringUtil::to_string(oTestedValueInBytes) + "\" instead of \"" + yat::StringUtil::to_string(uiExpectedValueInBytes) + "\"."
	, oTestedValueInBytes == uiExpectedValueInBytes);
}

void cppunit_YAT :: test_FSBytes_to_string_A (void)
{
	//******************** Test FSBytes ********************
	yat::uint64 uiInitialValueInBytes = 1248163264;
	std::string strExpectedValueInGBytes("1.248163264G");
	std::string strConvertedValueInGBytes("");

	// Define the element to be tested
	yat::FileName::FSBytes oTestedValueInBytes(uiInitialValueInBytes);
	strConvertedValueInGBytes = oTestedValueInBytes.to_string();

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad converted value : \""
	+ strConvertedValueInGBytes + "\" instead of \"" + strExpectedValueInGBytes + "\"."
	, strConvertedValueInGBytes == strExpectedValueInGBytes);
}

void cppunit_YAT :: test_FSBytes_to_string_B (void)
{
	//******************** Test FSBytes ********************
	yat::uint64 uiInitialValueInBytes = 1248163264;
	std::string strExpectedValueInGBytes("1.248163264G bytes");
	std::string strConvertedValueInGBytes("");

	// Define the element to be tested
	yat::FileName::FSBytes oTestedValueInBytes(uiInitialValueInBytes);
	strConvertedValueInGBytes = oTestedValueInBytes.to_string(true);

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad converted value : \""
	+ strConvertedValueInGBytes + "\" instead of \"" + strExpectedValueInGBytes + "\"."
	, strConvertedValueInGBytes == strExpectedValueInGBytes);
}

void cppunit_YAT :: test_FSBytes_operator_A (void)
{
	//******************** Test FSBytes ********************
	// Define the elements to be tested
	yat::FileName::FSBytes oTestedValueInBytesInf(11111);
	yat::FileName::FSBytes oTestedValueInBytesSup(22222);

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad comparison result : \""
	+ yat::StringUtil::to_string(oTestedValueInBytesInf) + "\" must be lower than  \"" + yat::StringUtil::to_string(oTestedValueInBytesSup) + "\"."
	, oTestedValueInBytesInf < oTestedValueInBytesSup);
}

void cppunit_YAT :: test_FSBytes_operator_B (void)
{
	//******************** Test FSBytes ********************
	// Define the elements to be tested
	yat::FileName::FSBytes oTestedValueInBytesInf(11111);
	yat::FileName::FSBytes oTestedValueInBytesSup(22222);

	// Check that we obtain the expected results.
	CPPUNIT_ASSERT_MESSAGE("FSBytes::to() : Bad comparison result : \""
	+ yat::StringUtil::to_string(oTestedValueInBytesSup) + "\" must be greater than  \"" + yat::StringUtil::to_string(oTestedValueInBytesInf) + "\"."
	, oTestedValueInBytesSup > oTestedValueInBytesInf);
}

// Commented code left to illustrate a test based on exception catching (for further developpements)
// void cppunit_YAT :: exceptionTest (void)
// {
	// // an exception has to be thrown here
	// CPPUNIT_ASSERT_THROW (Fraction (1, 0), DivisionByZeroException);
// }

