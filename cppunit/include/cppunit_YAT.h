#ifndef CPPUNIT_YAT_H
#define CPPUNIT_YAT_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <yat/utils/String.h>
#include <yat/utils/StringTemplate.h>
#include <yat/file/FileName.h>

using namespace std;

class cppunit_YAT : public CPPUNIT_NS :: TestFixture
{
	// First macro to indicate the beginning of the tests.
	CPPUNIT_TEST_SUITE (cppunit_YAT);

	// Marcos to launch the tests (each one is indicated by its specific function)
	// ----------------------------------------
	// Tests in the class "String"
	CPPUNIT_TEST (test_String_split_A);
	CPPUNIT_TEST (test_String_split_B);
	CPPUNIT_TEST (test_String_split_C);
	CPPUNIT_TEST (test_String_split_D);
	CPPUNIT_TEST (test_String_split_E);
	CPPUNIT_TEST (test_String_split_F);
	CPPUNIT_TEST (test_String_extract_token_A);
	CPPUNIT_TEST (test_String_extract_token_B);
	CPPUNIT_TEST (test_String_extract_token_C);
	CPPUNIT_TEST (test_String_extract_token_D);
	CPPUNIT_TEST (test_String_extract_token_right_A);
	CPPUNIT_TEST (test_String_extract_token_right_B);
	CPPUNIT_TEST (test_String_extract_token_right_C);
	CPPUNIT_TEST (test_String_extract_token_right_D);

	// Tests in the class "StringTemplate"
	CPPUNIT_TEST (test_StringTemplate_value_A);
	CPPUNIT_TEST (test_StringTemplate_value_B);
	CPPUNIT_TEST (test_StringTemplate_value_C);
	CPPUNIT_TEST (test_StringTemplate_substitute_A);
	CPPUNIT_TEST (test_StringTemplate_substitute_B);
	CPPUNIT_TEST (test_StringTemplate_substitute_C);
	CPPUNIT_TEST (test_StringTemplate_substitute_D);
	CPPUNIT_TEST (test_StringTemplate_substitute_E);
	CPPUNIT_TEST (test_StringTemplate_substitute_ex_A);
	CPPUNIT_TEST (test_StringTemplate_substitute_ex_B);
	CPPUNIT_TEST (test_StringTemplate_substitute_ex_C);

	// Tests in the class "FSBytes"
	CPPUNIT_TEST (test_FSBytes_to);
	CPPUNIT_TEST (test_FSBytes_from_A);
	CPPUNIT_TEST (test_FSBytes_from_B);
	CPPUNIT_TEST (test_FSBytes_to_string_A);
	CPPUNIT_TEST (test_FSBytes_to_string_B);
	CPPUNIT_TEST (test_FSBytes_operator_A);
	CPPUNIT_TEST (test_FSBytes_operator_B);

	// Commented code left to illustrate a test based on exception catching (for further developpements)
	// CPPUNIT_TEST (exceptionTest);

	// Last macro to indicate the end of the tests.
	CPPUNIT_TEST_SUITE_END ();

	public:
		// Functions to override
		void setUp (void);
		void tearDown (void);

	protected:
		// -------------------------------------------------------
		// Test functions. The name is "test_" + <Class name> + "_" + <function name> + "_" + <ID of function template> .
		// -------------------------------------------------------

		// Functions in class String
		void test_String_split_A (void);
		void test_String_split_B (void);
		void test_String_split_C (void);
		void test_String_split_D (void);
		void test_String_split_E (void);
		void test_String_split_F (void);
		void test_String_extract_token_A (void);
		void test_String_extract_token_B (void);
		void test_String_extract_token_C (void);
		void test_String_extract_token_D (void);
		void test_String_extract_token_right_A (void);
		void test_String_extract_token_right_B (void);
		void test_String_extract_token_right_C (void);
		void test_String_extract_token_right_D (void);

		// Functions in class StringTemplate
		void test_StringTemplate_value_A (void);
		void test_StringTemplate_value_B (void);
		void test_StringTemplate_value_C (void);
		void test_StringTemplate_value_D (void);
		void test_StringTemplate_value_E (void);
		void test_StringTemplate_substitute_A (void);
		void test_StringTemplate_substitute_B (void);
		void test_StringTemplate_substitute_C (void);
		void test_StringTemplate_substitute_D (void);
		void test_StringTemplate_substitute_E (void);
		void test_StringTemplate_substitute_ex_A (void);
		void test_StringTemplate_substitute_ex_B (void);
		void test_StringTemplate_substitute_ex_C (void);

		// Functions in class FSBytes
		void test_FSBytes_to (void);
		void test_FSBytes_from_A (void);
		void test_FSBytes_from_B (void);
		void test_FSBytes_to_string_A (void);
		void test_FSBytes_to_string_B (void);
		void test_FSBytes_operator_A (void);
		void test_FSBytes_operator_B (void);

		// Commented code left to illustrate a test based on exception catching (for further developpements)
		// void exceptionTest (void);

	private:
};

#endif
