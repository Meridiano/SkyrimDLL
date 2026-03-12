-- include subprojects
includes("lib/commonlibsse-ng")

-- set project constants
set_project("BlendedShorelines")
set_version("1.0.0")
set_license("MIT")
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- set mode and lock
set_defaultmode("releasedbg")
set_policy("package.requires_lock", true)

-- define targets
target("BlendedShorelines")
    add_rules("commonlibsse-ng.plugin", {
        name = "BlendedShorelines",
        author = "Meridiano",
        description = "Blended Shorelines Universal SKSE DLL"
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
        os.run(curl("https://raw.githubusercontent.com/metayeti/mINI/refs/heads/master/src/mini/ini.h", "lib/mini/ini.h"))
    end)