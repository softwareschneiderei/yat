// file: cppunit_YAT.cpp

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>

int main (int argc, char* argv[])
{
	// informs test-listener about testresults
	CPPUNIT_NS :: TestResult testresult;

	// register listener for collecting the test-results
	CPPUNIT_NS :: TestResultCollector collectedresults;
	testresult.addListener (&collectedresults);

	// register listener for per-test progress output
	CPPUNIT_NS :: BriefTestProgressListener progress;
	testresult.addListener (&progress);

	// insert test-suite at test-runner by registry
	CPPUNIT_NS :: TestRunner testrunner;
	testrunner.addTest (CPPUNIT_NS :: TestFactoryRegistry :: getRegistry ().makeTest ());
	testrunner.run (testresult);

	// output results in compiler-format
	CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults, std::cerr);
	compileroutputter.write ();
	
	// ELD_DEBUG Si assez de temps, voir l'utilisation du plugin pour XML
	// // Output XML for Jenkins CPPunit plugin
	// std::ofstream xmlFileOut("cppunit_Essai_02_Results.xml");
	// XmlOutputter xmlOut(&collectedresults, xmlFileOut);
	// xmlOut.write();

	// return 0 if tests were successful
	return collectedresults.wasSuccessful () ? 0 : 1;
}
