/* concurrentstresstest_impl.hh
 * This is the implementation file for the PPTest::ConcurrentStressTest
 * class template.
 * 
 * NOTE: This is actually a header file (extension to concurrentstresstest.hh). 
 * Do not declare this file as a source file in your project configuration.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 16-July-2015
 */

#include <cassert>

namespace PPTest
{

template <unsigned N>
template <class Fn, class... Args>
ConcurrentStressTest<N>::ConcurrentStressTest(Fn fn, Args... args): 
    start_flag_(false), abort_flag_(false), mx_(), cv_()
{
    // Create test threads.
    for (unsigned i=0; i<threads_.size(); ++i){
        std::thread t(&ConcurrentStressTest::test<Fn, Args...>,
                      this, fn, args...);
        threads_[i] = std::move(t);
    }
}


template <unsigned N>
ConcurrentStressTest<N>::~ConcurrentStressTest()
{
    if ( this->isUsable() ){
        // Quit joinable threads before destroying the object.
        mx_.lock();
        abort_flag_ = true;
        cv_.notify_all();
        mx_.unlock();
        for (unsigned i=0; i<threads_.size(); ++i){
            threads_[i].join();
        }
    }
}


template <unsigned N>
void ConcurrentStressTest<N>::startTest()
{
    // Check precondition:
    assert(this->isUsable() && "ConcurrentStressTest::startTest() can be "
           "called only once in a object lifetime.");
    
    // Allow threads to start.
    mx_.lock();
    start_flag_ = true;
    cv_.notify_all();
    mx_.unlock();
    
    // Wait untill all threads are finished.
    for (unsigned i=0; i<threads_.size(); ++i){
        threads_[i].join();
    }
}


template <unsigned N>
template <class Fn, class... Args>
void ConcurrentStressTest<N>::startTest(Fn fn, Args... args)
{
    bool ready(false);
    std::mutex mx;
    std::condition_variable cv;
    std::array<std::thread, N> threads;
    //Create threads
    for (unsigned i=0; i<threads.size(); ++i){
        std::thread t(&ConcurrentStressTest::staticTest<Fn, Args...>,
                      &ready, &mx, &cv, fn, args...);
        threads[i] = std::move(t);
    }
    // Start test
    mx.lock();
    ready = true;
    cv.notify_all();
    mx.unlock();
    for (unsigned i=0; i<threads.size(); ++i){
        threads[i].join();
    }
}


template <unsigned N>
bool ConcurrentStressTest<N>::isUsable() const
{
    return !start_flag_;
}


template <unsigned N>
template <class Fn, class... Args>
void ConcurrentStressTest<N>::test(Fn fn, Args... args)
{
    // Wait for permission to start.
    std::unique_lock<std::mutex> lock(mx_);
    while (!start_flag_ && !abort_flag_){
        cv_.wait(lock);
    }
    if (abort_flag_) return; //Test cancelled.
    lock.unlock();
    // Call function under test.
    fn(args...);
}


template <unsigned N>
template <class Fn, class... Args>
void ConcurrentStressTest<N>::staticTest(bool* start, std::mutex* mx, 
                       std::condition_variable* cv,
                       Fn fn, Args... args)
{
    // Wait for permission to start
    std::unique_lock<std::mutex> lock(*mx);
    cv->wait(lock, [&start]{return *start;} );
    // Call function under test
    fn(args...);
}

} // Namespace PPTest
