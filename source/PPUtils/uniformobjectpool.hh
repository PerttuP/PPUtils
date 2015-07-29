/* uniformobjectpool.hh
 * This header file defines the PPUtils::UniformObjectPool class template. This
 * class template can be used as a storage for complex objects or as a builder
 * class.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 29-June-2015
 */

#ifndef UNIFORMOBJECTPOOL_HH
#define UNIFORMOBJECTPOOL_HH

#include <vector>
#include <memory>
#include <functional>

namespace PPUtils
{

/*!
 * \brief The UniformObjectPool class
 *  This is a generic object pool. It may be used for storing complex objects
 *  when repeatly constructing them would be too expensive. It may also used as
 *  a builder/factory class.
 *  
 *  Note that there is no segmentation among stored objects i.e. all stored
 *  objects are concerned equal and user cannot choose which kind of object he 
 *  is going to get next. If this kind of functionality is desired, use 
 *  ObjectPool instead. Objects are stored in their current state, so make sure 
 *  that stored objects are in a re-usable state when released to the pool
 *  (or alternatively re-initialize all objects after reserveing them).
 *  
 *  This version of object pool is not thread safe. Use this version only from
 *  one thread. If you need to access object pool from multiple threads, use
 *  ConcurrentUniformObjectPool instead.
 *  
 *  Type arguments:
 *   \p T: Type of stored objects.
 *  
 *   \p Builder: A functor class that is used to construct new objects when 
 *  needed. Builder must provide a function operator taking no arguments and 
 *  returning a dynamically allocated T-object.
 */
template <class T, class Builder = typename std::function<T*()> >
class UniformObjectPool
{
public:
    
    /*!
     * \brief Constructor
     * \param builder Builder object that will be used to construct new objects.
     *  Ownership of builder is passed to this object.
     * \pre builder != nullptr.
     */
    explicit UniformObjectPool(Builder* builder);
    
    /*!
     * \brief Constructor
     * \param builder Builder object that will be used to construct new objects.
     *  This object uses a copy of builder to do so (Builder must be 
     *  copy-constructible).
     */
    explicit UniformObjectPool(const Builder& builder = DEFAULT_BUILDER);
    
    /*!
     * \brief Destructor destroys the pool and all its currently stored objects.
     */
    virtual ~UniformObjectPool();
    
    /*!
     * \brief Copy-constructor is forbidden.
     */
    UniformObjectPool(const UniformObjectPool&) = delete;
    
    /*!
     * \brief Assignment operator is forbidden.
     */
    UniformObjectPool& operator = (const UniformObjectPool&) = delete;
    
    /*!
     * \brief Reserve next object from pool.
     * \return Unique pointer to reserved object. Returned object may be re-used
     *  or constructed using Builder.
     * \pre None.
     */
    virtual typename std::unique_ptr<T> reserve();
    
    /*!
     * \brief Return object (back) to the pool.
     * \param object Object returned to the pool. Object may be reserved from
     *  the pool earlier or constructed other way.
     * \pre object != nullptr.
     * \post Object is stored in the pool and may be re-used calling the reserve
     *  method. Object is stored in its current state.
     */
    virtual void release(typename std::unique_ptr<T>&& object);
    
    /*!
     * \brief Return number of objects currently stored in the pool.
     * \pre None.
     */
    virtual unsigned size() const final;
    
    /*!
     * \brief Destroy all objects currently stored in the pool.
     * \pre None.
     * \post All currently stored objects are destoyed and pool contains no 
     *  re-usable objects.
     */
    virtual void clear();
  
    
protected:
    
    /*!
     * \brief DEFAULT_BUILDER
     *  This functor constructs stored objects using their default constructor.
     *  Instantiating this functor requires \p T to be default constructible.
     */
    static const typename std::function<T*()> DEFAULT_BUILDER;
    
    
private:
    
    typename std::vector<std::unique_ptr<T> > objects_;
    std::unique_ptr<Builder> builder_;
};

} // Namespace PPUtils

// Include template implementations.
#include "uniformobjectpool_impl.hh"

#endif // UNIFORMOBJECTPOOL_HH

