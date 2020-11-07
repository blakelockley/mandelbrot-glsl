#ifndef _TEXT_H
#define _TEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"
#include "linmath.h"

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>

/**
 * Initialse text shader and renderer.
 */
void init_text();

/**
 * Render text to the screen at given position.
 * 
 * @param text {char *} Text to be rendered.
 * @param pos {vec2} Position in screen coords to render text.
 */
void render_text(char *text, vec2 pos);

#endif