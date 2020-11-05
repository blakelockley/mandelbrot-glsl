#version 400 core

#define mult(a, b) vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x) 
#define dist(c) sqrt(c.x * c.x + c.y * c.y) 

const int MAX_ITER = 20;
const vec3 COLOUR_A = vec3(1, 0.5, 1);
const vec3 COLOUR_B = vec3(0.5, 0, 0);

uniform vec2 pos;
uniform float zoom;

uniform float width;
uniform float height;

uniform vec2 c = vec2(0.285, 0.01);

out vec4 color;

void main()
{
    vec2 px = gl_FragCoord.xy;
    double x = px.x / width * 2 - 1;
    double y = px.y / height;

    vec2 z = vec2(-2.5 / zoom + x * (4 / zoom ), (-2 / zoom) + y * (4 / zoom));

    int n = 0;
    while (n < MAX_ITER && dist(z) <= 2) {
         z = mult(z, z) + c;
         n += 1;
   }

   float a = (1.0f * n) / MAX_ITER;

   color = vec4(mix(COLOUR_A, COLOUR_B, a), 1.0f);
}
