require("premake", ">=5.0.0-alpha10")

workspace ("DestroyerOfWorlds")

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

    group ("Applications")
        project ("Tests")
            kind ("ConsoleApp")
            language ("C++")
            
			
            includedirs
            {
                "../Code/tests/include/",
                "../Code/network/include/",
                "../Code/protocol/include/",
                "../Code/core/include/"
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
		
    group ("Libraries")
        project ("Network")
            kind ("StaticLib")
            language ("C++")

            includedirs
            {
                "../Code/core/include/",
                "../Code/network/include/",
                "../Code/protocol/include/",
                "../ThirdParty/cryptopp/",
            }

            files
            {
                "../Code/network/include/**.h",
                "../Code/network/src/**.cpp",
            }

            links
            {
                "Core",
                "cryptopp"
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
                "../Code/core/include/",
                "../ThirdParty/cryptopp/",
            }

            files
            {
                "../Code/protocol/include/**.h",
                "../Code/protocol/src/**.cpp",
            }
            
            links
            {
                "Core",
                "cryptopp"
            }

            filter { "architecture:*86" }
                libdirs { "lib/x32" }
                targetdir ("lib/x32")

            filter { "architecture:*64" }
                libdirs { "lib/x64" }
                targetdir ("lib/x64")
                
        project ("Core")
            kind ("StaticLib")
            language ("C++")

            includedirs
            {
                "../Code/core/include/",
            }

            files
            {
                "../Code/core/include/**.h",
                "../Code/core/src/**.cpp",
            }

            filter { "architecture:*86" }
                libdirs { "lib/x32" }
                targetdir ("lib/x32")

            filter { "architecture:*64" }
                libdirs { "lib/x64" }
                targetdir ("lib/x64")
    
    group("ThirdParty")
        project "cryptopp"
            language "C++"
            kind "StaticLib"
            targetname "cryptopp"
           
                
            defines 
            {
                "_WINSOCK_DEPRECATED_NO_WARNINGS",
                "_LIB",
                "CRYPTOPP_DISABLE_SSSE3"
            }
            
            includedirs
            {
                "../ThirdParty/cryptopp/",
            }
            
            files 
            {
                "../ThirdParty/cryptopp/*.h",
                "../ThirdParty/cryptopp/3way.cpp",
                "../ThirdParty/cryptopp/adler32.cpp",
                "../ThirdParty/cryptopp/algebra.cpp",
                "../ThirdParty/cryptopp/algparam.cpp",
                "../ThirdParty/cryptopp/arc4.cpp",
                "../ThirdParty/cryptopp/asn.cpp",
                "../ThirdParty/cryptopp/authenc.cpp",
                "../ThirdParty/cryptopp/base32.cpp",
                "../ThirdParty/cryptopp/base64.cpp",
                "../ThirdParty/cryptopp/basecode.cpp",
                "../ThirdParty/cryptopp/bfinit.cpp",
                "../ThirdParty/cryptopp/blake2.cpp",
                "../ThirdParty/cryptopp/blake2b_simd.cpp",
                "../ThirdParty/cryptopp/blake2s_simd.cpp",
                "../ThirdParty/cryptopp/blowfish.cpp",
                "../ThirdParty/cryptopp/blumshub.cpp",
                "../ThirdParty/cryptopp/cast.cpp",
                "../ThirdParty/cryptopp/casts.cpp",
                "../ThirdParty/cryptopp/cbcmac.cpp",
                "../ThirdParty/cryptopp/ccm.cpp",
                "../ThirdParty/cryptopp/chacha.cpp",
                "../ThirdParty/cryptopp/chacha_avx.cpp",
                "../ThirdParty/cryptopp/chacha_simd.cpp",
                "../ThirdParty/cryptopp/channels.cpp",
                "../ThirdParty/cryptopp/cmac.cpp",
                "../ThirdParty/cryptopp/cpu.cpp",
                "../ThirdParty/cryptopp/crc.cpp",
                "../ThirdParty/cryptopp/cryptlib.cpp",
                "../ThirdParty/cryptopp/default.cpp",
                "../ThirdParty/cryptopp/des.cpp",
                "../ThirdParty/cryptopp/dessp.cpp",
                "../ThirdParty/cryptopp/dh.cpp",
                "../ThirdParty/cryptopp/dh2.cpp",
                "../ThirdParty/cryptopp/dll.cpp",
                "../ThirdParty/cryptopp/dsa.cpp",
                "../ThirdParty/cryptopp/eax.cpp",
                "../ThirdParty/cryptopp/ec2n.cpp",
                "../ThirdParty/cryptopp/eccrypto.cpp",
                "../ThirdParty/cryptopp/ecp.cpp",
                "../ThirdParty/cryptopp/elgamal.cpp",
                "../ThirdParty/cryptopp/emsa2.cpp",
                "../ThirdParty/cryptopp/eprecomp.cpp",
                "../ThirdParty/cryptopp/esign.cpp",
                "../ThirdParty/cryptopp/files.cpp",
                "../ThirdParty/cryptopp/filters.cpp",
                "../ThirdParty/cryptopp/fips140.cpp",
                "../ThirdParty/cryptopp/gcm.cpp",
                "../ThirdParty/cryptopp/gf256.cpp",
                "../ThirdParty/cryptopp/gf2n.cpp",
                "../ThirdParty/cryptopp/gf2_32.cpp",
                "../ThirdParty/cryptopp/gfpcrypt.cpp",
                "../ThirdParty/cryptopp/gost.cpp",
                "../ThirdParty/cryptopp/gzip.cpp",
                "../ThirdParty/cryptopp/hex.cpp",
                "../ThirdParty/cryptopp/hmac.cpp",
                "../ThirdParty/cryptopp/hrtimer.cpp",
                "../ThirdParty/cryptopp/ida.cpp",
                "../ThirdParty/cryptopp/idea.cpp",
                "../ThirdParty/cryptopp/integer.cpp",
                "../ThirdParty/cryptopp/iterhash.cpp",
                "../ThirdParty/cryptopp/luc.cpp",
                "../ThirdParty/cryptopp/mars.cpp",
                "../ThirdParty/cryptopp/marss.cpp",
                "../ThirdParty/cryptopp/md2.cpp",
                "../ThirdParty/cryptopp/md4.cpp",
                "../ThirdParty/cryptopp/md5.cpp",
                "../ThirdParty/cryptopp/misc.cpp",
                "../ThirdParty/cryptopp/modes.cpp",
                "../ThirdParty/cryptopp/mqueue.cpp",
                "../ThirdParty/cryptopp/mqv.cpp",
                "../ThirdParty/cryptopp/nbtheory.cpp",
                "../ThirdParty/cryptopp/oaep.cpp",
                "../ThirdParty/cryptopp/osrng.cpp",
                "../ThirdParty/cryptopp/panama.cpp",
                "../ThirdParty/cryptopp/pch.cpp",
                "../ThirdParty/cryptopp/pkcspad.cpp",
                "../ThirdParty/cryptopp/polynomi.cpp",
                "../ThirdParty/cryptopp/pssr.cpp",
                "../ThirdParty/cryptopp/pubkey.cpp",
                "../ThirdParty/cryptopp/queue.cpp",
                "../ThirdParty/cryptopp/rabin.cpp",
                "../ThirdParty/cryptopp/randpool.cpp",
                "../ThirdParty/cryptopp/rc2.cpp",
                "../ThirdParty/cryptopp/rc5.cpp",
                "../ThirdParty/cryptopp/rc6.cpp",
                "../ThirdParty/cryptopp/rdtables.cpp",
                "../ThirdParty/cryptopp/rijndael.cpp",
                "../ThirdParty/cryptopp/ripemd.cpp",
                "../ThirdParty/cryptopp/rng.cpp",
                "../ThirdParty/cryptopp/rsa.cpp",
                "../ThirdParty/cryptopp/rw.cpp",
                "../ThirdParty/cryptopp/safer.cpp",
                "../ThirdParty/cryptopp/salsa.cpp",
                "../ThirdParty/cryptopp/seal.cpp",
                "../ThirdParty/cryptopp/sse_simd.cpp",
                "../ThirdParty/cryptopp/seed.cpp",
                "../ThirdParty/cryptopp/serpent.cpp",
                "../ThirdParty/cryptopp/sha.cpp",
                "../ThirdParty/cryptopp/sha3.cpp",
                "../ThirdParty/cryptopp/shacal2.cpp",
                "../ThirdParty/cryptopp/shark.cpp",
                "../ThirdParty/cryptopp/sharkbox.cpp",
                "../ThirdParty/cryptopp/simple.cpp",
                "../ThirdParty/cryptopp/skipjack.cpp",
                "../ThirdParty/cryptopp/sosemanuk.cpp",
                "../ThirdParty/cryptopp/square.cpp",
                "../ThirdParty/cryptopp/squaretb.cpp",
                "../ThirdParty/cryptopp/strciphr.cpp",
                "../ThirdParty/cryptopp/tea.cpp",
                "../ThirdParty/cryptopp/tftables.cpp",
                "../ThirdParty/cryptopp/tiger.cpp",
                "../ThirdParty/cryptopp/tigertab.cpp",
                "../ThirdParty/cryptopp/ttmac.cpp",
                "../ThirdParty/cryptopp/twofish.cpp",
                "../ThirdParty/cryptopp/vmac.cpp",
                "../ThirdParty/cryptopp/wake.cpp",
                "../ThirdParty/cryptopp/whrlpool.cpp",
                "../ThirdParty/cryptopp/xtr.cpp",
                "../ThirdParty/cryptopp/xtrcrypt.cpp",
                "../ThirdParty/cryptopp/zdeflate.cpp",
                "../ThirdParty/cryptopp/zinflate.cpp",
                "../ThirdParty/cryptopp/zlib.cpp",
                "../ThirdParty/cryptopp/keccak_core.cpp"
            }
            
            filter "platforms:x64"
                files {
                    "../ThirdParty/cryptopp/x64dll.asm",
                    "../ThirdParty/cryptopp/x64masm.asm"
                }
            
            filter {"system:windows"}
                linkoptions { "/ignore:4221" }
                disablewarnings { "4005" }

            filter {"system:windows", "toolset:*_xp*"}
                defines { "USE_MS_CRYPTOAPI", "_WIN32_WINNT=0x502", "NTDDI_VERSION=0x05020300" }
