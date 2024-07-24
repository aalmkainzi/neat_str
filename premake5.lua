workspace "neat_str"
    configurations { "Debug", "Release" }

project "neat_str"
    kind "StaticLib"
    targetdir "./bin/%{cfg.buildcfg}"
    language "C"
    cdialect "gnu17"
    files { "include/**.h", "src/**.c" }
    includedirs { "include", "src" }
    
    os.mkdir("bin/Debug")
    os.mkdir("bin/Release")
    os.mkdir("obj/Debug")
    os.mkdir("obj/Release")
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"