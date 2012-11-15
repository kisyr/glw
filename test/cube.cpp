#include <GL/glew.h>
#include <GL/glfw.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define __GLW_ENABLE_EXCEPTIONS
#include "../glw.hpp"

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
    "   gl_Color = texture2D(u_sampler, f_texcoord);"
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

const unsigned int indices[3*12] = {
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

const unsigned int elements = 6*6;
    
int main()
{
    glfwInit();
    glfwOpenWindow(500,500,0,0,0,0,0,0,GLFW_WINDOW);
    glewInit();
    
    try {
        // Context.
        glw::Context context;
        context.enable(GL_DEPTH_TEST);
        
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
        glw::Buffer v_buffer(GL_STATIC_DRAW, sizeof(vertices), vertices);
        glw::Buffer i_buffer(GL_STATIC_DRAW, sizeof(indices), indices);
        
        // Textures.
        float texels[] = {
            1,0,0, 1,0,0, 1,0,0, 1,0,0,
            0,1,0, 0,1,0, 0,1,0, 0,1,0,
            0,0,1, 0,0,1, 0,0,1, 0,0,1,
            1,1,0, 1,1,0, 1,1,0, 1,1,0, };
        glw::Texture2D texture(GL_RGB, GL_FLOAT, 4,4, texels);
        glw::Sampler sampler(GL_NEAREST, GL_NEAREST, GL_REPEAT);
        
        // Rendering.
        while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC)) {
            float time = glfwGetTime();
        
            glm::mat4 proj = glm::perspective(50.0f, 1.0f, 1.0f, 100.0f);
            glm::mat4 view = glm::lookAt(
                glm::vec3(sin(time)*10, 5.0f, cos(time)*10),
                glm::vec3(0.0f),
                glm::vec3(0.0f,1.0f,0.0f));
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
        
            // Draw cube.
            program.setUniform("u_mvp", proj*view*model);
            program.setAttribute("v_position", v_buffer, 0, 20);
            program.setAttribute("v_texcoord", v_buffer, 12, 20);
            program.setSampler("u_sampler", sampler, texture);
            
            context.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            context.drawElements(
                program, 
                GL_TRIANGLE_STRIP, 0, elements,
                GL_UNSIGNED_INT, i_buffer);
            
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
