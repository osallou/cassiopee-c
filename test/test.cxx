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
	  searcher->search("ggc");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()!=3) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  std::list<Match*>::iterator it = matches.begin();
	  if((*it)->pos != 16) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if((*it)->pos != 19) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if((*it)->pos != 42) {
		  CPPUNIT_FAIL( "wrong position" );
	  }

	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testIndexWithReduction()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->search("ggc");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()!=3) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  std::list<Match*>::iterator it = matches.begin();
	  if((*it)->pos != 16) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if((*it)->pos != 19) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if((*it)->pos != 42) {
		  CPPUNIT_FAIL( "wrong position" );
	  }

	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testSearchWithError()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->max_subst = 1;
	  searcher->search("ggc");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;



	  std::list<Match*>::iterator it = matches.begin();
	  if((*it)->pos != 15) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
	  if((*it)->pos != 16) {
		  CPPUNIT_FAIL( "wrong position" );
	  }


	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testAmbiguity()
{
	char a = 'c';
	char b = 'g';
	if(Ambiguous::isequal(a,b)) {
	CPPUNIT_FAIL( "wrong equal matching");
	}
	b = 'b'; // c or g or t
	if(! Ambiguous::isequal(a,b)) {
		CPPUNIT_FAIL( "wrong ambiguous matching on b");
	}
	a = 'b';
	b = 'g';
	if(! Ambiguous::isequal(a,b)) {
		CPPUNIT_FAIL( "wrong ambiguous matching on b");
	}
	a = 'a';
	b = 'n';
	if(! Ambiguous::isequal(a,b)) {
		CPPUNIT_FAIL( "wrong ambiguous matching on n");
	}
	a = 'a';
	b = 'r';
	if(! Ambiguous::isequal(a,b)) {
		CPPUNIT_FAIL( "wrong ambiguous matching on r");
	}
	a = 'g';
	b = 'r';
	if(! Ambiguous::isequal(a,b)) {
		CPPUNIT_FAIL( "wrong ambiguous matching on r");
	}
	a = 't';
	b = 'r';
	if(Ambiguous::isequal(a,b)) {
		CPPUNIT_FAIL( "wrong ambiguous matching on r");
	}

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
