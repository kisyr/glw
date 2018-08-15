#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define __GLW_ENABLE_EXCEPTIONS
#include "glw_buffer.hpp"
#include "glw_program.hpp"

static const char* vsource = 
    "#version 330\n"
    "uniform float u_time;"
    "in vec3 v_position;"
    "in vec3 v_color;"
    "out vec4 f_color;"
    "void main() {"
    "   f_color = vec4(v_color * vec3(abs(sin(u_time))), 1);"
    "   gl_Position = vec4(v_position, 1.0);"
    "}";
static const char* fsource = 
    "#version 330\n"
    "in vec4 f_color;"
    "out vec4 gl_Color;"
    "void main() {"
    "   gl_Color = f_color;"
    "}";

int main()
{
    GLFWwindow* window;

    glfwInit();
    window = glfwCreateWindow(500, 500, "Example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    
    try {
        // Data buffers
        float positions[] = { -1,-1,+0, +0,+1,+0, +1,-1,+0 };
        float colors[] = { 1,0,0, 0,1,0, 0,0,1 };
        glw::Buffer p_buffer(GL_STATIC_DRAW, sizeof(positions), positions);
        glw::Buffer c_buffer(GL_STATIC_DRAW, sizeof(colors), colors);

        // Shader program
        glw::Program::Shaders shaders = {
            { GL_VERTEX_SHADER, vsource },
            { GL_FRAGMENT_SHADER, fsource } };
        glw::Program program(shaders);
        
        if(program.build() != GL_NO_ERROR) {
            std::cerr << program.log() << "\n";
            return 0;
        }

        program.setAttribute("v_position", p_buffer());
        program.setAttribute("v_color", c_buffer());

        // Rendering
        while(!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glClear(GL_COLOR_BUFFER_BIT);
            program.setUniform<float>("u_time", glfwGetTime());
            program.execute(GL_TRIANGLES, 0, 3);
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
