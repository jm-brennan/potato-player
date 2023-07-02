#pragma once

#ifdef USE_OPENGL_ES
#define GLAD_GLES2_IMPLEMENTATION
#include <glad/gles2.h>
#else
#include <glad/gl.h>
#endif