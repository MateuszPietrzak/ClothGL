#version 330 core

layout (location = 0) in vec2 input_pos;
layout (location = 1) in vec2 input_uv;

uniform mat4 projection;

out vec2 uv;

void main() {
    gl_Position = projection * vec4(input_pos, 1.0,  1.0);
    uv = input_uv;
}