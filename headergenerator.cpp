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

    outstream << "namespace LimitEngine {" << std::endl;

    outstream << "class " << classname << std::endl;
    outstream << "{" << std::endl;

    // codebin (just definition)
    outstream << "    static const char codebin[];" << std::endl;
    
    // codesize
    snprintf(tempstrbuf, sizeof(tempstrbuf), "    static constexpr size_t codesize = %du;", result.codelength);
    outstream << tempstrbuf << std::endl;

    // GetCodeBin
    outstream << "    static const char* GetCodeBin() { return codebin; }" << std::endl;

    // GetCodeSize
    outstream << "    static size_t GetCodeSize() { return codesize; }" << std::endl;

    outstream << "};" << std::endl;

    // codebin (data)
    outstream << "const char " << classname << "::codebin[] = {" << std::endl;
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

    outstream << "} // namespace LimitEngine" << std::endl;
    outstream << "#endif" << std::endl;

    return GenerateResult::OK;
}
} // namespace leshaderconverter