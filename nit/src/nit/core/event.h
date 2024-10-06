#pragma once

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

    // handle stay linked, etc..
    // handle pointers to same func. Use set?
    
    template<typename... Args>
    struct Event
    {
        Array<Delegate<void(Args...)>> delegates;
    };
    
    template<typename... Args>
    void AddListener(Event<Args...>& event, const Delegate<void(Args...)>& delegate)
    {
        event.delegates.push_back(delegate);
    }
    
    template<typename... Args>
    void RemoveListener(Event<Args...>& event, const Delegate<void(Args...)>& delegate)
    {
        auto it = std::find(event.delegates.begin(), event.delegates.end(), delegate);
        if (it == event.delegates.end())
        {
            return;
        }
        event.delegates.erase(it);
    }
}