#ifndef ALGO_HH
#define ALGO_HH

#include <functional>

namespace PPUtils
{
    

/*!
 * \brief The generic merge-sort algorithm.
 * 
 *  Type arguments:
 *  
 *  FwrdIter: STL-compatible iterator type, that is at liest a forward iterator.
 *  Iterator's value type must be default-constructible and move-assignable. 
 *  
 *  CMP: Callable object or function that accepts two constant references to
 *  FwrdIter's value type as parameters and returns bool. CMP defines the 
 *  sorting policy. The 'greatest' element will be the first in range from first
 *  to last. CMP defaults to std::less.
 * 
 * \param first Iterator to the first element in range to be sorted.
 * \param last Pass-end iterator pointing to the end of range to be sorted.
 * \param cmp The comparator that defines element's relative order.
 * 
 * \pre @p first' points to a legal element before or to the same as @p last.
 *  @p last can be reached by incrementing @p first finite times.
 *  All iterators between @p first and @p last must be valid.
 * 
 * \post Range from @p first to @p last is sorted in order defined by @p cmp.
 *  The 'greates' element is the first.
 * 
 *  Exception guarantee: Basic guarantee.
 * 
 *  Thread safety: All elements in range between @p first and @p last are 
 *  modified (order may change). @p cmp is accessed.
 * 
 *  Iterator validaty: All iterators, pointers and referances to objects in 
 *  range from @p first to @p last stay valid, but their referred object may 
 *  change.
 *  
 *  Complexity: time O(N log N), memory O(N). N is range width.
 *  
 * \exception std::bad_alloc, if memory allocation fails. May throw also if 
 *  FwrdIter's value type is not no-throw move-assignable and no-throw 
 *  default-constructible.
 */
template <class FwrdIter, class CMP = std::less<typename FwrdIter::value_type> >
void mergeSort(FwrdIter first, FwrdIter last, const CMP& cmp = CMP());


} // Namespace PPUtils


#include "algo_impl.hh"

#endif // ALGO_HH

