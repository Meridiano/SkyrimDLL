-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("BakaWorldMapFOV")
set_version("1.0.0")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
set_config("mode", "releasedbg")

-- add libs
add_requires("simpleini")

-- targets
target("BakaWorldMapFOV")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("simpleini")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "BakaWorldMapFOV",
        author = "shad0wshayd3-Meridiano",
        description = "Extended World Map FOV SKSE64 Plugin"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
