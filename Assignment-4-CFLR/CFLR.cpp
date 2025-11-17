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


void CFLR::solve()
{
    // Implement a worklist-based dynamic programming CFL-reachability solver.
    // The approach:
    // 1) initialize the worklist with all existing graph edges
    // 2) apply unary productions (e.g., Addr -> PT)
    // 3) repeatedly pop an edge and compose it with adjacent edges using
    //    the grammar productions to derive new edges until fixpoint.

    using ProdMap = std::unordered_map<uint64_t, std::vector<EdgeLabel>>;

    auto makeKey = [](EdgeLabel a, EdgeLabel b) {
        return (uint64_t(a) << 32) | uint64_t(b);
    };

    // Production rules (pair -> vector of resulting labels)
    ProdMap prods;

    // Basic productions used for Andersen-like points-to CFLR
    // Copy ; PT -> PT
    prods[makeKey(Copy, PT)].push_back(PT);
    // PT ; CopyBar -> PT
    prods[makeKey(PT, CopyBar)].push_back(PT);

    // Handle loads: (LoadBar ; PT) -> VP, (VP ; PT) -> PT
    // These intermediate symbols (VP) help derive points-to through loads
    prods[makeKey(LoadBar, PT)].push_back(VP);
    prods[makeKey(VP, PT)].push_back(PT);

    // You can extend this production table if necessary for more features.

    // Helper to add new edges to graph and worklist (if not already present)
    auto addNewEdge = [&](unsigned s, unsigned d, EdgeLabel l) {
        if (!graph->hasEdge(s, d, l))
        {
            graph->addEdge(s, d, l);
            workList.push(CFLREdge(s, d, l));
            return true;
        }
        return false;
    };

    // 1) initialize worklist with all existing edges
    for (auto &srcItr : graph->getSuccessorMap())
    {
        unsigned src = srcItr.first;
        for (auto &lblItr : srcItr.second)
        {
            EdgeLabel lbl = lblItr.first;
            for (auto dst : lblItr.second)
            {
                workList.push(CFLREdge(src, dst, lbl));
            }
        }
    }

    // 2) process worklist until fixpoint
    while (!workList.empty())
    {
        CFLREdge e = workList.pop();

        // unary productions
        if (e.label == Addr)
        {
            addNewEdge(e.src, e.dst, PT);
        }

        // forward composition: e (u->v,l1) with (v->w,l2) => (u->w, new)
        auto &succMap = graph->getSuccessorMap();
        auto succIt = succMap.find(e.dst);
        if (succIt != succMap.end())
        {
            for (auto &lbl2Itr : succIt->second)
            {
                EdgeLabel l2 = lbl2Itr.first;
                uint64_t key = makeKey(e.label, l2);
                auto pIt = prods.find(key);
                if (pIt == prods.end()) continue;
                for (auto w : lbl2Itr.second)
                {
                    for (auto resLbl : pIt->second)
                        addNewEdge(e.src, w, resLbl);
                }
            }
        }

        // backward composition: (x->u,l2) with e (u->v,l1) => (x->v, new)
        auto &predMap = graph->getPredecessorMap();
        auto predIt = predMap.find(e.src);
        if (predIt != predMap.end())
        {
            for (auto &lbl2Itr : predIt->second)
            {
                EdgeLabel l2 = lbl2Itr.first;
                uint64_t key = makeKey(l2, e.label);
                auto pIt = prods.find(key);
                if (pIt == prods.end()) continue;
                for (auto x : lbl2Itr.second)
                {
                    for (auto resLbl : pIt->second)
                        addNewEdge(x, e.dst, resLbl);
                }
            }
        }
    }
}