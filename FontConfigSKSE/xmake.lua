-- xmake requirement
set_xmakever("3.0.0")

-- include subprojects
includes("lib/commonlibsse-ng")

-- set project constants
set_project("FontConfigSKSE")
set_version("1.0.0")
set_license("GPL-3.0")
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set optionals
set_policy("package.requires_lock", true)
set_defaultmode("releasedbg")

-- define targets
target("FontConfigSKSE")
    add_rules("commonlibsse-ng.plugin", {
        name = "FontConfigSKSE",
        author = "Meridiano",
        description = "FontConfig Extended SKSE64 DLL"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
