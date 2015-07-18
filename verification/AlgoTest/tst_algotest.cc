#include <QString>
#include <QtTest>
#include <forward_list>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>

#include "algo.hh"

using IntPtr = std::unique_ptr<int>;
using IntPtrRef = const std::unique_ptr<int>&;
Q_DECLARE_METATYPE(std::forward_list<int>)
Q_DECLARE_METATYPE(std::function<bool(int,int)>)
Q_DECLARE_METATYPE(std::function<bool(IntPtrRef,IntPtrRef)>)

class AlgoTest : public QObject
{
    Q_OBJECT
    
public:
    AlgoTest();
    
private Q_SLOTS:
    
    // Test mergeSort-algorithm with copy-constructable objects (int).
    // and std::forward_list.
    void mergeSortTest_copyable();
    void mergeSortTest_copyable_data();
    
    // Test mergeSort-algorithm with non-copyable objects (std::unique_ptr<int>)
    // and std::vector.
    void mergeSortTest_not_copyable();
    void mergeSortTest_not_copyable_data();
};

AlgoTest::AlgoTest()
{
}

void AlgoTest::mergeSortTest_copyable()
{
    QFETCH(std::forward_list<int>, ctnr);
    QFETCH(std::function<bool(int,int)>, cmp);
    
    PPUtils::mergeSort(ctnr.begin(), ctnr.end(), cmp);
    
    auto it1 = ctnr.begin();
    auto it2 = ctnr.begin();
    if (it2 != ctnr.end()) ++it2;
    
    while (it2 != ctnr.end() && it1 != ctnr.end()){
        QVERIFY2( cmp(*it1, *it2) || !cmp(*it2, *it1),
                  "Elements are in wrong order after mergeSort.");
        ++it1; ++it2;
    }
}


void AlgoTest::mergeSortTest_copyable_data()
{
    using CMP = std::function<bool(int,int)>;
    QTest::addColumn<std::forward_list<int> >("ctnr");
    QTest::addColumn<CMP> ("cmp");
    
    std::forward_list<int> ctnr;
    
    QTest::newRow("0 ints, std::less") << ctnr << CMP(std::less<int>());
    
    std::vector<int> aux;
    for (int i=0; i<1000; ++i){
        aux.push_back(i);
    }
    std::shuffle(aux.begin(), aux.end(), std::default_random_engine());
    
    for (int i : aux){
        ctnr.push_front(i);
    }
    
    QTest::newRow("1000 ints, std::less") << ctnr << CMP(std::less<int>());
    QTest::newRow("1000 ints, std::greater") << ctnr << CMP(std::greater<int>());
    
    QTest::newRow("1000 ints, lambda") << ctnr
                                       << CMP([](int a, int b)
    {
        if (a%2==0 && b%2!=0) return true;
        else if (a%2!=0 && b%2==0) return false;
        else return a<b;
    });
    
    std::fill(ctnr.begin(), ctnr.end(), 0);
    QTest::newRow("equal ints, std::less") << ctnr << CMP(std::less<int>());
}


void AlgoTest::mergeSortTest_not_copyable()
{
    QFETCH (std::function<bool(IntPtrRef,IntPtrRef)>, cmp);
    
    // Generate test data.
    std::vector<IntPtr> ctnr;
    for (int i=0; i<1000; ++i){
        ctnr.push_back( std::move( IntPtr(new int(i)) ) );
    }
    std::shuffle(ctnr.begin(), ctnr.end(), std::default_random_engine());
    
    // Sort and verify
    PPUtils::mergeSort(ctnr.begin(), ctnr.end(), cmp);
    auto it1(ctnr.begin()), it2(++ctnr.begin());
    while (it2 != ctnr.end() && it1 != ctnr.end() ){
        if ( !(cmp(*it1, *it2) || !cmp(*it2, *it1)) )
        {
            QFAIL("Elements are in a wrong order after mergeSort.");
        }
        ++it1; ++it2;
    }
}

void AlgoTest::mergeSortTest_not_copyable_data()
{
    using CMP = std::function<bool(IntPtrRef,IntPtrRef)>;
    QTest::addColumn<CMP>("cmp");
    
    QTest::newRow("std::less") << CMP(std::less<IntPtrRef>());
    QTest::newRow("std::greater") << CMP(std::greater<IntPtrRef>());
    QTest::newRow("std::less_equal") << CMP(std::less_equal<IntPtrRef>());
    QTest::newRow("lambda") << CMP([](IntPtrRef a, IntPtrRef b){return *a<*b;});
}



QTEST_APPLESS_MAIN(AlgoTest)

#include "tst_algotest.moc"
