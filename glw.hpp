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

#include <GL/gl.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

#define __GLW_LAST_ERROR glw_last_error

#ifndef __GLW_NDEBUG
#define __GLW_HANDLE(Call) Call; glw_last_error = glGetError(); if(glw_last_error != GL_NO_ERROR)
#else
#define __GLW_HANDLE(Call) Call
#endif

namespace glw {

#ifdef __GLW_ENABLE_CPP11
#include <memory>
using std::shared_ptr;
using std::make_shared;
#else
/**
 * @class shared_ptr
 * @brief Minimal implementation of C++11 std::shared_ptr.
 */
template<class T>
class shared_ptr
{
    template<class U>
    friend class shared_ptr;
    
public:
    shared_ptr() :p(), c() {}
    
    explicit shared_ptr(T* s) :p(s), c(new unsigned(1)) {}

    shared_ptr(const shared_ptr& s) :p(s.p), c(s.c) { if(c) ++*c; }

    shared_ptr& operator=(const shared_ptr& s) 
    { 
        if(this!=&s) { 
            clear(); 
            p=s.p; 
            c=s.c; 
            if(c) ++*c; 
        } 
        return *this; 
    }

    template<class U>
    shared_ptr(const shared_ptr<U>& s) :p(s.p), c(s.c) { if(c) ++*c; }

    ~shared_ptr() { clear(); }

    void clear() 
    { 
        if(c) {
            if(*c==1) delete p; 
            if(!--*c) delete c; 
        } 
        c=0; 
        p=0; 
    }

    T* get() const { return (c)? p: 0; }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    operator bool() const { return p; }

private:
    T* p;
    unsigned* c;
};

template <class T>
shared_ptr<T> make_shared()
{
    return shared_ptr<T>(new T());
}
#endif

/**
 * @fn errorString
 * @brief Translates an OpenGL error code to a string.
 */
static inline const GLchar* errorString(const GLint code)
{
    switch(code) {
    case GL_NO_ERROR:           return "No error";
    case GL_INVALID_ENUM:       return "Invalid enum";
    case GL_INVALID_VALUE:      return "Invalid value";
    case GL_INVALID_OPERATION:  return "Invalid operation";
    case GL_STACK_OVERFLOW:     return "Stack overflow";
    case GL_STACK_UNDERFLOW:    return "Stack underflow";
    case GL_OUT_OF_MEMORY:      return "Out of memory";
    default:                    return "Unknown error";
    }
}

#ifdef __GLW_ENABLE_EXCEPTIONS
#include <exception>
/**
 * @class Error
 * @brief OpenGL error exception.
 */
class Error : public std::exception
{
public:
    Error() {}
    
    Error(const std::string& what__, GLint error__)
      : what_(what__),
        error_(error__) {}
      
    ~Error() throw() {}
    
    const GLchar* what() const throw() { return what_.c_str(); }
    GLint error() const throw() { return error_; }

private:
    std::string what_;
    GLint error_;
};
#endif

static GLuint glw_last_error;

static inline GLuint handleError(GLuint error)
{
#ifdef __GLW_ENABLE_EXCEPTIONS
    if(error != GL_NO_ERROR) {
        throw Error("", error);
    }
#endif
    return error;
}

static inline size_t sizeofType(GLenum type)
{
    switch(type) {
    case GL_FLOAT:          return sizeof(GLfloat);
    case GL_FLOAT_VEC2:     return sizeof(GLfloat) * 2;
    case GL_FLOAT_VEC3:     return sizeof(GLfloat) * 3;
    case GL_FLOAT_VEC4:     return sizeof(GLfloat) * 4;
    case GL_FLOAT_MAT3:     return sizeof(GLfloat) * 3*3;
    case GL_FLOAT_MAT4:     return sizeof(GLfloat) * 4*4;
    case GL_DOUBLE:         return sizeof(GLdouble);
    case GL_DOUBLE_VEC2:    return sizeof(GLdouble) * 2;
    case GL_DOUBLE_VEC3:    return sizeof(GLdouble) * 3;
    case GL_DOUBLE_VEC4:    return sizeof(GLdouble) * 4;
    case GL_DOUBLE_MAT3:    return sizeof(GLdouble) * 3*3;
    case GL_DOUBLE_MAT4:    return sizeof(GLdouble) * 4*4;
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    case GL_BYTE:           return sizeof(GLbyte);
    case GL_SHORT:          return sizeof(GLshort);
    case GL_INT:            return sizeof(GLint);
    default:                return GL_INVALID_VALUE;
    }
}

struct Ref
{
    GLuint handle;
};

struct ArrayRef : public Ref
{
    ArrayRef() { __GLW_HANDLE(glGenVertexArrays(1, &handle)) {} }
    ~ArrayRef() { if(handle) glDeleteVertexArrays(1, &handle); }
};

struct BufferRef : public Ref
{
    BufferRef() { __GLW_HANDLE(glGenBuffers(1, &handle)) {} }
    ~BufferRef() { if(handle) glDeleteBuffers(1, &handle); }
};

struct ProgramRef : public Ref
{
    ProgramRef() { __GLW_HANDLE(handle = glCreateProgram()) {} }
    ~ProgramRef() { if(handle) glDeleteProgram(handle); }
};

struct TextureRef : public Ref
{
    TextureRef() { __GLW_HANDLE(glGenTextures(1, &handle)) {} }
    ~TextureRef() { if(handle) glDeleteTextures(1, &handle); }
};

struct SamplerRef : public Ref
{
    SamplerRef() { __GLW_HANDLE(glGenSamplers(1, &handle)) {} }
    ~SamplerRef() { if(handle) glDeleteSamplers(1, &handle); }
};

/**
 * @class Wrapper
 * @brief All deriving classes should only contain fields that can
 *        copied safely by a default copy constructor. The object handle
 *        is wrapped by a ref-counted smart pointer.
 */
template <class R>
class Wrapper
{
protected:
    shared_ptr<R> object_;
    
    void create() { object_ = make_shared<R>(); }
    operator GLuint&() { return object_->handle; }

public:
    GLuint operator()() const { return object_ ? object_->handle : 0; }
};

/**
 * @class Buffer
 */
class Buffer : public Wrapper<BufferRef>
{
private:
    GLenum usage_;
    size_t size_;

public:
    Buffer()
      : usage_(0),
        size_(0) {}
    
    Buffer(
        const GLenum usage__,
        const size_t size__,
        const void* data__,
        GLuint* error__ = NULL)
      : usage_(usage__),
        size_(size__)
    {
        create();
        
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            if(error__) *error__ = __GLW_LAST_ERROR;
            return;
        }
        __GLW_HANDLE(glBufferData(GL_ARRAY_BUFFER, size__, data__, usage__)) {
            if(error__) *error__ = __GLW_LAST_ERROR;
            return;
        }
    }
    
    GLuint write(const GLint offset__, const size_t size__, const void* data__)
    {
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            return handleError(__GLW_LAST_ERROR);
        }
        __GLW_HANDLE(glBufferSubData(GL_ARRAY_BUFFER, offset__, size__, data__)) {
            return handleError(__GLW_LAST_ERROR);
        }
        return GL_NO_ERROR;
    }

    GLuint read(const GLint offset__, const size_t size__, void* data__)
    {
        void* mem;
        __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, *this)) {
            return handleError(__GLW_LAST_ERROR);
        }
        __GLW_HANDLE(mem = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY)) {
            return handleError(__GLW_LAST_ERROR);
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
            return handleError(__GLW_LAST_ERROR);
        }
        __GLW_HANDLE(glGetBufferParameteriv(GL_ARRAY_BUFFER, Name, &result)) {
            return handleError(__GLW_LAST_ERROR);
        }
        return result;
    }
};

struct ImageFormat
{
    GLenum type;
    GLenum order;
};

/** 
 * @class Texture
 */
class Texture : public Wrapper<TextureRef>
{
protected:
    GLenum target_;
    GLint format_;
    GLint size_x_;
    GLint size_y_;
    GLint size_z_;

    Texture() {}

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
        create();
    
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            if(error) *error = __GLW_LAST_ERROR;
            return;
        }
        __GLW_HANDLE(glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST)) {
            if(error) *error =__GLW_LAST_ERROR;
            return;
        }
        __GLW_HANDLE(glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST)) {
            if(error) *error = __GLW_LAST_ERROR;
            return;
        }
        __GLW_HANDLE(glTexParameteri(target_, GL_GENERATE_MIPMAP, GL_TRUE)) {
            if(error) *error = __GLW_LAST_ERROR;
            return;
        }
    }
    
public:
    template <GLenum Name>
    GLint getInfo(const GLint lod__) 
    {
        GLint result;
        __GLW_HANDLE(glBindTexture(target_, *this)) {
            return handleError(__GLW_LAST_ERROR);
        }
        __GLW_HANDLE(glGetTexLevelParameteriv(target_, lod__, Name, &result)) {
            return handleError(__GLW_LAST_ERROR);
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
    Texture2D() {}
    
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
            if(error) *error = __GLW_LAST_ERROR;
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
            return handleError(__GLW_LAST_ERROR);
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
            return handleError(__GLW_LAST_ERROR);
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
            return handleError(__GLW_LAST_ERROR);
        }
        __GLW_HANDLE(glGetTexImage(target_, lod__, format__.order, format__.type, data__)) {
            return handleError(__GLW_LAST_ERROR);
        }
        return GL_NO_ERROR;
    }
};

/**
 * @class Sampler
 */
class Sampler : public Wrapper<SamplerRef>
{
public:
    Sampler() {}
    
    Sampler(
        const GLenum min_filter__,
        const GLenum mag_filter__,
        const GLenum wrap__,
        GLuint* error = NULL)
    {
        create();
    
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_MIN_FILTER, min_filter__)) {
            if(error) *error = __GLW_LAST_ERROR;
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_MAG_FILTER, mag_filter__)) {
            if(error) *error = __GLW_LAST_ERROR;
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_WRAP_S, wrap__)) {
            if(error) *error = __GLW_LAST_ERROR;
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_WRAP_T, wrap__)) {
            if(error) *error = __GLW_LAST_ERROR;
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_WRAP_R, wrap__)) {
            if(error) *error = __GLW_LAST_ERROR;
        }
    }
};

/**
 * @class Program
 */
class Program : public Wrapper<ProgramRef>
{
public:
    struct Attribute
    {
        static const size_t name_size = 32;
        GLint location;
        char name[name_size];
        GLint size;
        GLenum type;
        size_t stride;
        size_t offset;
        GLuint buffer;
        bool dirty;
    };

    struct Uniform
    {
        static const size_t name_size = 32;
        GLint location;
        char name[name_size];
        GLint size;
        GLenum type;
        GLuint count;
        std::vector<GLbyte> data;
        bool dirty;
    };
    
    typedef std::vector<std::pair<GLenum, const GLchar*> > Shaders;
    typedef std::vector<Attribute> Attributes;
    typedef std::vector<Uniform> Uniforms;
    
private:
    Shaders sources_;
    Attributes attributes_;
    Uniforms uniforms_;
    shared_ptr<ArrayRef> array_;

    GLuint prepareAttributes()
    {
        Attribute* attribute;
        GLenum type;
        GLint size;

        __GLW_HANDLE(glBindVertexArray(array_->handle)) {
            return handleError(__GLW_LAST_ERROR);
        }

        for(int i = 0; i < attributes_.size(); ++i) {
            attribute = &attributes_[i];
            if(!attribute->dirty) continue;

            #define __GLW_IMPL_ATTRIB_TRANS(ContainerType, DataType, Size) \
                case ContainerType: type = DataType; size = Size; break;
            switch(attribute->type) {
            __GLW_IMPL_ATTRIB_TRANS(GL_FLOAT,               GL_FLOAT,           1);
            __GLW_IMPL_ATTRIB_TRANS(GL_FLOAT_VEC2,          GL_FLOAT,           2);
            __GLW_IMPL_ATTRIB_TRANS(GL_FLOAT_VEC3,          GL_FLOAT,           3);
            __GLW_IMPL_ATTRIB_TRANS(GL_FLOAT_VEC4,          GL_FLOAT,           4);
            __GLW_IMPL_ATTRIB_TRANS(GL_INT,                 GL_INT,             1);
            __GLW_IMPL_ATTRIB_TRANS(GL_INT_VEC2,            GL_INT,             2);
            __GLW_IMPL_ATTRIB_TRANS(GL_INT_VEC3,            GL_INT,             3);
            __GLW_IMPL_ATTRIB_TRANS(GL_INT_VEC4,            GL_INT,             4);
            __GLW_IMPL_ATTRIB_TRANS(GL_UNSIGNED_INT,        GL_UNSIGNED_INT,    1);
            __GLW_IMPL_ATTRIB_TRANS(GL_UNSIGNED_INT_VEC2,   GL_UNSIGNED_INT,    2);
            __GLW_IMPL_ATTRIB_TRANS(GL_UNSIGNED_INT_VEC3,   GL_UNSIGNED_INT,    3);
            __GLW_IMPL_ATTRIB_TRANS(GL_UNSIGNED_INT_VEC4,   GL_UNSIGNED_INT,    4);
            default: return handleError(GL_INVALID_OPERATION);
            }

            __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, attribute->buffer)) {
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(glVertexAttribPointer(
                attribute->location,
                size,
                type,
                GL_FALSE,
                attribute->stride,
                (void*)attribute->offset)) {
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(glEnableVertexAttribArray(i)) {
                return handleError(__GLW_LAST_ERROR);
            }
            attribute->dirty = false;
        }

        return GL_NO_ERROR;
    }

    GLuint prepareUniforms()
    {
        Uniform* uniform;

        for(int i = 0; i < uniforms_.size(); ++i) {
            uniform = &uniforms_[i];
            if(!uniform->dirty) continue;

            #define __GLW_IMPL_UNIFORM_TRANS(ContainerType, Function, Cast) \
                case ContainerType: __GLW_HANDLE(Function(uniform->location, uniform->size, (Cast)&uniform->data[0])) { \
                    return handleError(__GLW_LAST_ERROR); } break;
            #define __GLW_IMPL_UNIFORM_TRANS_MAT(ContainerType, Function, Cast) \
                case ContainerType: __GLW_HANDLE(Function(uniform->location, uniform->size, GL_FALSE, (Cast)&uniform->data[0])) { \
                    return handleError(__GLW_LAST_ERROR); } break;
            switch(uniform->type) {
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT,              glUniform1fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT_VEC2,         glUniform2fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT_VEC3,         glUniform3fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT_VEC4,         glUniform4fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS_MAT(GL_FLOAT_MAT2,     glUniformMatrix2fv, const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS_MAT(GL_FLOAT_MAT3,     glUniformMatrix3fv, const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS_MAT(GL_FLOAT_MAT4,     glUniformMatrix4fv, const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT,                glUniform1iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT_VEC2,           glUniform2iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT_VEC3,           glUniform3iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT_VEC4,           glUniform4iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_UNSIGNED_INT,       glUniform1uiv,      const GLuint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_UNSIGNED_INT_VEC2,  glUniform2uiv,      const GLuint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_UNSIGNED_INT_VEC3,  glUniform3uiv,      const GLuint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_UNSIGNED_INT_VEC4,  glUniform4uiv,      const GLuint*);
            default: return handleError(GL_INVALID_OPERATION);
            }
        }

        return GL_NO_ERROR;
    }

public:
    Program() {}
    
    Program(const Shaders& sources__, GLuint* error = NULL)
      : sources_(sources__),
        array_(make_shared<ArrayRef>())
    {
        create();
    }
    
    GLuint build()
    {
        if(sources_.size() == 0) {
            return handleError(GL_INVALID_VALUE);
        }

        Shaders::iterator it;
        for(it = sources_.begin(); it != sources_.end(); ++it) {
            GLint length = strlen(it->second);
            GLuint shader;

            __GLW_HANDLE(shader = glCreateShader(it->first)) {
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(glShaderSource(shader, 1, (const GLchar**)&it->second, &length)) {
                glDeleteShader(shader);
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(glCompileShader(shader)) {
                glDeleteShader(shader);
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(glAttachShader(*this, shader)) {
                glDeleteShader(shader);
                return handleError(__GLW_LAST_ERROR);
            }

            // Mark shader for deletion after program destruction.
            glDeleteShader(shader);
        }

        __GLW_HANDLE(glLinkProgram(*this)) {
            return handleError(__GLW_LAST_ERROR);
        }

        if(getInfo<GL_LINK_STATUS>() == GL_FALSE) {
            return handleError(GL_INVALID_OPERATION);
        }

        // Setup attributes.
        for(int i = 0; i < getInfo<GL_ACTIVE_ATTRIBUTES>(); ++i) {
            Attribute attribute = {0};
            __GLW_HANDLE(glGetActiveAttrib(
                *this,
                i,
                Attribute::name_size,
                NULL,
                &attribute.size,
                &attribute.type,
                attribute.name)) {
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(attribute.location = glGetAttribLocation(*this, attribute.name)) {
                return handleError(__GLW_LAST_ERROR);
            }
            attributes_.push_back(attribute);
        }

        // Setup uniforms.
        for(int i = 0; i < getInfo<GL_ACTIVE_UNIFORMS>(); ++i) {
            Uniform uniform = {0};
            __GLW_HANDLE(glGetActiveUniform(
                *this,
                i,
                Uniform::name_size,
                NULL,
                &uniform.size,
                &uniform.type,
                uniform.name)) {
                return handleError(__GLW_LAST_ERROR);
            }
            __GLW_HANDLE(uniform.location = glGetUniformLocation(*this, uniform.name)) {
                return handleError(__GLW_LAST_ERROR);
            }
            uniform.data.resize(sizeofType(uniform.type) * uniform.size);
            uniforms_.push_back(uniform);
        }

        return GL_NO_ERROR;
    }

    GLuint prepare()
    {
        if(prepareAttributes() != GL_NO_ERROR) {
            return handleError(__GLW_LAST_ERROR);
        }
        if(prepareUniforms() != GL_NO_ERROR) {
            return handleError(__GLW_LAST_ERROR);
        }
        return GL_NO_ERROR;
    }

    GLuint execute(
        const GLenum topology__, 
        const GLint offset__, 
        const GLint elements__)
    {
        static GLuint bound = 0;
        if(bound != *this) {
            __GLW_HANDLE(glUseProgram(*this)) {
                return handleError(__GLW_LAST_ERROR);
            }
            bound = *this;
        }
        if(prepare() != GL_NO_ERROR) {
            return handleError(__GLW_LAST_ERROR);
        }
        __GLW_HANDLE(glDrawArrays(topology__, offset__, elements__)) {
            return handleError(__GLW_LAST_ERROR);
        }
        return GL_NO_ERROR;
    }

    std::string log()
    {
        std::string result;
        result.resize(getInfo<GL_INFO_LOG_LENGTH>());
        GLint size;
        __GLW_HANDLE(glGetProgramInfoLog(*this, result.size(), &size, &result[0])) {
            return "";
        }
        return result;
    }
    
    GLuint setAttribute(
        const GLchar* name__,
        const GLuint buffer__,
        const size_t stride__ = 0,
        const size_t offset__ = 0)
    {
        Attribute* attribute = NULL;
        for(int i = 0; i < attributes_.size(); ++i) {
            if(strcmp(attributes_[i].name, name__) == 0) {
                attribute = &attributes_[i];
                break;
            }
        }
        if(!attribute) {
            return handleError(GL_INVALID_VALUE);
        }
        attribute->buffer = buffer__;
        attribute->offset = offset__;
        attribute->stride = stride__;
        attribute->dirty = true;
        return GL_NO_ERROR;
    }
   
    template <typename T>
    GLuint setUniform(
        const GLchar* name__,
        const T& value__,
        const GLuint count__ = 1)
    {
        Uniform* uniform = NULL;
        for(int i = 0; i < uniforms_.size(); ++i) {
            if(strcmp(uniforms_[i].name, name__) == 0) {
                uniform = &uniforms_[i];
                break;
            }
        }
        if(!uniform) {
            return handleError(GL_INVALID_VALUE);
        }
        const size_t size = sizeof(T) * count__;
        if(size > sizeofType(uniform->type) * uniform->size) {
            return handleError(GL_INVALID_VALUE);
        }
        memcpy(&uniform->data[0], &value__, size);
        uniform->dirty = true;
        return GL_NO_ERROR;
    }
    
    template <GLenum Name>
    GLint getInfo() 
    {
        GLint result;
        __GLW_HANDLE(glGetProgramiv(*this, Name, &result)) {
            return handleError(__GLW_LAST_ERROR);
        }
        return result;
    }
    
    const Attributes& attributes() const { return attributes_; }
    const Uniforms& uniforms() const { return uniforms_; }
};

} // namespace

#endif

