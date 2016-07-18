#include <QString>
#include <QtTest>
#include <map>
#include "concurrentpriorityqueue.hh"
#include "concurrentstresstest.hh"
#include <future>

struct MyStruct
{
    int priority;
    int order;

    MyStruct() : priority(-1), order(-1) {}
    MyStruct(int pri, int ord) : priority(pri), order(ord) {}
};

struct MyComparator
{
    bool operator()(const MyStruct& a, const MyStruct& b) const
    {
        return a.priority < b.priority;
    }
};

Q_DECLARE_METATYPE(std::vector<MyStruct>)
Q_DECLARE_METATYPE(MyComparator)


/**
 * @brief Unit tests for the ConcurrentPriorityQueue class template.
 */
class ConcurrentPriorityQueueTest : public QObject
{
    Q_OBJECT

public:
    ConcurrentPriorityQueueTest();

private Q_SLOTS:

    /**
     * @brief Test queue constructor.
     */
    void constructorTest();

    /**
     * @brief Test queue serially using default comparator (std::less).
     */
    void serialTestDefaultComparator();

    /**
     * @brief Test queue serially using custom element type and comparator.
     */
    void serialTestCustomComparator();
    void serialTestCustomComparator_data();

    /**
     * @brief Test queue using non-copyable element type.
     */
    void serialNotCopyable();

    /**
     * @brief Test first inserting items parallely. Then pop elements serially.
     */
    void parallelInsertSerialPopTest();

    /**
     * @brief Test inserting and popping elements from queue at the same time.
     */
    void parallelInsertParallelPopTest();
};

ConcurrentPriorityQueueTest::ConcurrentPriorityQueueTest()
{
}


void ConcurrentPriorityQueueTest::constructorTest()
{
    PPUtils::ConcurrentPriorityQueue<int> queue;
    int result = 10;
    bool success = queue.pop(result);
    QVERIFY(!success);
    QCOMPARE(result, 10);
}


void ConcurrentPriorityQueueTest::serialTestDefaultComparator()
{
    std::unique_ptr<PPUtils::ConcurrentPriorityQueue<int>> queue;
    queue.reset(new PPUtils::ConcurrentPriorityQueue<int>(std::less<int>()));
    std::vector<int> data = {1,3,5,7,9,2,4,6,8,0};
    std::vector<int> results;

    for (int i : data){
        queue->insert(i);
    }
    for (unsigned i=0; i<data.size(); ++i){
        int res;
        QVERIFY(queue->pop(res, 0));
        results.push_back(res);
    }
    int tmp;
    QVERIFY(!queue->pop(tmp));

    QCOMPARE(results.size(), data.size());
    for (unsigned i=1; i<results.size(); ++i){
        QVERIFY(results.at(i) < results.at(i-1));
    }
}


void ConcurrentPriorityQueueTest::serialTestCustomComparator()
{
    QFETCH(MyComparator, cmp);
    QFETCH(std::vector<MyStruct>, data);

    std::unique_ptr<PPUtils::ConcurrentPriorityQueue<MyStruct>> queue;
    queue.reset(new PPUtils::ConcurrentPriorityQueue<MyStruct>(cmp));

    for (MyStruct ms : data){
        queue->insert(ms);
    }

    std::vector<MyStruct> results;
    for (unsigned i=0; i<data.size(); ++i){
        MyStruct res;
        QVERIFY(queue->pop(res));
        results.push_back(res);
    }

    MyStruct tmp;
    QVERIFY(!queue->pop(tmp));
    QCOMPARE(tmp.priority, -1);
    QCOMPARE(tmp.order, -1);
    QCOMPARE(data.size(), results.size());

    for (unsigned i=1; i<results.size(); ++i){
        QVERIFY ( !MyComparator().operator()(results.at(i-1), results.at(i)) );
        if (results.at(i).priority == results.at(i-1).priority){
            QVERIFY(results.at(i-1).order < results.at(i).order);
        }
    }
}


void ConcurrentPriorityQueueTest::serialTestCustomComparator_data()
{
    QTest::addColumn<MyComparator>("cmp");
    QTest::addColumn< std::vector<MyStruct> >("data");

    QTest::newRow("case1")
            << MyComparator()
            << std::vector<MyStruct>
                {
                    MyStruct(0,0), MyStruct(1,0), MyStruct(2,0),
                    MyStruct(0,1), MyStruct(1,1), MyStruct(2,1),
                    MyStruct(0,2), MyStruct(1,2), MyStruct(2,2)
                };
}


void ConcurrentPriorityQueueTest::serialNotCopyable()
{
    auto cmp = [](const std::unique_ptr<int>& a, const std::unique_ptr<int>& b) {return *a > *b;};


    PPUtils::ConcurrentPriorityQueue<std::unique_ptr<int>> q(cmp);
    for (int i=0; i<10; ++i){
        std::unique_ptr<int> item(new int(i));
        q.insert(std::move(item));
    }

    std::vector<std::unique_ptr<int>> results;
    for (int i=0; i<10; ++i){
        std::unique_ptr<int> res;
        QVERIFY(q.pop(res));
        results.push_back( std::move(res) );
    }

    std::unique_ptr<int> tmp(nullptr);
    QVERIFY(!q.pop(tmp));
    QVERIFY(tmp == nullptr);
    QCOMPARE(results.size(), 10u);

    for (unsigned i=1; i<results.size(); ++i){
        QVERIFY((*results.at(i) > *results.at(i-1)));
    }
}


void populateQueue(PPUtils::ConcurrentPriorityQueue<int>& queue,
                   std::vector<int> elements)
{
    for (int i : elements){
        queue.insert(i);
    }
}


void ConcurrentPriorityQueueTest::parallelInsertSerialPopTest()
{
    PPUtils::ConcurrentPriorityQueue<int> q;
    std::vector<int> nums {1,2,3,4,5,6,7,8,9,0};

    PPTest::ConcurrentStressTest<10> tester(&populateQueue,
                                            std::reference_wrapper<PPUtils::ConcurrentPriorityQueue<int>>(q),
                                            nums);
    tester.startTest();

    std::vector<int> elements;
    int tmp;
    while (q.pop(tmp)){
        elements.push_back(tmp);
    }

    QCOMPARE(elements.size(), std::vector<int>::size_type(100));
    for (unsigned i=1; i<elements.size(); ++i){
        QVERIFY(elements.at(i) <= elements.at(i-1));
    }
}


void unpopulate(PPUtils::ConcurrentPriorityQueue<int>& q,
                std::vector<int>& output, int numOfElements)
{
    while (numOfElements > 0){
        int tmp;
        while (!q.pop(tmp, 100))
            ;
        output.push_back(tmp);
        --numOfElements;
    }
}


void ConcurrentPriorityQueueTest::parallelInsertParallelPopTest()
{
    PPUtils::ConcurrentPriorityQueue<int> q;
    std::vector<int> inputs {1,2,3,4,5,6,7,8,9,0};

    PPTest::ConcurrentStressTest<10> producer(&populateQueue,
                                              std::reference_wrapper<PPUtils::ConcurrentPriorityQueue<int>>(q),
                                              inputs);

    std::vector<int> outputs;
    std::future<void> f = std::async(&unpopulate,
                                     std::reference_wrapper<PPUtils::ConcurrentPriorityQueue<int>>(q),
                                     std::reference_wrapper<std::vector<int>>(outputs), 100);

    producer.startTest();
    f.get();

    std::vector<int> nums(10, 0);
    QCOMPARE(outputs.size(), 100u);
    for (int i : outputs){
        ++nums[i];
    }

    for (unsigned i=0; i<nums.size(); ++i){
        QCOMPARE(nums.at(i), 10);
    }
}



QTEST_APPLESS_MAIN(ConcurrentPriorityQueueTest)

#include "tst_concurrentpriorityqueuetest.moc"
