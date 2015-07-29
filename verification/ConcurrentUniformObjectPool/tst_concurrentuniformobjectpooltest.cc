#include <QString>
#include <QtTest>

class ConcurrentUniformObjectPoolTest : public QObject
{
    Q_OBJECT
    
public:
    ConcurrentUniformObjectPoolTest();
    
private Q_SLOTS:
    void testCase1();
};

ConcurrentUniformObjectPoolTest::ConcurrentUniformObjectPoolTest()
{
}

void ConcurrentUniformObjectPoolTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(ConcurrentUniformObjectPoolTest)

#include "tst_concurrentuniformobjectpooltest.moc"
