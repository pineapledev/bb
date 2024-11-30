#include "entity_utils.h"

namespace nit
{
    EntityID entity_find_by_name(const String& name)
    {
        for (EntityID entity : entity_get_group<Name>().entities)
        {
            if (entity_get<Name>(entity).data == name)
            {
                return entity;
            }
        }

        return NULL_ENTITY;
    }

    void entity_find_by_name(Array<EntityID>& entities, const String& name)
    {
        for (EntityID entity : entity_get_group<Name>().entities)
        {
            if (entity_get<Name>(entity).data == name)
            {
                entities.push_back(entity);
            }
        }
    }

    EntityID entity_find_by_uuid(UUID uuid)
    {
        for (EntityID entity : entity_get_group<UUID>().entities)
        {
            if (entity_get<UUID>(entity) == uuid)
            {
                return entity;
            }
        }

        return NULL_ENTITY;
    }

    void entity_find_by_uuid(Array<EntityID>& entities, UUID uuid)
    {
        for (EntityID entity : entity_get_group<UUID>().entities)
        {
            if (entity_get<UUID>(entity) == uuid)
            {
                entities.push_back(entity);
            }
        }
    }
}