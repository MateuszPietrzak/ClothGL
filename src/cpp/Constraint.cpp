//
// Created by mateuszp on 11/29/22.
//

#include "../../include/Constraint.h"

Constraint::Constraint(int _id_ball_1, int _id_ball_2, ConstraintType _type, float _dist):
    id_ball_1(_id_ball_1),
    id_ball_2(_id_ball_2),
    type(_type),
    dist(_dist) {}

glm::vec2 Constraint::displacement(glm::vec2 pos1, glm::vec2 pos2) {
    auto disp = glm::vec2(0.0f);
    auto vector = glm::vec2(0.0f);
    float mag;

    // with normalize 28-29 fps
    // without length 38 fps
    // without normalize 41 fps
    // without both 46 fps
    switch(type) {
        case CLOSER:
            vector = pos1 - pos2;
            if(glm::length2(vector) > dist * dist)
                return disp;
            mag = (dist - glm::length(vector)) / 2.0f;
            disp = glm::normalize(vector) * mag;
        case FURTHER:
            vector = pos1 - pos2;
            if(glm::length2(vector) < dist * dist)
                return disp;
            mag = (dist - glm::length(vector)) / 2.0f;
            disp = glm::normalize(vector) * mag;
    }

    return disp;
}
