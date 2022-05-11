//===- RoseConnectionClangPlugin.cpp ---------------------------------------------===//
//
// Example clang plugin which incooperate ROSE. 
//
//===----------------------------------------------------------------------===//
#include "RoseConnectionClangPlugin.hpp"

using namespace clang;
namespace {

class collectInfoVisitor : public RecursiveASTVisitor<collectInfoVisitor> {

public:
  explicit collectInfoVisitor(ASTContext *Context)
    : Context(Context) {}

  explicit collectInfoVisitor(ASTContext *Context,  llvm::StringRef inFile)
    : Context(Context), srcFile(inFile) {
//    llvm::outs() << "Constructing in " << srcFile << "\n";
    }

  bool VisitDecl(Decl *d) {

//    llvm::outs() << "Processing in " << srcFile << "\n";
    std::ostringstream get_the_address; 
    get_the_address << d;
    std::string address =  get_the_address.str(); 
    std::string jsonval = "";


    llvm::StringRef filename;

    FullSourceLoc fullLocation = Context->getFullLoc(d->getLocation());
    if(fullLocation.hasManager())
      filename = fullLocation.getManager().getFilename(fullLocation);


    FullSourceLoc beginFullLocation = Context->getFullLoc(d->getBeginLoc());
    FullSourceLoc endFullLocation = Context->getFullLoc(d->getEndLoc());

    if(srcFile.compare(filename.str()) == 0)
    if (beginFullLocation.isValid() && endFullLocation.isValid())
    {
      llvm::outs() << "Found Decl " << d->getDeclKindName () << " begin at "
                   << beginFullLocation.getSpellingLineNumber() << ":"
                   << beginFullLocation.getSpellingColumnNumber();
      llvm::outs() << " end at "
                   << endFullLocation.getSpellingLineNumber() << ":"
                   << endFullLocation.getSpellingColumnNumber() << ":" << filename << "\n";

      pt.put(address,jsonval);
      jsonval = address+".ASTname";
      pt.put(jsonval,d->getDeclKindName ());
      jsonval = address+".filename";
      pt.put(jsonval,filename.str());
      jsonval = address+".beginLine";
      pt.put(jsonval,beginFullLocation.getSpellingLineNumber());
      jsonval = address+".beginColumn";
      pt.put(jsonval,beginFullLocation.getSpellingColumnNumber());
      jsonval = address+".endLine";
      pt.put(jsonval,endFullLocation.getSpellingLineNumber());
      jsonval = address+".endColumn";
      pt.put(jsonval,endFullLocation.getSpellingColumnNumber());

    }
    return true;
  }

  bool VisitDeclaratorDecl(DeclaratorDecl* dd)
  {
    std::ostringstream get_the_address; 
    get_the_address << dd;
    std::string address =  get_the_address.str(); 
    std::string jsonval = "";

    llvm::StringRef filename;
    const Type* type = dd->getTypeSourceInfo()->getTypeLoc().getTypePtr();

    TypeLoc typeLoc = dd->getTypeSourceInfo()->getTypeLoc();
    FullSourceLoc beginFullLocation = Context->getFullLoc(typeLoc.getBeginLoc());
    if(beginFullLocation.hasManager())
      filename = beginFullLocation.getManager().getFilename(beginFullLocation);
    FullSourceLoc endFullLocation = Context->getFullLoc(typeLoc.getEndLoc());

    if(srcFile.compare(filename.str()) == 0)
    if (beginFullLocation.isValid() && endFullLocation.isValid())
    {
      llvm::outs() << "Found type " << type->getTypeClassName() << " begin at "
                   << beginFullLocation.getSpellingLineNumber() << ":"
                   << beginFullLocation.getSpellingColumnNumber();
      llvm::outs() << " end at "
                   << endFullLocation.getSpellingLineNumber() << ":"
                   << endFullLocation.getSpellingColumnNumber() << ":" << filename << "\n";

      pt.put(address,jsonval);
      jsonval = address+".ASTname";
      pt.put(jsonval,dd->getDeclKindName ());
      jsonval = address+".filename";
      pt.put(jsonval,filename.str());
      jsonval = address+".beginLine";
      pt.put(jsonval,beginFullLocation.getSpellingLineNumber());
      jsonval = address+".beginColumn";
      pt.put(jsonval,beginFullLocation.getSpellingColumnNumber());
      jsonval = address+".endLine";
      pt.put(jsonval,endFullLocation.getSpellingLineNumber());
      jsonval = address+".endColumn";
      pt.put(jsonval,endFullLocation.getSpellingColumnNumber());
    }
    return true;
  }

  bool VisitStmt(Stmt *s) {
    std::ostringstream get_the_address; 
    get_the_address << s;
    std::string address =  get_the_address.str(); 
    std::string jsonval = "";

    llvm::StringRef filename;

    FullSourceLoc beginFullLocation = Context->getFullLoc(s->getBeginLoc());
    if(beginFullLocation.hasManager())
      filename = beginFullLocation.getManager().getFilename(beginFullLocation);

    FullSourceLoc endFullLocation = Context->getFullLoc(s->getEndLoc());

    if(srcFile.compare(filename.str()) == 0)
    if (beginFullLocation.isValid() && endFullLocation.isValid())
    {
      llvm::outs() << "Found Stmt " << s->getStmtClassName () << " begin at "
                   << beginFullLocation.getSpellingLineNumber() << ":"
                   << beginFullLocation.getSpellingColumnNumber();
      llvm::outs() << " end at "
                   << endFullLocation.getSpellingLineNumber() << ":"
                   << endFullLocation.getSpellingColumnNumber() << ":" << filename << "\n";

      pt.put(address,jsonval);
      jsonval = address+".ASTname";
      pt.put(jsonval,s->getStmtClassName ());
      jsonval = address+".filename";
      pt.put(jsonval,filename.str());
      jsonval = address+".beginLine";
      pt.put(jsonval,beginFullLocation.getSpellingLineNumber());
      jsonval = address+".beginColumn";
      pt.put(jsonval,beginFullLocation.getSpellingColumnNumber());
      jsonval = address+".endLine";
      pt.put(jsonval,endFullLocation.getSpellingLineNumber());
      jsonval = address+".endColumn";
      pt.put(jsonval,endFullLocation.getSpellingColumnNumber());

    }
    return true;
  }

  bool VisitType(Type *t) {

//    llvm::outs() << "Found Type " << t->getTypeClassName () << "\n";
    return true;
  }

private:
  ASTContext *Context;
  std::string srcFile;
};


class RoseConnectionClangPluginConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  llvm::StringRef inFile;

public:
  RoseConnectionClangPluginConsumer(CompilerInstance &Instance, llvm::StringRef inputFile)
      : Instance(Instance), visitor(&Instance.getASTContext(), inputFile.str()), inFile(inputFile) {}

  void Initialize(ASTContext &Context) override {
    SourceManager& SrcMgr = Context.getSourceManager();
    llvm::errs() << "my customized initizlier for input file: " << inFile << "\"\n";
    
    ROSE_INITIALIZE;
    std::vector<std::string> roseFEArgs{"","-rose:skipfinalCompileStep",inFile.str()};
    project = ::frontend(roseFEArgs); 
    std::cout << "In Plugin: generated sgproject:" << project << std::endl;

 }

 virtual void HandleTranslationUnit(clang::ASTContext &Context) override{
   visitor.TraverseDecl(Context.getTranslationUnitDecl());
   std::stringstream ss;
   boost::property_tree::json_parser::write_json(ss, pt);
   std::ofstream jfile;
   jfile.open("srcLoc.json",std::ios::out);
   jfile << ss.rdbuf();
   jfile.close();
 }

private:
  collectInfoVisitor visitor;
};

class RoseConnectionClangPluginAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef inFile) override {
    return std::make_unique<RoseConnectionClangPluginConsumer>(CI, inFile);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "RoseConnectionClangPlugin arg = " << args[i] << "\n";

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
    ros << "Help for RoseConnectionClangPlugin plugin goes here\n";
  }

  PluginASTAction::ActionType getActionType() override {
    return AddBeforeMainAction;
  }

};

}

static FrontendPluginRegistry::Add<RoseConnectionClangPluginAction>
X("ROSE", "call ROSE");
