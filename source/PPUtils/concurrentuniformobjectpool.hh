#ifndef CONCURRENTUNIFORMOBJECTPOOL
#define CONCURRENTUNIFORMOBJECTPOOL

#include "uniformobjectpool.hh"
#include <mutex>

namespace PPUtils 
{

/*!
 * \brief The ConcurrentUniformObjectPool class
 *  This is a thread safe version of UniformObjectPool.
 */
template <class T, class Builder = std::function<T*()> >
class ConcurrentUniformObjectPool : public UniformObjectPool
{
public:
    
    /*!
     * \brief Constructor.
     * \param builder Builder object that will be used to construct new objects.
     * \pre builder != nullptr
     * \post Does the same as UniformObjectPool's corresponding constructor.
     */
    explicit ConcurrentUniformObjectPool(Builder* builder);
    
    /*!
     * \brief Constructor
     * \param builder Builder object that will be used to construct new objects.
     * \pre None.
     * \post Does the same as UniformObjectPool's corresponding constructor.
     */
    explicit ConcurrentUniformObjectPool(const Builder& builder = DEFAULT_BUILDER);
    
    /*!
     * \brief Destructor.
     * \pre  Undefined behaviour occurs, if any other thread is accessing the 
     *  object at the moment.
     * \post destroys this object and all its currently stored objects.
     */
    virtual ~ConcurrentUniformObjectPool();
    
    /*!
     * \brief Copy-constructor is forbidden.
     */
    ConcurrentUniformObjectPool(const ConcurrentUniformObjectPool&) = delete;
    
    /*!
     * \brief Move-constructor is forbidden.
     */
    ConcurrentUniformObjectPool(ConcurrentUniformObjectPool&&) = delete;
    
    /*!
     * \brief Copy-assingment operator is forbidden.
     */
    ConcurrentUniformObjectPool& operator=(const ConcurrentUniformObjectPool&) = delete;
    
    /*!
     * \brief Move-assignment operator is forbidden.
     */
    ConcurrentUniformObjectPool& operator=(ConcurrentUniformObjectPool&&) = delete;
    
    /*!
     * \brief Reimplements UniformObjectPool::reserve to be thread safe.
     */
    virtual typename std::unique_ptr<T> reserve();
    
    /*!
     * \brief Reimplements UniformObjectPool::release to be thread safe.
     */
    virtual void release(typename std::unique_ptr<T>&& object);
    
    /*!
     * \brief Reimplements UniformObjectPool::clear to be thread safe.
     */
    virtual void clear();
    
    
private:
    
    std::mutex mx_;
};

} // namespace PPUtils

// Include template method implementations
#include "concurrentuniformobjectpool_impl.hh"

#endif // CONCURRENTUNIFORMOBJECTPOOL

