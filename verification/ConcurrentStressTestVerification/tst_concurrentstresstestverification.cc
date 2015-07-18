/* ConcurrentStressTestVerification
 * This is a (limited) unit test for the ConcurrentStressTest class template.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 18-June-2015
 */

#include <QString>
#include <QtTest>
#include <functional>

#include "concurrentstresstest.hh"


class ConcurrentStressTestTest : public QObject
{
    Q_OBJECT
    
public:
    ConcurrentStressTestTest();
    
private Q_SLOTS:
    
    // Test constructor and destructor.
    void constructorDestructorTest();
    
    // Test instance version of startTest-method.
    void startTest_instance();
    
    // Test static version of startTest-method.
    void startTest_static();
    
private:
    
    std::mutex mx_;
    void increment(int* i)
    {
        mx_.lock();
        *i = *i+1;
        mx_.unlock();
    }
    
};


std::mutex global_mx_;
void increment_fn(int& i)
{
    global_mx_.lock();
    ++i;
    global_mx_.unlock();
}


class TestFunctor
{
public:
    void operator()(int& i1, int& i2, int& i3)
    {
        global_mx_.lock();
        ++i1; ++i2; ++i3;
        global_mx_.unlock();
    }
};


ConcurrentStressTestTest::ConcurrentStressTestTest() : mx_()
{
}


void ConcurrentStressTestTest::constructorDestructorTest()
{
    int i(0);
    {
        PPTest::ConcurrentStressTest<10> tester
                (&increment_fn, std::reference_wrapper<int>(i) );
        QVERIFY(tester.isUsable());
    }
    // Tester destroyed when usable -> should not crash.
    
    PPTest::ConcurrentStressTest<100> tester
            (&increment_fn, std::reference_wrapper<int>(i) );
    QVERIFY(tester.isUsable());
    tester.startTest();
    QVERIFY(!tester.isUsable());
    // tester destroyed when not usable -> should not crash.
}


void ConcurrentStressTestTest::startTest_instance()
{
    using PPTest::ConcurrentStressTest;
    // function pointer
    int i=0;
    {
        ConcurrentStressTest<100> tester(&increment_fn, 
                                         std::reference_wrapper<int>(i));
        QVERIFY(tester.isUsable());
        tester.startTest();
        QVERIFY(!tester.isUsable());
        QCOMPARE(i, 100);
    }
    
    // member function
    i=0;
    {
        ConcurrentStressTest<100> tester
                (std::mem_fn(&ConcurrentStressTestTest::increment), this, &i);
        QVERIFY(tester.isUsable());
        tester.startTest();
        QVERIFY(!tester.isUsable());
        QCOMPARE(i,100);
    }
    
    // functor with multiple arguments.
    i=0;
    {
         int j(0), k(0);
         TestFunctor fn;
         ConcurrentStressTest<100> tester(fn, 
                                          std::reference_wrapper<int>(i),
                                          std::reference_wrapper<int>(j),
                                          std::reference_wrapper<int>(k) );
         QVERIFY(tester.isUsable());
         tester.startTest();
         QVERIFY(!tester.isUsable());
         QCOMPARE(i, 100); QCOMPARE(j, 100); QCOMPARE(k, 100);
    }
}


void ConcurrentStressTestTest::startTest_static()
{
    using PPTest::ConcurrentStressTest;
    // function pointer
    int i=0;
    {
        ConcurrentStressTest<100>::startTest(&increment_fn, 
                                             std::reference_wrapper<int>(i));
        QCOMPARE(i, 100);
    }
    
    // member function
    i=0;
    {
        ConcurrentStressTest<100>::startTest
                (std::mem_fn(&ConcurrentStressTestTest::increment), this, &i);
        QCOMPARE(i,100);
    }
    
    // functor with multiple arguments.
    i=0;
    {
         int j(0), k(0);
         TestFunctor fn;
         ConcurrentStressTest<100>::startTest(fn,
                                              std::reference_wrapper<int>(i),
                                              std::reference_wrapper<int>(j),
                                              std::reference_wrapper<int>(k) );
         QCOMPARE(i, 100); QCOMPARE(j, 100); QCOMPARE(k, 100);
    }
}

QTEST_APPLESS_MAIN(ConcurrentStressTestTest)

#include "tst_concurrentstresstestverification.moc"
