#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "rose.h"

using namespace llvm;

extern SgProject* project;

//-----------------------------------------------------------------------------
// ROSEPass implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


// New PM implementation
struct ROSEPass : PassInfoMixin<ROSEPass> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    std::cout << "In LLVM Pass: sgproject:" << project << std::endl;
    std::cout << "In LLVM Pass: calling unparser:"  << std::endl;
    ::backend(project);

    return PreservedAnalyses::all();
  }
};
} // namespace

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
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "ROSEPass") {
                    FPM.addPass(ROSEPass());
                    return true;
                  }
                  return false;
                });
          // #2  Register ROSEPass as a step of an existing pipeline.
          // The insertion point is specified by using the
          // 'PB.registerPipelineStartEPCallback' callback. 
          PB.registerPipelineStartEPCallback(
              [&](ModulePassManager &MPM, OptimizationLevel Level) {
                MPM.addPass(createModuleToFunctionPassAdaptor(ROSEPass()));
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


