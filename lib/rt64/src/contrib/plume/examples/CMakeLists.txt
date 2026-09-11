set(CMAKE_CXX_STANDARD 17)

# Enable SDL Vulkan integration
set(PLUME_SDL_VULKAN_ENABLED ON CACHE BOOL "Enable SDL Vulkan integration" FORCE)

# Find SDL2 (required for examples)
find_package(SDL2 REQUIRED)

# Initialize shader compilation
include(${CMAKE_SOURCE_DIR}/examples/cmake/PlumeShaders.cmake)
plume_shaders_init()

# Add example subdirectories
add_subdirectory(triangle)
add_subdirectory(cube)
