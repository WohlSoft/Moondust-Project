#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <QQueue>
#include <functional>


/*!
    \brief Event queue entry

    Entry of an event queue, holds a delay time, pointer to variable, function.
    Also can hold a lamda-function code for execution.
*/
template<class T>
struct EventQueueEntry
{
    enum __Types
    {
        dummy=-1, //do nothing, skip this event
        caller=0,
        caller_t,//caller, call a function via pointer. Also possible with delay
        caller_func,
        timer,    //timer, wait a time
        wait_flag_var,    //do nothing while flag is true
        wait_flag_func,
        wait_flag_func_t,    //do nothing while flag is true
        wait_condition    //do nothing while flag is true
    };

    EventQueueEntry()
    {
        obj=NULL;
        call=NULL;
        call_t=NULL;
        delay=0;
        flag_var=NULL;
        flag_func=NULL;
        flag_func_t=NULL;
        type=dummy;
        flag_target=false;
    }

    ///
    /// \brief makeCaller
    ///
    /// Executes a static or any static non-member function
    /// \param _caller pointer to a static or any non-member function
    /// \param _delay time in milliseconds to wait before this function will be executed
    ///
    void makeCaller( void(*_caller)(void), int _delay=0)
    {
        type=caller;
        call = _caller;
        delay = _delay;
    }

    ///
    /// \brief makeCallerT
    ///
    /// Executes a member function of object of a type equal to defined on creation of templated event queue entry
    /// \param _obj pointer to object where function must be executed
    /// \param _delay time in milliseconds to wait before this function will be executed
    ///
    void makeCallerT(T*_obj, void(T::*_caller)(void), int _delay=0)
    {
        obj=_obj;
        type=caller_t;
        call_t = _caller;
        delay = _delay;
    }

    ///
    /// \brief makeCaller
    ///
    /// Executes a code of lamda-function
    /// \param _call_func lamda-function which returns void
    /// \param _delay time in milliseconds to wait before this function will be executed
    ///
    void makeCaller( std::function<void()> _call_func, int _delay=0)
    {
        type=caller_func;
        call_func=_call_func;
        delay = _delay;
    }

    ///
    /// \brief makeTimer
    ///
    /// Waits a specified time
    /// \param _delay milliseconds to wait
    ///
    void makeTimer(int _delay)
    {
        delay=_delay;
        type=timer;
    }

    ///
    /// \brief makeWaiterFlag
    /// Waits while flag is equal to target state
    /// \param _flag Pointer to boolean variable
    /// \param target target state
    /// \param _delay milliseconds to wait before checking of target state will be processed
    ///
    void makeWaiterFlag(bool *_flag, bool target=true, int _delay=0)
    {
        flag_var=_flag;
        delay=_delay;
        type=wait_flag_var;
        flag_target=target;
    }
    ///
    /// \brief makeWaiterFlag
    /// Waits while static or non-member function returns equal to target state
    /// \param _flag Pointer to function which returns a boolean
    /// \param target target state
    /// \param _delay milliseconds to wait before checking of target state will be processed
    ///
    void makeWaiterFlag(bool(*_flag)(), bool target=true, int _delay=0)
    {
        flag_func=_flag;
        delay=_delay;
        type=wait_flag_func;
        flag_target=target;
    }

    ///
    /// \brief makeWaiterFlagT
    /// Waits while member function returns equal to target state
    /// \param _flag Pointer to function which returns a boolean
    /// \param target target state
    /// \param _delay milliseconds to wait before checking of target state will be processed
    ///
    void makeWaiterFlagT(T*_obj, bool(T::*_flag)(), bool target=true, int _delay=0)
    {
        obj=_obj;
        flag_func_t=_flag;
        delay=_delay;
        type=wait_flag_func_t;
        flag_target=target;
    }

    ///
    /// \brief makeWaiterCond
    /// Waits while lamda-function returns equal to target state
    /// \param _flag lamda-function which returns a boolean
    /// \param target target state
    /// \param _delay milliseconds to wait before checking of target state will be processed
    ///
    void makeWaiterCond(std::function<bool()> _condition, bool target=true, int _delay=0)
    {
        condition = _condition;
        delay=_delay;
        type=wait_condition;
        flag_target=target;
    }

/*
    Private zone!
*/

    ///
    /// \brief trigger
    /// \warning this function is using by event queue internally. Use this function if you use this event outside of queue!
    ///
    /// Processes this event.
    /// \param step time delay between loop steps
    /// \return time left after delaying. Append this piece to step value in next time
    ///
    float trigger(float step=1)
    {
        delay-=step;
        switch(type)
        {
        case caller:
            if((delay<=0)&&(call))
                call();
            break;
        case caller_t:
            if((delay<=0)&&(obj)&&(call_t))
                (obj->*call_t)();
            break;
        case caller_func:
            if(delay<=0)
                call_func();
            break;
        case wait_flag_var:
            if((delay<=0)&&(flag_var))
            {
                if( (*flag_var)==flag_target )
                    delay=1;
            }
            break;
        case wait_flag_func:
            if((delay<=0)&&(flag_func))
            {
                if(flag_func()==flag_target)
                    delay=1;
            }
            break;
        case wait_flag_func_t:
            if((delay<=0)&&(obj)&&(flag_func_t))
            {
                if((obj->*flag_func_t)() == flag_target)
                    delay=1;
            }
            break;
        case wait_condition:
            if(delay<=0)
            {
                if( flag_target? ((condition())==true) : ((condition())==false) )
                    delay=1;
            }
            break;
            case timer: break; case dummy: break;
        }
        return delay; //return leaved time
    }

    T* obj;
    //for caller
    void (*call)(void);
    void (T::*call_t)(void);
    //for function
    std::function<void()> call_func;
    //for caller, function and for waiting timer
    float delay;
    //for a flag waiter
    bool *flag_var;
    bool (*flag_func)();
    bool (T::*flag_func_t)();
    //for a condition waiter
    std::function<bool()> condition;
    bool flag_target;

    //type of event
    __Types type;
};

/*!
    \brief Event queue

    Event queue provides sequenced and timed event processing.

    How to use:

    1) Create object:

    EventQueue<ParentClass > myEventQueue;

    2) Add into loop the calling of the processor which triggers events in queue

    myEventQueue.processEvents(ticks);

    where ticks - is a number of milliseconds time of each loop.

    3) when in some case you wanna process bunch of events
    - create an EventQueueEntry object and construct event of specific type

    EventQueueEntry<WorldScene >event1;
    event1.makeCaller(this, &myFunction, 1000);

    - add this object into end of queue

    myEventQueue.push_back(event1)

    We are added an event to execute function after 1000 milliseconds past previouse event.
    If event queue is empty, this event will be triggered in next loop step with waiting of 1000 millisecods

    4) If you wish abort all events in loop, call the abort() function inside
    myEventQueue.abort();
*/
template<class T>
class EventQueue
{
public:
    EventQueue()
    {
        left_time=0;
    }

    EventQueue(const EventQueue& eq)
    {
        left_time=eq.left_time;
        events=eq.events;
    }

    ~EventQueue()
    {}

    void processEvents(float timeStep=1.0f)
    {
        left_time = 0;
      process_event:
        if(events.isEmpty()) { left_time=0; return; }
        left_time = events.first().trigger(timeStep);
        if(left_time<=0)
        {
            events.pop_front();
            timeStep = fabs(left_time);
            goto process_event;
        }
    }

    void abort()
    {
        events.clear();
        left_time=0;
    }

    float left_time;
    QQueue<EventQueueEntry<T > > events;
};

#endif // EVENTQUEUE_H
