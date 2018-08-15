#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define __GLW_ENABLE_EXCEPTIONS
#include "glw_buffer.hpp"
#include "glw_program.hpp"
#include "glw_texture.hpp"

static const char* v_shader = 
    "#version 330\n"
    "uniform mat4 u_mvp;"
    "in vec3 v_position;"
    "in vec2 v_texcoord;"
    "out vec2 f_texcoord;"
    "void main() {"
    "   f_texcoord = v_texcoord;"
    "   gl_Position = u_mvp * vec4(v_position, 1.0);"
    "}";

static const char* f_shader = 
    "#version 330\n"
    "uniform sampler2D u_sampler;"
    "in vec2 f_texcoord;"
    "out vec4 gl_Color;"
    "void main() {"
    "   gl_Color = vec4(0);"
    "   gl_Color += texture2D(u_sampler, f_texcoord);"
//    "   gl_Color += vec4(f_texcoord.x, f_texcoord.y, 0, 1);"
    "}";

const float vertices[5*4*2] = {
    // front
    -1, -1, -1, 0, 0, 
    -1, +1, -1, 0, 1,
    +1, -1, -1, 1, 0,
    +1, +1, -1, 1, 1,
    // back
    -1, -1, +1, 1, 1,
    -1, +1, +1, 1, 0,
    +1, -1, +1, 0, 1,
    +1, +1, +1, 0, 0, };

const GLuint indices[3*12] = {
    // front 1
    0, 1, 2,
    // front 2
    1, 3, 2,
    // right 1
    2, 3, 6,
    // right 2
    3, 7, 6,
    // back 1
    4, 5, 6,
    // back 2
    5, 7, 6,
    // left 1
    0, 1, 4,
    // left 2
    1, 5, 4,
    // top 1
    1, 5, 3,
    // top 2
    5, 7, 3,
    // bottom 1
    0, 4, 2,
    // bottom 2
    4, 6, 2 };

const GLuint elements = 6*6;

#define R 255,0,0
#define G 0,255,0
#define B 0,0,255
const GLubyte texels[] = {
    R,R,R,R,R,R,R,R,
    G,G,G,G,G,G,G,G,
    B,B,B,B,B,B,B,B,
    R,R,R,R,R,R,R,R,
    G,G,G,G,G,G,G,G,
    B,B,B,B,B,B,B,B,
    R,R,R,R,R,R,R,R,
    G,G,G,G,G,G,G,G, };

int main()
{
    GLFWwindow* window;

    glfwInit();
    window = glfwCreateWindow(500, 500, "Example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    try {
        // Shader program.
        glw::Program::Shaders shaders = {
            { GL_VERTEX_SHADER, v_shader },
            { GL_FRAGMENT_SHADER, f_shader} };
        glw::Program program(shaders);
        
        if(program.build() != GL_NO_ERROR) {
            std::cerr << program.log() << "\n";
            return 0;
        }
        
        // Data buffers.
        glw::Buffer v_buffer(GL_STATIC_DRAW, sizeof(vertices), vertices);
        glw::Buffer i_buffer(GL_STATIC_DRAW, sizeof(indices), indices);
        
        // Textures.
        glw::ImageFormat format = { GL_UNSIGNED_BYTE, GL_RGB };
        glw::Texture2D texture(GL_RGB, format, 8,8, texels);
        
        // Rendering.
        while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            float time = glfwGetTime();
        
            glm::mat4 proj = glm::perspective(50.0f, 1.0f, 1.0f, 100.0f);
            glm::mat4 view = glm::lookAt(
                glm::vec3(sin(time), 1.0f, cos(time)) * glm::vec3(10.0f),
                glm::vec3(0.0f),
                glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));

            glClearColor(.75f, .75f, .75f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture());
        
            // Draw cube.
            program.setSampler("u_sampler", 0, texture());
            program.setUniform("u_mvp", proj*view*model);
            program.setAttribute("v_position", v_buffer(), 20, 0);
            program.setAttribute("v_texcoord", v_buffer(), 20, 12);
            program.execute(GL_TRIANGLES, elements, GL_UNSIGNED_INT, i_buffer());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } catch(const glw::Error& e) {
        std::cerr
            << e.what()
            << " : "
            << glw::error_string(e.error())
            << " ("
            << e.error()
            << ")\n";
    }

    return 0;
}
