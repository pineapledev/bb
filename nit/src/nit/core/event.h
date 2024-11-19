#pragma once

namespace nit
{
    template<typename T>
    struct Delegate;
    
    template<typename R, typename... Args>
    struct Delegate<R(Args...)>
    {
        R (*function_ptr)(Args...) = nullptr;
        
        static Delegate create(R (*function_ptr)(Args...))
        {
            Delegate delegate;
            delegate.function_ptr = function_ptr;
            return delegate;
        }
    };
    
    template<typename R, typename... Args>
    bool operator==(const Delegate<R(Args...)>& a, const Delegate<R(Args...)>& b)
    {
        return a.function_ptr == b.function_ptr;
    }
    
    template<typename R, typename... Args>
    bool operator!=(const Delegate<R(Args...)>& a, const Delegate<R(Args...)>& b)
    {
        return a.function_ptr != b.function_ptr;
    }

    template<typename R, typename... Args>
    bool delegate_empty(const Delegate<R(Args...)>& delegate)
    {
        return delegate.function_ptr == nullptr;
    }

    template<typename R, typename... Args>
    bool operator!(const Delegate<R(Args...)>& delegate)
    {
        return delegate_empty(delegate);
    }
    
    template<typename R, typename... Args>
    void delegate_bind(Delegate<R(Args...)>& delegate, R (*function_ptr)(Args...))
    {
        delegate.function_ptr = function_ptr;
    }

    template<typename R, typename... Args>
    void delegate_unbind(Delegate<R(Args...)>& delegate)
    {
        delegate.function_ptr = nullptr;
    }
    
    template<typename R, typename... Args>
    R delegate_invoke(Delegate<R(Args...)>& delegate, Args... args)
    {
        NIT_CHECK_MSG(!delegate_empty(delegate), "Trying to invoke empty delegate!");
        return delegate.function_ptr(std::forward<Args>(args)...);
    }

    enum class ListenerAction : u8
    {
        StayListening = 0,
        StopListening
    };

    template<typename... Args>
    using Listener = Delegate<ListenerAction(Args...)>; 
    
    template<typename... Args>
    struct Event
    {
        Array<Listener<Args...>> listeners;
    };

    template<typename... Args>
    bool event_empty(const Event<Args...>& event)
    {
        return event.listeners.empty();
    }
    
    template<typename... Args>
    bool operator !(const Event<Args...>& event)
    {
        return event_empty(event);
    }
    
    template<typename... Args>
    void event_add_listener(Event<Args...>& event, const Listener<Args...>& listener)
    {
        if (delegate_empty(listener))
        {
            NIT_CHECK_MSG(false, "Trying to add empty listener!");
            return;
        }
        event.listeners.push_back(listener);
    }

    template<typename... Args>
    void operator +=(Event<Args...>& event, const Listener<Args...>& listener)
    {
       event_add_listener(event, listener);
    }
    
    template<typename... Args>
    void event_remove_listener(Event<Args...>& event, const Listener<Args...>& listener)
    {
        if (delegate_empty(listener))
        {
            NIT_CHECK_MSG(false, "Trying to remove empty listener!");
            return;
        }
        auto it = std::find(event.listeners.begin(), event.listeners.end(), listener);
        if (it == event.listeners.end())
        {
            return;
        }
        event.listeners.erase(it);
    }
    
    template<typename... Args>
    void operator -=(Event<Args...>& event, const Listener<Args...>& listener)
    {
        event_remove_listener(event, listener);
    }
    
    template<typename... Args>
    void event_remove_all_listeners(Event<Args...>& event)
    {
        event.listeners.clear();
    }

    template<typename... Args>
    void event_broadcast(Event<Args...>& event, Args... args)
    {
        if (event_empty(event))
        {
            return;
        }
        
        Array<Listener<Args...>> listeners_to_remove;
        listeners_to_remove.reserve(event.listeners.size());
        
        for (Listener<Args...>& listener : event.listeners)
        {
            switch (listener.function_ptr(std::forward<Args>(args)...))
            {
            case ListenerAction::StayListening:
                break;
            case ListenerAction::StopListening:
                listeners_to_remove.push_back(listener);
                break;
            }
        }

        for (Listener<Args...>& listener : listeners_to_remove)
        {
            event_remove_listener(event, listener);
        }
    }
}