#include <cppunit/extensions/HelperMacros.h>

class CassiopeeTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( CassiopeeTest );
  CPPUNIT_TEST( testExample );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testExample();
};
