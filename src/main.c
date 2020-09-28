#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>
  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <complex.h>

#include "linmath.h"

float pos[2] = { 0, 0 };
double zoom = 1.0f;

int width = 640;
int height = 480;

static struct
{
    float x, y;
} vertices[4] =
{
    {  1,  1.0f }, // top right
    {  1, -1.0f }, // bottom right
    { -1, -1.0f }, // bottom left
    { -1,  1.0f }  // top left
};

static int indicies[6] = {
    3, 0, 2,
    0, 1, 2
};
 
static const char* vertex_shader_text =
"#version 330 core\n"

"layout(location = 0) in vec2 vPos;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330 core\n"

"#define mult(a, b) vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x) \n"
"#define dist(c) sqrt(c.x * c.x + c.y * c.y) \n"

"uniform vec2 pos;\n"
"uniform float zoom;\n"

"uniform float width;\n"
"uniform float height;\n"

"out vec4 color;\n"

"void main()\n"
"{\n"
"    vec2 px = gl_FragCoord.xy;\n"
"    vec2 c = vec2(-2 + (px.x / width) * 3 + pos.x, -1 + (px.y / height) * 2 + pos.y);\n"

"    int n = 0;\n"
"    vec2 z = vec2(0.0f, 0.0f);\n"
"    while (n < 80 && dist(z) <= 2) {\n"
"         z = mult(z, z) + c;\n"
"         n += 1;\n"
"   }\n"

"    color = vec4(n / 80.0f, n * 2 / 40.0f, 1 - n / 80.0f, 1.0f);\n"
"}\n";
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        pos[0] -= 0.1f;
    
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        pos[0] += 0.1f;
    
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        pos[1] += 0.1f;
    
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        pos[1] -= 0.1f;
    
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        zoom += 1.0f;
}
 
int main(void)
{
    GLFWwindow* window;
    GLuint vertex_shader, fragment_shader, program;
    GLint pos_location, zoom_location, width_location, height_location;
 
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
 
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
 
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
                          sizeof(vertices[0]), (void*) 0);
    
    // vCol
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));
 
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint result = GL_FALSE;
    GLint info_log_length;
    GLchar *info_buffer;

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        info_buffer = (char *)malloc(info_log_length + 1);
        glGetShaderInfoLog(vertex_shader, info_log_length, NULL, info_buffer);

        fprintf(stderr, "Vertex %s\n", info_buffer);
        free(info_buffer);
    }

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        info_buffer = (char *)malloc(info_log_length + 1);
        glGetShaderInfoLog(fragment_shader, info_log_length, NULL, info_buffer);

        fprintf(stderr, "Fragment %s\n", info_buffer);
        free(info_buffer);
    } 

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

    if (info_log_length > 0)
    {
        info_buffer = (char *)malloc(info_log_length + 1);
        glGetProgramInfoLog(program, info_log_length, NULL, info_buffer);

        fprintf(stderr, "%s\n", info_buffer);
        free(info_buffer);
    }
 
    pos_location = glGetUniformLocation(program, "pos");
    zoom_location = glGetUniformLocation(program, "zoom");
 
    width_location = glGetUniformLocation(program, "width");
    height_location = glGetUniformLocation(program, "height");

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
 
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
  
        glUseProgram(program);
        
        glUniform2fv(pos_location, 1, pos);
        glUniform1d(zoom_location, zoom);
        glUniform1f(width_location, (float) width);
        glUniform1f(height_location, (float) height);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
 