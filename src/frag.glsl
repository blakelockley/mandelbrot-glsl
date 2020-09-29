#version 330 core

#define mult(a, b) vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x) 
#define dist(c) sqrt(c.x * c.x + c.y * c.y) 

const int MAX_ITER = 500;

uniform vec2 pos;
uniform float zoom;

uniform float width;
uniform float height;

out vec4 color;

void main()
{
    vec2 px = gl_FragCoord.xy;
    vec2 c = vec2(-2 / zoom + (px.x / width) * (3 / zoom) + pos.x, -1 / zoom + (px.y / height) * (2 / zoom) + pos.y);

    int n = 0;
    vec2 z = vec2(0.0f, 0.0f);
    while (n < MAX_ITER && dist(z) <= 2) {
         z = mult(z, z) + c;
         n += 1;
   }

   float a = (1.0f * n) / MAX_ITER;

   color = vec4(mix(vec3(3 / 255.0f, 207 / 255.0f, 252 / 255.0f), vec3(240 / 255.0f, 252 / 255.0f, 3 / 255.0f), a), 1.0f);
}
