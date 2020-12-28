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

namespace leshaderconverter {
// -------------------------------------------------------------------
// Common utils
// -------------------------------------------------------------------
std::string ConvertToEntryPointString(const ShaderConverter::ConvertArguments::CompileTarget& target)
{
    static constexpr LPCSTR EntryPointStrings[] = {
        "",
        "VS_5_0",
        "VS_6_0",
        "PS_5_0",
        "PS_6_0"
    };
    return std::string(EntryPointStrings[static_cast<int>(target)]);
}

// -------------------------------------------------------------------
// ShaderCompiler_FXC
// -------------------------------------------------------------------
#if 1  // Region ShaderCompiler_FXC
ShaderCompilerInterface::CompileResult ShaderCompiler_FXC::Compile(const std::string &inputpath, const std::string& entrypoint, const std::string& target)
{
    Microsoft::WRL::ComPtr<ID3DBlob> code, errormsgs;
    static constexpr size_t WStrBufferSize = 0xffu;
    wchar_t winputpath[WStrBufferSize];
    size_t winputpath_convertednum = 0u;
    mbstowcs_s(&winputpath_convertednum, winputpath, inputpath.length(), inputpath.c_str(), _TRUNCATE);
    if (FAILED(D3DCompileFromFile(
        winputpath,
        nullptr,
        nullptr,
        entrypoint.c_str(),
        target.c_str(),
        0,
        0,
        &code,
        &errormsgs))) { // Failed
        wprintf(L"[ShaderCompiler_FXC] Compilation failed with errors:%hs\n", (const char*)errormsgs->GetBufferPointer());

        return CompileResult::CompilationFailed;
    }
    return CompileResult::OK;
}
#endif // Region ShaderCompiler_FXC
// -------------------------------------------------------------------
// ShaderCompiler_DXC
// -------------------------------------------------------------------
#if 1  // Region ShaderCompiler_DXC
ShaderCompilerInterface::CompileResult ShaderCompiler_DXC::Compile(const std::string& inputpath, const std::string& entrypoint, const std::string& target)
{
    assert(false);
    return CompileResult::OK;
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

    if (ShaderCompiler)
        ShaderCompiler->Compile(args.inputpath, args.entrypoint, ConvertToEntryPointString(args.target));

    // Release
    if (ShaderCompiler != nullptr) {
        delete ShaderCompiler;
        ShaderCompiler = nullptr;
    }

    return ConvertResult::OK;
}
} // namespace leshaderconverter