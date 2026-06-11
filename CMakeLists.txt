cmake_minimum_required(VERSION 3.15)
project(computergrafik_unterwasser_simulation C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
        src/haupt.cpp
        src/werkzeuge/renderWerkzeuge.cpp
        src/werkzeuge/renderWerkzeuge.h
        src/werkzeuge/ShaderLoader.cpp
        src/werkzeuge/ShaderLoader.h
        src/werkzeuge/textur.cpp
        src/werkzeuge/textur.h
        src/werkzeuge/objload.h
        src/werkzeuge/wesen.cpp
        src/werkzeuge/wesen.h
        src/werkzeuge/transform.cpp
        src/werkzeuge/transform.h
        src/werkzeuge/renderer.cpp
        src/werkzeuge/renderer.h
        src/dieWesen/uboot.cpp
        src/dieWesen/uboot.h
        src/dieWesen/ui/fadenkreuz.cpp
        src/dieWesen/ui/fadenkreuz.h
        src/werkzeuge/kamera.cpp
        src/werkzeuge/kamera.h
        src/szenes/unterwasserszene.cpp
        src/szenes/unterwasserszene.h
        src/werkzeuge/shaderManager.cpp
        src/werkzeuge/shaderManager.h
        src/werkzeuge/skyboxHelper.h
        src/dieWesen/wasser.cpp
        src/dieWesen/wasser.h
        src/dieWesen/earth.cpp
        src/dieWesen/earth.h
        src/dieWesen/moon.cpp
        src/dieWesen/moon.h
        src/werkzeuge/visual/lightManager.cpp
        src/werkzeuge/visual/lightManager.h
        src/dieWesen/ubootRotor.cpp
        src/dieWesen/ubootRotor.h
        src/dieWesen/particleEmitter.cpp
        src/dieWesen/particleEmitter.h
        src/dieWesen/timer.cpp
        src/dieWesen/timer.h
        src/dieWesen/oceanFloor.cpp
        src/dieWesen/oceanFloor.h
        src/dieWesen/ubootHeadlight.cpp
        src/dieWesen/ubootHeadlight.h
        src/werkzeuge/visual/tween.cpp
        src/werkzeuge/visual/tween.h
        src/werkzeuge/physics/raycast.cpp
        src/werkzeuge/physics/raycast.h
        src/werkzeuge/ui/wesenUI.cpp
        src/werkzeuge/ui/wesenUI.h
        src/werkzeuge/ui/uiLabel.cpp
        src/werkzeuge/ui/uiLabel.h
        src/werkzeuge/ui/worldspaceUI.cpp
        src/werkzeuge/ui/worldspaceUI.h
        src/werkzeuge/ui/uiContainers.cpp
        src/werkzeuge/ui/uiContainers.h
        src/dieWesen/ui/hudPanel.cpp
        src/dieWesen/ui/hudPanel.h
        src/werkzeuge/groupManager.cpp
        src/werkzeuge/groupManager.h
        src/werkzeuge/input.cpp
        src/werkzeuge/input.h
        src/werkzeuge/textureManager.cpp
        src/werkzeuge/textureManager.h
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
        src/dieWesen/jellyfish.h
        src/dieWesen/jellyfish.cpp
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
        external/glew-2.0.0/include/GL
        external/freeglut/include/GL
)

if(WIN32)
    message(STATUS "Configuring for Windows...")

    set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.3.8.bin.WIN64")
    set(GLEW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glew-2.0.0")
    set(FREEGLUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/freeglut")
    set(ASSIMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/assimp")

    target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${GLFW_DIR}/include
            ${GLEW_DIR}/include
            ${ASSIMP_DIR}/include
            ${FREEGLUT_DIR}/include
    )

    target_link_directories(${PROJECT_NAME} PRIVATE
            ${GLFW_DIR}/lib-mingw-w64
            ${GLEW_DIR}/lib/Release/x64
            ${FREEGLUT_DIR}/lib/x64
            ${ASSIMP_DIR}/lib-mingw
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            glew32s
            glfw3
            assimp
            freeglut
            opengl32
            gdi32
    )


    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/external/dlls/libassimp-6.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/external/dlls/zlib.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/external/freeglut/bin/x64/freeglut.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/external/dlls/libminizip-1.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

else()
    message(STATUS "Configuring for Non-Windows OS...")

    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")

    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(assimp REQUIRED)
    find_package(GLUT REQUIRED)

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
                ${GLUT_LIBRARIES}
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
                ${GLUT_LIBRARIES}
        )
    endif()
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)