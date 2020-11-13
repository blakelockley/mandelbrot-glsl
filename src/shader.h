/*
 * Utility functions for loading and managing OpenGL shaders.
*/

#ifndef _SHADER_H
#define _SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>

#include <complex.h>

char *load_file(char *filename);
GLuint load_shader(char *vert_filename, char *frag_filename);

#endif