#include "entity_utils.h"

namespace nit
{
    Entity entity_find_by_name(const String& name)
    {
        for (Entity entity : entity_get_group<Name>().entities)
        {
            if (entity_get<Name>(entity).data == name)
            {
                return entity;
            }
        }

        return NULL_ENTITY;
    }

    void entity_find_by_name(Array<Entity>& entities, const String& name)
    {
        for (Entity entity : entity_get_group<Name>().entities)
        {
            if (entity_get<Name>(entity).data == name)
            {
                entities.push_back(entity);
            }
        }
    }

    Entity FindEntityByUUID(UUID uuid)
    {
        for (Entity entity : entity_get_group<UUID>().entities)
        {
            if (entity_get<UUID>(entity) == uuid)
            {
                return entity;
            }
        }

        return NULL_ENTITY;
    }

    void FindEntitiesByUUID(Array<Entity>& entities, UUID uuid)
    {
        for (Entity entity : entity_get_group<UUID>().entities)
        {
            if (entity_get<UUID>(entity) == uuid)
            {
                entities.push_back(entity);
            }
        }
    }
}