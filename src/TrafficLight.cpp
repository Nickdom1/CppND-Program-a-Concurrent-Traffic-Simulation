#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> uLock(_mutex);
    _condition.wait(uLock, [this] { 
        return !_queue.empty(); 
    });

    T message = std::move(_queue.back());
    _queue.pop_back();
    return message;
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> uLock(_mutex);
    _queue.push_back(std::move(msg));
    _condition.notify_one();
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        if(_queue.receive() == TrafficLightPhase::green) {
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    auto start = std::chrono::system_clock::now();
    auto cycleTime = (rand() % (6000 + 1 - 4000));
    auto nextCycle = start + std::chrono::milliseconds(cycleTime);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        start = std::chrono::system_clock::now();
        if(start > nextCycle) {
            _currentPhase = (_currentPhase == TrafficLightPhase::green) ? TrafficLightPhase::red : TrafficLightPhase::green;
            _queue.send(std::move(_currentPhase));
            nextCycle = start + std::chrono::milliseconds(cycleTime);
        }
    }

    
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 

    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 

    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
}

