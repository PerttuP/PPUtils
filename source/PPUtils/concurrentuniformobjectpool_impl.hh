#ifndef CONCURRENTUNIFORMOBJECTPOOL_IMPL_HH
#define CONCURRENTUNIFORMOBJECTPOOL_IMPL_HH

namespace PPUtils 
{

template <class T, class Builder>
ConcurrentUniformObjectPool<T,Builder>::
ConcurrentUniformObjectPool(Builder* builder) :
    UniformObjectPool<T,Builder>(builder), mx_()
{
}


template <class T, class Builder>
ConcurrentUniformObjectPool<T,Builder>::
ConcurrentUniformObjectPool(const PPUtils::Builder& builder) :
    UniformObjectPool<T,Builder>(builder), mx_()
{
}


template <class T, class Builder>
ConcurrentUniformObjectPool<T,Builder>::~ConcurrentUniformObjectPool()
{   
}


template <class T, class Builder>
std::unique_ptr<T> ConcurrentUniformObjectPool<T,Builder>::reserve()
{
    std::unique_lock<std::mutex> lock(mx_);
    if (this->size() > 0){
        return UniformObjectPool(mx_);
    }
    lock.unlock();
    return typename std::unique_ptr<T>( this->getBuilder()->operator()() );
}


template <class T, class Builder>
void ConcurrentUniformObjectPool<T,Builder>::
release(std::unique_ptr<T>&& object)
{
    std::lock_guard<std::mutex> lock(mx_);
    UniformObjectPool::release( std::move(object) );
}


template <class T, class Builder>
void ConcurrentUniformObjectPool<T,Builder>::clear()
{
    std::lock_guard<std::mutex> lock(mx_);
    UniformObjectPool::clear();
}

} // namespace PPUtils

#endif // CONCURRENTUNIFORMOBJECTPOOL_IMPL_HH

