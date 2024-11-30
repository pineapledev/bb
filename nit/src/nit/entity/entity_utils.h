#pragma once
#include "entity.h"

namespace nit
{
    void   entity_find_by_name(Array<EntityID>& entities, const String& name);
    EntityID entity_find_by_name(const String& name);

    EntityID entity_find_by_uuid(UUID uuid);
    void   entity_find_by_uuid(Array<EntityID>& entities, UUID uuid);
}
