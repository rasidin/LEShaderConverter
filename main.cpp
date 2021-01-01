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
@file  main.cpp
@brief Main file
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include <stdio.h>

#include "arguments.h"
#include "shaderconverter.h"

enum class LEShaderConverterReturnCode
{
    OK = 0,
    InvalidArguments,
    CompilationFailed,
};

enum class LEShaderConverterVersion
{
    Version0_1 = 0,

    Lastest = Version0_1
};

static const char* LEShaderConverterVersionName[] = {
    "Version.0.1"
};

uint32_t ConvertToCompileOptions(const std::vector<std::string>& options)
{
    uint32_t output = 0u;
    for (std::vector<std::string>::const_iterator it = options.begin(); it != options.end(); ++it) {
        if (it->length() > 0) {
            if (it->compare("d") == 0 || it->compare("D") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::Debug);
            }
            else if (it->compare("sv") == 0 || it->compare("SV") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::SkipValidation);
            }
            else if (it->compare("so") == 0 || it->compare("SO") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::SkipOptimization);
            }
            else if (it->compare("pp") == 0 || it->compare("PP") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::PartialPrecision);
            }
            else if (it->compare("o0") == 0 || it->compare("O0") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::OptimizationLevel0);
            }
            else if (it->compare("o1") == 0 || it->compare("O1") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::OptimizationLevel1);
            }
            else if (it->compare("o2") == 0 || it->compare("O2") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::OptimizationLevel2);
            }
            else if (it->compare("o3") == 0 || it->compare("O3") == 0) {
                output |= static_cast<uint32_t>(leshaderconverter::ShaderCompilerInterface::CompileOptions::OptimizationLevel3);
            }
        }
    }
    return output;
}

void PrintUsage()
{
    printf("LEShaderConverter %s\n", LEShaderConverterVersionName[static_cast<int>(LEShaderConverterVersion::Lastest)]);
    printf("Progarammed by minseob (leeminseob@outlook.com)\n");
    printf("\n");
    printf("Usage : LEShaderConverter <ShaderFilePath> <OutputPath>\n");
}

leshaderconverter::ShaderConverter::ConvertArguments::CompileTarget ConvertStringToCompileTarget(const std::string& strtarget)
{
    if (strtarget.compare("VS_5_0") == 0) {
        return leshaderconverter::ShaderConverter::ConvertArguments::CompileTarget::VS_5_0;
    }
    else if (strtarget.compare("VS_6_0") == 0) {
        return leshaderconverter::ShaderConverter::ConvertArguments::CompileTarget::VS_6_0;
    }
    else if (strtarget.compare("PS_5_0") == 0) {
        return leshaderconverter::ShaderConverter::ConvertArguments::CompileTarget::PS_5_0;
    }
    else if (strtarget.compare("PS_6_0") == 0) {
        return leshaderconverter::ShaderConverter::ConvertArguments::CompileTarget::PS_6_0;
    }
    return leshaderconverter::ShaderConverter::ConvertArguments::CompileTarget::None;
}

int main(int argc, char **argv)
{
    leshaderconverter::Arguments arguments = leshaderconverter::ArgumentsFactory::CreateArguments(argc, argv);
    if (arguments.IsValid() == false) {
        PrintUsage();
        printf("[Arguments]\n");
        arguments.PrintInfo();
        return static_cast<int>(LEShaderConverterReturnCode::InvalidArguments);
    }

    arguments.PrintInfo();

    printf("Compile %s...\n", arguments.inputfilename.data());

    leshaderconverter::ShaderConverter shaderconverter;
    if (shaderconverter.Convert(leshaderconverter::ShaderConverter::ConvertArguments(
        arguments.inputfilename,
        arguments.outputfilename,
        arguments.name,
        arguments.entrypoint,
        ConvertStringToCompileTarget(arguments.target),
        ConvertToCompileOptions(arguments.options)
    )) != leshaderconverter::ShaderConverter::ConvertResult::OK) {
        return static_cast<int>(LEShaderConverterReturnCode::CompilationFailed);
    }

    return static_cast<int>(LEShaderConverterReturnCode::OK);
}