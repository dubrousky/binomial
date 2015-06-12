//
// Created by Aliaksandr Dubrouski on 6/10/15.
//
#include <cppunit/extensions/HelperMacros.h>
#include <thread_pool.h>
#include <string>
#include <unistd.h>

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
class ThreadPoolTest : public CppUnit::TestFixture {
public:


    void setUp() { }

    void tearDown() { }

    void testFutureThrow() {
        thread_pool<typename std::packaged_task<int()> > tp(5);
        std::packaged_task<int()> task([](){ throw std::string(""); return 7;});
        std::packaged_task<int()> task1([](){ throw std::string(""); return 70;});
        auto f = task.get_future();
        tp.submit(std::move(task));
        tp.submit(std::move(task1));
        CPPUNIT_ASSERT_THROW_MESSAGE("Getting failed future value must throw",
            f.get(),
            std::string
        );
    }

    void testFutureNoThrow() {
        thread_pool<typename std::packaged_task<int()> > tp(5);
        std::packaged_task<int()> task([](){ throw std::string(""); return 7;});
        std::packaged_task<int()> task1([](){ throw std::string(""); return 70;});
        auto f = task.get_future();
        tp.submit(std::move(task));
        tp.submit(std::move(task1));
        CPPUNIT_ASSERT_NO_THROW_MESSAGE("Checking failed future value must not throw",
                                     f.valid()
        );
    }

    void  testCalculate() {
        thread_pool<typename std::packaged_task<int()> > tp(5);
        int a = 7;
        int b = 70;
        std::packaged_task<int()> task([&a](){ sleep(1); return a;});
        std::packaged_task<int()> task1([&b](){ sleep(1); return b;});
        auto f = task.get_future();
        auto f1 = task1.get_future();
        tp.submit(std::move(task));
        tp.submit(std::move(task1));
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread pool must calculate the values",
                               f1.get()+f.get(), a+b );
    }

    void  testAsync() {
        thread_pool<typename std::packaged_task<int()> > tp(5);
        int a = 7;
        int b = 70;
        std::packaged_task<int()> task([&a](){ sleep(1); return a;});
        std::packaged_task<int()> task1([&b](){ sleep(3); return b;});
        auto f = async(std::move(task),tp);
        auto f1 = async(std::move(task1),tp);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Thread pool must calculate the values",
                                     f1.get()+f.get(), a+b );
    }
    // Add test cases to the test suite
    CPPUNIT_TEST_SUITE(ThreadPoolTest);
    // register your test methods here
    CPPUNIT_TEST(testFutureThrow);
    CPPUNIT_TEST(testFutureNoThrow);
    CPPUNIT_TEST(testCalculate);
        CPPUNIT_TEST(testAsync);
    CPPUNIT_TEST_SUITE_END();
};
// Globally register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(ThreadPoolTest);

