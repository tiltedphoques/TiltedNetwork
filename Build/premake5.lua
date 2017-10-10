require("premake", ">=5.0.0-alpha10")

workspace ("DestroyerOfWorlds")

    ------------------------------------------------------------------
    -- setup common settings
    ------------------------------------------------------------------
    configurations { "Debug", "Release" }
    defines { "_CRT_SECURE_NO_WARNINGS" }
    flags { "StaticRuntime" }
    floatingpoint ("Fast")
    location ("projects")
    startproject ("Tests")
    vectorextensions ("SSE2")

    platforms { "x32", "x64" }

    includedirs
    { 
        "../Include/", 
        "../Code/"
    }
	
	flags { "ExtraWarnings", "StaticRuntime", "FloatFast", "EnableSSE2" }

    filter { "action:vs*"}
        buildoptions { "/wd4512", "/wd4996", "/wd4018", "/Zm500" }
        
    filter { "action:gmake", "language:C++" }
            buildoptions { "-g --std=c++14 -fpermissive" }
            linkoptions ("-lm -lpthread -pthread -Wl,--no-as-needed -lrt -g -fPIC")
            
    filter { "configurations:Release" }
        defines { "NDEBUG"}
        optimize ("On")
        targetsuffix ("_r")
        
    filter { "configurations:Debug" }
        defines { "DEBUG" }
        optimize ("Off")
		symbols ( "On" )

	group ("Applications")
		project ("Tests")
			kind ("ConsoleApp")
			language ("C++")
			
			includedirs
            {
                "../Code/tests/include/",
				"../Code/network/include/",
				"../Code/protocol/include/",
            }

            files
            {
                "../Code/tests/include/**.h",
                "../Code/tests/src/**.cpp",
            }
			
			links
			{
				"Network",
				"Protocol"
			}
			
			filter { "architecture:*86" }
                libdirs { "lib/x32" }
                targetdir ("bin/x32")

            filter { "architecture:*64" }
                libdirs { "lib/x64" }
                targetdir ("bin/x64")
		
    group ("Libraries")
        project ("Network")
            kind ("StaticLib")
            language ("C++")

            includedirs
            {
                "../Code/network/include/",
            }

            files
            {
                "../Code/network/include/**.h",
                "../Code/network/src/**.cpp",
            }

            filter { "architecture:*86" }
                libdirs { "lib/x32" }
                targetdir ("lib/x32")

            filter { "architecture:*64" }
                libdirs { "lib/x64" }
                targetdir ("lib/x64")
				
		project ("Protocol")
            kind ("StaticLib")
            language ("C++")

            includedirs
            {
                "../Code/protocol/include/",
            }

            files
            {
                "../Code/protocol/include/**.h",
                "../Code/protocol/src/**.cpp",
            }

            filter { "architecture:*86" }
                libdirs { "lib/x32" }
                targetdir ("lib/x32")

            filter { "architecture:*64" }
                libdirs { "lib/x64" }
                targetdir ("lib/x64")
