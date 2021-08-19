#ifndef _MHSJ_FLEX_LEXER_H_
#define _MHSJ_FLEX_LEXER_H_

#ifndef YY_DECL
#define YY_DECL                                                         \
    yy::MHSJParser::symbol_type MHSJFlexLexer::yylex(MHSJDriver& driver)
#endif

// We need this for yyFlexLexer. If we don't #undef yyFlexLexer, the
// preprocessor chokes on the line `#define yyFlexLexer yyFlexLexer`
// in `FlexLexer.h`:
#undef yyFlexLexer
#include <FlexLexer.h>

// We need this for the yy::MHSJParser::symbol_type:
#include "MHSJParser.h"

// We need this for the yy::location type:
#include "location.hh"

class MHSJFlexLexer : public yyFlexLexer {
public:
    // Use the superclass's constructor:
    using yyFlexLexer::yyFlexLexer;

    // Provide the interface to `yylex`; `flex` will emit the
    // definition into `MHSJScanner.cpp`:
    yy::MHSJParser::symbol_type yylex(MHSJDriver& driver);

    // This seems like a reasonable place to put the location object
    // rather than it being static (in the sense of having internal
    // linkage at translation unit scope, not in the sense of being a
    // class variable):
    yy::location loc;
};

#endif // _MHSJ_FLEX_LEXER_H_
