workspace "neat_str"
    configurations { "Default" }

os.mkdir("bin/Debug")
os.mkdir("bin/Release")
os.mkdir("obj/Debug")
os.mkdir("obj/Release")

language "C"
cdialect "gnu17"
files { "include/**.h", "src/**.c" }
includedirs { "include", "src" }

filter "action:vs*"
    buildoptions { "/std:clatest" }

project "neat_str_debug"
    targetdir "./bin/Debug"
    targetname "neat_str"
    kind "StaticLib"
    
    defines { "DEBUG" }
    symbols "On"

project "neat_str_release"
    targetdir "./bin/Release"
    targetname "neat_str"
    kind "StaticLib"
    
    defines { "NDEBUG" }
    optimize "On"

project "neat_str_debug"
    targetdir "./bin/Debug"
    targetname "neat_str"
    
    defines { "DEBUG" }
    symbols "On"

project "neat_str_release_dynamic"
    targetdir "./bin/Release"
    targetname "neat_str"
    kind "SharedLib"
    
    defines { "NDEBUG" }
    optimize "On"

project "neat_str_debug_dynamic"
    targetdir "./bin/Debug"
    targetname "neat_str"
    kind "SharedLib"
    
    defines { "NDEBUG" }
    optimize "On"
