/* ConcurrentStressTestExample
 * 
 * A small expample program to show how to use the PPTest::ConcurrentStressTest
 * class.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 18-June-2015
 */

#include <iostream>
#include <mutex>
#include <functional>
#include "../../source/PPTest/concurrentstresstest.hh"


std::mutex mx;
void ConcurrentIncrement(int& i);

int main()
{
    int i(0);
    PPTest::ConcurrentStressTest<100> tester(&ConcurrentIncrement, 
                                             std::reference_wrapper<int>(i) );
    
    std::cout << std::boolalpha;
    std::cout << "Tester is usable?: " << tester.isUsable() << std::endl;
    
    tester.startTest();
    std::cout << "Test started!" << std::endl;
    std::cout << "Tester is usable?: " << tester.isUsable() << std::endl;
    std::cout << "i after test: " << i << " (expected 100)" << std::endl;
    
    if (i == 100){
        std::cout << "Test passed!" << std::endl;
    }
    else {
        std::cout << "Data race detected!" << std::endl;
    }
    
    return 0;
}


void ConcurrentIncrement(int& i)
{
    mx.lock();
    i = i+1;
    mx.unlock();
}


/*
 * Expected output:
 * 
 * Tester is usable?: true
 * Test started!
 * Tester is usable?: false
 * i after test: 100 (expected 100)
 * Test passed!
 */
