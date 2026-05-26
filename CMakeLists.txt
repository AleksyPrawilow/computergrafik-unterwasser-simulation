cmake_minimum_required(VERSION 3.15)
project(computergrafik_unterwasser_simulation C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        src/haupt.cpp
        src/Kamera.cpp
        src/Kamera.h
        src/Kiste.cpp
        src/Render_Utils.cpp
        src/Render_Utils.h
        src/Shader_Loader.cpp
        src/Shader_Loader.h
        src/Texture.cpp
        src/Texture.h
        src/obenwasser.hpp
        src/objload.h
)

set(SOIL_SOURCES
        src/SOIL/SOIL.c
        src/SOIL/SOIL.h
        src/SOIL/image_DXT.c
        src/SOIL/image_DXT.h
        src/SOIL/image_helper.c
        src/SOIL/image_helper.h
        src/SOIL/stb_image_aug.c
        src/SOIL/stb_image_aug.h
        src/SOIL/stbi_DDS_aug.h
        src/SOIL/stbi_DDS_aug_c.h
)

set(IMGUI_SOURCES
        external/imgui/imgui.cpp
        external/imgui/imgui.h
        external/imgui/imconfig.h
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_internal.h
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imstb_rectpack.h
        external/imgui/imstb_textedit.h
        external/imgui/imstb_truetype.h
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_glfw.h
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/backends/imgui_impl_opengl3.h
)

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${SOIL_SOURCES}
        ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/SOIL
        external/glm
        external/imgui
        external/imgui/backends
)

if(WIN32)
    message(STATUS "Configuring for Windows...")

    set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.3.8.bin.WIN64")
    set(GLEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glew-2.0.0")
    set(ASSIMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/assimp")

    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${GLFW_DIR}/include
            ${GLEW_DIR}/include
            ${ASSIMP_DIR}/include
    )

    target_link_directories(${PROJECT_NAME} PRIVATE
            ${GLFW_DIR}/lib-vc2022
            ${GLEW_DIR}/lib/Release/x64
            ${ASSIMP_DIR}/Release
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            opengl32
            glfw3
            glew32s
            assimp-vc141-mt
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/external/dlls/assimp-vc141-mt.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/external/dlls/zlib.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

else()
    message(STATUS "Configuring for Non-Windows OS...")

    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")

    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(GLUT REQUIRED) # Locates GLUT/FreeGLUT on the system

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${GLEW_INCLUDE_DIRS}
            ${GLUT_INCLUDE_DIR}
            "/opt/homebrew/include"
            "/opt/homebrew/include/GL"
            "/usr/local/include"
            "/usr/local/include/GL"
    )

    if(APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE
                OpenGL::GL
                GLEW::GLEW
                glfw
                assimp::assimp
                ${GLUT_LIBRARIES} # Links the FreeGLUT library
                "-framework Cocoa"
                "-framework IOKit"
                "-framework CoreVideo"
        )
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE
                OpenGL::GL
                GLEW::GLEW
                glfw
                assimp::assimp
                ${GLUT_LIBRARIES} # Links the FreeGLUT library
        )
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)