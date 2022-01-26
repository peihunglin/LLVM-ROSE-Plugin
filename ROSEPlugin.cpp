//===- ROSEPlugin.cpp ---------------------------------------------===//
//
// Example clang plugin which incooperate ROSE. 
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
#include "rose.h"

using namespace clang;

SgProject* project;

namespace {

class ROSEPluginConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  llvm::StringRef InFile;

public:
  ROSEPluginConsumer(CompilerInstance &Instance, llvm::StringRef InFile)
      : Instance(Instance), InFile(InFile) {}

  void Initialize(ASTContext &Context) override {
    SourceManager& SrcMgr = Context.getSourceManager();
    llvm::errs() << "my suctomized initizlier for input file" << InFile << "\"\n";
    
    ROSE_INITIALIZE;
    std::vector<std::string> roseFEArgs{"","-rose:skipfinalCompileStep",InFile.str()};
    project = ::frontend(roseFEArgs); 
    ::backend(project);

 }

};

class ROSEPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef InFile) override {
    return std::make_unique<ROSEPluginConsumer>(CI, InFile);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "ROSEPlugin arg = " << args[i] << "\n";

      // Example error handling.
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-an-error") {
        unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error,
                                            "invalid argument '%0'");
        D.Report(DiagID) << args[i];
        return false;
      } else if (args[i] == "-parse-template") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -parse-template argument"));
          return false;
        }
        ++i;
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }


  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for ROSEPlugin plugin goes here\n";
  }

  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }

};

}

static FrontendPluginRegistry::Add<ROSEPluginAction>
X("ROSE", "call ROSE");
