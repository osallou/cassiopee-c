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
  remove( "test/sequence.txt.cass.idx" );
}


void CassiopeeTest::tearDown()
{
  remove( "test/sequence.txt.cass.idx" );
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
		  cerr <<  matches.size() << "\n";
		  CPPUNIT_FAIL( "wrong number of match");
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

void CassiopeeTest::testLoadSave()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->index();
	  long save_nodes = indexer->getTree()->size();
	  indexer->save();
	  CassieIndexer* indexer2 = new CassieIndexer(seq);
	  indexer2->index();
	  if(! indexer2->index_loaded_from_file()) {
		  CPPUNIT_FAIL( "index not loaded from saved index" );
	  }
	  long load_nodes = indexer->getTree()->size();
      delete indexer;
	  delete indexer2;


	  if(load_nodes != save_nodes) {
		  CPPUNIT_FAIL( "loaded tree is different from saved tree" );
	  }

}

void CassiopeeTest::testMultiIndex()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->index();
	  long save_nodes = indexer->getTree()->size();
	  indexer->save();
	  indexer->index();
	  long save_nodes2 = indexer->getTree()->size();

	  if(save_nodes != save_nodes2) {
		  CPPUNIT_FAIL( "index loaded twice" );
	  }

	  delete indexer;



}

void CassiopeeTest::testIndexWithReduction()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
      indexer->max_index_depth = 4;
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

void CassiopeeTest::testSearchInReduction()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->search("gggcgcgggtgggtgggagagaa");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()!=1) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testProtein()
{
	  char sequence[] = "test/protein.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->mode = 2;
	  searcher->search("GHIKLMNPQR");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()!=1) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testSearchWithMorphism()
{
    char sequence[] = "test/sequence.txt";
    char* seq = sequence;
    CassieIndexer* indexer = new CassieIndexer(seq);
    indexer->do_reduction = true;
    indexer->max_index_depth = 5;
    indexer->index();

    CassieSearch* searcher = new CassieSearch(indexer);
    searcher->morphisms["g"] = "a";
    searcher->search("tgtg");
    searcher->sort();
    list<Match*> matches = searcher->matches;


    std::list<Match*>::iterator it = matches.begin();
    if((*it)->pos != 71) {
        CPPUNIT_FAIL( "wrong position" );
    }
}

void CassiopeeTest::testSearchWithError()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
      indexer->max_index_depth = 4;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->max_subst = 1;
	  searcher->search("ggc");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;


	  std::list<Match*>::iterator it = matches.begin();
      // œœœœœœœœœœcout << "#### " << (*it)->pos << "\n";
	  if((*it)->pos != 15) {
		  CPPUNIT_FAIL( "wrong position" );
	  }
	  std::advance(it, 1);
      // cout << "#### " << (*it)->pos << "\n";
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


void CassiopeeTest::testSearchAfterN()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = false;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->search("tata");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()==0) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testSearchWithN()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = false;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->nmax = 1;
	  searcher->search("gtata");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()==0) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  delete searcher;
	  delete indexer;
}


void CassiopeeTest::testReducedSearchAfterN()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->search("tata");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()==0) {
		  CPPUNIT_FAIL( "wrong number of match" );
	  }

	  delete searcher;
	  delete indexer;
}

void CassiopeeTest::testReducedSearchWithN()
{
	  char sequence[] = "test/sequence.txt";
	  char* seq = sequence;
	  CassieIndexer* indexer = new CassieIndexer(seq);
	  indexer->do_reduction = true;
	  indexer->index();

	  CassieSearch* searcher = new CassieSearch(indexer);
	  searcher->nmax = 1;
	  searcher->search("gtata");
	  searcher->sort();
	  list<Match*> matches = searcher->matches;

	  if(matches.size()==0) {
		  CPPUNIT_FAIL( "wrong number of match" );
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
