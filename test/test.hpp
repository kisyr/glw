#ifndef __GLW_TEST_HPP
#define __GLW_TEST_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstdio>

#define TEST_INIT test_init

#define TEST_ASSERT(Expr) assert(Expr)

void test_init()
{
    GLFWwindow* window;
    if(!glfwInit()) exit(EXIT_FAILURE);
    if(!(window = glfwCreateWindow(640, 480, "test", NULL, NULL))) exit(EXIT_FAILURE);
    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK) exit(EXIT_FAILURE);
}

#endif

