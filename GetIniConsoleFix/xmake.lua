-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")
add_requires("xbyak")

-- set project
set_project("GetIniConsoleFix")
set_version("1.0.1")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set configs
set_config("skyrim_se", false)
set_config("skyrim_vr", false)
set_config("mode", "releasedbg")

-- targets
target("GetIniConsoleFix")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("xbyak")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "GetIniConsoleFix",
        author = "Meridiano",
        description = "GetIni Console Fix SKSE64 DLL"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
