#ifndef __GLW_BUFFER_HPP
#define __GLW_BUFFER_HPP

#include "glw.hpp"

namespace glw {

class Buffer : public Wrapper
{
private:
    GLenum usage_;
    size_t size_;

public:
    Buffer(
        const GLenum usage__,
        const size_t size__,
        const void* data__,
        GLuint* error__ = NULL)
      : usage_(usage__),
        size_(size__)
    {
        __GLW_HANDLE(glGenBuffers(1, &handle_)) {}        
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            if(error__) *error__ = handle_error(__GLW_LAST_ERROR, "glBindBuffer");
            return;
        }
        __GLW_HANDLE(glBufferData(GL_ARRAY_BUFFER, size__, data__, usage__)) {
            if(error__) *error__ = handle_error(__GLW_LAST_ERROR, "glBufferData");
            return;
        }
    }

    ~Buffer()
    {
        if(handle_) glDeleteBuffers(1, &handle_);
    }

    GLuint write(const GLint offset__, const size_t size__, const void* data__)
    {
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindBuffer");
        }
        __GLW_HANDLE(glBufferSubData(GL_ARRAY_BUFFER, offset__, size__, data__)) {
            return handle_error(__GLW_LAST_ERROR, "glBufferSubData");
        }
        return GL_NO_ERROR;
    }

    GLuint read(const GLint offset__, const size_t size__, void* data__)
    {
        void* mem;
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindBuffer");
        }
        __GLW_HANDLE(mem = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY)) {
            return handle_error(__GLW_LAST_ERROR, "glMapBuffer");
        }
        memcpy(data__, (GLubyte*)mem + offset__, size__);
        __GLW_HANDLE(glUnmapBuffer(GL_ARRAY_BUFFER)) {}
        return GL_NO_ERROR;
    }

    template <GLenum Name>
    GLint getInfo()
    {
        GLint result;
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindBuffer");
        }
        __GLW_HANDLE(glGetBufferParameteriv(GL_ARRAY_BUFFER, Name, &result)) {
            return handle_error(__GLW_LAST_ERROR, "glGetBufferParameteriv");
        }
        return result;
    }
};

} // namespace

#endif

