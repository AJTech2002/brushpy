cmake_minimum_required(VERSION 3.3)
project(BrushPY)

enable_language(CXX)
enable_language(C)
enable_language(OBJC)
enable_language(OBJCXX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



# Setup Includes by recursively including all h files in include/
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "include/*.h")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link all files recursively in src/ (C/CPP/M/MM)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.[cm]" "src/*.cpp" "src/*.m" "src/*.mm")
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "src/*.metal")

add_executable(BrushPY MACOSX_BUNDLE ${SOURCE_FILES} ${HEADER_FILES})
target_sources(BrushPY PRIVATE ${SOURCE_FILES})

# Metal shaders are only compiled by the Xcode generator
if(CMAKE_GENERATOR STREQUAL "Xcode")
    target_sources(BrushPY PRIVATE ${SHADER_FILES})
endif()

target_link_libraries(BrushPY
    "-framework Foundation"
    "-framework AppKit"
    "-framework CoreGraphics"
    "-framework CoreImage"
    "-framework QuartzCore"
    "-framework Metal"
    "-framework CoreVideo"
)

# output
set_target_properties(BrushPY PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    # Metal shader debug info (Xcode only)
    XCODE_ATTRIBUTE_MTL_ENABLE_DEBUG_INFO "INCLUDE_SOURCE"
    XCODE_ATTRIBUTE_MTL_FAST_MATH "NO"
    XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym"
    XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug] "0"
)
