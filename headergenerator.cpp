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
@file  headergenerator.cpp
@brief Header for shader generator
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include "headergenerator.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include "md5.h"

namespace leshaderconverter {
HeaderGenerator::GenerateResult HeaderGenerator::WriteToFile(const char *filepath, const char *classname, const ShaderCompilationResult &result)
{
    std::ofstream outstream(filepath);

    if (!outstream) {
        return GenerateResult::FailedToOpenFile;
    }

    char tempstrbuf[0xff] = { 0 };
    const char* codeptr = static_cast<const char*>(result.code.get());
    assert(codeptr);

    outstream << "// Genarated by LEShaderConverter" << std::endl;
    outstream << "#ifndef _SHADER_" << classname << "_H_" << std::endl;
    outstream << "#define _SHADER_" << classname << "_H_" << std::endl << std::endl;

    outstream << "#include \"Renderer/Shader.h\"" << std::endl << std::endl;

    outstream << "namespace LimitEngine {" << std::endl;

    outstream << "class " << classname << " : public Shader" << std::endl;
    outstream << "{" << std::endl;

    // codebin (just definition)
    outstream << "    static const uint8 codebin[];" << std::endl;
    
    // codesize
    snprintf(tempstrbuf, sizeof(tempstrbuf), "    static constexpr size_t codesize = %du;", result.codelength);
    outstream << tempstrbuf << std::endl << std::endl;

    // bound resource
    std::vector<char*> texturenames;
    std::vector<char*> samplernames;
    std::vector<char*> uavnames;
    uint32_t currentboundresourceindex = 0u;
    for (uint32_t bridx = 0; bridx < result.boundresources.size(); bridx++) {
        const ShaderBoundResource& currentboundresource = result.boundresources[bridx];
        if (currentboundresource.inputtype == ShaderBoundResource::InputType::Texture) {
            assert(texturenames.size() <= currentboundresource.bindpoint);
            while (texturenames.size() != currentboundresource.bindpoint) {
                texturenames.push_back(nullptr);
            }
            texturenames.push_back(currentboundresource.name.get());
        }
        else if (currentboundresource.inputtype == ShaderBoundResource::InputType::Sampler) {
            assert(samplernames.size() <= currentboundresource.bindpoint);
            while (samplernames.size() != currentboundresource.bindpoint) {
                samplernames.push_back(nullptr);
            }
            samplernames.push_back(currentboundresource.name.get());
        }
        else if (currentboundresource.inputtype == ShaderBoundResource::InputType::UAVAppendStructured ||
                 currentboundresource.inputtype == ShaderBoundResource::InputType::UAVConsumeStructured ||
                 currentboundresource.inputtype == ShaderBoundResource::InputType::UAVRWByteAddress ||
                 currentboundresource.inputtype == ShaderBoundResource::InputType::UAVRWStrcturedWithCounter ||
                 currentboundresource.inputtype == ShaderBoundResource::InputType::UAVRWStructured ||
                 currentboundresource.inputtype == ShaderBoundResource::InputType::UAVRWTyped ||
                 currentboundresource.inputtype == ShaderBoundResource::InputType::UAVStructured) {
            uavnames.push_back(currentboundresource.name.get());
        }
    }

    if (texturenames.size() > 0) {
        outstream << "    static const char* texturenames[];" << std::endl;
    }

    if (samplernames.size() > 0) {
        outstream << "    static const char* samplernames[];" << std::endl;
    }

    if (uavnames.size() > 0) {
        outstream << "    static const char* uavnames[];" << std::endl;
    }

    if (texturenames.size() > 0 || samplernames.size() > 0)
        outstream << std::endl;

    // constant buffer structure
    outstream << "public:" << std::endl;
    for (uint32_t cbidx = 0; cbidx < result.constantbuffers.size(); cbidx++) {
        const ShaderConstantBuffer& currentconstantbuffer = result.constantbuffers[cbidx];

        uint32_t currentoffsetinbytes = 0u;
        uint32_t currentpaddingindex = 0u;
        snprintf(tempstrbuf, sizeof(tempstrbuf), "ConstantBuffer%d", cbidx);
        outstream << "    struct " << tempstrbuf << " {" << std::endl;
        for (uint32_t varidx = 0; varidx < currentconstantbuffer.variables.size(); varidx++) {
            const ShaderVariable& currentvariable = currentconstantbuffer.variables[varidx];
            if (currentvariable.sizeinbytes == 0) continue;
            assert(currentvariable.startoffsetinbytes >= currentoffsetinbytes);

            if (currentvariable.startoffsetinbytes > currentoffsetinbytes) {
                snprintf(tempstrbuf, sizeof(tempstrbuf), "padding%d[%d]", currentpaddingindex++, currentvariable.startoffsetinbytes - currentoffsetinbytes);
                outstream << "        uint8 " << tempstrbuf << ";" << std::endl;
                currentoffsetinbytes = currentvariable.startoffsetinbytes;
            }
            assert(currentvariable.sizeinbytes % 4 == 0);
            snprintf(tempstrbuf, sizeof(tempstrbuf), "%s[%d]", currentvariable.name.get(), currentvariable.sizeinbytes / 4);
            outstream << "        float " << tempstrbuf << ";" << std::endl;
            currentoffsetinbytes += currentvariable.sizeinbytes;
        }
        outstream << "    };" << std::endl;
    }

    outstream << "public:" << std::endl;
    // Static interfaces
    // GetHash
    MD5 shaderhash = MD5::Generate(result.code.get(), result.codelength);
    snprintf(tempstrbuf, sizeof(tempstrbuf), "0x%08x, 0x%08x, 0x%08x, 0x%08x", shaderhash.data32[0], shaderhash.data32[1], shaderhash.data32[2], shaderhash.data32[3]);
    outstream << "    static ShaderHash GetHash() { return ShaderHash(" << tempstrbuf << "); }" << std::endl;

    // Virtual interfaces
    // GetShaderHash
    outstream << "    virtual       ShaderHash GetShaderHash() const override { return GetHash(); }" << std::endl;
    // GetName
    outstream << "    virtual const String GetName() const override { return String(\"" << classname << "\"); }" << std::endl;

    // GetCodeBin
    outstream << "    virtual const uint8* GetCompiledCodeBin() const override { return &codebin[0]; }" << std::endl;

    // GetCodeSize
    outstream << "    virtual const size_t GetCompiledCodeSize() const override { return codesize; }" << std::endl;

    // GetConstantBufferCount
    outstream << "    virtual const uint32 GetConstantBufferCount() const override { return " << static_cast<uint32_t>(result.constantbuffers.size()) << "; }" << std::endl;

    // GetBoundTextureCount
    outstream << "    virtual const uint32 GetBoundTextureCount() const override { return " << static_cast<uint32_t>(texturenames.size()) << "; }" << std::endl;

    // GetBoundSamplerCount
    outstream << "    virtual const uint32 GetBoundSamplerCount() const override { return " << static_cast<uint32_t>(samplernames.size()) << "; }" << std::endl;

    // GetBoundUAVCount
    outstream << "    virtual const uint32 GetUAVCount() const override { return " << static_cast<uint32_t>(uavnames.size()) << "; }" << std::endl;

    outstream << "};" << std::endl;

    // codebin (data)
    outstream << "const uint8_t " << classname << "::codebin[] = {" << std::endl;
    for (uint32_t codebinidx = 0; codebinidx < result.codelength; codebinidx++) {
        if (codebinidx % 32 == 0u) {
            outstream << "    ";
        }
        if (codebinidx < result.codelength - 1) {
            snprintf(tempstrbuf, sizeof(tempstrbuf), "0x%02x, ", codeptr[codebinidx] & 0xff);
            outstream << tempstrbuf;
            if (codebinidx % 32 == 31u)
                outstream << std::endl;
        }
        else { // end of code
            snprintf(tempstrbuf, sizeof(tempstrbuf), "0x%02x };", codeptr[codebinidx] & 0xff);
            outstream << tempstrbuf << std::endl;
        }
    }

    // textures (data)
    if (texturenames.size() > 0) {
        outstream << "const char* " << classname << "::texturenames[] = {" << std::endl;
        for (std::vector<char*>::iterator it = texturenames.begin(); it != texturenames.end(); ++it) {
            if (*it) {
                outstream << "    \"" << *it << "\"," << std::endl;
            }
            else {
                outstream << "    \"\"," << std::endl;
            }
        }
        outstream << "};" << std::endl;
    }
    // samplers (data)
    if (samplernames.size() > 0) {
        outstream << "const char* " << classname << "::samplernames[] = {" << std::endl;
        for (std::vector<char*>::iterator it = samplernames.begin(); it != samplernames.end(); ++it) {
            if (*it) {
                outstream << "    \"" << *it << "\"," << std::endl;
            }
            else {
                outstream << "    \"\"," << std::endl;
            }
        }
        outstream << "};" << std::endl;
    }
    // UAVs (data)
    if (uavnames.size() > 0) {
        outstream << "const char* " << classname << "::uavnames[] = {" << std::endl;
        for (std::vector<char*>::iterator it = uavnames.begin(); it != uavnames.end(); ++it) {
            if (*it) {
                outstream << "    \"" << *it << "\"," << std::endl;
            }
            else {
                outstream << "    \"\"," << std::endl;
            }
        }
        outstream << "};" << std::endl;
    }

    outstream << "} // namespace LimitEngine" << std::endl;
    outstream << "#endif" << std::endl;

    return GenerateResult::OK;
}
} // namespace leshaderconverter