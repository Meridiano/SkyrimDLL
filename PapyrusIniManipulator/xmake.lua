-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("PapyrusIniManipulator")
set_version("1.9.7")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("package.requires_lock", true)

-- set configs
set_config("skyrim_vr", true)
set_config("mode", "releasedbg")

-- targets
target("PapyrusIniManipulator")
    -- add dependencies to target
    add_deps("commonlibsse-ng")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "PapyrusIniManipulator",
        author = "Meridiano",
        description = "SKSE64 plugin to r/w ini-files using Papyrus"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
