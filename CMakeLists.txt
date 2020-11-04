cmake_minimum_required(VERSION 3.16)
project(pbr)

set(CMAKE_CXX_STANDARD 11)

set(SCENE_SOURCES
        src/scene/scene.cpp
        src/scene/scene_object.cpp
        src/scene/sphere.cpp
        src/scene/light.cpp)

set(SOURCES
        src/system/manager/manager.cpp
        src/system/manager/primitive_manager.cpp
        src/system/manager/shader_manager.cpp
        src/system/manager/texture_manager.cpp
        src/system/opengl/primitive/full_primitive.cpp
        src/system/opengl/primitive/lines_primitive.cpp
        src/system/opengl/primitive/primitive.cpp
        src/system/opengl/shader.cpp
        src/system/opengl/texture.cpp
        src/system/camera.cpp
        src/system/input.cpp
        src/system/material.cpp
        src/system/renderer.cpp
        src/system/window.cpp
        src/util/nm_log.cpp
        src/util/nm_math.cpp
        src/util/util.cpp
        src/main.cpp)

# resource files
add_subdirectory(embedder)
embed(default_vert res/shader/default.vert)
embed(default_frag res/shader/default.frag)
embed(phong_vert res/shader/phong.vert)
embed(phong_frag res/shader/phong.frag)
embed(lines_vert res/shader/lines.vert)
embed(lines_frag res/shader/lines.frag)
embed(pbr_vert res/shader/pbr.vert)
embed(pbr_frag res/shader/pbr.frag)
embed(equirectangular_map_vert res/shader/cubemap/equirectangular_map.vert)
embed(equirectangular_map_frag res/shader/cubemap/equirectangular_map.frag)
embed(skybox_vert res/shader/skybox.vert)
embed(skybox_frag res/shader/skybox.frag)
embed(irradiance_map_vert res/shader/cubemap/irradiance_map.vert)
embed(irradiance_map_frag res/shader/cubemap/irradiance_map.frag)

embed(test_png res/tex/test.png)
embed(brick_diff_png res/tex/castle_brick_07/castle_brick_07_1k_png/castle_brick_07_diff_1k.png)
embed(brick_norm_png res/tex/castle_brick_07/castle_brick_07_1k_png/castle_brick_07_nor_1k.png)
embed(brick_ao_png res/tex/castle_brick_07/castle_brick_07_1k_png/castle_brick_07_ao_1k.png)
embed(brick_rough_png res/tex/castle_brick_07/castle_brick_07_1k_png/castle_brick_07_rough_1k.png)
embed(brick_disp_png res/tex/castle_brick_07/castle_brick_07_1k_png/castle_brick_07_disp_1k.png)
embed(cayley_interior_hdr res/tex/cayley_interior/cayley_interior_1k.hdr)
embed(studio_hdr res/tex/studio_small_03/studio_small_03_1k.hdr)

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
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${SCENE_SOURCES} ${EMBEDDED_RESOURCES})

# link glfw, glad, glm, stb
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} glad)
target_link_libraries(${CMAKE_PROJECT_NAME} glm)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/external/stb)