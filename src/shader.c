#include "shader.h"

GLuint load_shader(char *vert_filename, char *frag_filename)
{
    const char *const vertex_shader_text = load_file(vert_filename);
    const char *const fragment_shader_text = load_file(frag_filename);

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

    free((void *)vertex_shader_text);
    free((void *)fragment_shader_text);

    return program;
}

char *load_file(char *filename)
{
    long int len;
    char *buffer = NULL;

    FILE *fp;
    fp = fopen(filename, "r");

    if (!fp)
    {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    buffer = (char *)malloc(len + 1);

    if (buffer)
    {
        fseek(fp, 0, SEEK_SET);
        fread(buffer, sizeof(char), len, fp);
        buffer[len] = '\0';
    }

    return buffer;
}