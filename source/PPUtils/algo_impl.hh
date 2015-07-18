
#include <iterator>
#include <vector>
#include <algorithm>

namespace PPUtils
{


template <class FwrdIter, class CMP>
void mergeSort(FwrdIter first, FwrdIter last, 
               const CMP& cmp = typename std::less<typename FwrdIter::value_type>())
{
    // Stopping condition.
    int range = std::distance(first, last);
    if (range <= 1){
        return;
    }
    
    // Sort sub-ranges.
    FwrdIter mid = first;
    std::advance(mid, range/2);
    PPUtils::mergeSort(first, mid, cmp);
    PPUtils::mergeSort(mid, last, cmp);
    
    // Merge
    using E = typename FwrdIter::value_type;
    typename std::vector<E> aux_v(range);
    typename std::vector<E>::iterator aux_iter = aux_v.begin();
    FwrdIter lhs_iter = first;
    FwrdIter rhs_iter = mid;
    
    while (lhs_iter != mid){
        if (rhs_iter == last){
            std::move(lhs_iter, mid, aux_iter);
            aux_iter = aux_v.end();
            break;
        }
        else if (cmp(*rhs_iter, *lhs_iter)){
            *aux_iter++ = std::move(*rhs_iter++);
        }
        else{
            *aux_iter++ = std::move(*lhs_iter++);
        }
    }   
    std::move(aux_v.begin(), aux_iter, first);
}

} // Namespace PPUtils
