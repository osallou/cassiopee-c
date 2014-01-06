#include <cppunit/extensions/HelperMacros.h>

class CassiopeeTest : public CppUnit::TestFixture
{

  CPPUNIT_TEST_SUITE( CassiopeeTest );
  CPPUNIT_TEST( testIndex );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testIndex();
};
