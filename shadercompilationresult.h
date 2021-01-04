/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  shadercompilationresult.h
@brief Compilation result of shader
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#ifndef LESHADERCONVERTER_SHADERCOMPILATIONRESULT_H_
#define LESHADERCONVERTER_SHADERCOMPILATIONRESULT_H_

#include <memory>
#include <vector>

namespace leshaderconverter {
struct ShaderVariable
{
    std::unique_ptr<char>   name;                       // Name of the variable
    uint32_t                startoffsetinbytes = 0u;    // Offset in constant buffer
    uint32_t                sizeinbytes = 0u;           // Size of variable (in bytes)
    uint32_t                starttexture = -1;          // First texture index (-1 if no textures used)
    uint32_t                texturenum = 0u;            // Number of textures
    uint32_t                startsampler = -1;          // First sampler index (-1 if no samplers used)
    uint32_t                samplernum = 0;             // Number of samplers
};

struct ShaderConstantBuffer
{
    size_t size = 0u;
    std::vector<ShaderVariable> variables;

    inline ShaderVariable& AddShaderVariable() { variables.push_back(ShaderVariable()); return variables.back(); }
};

struct ShaderBoundResource
{
    std::unique_ptr<char> name;
    enum class InputType
    {
        CBuffer = 0,
        TBuffer,
        Texture,
        Sampler,
        UAVRWTyped,
        Structured,
        UAVStructured,
        UAVRWStructured,
        ByteAddress,
        UAVRWByteAddress,
        UAVAppendStructured,
        UAVConsumeStructured,
        UAVRWStrcturedWithCounter
    } inputtype;
    uint32_t bindpoint;
    uint32_t bindnum;
    enum class ReturnType
    {
        Unknown = 0,
        UNorm,
        SNorm,
        SInt,
        UInt,
        Float,
        Mixed,
        Double,
        Continued
    } returntype;
    enum class SRVDimensionType
    {
        Unknown = 0,
        Buffer,
        Texture1D,
        Texture1DArray,
        Texture2D,
        Texture2DArray,
        Texture2DMS,
        Texture2DMSArray,
        Texture3D,
        TextureCube,
        TextureCubeArray
    } srvdimensiontype;
    uint32_t samplenum;
};

struct ShaderCompilationResult
{
    enum class CompileResultValue {
        OK = 0,
        CompilationFailed,
        NotFound
    } resultcode = CompileResultValue::CompilationFailed;

    std::unique_ptr<char> code;
    size_t codelength = 0u;

    std::vector<ShaderConstantBuffer> constantbuffers;
    std::vector<ShaderBoundResource>  boundresources;

    inline bool IsValid() const { return resultcode == CompileResultValue::OK && code.get() && codelength > 0u; }

    inline ShaderConstantBuffer& AddConstantBuffer() { constantbuffers.push_back(ShaderConstantBuffer()); return constantbuffers.back(); }
    inline ShaderBoundResource& AddBoundResources() { boundresources.push_back(ShaderBoundResource()); return boundresources.back(); }
};
} // namespace leshaderconverter

#endif