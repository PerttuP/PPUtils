/* activeobject.hh
 *
 * This header defines the ActiveObject abstract base class for active objects.
 * Active objects are objects, that work independly in their own thread.
 *
 * Author: Perttu Paarlahti     perttu.paarlahti@gmail.com
 * Created: 20-Sep-2015
 */

#ifndef ACTIVEOBJECT_HH
#define ACTIVEOBJECT_HH

#include <thread>
#include <mutex>

namespace PPUtils
{

/*!
 * \brief The ActiveObject class
 *  Abstract base class for active objects.
 */
class ActiveObject
{
public:

    /*!
     * \brief Constructor
     * \pre None.
     * \post Active object is initialized, but the actions are not executed until
     * start-method is called.
     */
    ActiveObject();

    /*!
     * \brief Destructor.
     * \post If active object is running, actions are stopped and waited
     *  to finish.
     */
    virtual ~ActiveObject();

    //! Copy-constructor is forbidden.
    ActiveObject(const ActiveObject&) = delete;

    //! Copy-assignment is forbidden.
    ActiveObject& operator=(const ActiveObject&) = delete;

    /*!
     * \brief Starts the actions if not already started.
     * \pre None.
     * \post Active object starts its actions, and keeps doing it in its own
     *  thread until told to stop.
     */
    virtual void start() final;

    /*!
     * \brief stop Tells action thread to stop.
     * \param waitToFinish If true, action thread is joined. Else detatched.
     * \pre None.
     * \post. If action thread was running, it is told to stop.
     */
    virtual void stop(bool waitToFinish = true) final;

    /*!
     * \brief Check if action thread is running.
     * \return True, if action thread is running.
     * \pre None.
     */
    virtual bool isStarted() final;


protected:

    /*!
     * \brief This method determines the actions the active object performs.
     *  This method must return in finite time. This method is called
     *  constantly until active object is told to stop.
     */
    virtual void action() = 0;

    /*!
     * \brief Tells thread to be stopped at next action loop iteration.
     *  Unlike stop(), the thread is not joined or detached at this point.
     *  This method is useful, if action itself wants to quit.
     * \pre None.
     * \post action is not called again.
     */
    virtual void stopOnNextLoop() final;


private:

    bool stop_flag_;
    std::thread thread_;
    std::mutex mx_;

    void actionLoop();
};

} // Namespace PPUtils

#endif // ACTIVEOBJECT_HH

