#include "uuid.h"

namespace nit
{
    bool uuid_valid(const UUID& uuid)
    {
        return uuid.data != 0;
    }

    bool operator==(const UUID& a, const UUID& b)
    {
        return a.data == b.data;
    }

    bool operator!=(const UUID& a, const UUID& b)
    {
        return !(a == b);
    }
}