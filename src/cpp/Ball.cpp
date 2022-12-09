//
// Created by mateuszp on 11/29/22.
//

#include "../../include/Ball.h"

Ball::Ball(int _id, Vector3 _pos, Vector3 _vel, float _mass, float _step = 0) :
    id(_id),
    pos(_pos),
    mass(_mass),
    force(Vector3()),
    size(pow(mass, 1.0f/3.0f) * 10.0f),
    fixed(false),
    temp_fixed(false) {
    prev_pos = pos + (_vel * _step);
}

void Ball::update(float _step) {
    if(fixed || temp_fixed) {
        force = Vector3();
        return;
    }
    auto acceleration = force / mass;
    auto temp = pos;
    pos *= 2.0f;
    pos -= prev_pos;
    pos += acceleration * _step * _step;
    prev_pos = temp;
    force = Vector3();
}
