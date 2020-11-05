#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLCOREARB

#include <GLFW/glfw3.h>
  
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "loader.h"
#include "linmath.h"

vec2 pos = { 0, 0 };
vec2 vec = { 0, 0 };

vec2 c = { 0.285, 0.01 };

float zoom = 1.0f;
float zoom_direction = 0.0f;

int width = 1280;
int height = 640;

struct timespec lastm = { 0 }, currentm;
struct stat buffer;

static struct
{
    float x, y;
} vertices[8] =
{
    {  0,  1.0f }, // top right
    {  0, -1.0f }, // bottom right
    { -1, -1.0f }, // bottom left
    { -1,  1.0f },  // top left
    
    // Second triangle
    {  1,  1.0f }, // top right
    {  1, -1.0f }, // bottom right
    {  0, -1.0f }, // bottom left
    {  0,  1.0f }  // top left
};

static int indicies[12] = {
    3, 0, 2,
    0, 1, 2,

    7, 4, 6,
    4, 5, 6
};
 
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    c[0] = xpos / (width / 2) * 2 - 1;
    c[1] = ypos / height * -2 + 1;
}

GLuint load_shader(char *frag_filename) {

    const char * const vertex_shader_text = load_file("src/vert.glsl");
    const char * const fragment_shader_text = load_file(frag_filename);

    GLuint vertex_shader, fragment_shader, program;

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

    free((void *) vertex_shader_text);
    free((void *) fragment_shader_text);

    return program;
}
 
int main(void)
{
    GLFWwindow* window;
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
    
    program = load_shader("src/frag_m.glsl");
    program_j = load_shader("src/frag_j.glsl");

    stat("src/frag_m.glsl", &buffer);
    lastm = buffer.st_mtimespec;
 
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
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniform2fv(pos_location, 1, pos);
        glUniform1f(zoom_location, zoom);
        glUniform1f(width_location, (float) width);
        glUniform1f(height_location, (float) height);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  
        glUseProgram(program_j);
        glUniform2fv(j_pos_location, 1, pos);
        glUniform1f(j_zoom_location, 1.0f);
        glUniform1f(j_width_location, (float) width);
        glUniform1f(j_height_location, (float) height);
        glUniform2fv(j_c_location, 1, c);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *) (6 * sizeof(int)));
 
        glfwSwapBuffers(window);
        glfwPollEvents();

        stat("src/frag_m.glsl", &buffer);
        currentm = buffer.st_mtimespec;

        if (currentm.tv_sec > lastm.tv_sec) {
            printf("Reloading fragment shader...\n");

            program = load_shader("src/frag_m.glsl");
            lastm = currentm;
        }
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
 