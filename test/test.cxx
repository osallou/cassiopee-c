#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TestResult.h>

#include "test.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CassiopeeTest );


void CassiopeeTest::setUp()
{
}


void CassiopeeTest::tearDown()
{
}


void CassiopeeTest::testExample()
{
  CPPUNIT_FAIL( "not implemented" );
}


int main(int argc, char* argv[])
{
  CppUnit::TestResultCollector result;
  CppUnit::TestResult controller;

  controller.addListener( &result );

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  // Run the tests.
  runner.run(controller);

  // Print test in a compiler compatible format.
  CppUnit::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write();
 
  // Uncomment this for XML output
  std::ofstream file( "cppunit-report.xml" );
  CppUnit::XmlOutputter xml( &result, file );
  xml.write();
  file.close();

  // Return error code 1 if the one of test failed.
  return result.wasSuccessful() ? 0 : 1;
}
