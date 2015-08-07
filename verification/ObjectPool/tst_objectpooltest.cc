#include <QString>
#include <QtTest>

#include "objectpool.hh"

// Simple class hierarchy for testing:
// ----------------------------------------------------------------------------
class Base
{
public:
    virtual QString name() const = 0;
};

class DerivA : public Base
{
public:
    virtual QString name() const {return QString("DerivA");}
};

class DerivB : public Base
{
public:
    virtual QString name() const{return QString("DerivB");}
};

class BaseFactory
{
public:
    
    static Base* create(const QString& name){
        if (name == "DerivA"){
            return new DerivA();
        }
        else if (name == "DerivB"){
            return new DerivB();
        }
        return nullptr;
    }
    
    static Base* clone(Base* other){
        if (typeid(*other) == typeid(DerivA)){
            return new DerivA();
        }
        else if (typeid(*other) == typeid(DerivB)){
            return new DerivB();
        }
        return nullptr;
    }
};

class BaseFactoryWrapper
{
    BaseFactory f_;
    int id_;
public:
    BaseFactoryWrapper(int id = 0): id_(id) {}
    
    Base* operator()(const QString& name){
        return f_.create(name);
    }
    Base* operator()(Base* other){
        return f_.clone(other);
    }
    
    int getID() const {return id_;}
};

class Selector
{
    int id_;
    
public:
    Selector(int id=0): id_(id) {}
    
    const char* operator()(Base* b){
        if (typeid(*b) == typeid(DerivA)){
            return "DerivA";
        }
        else if (typeid(*b) == typeid(DerivB)){
            return "DerivB";
        }
        return "";
    }
    const char* operator()(const QString& name){
        if (name == "DerivA"){
            return "DerivA";
        }
        else if (name == "DerivB"){
            return "DerivB";
        }
        return "";
    }
    int getID() const {return id_;}
};

//-----------------------------------------------------------------------------


class ObjectPoolTest : public QObject
{
    Q_OBJECT
    
public:
    ObjectPoolTest();
    
private Q_SLOTS:
    
    // Test the pointer constructor.
    void constructorTest1();
    
    // Test the reference constructor.
    void constructorTest2();
    
    // Test releasing objects.
    void releaseTest();
    
    // Test reserving objects
    void reserveTest();
    
    // Test move-constructor
    void moveConstructorTest();
    
    void moveAssignmentTest();
};

ObjectPoolTest::ObjectPoolTest()
{
}


void ObjectPoolTest::constructorTest1()
{
    // Constructor with class selector and builder.
    {
        typedef PPUtils::ObjectPool<Base, BaseFactoryWrapper, const char*, Selector> BasePool;
        BaseFactoryWrapper* builder = new BaseFactoryWrapper();
        Selector* selector = new Selector();
        BasePool pool(builder, selector);
        QVERIFY2(pool.getBuilder() == builder, "Builder does not match.");
        QVERIFY2(pool.getSelector() == selector, "Selector does not match.");
        QCOMPARE(pool.size(), 0u);
        QCOMPARE(pool.size("DerivA"), 0u);
        QCOMPARE(pool.size("DerivB"), 0u);
        QCOMPARE(pool.size("Unknown"), 0u);
    }
    
    // Constructor with function wrappers.
    {
        typedef std::function<Base*(QString)> BuilderFn;
        typedef std::function<const char*(QString)> SelectorFn;
        typedef PPUtils::ObjectPool<Base, BuilderFn,const char*, SelectorFn> BasePool;
        BuilderFn* builder = new BuilderFn(&BaseFactory::create);
        SelectorFn* selector = new SelectorFn( [](QString s){return s.toStdString().c_str();} );
        BasePool pool(builder, selector);
        QVERIFY2(pool.getBuilder() == builder, "Builder does not match.");
        QVERIFY2(pool.getSelector() == selector, "Selector does not match.");
        QCOMPARE(pool.size(), 0u);
        QCOMPARE(pool.size("DerivA"), 0u);
        QCOMPARE(pool.size("DerivB"), 0u);
        QCOMPARE(pool.size("Unknown"), 0u);
    }
}


void ObjectPoolTest::constructorTest2()
{
    // Constructor with class selector and builder.
    {
        typedef PPUtils::ObjectPool<Base, BaseFactoryWrapper, const char*, Selector> BasePool;
        BaseFactoryWrapper builder(1);
        Selector selector(2);
        BasePool pool(builder, selector);
        QVERIFY2(pool.getBuilder()->getID() == builder.getID(), 
                 "Builder does not match.");
        QVERIFY2(pool.getSelector()->getID() == selector.getID(), 
                 "Selector does not match.");
        QCOMPARE(pool.size(), 0u);
        QCOMPARE(pool.size("DerivA"), 0u);
        QCOMPARE(pool.size("DerivB"), 0u);
        QCOMPARE(pool.size("Unknown"), 0u);
    }
    
    // Constructor with function wrappers.
    {
        typedef std::function<Base*(QString)> BuilderFn;
        typedef std::function<const char*(QString)> SelectorFn;
        typedef PPUtils::ObjectPool<Base, BuilderFn,const char*, SelectorFn> BasePool;
        BuilderFn builder(&BaseFactory::create);
        SelectorFn selector( [](QString s){return s.toStdString().c_str();} );
        BasePool pool(builder, selector);
        QCOMPARE(pool.size(), 0u);
        QCOMPARE(pool.size("DerivA"), 0u);
        QCOMPARE(pool.size("DerivB"), 0u);
        QCOMPARE(pool.size("Unknown"), 0u);
    }
}


void ObjectPoolTest::releaseTest()
{
    typedef PPUtils::ObjectPool<Base, BaseFactoryWrapper, const char*, Selector> BasePool;
    BaseFactoryWrapper* builder = new BaseFactoryWrapper();
    Selector* selector = new Selector();
    BasePool pool(builder, selector);
    
    for (unsigned int i=1; i<=10; ++i){
        std::unique_ptr<Base> a(new DerivA());
        std::unique_ptr<Base> b(new DerivB());
        
        pool.release( std::move(a) );
        QCOMPARE(pool.size("DerivA"), i);
        QCOMPARE(pool.size("DerivB"), i-1);
        QCOMPARE(pool.size(), 2*i-1);
        
        pool.release( std::move(b) );
        QCOMPARE(pool.size("DerivA"), i);
        QCOMPARE(pool.size("DerivB"), i);
        QCOMPARE(pool.size(), 2*i);
    }
    
    pool.clear();
    QCOMPARE(pool.size("DerivA"), 0u);
    QCOMPARE(pool.size("DerivB"), 0u);
    QCOMPARE(pool.size(), 0u);
}


void ObjectPoolTest::reserveTest()
{
    typedef PPUtils::ObjectPool<Base, BaseFactoryWrapper, const char*, Selector> BasePool;
    BaseFactoryWrapper* builder = new BaseFactoryWrapper();
    Selector* selector = new Selector();
    BasePool pool(builder, selector);
    
    // Reserve based on key.
    std::unique_ptr<Base> a = pool.reserve(QString("DerivA"));
    QCOMPARE(a->name(), QString("DerivA"));
    QCOMPARE(pool.size(), 0u);
    
    // Reserve based on clone.
    a = pool.reserve(a.get());
    QCOMPARE(a->name(), QString("DerivA"));
    QCOMPARE(pool.size(), 0u);
    
    // Re-reserve object based on key.
    Base* p = a.get();
    pool.release( std::move(a) );
    QCOMPARE(pool.size(), 1u);
    QCOMPARE(pool.size("DerivA"), 1u);
    QCOMPARE(pool.size("DerivB"), 0u);
    a = pool.reserve("DerivA");
    QVERIFY(a.get() == p);
    QCOMPARE(pool.size(), 0u);
    
    // Re-reserve object based on clone.
    p = a.get();
    pool.release( std::move(a) );
    QCOMPARE(pool.size(), 1u);
    QCOMPARE(pool.size("DerivA"), 1u);
    QCOMPARE(pool.size("DerivB"), 0u);
    a = pool.reserve(p);
    QVERIFY(a.get() == p);
    QCOMPARE(pool.size(), 0u);
    
}


void ObjectPoolTest::moveConstructorTest()
{
    typedef PPUtils::ObjectPool<Base, BaseFactoryWrapper, const char*, Selector> BasePool;
    BaseFactoryWrapper* builder = new BaseFactoryWrapper();
    Selector* selector = new Selector();
    BasePool pool(builder, selector);
    std::vector<Base*> addresses;
    unsigned size(0), size_a(0), size_b(0);
    
    // Add some objects in original pool
    for (unsigned i=0; i<10; ++i){
        std::unique_ptr<Base> a(new DerivA());
        std::unique_ptr<Base> b(new DerivB());
        addresses.push_back(a.get());
        addresses.push_back(b.get());
        pool.release( std::move(a) );
        pool.release( std::move(b) );
        size += 2;
        ++size_a;
        ++size_b;
    }
    
    // Move-construction
    BasePool pool2( std::move(pool) );
    QVERIFY2 (pool2.getBuilder() == builder, "Builder does not match");
    QVERIFY2 (pool2.getSelector() == selector, "Selector does not match");
    QCOMPARE (pool2.size(), size);
    QCOMPARE (pool2.size("DerivA"), size_a);
    QCOMPARE (pool2.size("DerivB"), size_b);
    
    // Verify that stored objects are the same.
    while (pool2.size("DerivA") > 0){
        std::unique_ptr<Base> a(pool2.reserve("DerivA"));
        QVERIFY2(std::find(addresses.begin(), addresses.end(), a.get()) != addresses.end(),
                 "Unknown pointer of type DerivA");
    }
    while (pool2.size("DerivB") > 0){
        std::unique_ptr<Base> b(pool2.reserve("DerivB"));
        QVERIFY2(std::find(addresses.begin(), addresses.end(), b.get()) != addresses.end(),
                 "Unknown pointer of type DerivB");
    }
    QCOMPARE(pool2.size(), 0u);
}


void ObjectPoolTest::moveAssignmentTest()
{
    typedef PPUtils::ObjectPool<Base, BaseFactoryWrapper, const char*, Selector> BasePool;
    BaseFactoryWrapper* builder = new BaseFactoryWrapper();
    Selector* selector = new Selector();
    BasePool pool(builder, selector);
    std::vector<Base*> addresses;
    unsigned size(0), size_a(0), size_b(0);
    
    // Add some objects in original pool
    for (unsigned i=0; i<10; ++i){
        std::unique_ptr<Base> a(new DerivA());
        std::unique_ptr<Base> b(new DerivB());
        addresses.push_back(a.get());
        addresses.push_back(b.get());
        pool.release( std::move(a) );
        pool.release( std::move(b) );
        size += 2;
        ++size_a;
        ++size_b;
    }
    
    // Move-assignment.
    BasePool pool2;
    pool2 = std::move(pool);
    QVERIFY2 (pool2.getBuilder() == builder, "Builder does not match");
    QVERIFY2 (pool2.getSelector() == selector, "Selector does not match");
    QCOMPARE (pool2.size(), size);
    QCOMPARE (pool2.size("DerivA"), size_a);
    QCOMPARE (pool2.size("DerivB"), size_b);
    
    // Verify that stored objects are the same.
    while (pool2.size("DerivA") > 0){
        std::unique_ptr<Base> a(pool2.reserve("DerivA"));
        QVERIFY2(std::find(addresses.begin(), addresses.end(), a.get()) != addresses.end(),
                 "Unknown pointer of type DerivA");
    }
    while (pool2.size("DerivB") > 0){
        std::unique_ptr<Base> b(pool2.reserve("DerivB"));
        QVERIFY2(std::find(addresses.begin(), addresses.end(), b.get()) != addresses.end(),
                 "Unknown pointer of type DerivB");
    }
    QCOMPARE(pool2.size(), 0u);
}


QTEST_APPLESS_MAIN(ObjectPoolTest)

#include "tst_objectpooltest.moc"
