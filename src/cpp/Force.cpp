//
// Created by mateuszp on 11/29/22.
//

#include "../../include/Force.h"

Force::Force(int _id_ball_1, ForceType _type, float _coeff) :
    id_ball_1(_id_ball_1),
    id_ball_2(-1),
    type(_type),
    coeff(_coeff),
    start_len(0.0f) {}

Force::Force(int _id_ball_1, int _id_ball_2, ForceType _type, float _coeff) :
    id_ball_1(_id_ball_1),
    id_ball_2(_id_ball_2),
    type(_type),
    coeff(_coeff),
    start_len(0.0f) {}

Force::Force(int _id_ball_1, int _id_ball_2, ForceType _type, float _coeff, float _start_len) :
    id_ball_1(_id_ball_1),
    id_ball_2(_id_ball_2),
    type(_type),
    coeff(_coeff),
    start_len(_start_len) {}

glm::vec2 Force::force(glm::vec2 pos1, glm::vec2 pos2, float mass1, float mass2) {
    auto force = glm::vec2(0.0f);
    auto vector = glm::vec2(0.0f);
    float mag_sq;

    switch(type){
        case GRAVITY:
            vector = pos1 - pos2;
            mag_sq = glm::length2(vector);
            vector = glm::normalize(vector);
            force = vector * (-coeff * mass1 * mass2 / mag_sq);
            break;
        case SPRING:
            vector = pos1 - pos2;
            force = vector * (-coeff * (glm::length(vector) - start_len));
            break;
        case SIMPLE_GRAVITY:
            force = glm::vec2(0.0f, -coeff * mass1);
            break;
    }

    return force;
}
