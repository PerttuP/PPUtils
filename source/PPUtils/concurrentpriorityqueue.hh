/**
 * @file
 * @brief Defines the ConcurrentPriorityQueue class template.
 * @author Perttu Paarlati 2016
 */

#ifndef CONCURRENTPRIORITYQUEUE_HH
#define CONCURRENTPRIORITYQUEUE_HH

#include <vector>
#include <functional>
#include <algorithm>
#include <mutex>
#include <condition_variable>

namespace PPUtils
{

/**
 * @brief Thread safe priority queue. Organizes elements to decreasing
 *  priority order. Elements of same priority level are stored in the
 *  FIFO-order.
 *
 *  Type parameters:
 *  @c T: The element type. If not stated otherwise, @c T is expected only to
 *  have default constructor and move-assignment operator.
 */
template <class T>
class ConcurrentPriorityQueue
{
public:

    /**
     * @brief Comparator determines priority order of elements.
     *  Comparator shall return true, if the first parameter is considered to
     *  have lower priority than the latter one.
     */
    typedef std::function<bool(const T&, const T&)> Comparator;


    /**
     * @brief Constructor.
     * @param cmp Comparator object.
     * @pre None.
     * @post Empty queue using copy of @p cmp is created.
     */
    ConcurrentPriorityQueue(const Comparator& cmp = std::less<T>()) :
        data_(), cmp_(cmp), mx_(), cv_()
    {
    }


    /**
     * @brief Destructor.
     * @pre Make sure that no thread is using the queue before destroying it.
     * @post All items in queue are destroyed.
     */
    ~ConcurrentPriorityQueue()
    {
    }


    /**
     * @brief Insert new item to the queue.
     * @param item Item to be inserted.
     * @pre None.
     * @post @p item is placed to place determined by queue's comparator.
     */
    void insert(T&& item)
    {
        std::unique_lock<std::mutex> lock(mx_);
        typename std::vector<T>::iterator it = std::lower_bound(data_.begin(), data_.end(), item, cmp_);
        data_.insert(it, std::move(item));
        lock.unlock();
        cv_.notify_one();
    }


    /**
     * @brief insert new item to the queue.
     * @param item Item to be inserted.
     * @pre None.
     * @post Copy of @p item is placed to place determined by queue's comparator.
     */
    void insert(const T& item)
    {
        T itemCopy(item);
        this->insert(std::move(itemCopy));
    }


    /**
     * @brief Fetch and remove the topmost element from the queue.
     *  If queue is empty, wait for items to be consumed, or for @p timeoutMs.
     * @param item Item fetched from the queue. If pop times out, item is not changed.
     * @param timeoutMs Time to be waited before timeout (in milliseconds).
     * @return True, if item has been fetched and removed successfully.
     *  False, if operation times out.
     */
    bool pop(T& item, int timeoutMs = 0)
    {
        std::unique_lock<std::mutex> lock(mx_);

        if (data_.empty()){
            // Nothing to consume. Wait for new items.
            std::cv_status status = cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs));
            if (status == std::cv_status::timeout){
                // Timeout
                return false;
            }
            else if (data_.empty()) {
                // Another thread consumed added items.
                return false;
            }
        }

        item = std::move(data_[data_.size()-1]);
        data_.pop_back();
        return true;
    }


private:

    std::vector<T> data_;
    const Comparator cmp_;
    std::mutex mx_;
    std::condition_variable cv_;
};

} // PPUtils

#endif // CONCURRENTPRIORITYQUEUE_HH

