#ifndef __GLW_SAMPLER_HPP
#define __GLW_SAMPLER_HPP

#include "glw.hpp"

struct SamplerReference : public Reference
{
    SamplerReference() { __GLW_HANDLE(glGenSamplers(1, &handle)) {} }
    ~SamplerReference() { if(handle) glDeleteSamplers(1, &handle); }
};

class Sampler : public Wrapper<SamplerReference>
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
            if(error) *error = handleError(__GLW_LAST_ERROR, "glSamplerParameteri");
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_MAG_FILTER, mag_filter__)) {
            if(error) *error = handleError(__GLW_LAST_ERROR, "glSamplerParameteri");
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_WRAP_S, wrap__)) {
            if(error) *error = handleError(__GLW_LAST_ERROR, "glSamplerParameteri");
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_WRAP_T, wrap__)) {
            if(error) *error = handleError(__GLW_LAST_ERROR, "glSamplerParameteri");
        }
        __GLW_HANDLE(glSamplerParameteri(*this, GL_TEXTURE_WRAP_R, wrap__)) {
            if(error) *error = handleError(__GLW_LAST_ERROR, "glSamplerParameteri");
        }
    }
};

#endif

