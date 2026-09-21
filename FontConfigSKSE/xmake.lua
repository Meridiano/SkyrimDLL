-- xmake requirement
set_xmakever("3.0.0")

package("glaze_8")
    set_base("glaze")
    add_versions("v8.4.0", "4ee6f2ec68e8c763553d6a16e0d79cd51fac7c58cbd98783760f418ce82c9a91")
package_end()

-- include subprojects
includes("lib/commonlibsse-ng")
add_requires("glaze_8 v8.4.0")

-- set project constants
set_project("FontConfigSKSE")
set_version("1.1.0")
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
    add_packages("glaze_8")
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
