cmake_minimum_required(VERSION 3.1)
project(potato-player)

set(CMAKE_BUILD_TYPE Debug)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCES 
    "${SRC_DIR}/main.cpp"
    "${SRC_DIR}/gpio.cpp"
    "${SRC_DIR}/gpio.h"
    "${SRC_DIR}/utils.h"
)

set(INCLUDES 
)

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${INCLUDE_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)


# glm
#set(GLM_INCLUDE_DIR "${INCLUDE_DIR}/glm")
#target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_INCLUDE_DIR}")

# GLFW
#set(GLFW_DIR "${LIB_DIR}/glfw-3.3.2")
#set(GLFW_BUILD_EXAMPLES OFF)
#set(GLFW_BUILD_TESTS OFF)
#set(GLFW_BUILD_DOCS OFF)
#set(GLFW_INSTALL OFF)
#add_subdirectory("${GLFW_DIR}")
#target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
#target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
#target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
#set(GLAD_DIR "${LIB_DIR}/glad")
#add_library("glad" "${GLAD_DIR}/src/glad.c")
#target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
#target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
#target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")


# TODO can compile a much leaner version of freetype apparantly to just do ttf stuff
# freetype
#set(FREETYPE_DIR "${LIB_DIR}/freetype-2.10.2")
#add_subdirectory("${FREETYPE_DIR}")
#target_include_directories(${PROJECT_NAME} PRIVATE "${FREETYPE_DIR}/include/freetype")
#target_link_libraries(${PROJECT_NAME} "freetype" "${CMAKE_DL_LIBS}")