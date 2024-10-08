#pragma once
#include "nit/render/font.h"

namespace Nit
{
    struct Text
    {
        bool             visible = true;
        String           text    = "EMPTY TEXT";
        Font*            font    = nullptr;
        ID               font_id = 0;
        Vector4          tint    = V4_ONE;
        f32              spacing = 1.f;
        f32              size    = 1.f;
    };

    void AddFontToText(Text& text, ID font_id);
    void RemoveFontFromText(Text& text);
}