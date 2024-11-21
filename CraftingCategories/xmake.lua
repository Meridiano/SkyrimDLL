-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("CraftingCategories")
set_version("1.1.1")
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
add_requires("jsoncpp")

-- set configs
set_config("skyrim_vr", true)
set_config("mode", "releasedbg")

-- targets
target("CraftingCategories")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("jsoncpp")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "CraftingCategories",
        author = "Parapets & Meridiano",
        description = "NG patch for Constructible Object Custom Keyword System"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")

    -- curl downloader
    local function curl(url, path)
        return format('curl -k "%s" -o "%s" --create-dirs', url, path)
    end
    on_load(function (target)
        os.run(curl("https://raw.githubusercontent.com/Tessil/ordered-map/refs/heads/master/include/tsl/ordered_hash.h", "lib/tsl/ordered_hash.h"))
        os.run(curl("https://raw.githubusercontent.com/Tessil/ordered-map/refs/heads/master/include/tsl/ordered_map.h", "lib/tsl/ordered_map.h"))
        os.run(curl("https://raw.githubusercontent.com/Tessil/ordered-map/refs/heads/master/include/tsl/ordered_set.h", "lib/tsl/ordered_set.h"))
    end)