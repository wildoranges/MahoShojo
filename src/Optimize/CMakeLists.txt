add_library(
        pass STATIC
        Pass.cpp
        CFGSimplifier.cpp
        DominateTree.cpp
        mem2reg.cpp
        LIR.cpp
        ActiveVar.cpp
        ConstPropagation.cpp
        DeadCodeElimination.cpp
        CFG_analyse.cpp
        LoopInvariant.cpp
        AvailableExpr.cpp
        FuncInline.cpp
        MovConst.cpp
        LoopExpansion.cpp
        Global2Local.cpp
        SideEffectAnalysis.cpp
        SCCP.cpp
)

