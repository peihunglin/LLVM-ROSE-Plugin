#include "LLVMRosePass.hpp"

extern SgProject* project;

//-----------------------------------------------------------------------------
// ROSEPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.

using namespace LLVMRosePass;

// New PM implementation
PreservedAnalyses ROSEPass::run(Module &M, ModuleAnalysisManager &MAM) {

  // std::cout <<"Module info:" << M.getInstructionCount ()  << std::endl;
  auto &FAM = MAM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();
 
  auto  &funcList = M.getFunctionList();
  Module::global_iterator gv_iter;
  Module::iterator func_iter;

// Get ROSE node mapping in the beginning
  ROSENodeMap = getRoseNodeInfo();

  for(auto a:ROSENodeMap)
  {
    SgNode* node = a.first;
    std::pair<int,int> srcinfo  = a.second;
    outs() << "SgNode: " << node << " src info: [" << srcinfo.first << ":" << srcinfo.second << "]\n";
  }


  std::string header = "==================================================";

  for (gv_iter = M.global_begin(); gv_iter != M.global_end(); gv_iter++)
  {
     outs() << header << "\n";
     runOnGVariable(*gv_iter);
  }
  
  // Functions
  for (func_iter = M.begin(); func_iter != M.end(); func_iter++)
  {

     // check the alias analysis only when the function definition is defined
     if(!func_iter->isDeclaration())
     {
       outs() << header << "\n";
       runOnFunction(*func_iter, FAM);
     }
  }

  std::cout << "In LLVM Pass: sgproject:" << project << std::endl;
  std::cout << "In LLVM Pass: calling unparser:"  << std::endl;
  ::backend(project);

  return PreservedAnalyses::all();
}

PreservedAnalyses ROSEPass::runOnFunction(Function &F, FunctionAnalysisManager &FAM)
{
	unsigned int i = 0;
	Function::arg_iterator arg_iter;
	Function::iterator	bb_iter;
	BasicBlock::iterator inst_iter;
   
        AAResults& AAR = FAM.getResult<AAManager>(F);

  	outs() << "Name: " << F.getName() << "\n";
	
	// Return type
	outs() << i << ". Return Type: " << *F.getReturnType() << "\n";
	i += 1;

	// Arguments
	outs() << i << ". Arguments: ";
	if (F.arg_size() == 0)
	{
		outs() << "No Arguments" << "\n";
	}
	else
	{
		for (arg_iter = F.arg_begin(); arg_iter != F.arg_end(); arg_iter++)
		{
			outs() << *arg_iter;
			
			if (arg_iter != F.arg_end())
			{
				outs() << ", ";
			}
		}

		outs() << "\n";
	}
	i += 1;

        // map to record all operands, and their line/column info from the instruction
        std::map<Value*, std::pair<unsigned,unsigned>> valueList;
	// BasicBlocks
	outs() << i << ". IR: " << "\n";
	if (F.isDeclaration() == true)
	{
		outs() << "Declaration. No IR" << "\n";
	}
	else
	{
		for (bb_iter = F.begin(); bb_iter != F.end(); bb_iter++)
		{
			// Each BB is made of one/more instructions.
			// Print them.
			for (inst_iter = (*bb_iter).begin(); inst_iter != (*bb_iter).end(); inst_iter++)
			{
				outs() << *inst_iter << "\n";
                                std::pair<unsigned, unsigned> srcinfo;
                                StringRef File;
                                StringRef Dir;
                                if (DILocation *Loc = inst_iter->getDebugLoc()) { // Here *inst_iter is an LLVM instruction
                                    //Line = Loc->getLine();
                                    //Column = inst_iter->getDebugLoc()->getColumn();
                                    srcinfo.first = Loc->getLine();
                                    srcinfo.second = inst_iter->getDebugLoc()->getColumn();
                                    File = Loc->getFilename();
                                    Dir = Loc->getDirectory();
                                    // outs() << "inst file: "<< File << " line:column = " << Line << ":" << Column  << "\n"; 
                                }
                                for(unsigned opi = 0; opi < inst_iter->getNumOperands() ; opi++)
                                {
                                  Value* val = inst_iter->getOperand (opi);
                                  if(valueList.find(val) == valueList.end())
                                     valueList.insert({val,srcinfo});
                                }	
			}
		}
	}

        for(std::map<Value*, std::pair<unsigned,unsigned>>::iterator ii = valueList.begin(); ii != valueList.end(); ii++)
        {
          for(std::map<Value*, std::pair<unsigned,unsigned>>::iterator jj = std::next(ii); jj != valueList.end(); jj++)
          {
             std::string init = "" ;
             Value* v1 = ii->first;
             std::pair<unsigned,unsigned> src1 = ii->second;
             std::string v1info;
             llvm::raw_string_ostream  os1(init);
             v1->print(os1);
             v1info = "(" + os1.str() + ")[" + std::to_string(src1.first) + ":" + std::to_string(src1.second) + "]";
             if(matchROSESrcInfo(src1))
             {
               std::pair<SgNode*, std::pair<unsigned ,unsigned >> RoseMapInfo = getMatchingROSESrcInfo(src1);
               v1info += " ==> SgNode:" + RoseMapInfo.first->class_name()  + "[" + std::to_string(RoseMapInfo.second.first) + ":" + std::to_string(RoseMapInfo.second.second) + "]";
             }

             Value* v2 = jj->first;
             std::pair<unsigned,unsigned> src2 = jj->second;
             std::string v2info;
             llvm::raw_string_ostream  os2(init);
             v2->print(os2);
             v2info = "(" + os2.str() + ")[" + std::to_string(src2.first) + ":" + std::to_string(src2.second) + "]";
             if(matchROSESrcInfo(src2))
             {
               std::pair<SgNode*, std::pair<unsigned ,unsigned >> RoseMapInfo = getMatchingROSESrcInfo(src2);
               v2info += " ==> SgNode:" + RoseMapInfo.first->class_name()  + "[" + std::to_string(RoseMapInfo.second.first) + ":" + std::to_string(RoseMapInfo.second.second) + "]";
             }

             const AliasResult::Kind result = AAR.alias(v1, v2);
             if(static_cast<int>(result) != 0)
             {
                outs() <<  getAliasResult(result) << ":\n";
                outs() << "\t op1: " << v1info << "\n" ;
                outs() << "\t op2: " << v2info << "\n" ;
             }
          }
        }

  	return PreservedAnalyses::all();
}

bool ROSEPass::matchROSESrcInfo(std::pair<unsigned,unsigned> llvmsrcinfo)
{
  for(auto a : ROSEPass::ROSENodeMap)
  {
      if (a.second == llvmsrcinfo )
        return true;
  }
  return false;
}

std::pair<SgNode*, std::pair<unsigned ,unsigned >> ROSEPass::getMatchingROSESrcInfo(std::pair<unsigned,unsigned> llvmsrcinfo)
{
  for(auto a : ROSEPass::ROSENodeMap)
  {
      if (a.second == llvmsrcinfo )
        return a;
  }
  return {nullptr,{0,0}};
}


PreservedAnalyses ROSEPass::runOnGVariable(GlobalVariable &G)
{	
	outs() << G << "\n";
	return PreservedAnalyses::all();
}


std::string ROSEPass::getAliasResult(AliasResult::Kind kind) const {
   std::string result;
   switch (kind) {
   case AliasResult::Kind::NoAlias:
      result =  "NoAlias";
     break;
   case AliasResult::Kind::MayAlias:
      result =  "MayAlias";
     break;
   case AliasResult::Kind::PartialAlias:
      result =  "PartialAlias";
     break;
   case AliasResult::Kind::MustAlias:
      result =  "MustAlias";
     break;
   }
   return result; 
}

std::map<SgNode*, std::pair<unsigned,unsigned>> ROSEPass::getRoseNodeInfo()
{
  nodeTraversal travese;
  travese.traverseInputFiles(project,preorder);
  return travese.getNodeMap();
}

void nodeTraversal::push_map_record(SgNode* node, std::pair<unsigned,unsigned> srcInfo)
{
  if(nodeTraversal::m.find(node) == nodeTraversal::m.end())
  {
    nodeTraversal::m.insert({node, srcInfo});
  }
  else
  {
    outs() << "SgNode " << node << " is pushed already\n" ;
  }
}

void nodeTraversal::visit(SgNode* n)
{
  SgLocatedNode* locatedNode = isSgLocatedNode(n);
  if(locatedNode)
  {
     Sg_File_Info* fileInfo = locatedNode->get_file_info();
     std::pair<unsigned , unsigned > srcinfo = std::make_pair((unsigned)fileInfo->get_line(), (unsigned)fileInfo->get_col());
     push_map_record(locatedNode, srcinfo); 
  }

}
//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getROSEPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "ROSEPass", LLVM_VERSION_STRING,
          // #1 REGISTRATION FOR "opt -passes=print<ROSEPass>"
          // Register ROSEPass so that it can be used when
          // specifying pass pipelines with `-passes=`.
          [](PassBuilder &PB) {
            FunctionAnalysisManager FAM;
            PB.registerFunctionAnalyses(FAM);
            ModuleAnalysisManager MAM;
            PB.registerModuleAnalyses(MAM);
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "ROSEPass") {
                    MPM.addPass(ROSEPass());
                    return true;
                  }
                  return false;
                });
          // #2  Register ROSEPass as a step of an existing pipeline.
          // The insertion point is specified by using the
          // 'PB.registerPipelineStartEPCallback' callback. 
          PB.registerOptimizerLastEPCallback(
              [&](ModulePassManager &MPM, OptimizationLevel Level) {
                MPM.addPass(ROSEPass());
              });


          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize ROSEPass when added to the pass pipeline on the
// command line, i.e. via '-passes=hello-world'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getROSEPassPluginInfo();
}


