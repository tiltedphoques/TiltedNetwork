function CreateNetworkProject(basePath, coreBasePath)
    project ("Network")
        kind ("StaticLib")
        language ("C++")

        includedirs
        {
            coreBasePath .. "/Code/core/include/",
            basePath .. "/Code/network/include/",
            basePath .. "/Code/protocol/include/",
        }

        files
        {
            basePath .. "/Code/network/include/**.h",
            basePath .. "/Code/network/src/**.cpp",
        }

end

function CreateProtocolProject(basePath, coreBasePath)
    project ("Protocol")
        kind ("StaticLib")
        language ("C++")

        includedirs
        {
            basePath .. "/Code/protocol/include/",
            coreBasePath .. "/Code/core/include/",
            basePath .. "/ThirdParty/cryptopp/",
        }

        files
        {
            basePath .. "/Code/protocol/include/**.h",
            basePath .. "/Code/protocol/src/**.cpp",
        }
        
        links
        {
            "Core",
            "cryptopp"
        }


end

function CreateCryptoppProject(basePath)
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
            basePath .. "/ThirdParty/cryptopp/",
        }
        
        files 
        {
            basePath .. "/ThirdParty/cryptopp/*.h",
            basePath .. "/ThirdParty/cryptopp/3way.cpp",
            basePath .. "/ThirdParty/cryptopp/adler32.cpp",
            basePath .. "/ThirdParty/cryptopp/algebra.cpp",
            basePath .. "/ThirdParty/cryptopp/algparam.cpp",
            basePath .. "/ThirdParty/cryptopp/arc4.cpp",
            basePath .. "/ThirdParty/cryptopp/asn.cpp",
            basePath .. "/ThirdParty/cryptopp/authenc.cpp",
            basePath .. "/ThirdParty/cryptopp/base32.cpp",
            basePath .. "/ThirdParty/cryptopp/base64.cpp",
            basePath .. "/ThirdParty/cryptopp/basecode.cpp",
            basePath .. "/ThirdParty/cryptopp/bfinit.cpp",
            basePath .. "/ThirdParty/cryptopp/blake2.cpp",
            basePath .. "/ThirdParty/cryptopp/blake2b_simd.cpp",
            basePath .. "/ThirdParty/cryptopp/blake2s_simd.cpp",
            basePath .. "/ThirdParty/cryptopp/blowfish.cpp",
            basePath .. "/ThirdParty/cryptopp/blumshub.cpp",
            basePath .. "/ThirdParty/cryptopp/cast.cpp",
            basePath .. "/ThirdParty/cryptopp/casts.cpp",
            basePath .. "/ThirdParty/cryptopp/cbcmac.cpp",
            basePath .. "/ThirdParty/cryptopp/ccm.cpp",
            basePath .. "/ThirdParty/cryptopp/chacha.cpp",
            basePath .. "/ThirdParty/cryptopp/chacha_avx.cpp",
            basePath .. "/ThirdParty/cryptopp/chacha_simd.cpp",
            basePath .. "/ThirdParty/cryptopp/channels.cpp",
            basePath .. "/ThirdParty/cryptopp/cmac.cpp",
            basePath .. "/ThirdParty/cryptopp/cpu.cpp",
            basePath .. "/ThirdParty/cryptopp/crc.cpp",
            basePath .. "/ThirdParty/cryptopp/cryptlib.cpp",
            basePath .. "/ThirdParty/cryptopp/default.cpp",
            basePath .. "/ThirdParty/cryptopp/des.cpp",
            basePath .. "/ThirdParty/cryptopp/dessp.cpp",
            basePath .. "/ThirdParty/cryptopp/dh.cpp",
            basePath .. "/ThirdParty/cryptopp/dh2.cpp",
            basePath .. "/ThirdParty/cryptopp/dll.cpp",
            basePath .. "/ThirdParty/cryptopp/dsa.cpp",
            basePath .. "/ThirdParty/cryptopp/eax.cpp",
            basePath .. "/ThirdParty/cryptopp/ec2n.cpp",
            basePath .. "/ThirdParty/cryptopp/eccrypto.cpp",
            basePath .. "/ThirdParty/cryptopp/ecp.cpp",
            basePath .. "/ThirdParty/cryptopp/elgamal.cpp",
            basePath .. "/ThirdParty/cryptopp/emsa2.cpp",
            basePath .. "/ThirdParty/cryptopp/eprecomp.cpp",
            basePath .. "/ThirdParty/cryptopp/esign.cpp",
            basePath .. "/ThirdParty/cryptopp/files.cpp",
            basePath .. "/ThirdParty/cryptopp/filters.cpp",
            basePath .. "/ThirdParty/cryptopp/fips140.cpp",
            basePath .. "/ThirdParty/cryptopp/gcm.cpp",
            basePath .. "/ThirdParty/cryptopp/gf256.cpp",
            basePath .. "/ThirdParty/cryptopp/gf2n.cpp",
            basePath .. "/ThirdParty/cryptopp/gf2_32.cpp",
            basePath .. "/ThirdParty/cryptopp/gfpcrypt.cpp",
            basePath .. "/ThirdParty/cryptopp/gost.cpp",
            basePath .. "/ThirdParty/cryptopp/gzip.cpp",
            basePath .. "/ThirdParty/cryptopp/hex.cpp",
            basePath .. "/ThirdParty/cryptopp/hmac.cpp",
            basePath .. "/ThirdParty/cryptopp/hrtimer.cpp",
            basePath .. "/ThirdParty/cryptopp/ida.cpp",
            basePath .. "/ThirdParty/cryptopp/idea.cpp",
            basePath .. "/ThirdParty/cryptopp/integer.cpp",
            basePath .. "/ThirdParty/cryptopp/iterhash.cpp",
            basePath .. "/ThirdParty/cryptopp/luc.cpp",
            basePath .. "/ThirdParty/cryptopp/mars.cpp",
            basePath .. "/ThirdParty/cryptopp/marss.cpp",
            basePath .. "/ThirdParty/cryptopp/md2.cpp",
            basePath .. "/ThirdParty/cryptopp/md4.cpp",
            basePath .. "/ThirdParty/cryptopp/md5.cpp",
            basePath .. "/ThirdParty/cryptopp/misc.cpp",
            basePath .. "/ThirdParty/cryptopp/modes.cpp",
            basePath .. "/ThirdParty/cryptopp/mqueue.cpp",
            basePath .. "/ThirdParty/cryptopp/mqv.cpp",
            basePath .. "/ThirdParty/cryptopp/nbtheory.cpp",
            basePath .. "/ThirdParty/cryptopp/oaep.cpp",
            basePath .. "/ThirdParty/cryptopp/osrng.cpp",
            basePath .. "/ThirdParty/cryptopp/panama.cpp",
            basePath .. "/ThirdParty/cryptopp/pch.cpp",
            basePath .. "/ThirdParty/cryptopp/pkcspad.cpp",
            basePath .. "/ThirdParty/cryptopp/polynomi.cpp",
            basePath .. "/ThirdParty/cryptopp/pssr.cpp",
            basePath .. "/ThirdParty/cryptopp/pubkey.cpp",
            basePath .. "/ThirdParty/cryptopp/queue.cpp",
            basePath .. "/ThirdParty/cryptopp/rabin.cpp",
            basePath .. "/ThirdParty/cryptopp/randpool.cpp",
            basePath .. "/ThirdParty/cryptopp/rc2.cpp",
            basePath .. "/ThirdParty/cryptopp/rc5.cpp",
            basePath .. "/ThirdParty/cryptopp/rc6.cpp",
            basePath .. "/ThirdParty/cryptopp/rdtables.cpp",
            basePath .. "/ThirdParty/cryptopp/rijndael.cpp",
            basePath .. "/ThirdParty/cryptopp/ripemd.cpp",
            basePath .. "/ThirdParty/cryptopp/rng.cpp",
            basePath .. "/ThirdParty/cryptopp/rsa.cpp",
            basePath .. "/ThirdParty/cryptopp/rw.cpp",
            basePath .. "/ThirdParty/cryptopp/safer.cpp",
            basePath .. "/ThirdParty/cryptopp/salsa.cpp",
            basePath .. "/ThirdParty/cryptopp/seal.cpp",
            basePath .. "/ThirdParty/cryptopp/sse_simd.cpp",
            basePath .. "/ThirdParty/cryptopp/seed.cpp",
            basePath .. "/ThirdParty/cryptopp/serpent.cpp",
            basePath .. "/ThirdParty/cryptopp/sha.cpp",
            basePath .. "/ThirdParty/cryptopp/sha3.cpp",
            basePath .. "/ThirdParty/cryptopp/shacal2.cpp",
            basePath .. "/ThirdParty/cryptopp/shark.cpp",
            basePath .. "/ThirdParty/cryptopp/sharkbox.cpp",
            basePath .. "/ThirdParty/cryptopp/simple.cpp",
            basePath .. "/ThirdParty/cryptopp/skipjack.cpp",
            basePath .. "/ThirdParty/cryptopp/sosemanuk.cpp",
            basePath .. "/ThirdParty/cryptopp/square.cpp",
            basePath .. "/ThirdParty/cryptopp/squaretb.cpp",
            basePath .. "/ThirdParty/cryptopp/strciphr.cpp",
            basePath .. "/ThirdParty/cryptopp/tea.cpp",
            basePath .. "/ThirdParty/cryptopp/tftables.cpp",
            basePath .. "/ThirdParty/cryptopp/tiger.cpp",
            basePath .. "/ThirdParty/cryptopp/tigertab.cpp",
            basePath .. "/ThirdParty/cryptopp/ttmac.cpp",
            basePath .. "/ThirdParty/cryptopp/twofish.cpp",
            basePath .. "/ThirdParty/cryptopp/vmac.cpp",
            basePath .. "/ThirdParty/cryptopp/wake.cpp",
            basePath .. "/ThirdParty/cryptopp/whrlpool.cpp",
            basePath .. "/ThirdParty/cryptopp/xtr.cpp",
            basePath .. "/ThirdParty/cryptopp/xtrcrypt.cpp",
            basePath .. "/ThirdParty/cryptopp/zdeflate.cpp",
            basePath .. "/ThirdParty/cryptopp/zinflate.cpp",
            basePath .. "/ThirdParty/cryptopp/zlib.cpp",
            basePath .. "/ThirdParty/cryptopp/keccak_core.cpp"
        }
        
        filter "platforms:x64"
            files {
                basePath .. "/ThirdParty/cryptopp/x64dll.asm",
                basePath .. "/ThirdParty/cryptopp/x64masm.asm"
            }
        
        filter {"system:windows"}
            linkoptions { "/ignore:4221" }
            disablewarnings { "4005" }

        filter {"system:windows", "toolset:*_xp*"}
            defines { "USE_MS_CRYPTOAPI", "_WIN32_WINNT=0x502", "NTDDI_VERSION=0x05020300" }
            
        filter {}

end