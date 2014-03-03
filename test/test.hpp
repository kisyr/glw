#ifndef __GLW_TEST_HPP
#define __GLW_TEST_HPP

#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstdio>

#define TEST_INIT test_init

#define TEST_ASSERT(Expr) assert(Expr)

void test_init()
{
    if(!glfwInit()) exit(EXIT_FAILURE);
    if(!glfwOpenWindow(640,480,0,0,0,0,0,0,GLFW_WINDOW)) exit(EXIT_FAILURE);
    if(glewInit() != GLEW_OK) exit(EXIT_FAILURE);
}

#endif

