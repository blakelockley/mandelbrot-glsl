#include "text.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static int has_init = 0;
static GLuint program;
static GLuint texture;
static GLuint char_sampler_location;
static GLuint vao, ebo, vbo[2];

static unsigned char* image;

extern int width, height;

float posCoords[] = {
    -0.9f, 1.0f, // top right
    -0.9f, 0.9f, // bottom right
    -1.0f, 0.9f, // bottom left
    -1.0f, 1.0f, // top left
};

float texCoords[] = {
    1.0f, 1.0f, // top right
    1.0f, 0.0f, // bottom right
    0.0f, 0.0f, // bottom left
    0.0f, 1.0f, // top left
};

int indicies[6] = {
    3, 0, 2,
    0, 1, 2
};

void init_text()
{

    // Check and set init flag
    if (has_init)
        return;
    has_init = 1;

    // Shader
    const char *const vertex_shader_text = load_file("src/text_vert.glsl");
    const char *const fragment_shader_text = load_file("src/text_frag.glsl");

    GLuint vertex_shader, fragment_shader;

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

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glGenBuffers(2, vbo);

    // vPos
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(posCoords[0]) * 2, (void *)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posCoords), posCoords, GL_STREAM_DRAW);

    // vTex
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(texCoords[0]) * 2, (void *)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Load character image;

    int w, h, n;
    image = stbi_load("assets/characters.png", &w, &h, &n, STBI_rgb_alpha);

    glGenTextures(1, &texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei( GL_UNPACK_ROW_LENGTH, w );
    
    //stbi_image_free(image);
}

void render_text(char *text, vec2 pos)
{
    const float scale = 4;

    const float char_width = 8.0f / width * scale;
    const float char_height = 8.0f / height * scale;

    glUseProgram(program);
    glBindVertexArray(vao);

    for (int i = 0; i < strlen(text); i++)
    {
        // Set quad position

        // right, top
        posCoords[0] = pos[0] + char_width;
        posCoords[1] = pos[1];

        // right, bottom
        posCoords[2] = pos[0] + char_width;
        posCoords[3] = pos[1] + char_height;

        // left, bottom
        posCoords[4] = pos[0];
        posCoords[5] = pos[1] + char_height;

        // left, top
        posCoords[6] = pos[0];
        posCoords[7] = pos[1];

        // Set texture position
        const char ch = text[i] - 32;
        const int xpos = (ch % 16) * 8;
        const int ypos = (ch / 16) * 8;

        glPixelStorei(GL_UNPACK_SKIP_PIXELS, xpos);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, ypos);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(posCoords), posCoords, GL_STREAM_DRAW);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);

        pos[0] += char_width;
    }

}