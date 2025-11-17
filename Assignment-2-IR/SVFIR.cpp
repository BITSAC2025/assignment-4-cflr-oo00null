/**
 * SVFIR.cpp
 * @author kisslune
 */

#include "Graphs/SVFG.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include <cstdlib>
#include <string>
#include <vector>

using namespace SVF;
using namespace llvm;
using namespace std;

int main(int argc, char** argv)
{
    int arg_num = 0;
    int extraArgc = 4;
    char** arg_value = new char*[argc + extraArgc];
    for (; arg_num < argc; ++arg_num) {
        arg_value[arg_num] = argv[arg_num];
    }
    std::vector<std::string> moduleNameVec;

    int orgArgNum = arg_num;
    arg_value[arg_num++] = (char*)"-model-arrays=true";
    arg_value[arg_num++] = (char*)"-pre-field-sensitive=false";
    arg_value[arg_num++] = (char*)"-model-consts=true";
    arg_value[arg_num++] = (char*)"-stat=false";
    assert(arg_num == (orgArgNum + extraArgc) && "more extra arguments? Change the value of extraArgc");

    moduleNameVec = OptionBase::parseOptions(arg_num, arg_value, "SVF IR", "[options] <input-bitcode...>");

    std::vector<std::string> processed;
    for (const auto &f : moduleNameVec)
    {
        if (f.size() > 2 && f.substr(f.size() - 2) == ".c")
        {
            std::string out = f.substr(0, f.size() - 2) + ".ll";
            std::string cmd = std::string("clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone -fno-discard-value-names ") + f + " -o " + out;
            std::system(cmd.c_str());
            processed.push_back(out);
        }
        else
            processed.push_back(f);
    }

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(processed);
    svfModule->buildSymbolTableInfo();

    SVFIRBuilder builder(svfModule);
    cout << "Generating SVFIR(PAG), call graph and ICFG ..." << endl;

    SVFIR* pag = builder.build();
    if (!pag)
        return 1;

    return 0;
}
