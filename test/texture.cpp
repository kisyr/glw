#include "test.hpp"
#include "glw_texture.hpp"

int main()
{
    TEST_INIT();

    GLuint error = GL_NO_ERROR;

    #define R 255,0,0,255
    #define G 0,255,0,255
    #define B 0,0,255,255
    #define A 0,0,0,255
    const size_t data_cols = 4;
    const size_t data_rows = 4;
    GLubyte write_data[data_cols * data_rows * 4] = {
        R,R,R,R,
        G,G,G,G,
        B,B,B,B,
        A,A,A,A, };
    GLubyte read_data[data_cols * data_rows * 4] = {0};

    glw::ImageFormat format = { GL_UNSIGNED_BYTE, GL_RGBA };

    glw::Texture2D texture(GL_RGBA, format, data_cols,data_rows, NULL, &error);
    TEST_ASSERT(error == GL_NO_ERROR);

    error = texture.write(0, format, 0,0, data_cols,data_rows, write_data);
    TEST_ASSERT(error == GL_NO_ERROR);

    TEST_ASSERT(texture.getInfo<GL_TEXTURE_INTERNAL_FORMAT>(0) == GL_RGBA);
    TEST_ASSERT(texture.getInfo<GL_TEXTURE_WIDTH>(0) == data_cols);
    TEST_ASSERT(texture.getInfo<GL_TEXTURE_HEIGHT>(0) == data_rows);

    error = texture.read(0, format, 0,0, data_cols,data_rows, read_data);
    TEST_ASSERT(error == GL_NO_ERROR);

    TEST_ASSERT(memcmp(write_data, read_data, sizeof(write_data)) == 0);

    return EXIT_SUCCESS;
}

