#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <map>
#include "../../include/Shader.h"
#include "../../include/Ball.h"
#include "../../include/Constraint.h"
#include "../../include/Force.h"

GLFWwindow *window;
int SCR_WIDTH = 1200;
int SCR_HEIGHT = 800;

float STEP_SIZE = 1.0f / 100.0f;
float VEL_COEF = 1.0f / 1.0f;
float GRAV_COEF = 100000.0f;
float SIMPLE_GRAVITY_COEF = 500.0f;
int JACOBIAN_COEF = 10;
float SPRING_COEF = 1.5f;
float MAX_STRETCH = 1.2f;
float MIN_STRETCH = 0.1f;
float MASS = 3.0f;
float EPS = 1.0f / 10000.0f;
int HEIGHT = 40;
int WIDTH = 40;

std::map<int, Ball*> balls;
std::vector<Constraint*> constraints;
std::vector<Force*> forces;
glm::vec2 pos_c1, pos_c2;
int ball_id;

bool pressed;
bool carrying_ball;
Ball *pressed_ball;

std::vector<float> vertices;
std::vector<unsigned int> indices;

void init();
void setup();
void processForces();
void processJacobian();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {
    init();
    setup();


    unsigned int VAO, VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);


    auto *shader = new Shader("src/glsl/shader.vert", "src/glsl/shader.frag");

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.20f, 0.19f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        processForces();
        for(int jk = 0; jk < JACOBIAN_COEF; ++jk)
            processJacobian();

        for(auto ball: balls)
            ball.second->update();

        //RENDER

        indices.clear();

        for(auto force_o : forces) {
            int id1 = force_o->id_ball_1;
            int id2 = force_o->id_ball_2;
            if(force_o->type != SPRING) continue;

            auto ball1 = balls[id1];
            auto ball2 = balls[id2];
            vertices[2 * id1] = ball1->pos.x;
            vertices[2 * id1 + 1] = ball1->pos.y;
            vertices[2 * id2] = ball2->pos.x;
            vertices[2 * id2 + 1] = ball2->pos.y;
            indices.push_back(id1);
            indices.push_back(id2);
        }


        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        shader->use();

        glm::mat4 ortho = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
        shader->setMat4("projection", ortho);

        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create a window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void setup() {
    pos_c1 = glm::vec2(-1.0f, 0.0f);
    pos_c2 = glm::vec2(-1.0f, 0.0f);
    ball_id = 0;

    float ver_pad = SCR_HEIGHT / 4.0* 3.0;
	float hor_pad = SCR_WIDTH / 4.0 * 3.0;
	float ver_dist = (SCR_HEIGHT - 2.0 * ver_pad) / (HEIGHT - 1);
	float hor_dist = (SCR_WIDTH - 2.0 * hor_pad) / (WIDTH - 1);
    auto pos = glm::vec2(0.0f);

    for(int i = 0; i < HEIGHT; ++i) {
        for(int j = 0; j < WIDTH; ++j) {

            pos.x = hor_pad + hor_dist * j;
            pos.y = SCR_HEIGHT - ver_pad - ver_dist * i;
            auto new_ball = new Ball(ball_id, pos, glm::vec2(0.0f), MASS, STEP_SIZE);

            auto fr = new Force(ball_id, SIMPLE_GRAVITY, -SIMPLE_GRAVITY_COEF);
            forces.push_back(fr);
            Constraint *con;

            if(i != 0) {
                auto ball = balls[ball_id - WIDTH];
                fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, glm::distance(new_ball->pos,ball->pos));
				forces.push_back(fr);
				con = new Constraint(ball_id, ball->id, FURTHER, glm::distance(new_ball->pos,ball->pos) * MAX_STRETCH);
				constraints.push_back(con);
				con = new Constraint(ball_id, ball->id, CLOSER, glm::distance(new_ball->pos,ball->pos) * MIN_STRETCH);
				constraints.push_back(con);
            }
            if(j != 0) {
				auto ball = balls[ball_id - 1];
				fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, glm::distance(new_ball->pos,ball->pos));
				forces.push_back(fr);
				con = new Constraint(ball_id, ball->id, FURTHER, glm::distance(new_ball->pos,ball->pos) * MAX_STRETCH);
				constraints.push_back(con);
				con = new Constraint(ball_id, ball->id, CLOSER, glm::distance(new_ball->pos,ball->pos) * MIN_STRETCH);
				constraints.push_back(con);
			}
            if(i != 0 && j != 0) {
                auto ball = balls[ball_id - 1 - WIDTH];
                fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, glm::distance(new_ball->pos,ball->pos));
                forces.push_back(fr);
                con = new Constraint(ball_id, ball->id, FURTHER, glm::distance(new_ball->pos,ball->pos) * MAX_STRETCH);
                constraints.push_back(con);
                con = new Constraint(ball_id, ball->id, CLOSER, glm::distance(new_ball->pos,ball->pos) * MIN_STRETCH);
                constraints.push_back(con);
            }
            if(i != 0 && j != WIDTH - 1) {
                auto ball = balls[ball_id + 1 - WIDTH];
                fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, glm::distance(new_ball->pos,ball->pos));
                forces.push_back(fr);
                con = new Constraint(ball_id, ball->id, FURTHER, glm::distance(new_ball->pos,ball->pos) * MAX_STRETCH);
                constraints.push_back(con);
                con = new Constraint(ball_id, ball->id, CLOSER, glm::distance(new_ball->pos,ball->pos) * MIN_STRETCH);
                constraints.push_back(con);
            }

            if(i == 0 && (j == 0 || j == WIDTH-1))
				new_ball->fixed = true;

            balls[ball_id] = new_ball;
            vertices.push_back(new_ball->pos.x);
            vertices.push_back(new_ball->pos.y);

            ball_id++;
        }
    }

}

void processForces() {
    for(auto force_o : forces) {
        int id1 = force_o->id_ball_1;
        int id2 = force_o->id_ball_2;

        if(force_o->type == SIMPLE_GRAVITY) {
            auto ball1 = balls[id1];
            auto pos1 = ball1->pos;
            float mass1 = ball1->mass;
            auto force = force_o->force(pos1, pos1, mass1, mass1);
            ball1->force += force;
        } else {
            auto ball1 = balls[id1];
            auto ball2 = balls[id2];
            auto pos1 = ball1->pos;
            auto pos2 = ball2->pos;
            float mass1 = ball1->mass;
            float mass2 = ball2->mass;
            auto force = force_o->force(pos1, pos2, mass1, mass2);
            ball1->force += force;
            ball2->force += force * -1.0f;
        }
    }
}

void processJacobian() {
    for(auto cons : constraints) {
        int id1 = cons->id_ball_1;
        int id2 = cons->id_ball_2;

        auto ball1 = balls[id1];
        auto ball2 = balls[id2];
        auto pos1 = ball1->pos;
        auto pos2 = ball2->pos;
        auto disp = cons->displacement(pos1, pos2);

        switch(cons->type) {
            case CLOSER:
                if(glm::length(disp) > EPS) {
                    if(ball1->fixed)
                        ball2->pos -= disp * 2.0f;
                    else if(ball2->fixed)
                        ball1->pos += disp * 2.0f;
                    else {
                        ball1->pos += disp;
                        ball2->pos -= disp;
                    }
                }
                break;
            case FURTHER:
                if(glm::length(disp) > EPS) {
                    if(ball1->fixed)
                        ball2->pos -= disp * 2.0f;
                    else if(ball2->fixed)
                        ball1->pos += disp * 2.0f;
                    else {
                        ball1->pos += disp;
                        ball2->pos -= disp;
                    }
                }
                break;
            default:
                break;
        }
    }
}
