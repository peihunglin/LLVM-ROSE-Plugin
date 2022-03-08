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
  std::map<SgNode*, std::pair<unsigned ,unsigned >> ROSENodeMap;

  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);

  PreservedAnalyses runOnFunction(Function &F, FunctionAnalysisManager &FAM);

  PreservedAnalyses runOnGVariable(GlobalVariable &G);

  std::string getAliasResult(AliasResult::Kind kind) const ;

  std::map<SgNode*, std::pair<unsigned ,unsigned >> getRoseNodeInfo();

  bool matchROSESrcInfo(std::pair<unsigned,unsigned>);
  std::pair<SgNode*, std::pair<unsigned ,unsigned >> getMatchingROSESrcInfo(std::pair<unsigned,unsigned>);
};


class nodeTraversal : public AstSimpleProcessing
{
  public:
      typedef std::map<SgNode*, std::pair<unsigned ,unsigned >> node_map_t; 
      const node_map_t  getNodeMap() const {return m;}

      virtual void visit(SgNode* n);
      void push_map_record(SgNode* node, std::pair<unsigned ,unsigned > srcInfo);
  private:
      node_map_t m;
};

}
#endif // LLVM_ROSE_PASS_H
