#version 330 core

layout (location = 0) in vec2 input_pos;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(input_pos, 1.0,  1.0);
}