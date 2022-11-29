//
// Created by mateuszp on 11/3/22.
//

#ifndef OPENGLTEMPLATE_SHADER_H
#define OPENGLTEMPLATE_SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

class Shader {
public:
    Shader(const char* vertex_shader_path, const char* fragment_shader_path);

    void use();

    void setBool(const std::string &name, bool val);
    void setInt(const std::string &name, int val);
    void setFloat(const std::string &name, float val);
    void setMat4(const std::string &name, glm::mat4 &val);
    void setVec3(const std::string &name, glm::vec3 &val);
private:
    unsigned int ID;
};


#endif //OPENGLTEMPLATE_SHADER_H
