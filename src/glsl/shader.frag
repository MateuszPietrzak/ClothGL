#version 330 core

uniform int width;
uniform int height;

in vec2 uv;

out vec4 frag_col;

void main() {
    frag_col = vec4(uv.x / width, uv.y / height, 0.5, 0.0);
}