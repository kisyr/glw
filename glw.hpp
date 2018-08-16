/**
 * Copyright (c) 2012 Kim Syrjälä <kim.syrjala@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef __GLW_HPP
#define __GLW_HPP

#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#define __GLW_LAST_ERROR glw_last_error

#define __GLW_HANDLE(Call) \
    Call; \
    glw_last_error = glGetError(); \
    if(glw_last_error != GL_NO_ERROR)

namespace glw {

#ifdef __GLW_ENABLE_EXCEPTIONS
#include <exception>
class Error : public std::exception
{
public:
    Error() {}

    Error(const std::string& what__, GLuint error__)
      : what_(what__),
        error_(error__) {}

    ~Error() throw() {}

    const GLchar* what() const throw() { return what_.c_str(); }
    GLuint error() const throw() { return error_; }

private:
    std::string what_;
    GLuint error_;
};
#endif

static GLuint glw_last_error;

static inline GLuint handle_error(const GLuint error, const GLchar* function = "")
{
#ifdef __GLW_ENABLE_EXCEPTIONS
    if(error != GL_NO_ERROR) {
        throw Error(function, error);
    }
#endif
    return error;
}

static inline const GLchar* error_string(const GLuint code)
{
    switch(code) {
    case GL_NO_ERROR:           return "No error";
    case GL_INVALID_ENUM:       return "Invalid enum";
    case GL_INVALID_VALUE:      return "Invalid value";
    case GL_INVALID_OPERATION:  return "Invalid operation";
    case GL_OUT_OF_MEMORY:      return "Out of memory";
    default:                    return "Unknown error";
    }
}

static inline size_t sizeof_type(const GLenum type)
{
    switch(type) {
    case GL_FLOAT:          return sizeof(GLfloat);
    case GL_FLOAT_VEC2:     return sizeof(GLfloat) * 2;
    case GL_FLOAT_VEC3:     return sizeof(GLfloat) * 3;
    case GL_FLOAT_VEC4:     return sizeof(GLfloat) * 4;
    case GL_FLOAT_MAT3:     return sizeof(GLfloat) * 3*3;
    case GL_FLOAT_MAT4:     return sizeof(GLfloat) * 4*4;
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    case GL_BYTE:           return sizeof(GLbyte);
    case GL_SHORT:          return sizeof(GLshort);
    case GL_INT:            return sizeof(GLint);
    default:                return GL_INVALID_VALUE;
    }
}

class Wrapper
{
private:
    Wrapper(const Wrapper&);

protected:
    GLuint handle_;

    Wrapper() : handle_(0) {}
    virtual ~Wrapper() {}
    operator GLuint&() { return handle_; }

public:
    GLuint id() const { return handle_; }
    GLuint operator()() const { return handle_; }
};

} // namespace

#endif

