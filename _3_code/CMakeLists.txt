cmake_minimum_required(VERSION 3.10)

# Default to release build if not specified
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (EMSCRIPTEN)
set(name 3)
else()
set(name _3_code)
endif()
project(${name})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DDISABLE_ENGINE_AUDIO)

include_directories(
	src/
	../miniaudio
	../nameof/include
	../zstd/lib
	../stb
	../glfw/include
	../3rd
	../xx
)

file(GLOB sources
	../xx/*.h
	src/*.cpp
	src/*.c
	src/*.h
)

if (MSVC)
	list(APPEND sources "../xx/xx.natvis")
endif()

add_executable(${name} ${sources})


source_group("" FILES ${sources})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../ FILES ${sources})

SET(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING" FORCE)
SET(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "USE_MSVC_RUNTIME_LIBRARY_DLL" FORCE)
SET(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "ZSTD_BUILD_PROGRAMS" FORCE)
SET(ZSTD_BUILD_SHARED OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_BUILD_TESTS OFF CACHE BOOL "ZSTD_BUILD_SHARED" FORCE)
SET(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL "ZSTD_LEGACY_SUPPORT" FORCE)
SET(ZSTD_MULTITHREAD_SUPPORT_DEFAULT OFF CACHE BOOL "ZSTD_MULTITHREAD_SUPPORT_DEFAULT" FORCE)
SET(ZSTD_MULTITHREAD_SUPPORT OFF CACHE BOOL "ZSTD_MULTITHREAD_SUPPORT" FORCE)
add_subdirectory(../zstd/build/cmake zstd)

if (EMSCRIPTEN)
	target_link_libraries(${name} libzstd_static)
else()
	SET(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW_BUILD_DOCS" FORCE)
	SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW_BUILD_EXAMPLES" FORCE)
	SET(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW_BUILD_TESTS" FORCE)
	SET(GLFW_INSTALL OFF CACHE BOOL "GLFW_INSTALL" FORCE)
	add_subdirectory(../glfw glfw)

	target_link_libraries(${name} glfw libzstd_static)
endif()

if (WIN32)
	target_link_libraries(${name} winmm)
endif()

if (EMSCRIPTEN)
	macro(append_linker_flags FLAGS)
	  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAGS}")
	endmacro()

	set(CMAKE_EXECUTABLE_SUFFIX ".html")

	# Link in the JS library file for support code
	append_linker_flags("--js-library ${CMAKE_CURRENT_LIST_DIR}/../xx/xx2d.js")

	# Link in to WebGL/GLES system library
	append_linker_flags("-lGL")

#	# Enable Closure compiler for aggressive JS size minification
	append_linker_flags("--closure 1")

#	# Enable aggressive MINIMAL_RUNTIME mode. ( can disable --closure 1 enable this, then disable this enable closure gen minimal .html )
#	append_linker_flags("-sMINIMAL_RUNTIME=2")

	# When marshalling C UTF-8 strings across the JS<->Wasm language boundary, favor smallest generated code size
	# rather than performance
	append_linker_flags("-sTEXTDECODER=2")

	# Require WebGL 2 support in target browser, for smallest generated code size. (pass -sMIN_WEBGL_VERSION=1 to dual-target WebGL 1 and WebGL 2)
	append_linker_flags("-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2")

	# Tell the example code in webgl.c that we are only targeting WebGL 2.
	add_definitions(-DMAX_WEBGL_VERSION=2)

	# The generated build output is only to be expected to be run in a web browser, never in a native CLI shell, or in a web worker.
	append_linker_flags("-sENVIRONMENT=web")
 
 	# Choose the oldest browser versions that should be supported. The higher minimum bar you choose, the less
 	# emulation code may be present for old browser quirks.
 	append_linker_flags("-sMIN_FIREFOX_VERSION=70")
 	append_linker_flags("-sMIN_SAFARI_VERSION=130000")
 	append_linker_flags("-sMIN_IE_VERSION=0x7FFFFFFF") # Do not support Internet Explorer at all (this is the Emscripten default, shown here for posterity)
 	append_linker_flags("-sMIN_CHROME_VERSION=80")
 
 	# Fine tuning for code size: do not generate code to abort program execution on malloc() failures, that will
 	# not be interesting here.
 	append_linker_flags("-sABORTING_MALLOC=0")
 
 	# Reduce WebGL code size: We do not need GLES2 emulation for automatic GL extension enabling
 	append_linker_flags("-sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0")
 
 	# Reduce WebGL code size: We do not need GLES2 emulation for GL extension names
 	append_linker_flags("-sGL_EXTENSIONS_IN_PREFIXED_FORMAT=0")
 
 	# Reduce WebGL code size: No need to specify the GL_VENDOR/GL_RENDERER etc. fields in format required by GLES2 spec.
 	append_linker_flags("-sGL_EMULATE_GLES_VERSION_STRING_FORMAT=0")
 
 	# Reduce WebGL code size at the expense of performance (this only has an effect in WebGL 1, practically a no-op here)
 	append_linker_flags("-sGL_POOL_TEMP_BUFFERS=0")
 
 	# Reduce WebGL code size: WebGL bindings layer should not keep track of certain WebGL
 	# errors that are only meaningful for C/C++ applications. (good to enable for release when glGetError() is not used, but disable in debug)
 	append_linker_flags("-sGL_TRACK_ERRORS=0")
 
 	# Reduce WebGL code size: do not emit code for extensions that we might not need.
 	append_linker_flags("-sGL_SUPPORT_SIMPLE_ENABLE_EXTENSIONS=0")
 
 	# Reduce code size: We do not need libc errno field support in our build output.
 	append_linker_flags("-sSUPPORT_ERRNO=0")
 
 	# Reduce code size: We do not need native POSIX filesystem emulation support (Emscripten FS/MEMFS)
 	append_linker_flags("-sFILESYSTEM=0")

	# download support
	append_linker_flags("-sFETCH")

	# big memory support
	append_linker_flags("-sALLOW_MEMORY_GROWTH=1")

	if (${CMAKE_BUILD_TYPE} MATCHES "Rel*")
 		# Optimization flag to optimize aggressively for size. (other options -Os, -O3, -O2, -O1, -O0)
 		append_linker_flags("-Oz")
	else()
		# enable catch all exceptions
		append_linker_flags("-sNO_DISABLE_EXCEPTION_CATCHING=1")
	endif()



	target_precompile_headers(${name}
		PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>
	)

	file(GLOB_RECURSE assets
		res/*.*
	)
	foreach(asset ${assets})
		file(COPY "${asset}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res/")
	endforeach()

	file(COPY "../xx/favicon.ico" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")
	file(COPY "src/index.htm" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/")

else()

	if(MSVC)

		target_link_options(${name} PUBLIC
			$<$<CONFIG:Debug>:/NODEFAULTLIB:libcmtd.lib>
			$<$<CONFIG:MinSizeRel>:/NODEFAULTLIB:libcmt.lib>
			$<$<CONFIG:Release>:/NODEFAULTLIB:libcmt.lib>
			$<$<CONFIG:RelWithDebInfo>:/NODEFAULTLIB:libcmt.lib>
		)
		
		target_compile_definitions(${name} PUBLIC
			"$<$<CONFIG:MinSizeRel>:_SECURE_SCL=0>"
			"$<$<CONFIG:MinSizeRel>:_HAS_ITERATOR_DEBUGGING=0>"
			"$<$<CONFIG:Release>:_SECURE_SCL=0>"
			"$<$<CONFIG:Release>:_HAS_ITERATOR_DEBUGGING=0>"
			"$<$<CONFIG:RelWithDebInfo>:_SECURE_SCL=0>"
			"$<$<CONFIG:RelWithDebInfo>:_HAS_ITERATOR_DEBUGGING=0>"
		)

		add_compile_options($<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
		target_compile_options(${name}
			PRIVATE $<$<PLATFORM_ID:Windows>:/std:c17>
			PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>
			PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>
		)

		set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

		set_source_files_properties(src/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
		target_compile_options(${name} PRIVATE $<$<PLATFORM_ID:Windows>:/Yupch.h>)
		
	else()

		target_precompile_headers(${name}
			PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>
		)
		
		target_compile_options(${name} PRIVATE -Wno-invalid-offsetof)

	endif()

endif()
