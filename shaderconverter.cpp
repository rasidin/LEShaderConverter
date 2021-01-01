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
@file  shaderconverter.cpp
@brief Shader converter implementation
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include "shaderconverter.h"

#include <windows.h>
#include <stdlib.h>
#include <wrl/client.h>  // for ComPtr
#include <cassert>
#include <d3dcompiler.h> // for fxc
#include <dxcapi.h>      // for dxc

#include "headergenerator.h"

namespace leshaderconverter {
// -------------------------------------------------------------------
// Common utils
// -------------------------------------------------------------------
std::string ConvertToEntryPointString(const ShaderConverter::ConvertArguments::CompileTarget& target)
{
    static constexpr LPCSTR EntryPointStrings[] = {
        "",
        "vs_5_0",
        "vs_6_0",
        "ps_5_0",
        "ps_6_0"
    };
    return std::string(EntryPointStrings[static_cast<int>(target)]);
}

ShaderVariable ConvertToShaderVariable(const D3D12_SHADER_VARIABLE_DESC& desc)
{
    ShaderVariable output;
    output.name = _strdup(desc.Name);
    output.startoffsetinbytes = desc.StartOffset;
    output.sizeinbytes = desc.Size;
    output.starttexture = desc.StartTexture;
    output.texturenum = desc.TextureSize;
    output.startsampler = desc.StartSampler;
    output.samplernum = desc.SamplerSize;
    return output;
}

ShaderBoundResource ConvertToShaderBoundResource(const D3D12_SHADER_INPUT_BIND_DESC& desc)
{
    static const ShaderBoundResource::InputType ConvertToShaderBoundResourceInputType[] = {
        ShaderBoundResource::InputType::CBuffer,                        // D3D_SIT_CBUFFER
        ShaderBoundResource::InputType::TBuffer,                        // D3D_SIT_TBUFFER
        ShaderBoundResource::InputType::Texture,                        // D3D_SIT_TBUFFER
        ShaderBoundResource::InputType::Sampler,                        // D3D_SIT_SAMPLER
        ShaderBoundResource::InputType::UAVRWTyped,                     // D3D_SIT_UAV_RWTYPED
        ShaderBoundResource::InputType::Structured,                     // D3D_SIT_STRUCTURED
        ShaderBoundResource::InputType::UAVRWStructured,                // D3D_SIT_UAV_RWSTRUCTURED
        ShaderBoundResource::InputType::ByteAddress,                    // D3D_SIT_BYTEADDRESS
        ShaderBoundResource::InputType::UAVRWByteAddress,               // D3D_SIT_UAV_RWBYTEADDRESS
        ShaderBoundResource::InputType::UAVAppendStructured,            // D3D_SIT_UAV_APPEND_STRUCTURED
        ShaderBoundResource::InputType::UAVConsumeStructured,           // D3D_SIT_UAV_CONSUME_STRUTURED
        ShaderBoundResource::InputType::UAVRWStrcturedWithCounter       // D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER
    };
    static const ShaderBoundResource::ReturnType ConvertToShaderBoundResourceReturnType[] = {
        ShaderBoundResource::ReturnType::Unknown,
        ShaderBoundResource::ReturnType::UNorm,     // D3D_RETURN_TYPE_UNORM
        ShaderBoundResource::ReturnType::SNorm,     // D3D_RETURN_TYPE_SNORM
        ShaderBoundResource::ReturnType::SInt,      // D3D_RETURN_TYPE_SINT
        ShaderBoundResource::ReturnType::UInt,      // D3D_RETURN_TYPE_UINT
        ShaderBoundResource::ReturnType::Float,     // D3D_RETURN_TYPE_FLOAT
        ShaderBoundResource::ReturnType::Mixed,     // D3D_RETURN_TYPE_MIXED
        ShaderBoundResource::ReturnType::Double,    // D3D_RETURN_TYPE_DOUBLE
        ShaderBoundResource::ReturnType::Continued  // D3D_RETURN_TYPE_CONTINUED
    };
    static const ShaderBoundResource::SRVDimensionType ConverToShaderBoundResourceSRVDimensionType[] = {
        ShaderBoundResource::SRVDimensionType::Unknown,             // D3D_SRV_DIMENSION_UNKNOWN
        ShaderBoundResource::SRVDimensionType::Buffer,              // D3D_SRV_DIMENSION_BUFFER
        ShaderBoundResource::SRVDimensionType::Texture1D,           // D3D_SRV_DIMENSION_TEXTURE1D
        ShaderBoundResource::SRVDimensionType::Texture1DArray,      // D3D_SRV_DIMENSION_TEXTURE1DARRAY
        ShaderBoundResource::SRVDimensionType::Texture2D,           // D3D_SRV_DIMENSION_TEXTURE2D
        ShaderBoundResource::SRVDimensionType::Texture2DArray,      // D3D_SRV_DIMENSION_TEXTURE2DARRAY
        ShaderBoundResource::SRVDimensionType::Texture2DMS,         // D3D_SRV_DIMENSION_TEXTURE2DMS
        ShaderBoundResource::SRVDimensionType::Texture2DMSArray,    // D3D_SRV_DIMENSION_TEXTURE2DMSARRAY
        ShaderBoundResource::SRVDimensionType::Texture3D,           // D3D_SRV_DIMENSION_TEXTURE3D
        ShaderBoundResource::SRVDimensionType::TextureCube,         // D3D_SRV_DIMENSION_TEXTURECUBE
        ShaderBoundResource::SRVDimensionType::TextureCubeArray,    // D3D_SRV_DIMENSION_TEXTURECUBEARRAY
    };

    ShaderBoundResource output;
    output.name = _strdup(desc.Name);
    output.inputtype = ConvertToShaderBoundResourceInputType[static_cast<uint32_t>(desc.Type)];
    output.bindpoint = desc.BindPoint;
    output.bindnum = desc.BindCount;
    output.returntype = ConvertToShaderBoundResourceReturnType[static_cast<uint32_t>(desc.ReturnType)];
    output.srvdimensiontype = ConverToShaderBoundResourceSRVDimensionType[static_cast<uint32_t>(desc.Dimension)];
    output.samplenum = desc.NumSamples;
    return output;
}

uint32_t ConvertToD3DCOMPILEFlags(uint32_t options)
{
    uint32_t output = 0u;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::Debug)) output |= D3DCOMPILE_DEBUG;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::SkipValidation)) output |= D3DCOMPILE_SKIP_VALIDATION;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::SkipOptimization)) output |= D3DCOMPILE_SKIP_OPTIMIZATION;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::PartialPrecision)) output |= D3DCOMPILE_PARTIAL_PRECISION;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::OptimizationLevel0)) output |= D3DCOMPILE_OPTIMIZATION_LEVEL0;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::OptimizationLevel1)) output |= D3DCOMPILE_OPTIMIZATION_LEVEL1;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::OptimizationLevel2)) output |= D3DCOMPILE_OPTIMIZATION_LEVEL2;
    if (options & static_cast<uint32_t>(ShaderCompilerInterface::CompileOptions::OptimizationLevel3)) output |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
    return output;
}

// -------------------------------------------------------------------
// ShaderVariable
// -------------------------------------------------------------------
void ShaderVariable::Release()
{
    if (name) ::free(name);
    name = nullptr;
}

// -------------------------------------------------------------------
// ShaderBoundResource
// -------------------------------------------------------------------
void ShaderBoundResource::Release()
{
    if (name) ::free(name);
    name = nullptr;
}

// -------------------------------------------------------------------
// ShaderCompiler_FXC
// -------------------------------------------------------------------
#if 1  // Region ShaderCompiler_FXC
ShaderCompilationResult ShaderCompiler_FXC::Compile(const std::string &inputpath, const std::string& entrypoint, const std::string& target, uint32_t options)
{
    ShaderCompilationResult result;

    Microsoft::WRL::ComPtr<ID3DBlob> code, errormsgs;

    static constexpr size_t kWStrBufferSize = 0xffu;
    wchar_t winputpath[kWStrBufferSize];
    size_t winputpath_convertednum = 0u;
    mbstowcs_s(&winputpath_convertednum, winputpath, inputpath.size() + 1, inputpath.c_str(), _TRUNCATE);
    if (SUCCEEDED(D3DCompileFromFile(
        winputpath,
        nullptr,
        nullptr,
        entrypoint.c_str(),
        target.c_str(),
        ConvertToD3DCOMPILEFlags(options),
        0u,
        code.GetAddressOf(),
        errormsgs.GetAddressOf()))) { // Succeeded
        Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shaderreflection;
        if (SUCCEEDED(D3DReflect(code->GetBufferPointer(), code->GetBufferSize(), IID_PPV_ARGS(&shaderreflection)))) {
            D3D12_SHADER_DESC shaderdesc;
            shaderreflection->GetDesc(&shaderdesc);
            for (uint32_t cbidx = 0; cbidx < shaderdesc.ConstantBuffers; cbidx++) {
                ID3D12ShaderReflectionConstantBuffer* constantbuffer = shaderreflection->GetConstantBufferByIndex(cbidx);
                D3D12_SHADER_BUFFER_DESC sbdesc;
                constantbuffer->GetDesc(&sbdesc);

                ShaderConstantBuffer& newconstantbuffer = result.AddConstantBuffer();
                for (uint32_t varidx = 0; varidx < sbdesc.Variables; varidx++) {
                    ID3D12ShaderReflectionVariable* srvar = constantbuffer->GetVariableByIndex(varidx);
                    D3D12_SHADER_VARIABLE_DESC vardesc;
                    if (SUCCEEDED(srvar->GetDesc(&vardesc))) {
                        newconstantbuffer.AddShaderVariable(ConvertToShaderVariable(vardesc));
                    }
                }
            }
            for (uint32_t rbidx = 0; rbidx < shaderdesc.BoundResources; rbidx++) {
                D3D12_SHADER_INPUT_BIND_DESC ibdesc;
                shaderreflection->GetResourceBindingDesc(rbidx, &ibdesc);
            }

            result.code.reset(new char[code->GetBufferSize()]);
            ::memcpy(result.code.get(), code->GetBufferPointer(), code->GetBufferSize());
            result.codelength = code->GetBufferSize();
            result.resultcode = ShaderCompilationResult::CompileResultValue::OK;
        }
        else {
            wprintf(L"[ShaderCompiler_FXC] Failed to get reflection of compiled shader.\n");
        }
    }
    else { // Failed
        if (errormsgs.Get() != nullptr) {
            wprintf(L"[ShaderCompiler_FXC] Compilation failed with errors:%hs\n", (const char*)errormsgs->GetBufferPointer());
            result.resultcode = ShaderCompilationResult::CompileResultValue::CompilationFailed;
        }
        else {
            result.resultcode = ShaderCompilationResult::CompileResultValue::NotFound;
        }
        return result;
    }
    return result;
}
#endif // Region ShaderCompiler_FXC

// -------------------------------------------------------------------
// ShaderCompiler_DXC
// -------------------------------------------------------------------
#if 1  // Region ShaderCompiler_DXC
ShaderCompilationResult ShaderCompiler_DXC::Compile(const std::string& inputpath, const std::string& entrypoint, const std::string& target, uint32_t options)
{
    assert(false); // Unimplemented
    ShaderCompilationResult result;

    return result;
}
#endif // Region ShaderCompiler_DXC

ShaderConverter::ConvertResult ShaderConverter::Convert(const ConvertArguments& args)
{
    ShaderCompilerInterface* ShaderCompiler = nullptr;

    // Create
    switch (args.target)
    {
    case ConvertArguments::CompileTarget::VS_5_0:
    case ConvertArguments::CompileTarget::PS_5_0:
        ShaderCompiler = new ShaderCompiler_FXC();
        break;
    case ConvertArguments::CompileTarget::VS_6_0:
    case ConvertArguments::CompileTarget::PS_6_0:
        ShaderCompiler = new ShaderCompiler_DXC();
        break;
    }

    if (ShaderCompiler) {
        ShaderCompilationResult result = ShaderCompiler->Compile(args.inputpath, args.entrypoint, ConvertToEntryPointString(args.target), args.options);
        if (result.IsValid()) {
            std::string outputheaderpath = args.outputpath + ".h";
            if (HeaderGenerator::WriteToFile(outputheaderpath.data(), args.classname.data(), result) != HeaderGenerator::GenerateResult::OK) {
                return ConvertResult::HeaderGenerationFailed;
            }
        }
        else {
            return ConvertResult::ShaderComplationFailed;
        }
    }

    // Release
    if (ShaderCompiler != nullptr) {
        delete ShaderCompiler;
        ShaderCompiler = nullptr;
    }

    return ConvertResult::OK;
}
} // namespace leshaderconverter