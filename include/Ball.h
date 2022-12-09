//
// Created by mateuszp on 11/29/22.
//

#ifndef CLOTHGL_BALL_H
#define CLOTHGL_BALL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include "Vector3.h"

class Ball {
public:
    Ball(int _id, Vector3 _pos, Vector3 _vel, float _mass, float _step);
    void update(float _step);

    Vector3 pos, prev_pos, force;
    float mass, size;
    int id;
    bool fixed, temp_fixed;
};


#endif //CLOTHGL_BALL_H
