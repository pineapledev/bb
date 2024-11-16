#pragma once

namespace nit
{
    struct Name
    {
        String data;
    };
    
    bool   name_empty(const Name& name);
    bool   operator==(const Name& a, const Name& b);
    bool   operator!=(const Name& a, const Name& b);
    bool   name_contains(const Name& name, const String& other);
    
    void   register_name_component();
    
    
}
