-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("SkyRegEx")
set_version("1.2.0")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set policies
set_policy("package.requires_lock", true)

-- add libs
add_requires("pcre2", { configs = { bitwidth = 16 } })

-- set configs
set_config("skyrim_vr", true)
set_config("mode", "releasedbg")

-- targets
target("SkyRegEx")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("pcre2")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "SkyRegEx",
        author = "Meridiano",
        description = "Skyrim Regular Expressions DLL"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
