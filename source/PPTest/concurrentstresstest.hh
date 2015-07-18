/* concurrentstresstest.hh
 * This header defines the PPTest::ConcurrentStressTest class template.
 * The class template is useful when testing concurrent programs for data races
 * and deadlocks.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 16-July-2015
 */

#ifndef CONCURRENTSTRESSTEST_HH
#define CONCURRENTSTRESSTEST_HH

#include <array>
#include <mutex>
#include <condition_variable>
#include <thread>


namespace PPTest
{

/*!
 * \brief The ConcurrentStressTest class
 *  This class helps testing concurrent functions. Using this class helps 
 *  detecting data races and deadlocks at liest in most common cases.
 *  
 *  Type argument N is the number of test threads that will call the function 
 *  under test simultaniously.
 */
template <unsigned N>
class ConcurrentStressTest
{
public:
    
    /*!
     * \brief ConcurrentStressTest constructor.
     *  Type arguments: \p Fn is a callable object that accepts Args-pack as its
     *  parameters. \p Fn should not throw exceptions. \p Fn must return in a 
     *  finite time. \p Fn and \p Args... must be copy-constructable.
     * 
     * \param fn Function under test.
     * \param args Parameters for the function under test.
     * 
     * \pre None.
     * 
     * \post \p N threads have been spawned. Threads are synchronised to call
     *  \p fn with \p args as its parameters when startTest() is called.
     *  This object is usable.
     */
    template <class Fn, class... Args>
    explicit ConcurrentStressTest(Fn fn, Args... args);
    
    
    /*!
     * \brief Destructor.
     * \pre None.
     * \post This object and all its test threads are safely destoyed.
     */
    ~ConcurrentStressTest();
      
    //! Copy-constructor is forbidden.
    ConcurrentStressTest(const ConcurrentStressTest<N>&) = delete;
    
    //! Assignment operator is forbidden.
    ConcurrentStressTest& operator= (const ConcurrentStressTest&) = delete;
    
    
    /*!
     * \brief Start stress test. Test threads call function under test 
     *  simultaniously.
     * \pre Object must be usable (isUsable() returns true).
     * \post On success, all test threads are finished. If function under test
     *  has concurrency problems, this function may never return. This object
     *  is no more usable after this function call (isUsable() returns false).
     */
    void startTest();
    
    
    /*!
     * \brief Static version of startTest().
     *  Type arguments: \p Fn is a callable object that accepts Args-pack as its
     *  parameters. \p Fn should not throw exceptions. \p Fn must return in a 
     *  finite time. \p Fn and \p Args... must be copy-constructable.
     * 
     * \param fn Function under test.
     * \param args Argument pack for the function under test.
     * \pre None.
     * \post Spawns \p N test threads that call \fn simultaniously. On success,
     *  all threads are finished. If function under test has concurrency 
     *  problems, this function may never return.
     */
    template <class Fn, class... Args>
    static void startTest(Fn fn, Args... args);
    
    
    /*!
     * \brief Check if object is usable (startTest() has not been called yet).
     * \return 
     */
    bool isUsable() const;
    
    
private:
    
    bool start_flag_;
    bool abort_flag_;
    std::mutex mx_;
    std::condition_variable cv_;
    std::array<std::thread, N> threads_;
    
    
    /* Aid function, that is started in its own thread. function calls function
     * under test after it has been given a permission.
     */
    template <class Fn, class... Args>
    void test(Fn fn, Args... args);
        
    // Static version of ConcurrentStressTest::test.
    template <class Fn, class... Args>
    static void staticTest(bool* start, std::mutex* mx, 
                           std::condition_variable* cv,
                           Fn fn, Args... args);
};


} // Namespace PPTest

// Include method implementations.
#include "concurrentstresstest_impl.hh"

#endif // CONCURRENTSTRESSTEST_HH
