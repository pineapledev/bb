#pragma once
#include "entity.h"

namespace nit
{
    void   entity_find_by_name(Array<Entity>& entities, const String& name);
    Entity entity_find_by_name(const String& name);

    Entity FindEntityByUUID(UUID uuid);
    void   FindEntitiesByUUID(Array<Entity>& entities, UUID uuid);
}
