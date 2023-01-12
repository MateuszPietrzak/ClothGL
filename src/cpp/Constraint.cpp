//
// Created by mateuszp on 11/29/22.
//

#include "../../include/Constraint.h"

Constraint::Constraint(int _id_ball_1, int _id_ball_2, ConstraintType _type, float _dist):
    id_ball_1(_id_ball_1),
    id_ball_2(_id_ball_2),
    type(_type),
    dist(_dist) {}

Vector3 Constraint::displacement(Vector3 pos1, Vector3 pos2) {
    auto disp = Vector3();
    auto vector = Vector3();
    float mag;

    // with normalize 28-29 fps
    // without length 38 fps
    // without normalize 41 fps
    // without both 46 fps
    switch(type) {
        case CLOSER:
            vector = pos1 - pos2;
            if(vector.length2() > dist * dist)
                return disp;
            mag = (dist - vector.length()) / 2.0f;
            vector.normalize();
            disp = vector * mag;
        case FURTHER:
            vector = pos1 - pos2;
            if(vector.length2() < dist * dist)
                return disp;
            mag = (dist - vector.length()) / 2.0f;
            vector.normalize();
            disp = vector * mag;
    }

    return disp;
}
