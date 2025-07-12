-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("FixDualWieldParrying")
set_version("1.0.0")
set_license("MIT")

-- set defaults
set_languages("c++23")
set_warnings("allextra")
set_defaultmode("releasedbg")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- targets
target("FixDualWieldParrying")
    -- add dependencies to target
    add_deps("commonlibsse-ng")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "FixDualWieldParrying",
        author = "Meridiano",
        description = "Dual Wield Parrying SKSE/NG Fix"
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
        os.run(curl("https://raw.githubusercontent.com/okdshin/PicoSHA2/refs/heads/master/picosha2.h", "lib/okdshin/picosha2.h"))
    end)