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
#ifndef LESHADERCONVERTER_SHADERCONVERTER_H_
#define LESHADERCONVERTER_SHADERCONVERTER_H_

#include <string>
#include <vector>

#include "shadercompilationresult.h"

namespace leshaderconverter {
class ShaderCompilerInterface
{
public:
    enum class CompileOptions {
        None                = 0,
        Debug               = 1 << 0,
        SkipValidation      = 1 << 1,
        SkipOptimization    = 1 << 2,
        PartialPrecision    = 1 << 5,
        OptimizationLevel0  = 1 <<14,
        OptimizationLevel1  = 1 <<15,
        OptimizationLevel2  = 1 <<16,
        OptimizationLevel3  = 1 <<17,
    };

public:
    virtual ShaderCompilationResult Compile(const std::string& inputpath, const std::string& entrypoint, const std::string& target, uint32_t options) = 0;
};

class ShaderCompiler_FXC : public ShaderCompilerInterface
{
public:
    virtual ShaderCompilationResult Compile(const std::string& inputpath, const std::string& entrypoint, const std::string& target, uint32_t options) override;
};

class ShaderCompiler_DXC : public ShaderCompilerInterface
{
public:
    virtual ShaderCompilationResult Compile(const std::string& inputpath, const std::string& entrypoint, const std::string& target, uint32_t options) override;
};

class ShaderConverter
{
public:
    enum class ConvertResult {
        OK = 0,
        FailedToOpenFile,
        ShaderComplationFailed,
        HeaderGenerationFailed,
    };
    
    struct ConvertArguments {
        enum class CompileTarget {
            None = 0,
            VS_5_0,     // fxc
            VS_6_0,         // dxc
            PS_5_0,         // fxc
            PS_6_0          // dxc
        } target = CompileTarget::None;
        std::string inputpath;
        std::string outputpath;
        std::string classname;
        std::string entrypoint;
        uint32_t options;
        
        ConvertArguments(
            const std::string& in_inputpath,
            const std::string& in_outputpath,
            const std::string& in_classname,
            const std::string& in_entrypoint,
            const CompileTarget& in_target,
            uint32_t in_options)
            : inputpath(in_inputpath)
            , outputpath(in_outputpath)
            , classname(in_classname)
            , entrypoint(in_entrypoint)
            , target(in_target)
            , options(in_options)
        {}
    };

public:
    ShaderConverter() {}
    virtual ~ShaderConverter() {}

    ConvertResult Convert(const ConvertArguments &args);
};
} // namespace leshaderconverter

#endif