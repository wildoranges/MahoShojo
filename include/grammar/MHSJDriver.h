#ifndef _MHSJ_DRIVER_H_
#define _MHSJ_DRIVER_H_

#include <fstream>
#include <string>
#include <map>

// Generated by bison:
# include "MHSJParser.h"

#include "MHSJFlexLexer.h"

// Conducting the whole scanning and parsing of MHSJ.
class MHSJDriver
{
public:
    MHSJDriver();
    virtual ~MHSJDriver();
    
    std::map<std::string, int> variables;
    
    int result;
    
    // MHSJ lexer
    MHSJFlexLexer lexer;

    std::ifstream instream;

    // Handling the MHSJ scanner.
    void scan_begin();
    void scan_end();
    bool trace_scanning;
    
    // Run the parser on file F.
    // Return 0 on success.
    SyntaxTree::Node* parse(const std::string& f);
    // The name of the file being parsed.
    // Used later to pass the file name to the location tracker.
    std::string file;
    // Whether parser traces should be generated.
    bool trace_parsing;

    // Error handling.
    void error(const yy::location& l, const std::string& m);
    void error(const std::string& m);

    SyntaxTree::Node* root = nullptr;
};

#endif // _MHSJ_DRIVER_H_
