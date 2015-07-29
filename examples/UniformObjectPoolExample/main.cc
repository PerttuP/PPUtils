/*
 * This is a simple example program to demonstrate the usage of the 
 * PPUtils::UniformObjectPool class template.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 29-June-2015
 */

#include <iostream>
#include <functional>

#include "../../source/PPUtils/uniformobjectpool.hh"

class VeryComplexClass
{
    static int n;
public:   
    VeryComplexClass()
    {
        // Very expensive construction
        std::cout << "Constructing a massively complex object" << std::endl;
        ++n;
    }
    
    void foo()
    {
        std::cout << "Expensive constructor was called " << n << " times\n";
    }   
};

int VeryComplexClass::n = 0;


int main()
{
    // Create the object pool
    PPUtils::UniformObjectPool<VeryComplexClass> pool;
    std::cout << "Pool contains: " << pool.size() << " objects" << std::endl;
    
    std::unique_ptr<VeryComplexClass> obj = pool.reserve();
    std::cout << "Pool contains: " << pool.size() << " objects" << std::endl;
    
    pool.release( std::move(obj) );
    std::cout << "Pool contains: " << pool.size() << " objects" << std::endl;
    
    obj = pool.reserve();
    std::cout << "Pool contains: " << pool.size() << " objects" << std::endl;
    obj->foo();
    
    return 0;
}


/*
 * Expected output:
 * 
 * Pool contains 0 objects
 * Constructing a massively complex object
 * Pool contains 0 objects
 * Pool contains 1 objects
 * Pool contains 0 objects
 * Expensive constructor was called 1 times
 * 
 */

