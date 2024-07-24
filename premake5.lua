project "neat_str"
    kind "StaticLib"
    targetdir "./bin/%{cfg.buildcfg}"
    language "C"
    cdialect "gnu17"
    files { "include/**.h", "src/**.c" }