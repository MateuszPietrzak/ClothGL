//
// Created by mateuszp on 11/29/22.
//

#include "../../include/Ball.h"

Ball::Ball(int _id, glm::vec2 _pos, glm::vec2 _vel, float _mass, float _step) :
    id(_id),
    pos(_pos),
    mass(_mass),
    step(_step),
    force(glm::vec2(0.0f)),
    size(pow(mass, 1.0f/3.0f) * 10.0f),
    fixed(false),
    temp_fixed(false) {
    prev_pos = pos + (_vel * step);
}

void Ball::update() {
    if(fixed || temp_fixed) {
        force = glm::vec2(0.0f);
        return;
    }
    auto acceleration = force / mass;
    auto temp = pos;
    pos *= 2.0f;
    pos -= prev_pos;
    pos += acceleration * step * step;
    prev_pos = temp;
    force = glm::vec2(0.0f);
}
