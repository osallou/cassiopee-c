#include <cppunit/extensions/HelperMacros.h>

class CassiopeeTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( CassiopeeTest );
  CPPUNIT_TEST( testIndex );
  CPPUNIT_TEST( testIndexWithReduction );
  CPPUNIT_TEST( testSearchInReduction );
  CPPUNIT_TEST( testAmbiguity );
  CPPUNIT_TEST( testSearchWithError );
  CPPUNIT_TEST( testSearchAfterN );
  CPPUNIT_TEST( testSearchWithN );
  CPPUNIT_TEST( testReducedSearchAfterN );
  CPPUNIT_TEST( testReducedSearchWithN );
  CPPUNIT_TEST( testProtein );
  CPPUNIT_TEST( testLoadSave );
  CPPUNIT_TEST( testMultiIndex );
  CPPUNIT_TEST( testSearchWithMorphism );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testIndex();
  void testIndexWithReduction();
  void testSearchInReduction();
  void testAmbiguity();
  void testSearchWithError();
  void testSearchWithMorphism();
  void testSearchAfterN();
  void testSearchWithN();
  void testReducedSearchAfterN();
  void testReducedSearchWithN();
  void testProtein();
  void testLoadSave();
  void testMultiIndex();

};
