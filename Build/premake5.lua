require("premake", ">=5.0.0-alpha10")

include "module.lua"
include "../../TiltedCore/Build/module.lua"

workspace ("Tilted Network")

    ------------------------------------------------------------------
    -- setup common settings
    ------------------------------------------------------------------
    configurations { "Debug", "Release" }
    defines { "_CRT_SECURE_NO_WARNINGS" }

    location ("projects")
    startproject ("Tests")

    staticruntime "On"
    floatingpoint "Fast"
    vectorextensions "SSE2"
    warnings "Extra"

    cppdialect "C++17"

    platforms { "x32", "x64" }

    includedirs
    {
        "../ThirdParty/",
        "../Code/"
    }


    filter { "action:vs*"}
        buildoptions { "/wd4512", "/wd4996", "/wd4018", "/Zm500" }

    filter { "action:gmake2", "language:C++" }
        buildoptions { "-g -fpermissive" }
        linkoptions ("-lm -lpthread -pthread -Wl,--no-as-needed -lrt -g -fPIC")

    filter { "configurations:Release" }
        defines { "NDEBUG"}
        optimize ("On")
        targetsuffix ("_r")

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        optimize ("Off")
        symbols ( "On" )

    filter { "architecture:*86" }
        libdirs { "lib/x32" }
        targetdir ("lib/x32")

    filter { "architecture:*64" }
        libdirs { "lib/x64" }
        targetdir ("lib/x64")

    filter {}

    group ("Applications")
        project ("Tests")
            kind ("ConsoleApp")
            language ("C++")


            includedirs
            {
                "../Code/tests/include/",
                "../Code/network/include/",
                "../Code/protocol/include/",
                "../../TiltedCore/Code/core/include/"
            }

             files
             {
                "../Code/tests/include/**.h",
                "../Code/tests/src/**.cpp",
            }

            links
            {
                "Network",
                "Protocol",
                "Core",
                "cryptopp"
            }


            filter { "architecture:*86" }
                libdirs { "lib/x32" }
                targetdir ("bin/x32")

            filter { "architecture:*64" }
                libdirs { "lib/x64" }
                targetdir ("bin/x64")
		
    premake.extensions.network.generate()
