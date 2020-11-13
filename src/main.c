#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shader.h"
#include "text.h"
#include "linmath.h"

vec2 pos = {0, 0};
vec2 vec = {0, 0};

vec2 c = {0, 0};

float zoom = 1.0f;
float zoom_direction = 0.0f;

int width = 1280;
int height = 640;

static struct
{
    float x, y;
} vertices[8] =
    {
        {0, 1.0f},   // top right
        {0, -1.0f},  // bottom right
        {-1, -1.0f}, // bottom left
        {-1, 1.0f},  // top left

        // Second triangle
        {1, 1.0f},  // top right
        {1, -1.0f}, // bottom right
        {0, -1.0f}, // bottom left
        {0, 1.0f}   // top left
};

static int indicies[12] = {
    3, 0, 2,
    0, 1, 2,

    7, 4, 6,
    4, 5, 6};

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        vec[0] = -1 / zoom;

    else if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
        vec[0] = 0;

    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        vec[0] = 1 / zoom;

    else if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
        vec[0] = 0;

    else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        vec[1] = 1 / zoom;

    else if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
        vec[1] = 0;

    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        vec[1] = -1 / zoom;

    else if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
        vec[1] = 0;

    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        zoom_direction = 1;

    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        zoom_direction = 0;

    else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
        zoom_direction = -1;

    else if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE)
        zoom_direction = 0;
}

static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    c[0] = xpos / (width / 2) * 2 - 1;
    c[1] = ypos / height * -2 + 1;
}

int main(void)
{
    GLFWwindow *window;
    GLint pos_location, zoom_location, width_location, height_location;
    GLint j_pos_location, j_zoom_location, j_width_location, j_height_location, j_c_location;
    GLuint program, program_j;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);

    window = glfwCreateWindow(width, height, "Mandelbrot Set", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    unsigned int vao, ebo, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // vPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);

    program = load_shader("src/vert.glsl", "src/frag_m.glsl");
    program_j = load_shader("src/vert.glsl", "src/frag_j.glsl");

    pos_location = glGetUniformLocation(program, "pos");
    zoom_location = glGetUniformLocation(program, "zoom");
    width_location = glGetUniformLocation(program, "width");
    height_location = glGetUniformLocation(program, "height");

    // Julia set
    j_pos_location = glGetUniformLocation(program_j, "pos");
    j_zoom_location = glGetUniformLocation(program_j, "zoom");
    j_width_location = glGetUniformLocation(program_j, "width");
    j_height_location = glGetUniformLocation(program_j, "height");
    j_c_location = glGetUniformLocation(program_j, "c");

    init_text();

    char str[64];
    double current_time, delta;
    double previous_time = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        current_time = glfwGetTime();
        delta = current_time - previous_time;
        previous_time = current_time;

        pos[0] += vec[0] * delta;
        pos[1] += vec[1] * delta;

        if (zoom_direction)
            zoom += delta * zoom * zoom_direction;

        if (zoom < 1)
            zoom = 1;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        glUseProgram(program);
        glUniform2fv(pos_location, 1, pos);
        glUniform1f(zoom_location, zoom);
        glUniform1f(width_location, (float)width);
        glUniform1f(height_location, (float)height);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(program_j);
        glUniform2fv(j_pos_location, 1, pos);
        glUniform1f(j_zoom_location, 1.0f);
        glUniform1f(j_width_location, (float)width);
        glUniform1f(j_height_location, (float)height);
        glUniform2fv(j_c_location, 1, c);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)(6 * sizeof(int)));

        vec2 textPos = {-1, -0.98};
        sprintf(str, "(%f, %fi)  Zoom: %f", c[0], c[1], zoom);

        render_text(str, textPos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    deinit_text();
    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
