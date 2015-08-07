#include <iostream>
#include "../../source/PPUtils/objectpool.hh"


// There exists an abstract base class with numerous subclasses:
//----------------------------------------------------------------------------
// Base.hh

class Base
{
public:
    virtual void hello() = 0;
    virtual int getID() const = 0;
};


class SubA : public Base
{
public:
    virtual void hello() {std::cout << "Hello from A" << std::endl;}
    virtual int getID() const {return 0;}
};


class SubB : public Base
{
public:
    virtual void hello() {std::cout << "Hello from B" << std::endl;}
    virtual int getID() const {return 1;}
};


class BaseFactory
{
public:
    static Base* getInstance(int ID){
        switch (ID) {
        case 0:
            return new SubA;
        case 1:
            return new SubB;
        default:
            return nullptr;
        }
    }
};

//-----------------------------------------------------------------------------
// BasePool.hh
// #include "Base.hh"
// #include "ObjectPool.hh"

// To use ObjectPool, we need to define selector:
class BaseSelector
{
public:
    // Selector for Base objects
    int operator () (Base* b) {return b->getID();}
    // Selector for builder argument(s).
    int operator () (int key) {return key;}
};

// We also need to wrap the factory to a callable object
typedef std::function<Base*(int)> Builder_t;
const Builder_t BUILDER(&BaseFactory::getInstance);

// It's convenient to typedef ObjectPool to more descriptive and shorted name.
typedef PPUtils::ObjectPool<Base, Builder_t, int, BaseSelector> BasePool;

// ----------------------------------------------------------------------------

// main.cc
// #include "Base.hh"
// #include "BasePool.hh"

int main()
{
    BasePool pool(new Builder_t(BUILDER), new BaseSelector );
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    std::unique_ptr<Base> b1 = pool.reserve(0);
    b1->hello();
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    std::unique_ptr<Base> b2 = pool.reserve(1);
    b2->hello();
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    pool.release( std::move(b1) );
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    pool.release( std::move(b2) );
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    b1 = pool.reserve(0);
    b1->hello();
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    pool.clear();
    std::cout << "Pool contains: " << pool.size() << " objects." << std::endl;
    
    return 0;
}

/*
 * Expected output:
 * 
 * Pool contains: 0 objects.
 * Hello from A
 * Pool contains: 0 objects.
 * Hello from B
 * Pool contains: 1 objects.
 * Pool contains: 2 objects.
 * Hello from A
 * Pool contains: 1 objects.
 * Pool contains: 0 objects.
 * 
 */

