#include "test.hpp"

int main()
{
    TEST_INIT();

    GLuint error = GL_NO_ERROR;

    const char* vsource = 
        "#version 330\n"
        "in vec2 v_position;"
        "uniform float u_time;"
        "void main() { gl_Position = vec4(v_position, u_time, 1); }";
    const char* fsource = 
        "#version 330\n"
        "out vec4 f_color;"
        "void main() { f_color = vec4(1,0,0,1); }";
    const float data[2*3] = {0};

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

    glw::Program::Shaders shaders = {
        { GL_VERTEX_SHADER, vsource },
        { GL_FRAGMENT_SHADER, fsource } };

    glw::Program program(shaders, &error);
    TEST_ASSERT(error == GL_NO_ERROR);

    error = program.build();
    if(error != GL_NO_ERROR) {
        std::cerr << "Log: " << program.log() << std::endl;
    }
    TEST_ASSERT(error == GL_NO_ERROR);

    TEST_ASSERT(program.getInfo<GL_LINK_STATUS>() == GL_TRUE);
    TEST_ASSERT(program.getInfo<GL_ACTIVE_ATTRIBUTES>() == 1);
    TEST_ASSERT(program.getInfo<GL_ACTIVE_UNIFORMS>() == 1);

    error = program.setAttribute("v_position", buffer);
    TEST_ASSERT(error == GL_NO_ERROR);

    error = program.setUniform("u_time", 0.f);
    TEST_ASSERT(error == GL_NO_ERROR);

    error = program.prepare();
    TEST_ASSERT(error == GL_NO_ERROR);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    return EXIT_SUCCESS;
}

