project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    targetdir (binariesdir)
    objdir (intermediatesdir)

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "system:windows"
        systemversion "latest"