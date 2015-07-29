/* uniformobjectpool_impl.hh
 * This is the implementation file for the PPUtils::UniforObjectPool class
 * template methods.
 * 
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 29-June-2015
 */

#ifndef UNIFORMOBJECTPOOL_IMPL_HH
#define UNIFORMOBJECTPOOL_IMPL_HH

#include <cassert>

namespace PPUtils 
{

template<class T, class Builder>
const std::function<T*()> UniformObjectPool<T,Builder>::DEFAULT_BUILDER ([](){return new T();});

template <class T, class Builder>
UniformObjectPool<T, Builder>::UniformObjectPool(Builder* builder) :
    objects_(), builder_(builder)
{
    assert(builder_ != nullptr);
}


template <class T, class Builder>
UniformObjectPool<T, Builder>::UniformObjectPool(const Builder& builder) :
    objects_(), builder_(new Builder(builder))
{
}


template <class T, class Builder>
UniformObjectPool<T, Builder>::~UniformObjectPool()
{
}


UniformObjectPool::UniformObjectPool(UniformObjectPool&& other) noexcept
{
    std::swap(this->objects_, other.objects_);
    std::swap(this->builder_, other.builder_);
}


UniformObjectPool& UniformObjectPool::operator =(UniformObjectPool& other) noexcept
{
    if (&other != this){
        std::swap(this->objects_, other.objects_);
        std::swap(this->builder_, other.builder_);
    }
    return *this;
}


template <class T, class Builder>
std::unique_ptr<T> UniformObjectPool<T, Builder>::reserve()
{
    typename std::unique_ptr<T> rv(nullptr);
    if (!objects_.empty()){
        rv.swap(objects_.back());
        objects_.pop_back();
    }
    else {
        rv.reset( builder_->operator()() );
    }
    return rv;
}


template <class T, class Builder>
void UniformObjectPool<T, Builder>::release(std::unique_ptr<T>&& object)
{
    assert(object != nullptr);
    objects_.push_back( std::move(object) );
}


template <class T, class Builder>
unsigned UniformObjectPool<T, Builder>::size() const
{
    return objects_.size();
}


template <class T, class Builder>
void UniformObjectPool<T, Builder>::clear()
{
    objects_.clear();
}


template <class T, class Builder>
Builder* UniformObjectPool<T,Builder>::getBuilder() const
{
    return builder_.get();
}

} // Namespace PPUtils


#endif // UNIFORMOBJECTPOOL_IMPL_HH

