//
// Created by Aliaksandr Dubrouski on 6/10/15.
//

#include <cppunit/extensions/HelperMacros.h>
#include <binomial.h>

/**
* To test the protected and private functionality one may inherit
* directly from the tested class and test case class
*
* The methods ot the original class shall be tested in the methods
* of the test  class.
* There is a correspondence between classes/methods and
* test suites/test cases:
* * test class -> test suite
* * test method -> test case
*
* If some preparations are required before test execution - consider
* inheritance from the Fixture class.
*
*
* Test needs preparation work to be performed
* so the fixture is used.
*/
class BinomialTest : public CppUnit::TestFixture {
public:


    void setUp() { }

    void tearDown() { }

    //Property tests
    //C(n,0), C(n,1), C(n,n-1), C(n,n), C(n,k) = C(n-1,k-1) + C(n-1,k), C(n,k) == C(n,n-k)

    void testCn0() {
        binomial<int> bb;
        for(int i=1; i<13; ++i ) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",1,bb(i,0));
        }
    }

    void testCn1() {
        binomial<int> bb;
        for(int i=1; i<13; ++i ) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,1) must be n",i,bb(i,1));
        }
    }

    void testCnkCnnk() {
        binomial<int> bb;
        for(int i=1; i<13; ++i ) {
            for(int j=1; j<i; ++j) {
                CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,k) must be equal to C(n,n-k)", bb(i,j), bb(i, i-j));
            }
        }
    }

    void testTest() {

        CPPUNIT_ASSERT_MESSAGE("Module Id of current task should be unloaded",
                               false );
        CPPUNIT_FAIL("Failed loading test library");
    }



    // Add test cases to the test suite
    CPPUNIT_TEST_SUITE(BinomialTest);
    // register your test methods here
    CPPUNIT_TEST(testCn0);
    CPPUNIT_TEST(testCn1);
    CPPUNIT_TEST(testCnkCnnk);
    CPPUNIT_TEST_SUITE_END();
};
// Globally register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(BinomialTest);