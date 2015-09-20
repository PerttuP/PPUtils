/* activeobject.cc
 *
 * This is the implementation file for final virtual methods for the abstract
 * ActiveObject class defined in activeobject.hh.
 *
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 20-Sep-2015
 */

#include "activeobject.hh"

namespace PPUtils
{


ActiveObject::ActiveObject() :
    stop_flag_(true), thread_(), mx_()
{
}


ActiveObject::~ActiveObject()
{
    std::unique_lock<std::mutex> lock(mx_);
    if (!stop_flag_){
        stop_flag_ = true;
        lock.unlock();
        thread_.join();
    }
}


void ActiveObject::start()
{
    std::lock_guard<std::mutex> lock(mx_);
    if (stop_flag_){
        stop_flag_ = false;
        thread_ = std::move(std::thread(ActiveObject::actionLoop, this));
    }
}


void ActiveObject::stop(bool waitToFinish)
{
    std::unique_lock<std::mutex> lock(mx_);
    if (!stop_flag_){
        stop_flag_ = true;
        lock.unlock();
        if (waitToFinish) {
            thread_.join();
        } else {
            thread_.detach();
        }
    }
}


bool ActiveObject::isStarted()
{
    std::lock_guard<std::mutex> lock(mx_);
    return !stop_flag_;
}


void ActiveObject::stopOnNextLoop()
{
    std::lock_guard<std::mutex> lock(mx_);
    stop_flag_ = true;
}


void ActiveObject::actionLoop()
{
    while (true){
        std::unique_lock<std::mutex> lock(mx_);
        if (stop_flag_){
            return;
        }
        lock.unlock();
        this->action();
    }
}

} // namespace PPUtils
