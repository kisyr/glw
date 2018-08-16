#ifndef __GLW_PROGRAM_HPP
#define __GLW_PROGRAM_HPP

#include "glw.hpp"

namespace glw {

class BuildError : public std::exception
{
public:
    BuildError(const GLuint program_, const std::string& log_)
      : program_(program_),
        log_(log_) {}

    ~BuildError() throw() {}

    GLuint program() const { return program_; }
    const char* log() const { return log_.c_str(); }

private:
    GLuint program_;
    std::string log_;
};

class Program : public Wrapper
{
public:
    struct Attribute
    {
        static const size_t name_size = 32;
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
        char name[name_size];
        GLint size;
        GLenum type;
        GLuint texture;
        std::vector<GLubyte> data;
        bool dirty;
    };

    struct Shader
    {
        GLenum type;
        const GLchar* source;
    };
    
    typedef std::vector<Attribute> Attributes;
    typedef std::vector<Uniform> Uniforms;
    typedef std::vector<Shader> Shaders;
   
private:
    Shaders sources_;
    Attributes attributes_;
    Uniforms uniforms_;

    GLuint prepareAttributes()
    {
        Attribute* attribute;
        GLenum type;
        GLint size;

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
            default: return handle_error(GL_INVALID_OPERATION, "Program::prepareAttributes");
            }

            __GLW_HANDLE(glBindBuffer(GL_ARRAY_BUFFER, attribute->buffer)) {
                return handle_error(__GLW_LAST_ERROR, "glBindBuffer");
            }
            __GLW_HANDLE(glVertexAttribPointer(
                i,
                size,
                type,
                GL_FALSE,
                attribute->stride,
                (void*)attribute->offset)) {
                return handle_error(__GLW_LAST_ERROR, "glVertexAttribPointer");
            }
            __GLW_HANDLE(glEnableVertexAttribArray(i)) {
                return handle_error(__GLW_LAST_ERROR, "glEnableVertexAttribArray");
            }
            attribute->dirty = false;
        }

        return GL_NO_ERROR;
    }

    GLuint prepareUniforms()
    {
        Uniform* uniform;
        GLenum texture_type;

        for(int i = 0; i < uniforms_.size(); ++i) {
            uniform = &uniforms_[i];
            if(!uniform->dirty) continue;

            texture_type = 0;
            switch(uniform->type) {
            //case GL_SAMPLER_1D: texture_type = GL_TEXTURE_1D; break;
            case GL_SAMPLER_2D: texture_type = GL_TEXTURE_2D; break;
            //case GL_SAMPLER_3D: texture_type = GL_TEXTURE_3D; break;
            }

            if(texture_type != 0) {
                 __GLW_HANDLE(glActiveTexture(GL_TEXTURE0 + *reinterpret_cast<GLint*>(uniform->data.data()))) {
                    return handle_error(__GLW_LAST_ERROR, "glActiveTexture");
                }
                __GLW_HANDLE(glBindTexture(texture_type, uniform->texture)) {
                    return handle_error(__GLW_LAST_ERROR, "glBindTexture");
                }
            }

            #define __GLW_IMPL_UNIFORM_TRANS(ContainerType, Function, Cast) \
                case ContainerType: __GLW_HANDLE(Function(i, uniform->size, reinterpret_cast<Cast>(&uniform->data[0]))) { \
                    return handle_error(__GLW_LAST_ERROR, #Function); } break;
            #define __GLW_IMPL_UNIFORM_TRANS_MAT(ContainerType, Function, Cast) \
                case ContainerType: __GLW_HANDLE(Function(i, uniform->size, GL_FALSE, reinterpret_cast<Cast>(&uniform->data[0]))) { \
                    return handle_error(__GLW_LAST_ERROR, #Function); } break;
            switch(uniform->type) {
            __GLW_IMPL_UNIFORM_TRANS(GL_SAMPLER_2D,         glUniform1iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT,              glUniform1fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT_VEC2,         glUniform2fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT_VEC3,         glUniform3fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_FLOAT_VEC4,         glUniform4fv,       const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT,                glUniform1iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT_VEC2,           glUniform2iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT_VEC3,           glUniform3iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS(GL_INT_VEC4,           glUniform4iv,       const GLint*);
            __GLW_IMPL_UNIFORM_TRANS_MAT(GL_FLOAT_MAT2,     glUniformMatrix2fv, const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS_MAT(GL_FLOAT_MAT3,     glUniformMatrix3fv, const GLfloat*);
            __GLW_IMPL_UNIFORM_TRANS_MAT(GL_FLOAT_MAT4,     glUniformMatrix4fv, const GLfloat*);
            default: return handle_error(GL_INVALID_OPERATION, "Program::prepareUniforms");
            }
        }

        return GL_NO_ERROR;
    }

public:
    Program(const Shaders& sources__, GLuint* error = NULL)
      : sources_(sources__)
    {
        __GLW_HANDLE(handle_ = glCreateProgram()) {}
    }

    ~Program()
    {
        if(handle_) glDeleteProgram(handle_);
    }
    
    GLuint build()
    {
        if(sources_.size() == 0) {
            return handle_error(GL_INVALID_VALUE, "Program::build");
        }

        Shaders::iterator it;
        for(it = sources_.begin(); it != sources_.end(); ++it) {
            GLint length = strlen(it->source);
            GLuint shader;

            __GLW_HANDLE(shader = glCreateShader(it->type)) {
                return handle_error(__GLW_LAST_ERROR, "glCreateShader");
            }
            __GLW_HANDLE(glShaderSource(shader, 1, (const GLchar**)&it->source, &length)) {
                glDeleteShader(shader);
                return handle_error(__GLW_LAST_ERROR, "glShaderSource");
            }
            __GLW_HANDLE(glCompileShader(shader)) {
                glDeleteShader(shader);
                return handle_error(__GLW_LAST_ERROR, "glCompileShader");
            }
            __GLW_HANDLE(glAttachShader(*this, shader)) {
                glDeleteShader(shader);
                return handle_error(__GLW_LAST_ERROR, "glAttachShader");
            }

            // Mark shader for deletion after program destruction.
            glDeleteShader(shader);
        }

        __GLW_HANDLE(glLinkProgram(*this)) {
            return handle_error(__GLW_LAST_ERROR, "glLinkProgram");
        }

        if(getInfo<GL_LINK_STATUS>() == GL_FALSE) {
#ifdef __GLW_ENABLE_EXCEPTIONS
            throw BuildError(*this, this->log());
#endif
            return GL_INVALID_OPERATION;
        }

        // Setup attributes.
        attributes_.resize(getInfo<GL_ACTIVE_ATTRIBUTES>());
        for(int i = 0; i < getInfo<GL_ACTIVE_ATTRIBUTES>(); ++i) {
            Attribute attribute = {0};
            GLuint location;
            __GLW_HANDLE(glGetActiveAttrib(
                *this,
                i,
                Attribute::name_size,
                NULL,
                &attribute.size,
                &attribute.type,
                attribute.name)) {
                return handle_error(__GLW_LAST_ERROR, "glGetActiveAttrib");
            }
            __GLW_HANDLE(location = glGetAttribLocation(*this, attribute.name)) {
                return handle_error(__GLW_LAST_ERROR, "glGetAttribLocation");
            }
            attributes_[location] = attribute;
        }

        // Setup uniforms.
        uniforms_.resize(getInfo<GL_ACTIVE_UNIFORMS>());
        for(int i = 0; i < getInfo<GL_ACTIVE_UNIFORMS>(); ++i) {
            Uniform uniform = {0};
            GLuint location;
            __GLW_HANDLE(glGetActiveUniform(
                *this,
                i,
                Uniform::name_size,
                NULL,
                &uniform.size,
                &uniform.type,
                uniform.name)) {
                return handle_error(__GLW_LAST_ERROR, "glGetActiveUniform");
            }
            __GLW_HANDLE(location = glGetUniformLocation(*this, uniform.name)) {
                return handle_error(__GLW_LAST_ERROR, "glGetUniformLocation");
            }
            uniform.data.resize(sizeof_type(uniform.type) * uniform.size);
            uniforms_[location] = uniform;
        }

        return GL_NO_ERROR;
    }

    GLuint prepare()
    {
        if(prepareAttributes() != GL_NO_ERROR) {
            return handle_error(__GLW_LAST_ERROR, "Program::prepare");
        }
        if(prepareUniforms() != GL_NO_ERROR) {
            return handle_error(__GLW_LAST_ERROR, "Program::prepare");
        }
        return GL_NO_ERROR;
    }

    GLuint execute(
        const GLenum topology__, 
        const GLint offset__, 
        const GLint elements__)
    {
        __GLW_HANDLE(glUseProgram(*this)) {
            return handle_error(__GLW_LAST_ERROR, "glUseProgram");
        }
        if(prepare() != GL_NO_ERROR) {
            return handle_error(__GLW_LAST_ERROR, "Program::execute");
        }
        __GLW_HANDLE(glDrawArrays(topology__, offset__, elements__)) {
            return handle_error(__GLW_LAST_ERROR, "glDrawArrays");
        }
        return GL_NO_ERROR;
    }

    GLuint execute(
        const GLenum topology__, 
        const GLint elements__,
        const GLenum element_type__,
        const GLuint element_buffer__)
    {
        __GLW_HANDLE(glUseProgram(*this)) {
            return handle_error(__GLW_LAST_ERROR, "glUseProgram");
        }
        if(prepare() != GL_NO_ERROR) {
            return handle_error(__GLW_LAST_ERROR, "Program::execute");
        }
        __GLW_HANDLE(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer__)) {
            return handle_error(__GLW_LAST_ERROR, "glBindBuffer");
        }
        __GLW_HANDLE(glDrawElements(
            topology__,
            elements__,
            element_type__,
            0)) {
            return handle_error(__GLW_LAST_ERROR, "glDrawElements");
        }
        return GL_NO_ERROR;
    }

    std::string log()
    {
        std::string result;
        result.resize(getInfo<GL_INFO_LOG_LENGTH>());
        GLint size;
        __GLW_HANDLE(glGetProgramInfoLog(*this, result.size(), &size, &result[0])) {
            handle_error(__GLW_LAST_ERROR, "glGetProgramInfoLog");
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
            return handle_error(GL_INVALID_VALUE, "Program::setAttribute");
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
            return handle_error(GL_INVALID_VALUE, "Program::setUniform");
        }
        const size_t size = sizeof(T) * count__;
        if(size > sizeof_type(uniform->type) * uniform->size) {
            return handle_error(GL_INVALID_VALUE, "Program::setUniform");
        }
        memcpy(&uniform->data[0], &value__, size);
        uniform->dirty = true;
        return GL_NO_ERROR;
    }

    GLuint setSampler(
        const GLchar* name__,
        GLint unit__,
        GLuint texture__) 
    {
        Uniform* uniform = NULL;
        for(int i = 0; i < uniforms_.size(); ++i) {
            if(strcmp(uniforms_[i].name, name__) == 0) {
                uniform = &uniforms_[i];
                break;
            }
        }
        if(!uniform) {
            return handle_error(GL_INVALID_VALUE, "Program::setSampler");
        }
        const size_t size = sizeof(GLint);
        if(size > sizeof_type(uniform->type) * uniform->size) {
            return handle_error(GL_INVALID_VALUE, "Program::setSampler");
        }
        uniform->texture = texture__;
        memcpy(&uniform->data[0], &unit__, size);
        uniform->dirty = true;
        return GL_NO_ERROR;
    }

    template <GLenum Name>
    GLint getInfo() 
    {
        GLint result;
        __GLW_HANDLE(glGetProgramiv(*this, Name, &result)) {
            return handle_error(__GLW_LAST_ERROR, "glGetProgramiv");
        }
        return result;
    }

    const Attributes& attributes() const { return attributes_; }
    const Uniforms& uniforms() const { return uniforms_; }
};

} // namespace

#endif

