#version 330 core

uniform sampler2D tex;

in vec2 uv;

out vec4 frag_col;

void main() {
    frag_col = vec4(uv.x, uv.y, 0.5, 0.0);
//    frag_col = texture(tex, vec2(uv.x, uv.y));
}