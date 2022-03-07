#ifndef LLVM_ROSE_PASS_H                                        

#define LLVM_ROSE_PASS_H

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugInfoMetadata.h>


#include "rose.h"
using namespace llvm;

namespace LLVMRosePass{

// New PM implementation
class ROSEPass : public PassInfoMixin<ROSEPass> {
  // Main entry point, takes IR unit to run the pass on (&M) and the
  // corresponding pass manager (to be queried if need be)
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);

  PreservedAnalyses runOnFunction(Function &F, FunctionAnalysisManager &FAM);

  PreservedAnalyses runOnGVariable(GlobalVariable &G);

  std::string getAliasResult(AliasResult::Kind kind) const ;
};


}
#endif // LLVM_ROSE_PASS_H
