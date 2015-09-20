#include <iostream>
#include <chrono>
#include "activeobject.hh"


class ActiveCounter : public PPUtils::ActiveObject
{
public:

    ActiveCounter() : ActiveObject(), count_(0) {}
    virtual ~ActiveCounter() {}

    int getCount() {
        if (this->isStarted()){
            throw "Not finished!";
        }
        return count_;
    }

protected:
    virtual void action(){
        ++count_;
    }

private:
    int count_;
};


int main()
{
    ActiveCounter counter;
    std::cout << "How many incrementations can you do in 1s?" << std::endl;

    counter.start();
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    while (std::chrono::system_clock::now() - start < std::chrono::seconds(1) ){
        // Do nothing (dumb waiting)
    }
    counter.stop();

    std::cout << "Answer: " << counter.getCount() << std::endl;
    return 0;
}
