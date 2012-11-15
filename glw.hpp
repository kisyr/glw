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
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <memory>

#ifndef __GLW_ENABLE_EXCEPTIONS
#define __GLW_ERROR(Src, Code) std::cerr << "gl.hpp: " << trimCall(Src) << "(" << Code << ")\n";
#else
#define __GLW_ERROR(Src, Code) throw Error(trimCall(Src), Code);
#endif

#define __GLW_LOG(...) std::cout << "gl.hpp: " << fmtString(__VA_ARGS__) << "\n";
#define __GLW_CHECK(Call) ((Call), checkError(#Call))

namespace glw {

typedef GLubyte ubyte_t;
typedef GLushort ushort_t;
typedef GLuint uint_t;
typedef GLbyte byte_t;
typedef GLshort short_t;
typedef GLint int_t;
typedef GLenum enum_t;

#ifdef glm_glm
typedef glm::ivec4 ivec4_t;
typedef glm::vec2 vec2_t;
typedef glm::vec3 vec3_t;
typedef glm::vec4 vec4_t;
typedef glm::mat3 mat3_t;
typedef glm::mat4 mat4_t;
#else
struct ivec4_t { int_t x,y,z,w; };
struct vec2_t { float x,y; };
struct vec3_t { float x,y,z; };
struct vec4_t { float x,y,z,w; };
struct mat3_t { float m[3][3]; };
struct mat4_t { float m[4][4]; };
#endif

namespace detail {

} // namespace detail

/** 
 * @fn trimCall
 */
inline std::string trimCall(const std::string& call)
{
    size_t n = call.find_first_of('(');
    if( n > 0 )
        return std::string(call.c_str(), n);
    return call;
}

/**
 * @fn errorString
 * @brief Translates an OpenGL error code to a string.
 */
inline std::string errString(int_t code__)
{
    switch(code__) {
        case GL_NO_ERROR:
            return "No Error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_STACK_OVERFLOW:
            return "Stack overflow";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown Error";
    }
}

/**
 * @fn fmtString
 * @brief Returns a formatted string.
 */
inline std::string fmtString(const char* fmt, ...)
{
    // TODO: auto size buffer.
    char buffer[1024*8];
    
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    
    return std::string(buffer);
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
    
    Error(const std::string& what__, int_t err__)
      : what_(what__),
        err_(err__) {}
      
    ~Error() throw() {}
    
    const char* what() const throw() { return what_.c_str(); }
    
    int_t err() const throw() { return err_; }

private:
    std::string what_;
    int_t err_;
};
#endif

/**
 * @fn checkError
 */
inline void checkError(const char* call)
{
#ifdef __GLW_VERBOSE
    __GLW_LOG(call);
#endif
    int_t err = glGetError();
    if(err != GL_NO_ERROR)
        __GLW_ERROR(call, err);
}

/**
 * @fn containerType
 * @brief Gives data type and size for a container type.
 */
inline void containerType(
    enum_t container_type, 
    enum_t* type, 
    int_t* size_row,
    int_t* size_col)
{
    #define __GLW_IMPL_CONSTANTTYPE(Container, Type, Rows, Cols) \
        case Container: *type = Type; *size_row = Rows; *size_col = Cols; break;
    switch(container_type) {
        __GLW_IMPL_CONSTANTTYPE(GL_FLOAT,      GL_FLOAT, 1, 1);
        __GLW_IMPL_CONSTANTTYPE(GL_FLOAT_VEC2, GL_FLOAT, 2, 1);
        __GLW_IMPL_CONSTANTTYPE(GL_FLOAT_VEC3, GL_FLOAT, 3, 1);
        __GLW_IMPL_CONSTANTTYPE(GL_FLOAT_VEC4, GL_FLOAT, 4, 1);
        __GLW_IMPL_CONSTANTTYPE(GL_FLOAT_MAT3, GL_FLOAT, 3, 3);
        __GLW_IMPL_CONSTANTTYPE(GL_FLOAT_MAT4, GL_FLOAT, 4, 4);
        __GLW_IMPL_CONSTANTTYPE(GL_SAMPLER_2D, GL_INT,   1, 1);
        default:
            __GLW_LOG("containerType invalid type %i", container_type);
            break;
    }
    #undef __GLW_IMPL_CONSTANTTYPE
}

/**
 * @fn sizeofType
 * @brief Gives size in bytes for data type.
 */
inline size_t sizeofType(enum_t type)
{
    #define __GLW_IMPL_SIZEOFTYPE(Type, Size) \
        case Type: return Size;
    switch(type) {
        __GLW_IMPL_SIZEOFTYPE(GL_FLOAT,          sizeof(float));
        __GLW_IMPL_SIZEOFTYPE(GL_DOUBLE,         sizeof(double));
        __GLW_IMPL_SIZEOFTYPE(GL_UNSIGNED_BYTE,  sizeof(ubyte_t));
        __GLW_IMPL_SIZEOFTYPE(GL_UNSIGNED_SHORT, sizeof(ushort_t));
        __GLW_IMPL_SIZEOFTYPE(GL_UNSIGNED_INT,   sizeof(uint_t));
        __GLW_IMPL_SIZEOFTYPE(GL_BYTE,           sizeof(byte_t));
        __GLW_IMPL_SIZEOFTYPE(GL_SHORT,          sizeof(short_t));
        __GLW_IMPL_SIZEOFTYPE(GL_INT,            sizeof(int_t));
    }
    #undef __GLW_IMPL_SIZEOFTYPE
}

/**
 * @struct Name
 */
struct Name
{
    uint_t name;
    
    operator uint_t() const { return name; }
};

struct ArrayName : public Name
{
    ArrayName() { __GLW_CHECK(glGenVertexArrays(1, &name)); }
    ~ArrayName() { if(name) glDeleteVertexArrays(1, &name); }
};

struct BufferName : public Name
{
    BufferName() { __GLW_CHECK(glGenBuffers(1, &name)); }
    ~BufferName() { if(name) glDeleteBuffers(1, &name); }
};

struct ProgramName : public Name
{
    ProgramName() { __GLW_CHECK(name = glCreateProgram()); }
    ~ProgramName() { if(name) glDeleteProgram(name); }
};

struct TextureName : public Name
{
    TextureName() { __GLW_CHECK(glGenTextures(1, &name)); }
    ~TextureName() { if(name) glDeleteTextures(1, &name); }
};

struct SamplerName : public Name
{
    SamplerName() { __GLW_CHECK(glGenSamplers(1, &name)); }
    ~SamplerName() { if(name) glDeleteSamplers(1, &name); }
};

/**
 * @class Wrapper
 */
template <class T>
class Wrapper
{
protected:
    std::shared_ptr<T> object_;
    
    void create() { object_ = std::make_shared<T>(); }
    operator uint_t&() { return object_->name; }

public:
    Wrapper() : object_(NULL) {}

    uint_t name() const { return object_->name; }
    bool valid() const { return object_ && object_->name >= 0; }
};

/**
 * @class Buffer
 */
class Buffer : public Wrapper<BufferName>
{
private:
    enum_t usage_;
    size_t size_;

public:
    Buffer()
      : usage_(0),
        size_(0) {}
    
    Buffer(
        enum_t usage__,
        size_t size__,
        const void* data__)
      : usage_(usage__),
        size_(size__)
    {
        create();
        
        __GLW_CHECK(glBindBuffer(GL_ARRAY_BUFFER, *this));
        __GLW_CHECK(glBufferData(GL_ARRAY_BUFFER, size__, data__, usage__));
    }
    
    int_t getInfo(enum_t param__)
    {
        int_t result;
        __GLW_CHECK(glBindBuffer(GL_ARRAY_BUFFER, *this));
        __GLW_CHECK(glGetBufferParameteriv(GL_ARRAY_BUFFER, param__, &result));
        return result;
    }
    
    enum_t usage() const { return usage_; }
    size_t size() const { return size_; }
};

/** 
 * @class Texture
 */
class Texture : public Wrapper<TextureName>
{
protected:
    enum_t target_;
    int_t format_;
    int_t size_x_;
    int_t size_y_;
    int_t size_z_;

    Texture() {}

    Texture( 
        enum_t target__,
        enum_t format__,
        int_t size_x__, 
        int_t size_y__,
        int_t size_z__)
      : target_(target__),
        format_(format__),
        size_x_(size_x__),
        size_y_(size_y__),
        size_z_(size_z__)
    {
        create();
    
        __GLW_CHECK(glBindTexture(target_, *this));
        __GLW_CHECK(glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        __GLW_CHECK(glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        __GLW_CHECK(glTexParameteri(target_, GL_GENERATE_MIPMAP, GL_TRUE));
    }
    
public:
    enum_t target() const { return target_; }
    int_t format() const { return format_; }
    int_t width() const { return size_x_; }
    int_t height() const { return size_y_; }
    int_t depth() const { return size_z_; }
};

/**
 * @class Texture1D
 */
class Texture1D : public Texture
{
public:
    Texture1D() {}
    
    Texture1D(
        int_t format__,
        int_t size_x__)
      : Texture(GL_TEXTURE_1D, format__, size_x__, 0, 0)
    {
        __GLW_CHECK(glTexImage1D(
            target_, 
            0, 
            format_, 
            size_x_, 
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            NULL));
    }
    
    Texture1D(
        int_t format__,
        enum_t type__,
        int_t size_x__, 
        const void* data__)
      : Texture(GL_TEXTURE_1D, format__, size_x__, 0, 0)
    {
        __GLW_CHECK(glTexImage1D(
            target_, 
            0, 
            format_, 
            size_x_, 
            0, 
            format_, 
            type__, 
            data__));
    }
};

/**
 * @class Texture2D
 */
class Texture2D : public Texture
{
public:
    Texture2D() {}
    
    Texture2D(
        int_t format__,
        int_t size_x__,
        int_t size_y__)
      : Texture(GL_TEXTURE_2D, format__, size_x__, size_y__, 0)
    {
        __GLW_CHECK(glTexImage2D(
            target_, 
            0, 
            format_, 
            size_x_, 
            size_y_,
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            NULL));
    }
    
    Texture2D(
        int_t format__,
        enum_t type__,
        int_t size_x__, 
        int_t size_y__,
        const void* data__)
      : Texture(GL_TEXTURE_2D, format__, size_x__, size_y__, 0)
    {
        __GLW_CHECK(glTexImage2D(
            target_, 
            0, 
            format_, 
            size_x_, 
            size_y_,
            0, 
            format_, 
            type__, 
            data__));
    }
};

/**
 * @class Texture3D
 */
class Texture3D : public Texture
{
public:
    Texture3D() {}
    
    Texture3D(
        int_t format__,
        int_t size_x__,
        int_t size_y__,
        int_t size_z__)
      : Texture(GL_TEXTURE_3D, format__, size_x__, size_y__, size_z__)
    {
        __GLW_CHECK(glTexImage3D(
            target_, 
            0, 
            format_, 
            size_x_, 
            size_y_,
            size_z_,
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            NULL));
    }
    
    Texture3D(
        int_t format__,
        enum_t type__,
        int_t size_x__, 
        int_t size_y__,
        int_t size_z__,
        const void* data__)
      : Texture(GL_TEXTURE_3D, format__, size_x__, size_y__, size_z__)
    {
        __GLW_CHECK(glTexImage3D(
            target_, 
            0, 
            format_, 
            size_x_, 
            size_y_,
            size_z_,
            0, 
            format_, 
            type__, 
            data__));
    }
};

/**
 * @class Sampler
 */
class Sampler : public Wrapper<SamplerName>
{
public:
    Sampler() {}
    
    Sampler( 
        enum_t min_filter__,
        enum_t mag_filter__,
        enum_t wrap__)
    {
        create();
    
        __GLW_CHECK(glSamplerParameteri(*this, GL_TEXTURE_MIN_FILTER, min_filter__));
        __GLW_CHECK(glSamplerParameteri(*this, GL_TEXTURE_MAG_FILTER, mag_filter__));
        __GLW_CHECK(glSamplerParameteri(*this, GL_TEXTURE_WRAP_S, wrap__));
        __GLW_CHECK(glSamplerParameteri(*this, GL_TEXTURE_WRAP_T, wrap__));
        __GLW_CHECK(glSamplerParameteri(*this, GL_TEXTURE_WRAP_R, wrap__));
    }
};

/**
 * @struct Shader
 */
struct Shader
{
    Shader() {}
    
    Shader(uint_t target__, const std::string& code__)
      : target(target__),
        code(code__) {}

    Shader(uint_t target__, const std::ifstream& file__)
      : target(target__)
    {
        code = std::string(
            (std::istreambuf_iterator<char>(file__.rdbuf())),
            std::istreambuf_iterator<char>());
    }
    
    uint_t target;
    std::string code;
};

/**
 * @class Program
 */
class Program : public Wrapper<ProgramName>
{
public:
    struct Binary
    {
        std::vector<byte_t> data;
        enum_t format;
    };
    
    struct Attribute
    {
        bool active;
        int_t location;
        enum_t type;
        int_t size;
        size_t offset;
        size_t stride;
        uint_t buffer;
        
        Attribute()
          : active(false),
            location(0),
            type(0),
            size(0),
            offset(0),
            stride(0),
            buffer(0) {}
    };

    struct Uniform
    {
        bool active;
        int_t location;
        enum_t type;
        uint_t count;
        std::vector<byte_t> data;
        bool update;
        
        Uniform()
          : active(false),
            location(0),
            type(0),
            count(0),
            update(false) {}
    };
    
    struct Sampler
    {
        bool active;
        int_t unit;
        enum_t target;
        uint_t texture;
        uint_t sampler;
        
        Sampler()
          : active(false),
            unit(0),
            target(0),
            texture(0),
            sampler(0) {}
    };
    
    typedef std::vector<Shader> Shaders;
    typedef std::map<std::string, Attribute> Attributes;
    typedef std::map<std::string, Uniform> Uniforms;
    typedef std::map<std::string, Sampler> Samplers;
    
private:
    Shaders sources_;
    Attributes attributes_;
    Uniforms uniforms_;
    Samplers samplers_;
    Binary binary_;
    std::shared_ptr<ArrayName> array_;
    std::string log_;
    
    int_t attach(const Shader& source__)
    {
        uint_t new_shader;
        __GLW_CHECK(new_shader = glCreateShader(source__.target));
        const char* str = source__.code.c_str();
        int_t sz = source__.code.size();
        __GLW_CHECK(glShaderSource(new_shader, 1, &str, &sz));
        __GLW_CHECK(glCompileShader(new_shader));
        
        int_t status;
        __GLW_CHECK(glGetShaderiv(new_shader, GL_COMPILE_STATUS, &status));
        if(status == GL_FALSE) {
            std::string type;
            switch(source__.target) {
                case GL_VERTEX_SHADER: type = "VERTEX_SHADER"; break;
                case GL_FRAGMENT_SHADER: type = "FRAGMENT_SHADER"; break;
                case GL_GEOMETRY_SHADER: type = "GEOMETRY_SHADER"; break;
            }
            
            int_t llen, slen;
            __GLW_CHECK(glGetShaderiv(new_shader, GL_INFO_LOG_LENGTH, &llen));
            if(llen > 0) {
                std::string info_log;
                info_log.resize(llen);
                __GLW_CHECK(glGetShaderInfoLog(new_shader, llen, &slen, &info_log[0]));
                
                log_ += fmtString(
                    "-- SHADER COMPILE ERROR (%s) --\n%s",
                    type.c_str(),
                    info_log.c_str());
            }
        }
        
        if(status == GL_TRUE)
            __GLW_CHECK(glAttachShader(*this, new_shader));
        
        // Flag shader for deletion after program is deleted. 
        glDeleteShader(new_shader);
        
        return status;
    }
    
    int_t link()
    {
        __GLW_CHECK(glLinkProgram(*this));
        int_t status;
        __GLW_CHECK(glGetProgramiv(*this, GL_LINK_STATUS, &status));
        if(!status) {
            int_t llen, slen;
            __GLW_CHECK(glGetProgramiv(*this, GL_INFO_LOG_LENGTH, &llen));
            if(llen > 0) {
                std::string info_log;
                info_log.resize(llen);
                __GLW_CHECK(glGetProgramInfoLog(*this, llen, &slen, &info_log[0]));
                
                log_ += fmtString(
                    "-- PROGRAM LINK ERROR --\n%s",
                    info_log.c_str());
            }
        }
        
        return status;
    }
    
    void initAttributes()
    {
        int_t num_attributes = -1;
        __GLW_CHECK(glGetProgramiv(*this, GL_ACTIVE_ATTRIBUTES, &num_attributes));
        for(int_t index = 0; index < num_attributes; ++index) {
            int_t len;
            int_t count;
            enum_t type;
            char name[100];
            int_t location;
            
            __GLW_CHECK(glGetActiveAttrib(
                *this, 
                index,
                sizeof(name)-1,
                &len,
                &count,
                &type,
                name));
            __GLW_CHECK(location = glGetAttribLocation(*this, name));
            
            Attribute& attribute = attributes_[std::string(name)];
            attribute.active = true;
            attribute.location = location;
            
            int_t unused;
            containerType(type, &attribute.type, &attribute.size, &unused);
            
            /*
            std::cout << "attribute[" << attribute.location << "]\n"
                      << "\tname: " << name << "\n";
            */
        }
    }
    
    void initUniforms()
    {
        int_t num_uniforms = -1;
        __GLW_CHECK(glGetProgramiv(*this, GL_ACTIVE_UNIFORMS, &num_uniforms));
        for(int_t index = 0; index < num_uniforms; ++index) {
            int_t len;
            int_t count;
            enum_t type;
            char name[100];
            int_t location;
            
            __GLW_CHECK(glGetActiveUniform(
                *this,
                index,
                sizeof(name)-1,
                &len,
                &count,
                &type,
                name));
            __GLW_CHECK(location = glGetUniformLocation(*this, name));
            
            Uniform& uniform = uniforms_[std::string(name)];
            uniform.active = true;
            uniform.location = location;
            uniform.type = type;
            uniform.count = count;
            
            // Allocate space for data.
            int_t rows;
            int_t cols;
            containerType(type, &type, &rows, &cols);
            
            size_t bytes = sizeofType(type) * rows * cols * count;
            uniform.data.resize(bytes);
            
            /*
            std::cout << "uniform[" << uniform.location << "]\n"
                      << "\tname: " << name << "\n"
                      << "\tbytes: " << bytes << "\n";
            */
        }
    }
    
    void prepareAttributes()
    {
        for(uint_t i = 0; i < 16; ++i) {
            __GLW_CHECK(glDisableVertexAttribArray(i));
        }
        
        Attributes::const_iterator it;
        for(it = attributes_.begin(); it != attributes_.end(); ++it) {
            const Attribute& attribute = it->second;
            
            if(!attribute.active) 
                continue;
        
            __GLW_CHECK(glEnableVertexAttribArray(attribute.location));
            __GLW_CHECK(glBindBuffer(GL_ARRAY_BUFFER, attribute.buffer));
            __GLW_CHECK(glVertexAttribPointer(
                attribute.location, 
                attribute.size, 
                attribute.type, 
                GL_FALSE, 
                attribute.stride,
                (void*)attribute.offset));
        }
    }
    
    void prepareUniforms()
    {
        Uniforms::iterator it;
        for(it = uniforms_.begin(); it != uniforms_.end(); ++it) {
            Uniform& uniform = it->second;
            int_t location = uniform.location;
            
            if(!uniform.active) 
                continue;
            
            // Skip updating uniforms already updated.
            if(uniform.update == false)
                continue;
            
            switch(uniform.type) {
                case GL_INT:
                case GL_SAMPLER_1D:
                case GL_SAMPLER_2D:
                case GL_SAMPLER_3D:
                    __GLW_CHECK(glUniform1iv(location, uniform.count, (int_t*)&uniform.data[0]));
                    break;
                case GL_FLOAT:
                    __GLW_CHECK(glUniform1fv(location, uniform.count, (float*)&uniform.data[0]));
                    break;
                case GL_FLOAT_VEC2:
                    __GLW_CHECK(glUniform2fv(location, uniform.count, (float*)&uniform.data[0]));
                    break;
                case GL_FLOAT_VEC3:
                    __GLW_CHECK(glUniform3fv(location, uniform.count, (float*)&uniform.data[0]));
                    break;
                case GL_FLOAT_VEC4:
                    __GLW_CHECK(glUniform4fv(location, uniform.count, (float*)&uniform.data[0]));
                    break;
                case GL_FLOAT_MAT3:
                    __GLW_CHECK(glUniformMatrix3fv(location, uniform.count, GL_FALSE, (float*)&uniform.data[0]));
                    break;
                case GL_FLOAT_MAT4:
                    __GLW_CHECK(glUniformMatrix4fv(location, uniform.count, GL_FALSE, (float*)&uniform.data[0]));
                    break;
                default:
                    __GLW_LOG("Program::prepareUniforms invalid type %i", uniform.type);
                    break;
            }
            
            // Mark this uniform as up to date.
            uniform.update = false;
        }
    }
    
    void prepareSamplers()
    {
        Samplers::const_iterator it;
        for(it = samplers_.begin(); it != samplers_.end(); ++it) {
            const Sampler& sampler = it->second;
            
            if(!sampler.active) 
                continue;
        
            __GLW_CHECK(glActiveTexture(GL_TEXTURE0 + sampler.unit));
            __GLW_CHECK(glBindTexture(sampler.target, sampler.texture));
            __GLW_CHECK(glEnable(sampler.target));
            __GLW_CHECK(glBindSampler(sampler.unit, sampler.sampler));
        }
    }

public:
    Program() {}
    
    Program(const Shaders& sources__)
      : sources_(sources__),
        array_(std::make_shared<ArrayName>())
    {
        create();
    }
    
    Program(const Binary& binary__)
      : binary_(binary__),
        array_(std::make_shared<ArrayName>())
    {
        create();
        
        __GLW_CHECK(glProgramBinary(
            *this, 
            binary_.format, 
            &binary_.data[0],
            binary_.data.size()));
        
        initAttributes();
        initUniforms();
    }
    
    int_t build()
    {
        if(sources_.size() == 0)
            return GL_FALSE;
    
        // Compile.
        Shaders::iterator it;
        for(it = sources_.begin(); it != sources_.end(); ++it)
            if(attach(*it) == GL_FALSE)
                return GL_FALSE;
        
        // Link.
        if(link() == GL_FALSE)
            return GL_FALSE;
        
        // Get binary.
        binary_.data.resize(getInfo(GL_PROGRAM_BINARY_LENGTH));
        int_t binary_length;
        __GLW_CHECK(glGetProgramBinary(
            *this, 
            binary_.data.size(),
            &binary_length,
            &binary_.format,
            &binary_.data[0]));
        
        // Get attributes and uniforms.
        initAttributes();
        initUniforms();
        
        return GL_TRUE;
    }
    
    template <typename T>
    void setUniform(
        const std::string& name__,
        const T& value__,
        uint_t count__ = 1)
    {
        Uniform& uniform = uniforms_[name__];
        
        if(!uniform.active) {
            __GLW_LOG("Program::setUniform invalid name %s", name__.c_str());
            return;
        }
        
        const size_t size = sizeof(T) * count__;
        if(size > uniform.data.size()) {
            __GLW_LOG("Program::setUniform size overflow %i > %i for %s", 
                size, uniform.data.size(), name__.c_str());
            return;
        }
        
        memcpy(&uniform.data[0], &value__, size);
        uniform.update = true;
    }
    
    void setAttribute(
        const std::string& name__,
        const Buffer& buffer__,
        size_t offset__ = 0,
        size_t stride__ = 0)
    {
        Attribute& attribute = attributes_[name__];
        
        if(!attribute.active) {
            __GLW_LOG("Program::setAttribute invalid name %s", name__.c_str());
            return;
        }
        
        attribute.offset = offset__;
        attribute.stride = stride__;
        attribute.buffer = buffer__.name();
    }
    
    void setSampler(
        const std::string& name__,
        const glw::Sampler& sampler__,
        const Texture& texture__,
        int_t unit__ = 0)
    {
        setUniform(name__, unit__);
        
        Sampler& sampler = samplers_[name__];
        sampler.active = true;
        sampler.unit = unit__;
        sampler.target = texture__.target();
        sampler.texture = texture__.name();
        sampler.sampler = sampler__.name();
    }
    
    void prepare()
    {
        __GLW_CHECK(glBindVertexArray(*array_));
        
        prepareAttributes();
        prepareUniforms();
        prepareSamplers();
    }
    
    int_t getInfo(enum_t param__)
    {
        int_t result;
        __GLW_CHECK(glGetProgramiv(*this, param__, &result));
        return result;
    }
    
    const Attributes& attributes() const { return attributes_; }
    const Uniforms& uniforms() const { return uniforms_; }
    const Binary& binary() const { return binary_; }
    std::string log() const { return log_; }
};

/**
 * @class Context
 */
class Context
{
private:
    uint_t last_program_;

    void prepareDraw(Program& program__)
    {
        if(last_program_ != program__.name()) {
            __GLW_CHECK(glUseProgram(program__.name()));
        }
        last_program_ = program__.name();
        program__.prepare();
    }
    
public:
    void clear(enum_t buffers__)
    {
        __GLW_CHECK(glClear(buffers__));
    }
    
    void drawArrays(
        Program& program__,
        enum_t primitive__, 
        uint_t offset__, 
        uint_t count__)
    {
        prepareDraw(program__);
        
        __GLW_CHECK(glDrawArrays(primitive__, offset__, count__));
    }
    
    void drawElements(
        Program& program__,
        enum_t primitive__, 
        uint_t offset__, 
        uint_t count__, 
        enum_t type__,
        const Buffer& elements__)
    {
        prepareDraw(program__);
        
        __GLW_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements__.name()));
        __GLW_CHECK(glDrawElements(
            primitive__, 
            count__, 
            type__, 
            (void*)offset__));
        __GLW_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }
    
    void enable(enum_t param__)
    {
        __GLW_CHECK(glEnable(param__));
    }
};

} // namespace gl

#endif
