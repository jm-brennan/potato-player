cmake_minimum_required(VERSION 3.1)
project(potato-player)
include(CMakePrintHelpers)

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug)

option(USE_OPENGL_ES "Whether to use OpenGL ES" OFF)

if(USE_OPENGL_ES)
  add_definitions(-DUSE_OPENGL_ES)
endif()



# use, i.e. don't skip the full RPATH for the build tree
#set(CMAKE_SKIP_BUILD_RPATH FALSE)
 
# when building, don't use the install RPATH already
# (but later on when installing)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
 
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCES 
"${SRC_DIR}/main.cpp"
"${SRC_DIR}/miniaudio.h"

#"${SRC_DIR}/gpio.cpp"
#"${SRC_DIR}/gpio.h"
"${SRC_DIR}/utils.h"
)

#list(APPEND CMAKE_PREFIX_PATH "${LIB_DIR}/SFML-2.5.1")


# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${INCLUDE_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)
#cmake_print_variables(GLUT)
#include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
#target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )


set(GLFW_DIR "${LIB_DIR}/glfw-3.3.2")
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")


# glad
if(USE_OPENGL_ES)
    set(GLAD_DIR "${LIB_DIR}/glad_es")
    add_library("glad" "${GLAD_DIR}/src/gles2.c")
else()
    set(GLAD_DIR "${LIB_DIR}/glad")
    add_library("glad" "${GLAD_DIR}/src/gl.c")
endif()
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")


# sfml
#target_include_directories(${PROJECT_NAME} PRIVATE "${LIB_DIR}/SFML-2.5.1/include")
#target_include_directories(${PROJECT_NAME} PRIVATE "${LIB_DIR}/SFML-2.5.1/include/SFML")
#target_include_directories(${PROJECT_NAME} PRIVATE "${LIB_DIR}/SFML-2.5.1/include/SFML/Audio")
#find_library(SFML_Lib sfml-audio)

#target_link_libraries(${PROJECT_NAME} PRIVATE "${SFML_Lib}")
#find_library(FLAC_Lib FLAC)
#cmake_print_variables(FLAC_Lib)
#target_link_libraries(${PROJECT_NAME} PRIVATE "${FLAC_Lib}")

# glm
set(GLM_INCLUDE_DIR "${LIB_DIR}/glm")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLM_INCLUDE_DIR}")

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