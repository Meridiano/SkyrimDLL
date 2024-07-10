set_xmakever("2.7.2")

-- project
set_project("MagExtenderNG")
set_version("1.3.0")
set_license("MIT License")
set_languages("c++20")
set_optimize("faster")
set_warnings("allextra", "error")

-- allowed
set_allowedarchs("windows|x64")
set_allowedmodes("debug", "releasedbg")

-- default
set_defaultarchs("windows|x64")
set_defaultmode("releasedbg")

-- rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- policies
set_policy("package.requires_lock", true)

-- packages
add_requires("fmt", "spdlog")
add_requires("commonlibsse-ng", { configs = { skyrim_vr = true }})

-- includes
includes("res/package.lua")

-- targets
target("MagExtenderNG")
    add_packages("fmt", "spdlog", "commonlibsse-ng")

    add_rules("@commonlibsse-ng/plugin", {
        name = "MagExtenderNG",
        author = "Grimy Bunyip & Meridiano",
        description = "SKSE64 plugin from Grimy Utilities"
    })

    add_files("src/plugin/**.cpp")
    add_headerfiles("src/plugin/**.h")
    add_includedirs("src/plugin")
    set_pcxxheader("src/plugin/pch.h")

    add_rules("mod.package", {
        ["@{target}-@{target_ver}.zip"] = {
            { "@{target_dir}", "@{target}.dll", "Data/SKSE/Plugins/" },
            { "@{target_dir}", "@{target}.pdb", "Data/SKSE/Plugins/" },
        }
    })
