-- minimum xmake version
set_xmakever("2.7.8")

-- project
set_project("CraftingCategories")
set_version("1.1.1")
set_license("MIT License")
set_languages("c++20")
set_optimize("faster")

-- errors
set_warnings("allextra", "error")
set_exceptions("cxx")

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
add_requires("fmt 10.2.1", "spdlog 1.13.0", "jsoncpp")
add_requires("commonlibsse-ng", { configs = { skyrim_vr = true }})

-- targets
target("CraftingCategories")
    add_packages("fmt", "spdlog", "jsoncpp", "commonlibsse-ng")

    add_rules("@commonlibsse-ng/plugin", {
        name = "CraftingCategories",
        author = "Parapets & Meridiano",
        description = "NG patch for Constructible Object Custom Keyword System"
    })

    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

    -- copy build files
    after_build(function(target)
        local copy = function(env, ext)
            for _, env in pairs(env:split(";")) do
                if os.exists(env) then
                    local plugins = path.join(env, ext, "SKSE/Plugins")
                    os.mkdir(plugins)
                    os.trycp(target:targetfile(), plugins)
                    os.trycp(target:symbolfile(), plugins)
                end
            end
        end
        if os.getenv("SKYRIM_MODS_PATH") then
            copy(os.getenv("SKYRIM_MODS_PATH"), target:name())
        elseif os.getenv("SKYRIM_PATH") then
            copy(os.getenv("SKYRIM_PATH"), "Data")
        end
    end)
