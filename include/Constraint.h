//
// Created by mateuszp on 11/29/22.
//

#ifndef CLOTHGL_CONSTRAINT_H
#define CLOTHGL_CONSTRAINT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include "Vector3.h"

enum ConstraintType {
    CLOSER,
    FURTHER
};

class Constraint {
public:
    Constraint(int _id_ball_1, int _id_ball_2, ConstraintType _type, float _dist);
    Vector3 displacement(Vector3 pos1, Vector3 pos2);

    int id_ball_1, id_ball_2;
    ConstraintType type;
    float dist;
};


#endif //CLOTHGL_CONSTRAINT_H
