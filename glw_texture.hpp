#ifndef __GLW_TEXTURE_HPP
#define __GLW_TEXTURE_HPP

#include "glw.hpp"

namespace glw {

struct ImageFormat
{
    GLenum type;
    GLenum order;
};

class Texture : public Wrapper
{
protected:
    GLenum target_;
    GLint format_;
    GLint size_x_;
    GLint size_y_;
    GLint size_z_;

    Texture(
        const GLenum target__,
        const GLenum format__,
        const GLint size_x__, 
        const GLint size_y__,
        const GLint size_z__,
        GLuint* error)
      : target_(target__),
        format_(format__),
        size_x_(size_x__),
        size_y_(size_y__),
        size_z_(size_z__)
    {

        __GLW_HANDLE(glGenTextures(1, &handle_)) {}
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            if(error) *error = handle_error(__GLW_LAST_ERROR, "glBindTexture");
            return;
        }
        __GLW_HANDLE(glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST)) {
            if(error) *error = handle_error(__GLW_LAST_ERROR, "glTexParameteri");
            return;
        }
        __GLW_HANDLE(glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST)) {
            if(error) *error = handle_error(__GLW_LAST_ERROR, "glTexParameteri");
            return;
        }
        __GLW_HANDLE(glGenerateMipmap(target_)) {
            if(error) *error = handle_error(__GLW_LAST_ERROR, "glGenerateMipmap");
        }
#if 0
        __GLW_HANDLE(glTexParameteri(target_, GL_GENERATE_MIPMAP, GL_TRUE)) {
            if(error) *error = handle_error(__GLW_LAST_ERROR, "glTexParameteri");
            return;
        }
#endif
    }

    ~Texture()
    {
        if(handle_) glDeleteTextures(1, &handle_);
    }

public:
    GLuint bind()
    {
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindTexture");
        }
    }

    template <GLenum Name>
    GLint getInfo(const GLint lod__) 
    {
        GLint result;
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindTexture");
        }
        __GLW_HANDLE(glGetTexLevelParameteriv(target_, lod__, Name, &result)) {
            return handle_error(__GLW_LAST_ERROR, "glGetTexLevelParameteriv");
        }
        return result;
    }

    GLenum target() const { return target_; }
    GLint format() const { return format_; }
    GLint width() const { return size_x_; }
    GLint height() const { return size_y_; }
    GLint depth() const { return size_z_; }
};

class Texture2D : public Texture
{
public:
    Texture2D(
        const GLint internal_format__,
        const ImageFormat& format__,
        const GLint size_x__, 
        const GLint size_y__,
        const void* data__,
        GLuint* error = NULL)
      : Texture(GL_TEXTURE_2D, internal_format__, size_x__, size_y__, 0, error)
    {
        if(error && *error != GL_NO_ERROR) {
            return;
        }
        __GLW_HANDLE(glTexImage2D(
            target_, 
            0, 
            internal_format__, 
            size_x_, 
            size_y__,
            0, 
            format__.order, 
            format__.type, 
            data__)) {
            if(error) *error = handle_error(__GLW_LAST_ERROR, "glTexImage2D");
        }
    }

    GLuint write(
        const GLint lod__,
        const ImageFormat& format__,
        const GLint offset_x__,
        const GLint offset_y__,
        const GLint size_x__,
        const GLint size_y__,
        const void* data__)
    {
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindTexture");
        }
        __GLW_HANDLE(glTexSubImage2D(
            target_,
            lod__,
            offset_x__,
            offset_y__,
            size_x__,
            size_y__,
            format__.order,
            format__.type,
            data__)) {
            return handle_error(__GLW_LAST_ERROR, "glTexSubImage2D");
        }
        return GL_NO_ERROR;
    }

    GLuint read(
        const GLint lod__,
        const ImageFormat& format__,
        const GLint offset_x__,
        const GLint offset_y__,
        const GLint size_x__,
        const GLint size_y__,
        void* data__)
    {
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            return handle_error(__GLW_LAST_ERROR, "glBindTexture");
        }
        __GLW_HANDLE(glGetTexImage(target_, lod__, format__.order, format__.type, data__)) {
            return handle_error(__GLW_LAST_ERROR, "glGetTexImage");
        }
        return GL_NO_ERROR;
    }
};

} // namespace glw

#endif

