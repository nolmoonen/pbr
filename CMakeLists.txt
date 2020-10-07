cmake_minimum_required(VERSION 3.16)
project(pbr)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
        src/main.cpp
        src/system/window.cpp src/system/window.hpp
        src/system/input.cpp src/system/input.hpp
        src/system/opengl.cpp src/system/opengl.hpp
        src/system/renderer.cpp src/system/renderer.hpp
        src/system/shader_manager.cpp src/system/shader_manager.hpp
        src/system/camera.cpp src/system/camera.hpp
        src/system/manager.cpp src/system/manager.hpp
        src/system/texture_manager.cpp src/system/texture_manager.hpp
        src/util/nm_log.cpp src/util/nm_log.hpp
        src/system/mesh_manager.cpp src/system/mesh_manager.hpp)

# resource files
add_subdirectory(embedder)
embed(default_vert res/shader/default.vert)
embed(default_frag res/shader/default.frag)
embed(instance_vert res/shader/instance.vert)
embed(phong_vert res/shader/phong.vert)
embed(phong_frag res/shader/phong.frag)
embed(lines_vert res/shader/lines.vert)
embed(lines_frag res/shader/lines.frag)
embed(pbr_vert res/shader/pbr.vert)
embed(pbr_frag res/shader/pbr.frag)

embed(test_png res/tex/test.png)
embed(brick_diff_png res/tex/castle_brick_07_1k_png/castle_brick_07_diff_1k.png)
embed(brick_norm_png res/tex/castle_brick_07_1k_png/castle_brick_07_nor_1k.png)

# build glfw, glad, and glm before adding compile options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glfw-3.3.2)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glad-0.1.33)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/glm-0.9.9.8)

# link MinGW libraries statically
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# add extra warnings
add_compile_options(-Wall -Wextra -pedantic)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${EMBEDDED_RESOURCES})

# link glfw, glad, glm, stb
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} glad)
target_link_libraries(${CMAKE_PROJECT_NAME} glm)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/external/stb)