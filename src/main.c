#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>
  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <complex.h>

#include "linmath.h"

long double complex pos;
long double zoom;

static const struct
{
    float x, y;
} vertices[4] =
{
    {  1 + (1/3.0f),  1.0f }, // top right
    {  1 + (1/3.0f), -1.0f }, // bottom right
    { -1 - (1/3.0f), -1.0f }, // bottom left
    { -1 - (1/3.0f),  1.0f }  // top left
};

static int indicies[6] = {
    3, 0, 2,
    0, 1, 2
};
 
static const char* vertex_shader_text =
"#version 330 core\n"

"layout(location = 0) in vec2 vPos;\n"
"uniform mat4 mvp;\n"

"void main()\n"
"{\n"
"    gl_Position = mvp * vec4(vPos, 0.0, 1.0);\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330 core\n"
"#define mult(a, b) vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x) \n"
"#define leng(c) sqrt(c.x * c.x + c.y * c.y) \n"

"out vec4 color;\n"

"void main()\n"
"{\n"
"    vec2 pos = vec2(gl_FragCoord.x / 320 - 2.5, gl_FragCoord.y / 320 - 2);\n"
"    vec2 c = pos;\n"

"    for (int i = 0; i < 100; i++) {\n"
"         c = mult(c, c) + pos;\n"
"   }\n"

"    color = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"    if (leng(c) < 2) {\n"
"       color = vec4(c.x * c.x, c.y * c.y, 0.0f, 1.0f);\n"
"    }\n"
"}\n";
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 
int main(void)
{
    GLFWwindow* window;
    GLuint vertex_shader, fragment_shader, program;
    GLint mvp_location;
 
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);

    window = glfwCreateWindow(640, 640, "Mandelbrot Set", NULL, NULL);
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
 
    mvp_location = glGetUniformLocation(program, "mvp");

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, v, p, mvp;
 
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
 
        // Model
        mat4x4_identity(m);
        mat4x4_translate(m, 0.f, 0.f, -1.f);

        // View
        mat4x4_identity(v);
        
        // Projection
        mat4x4_identity(p);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        // Generate MVP
        mat4x4_mul(mvp, v, m);
        mat4x4_mul(mvp, p, mvp);
 
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
 