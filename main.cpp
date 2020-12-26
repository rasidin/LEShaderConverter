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
};

enum class LEShaderConverterVersion
{
    Version0_1 = 0,

    Lastest = Version0_1
};

static const char* LEShaderConverterVersionName[] = {
    "Version.0.1"
};

void PrintUsage()
{
    printf("LEShaderConverter %s\n", LEShaderConverterVersionName[static_cast<int>(LEShaderConverterVersion::Lastest)]);
    printf("Progarammed by minseob (leeminseob@outlook.com)\n");
    printf("\n");
    printf("Usage : LEShaderConverter <ShaderFilePath> <OutputPath>\n");
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

    return static_cast<int>(LEShaderConverterReturnCode::OK);
}