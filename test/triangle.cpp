#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>

#define __GLW_ENABLE_CHECKING
#define __GLW_ENABLE_EXCEPTIONS
#include "../glw.hpp"

static const char* v_shader = 
    "#version 330\n"
    "in vec3 v_position;"
    "in vec3 v_color;"
    "out vec4 f_color;"
    "void main() {"
    "   f_color = vec4(v_color, 1);"
    "   gl_Position = vec4(v_position, 1.0);"
    "}";

static const char* f_shader = 
    "#version 330\n"
    "in vec4 f_color;"
    "out vec4 gl_Color;"
    "void main() {"
    "   gl_Color = f_color;"
    "}";

int main()
{
    glfwInit();
    glfwOpenWindow(500,500, 0,0,0,0,0,0, GLFW_WINDOW);
    glewInit();
    
    try {
        // Context.
        glw::Context context;
        
        // Shader program.
        glw::Program::Shaders shaders = {
            glw::Shader(GL_VERTEX_SHADER, v_shader),
            glw::Shader(GL_FRAGMENT_SHADER, f_shader), };
        glw::Program program(shaders);
        
        if(program.build() == GL_FALSE) {
            std::cerr << program.log() << "\n";
            return 0;
        }
        
        // Data buffers.
        float positions[] = { -1,-1,+0, +0,+1,+0, +1,-1,+0 };
        float colors[] = { 1,0,0, 0,1,0, 0,0,1 };
        glw::Buffer p_buffer(GL_STATIC_DRAW, sizeof(positions), positions);
        glw::Buffer c_buffer(GL_STATIC_DRAW, sizeof(colors), colors);
        
        // Rendering.
        while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC)) {
            // Draw a triangle.
            program.setAttribute("v_position", p_buffer);
            program.setAttribute("v_color", c_buffer);
            
            context.clear(GL_COLOR_BUFFER_BIT);
            context.drawArrays(program, GL_TRIANGLES, 0, 3);
            
            glfwSwapBuffers();
        }
    } catch(const glw::Error& e) {
        std::cerr 
            << e.what()
            << " : "
            << glw::errString(e.err())
            << " ("
            << e.err()
            << ")\n";
    }

    return 0;
}
