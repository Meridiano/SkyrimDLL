cmake_minimum_required(VERSION 3.21)

project(
	ShowSleepWaitMenu
	VERSION 1.0.1
	LANGUAGES CXX
)

set(PROJECT_AUTHOR "Meridiano")
set(PROJECT_LICENSE "MIT License")
set(PROJECT_DESCRIPTION "Show Sleep-Wait Menu SKSE DLL")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(SkyrimPlugin)

find_package(magic_enum CONFIG REQUIRED)
find_package(xbyak CONFIG REQUIRED)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CLIB_UTIL_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	xbyak::xbyak
	magic_enum::magic_enum
)

# https://gitlab.kitware.com/cmake/cmake/-/issues/24922#note_1371990
if(MSVC_VERSION GREATER_EQUAL 1936 AND MSVC_IDE)
	file(CONFIGURE OUTPUT "${CMAKE_BINARY_DIR}/Directory.Build.props" CONTENT [==[
<Project>
  <ItemDefinitionGroup>
    <ClCompile>
      <BuildStlModules>false</BuildStlModules>
    </ClCompile>
  </ItemDefinitionGroup>
</Project>
]==] @ONLY)
endif()
