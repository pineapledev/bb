outputdir                      = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
binariesdir                    = "%{wks.location}/bin/"     .. outputdir .. "/%{prj.name}"
intermediatesdir               = "%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}"

workspace "nit"

    architecture   "x86_64"
    configurations { "Debug", "Release", "Dist" }
    startproject   "app"

project "nit"

    kind          "StaticLib"
    language      "C++"
    cppdialect    "C++20"
    location      "nit"
    targetdir     (binariesdir)
    objdir        (intermediatesdir)
    pchheader     "nit_pch.h"
    pchsource     "nit/src/nit_pch.cpp"
    forceincludes { "nit_pch.h" }
    
    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING",
        "AL_LIBTYPE_STATIC",
    }

    disablewarnings 
    {
    }

    includedirs 
    { 
        "nit/src",
        "nit/src/nit",
        "3rd/glfw/include",
        "3rd/glad/include",
        "3rd/stb/include",
        "3rd/openal/include",
        "3rd/openal/src",
        "3rd/openal/src/common",
        "3rd/yaml/include",
        "3rd/assimp/include",
        "3rd/imgui/src"
    }
    
    links
    {
        "glfw",
        "glad",
        "openal",
        "yaml",
        "assimp",
        "imgui"
    }

    files { "nit/src/**.h", "nit/src/**.cpp", "nit/src/**.inl" }

    filter "system:windows"
        links
        {
            "opengl32.lib"
        }

    filter "system:linux"
        links
        {
            "OpenGL"        
        }

    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines "NIT_DEBUG"

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines "NIT_RELEASE"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        defines "NIT_DIST"

    filter "system:windows"
        systemversion "latest"

project "app"

    kind          "ConsoleApp"
    language      "C++"
    cppdialect    "C++20"
    location      "app"
    targetdir     (binariesdir)
    objdir        (intermediatesdir)
    pchheader     "app_pch.h"
    pchsource     "app/src/app_pch.cpp"
    forceincludes { "app_pch.h" }
    
    defines
    {
    }

    disablewarnings 
    {
    }

    includedirs 
    {
        "nit/src",
        "app/src",
        "3rd/imgui/src",
        "3rd/yaml/include"
    }
    
    links
    {
        "nit"
    }

    files { "app/src/**.h", "app/src/**.cpp" }

    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines "NIT_DEBUG"

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines "NIT_RELEASE"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        defines "NIT_DIST"

    filter "system:windows"
        systemversion "latest"

group "3rd"

project "glfw"

    kind          "StaticLib"
    language      "C++"
    cppdialect    "C++20"
    location      "3rd/glfw"
    staticruntime "off"
    targetdir     (binariesdir)
    objdir        (intermediatesdir)

    files
    {
        "3rd/glfw/include/glfw/glfw3.h",
        "3rd/glfw/include/glfw/glfw3native.h",
        "3rd/glfw/src/internal.h",
        "3rd/glfw/src/platform.h",
        "3rd/glfw/src/mappings.h",
        "3rd/glfw/src/context.c",
        "3rd/glfw/src/init.c",
        "3rd/glfw/src/input.c",
        "3rd/glfw/src/monitor.c",
        "3rd/glfw/src/platform.c",
        "3rd/glfw/src/vulkan.c",
        "3rd/glfw/src/window.c",
        "3rd/glfw/src/egl_context.c",
        "3rd/glfw/src/osmesa_context.c",
        "3rd/glfw/src/null_platform.h",
        "3rd/glfw/src/null_joystick.h",
        "3rd/glfw/src/null_init.c",
        "3rd/glfw/src/null_monitor.c",
        "3rd/glfw/src/null_window.c",
        "3rd/glfw/src/null_joystick.c",
    }

    filter "system:linux"
        pic "On"
        systemversion "latest"
        staticruntime "off"

        files
        {
            "3rd/glfw/src/x11_init.c",
            "3rd/glfw/src/x11_monitor.c",
            "3rd/glfw/src/x11_window.c",
            "3rd/glfw/src/xkb_unicode.c",
            "3rd/glfw/src/posix_time.c",
            "3rd/glfw/src/posix_thread.c",
            "3rd/glfw/src/glx_context.c",
            "3rd/glfw/src/egl_context.c",
            "3rd/glfw/src/osmesa_context.c",
            "3rd/glfw/src/linux_joystick.c"
        }

        defines
        {
            "_GLFW_X11"      
        }

    filter "system:windows"
        systemversion "latest"
        staticruntime "off"

        files
        {
            "3rd/glfw/src/win32_init.c",
            "3rd/glfw/src/win32_module.c",
            "3rd/glfw/src/win32_joystick.c",
            "3rd/glfw/src/win32_monitor.c",
            "3rd/glfw/src/win32_time.h",
            "3rd/glfw/src/win32_time.c",
            "3rd/glfw/src/win32_thread.h",
            "3rd/glfw/src/win32_thread.c",
            "3rd/glfw/src/win32_window.c",
            "3rd/glfw/src/wgl_context.c",
            "3rd/glfw/src/egl_context.c",
            "3rd/glfw/src/osmesa_context.c"
        }

        defines 
        { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

        filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines "NIT_DEBUG"

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines "NIT_RELEASE"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        defines "NIT_DIST"

project "glad"

    kind          "StaticLib"
    language      "C"
    staticruntime "off"
    location      "3rd/glad"
    targetdir     (binariesdir)
    objdir        (intermediatesdir)

    files
    {
        "3rd/glad/include/glad/glad.h",
        "3rd/glad/include/KHR/khrplatform.h",
        "3rd/glad/src/glad.c"
    }

    includedirs
    {
        "3rd/glad/include"
    }

    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines "NIT_DEBUG"

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines "NIT_RELEASE"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        defines "NIT_DIST"

    filter "system:windows"
        systemversion "latest"

project "openal"
    
        kind          "StaticLib"
        language      "C++"
        cppdialect    "C++20"
        location      "3rd/openal"
        staticruntime "off"
        
        targetdir (binariesdir)
        objdir    (intermediatesdir)
    
        disablewarnings 
        { 
            "5030",
            "4065",
            "4834"
        }
        
        includedirs
        {
            "3rd/openal/src",
            "3rd/openal/src/alc",
            "3rd/openal/src/common",
            "3rd/openal/include",
            "3rd/openal/include/AL"
        }
        
        files
        {
            "3rd/openal/src/**.h",
            "3rd/openal/src/**.cpp"
        }
        
        excludes
        {
            "3rd/openal/src/alc/mixer/mixer_neon.cpp"
        }
        
        defines
        {
            "AL_LIBTYPE_STATIC"
        }
    
        filter "system:windows"
            systemversion "latest"
        
            defines
            {
                "WIN32",
                "_WINDOWS",
                "AL_BUILD_LIBRARY",
                "AL_ALEXT_PROTOTYPES",
                "_WIN32",
                "_WIN32_WINNT=0x0502",
                "_CRT_SECURE_NO_WARNINGS",
                "NOMINMAX",
                "CMAKE_INTDIR=\"Debug\"",
                "OpenAL_EXPORTS",
                "_SILENCE_NODISCARD_LOCK_WARNINGS"
            }
        
            links
            {
                "winmm"
            }
    
        filter "system:linux"
            defines
            {
                "AL_BUILD_LIBRARY",
                "AL_ALEXT_PROTOTYPES",
                "LINUX",
            }
        
            links
            {
                "pthread",    -- Necesario para concurrencia en Linux
                "dl",         -- Biblioteca para la carga dinámica de objetos
                "m",          -- Matemáticas
                "pulse",      -- PulseAudio
                "pulse-simple" -- Dependencia adicional para PulseAudio
            }
    
        filter "configurations:Debug"
            symbols "On"
            runtime "Debug"
            defines "NIT_DEBUG"
    
        filter "configurations:Release"
            optimize "On"
            runtime "Release"
            defines "NIT_RELEASE"
    
        filter "configurations:Dist"
            runtime "Release"
            optimize "On"
            defines "NIT_DIST"

project "yaml"

    kind          "StaticLib"
    language      "C++"
    cppdialect    "C++20"
    location      "3rd/yaml"
    staticruntime "off"

    targetdir (binariesdir)
    objdir    (intermediatesdir)

	files
	{
		"3rd/yaml/src/**.h",
		"3rd/yaml/src/**.cpp",
		"3rd/yaml/include/**.h"
	}

	includedirs
	{
		"3rd/yaml/include"
	}

    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines "NIT_DEBUG"

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines "NIT_RELEASE"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        defines "NIT_DIST"

    filter "system:windows"
        systemversion "latest"

project "assimp"

    kind          "StaticLib"
    language      "C++"
    location      "3rd/assimp"
    staticruntime "off"

    targetdir (binariesdir)
    objdir    (intermediatesdir)

  defines 
  {
      -- "SWIG",
      "ASSIMP_BUILD_NO_OWN_ZLIB",

      "ASSIMP_BUILD_NO_X_IMPORTER",
      "ASSIMP_BUILD_NO_3DS_IMPORTER",
      "ASSIMP_BUILD_NO_MD3_IMPORTER",
      "ASSIMP_BUILD_NO_MDL_IMPORTER",
      "ASSIMP_BUILD_NO_MD2_IMPORTER",
      -- "ASSIMP_BUILD_NO_PLY_IMPORTER",
      "ASSIMP_BUILD_NO_ASE_IMPORTER",
      -- "ASSIMP_BUILD_NO_OBJ_IMPORTER",
      "ASSIMP_BUILD_NO_AMF_IMPORTER",
      "ASSIMP_BUILD_NO_HMP_IMPORTER",
      "ASSIMP_BUILD_NO_SMD_IMPORTER",
      "ASSIMP_BUILD_NO_MDC_IMPORTER",
      "ASSIMP_BUILD_NO_MD5_IMPORTER",
      "ASSIMP_BUILD_NO_STL_IMPORTER",
      "ASSIMP_BUILD_NO_LWO_IMPORTER",
      "ASSIMP_BUILD_NO_DXF_IMPORTER",
      "ASSIMP_BUILD_NO_NFF_IMPORTER",
      "ASSIMP_BUILD_NO_RAW_IMPORTER",
      "ASSIMP_BUILD_NO_OFF_IMPORTER",
      "ASSIMP_BUILD_NO_AC_IMPORTER",
      "ASSIMP_BUILD_NO_BVH_IMPORTER",
      "ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
      "ASSIMP_BUILD_NO_IRR_IMPORTER",
      "ASSIMP_BUILD_NO_Q3D_IMPORTER",
      "ASSIMP_BUILD_NO_B3D_IMPORTER",
      -- "ASSIMP_BUILD_NO_COLLADA_IMPORTER",
      "ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
      "ASSIMP_BUILD_NO_CSM_IMPORTER",
      "ASSIMP_BUILD_NO_3D_IMPORTER",
      "ASSIMP_BUILD_NO_LWS_IMPORTER",
      "ASSIMP_BUILD_NO_OGRE_IMPORTER",
      "ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
      "ASSIMP_BUILD_NO_MS3D_IMPORTER",
      "ASSIMP_BUILD_NO_COB_IMPORTER",
      "ASSIMP_BUILD_NO_BLEND_IMPORTER",
      "ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
      "ASSIMP_BUILD_NO_NDO_IMPORTER",
      "ASSIMP_BUILD_NO_IFC_IMPORTER",
      "ASSIMP_BUILD_NO_XGL_IMPORTER",
      "ASSIMP_BUILD_NO_FBX_IMPORTER",
      "ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
      -- "ASSIMP_BUILD_NO_GLTF_IMPORTER",
      "ASSIMP_BUILD_NO_C4D_IMPORTER",
      "ASSIMP_BUILD_NO_3MF_IMPORTER",
      "ASSIMP_BUILD_NO_X3D_IMPORTER",
      "ASSIMP_BUILD_NO_MMD_IMPORTER",
      
      "ASSIMP_BUILD_NO_STEP_EXPORTER",
      "ASSIMP_BUILD_NO_SIB_IMPORTER",

      -- "ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS",
      -- "ASSIMP_BUILD_NO_FLIPUVS_PROCESS",
      -- "ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS",
      -- "ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS",
      "ASSIMP_BUILD_NO_JOINVERTICES_PROCESS",
      -- "ASSIMP_BUILD_NO_TRIANGULATE_PROCESS",
      "ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS",
      -- "ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS",
      "ASSIMP_BUILD_NO_REMOVEVC_PROCESS",
      "ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS",
      "ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS",
      "ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS",
      -- "ASSIMP_BUILD_NO_VALIDATEDS_PROCESS",
      "ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS",
      "ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS",
      "ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS",
      "ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS",
      "ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS",
      "ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS",
      "ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS",
      "ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS",
      "ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS",
      "ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS",
      "ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS",
      "ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS",
      "ASSIMP_BUILD_NO_DEBONE_PROCESS",
      "ASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS",
      "ASSIMP_BUILD_NO_GLOBALSCALE_PROCESS",
      "_CRT_SECURE_NO_WARNINGS"
  }

    disablewarnings 
    { 
        "4267",
        "4065"
    }

  files 
  {
      "3rd/assimp/include/**",
      "3rd/assimp/code/Assimp.cpp",
      "3rd/assimp/code/BaseImporter.cpp",
      "3rd/assimp/code/ColladaLoader.cpp",
      "3rd/assimp/code/ColladaParser.cpp",
      "3rd/assimp/code/CreateAnimMesh.cpp",
      "3rd/assimp/code/PlyParser.cpp",
      "3rd/assimp/code/PlyLoader.cpp",
      "3rd/assimp/code/BaseProcess.cpp",
      "3rd/assimp/code/EmbedTexturesProcess.cpp",
      "3rd/assimp/code/ConvertToLHProcess.cpp",
      "3rd/assimp/code/DefaultIOStream.cpp",
      "3rd/assimp/code/DefaultIOSystem.cpp",
      "3rd/assimp/code/DefaultLogger.cpp",
      "3rd/assimp/code/GenVertexNormalsProcess.cpp",
      "3rd/assimp/code/Importer.cpp",
      "3rd/assimp/code/ImporterRegistry.cpp",
      "3rd/assimp/code/MaterialSystem.cpp",
      "3rd/assimp/code/PostStepRegistry.cpp",
      "3rd/assimp/code/ProcessHelper.cpp",
      "3rd/assimp/code/scene.cpp",
      "3rd/assimp/code/ScenePreprocessor.cpp",
      "3rd/assimp/code/ScaleProcess.cpp",
      "3rd/assimp/code/SGSpatialSort.cpp",
      "3rd/assimp/code/SkeletonMeshBuilder.cpp",
      "3rd/assimp/code/SpatialSort.cpp",
      "3rd/assimp/code/TriangulateProcess.cpp",
      "3rd/assimp/code/ValidateDataStructure.cpp",
      "3rd/assimp/code/Version.cpp",
      "3rd/assimp/code/VertexTriangleAdjacency.cpp",
      "3rd/assimp/code/ObjFileImporter.cpp",
      "3rd/assimp/code/ObjFileMtlImporter.cpp",
      "3rd/assimp/code/ObjFileParser.cpp",
      "3rd/assimp/code/glTFImporter.cpp",
      "3rd/assimp/code/glTF2Importer.cpp",
      "3rd/assimp/code/MakeVerboseFormat.cpp",
      "3rd/assimp/code/CalcTangentsProcess.cpp",
      "3rd/assimp/code/ScaleProcess.cpp",
      "3rd/assimp/code/EmbedTexturesProcess.cpp",
      "3rd/assimp/contrib/irrXML/*"
  }

  includedirs 
  {
      "3rd/assimp/include",
      "3rd/assimp/contrib/irrXML",
      "3rd/assimp/contrib/zlib",
      "3rd/assimp/contrib/rapidjson/include"
  }

   filter "system:windows"
      systemversion "latest"

      filter "configurations:Debug"
      symbols "On"
      runtime "Debug"
      defines "NIT_DEBUG"

  filter "configurations:Release"
      optimize "On"
      runtime "Release"
      defines "NIT_RELEASE"

  filter "configurations:Dist"
      runtime "Release"
      optimize "On"
      defines "NIT_DIST"


project "imgui"

    kind          "StaticLib"
    language      "C++"
    cppdialect    "C++20"
    location      "3rd/imgui"
    staticruntime "off"
    targetdir     (binariesdir)
    objdir        (intermediatesdir)

    files
	{
		"3rd/imgui/src/*.h",
		"3rd/imgui/src/*.cpp"
	}
	
	includedirs
	{
        "3rd/glfw/include",
		"3rd/imgui/src"
	}

    links
    {
        "glfw"
    }


    filter "system:windows"
        systemversion "latest"
    
    filter "configurations:Debug"
        symbols "On"
        runtime "Debug"
        defines "NIT_DEBUG"

    filter "configurations:Release"
        optimize "On"
        runtime "Release"
        defines "NIT_RELEASE"

    filter "configurations:Dist"
		runtime "Release"
		optimize "On"
        defines "NIT_DIST"

group ""