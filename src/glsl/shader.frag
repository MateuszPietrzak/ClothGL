#version 330 core

uniform int width;
uniform int height;


out vec4 frag_col;

void main() {
    frag_col = vec4(gl_FragCoord.x / width, gl_FragCoord.y / height, 0.5, 0.0);
}