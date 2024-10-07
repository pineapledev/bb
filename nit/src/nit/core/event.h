﻿#pragma once

namespace Nit
{
    template<typename T>
    struct Delegate;
    
    template<typename R, typename... Args>
    struct Delegate<R(Args...)>
    {
        R (*function_ptr)(Args...) = nullptr;
        
        static Delegate Create(R (*function_ptr)(Args...))
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
    void Bind(Delegate<R(Args...)>& delegate, R (*function_ptr)(Args...))
    {
        delegate.function_ptr = function_ptr;
    }
    
    template<typename R, typename... Args>
    R Invoke(Delegate<R(Args...)>& delegate, Args&&... args)
    {
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
    void AddListener(Event<Args...>& event, const Listener<Args...>& listener)
    {
        event.listeners.push_back(listener);
    }
    
    template<typename... Args>
    void RemoveListener(Event<Args...>& event, const Listener<Args...>& listener)
    {
        auto it = std::find(event.listeners.begin(), event.listeners.end(), listener);
        if (it == event.listeners.end())
        {
            return;
        }
        event.listeners.erase(it);
    }

    template<typename... Args>
    void Broadcast(Event<Args...>& event, Args&&... args)
    {
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
            RemoveListener(event, listener);
        }
    }
}