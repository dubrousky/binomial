//
// Created by Aliaksandr Dubrouski on 6/10/15.
//

#include <cppunit/extensions/HelperMacros.h>
#include <binomial.h>
#include <binomials.h>

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
        binomials<int,calculate::seq_multiply> bb0;
        binomials<int,calculate::seq_factorize> bb1;
        binomials<int,calculate::async_multiply> bb2;
        binomials<int,calculate::async_factorize> bb3;

        for(int i=1; i<13; ++i ) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",1,bb0(i,0));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",1,bb1(i,0));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",1,bb2(i,0));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",1,bb3(i,0));
        }
    }

    void testCn1() {
        binomials<int,calculate::seq_multiply> bb0;
        binomials<int,calculate::seq_factorize> bb1;
        binomials<int,calculate::async_multiply> bb2;
        binomials<int,calculate::async_factorize> bb3;
        for(int i=1; i<13; ++i ) {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",i,bb0(i,1));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",i,bb1(i,1));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",i,bb2(i,1));
            CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,0) must be 1",i,bb3(i,1));
        }
    }

    void testCnkCnnk() {
        binomials<int,calculate::seq_multiply> bb0;
        binomials<int,calculate::seq_factorize> bb1;
        binomials<int,calculate::async_multiply> bb2;
        binomials<int,calculate::async_factorize> bb3;
        for(int i=1; i<13; ++i ) {
            for(int j=1; j<i; ++j) {
                CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,k) must be equal to C(n,n-k)", bb0(i,j), bb0(i, i-j));
                CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,k) must be equal to C(n,n-k)", bb1(i,j), bb1(i, i-j));
                CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,k) must be equal to C(n,n-k)", bb2(i,j), bb2(i, i-j));
                CPPUNIT_ASSERT_EQUAL_MESSAGE("C(n,k) must be equal to C(n,n-k)", bb3(i,j), bb3(i, i-j));
            }
        }
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