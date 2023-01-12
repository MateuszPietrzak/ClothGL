//
// Created by mateuszp on 11/29/22.
//

#ifndef CLOTHGL_FORCE_H
#define CLOTHGL_FORCE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include "Vector3.h"

enum ForceType {
    GRAVITY,
    SIMPLE_GRAVITY,
    SPRING
};

class Force {
public:
    Force(int _id_ball_1, ForceType _type, float _coeff);
    Force(int _id_ball_1, int _id_ball_2, ForceType _type, float _coeff);
    Force(int _id_ball_1, int _id_ball_2, ForceType _type, float _coeff, float _start_len);

    Vector3 force(Vector3 pos1, Vector3 pos2, float mass1, float mass2);

    int id_ball_1, id_ball_2;
    ForceType type;
    float coeff;
    float start_len;

};


#endif //CLOTHGL_FORCE_H
