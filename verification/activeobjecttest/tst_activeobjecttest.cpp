#include <QString>
#include <QtTest>

#include "activeobject.hh"
#include <chrono>


/*!
 * \brief The TestObject class
 * Stub subclass to test the ActiveObjectTest base class functions.
 */
class TestObject : public PPUtils::ActiveObject
{
public:
    TestObject() : PPUtils::ActiveObject(), actionCalled_(false) {}
    virtual ~TestObject() {}

    bool actionCalled() const {return actionCalled_;}

    void reset() {actionCalled_ = false;}

protected:
    virtual void action() {actionCalled_ = true;}

private:
    bool actionCalled_;
};



/*!
 * \brief The ActiveObjectTest class
 *  The tester class.
 */
class ActiveObjectTest : public QObject
{
    Q_OBJECT

public:
    ActiveObjectTest();


private Q_SLOTS:

    /*!
     * \brief Test ActiveObject constructor:
     *  - Act: call default constructor.
     *  - Expected behaviour:
     *      constructed object is not started.
     */
    void constructorTest();

    /*!
     * \brief Test the start method.
     *  - Act: Create TestObject and call start(). Wait 100 ms, and call stop().
     *  - Expected behaviour:
     *      * isStarted returns true after calling start().
     *      * isStarted returns false after calling stop().
     *      * TestObject's action has been called at least once by time of stoping.
     */
    void startTest();

    /*!
     * \brief Test calling the start method multiple times (twice).
     *  - Act: Create TestObject and call start(). Wait 100 ms, and call stop().
     *         Repeat. Reset TestObject between repetitions.
     * - Expected behaviour:
     *      * isStarted returns true after calling start().
     *      * isStarted returns false after calling stop().
     *      * TestObject's action has been called at least once by time of stoping.
     *      * Behaves the same way on each repetition.
     */
    void restartTest();
};


ActiveObjectTest::ActiveObjectTest()
{
}


void ActiveObjectTest::constructorTest()
{
    TestObject test;
    QVERIFY2 (!test.isStarted(),
              "Constructed object is started at construction");
}


void ActiveObjectTest::startTest()
{
    TestObject test;
    test.start();
    QVERIFY( test.isStarted() );

    // Wait for a while (it takes some time for a thread to start).
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() - start < std::chrono::milliseconds(100) );

    test.stop();
    QVERIFY( !test.isStarted() );
    QVERIFY( test.actionCalled() );
}


void ActiveObjectTest::restartTest()
{
    TestObject test;
    // First start
    test.start();
    QVERIFY( test.isStarted() );

    // Wait for a while (it takes some time for a thread to start).
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() - start < std::chrono::milliseconds(100) );

    test.stop();
    QVERIFY( !test.isStarted() );
    QVERIFY( test.actionCalled() );

    // Restart
    test.reset();
    test.start();
    QVERIFY( test.isStarted() );

    // Wait for a while (it takes some time for a thread to start).
    start = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() - start < std::chrono::milliseconds(100) );

    test.stop();
    QVERIFY( !test.isStarted() );
    QVERIFY( test.actionCalled() );

}


QTEST_APPLESS_MAIN(ActiveObjectTest)

#include "tst_activeobjecttest.moc"
