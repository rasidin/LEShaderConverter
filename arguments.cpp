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
@author minseob (https://github.com/rasidin)
**********************************************************************/
#include "arguments.h"

#include <cassert>

namespace leshaderconverter {
bool Arguments::IsValid() const 
{
    if (inputfilename.empty() || name.empty() || entrypoint.empty() || target.empty())
        return false;
    return true;
}

void Arguments::PrintInfo() const
{
    if (inputfilename.length() > 0)
        printf("Input = %s\n", inputfilename.data());
    if (outputfilename.length() > 0)
        printf("Output = %s\n", outputfilename.data());
    if (name.length() > 0)
        printf("Name = %s\n", outputfilename.data());
    if (entrypoint.length() > 0)
        printf("Entry = %s\n", entrypoint.data());
    if (target.length() > 0)
        printf("Target = %s\n", target.data());
    for (uint32_t optidx = 0; optidx < options.size(); optidx++) {
        printf("Option[%d] = %s\n", optidx, options[optidx].data());
    }
}

Arguments ArgumentsFactory::CreateArguments(int argc, char** argv)
{
    Arguments output;

    if (argc > 1) {
        for (int argidx = 1; argidx < argc; argidx++) {
            if (argv[argidx][0] == '-') { // Option
                if (argv[argidx][1] == 'E') {
                    output.entrypoint = &argv[argidx][3];
                }
                else if (argv[argidx][1] == 'T') {
                    output.target = &argv[argidx][3];
                }
                else if (argv[argidx][1] == 'N') {
                    output.name = &argv[argidx][3];
                }
                else {
                    output.options.push_back(std::string(&argv[argidx][1]));
                }
            }
            else if (output.inputfilename.empty()) { // Input file
                output.inputfilename = argv[argidx];
            }
            else { // Output file
                output.outputfilename = argv[argidx];
            }
        }
        if (output.outputfilename.length() == 0) { // Generate Output filename
            assert(false); // Unimplemented
        }
    }

    return output;
}
}