#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <QQueue>
#include <functional>

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

    void makeCaller( void(*_caller)(void), int _delay=0)
    {
        type=caller;
        call = _caller;
        delay = _delay;
    }

    void makeCallerT(T*_obj, void(T::*_caller)(void), int _delay=0)
    {
        obj=_obj;
        type=caller_t;
        call_t = _caller;
        delay = _delay;
    }


    void makeCaller( std::function<void()> _call_func, int _delay=0)
    {
        type=caller_func;
        call_func=_call_func;
        delay = _delay;
    }

    void makeTimer(int _delay)
    {
        delay=_delay;
        type=timer;
    }

    void makeWaiterFlag(bool *_flag, bool target=true, int _delay=0)
    {
        flag_var=_flag;
        delay=_delay;
        type=wait_flag_var;
        flag_target=target;
    }

    void makeWaiterFlag(bool(*_flag)(), bool target=true, int _delay=0)
    {
        flag_func=_flag;
        delay=_delay;
        type=wait_flag_func;
        flag_target=target;
    }
    void makeWaiterFlagT(T*_obj, bool(T::*_flag)(), bool target=true, int _delay=0)
    {
        obj=_obj;
        flag_func_t=_flag;
        delay=_delay;
        type=wait_flag_func_t;
        flag_target=target;
    }


    void makeWaiterCond(std::function<bool()> _condition, bool target=true, int _delay=0)
    {
        condition = _condition;
        delay=_delay;
        type=wait_condition;
        flag_target=target;
    }

    int trigger(int step=1)
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
                if( flag_target? ((*flag_var)==true) : ((*flag_var)==false) )
                    delay=1;
            }
            break;
        case wait_flag_func:
            if((delay<=0)&&(flag_func))
            {
                if( flag_target? ((flag_func())==true) : ((flag_func())==false) )
                    delay=1;
            }
            break;
        case wait_flag_func_t:
            if((delay<=0)&&(obj)&&(flag_func_t))
            {
                if( flag_target? ( ((obj->*flag_func_t)()) ==true) : ( ((obj->*flag_func_t)())==false) )
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
    int delay;
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

template<class T>
class EventQueue
{
public:
    EventQueue()
    {
        left_time=0;
    }

    ~EventQueue()
    {}

    void processEvents(int timeStep=1)
    {
        int appendTime=0;
        process_event:
        if(events.isEmpty()) { left_time=0; return; }
        left_time = events.first().trigger(timeStep+appendTime);
        if(left_time<=0)
        {
            events.pop_front();
            left_time=0;
            appendTime=left_time*-1;
            goto process_event;
        }
    }

    void abort()
    {
        events.clear();
        left_time=0;
    }

    int left_time;
    QQueue<EventQueueEntry<T > > events;
};

#endif // EVENTQUEUE_H
