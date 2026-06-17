# --- Project ---

cmake_minimum_required(VERSION 4.3.0)

# Standalone BBE repo: lib/ and vcpkg.json live at the repo root.
# F4SE workspace:     lib/ and vcpkg.json live two levels up (mods/YourMod → workspace root).
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/lib/CommonLibF4/CMakeLists.txt")
	set(F4SE_DEPS_ROOT "${CMAKE_CURRENT_LIST_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../lib/CommonLibF4/CMakeLists.txt")
	set(F4SE_DEPS_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
else()
	message(FATAL_ERROR "CommonLibF4 not found. Clone with submodules or build from the F4SE workspace.")
endif()

if(NOT F4SE_DEPS_ROOT STREQUAL CMAKE_CURRENT_LIST_DIR)
	if(NOT DEFINED VCPKG_MANIFEST_DIR OR VCPKG_MANIFEST_DIR STREQUAL "")
		set(
			VCPKG_MANIFEST_DIR
			"${F4SE_DEPS_ROOT}"
			CACHE PATH "F4SE workspace vcpkg manifest"
			FORCE
		)
	endif()
	if(NOT DEFINED VCPKG_INSTALLED_DIR OR VCPKG_INSTALLED_DIR STREQUAL "")
		set(
			VCPKG_INSTALLED_DIR
			"${F4SE_DEPS_ROOT}/vcpkg_installed"
			CACHE PATH "F4SE workspace vcpkg installed"
			FORCE
		)
	endif()
endif()

# Change these to rename / re-version your plugin.
set(COMMONLIB_PLUGIN_NAME "BuildingBudgetExtender")
set(COMMONLIB_PLUGIN_AUTHOR "NOBODY")
set(COMMONLIB_PLUGIN_VERSION "1.1.0.0")
set(COMMONLIB_PLUGIN_LICENSE "MIT License")
set(COMMONLIB_PLUGIN_DESCRIPTION "F4SE plugin - automatically extends settlement build limits.")

string(TIMESTAMP COMMONLIB_PLUGIN_COPYRIGHT_YEAR "%Y" UTC)

project(
	"${COMMONLIB_PLUGIN_NAME}"
	VERSION "${COMMONLIB_PLUGIN_VERSION}"
	DESCRIPTION "${COMMONLIB_PLUGIN_DESCRIPTION}"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

include(GNUInstallDirs)

# --- Which Fallout 4 runtimes the plugin supports ---
# OG = pre-next-gen (1.10.163), NG = next-gen (1.10.980+), AE included, VR off.
set(COMMONLIB_RUNTIME_VR OFF)
set(COMMONLIB_RUNTIME_OG ON)
set(COMMONLIB_RUNTIME_NG ON)
set(COMMONLIB_RUNTIME_AE ON)

# --- Optional CommonLibF4 features ---
set(COMMONLIB_OPTION_FMT ON)
set(COMMONLIB_OPTION_XBYAK OFF)
set(COMMONLIB_OPTION_INI OFF)
set(COMMONLIB_OPTION_TOML OFF)
set(COMMONLIB_OPTION_JSON OFF)

# --- Add Sources ---

set(
	PCH_SOURCE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/PCH.hpp"
)

set(
	COMMONLIB_ROOT_DIR
	"${F4SE_DEPS_ROOT}/lib/CommonLibF4"
)

include("${COMMONLIB_ROOT_DIR}/cmake/GenerateXsePluginInfo.cmake")
include("${COMMONLIB_ROOT_DIR}/cmake/SetXseOutputPath.cmake")
include("${COMMONLIB_ROOT_DIR}/cmake/CopyXseOutput.cmake")

generate_xse_plugin_info("${COMMONLIB_ROOT_DIR}")

file(
	GLOB_RECURSE
	SOURCES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*"
)

list(
	REMOVE_ITEM
	SOURCES
	"${PCH_SOURCE}"
)

source_group(
	TREE
	"${CMAKE_CURRENT_SOURCE_DIR}"
	FILES
	${SOURCES}
)

source_group(
	TREE
	"${CMAKE_CURRENT_BINARY_DIR}/include"
	FILES
	"${XSE_PLUGIN_INFO_CPP_TARGET}"
)

# --- Include ---

if(NOT TARGET CommonLibF4)
	add_subdirectory("${COMMONLIB_ROOT_DIR}" CommonLibF4)
endif()

add_library(
	"${PROJECT_NAME}"
	SHARED
	${SOURCES}
	"${XSE_PLUGIN_INFO_CPP_TARGET}"
	"${WIN32_VERSION_TARGET}"
)

target_include_directories(
	"${PROJECT_NAME}"
	PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_precompile_headers(
	"${PROJECT_NAME}"
	PRIVATE
	"${PCH_SOURCE}"
)

set_xse_output_path("${PROJECT_NAME}")

# --- Link ---

target_link_libraries(
	"${PROJECT_NAME}"
	PRIVATE
	CommonLibF4::CommonLibF4
)

# --- Copy Output ---

copy_xse_copy_output("${PROJECT_NAME}")

if(XSE_OUTPUT_PATH)
	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND
			"${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/BuildingBudgetExtender.ini" "${XSE_OUTPUT_PATH}/BuildingBudgetExtender.ini"
	)

	set(MCM_CONFIG_DEST "${XSE_OUTPUT_PATH}/../../MCM/Config/BuildingBudgetExtender")
	set(MCM_CONFIG_SRC "${CMAKE_CURRENT_SOURCE_DIR}/mcm/Data/MCM/Config/BuildingBudgetExtender")

	add_custom_command(
		TARGET "${PROJECT_NAME}"
		POST_BUILD
		COMMAND
			"${CMAKE_COMMAND}" -E make_directory "${MCM_CONFIG_DEST}"
		COMMAND
			"${CMAKE_COMMAND}" -E copy_if_different "${MCM_CONFIG_SRC}/config.json" "${MCM_CONFIG_DEST}/config.json"
		COMMAND
			"${CMAKE_COMMAND}" -E copy_if_different "${MCM_CONFIG_SRC}/settings.ini" "${MCM_CONFIG_DEST}/settings.ini"
	)
endif()
