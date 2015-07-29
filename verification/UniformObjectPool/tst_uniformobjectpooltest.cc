#include <QString>
#include <QtTest>
#include <functional>
#include <type_traits>
#include "uniformobjectpool.hh"


Q_DECLARE_METATYPE(std::function<int*()>)


// Some stub classes to test ability to store subclasses of abstract base class

// Abstract base class.
class MyBaseClass
{
public:
    MyBaseClass() = default;
    virtual ~MyBaseClass() = default;
    virtual int foo() = 0;
};

// Concrete subclass.
class MySubClass : public MyBaseClass
{
    int i_;
public:
    explicit MySubClass(int i) : MyBaseClass(), i_(i) {}
    MySubClass(const MySubClass&) = delete;
    MySubClass& operator = (const MySubClass &) = delete;
    virtual int foo(){return i_;}
};

// Builder class for MyBaseClass subclasses.
class MyBuilder
{
    int i_;
public:
    MyBuilder(int i) : i_(i) {}
    MyBaseClass* operator()(){return new MySubClass(i_);}
};   


// Qt-test unit test class. 
class UniformObjectPoolTest : public QObject
{
    Q_OBJECT
    
public:
    UniformObjectPoolTest();
    
private Q_SLOTS:
    
    // Test the pointer version of constructor.
    void pointerConstructorTest();
    void pointerConstructorTest_data();
    
    // Test the reference version of constructor.
    void referenceConstructorTest();
    void referenceConstructorTest_data();
    
    // Test storing non-copyable object inherited from an abstract class.
    void noCopyableObjectTest();
    
};

UniformObjectPoolTest::UniformObjectPoolTest()
{
}


void UniformObjectPoolTest::pointerConstructorTest()
{
    QFETCH(std::function<int*()>, builder);
    PPUtils::UniformObjectPool<int, std::function<int*()>> pool(&builder);
    QCOMPARE(pool.size(), 0u);
    
    // create 10 ints using the pool.
    std::vector<int*> addresses;
    std::vector<std::unique_ptr<int>> u_pointers;
    for (int i=0; i<10; ++i){
        std::unique_ptr<int> u_ptr = pool.reserve();
        addresses.push_back(u_ptr.get());
        u_pointers.push_back( std::move(u_ptr) );
        QCOMPARE(pool.size(), 0u);
    }
    // Release reserved ints
    for (int i=0; i<10; ++i){
        std::unique_ptr<int> u_ptr(nullptr);
        u_ptr.swap( u_pointers.back() );
        u_pointers.pop_back();
        pool.release( std::move(u_ptr) );
        QCOMPARE(pool.size(), unsigned(i+1));
    }
    
    // Re-reserve created ints.
    for (int i=0; i<10; ++i){
        u_pointers.push_back( std::move(pool.reserve()) );
    }
    QCOMPARE(pool.size(), 0u);
    
    // Verify that recently reserved ints are exact same objects as earlier.
    for (const std::unique_ptr<int>& u_ptr : u_pointers){
        QVERIFY2 (std::find(addresses.begin(), addresses.end(), u_ptr.get())
                            != addresses.end(),
                  "Object address has changed.");
    }
}


void UniformObjectPoolTest::pointerConstructorTest_data()
{
    using Builder = std::function<int*()>;
    QTest::addColumn<Builder>("builder");
    
    QTest::newRow("init 1") << Builder( [](){return new int(1);} );
}


void UniformObjectPoolTest::referenceConstructorTest()
{
    QFETCH(std::function<int*()>, builder);
    PPUtils::UniformObjectPool<int, std::function<int*()>> pool(builder);
    QCOMPARE(pool.size(), 0u);
    
    // create 10 ints using the pool.
    std::vector<int*> addresses;
    std::vector<std::unique_ptr<int>> u_pointers;
    for (int i=0; i<10; ++i){
        std::unique_ptr<int> u_ptr = pool.reserve();
        addresses.push_back(u_ptr.get());
        u_pointers.push_back( std::move(u_ptr) );
        QCOMPARE(pool.size(), 0u);
    }
    // Release reserved ints
    for (int i=0; i<10; ++i){
        std::unique_ptr<int> u_ptr(nullptr);
        u_ptr.swap( u_pointers.back() );
        u_pointers.pop_back();
        pool.release( std::move(u_ptr) );
        QCOMPARE(pool.size(), unsigned(i+1));
    }
    
    // Re-reserve created ints.
    for (int i=0; i<10; ++i){
        u_pointers.push_back( std::move(pool.reserve()) );
    }
    QCOMPARE(pool.size(), 0u);
    
    // Verify that recently reserved ints are exact same objects as earlier.
    for (const std::unique_ptr<int>& u_ptr : u_pointers){
        QVERIFY2 (std::find(addresses.begin(), addresses.end(), u_ptr.get())
                            != addresses.end(),
                  "Object address has changed.");
    }
}


void UniformObjectPoolTest::referenceConstructorTest_data()
{
    using PPUtils::UniformObjectPool;
    using Builder = std::function<int*()>;
    QTest::addColumn<Builder>("builder");
    
    QTest::newRow("custom builder") << Builder([](){return new int(1);});
}


void UniformObjectPoolTest::noCopyableObjectTest()
{
    QVERIFY(!std::is_copy_constructible<MySubClass>::value);
    QVERIFY(!std::is_copy_assignable<MySubClass>::value);
    QVERIFY(!std::is_default_constructible<MySubClass>::value);
    
    // Reference constructor
    {
        MyBuilder b(1);
        PPUtils::UniformObjectPool<MyBaseClass, MyBuilder> pool(b);
        QVERIFY(pool.reserve() != nullptr);
        QCOMPARE(pool.size(), 0u);
    }
    
    // Pointer constructor
    {
        // Create pool
        MyBuilder b(2);
        PPUtils::UniformObjectPool<MyBaseClass, MyBuilder> pool(&b);
        QVERIFY(pool.reserve() != nullptr);
        QCOMPARE(pool.size(), 0u);
        
        // Test reserve, release, size and clear.
        std::unique_ptr<MyBaseClass> u_ptr = pool.reserve();
        QCOMPARE(u_ptr->foo(), 2);
        QCOMPARE(pool.size(), 0u);
        pool.release( std::move(u_ptr) );
        QCOMPARE(pool.size(), 1u);
        pool.clear();
        QCOMPARE(pool.size(), 0u);
    }
}

QTEST_APPLESS_MAIN(UniformObjectPoolTest)

#include "tst_uniformobjectpooltest.moc"
