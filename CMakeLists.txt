cmake_minimum_required(VERSION 3.3)
project(BrushPY)

enable_language(CXX)
enable_language(C)
enable_language(OBJC)
enable_language(OBJCXX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



# Setup Includes by recursively including all h files in include/
file(GLOB_RECURSE HEADER_FILES "include/*.h")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link all files recursively in src/ (C/CPP/M/MM)
file(GLOB_RECURSE SOURCE_FILES "src/*.[cm]" "src/*.cpp" "src/*.m" "src/*.mm")
add_executable(BrushPY ${SOURCE_FILES} ${HEADER_FILES})
target_sources(BrushPY PRIVATE ${SOURCE_FILES})

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
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
