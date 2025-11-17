/**
 * CFLR.cpp
 * @author kisslune 
 */

#include "A4Header.h"

using namespace SVF;
using namespace llvm;
using namespace std;

int main(int argc, char **argv)
{
    auto moduleNameVec =
            OptionBase::parseOptions(argc, argv, "Whole Program Points-to Analysis",
                                     "[options] <input-bitcode...>");

    LLVMModuleSet::buildSVFModule(moduleNameVec);

    SVFIRBuilder builder;
    auto pag = builder.build();
    //pag->dump();

    CFLR solver;
    solver.buildGraph(pag);
    // TODO: complete this method
    solver.solve();
    solver.dumpResult();

    LLVMModuleSet::releaseLLVMModuleSet();
    return 0;
}


void CFLR::solve()
{
    auto &succ = graph->getSuccessorMap();
    auto &pred = graph->getPredecessorMap();

    for (auto &srcItr : succ)
    {
        unsigned src = srcItr.first;
        auto it = srcItr.second.find(Addr);
        if (it != srcItr.second.end())
        {
            for (auto dst : it->second)
            {
                if (!graph->hasEdge(src, dst, PT))
                {
                    graph->addEdge(src, dst, PT);
                    workList.push(CFLREdge(src, dst, PT));
                }
            }
        }
    }

    while (!workList.empty())
    {
        CFLREdge e = workList.pop();
        if (e.label == PT)
        {
            unsigned u = e.src;
            unsigned o = e.dst;

            auto cf = succ[u].find(Copy);
            if (cf != succ[u].end())
            {
                for (auto v : cf->second)
                {
                    if (!graph->hasEdge(v, o, PT))
                    {
                        graph->addEdge(v, o, PT);
                        workList.push(CFLREdge(v, o, PT));
                    }
                }
            }

            auto cb = pred[u].find(Copy);
            if (cb != pred[u].end())
            {
                for (auto v : cb->second)
                {
                    if (!graph->hasEdge(v, o, PT))
                    {
                        graph->addEdge(v, o, PT);
                        workList.push(CFLREdge(v, o, PT));
                    }
                }
            }

            auto sp = pred[u].find(Store);
            if (sp != pred[u].end())
            {
                for (auto y : sp->second)
                {
                    auto yt = succ[y].find(PT);
                    if (yt != succ[y].end())
                    {
                        for (auto t : yt->second)
                        {
                            if (!graph->hasEdge(t, o, SV))
                            {
                                graph->addEdge(t, o, SV);
                                workList.push(CFLREdge(t, o, SV));
                            }
                        }
                    }
                }
            }

            auto lf = succ[u].find(Load);
            if (lf != succ[u].end())
            {
                auto so = pred[o].find(SV);
                if (so != pred[o].end())
                {
                    for (auto x : lf->second)
                    {
                        for (auto t : so->second)
                        {
                            if (!graph->hasEdge(x, t, PT))
                            {
                                graph->addEdge(x, t, PT);
                                workList.push(CFLREdge(x, t, PT));
                            }
                        }
                    }
                }
            }
        }
        else if (e.label == SV)
        {
            unsigned t = e.src;
            unsigned o = e.dst;

            auto po = pred[o].find(PT);
            if (po != pred[o].end())
            {
                for (auto p : po->second)
                {
                    auto lf = succ[p].find(Load);
                    if (lf != succ[p].end())
                    {
                        for (auto x : lf->second)
                        {
                            if (!graph->hasEdge(x, t, PT))
                            {
                                graph->addEdge(x, t, PT);
                                workList.push(CFLREdge(x, t, PT));
                            }
                        }
                    }
                }
            }
        }
    }
}
