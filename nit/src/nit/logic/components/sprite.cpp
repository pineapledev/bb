#include "sprite.h"

namespace Nit
{
    void AddTextureToSprite(Sprite& sprite, ID texture_id)
    {
        RemoveTextureFromSprite(sprite);

        if (IsAssetValid(texture_id))
        {
            sprite.texture_id = texture_id;
            sprite.texture = GetAssetDataPtr<Texture2D>(sprite.texture_id);
            RetainAsset(sprite.texture_id);
        }
    }

    void RemoveTextureFromSprite(Sprite& sprite)
    {
        if (IsAssetValid(sprite.texture_id))
        {
            sprite.texture = nullptr;
            ReleaseAsset(sprite.texture_id);
        }
    }
}