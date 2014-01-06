#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TestResult.h>

#include "test.h"
#include <unistd.h>
#include <iterator>
#include <stdio.h>
#include "Cassiopee.h"



// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( CassiopeeTest );



void CassiopeeTest::setUp()
{
}


void CassiopeeTest::tearDown()
{
}


void CassiopeeTest::testIndex()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  list<long> matches = searcher->search("ggc");
	  matches.sort();
	  if(matches.size()!=3) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  std::list<long>::iterator it = matches.begin();
	  if(*it != 16) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if(*it != 19) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if(*it != 42) {
		  CPPUNIT_FAIL( "wrong position" );
	  }

	  delete searcher;
	  delete indexer;
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
