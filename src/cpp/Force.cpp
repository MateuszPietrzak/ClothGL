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

Vector3 Force::force(Vector3 pos1, Vector3 pos2, float mass1, float mass2) {
    auto force = Vector3();
    Vector3 vector;
    float mag_sq;

    switch(type){
        case GRAVITY:
            vector = pos1 - pos2;
            mag_sq = vector.length2();
            vector.normalize();
            force = vector * (-coeff * mass1 * mass2 / mag_sq);
            break;
        case SPRING:
            vector = pos1 - pos2;
            force = vector * (-coeff * (vector.length() - start_len));
            break;
        case SIMPLE_GRAVITY:
            force = Vector3(0.0f, -coeff * mass1);
            break;
    }

    return force;
}
