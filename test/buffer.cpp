#include "test.hpp"

int main()
{
    TEST_INIT();

    const int write_data[16] = { 0,0,0,1, 0,1,0,1, 1,0,0, 1,1,0,1 };
    int read_data[16];

    glw::uint_t error = GL_NO_ERROR;

    glw::Buffer buffer_a (GL_STATIC_DRAW, sizeof(write_data), write_data, &error);
    TEST_ASSERT(error == GL_NO_ERROR);

    TEST_ASSERT(buffer_a.getInfo<GL_BUFFER_USAGE>() == GL_STATIC_DRAW);
    TEST_ASSERT(buffer_a.getInfo<GL_BUFFER_SIZE>() == sizeof(write_data));

    glw::Buffer buffer_b (GL_STATIC_DRAW, sizeof(write_data), NULL, &error);
    TEST_ASSERT(error == GL_NO_ERROR);

    error = buffer_b.write(0, sizeof(write_data), write_data);
    TEST_ASSERT(error == GL_NO_ERROR);

    error = buffer_b.read(sizeof(int)*4, sizeof(int)*12, read_data);
    TEST_ASSERT(error == GL_NO_ERROR);

    TEST_ASSERT(memcmp(write_data+4, read_data, sizeof(int)*12) == 0);

    error = buffer_a.read(sizeof(int)*0, sizeof(int)*16, read_data);
    TEST_ASSERT(error == GL_NO_ERROR);

    TEST_ASSERT(memcmp(write_data, read_data, sizeof(int)*16) == 0);

    return EXIT_SUCCESS;
}

