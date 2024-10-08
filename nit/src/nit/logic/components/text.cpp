#include "text.h"

namespace Nit
{
    void AddFontToText(Text& text, ID font_id)
    {
        RemoveFontFromText(text);

        if (IsAssetValid(font_id))
        {
            text.font_id = font_id;
            text.font = GetAssetDataPtr<Font>(text.font_id);
            RetainAsset(text.font_id);
        }
    }

    void RemoveFontFromText(Text& text)
    {
        if (IsAssetValid(text.font_id))
        {
            text.font = nullptr;
            ReleaseAsset(text.font_id);
        }
    }
}