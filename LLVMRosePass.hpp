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
#include "SgNodeHelper.h"
using namespace llvm;

namespace LLVMRosePass{

// New PM implementation
class ROSEPass : public PassInfoMixin<ROSEPass> {
  // Main entry point, takes IR unit to run the pass on (&M) and the
  // corresponding pass manager (to be queried if need be)
public:
  std::map<SgNode*, std::pair<unsigned ,unsigned >> ROSENodeMap;

  // run the pass on the module
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM);
  
  PreservedAnalyses runOnFunction(Function &F, FunctionAnalysisManager &FAM);

  PreservedAnalyses runOnGVariable(GlobalVariable &G);
  // get the aliias result in string output
  std::string getAliasResult(AliasResult::Kind kind) const ;
  // get the map to record ROSE SgLocatedNode and src line/column
  std::map<SgNode*, std::pair<unsigned ,unsigned >> getRoseNodeInfo();
  // check if a SgLocatedNode has same line/column 
  bool matchROSESrcInfo(std::pair<unsigned,unsigned>);
  // get the line/column from the matched ROSE SgLocatedNode 
  std::pair<SgNode*, std::pair<unsigned ,unsigned >> getMatchingROSESrcInfo(std::pair<unsigned,unsigned>);
// get the operand information into a single string
  std::string getOperandInfo(Value* v, std::pair<unsigned ,unsigned > srcinfo);
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
