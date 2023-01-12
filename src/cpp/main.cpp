#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <vector>
#include <map>
#include "../../include/Shader.h"
#include "../../include/Ball.h"
#include "../../include/Constraint.h"
#include "../../include/Force.h"
#include "../../include/Texture.h"


GLFWwindow *window;
int SCR_WIDTH = 1200;
int SCR_HEIGHT = 800;

double STEP_SIZE = 1.0f / 100.0f;
float SIMPLE_GRAVITY_COEF = 500.0f;
int JACOBIAN_COEF = 10;
float SPRING_COEF = 1.5f;
float MAX_STRETCH = 1.2f;
float MIN_STRETCH = 0.1f;
float MASS = 3.0f;
bool ADD_CLOSER = false;
float EPS = 1.0f / 10000.0f;
int HEIGHT = 100;
int WIDTH = 100;

std::vector<Ball*> balls;
std::vector<Constraint*> constraints;
std::vector<Force*> forces;
Vector3 pos_c1;
int ball_id;

bool pressed;
bool carrying_ball;
Ball *pressed_ball;

std::vector<float> vertices;
std::vector<unsigned int> indices;

float mouseX = 0.0f, mouseY = 0.0f;

int frame_count;
double mean_frame_rate;
double frame_time = 0.0, total_time = 0.0, start_frame = 0.0, last_time = 0.0, last_time_sm = 0.0, current_time = 0.0;

void init();
void setup();
void processForces();
void processJacobian();
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) ;
void processInput(GLFWwindow* window);

int main() {
    init();
    setup();


    unsigned int VAO, VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    auto *shader = new Shader("src/glsl/shader.vert", "src/glsl/shader.frag");
    unsigned int texture = Texture::createTexture("assets/image.png");

    while(!glfwWindowShouldClose(window)) {
        start_frame = last_time = glfwGetTime();
        // Something
        processInput(window);

        glClearColor(0.20f, 0.19f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // End of something
        current_time = glfwGetTime();
        frame_time = current_time - last_time;
        std::cout << "Something time: " << frame_time * 1000 << "ms" << std::endl;
        last_time = glfwGetTime();

        // -----------------------------------------------------------------------------------------------------------
        // Physics
        last_time_sm = glfwGetTime();
        // Move held ball
        if(carrying_ball)
            pressed_ball->pos = Vector3(mouseX, mouseY, 0.0f);
        current_time = glfwGetTime();
        frame_time = current_time - last_time_sm;
        std::cout << "Move 1 time: " << frame_time * 1000 << "ms" << std::endl;
        last_time_sm = glfwGetTime();

        // process forces
        processForces();
        current_time = glfwGetTime();
        frame_time = current_time - last_time_sm;
        std::cout << "Process forces time: " << frame_time * 1000 << "ms" << std::endl;
        last_time_sm = glfwGetTime();

        // process jacobian
        for(int jk = 0; jk < JACOBIAN_COEF; ++jk)
            processJacobian();
        current_time = glfwGetTime();
        frame_time = current_time - last_time_sm;
        std::cout << "Process Jacobian time: " << frame_time * 1000 << "ms" << std::endl;
        last_time_sm = glfwGetTime();

        // update balls
        if(total_time != 0) {
            STEP_SIZE = total_time * 0.6;
            STEP_SIZE = std::min(100.0 / (WIDTH * HEIGHT), STEP_SIZE);
            std::cout << "Step: " << STEP_SIZE << std::endl;
        }

        for(auto ball: balls)
            ball->update(STEP_SIZE);
        current_time = glfwGetTime();
        frame_time = current_time - last_time_sm;
        std::cout << "Update balls time: " << frame_time * 1000 << "ms" << std::endl;
        last_time_sm = glfwGetTime();


        // End of physics
        current_time = glfwGetTime();
        frame_time = current_time - last_time;
        std::cout << "Physics time: " << frame_time * 1000 << "ms" << std::endl;
        last_time = glfwGetTime();

        // -----------------------------------------------------------------------------------------------------------
        // Render
        for(auto force_o : forces) {
            int id1 = force_o->id_ball_1;
            int id2 = force_o->id_ball_2;
            if (force_o->type != SPRING) continue;

            auto ball1 = balls[id1];
            auto ball2 = balls[id2];
            vertices[4 * id1] = ball1->pos.x;
            vertices[4 * id1 + 1] = ball1->pos.y;
            vertices[4 * id2] = ball2->pos.x;
            vertices[4 * id2 + 1] = ball2->pos.y;
        }

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);


        glBindVertexArray(0);

        shader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader->setInt("tex", 0);

        glm::mat4 ortho = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
        shader->setMat4("projection", ortho);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        // End of render
        current_time = glfwGetTime();
        frame_time = current_time - last_time;
        total_time = current_time - start_frame;
        last_time = current_time;
        std::cout << "Render time: " << frame_time * 1000 << "ms" << std::endl;
        std::cout << "TOTAL FRAME time: " << total_time * 1000 << "ms; FRAME RATE: " << 1.0 / total_time << std::endl;
        frame_count ++;
        mean_frame_rate += 1.0 / total_time;
    }

    std::cout << "Mean FRAME RATE: " << mean_frame_rate / frame_count;
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
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        pos_c1 = Vector3(mouseX, mouseY);
        pressed = true;
        for(auto ball : balls) {
            if(ball->pos.dist(pos_c1) < ball->size / 2.0f) {
                ball->temp_fixed = true;
                pressed_ball = ball;
                carrying_ball = true;
            }
        }
    } else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        if(!pressed)
            return;
        pressed = false;
        if(carrying_ball) {
            pressed_ball->prev_pos = pressed_ball->pos;
            pressed_ball->temp_fixed = false;
            carrying_ball = false;
        }
    }

}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = (float)xpos;
    mouseY = (float)ypos;
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void setup() {
    pos_c1 = Vector3(-1.0f, 0.0f);
    ball_id = 0;

    float ver_pad = SCR_HEIGHT / 4.0* 3.0;
	float hor_pad = SCR_WIDTH / 4.0 * 3.0;
	float ver_dist = (SCR_HEIGHT - 2.0 * ver_pad) / (HEIGHT - 1);
	float hor_dist = (SCR_WIDTH - 2.0 * hor_pad) / (WIDTH - 1);
    auto pos = Vector3();

    balls.resize(HEIGHT*WIDTH);

    for(int i = 0; i < HEIGHT; ++i) {
        for(int j = 0; j < WIDTH; ++j) {

            pos.x = hor_pad + hor_dist * j;
            pos.y = SCR_HEIGHT - ver_pad - ver_dist * i;
            auto new_ball = new Ball(ball_id, pos, Vector3(), MASS, STEP_SIZE);

            auto fr = new Force(ball_id, SIMPLE_GRAVITY, -SIMPLE_GRAVITY_COEF);
            forces.push_back(fr);
            Constraint *con;

            if(i != 0) {
                auto ball = balls[ball_id - WIDTH];
                fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, new_ball->pos.dist(ball->pos));

				forces.push_back(fr);
				con = new Constraint(ball_id, ball->id, FURTHER, new_ball->pos.dist(ball->pos) * MAX_STRETCH);
				constraints.push_back(con);
                if(ADD_CLOSER) {
                    con = new Constraint(ball_id, ball->id, CLOSER,
                                         new_ball->pos.dist(ball->pos) * MIN_STRETCH);
                    constraints.push_back(con);
                }
            }
            if(j != 0) {
				auto ball = balls[ball_id - 1];
				fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, new_ball->pos.dist(ball->pos));
				forces.push_back(fr);
				con = new Constraint(ball_id, ball->id, FURTHER, new_ball->pos.dist(ball->pos) * MAX_STRETCH);
				constraints.push_back(con);
                if(ADD_CLOSER) {
                    con = new Constraint(ball_id, ball->id, CLOSER,
                                         new_ball->pos.dist( ball->pos) * MIN_STRETCH);
                    constraints.push_back(con);
                }
			}
            if(i != 0 && j != 0) {
                auto ball = balls[ball_id - 1 - WIDTH];
                fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, new_ball->pos.dist(ball->pos));
                forces.push_back(fr);
                con = new Constraint(ball_id, ball->id, FURTHER, new_ball->pos.dist(ball->pos) * MAX_STRETCH);
                constraints.push_back(con);
                if(ADD_CLOSER) {
                    con = new Constraint(ball_id, ball->id, CLOSER,
                                         new_ball->pos.dist( ball->pos) * MIN_STRETCH);
                    constraints.push_back(con);
                }
            }
            if(i != 0 && j != WIDTH - 1) {
                auto ball = balls[ball_id + 1 - WIDTH];
                fr = new Force(ball_id, ball->id, SPRING, SPRING_COEF, new_ball->pos.dist(ball->pos));
                forces.push_back(fr);
                con = new Constraint(ball_id, ball->id, FURTHER, new_ball->pos.dist(ball->pos) * MAX_STRETCH);
                constraints.push_back(con);
                if(ADD_CLOSER) {
                    con = new Constraint(ball_id, ball->id, CLOSER,
                                         new_ball->pos.dist( ball->pos) * MIN_STRETCH);
                    constraints.push_back(con);
                }
            }

            if((i == 0  /*|| i == HEIGHT-1*/) && (j == 0 || j == WIDTH-1))
				new_ball->fixed = true;
            balls[ball_id] = new_ball;
            vertices.push_back(new_ball->pos.x);
            vertices.push_back(new_ball->pos.y);
            vertices.push_back(1.0f - (float)j / (float)(HEIGHT-1));
            vertices.push_back(1.0f - (float)i / (float)(WIDTH-1));

            ball_id++;
        }
    }

    for(int i = 0; i < HEIGHT-1; ++i) {
        for(int j = 0; j < WIDTH-1; ++j) {
            indices.push_back(i*WIDTH+j);
            indices.push_back((i+1)*WIDTH+j);
            indices.push_back(i*WIDTH+j+1);

            indices.push_back((i+1)*WIDTH+j);
            indices.push_back((i+1)*WIDTH+j+1);
            indices.push_back(i*WIDTH+j+1);
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
    // double time_1 = 0.0, time_2 = 0.0, last_time_ss = 0.0;
    // last_time_ss = glfwGetTime();
    // all 1.06ms
    for(auto cons : constraints) {
        int id1 = cons->id_ball_1;
        int id2 = cons->id_ball_2;

        auto ball1 = balls[id1];
        auto ball2 = balls[id2];
        auto pos1 = ball1->pos;
        auto pos2 = ball2->pos;
        auto disp = cons->displacement(pos1, pos2);

        switch (cons->type) {
            case CLOSER:
                if (ball1->fixed)
                    ball2->pos -= disp * 2.0f;
                else if (ball2->fixed)
                    ball1->pos += disp * 2.0f;
                else {
                    ball1->pos += disp;
                    ball2->pos -= disp;
                }
                break;
            case FURTHER:
                if (ball1->fixed)
                    ball2->pos -= disp * 2.0f;
                else if (ball2->fixed)
                    ball1->pos += disp * 2.0f;
                else {
                    ball1->pos += disp;
                    ball2->pos -= disp;
                }
                break;
            default:
                break;
        }
    }
    /*
    current_time = glfwGetTime();
    time_2 = current_time - last_time_ss;
    std::cout << "Time 2: " << time_2 * 1000 << "ms" << std::endl;
    */
}
