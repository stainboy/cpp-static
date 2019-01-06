//===- PrintFunctionNames.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Example clang plugin which simply prints the names of all the top-level decls
// in the input file.
//
//===----------------------------------------------------------------------===//

#include <vector>
#include <string>
#include <unordered_map>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclObjC.h"
#include "clang/AST/DeclVisitor.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Module.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using std::string;
using std::set;
using std::vector;
using std::unordered_map;
using std::make_pair;







namespace {

class PrintFunctionsConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  std::set<std::string> ParsedTemplates;

public:
  PrintFunctionsConsumer(CompilerInstance &Instance,
                         std::set<std::string> ParsedTemplates)
      : Instance(Instance), ParsedTemplates(ParsedTemplates) {}

  // bool HandleTopLevelDecl(DeclGroupRef DG) override {
  //   for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
  //     const Decl *D = *i;
  //     if (const NamedDecl *ND = dyn_cast<NamedDecl>(D))
  //       llvm::errs() << "top-level-decl: \"" << ND->getNameAsString() <<
  //       "\"\n";
  //   }

  //   return true;
  // }

  void HandleTranslationUnit(ASTContext &context) override {
    // if (!Instance.getLangOpts().DelayedTemplateParsing)
    //   return;

    // This demonstrates how to force instantiation of some templates in
    // -fdelayed-template-parsing mode. (Note: Doing this unconditionally for
    // all templates is similar to not using -fdelayed-template-parsig in the
    // first place.)
    // The advantage of doing this in HandleTranslationUnit() is that all
    // codegen (when using -add-plugin) is completely finished and this can't
    // affect the compiler output.
    struct Visitor : public RecursiveASTVisitor<Visitor> {
      const std::set<std::string> &ParsedTemplates;
      ASTContext &context;
      Visitor(const std::set<std::string> &ParsedTemplates, ASTContext &context)
          : ParsedTemplates(ParsedTemplates),
            context(context) {}

      bool VisitTypedefDecl(TypedefDecl *D) {
        // llvm::errs() << "Typedef " << D->getNameAsString() << "\n";
        return true;
      }

      bool VisitFunctionDecl(FunctionDecl *FD) {
        
        if (auto *cxx_func_decl = dyn_cast<CXXMethodDecl>(FD)) {
          FD->getBeginLoc().print(llvm::errs(), context.getSourceManager());
          llvm::errs() << " CXXMethodDecl " << FD->getNameAsString() <<
            " parentClass=" << cxx_func_decl->getParent()->getNameAsString() <<
            " hasBody=" << cxx_func_decl->hasBody() << " hasInlineBody=" << cxx_func_decl->hasInlineBody() << "\n";

          cxx_func_decl->print(llvm::errs());
          llvm::errs() << "\n";


          auto name = cxx_func_decl->getParent()->getNameAsString();
          types[name] = cxx_func_decl->getParent();

          // auto clz = methods[name];
          // clz.push_back(cxx_func_decl);
          // methods[name] = clz;
          methods[name].push_back(cxx_func_decl);

          // types.insert(make_pair(name, cxx_func_decl->getParent())); 
        }

        // llvm::errs() << "Function " << FD->getNameAsString() << "\n";
        return true;
      }

      bool VisitVarDecl(const VarDecl *D) {
        // llvm::errs() << "Var " << D->getNameAsString() << "\n";
        return true;
      }

      bool VisitFieldDecl(const FieldDecl *D) {
        // llvm::errs() << "Field " << D->getNameAsString() << "\n";
        return true;
      }

      // bool VisitFunctionDecl(const FunctionDecl *D) {
      //   DataConsumer.handleFunction(D);
      //   return true;
      // }

      unordered_map<string, vector<CXXMethodDecl*>> methods;
      unordered_map<string, CXXRecordDecl*> types;

      set<FunctionDecl *> LateParsedDecls;


      public: void render() const {
        for (auto x : types) {
            string name = x.first;
            llvm::errs() << "class " << name << " {" << "\n"; 
            for (auto m : methods.at(name)) {
              if (m->hasBody()) {
                m->print(llvm::errs());
              }
            }
            llvm::errs() << "}\n"; 
        }
      }

    } v(ParsedTemplates, context);
    v.TraverseDecl(context.getTranslationUnitDecl());

    v.render();
    // llvm::errs() << "-------dumpColor-------"
    //              << "\n";
    // context.getTranslationUnitDecl()->dumpColor();

    // llvm::errs() << "-------dump-------"
    //              << "\n";
    // context.getTranslationUnitDecl()->dump();

    // //TypedefDecl c;

    // llvm::errs() << "-------print-------"
    //              << "\n";
    // context.getTranslationUnitDecl()->print(llvm::errs());


  // DeclPrinter2 Printer(llvm::errs(), context.getPrintingPolicy(), context, 0,
  //                     false);
  // Printer.Visit(context.getTranslationUnitDecl());


    // clang::Sema &sema = Instance.getSema();
    // for (const FunctionDecl *FD : v.LateParsedDecls) {
    //   clang::LateParsedTemplate &LPT =
    //       *sema.LateParsedTemplateMap.find(FD)->second;
    //   sema.LateTemplateParser(sema.OpaqueParser, LPT);
    //   llvm::errs() << "late-parsed-decl: \"" << FD->getNameAsString() <<
    //   "\"\n";
    // }
  }
};

class PrintFunctionNamesAction : public PluginASTAction {
  std::set<std::string> ParsedTemplates;

protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<PrintFunctionsConsumer>(CI, ParsedTemplates);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "PrintFunctionNames arg = " << args[i] << "\n";

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
        ParsedTemplates.insert(args[i]);
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Help for PrintFunctionNames plugin goes here\n";
  }
};

} // namespace

static FrontendPluginRegistry::Add<PrintFunctionNamesAction>
    X("c2j", "print function names");
