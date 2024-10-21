-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("CameraFollows")
set_version("1.5.7")
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
add_requires("nlohmann_json")

-- set configs
set_config("skyrim_vr", false)
set_config("mode", "releasedbg")

-- targets
target("CameraFollows")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("nlohmann_json")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "CameraFollows",
        author = "Meridiano",
        description = "Camera Follows SKSE Plugin"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
