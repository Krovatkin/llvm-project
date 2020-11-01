//------------------------------------------------------------------------------
// Using AST matchers with RefactoringTool. Demonstrates:
//
// * How to use Replacements to collect replacements in a matcher instead of
//   directly applying fixes to a Rewriter.
//
// Eli Bendersky (eliben@gmail.com)
// This code is in the public domain
//------------------------------------------------------------------------------
#include <string>
#include <iostream>

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class IfStmtHandler : public MatchFinder::MatchCallback {
public:
  IfStmtHandler(std::map< std::string, Replacements > & Replace) : Replace(Replace) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // The matched 'if' statement was bound to 'ifStmt'.
    if (const IfStmt *IfS = Result.Nodes.getNodeAs<clang::IfStmt>("ifStmt")) {
      std::cerr << "found if!\n";
      const Stmt *Then = IfS->getThen();
      Replacement Rep(*(Result.SourceManager), Then->getBeginLoc(), 0,
                      "// the 'if' part\n");
      std::cerr << "file_path = " << std::string(Rep.getFilePath()) << std::endl;
      auto& Rs = Replace[std::string(Rep.getFilePath())];
      std::cerr << "got Replacements\n"; 
      auto status = Rs.add(Rep);
      if (status) {
      	std::cerr << "oops some error \n";
      }
      std::cerr << "added!\n";
      if (const Stmt *Else = IfS->getElse()) {
        Replacement Rep(*(Result.SourceManager), Else->getBeginLoc(), 0,
                        "// the 'else' part\n");
	std::cerr << "file_path = " << std::string(Rep.getFilePath()) << std::endl;
	auto status = Replace[std::string(Rep.getFilePath())].add(Rep);
	if (status) {
		std::cerr << "oops some error 2\n";
	}
      }
    }
  }

private:
  std::map< std::string, Replacements > &Replace;
};

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  IfStmtHandler HandlerForIf(Tool.getReplacements());

  MatchFinder Finder;
  Finder.addMatcher(ifStmt().bind("ifStmt"), &HandlerForIf);

  // Run the tool and collect a list of replacements. We could call runAndSave,
  // which would destructively overwrite the files with their new contents.
  // However, for demonstration purposes it's interesting to print out the
  // would-be contents of the rewritten files instead of actually rewriting
  // them.
  if (int Result = Tool.run(newFrontendActionFactory(&Finder).get())) {
      std::cerr << "result = " << Result << std::endl;
      return Result;
  }

  // We need a SourceManager to set up the Rewriter.
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  DiagnosticsEngine Diagnostics(
      IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), &*DiagOpts,
      new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts), true);
  SourceManager Sources(Diagnostics, Tool.getFiles());
  std::cerr << "init diagnostics\n";
  // Apply all replacements to a rewriter.
  Rewriter Rewrite(Sources, LangOptions());
  Tool.applyAllReplacements(Rewrite);
  std::cerr << "apply replaces\n";
  // Query the rewriter for all the files it has rewritten, dumping their new
  // contents to stdout.
  for (Rewriter::buffer_iterator I = Rewrite.buffer_begin(),
                                 E = Rewrite.buffer_end();
       I != E; ++I) {
    const FileEntry *Entry = Sources.getFileEntryForID(I->first);
    llvm::outs() << "Rewrite buffer for file: " << Entry->getName() << "\n";
    I->second.write(llvm::outs());
  }

  return 0;
}

