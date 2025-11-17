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
    pag->dump();

    CFLR solver;
    solver.buildGraph(pag);
    // TODO: complete this method
    solver.solve();
    solver.dumpResult();

    LLVMModuleSet::releaseLLVMModuleSet();
    return 0;
}


// void CFLR::solve()
// {
//     // TODO: complete this function. The implementations of graph and worklist are provided.
//     //  You need to:
//     //  1. implement the grammar production rules into code;
//     //  2. implement the dynamic-programming CFL-reachability algorithm.
//     //  You may need to add your new methods to 'CFLRGraph' and 'CFLR'.
// }

void CFLR::solve()
{
    // Initialize worklist with all base edges (Addr, Copy, Store, Load)
    for (auto &nodeItr : graph->getSuccessorMap())
    {
        unsigned src = nodeItr.first;
        for (auto &lblItr : nodeItr.second)
        {
            EdgeLabel label = lblItr.first;
            for (auto dst : lblItr.second)
            {
                workList.push(CFLREdge(src, dst, label));
            }
        }
    }

    // Main CFL-reachability algorithm
    while (!workList.empty())
    {
        CFLREdge edge = workList.pop();
        unsigned src = edge.src;
        unsigned dst = edge.dst;
        EdgeLabel label = edge.label;

        // Grammar production rules:
        // The grammar follows the Dyck-CFL pattern for pointer analysis
        
        // Rule 1: PV -> Addr
        // p --Addr--> o implies p --PV--> o (p points-to-value o)
        if (label == Addr)
        {
            if (!graph->hasEdge(src, dst, PV))
            {
                graph->addEdge(src, dst, PV);
                graph->addEdge(dst, src, VPBar);
                workList.push(CFLREdge(src, dst, PV));
                workList.push(CFLREdge(dst, src, VPBar));
            }
        }

        // Rule 2: SV -> PV Copy
        // p --PV--> o1, o1 --Copy--> o2 implies p --SV--> o2
        if (label == PV)
        {
            auto &copySuccs = graph->getSuccessorMap()[dst][Copy];
            for (auto next : copySuccs)
            {
                if (!graph->hasEdge(src, next, SV))
                {
                    graph->addEdge(src, next, SV);
                    graph->addEdge(next, src, VSBar);
                    workList.push(CFLREdge(src, next, SV));
                    workList.push(CFLREdge(next, src, VSBar));
                }
            }
        }
        if (label == Copy)
        {
            auto &pvPreds = graph->getPredecessorMap()[src][PV];
            for (auto prev : pvPreds)
            {
                if (!graph->hasEdge(prev, dst, SV))
                {
                    graph->addEdge(prev, dst, SV);
                    graph->addEdge(dst, prev, VSBar);
                    workList.push(CFLREdge(prev, dst, SV));
                    workList.push(CFLREdge(dst, prev, VSBar));
                }
            }
        }

        // Rule 3: PV -> SV CopyBar
        // p --SV--> o1, o2 --Copy--> o1 implies p --PV--> o2
        if (label == SV)
        {
            auto &copyBarPreds = graph->getPredecessorMap()[dst][CopyBar];
            for (auto prev : copyBarPreds)
            {
                if (!graph->hasEdge(src, prev, PV))
                {
                    graph->addEdge(src, prev, PV);
                    graph->addEdge(prev, src, VPBar);
                    workList.push(CFLREdge(src, prev, PV));
                    workList.push(CFLREdge(prev, src, VPBar));
                }
            }
        }
        if (label == CopyBar)
        {
            auto &svSuccs = graph->getSuccessorMap()[dst][SV];
            for (auto next : svSuccs)
            {
                if (!graph->hasEdge(next, src, PV))
                {
                    graph->addEdge(next, src, PV);
                    graph->addEdge(src, next, VPBar);
                    workList.push(CFLREdge(next, src, PV));
                    workList.push(CFLREdge(src, next, VPBar));
                }
            }
        }

        // Rule 4: VF -> Store VP
        // p --Store--> q, q --VP--> o implies p --VF--> o
        if (label == Store)
        {
            auto &vpSuccs = graph->getSuccessorMap()[dst][VPBar];
            for (auto next : vpSuccs)
            {
                if (!graph->hasEdge(src, next, VF))
                {
                    graph->addEdge(src, next, VF);
                    graph->addEdge(next, src, FVBar);
                    workList.push(CFLREdge(src, next, VF));
                    workList.push(CFLREdge(next, src, FVBar));
                }
            }
        }
        if (label == VPBar)
        {
            auto &storePreds = graph->getPredecessorMap()[src][Store];
            for (auto prev : storePreds)
            {
                if (!graph->hasEdge(prev, dst, VF))
                {
                    graph->addEdge(prev, dst, VF);
                    graph->addEdge(dst, prev, FVBar);
                    workList.push(CFLREdge(prev, dst, VF));
                    workList.push(CFLREdge(dst, prev, FVBar));
                }
            }
        }

        // Rule 5: VA -> VF PV
        // p --VF--> o1, o1 --PV--> o2 implies p --VA--> o2
        if (label == VF)
        {
            auto &pvSuccs = graph->getSuccessorMap()[dst][PV];
            for (auto next : pvSuccs)
            {
                if (!graph->hasEdge(src, next, VA))
                {
                    graph->addEdge(src, next, VA);
                    graph->addEdge(next, src, AVBar);
                    workList.push(CFLREdge(src, next, VA));
                    workList.push(CFLREdge(next, src, AVBar));
                }
            }
        }
        if (label == PV)
        {
            auto &vfPreds = graph->getPredecessorMap()[src][VF];
            for (auto prev : vfPreds)
            {
                if (!graph->hasEdge(prev, dst, VA))
                {
                    graph->addEdge(prev, dst, VA);
                    graph->addEdge(dst, prev, AVBar);
                    workList.push(CFLREdge(prev, dst, VA));
                    workList.push(CFLREdge(dst, prev, AVBar));
                }
            }
        }

        // Rule 6: LV -> Load VP
        // p --Load--> q, q --VP--> o implies p --LV--> o
        if (label == Load)
        {
            auto &vpSuccs = graph->getSuccessorMap()[dst][VPBar];
            for (auto next : vpSuccs)
            {
                if (!graph->hasEdge(src, next, LV))
                {
                    graph->addEdge(src, next, LV);
                    graph->addEdge(next, src, VLBar);
                    workList.push(CFLREdge(src, next, LV));
                    workList.push(CFLREdge(next, src, VLBar));
                }
            }
        }
        if (label == VPBar)
        {
            auto &loadPreds = graph->getPredecessorMap()[src][Load];
            for (auto prev : loadPreds)
            {
                if (!graph->hasEdge(prev, dst, LV))
                {
                    graph->addEdge(prev, dst, LV);
                    graph->addEdge(dst, prev, VLBar);
                    workList.push(CFLREdge(prev, dst, LV));
                    workList.push(CFLREdge(dst, prev, VLBar));
                }
            }
        }

        // Rule 7: PV -> LV VA
        // p --LV--> o1, o1 --VA--> o2 implies p --PV--> o2
        if (label == LV)
        {
            auto &vaSuccs = graph->getSuccessorMap()[dst][VA];
            for (auto next : vaSuccs)
            {
                if (!graph->hasEdge(src, next, PV))
                {
                    graph->addEdge(src, next, PV);
                    graph->addEdge(next, src, VPBar);
                    workList.push(CFLREdge(src, next, PV));
                    workList.push(CFLREdge(next, src, VPBar));
                }
            }
        }
        if (label == VA)
        {
            auto &lvPreds = graph->getPredecessorMap()[src][LV];
            for (auto prev : lvPreds)
            {
                if (!graph->hasEdge(prev, dst, PV))
                {
                    graph->addEdge(prev, dst, PV);
                    graph->addEdge(dst, prev, VPBar);
                    workList.push(CFLREdge(prev, dst, PV));
                    workList.push(CFLREdge(dst, prev, VPBar));
                }
            }
        }

        // Rule 8: PT -> PV (final points-to result)
        // p --PV--> o implies p --PT--> o
        if (label == PV)
        {
            if (!graph->hasEdge(src, dst, PT))
            {
                graph->addEdge(src, dst, PT);
                graph->addEdge(dst, src, PTBar);
                workList.push(CFLREdge(src, dst, PT));
                workList.push(CFLREdge(dst, src, PTBar));
            }
        }
    }
}