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
@file  shaderconverter.h
@brief Shader converter implementation
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#include "arguments.h"

namespace leshaderconverter {
bool Arguments::IsValid() const 
{
    if (inputfilename.empty())
        return false;
    return true;
}

void Arguments::PrintInfo() const
{
    if (inputfilename.length() > 0)
        printf("Input = %s\n", inputfilename.data());
    if (outputfilename.length() > 0)
        printf("Output = %s\n", outputfilename.data());
}

Arguments ArgumentsFactory::CreateArguments(int argc, char** argv)
{
    Arguments output;

    if (argc > 1) {
        for (int argidx = 1; argidx < argc; argidx++) {
            if (argv[argidx][0] == '-') { // Option
            }
            else if (output.inputfilename.empty()) { // Input file
                output.inputfilename = argv[argidx];
            }
            else { // Output file
                output.outputfilename = argv[argidx];
            }
        }
    }

    return output;
}
}