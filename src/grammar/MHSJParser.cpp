// A Bison parser, made by GNU Bison 3.7.6.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "MHSJParser.h"


// Unqualified %code blocks.
#line 35 "../grammar/MHSJParser.yy"

#include "MHSJDriver.h"
#define yylex driver.lexer.yylex

#line 51 "MHSJParser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 143 "MHSJParser.cpp"

  /// Build a parser object.
  MHSJParser::MHSJParser (MHSJDriver& driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  MHSJParser::~MHSJParser ()
  {}

  MHSJParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/



  // by_state.
  MHSJParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  MHSJParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  MHSJParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  MHSJParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  MHSJParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  MHSJParser::symbol_kind_type
  MHSJParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  MHSJParser::stack_symbol_type::stack_symbol_type ()
  {}

  MHSJParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_CompUnit: // CompUnit
        value.YY_MOVE_OR_COPY< SyntaxTree::Assembly* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_Block: // Block
        value.YY_MOVE_OR_COPY< SyntaxTree::BlockStmt* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_OptionRet: // OptionRet
      case symbol_kind::S_Exp: // Exp
      case symbol_kind::S_RelExp: // RelExp
      case symbol_kind::S_EqExp: // EqExp
      case symbol_kind::S_LAndExp: // LAndExp
      case symbol_kind::S_LOrExp: // LOrExp
      case symbol_kind::S_CondExp: // CondExp
        value.YY_MOVE_OR_COPY< SyntaxTree::Expr* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FuncDef: // FuncDef
        value.YY_MOVE_OR_COPY< SyntaxTree::FuncDef* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FuncFParam: // FuncFParam
        value.YY_MOVE_OR_COPY< SyntaxTree::FuncParam* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_InitVal: // InitVal
      case symbol_kind::S_InitValList: // InitValList
      case symbol_kind::S_CommaInitValList: // CommaInitValList
        value.YY_MOVE_OR_COPY< SyntaxTree::InitVal* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LVal: // LVal
        value.YY_MOVE_OR_COPY< SyntaxTree::LVal* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_Number: // Number
      case symbol_kind::S_String: // String
        value.YY_MOVE_OR_COPY< SyntaxTree::Literal* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ArrayExpList: // ArrayExpList
      case symbol_kind::S_ExpList: // ExpList
      case symbol_kind::S_CommaExpList: // CommaExpList
        value.YY_MOVE_OR_COPY< SyntaxTree::PtrList<SyntaxTree::Expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FParamList: // FParamList
      case symbol_kind::S_CommaFParamList: // CommaFParamList
        value.YY_MOVE_OR_COPY< SyntaxTree::PtrList<SyntaxTree::FuncParam> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_GlobalDecl: // GlobalDecl
        value.YY_MOVE_OR_COPY< SyntaxTree::PtrList<SyntaxTree::GlobalDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_BlockItemList: // BlockItemList
      case symbol_kind::S_BlockItem: // BlockItem
        value.YY_MOVE_OR_COPY< SyntaxTree::PtrList<SyntaxTree::Stmt> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ConstDecl: // ConstDecl
      case symbol_kind::S_ConstDefList: // ConstDefList
      case symbol_kind::S_VarDecl: // VarDecl
      case symbol_kind::S_VarDefList: // VarDefList
        value.YY_MOVE_OR_COPY< SyntaxTree::PtrList<SyntaxTree::VarDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_Stmt: // Stmt
      case symbol_kind::S_IfStmt: // IfStmt
        value.YY_MOVE_OR_COPY< SyntaxTree::Stmt* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_BType: // BType
        value.YY_MOVE_OR_COPY< SyntaxTree::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ConstDef: // ConstDef
      case symbol_kind::S_VarDef: // VarDef
        value.YY_MOVE_OR_COPY< SyntaxTree::VarDef* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FLOATCONST: // FLOATCONST
        value.YY_MOVE_OR_COPY< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INTCONST: // INTCONST
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRINGCONST: // STRINGCONST
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  MHSJParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_CompUnit: // CompUnit
        value.move< SyntaxTree::Assembly* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_Block: // Block
        value.move< SyntaxTree::BlockStmt* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_OptionRet: // OptionRet
      case symbol_kind::S_Exp: // Exp
      case symbol_kind::S_RelExp: // RelExp
      case symbol_kind::S_EqExp: // EqExp
      case symbol_kind::S_LAndExp: // LAndExp
      case symbol_kind::S_LOrExp: // LOrExp
      case symbol_kind::S_CondExp: // CondExp
        value.move< SyntaxTree::Expr* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FuncDef: // FuncDef
        value.move< SyntaxTree::FuncDef* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FuncFParam: // FuncFParam
        value.move< SyntaxTree::FuncParam* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_InitVal: // InitVal
      case symbol_kind::S_InitValList: // InitValList
      case symbol_kind::S_CommaInitValList: // CommaInitValList
        value.move< SyntaxTree::InitVal* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_LVal: // LVal
        value.move< SyntaxTree::LVal* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_Number: // Number
      case symbol_kind::S_String: // String
        value.move< SyntaxTree::Literal* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ArrayExpList: // ArrayExpList
      case symbol_kind::S_ExpList: // ExpList
      case symbol_kind::S_CommaExpList: // CommaExpList
        value.move< SyntaxTree::PtrList<SyntaxTree::Expr> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FParamList: // FParamList
      case symbol_kind::S_CommaFParamList: // CommaFParamList
        value.move< SyntaxTree::PtrList<SyntaxTree::FuncParam> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_GlobalDecl: // GlobalDecl
        value.move< SyntaxTree::PtrList<SyntaxTree::GlobalDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_BlockItemList: // BlockItemList
      case symbol_kind::S_BlockItem: // BlockItem
        value.move< SyntaxTree::PtrList<SyntaxTree::Stmt> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ConstDecl: // ConstDecl
      case symbol_kind::S_ConstDefList: // ConstDefList
      case symbol_kind::S_VarDecl: // VarDecl
      case symbol_kind::S_VarDefList: // VarDefList
        value.move< SyntaxTree::PtrList<SyntaxTree::VarDef> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_Stmt: // Stmt
      case symbol_kind::S_IfStmt: // IfStmt
        value.move< SyntaxTree::Stmt* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_BType: // BType
        value.move< SyntaxTree::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ConstDef: // ConstDef
      case symbol_kind::S_VarDef: // VarDef
        value.move< SyntaxTree::VarDef* > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_FLOATCONST: // FLOATCONST
        value.move< float > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INTCONST: // INTCONST
        value.move< int > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRINGCONST: // STRINGCONST
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  MHSJParser::stack_symbol_type&
  MHSJParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_CompUnit: // CompUnit
        value.copy< SyntaxTree::Assembly* > (that.value);
        break;

      case symbol_kind::S_Block: // Block
        value.copy< SyntaxTree::BlockStmt* > (that.value);
        break;

      case symbol_kind::S_OptionRet: // OptionRet
      case symbol_kind::S_Exp: // Exp
      case symbol_kind::S_RelExp: // RelExp
      case symbol_kind::S_EqExp: // EqExp
      case symbol_kind::S_LAndExp: // LAndExp
      case symbol_kind::S_LOrExp: // LOrExp
      case symbol_kind::S_CondExp: // CondExp
        value.copy< SyntaxTree::Expr* > (that.value);
        break;

      case symbol_kind::S_FuncDef: // FuncDef
        value.copy< SyntaxTree::FuncDef* > (that.value);
        break;

      case symbol_kind::S_FuncFParam: // FuncFParam
        value.copy< SyntaxTree::FuncParam* > (that.value);
        break;

      case symbol_kind::S_InitVal: // InitVal
      case symbol_kind::S_InitValList: // InitValList
      case symbol_kind::S_CommaInitValList: // CommaInitValList
        value.copy< SyntaxTree::InitVal* > (that.value);
        break;

      case symbol_kind::S_LVal: // LVal
        value.copy< SyntaxTree::LVal* > (that.value);
        break;

      case symbol_kind::S_Number: // Number
      case symbol_kind::S_String: // String
        value.copy< SyntaxTree::Literal* > (that.value);
        break;

      case symbol_kind::S_ArrayExpList: // ArrayExpList
      case symbol_kind::S_ExpList: // ExpList
      case symbol_kind::S_CommaExpList: // CommaExpList
        value.copy< SyntaxTree::PtrList<SyntaxTree::Expr> > (that.value);
        break;

      case symbol_kind::S_FParamList: // FParamList
      case symbol_kind::S_CommaFParamList: // CommaFParamList
        value.copy< SyntaxTree::PtrList<SyntaxTree::FuncParam> > (that.value);
        break;

      case symbol_kind::S_GlobalDecl: // GlobalDecl
        value.copy< SyntaxTree::PtrList<SyntaxTree::GlobalDef> > (that.value);
        break;

      case symbol_kind::S_BlockItemList: // BlockItemList
      case symbol_kind::S_BlockItem: // BlockItem
        value.copy< SyntaxTree::PtrList<SyntaxTree::Stmt> > (that.value);
        break;

      case symbol_kind::S_ConstDecl: // ConstDecl
      case symbol_kind::S_ConstDefList: // ConstDefList
      case symbol_kind::S_VarDecl: // VarDecl
      case symbol_kind::S_VarDefList: // VarDefList
        value.copy< SyntaxTree::PtrList<SyntaxTree::VarDef> > (that.value);
        break;

      case symbol_kind::S_Stmt: // Stmt
      case symbol_kind::S_IfStmt: // IfStmt
        value.copy< SyntaxTree::Stmt* > (that.value);
        break;

      case symbol_kind::S_BType: // BType
        value.copy< SyntaxTree::Type > (that.value);
        break;

      case symbol_kind::S_ConstDef: // ConstDef
      case symbol_kind::S_VarDef: // VarDef
        value.copy< SyntaxTree::VarDef* > (that.value);
        break;

      case symbol_kind::S_FLOATCONST: // FLOATCONST
        value.copy< float > (that.value);
        break;

      case symbol_kind::S_INTCONST: // INTCONST
        value.copy< int > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRINGCONST: // STRINGCONST
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  MHSJParser::stack_symbol_type&
  MHSJParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_CompUnit: // CompUnit
        value.move< SyntaxTree::Assembly* > (that.value);
        break;

      case symbol_kind::S_Block: // Block
        value.move< SyntaxTree::BlockStmt* > (that.value);
        break;

      case symbol_kind::S_OptionRet: // OptionRet
      case symbol_kind::S_Exp: // Exp
      case symbol_kind::S_RelExp: // RelExp
      case symbol_kind::S_EqExp: // EqExp
      case symbol_kind::S_LAndExp: // LAndExp
      case symbol_kind::S_LOrExp: // LOrExp
      case symbol_kind::S_CondExp: // CondExp
        value.move< SyntaxTree::Expr* > (that.value);
        break;

      case symbol_kind::S_FuncDef: // FuncDef
        value.move< SyntaxTree::FuncDef* > (that.value);
        break;

      case symbol_kind::S_FuncFParam: // FuncFParam
        value.move< SyntaxTree::FuncParam* > (that.value);
        break;

      case symbol_kind::S_InitVal: // InitVal
      case symbol_kind::S_InitValList: // InitValList
      case symbol_kind::S_CommaInitValList: // CommaInitValList
        value.move< SyntaxTree::InitVal* > (that.value);
        break;

      case symbol_kind::S_LVal: // LVal
        value.move< SyntaxTree::LVal* > (that.value);
        break;

      case symbol_kind::S_Number: // Number
      case symbol_kind::S_String: // String
        value.move< SyntaxTree::Literal* > (that.value);
        break;

      case symbol_kind::S_ArrayExpList: // ArrayExpList
      case symbol_kind::S_ExpList: // ExpList
      case symbol_kind::S_CommaExpList: // CommaExpList
        value.move< SyntaxTree::PtrList<SyntaxTree::Expr> > (that.value);
        break;

      case symbol_kind::S_FParamList: // FParamList
      case symbol_kind::S_CommaFParamList: // CommaFParamList
        value.move< SyntaxTree::PtrList<SyntaxTree::FuncParam> > (that.value);
        break;

      case symbol_kind::S_GlobalDecl: // GlobalDecl
        value.move< SyntaxTree::PtrList<SyntaxTree::GlobalDef> > (that.value);
        break;

      case symbol_kind::S_BlockItemList: // BlockItemList
      case symbol_kind::S_BlockItem: // BlockItem
        value.move< SyntaxTree::PtrList<SyntaxTree::Stmt> > (that.value);
        break;

      case symbol_kind::S_ConstDecl: // ConstDecl
      case symbol_kind::S_ConstDefList: // ConstDefList
      case symbol_kind::S_VarDecl: // VarDecl
      case symbol_kind::S_VarDefList: // VarDefList
        value.move< SyntaxTree::PtrList<SyntaxTree::VarDef> > (that.value);
        break;

      case symbol_kind::S_Stmt: // Stmt
      case symbol_kind::S_IfStmt: // IfStmt
        value.move< SyntaxTree::Stmt* > (that.value);
        break;

      case symbol_kind::S_BType: // BType
        value.move< SyntaxTree::Type > (that.value);
        break;

      case symbol_kind::S_ConstDef: // ConstDef
      case symbol_kind::S_VarDef: // VarDef
        value.move< SyntaxTree::VarDef* > (that.value);
        break;

      case symbol_kind::S_FLOATCONST: // FLOATCONST
        value.move< float > (that.value);
        break;

      case symbol_kind::S_INTCONST: // INTCONST
        value.move< int > (that.value);
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRINGCONST: // STRINGCONST
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  MHSJParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  MHSJParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  MHSJParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  MHSJParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  MHSJParser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  MHSJParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  MHSJParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  MHSJParser::debug_level_type
  MHSJParser::debug_level () const
  {
    return yydebug_;
  }

  void
  MHSJParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  MHSJParser::state_type
  MHSJParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  MHSJParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  MHSJParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  MHSJParser::operator() ()
  {
    return parse ();
  }

  int
  MHSJParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
#line 24 "../grammar/MHSJParser.yy"
{
// Initialize the initial location.
yyla.location.begin.filename = yyla.location.end.filename = &driver.file;
}

#line 789 "MHSJParser.cpp"


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (driver));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_CompUnit: // CompUnit
        yylhs.value.emplace< SyntaxTree::Assembly* > ();
        break;

      case symbol_kind::S_Block: // Block
        yylhs.value.emplace< SyntaxTree::BlockStmt* > ();
        break;

      case symbol_kind::S_OptionRet: // OptionRet
      case symbol_kind::S_Exp: // Exp
      case symbol_kind::S_RelExp: // RelExp
      case symbol_kind::S_EqExp: // EqExp
      case symbol_kind::S_LAndExp: // LAndExp
      case symbol_kind::S_LOrExp: // LOrExp
      case symbol_kind::S_CondExp: // CondExp
        yylhs.value.emplace< SyntaxTree::Expr* > ();
        break;

      case symbol_kind::S_FuncDef: // FuncDef
        yylhs.value.emplace< SyntaxTree::FuncDef* > ();
        break;

      case symbol_kind::S_FuncFParam: // FuncFParam
        yylhs.value.emplace< SyntaxTree::FuncParam* > ();
        break;

      case symbol_kind::S_InitVal: // InitVal
      case symbol_kind::S_InitValList: // InitValList
      case symbol_kind::S_CommaInitValList: // CommaInitValList
        yylhs.value.emplace< SyntaxTree::InitVal* > ();
        break;

      case symbol_kind::S_LVal: // LVal
        yylhs.value.emplace< SyntaxTree::LVal* > ();
        break;

      case symbol_kind::S_Number: // Number
      case symbol_kind::S_String: // String
        yylhs.value.emplace< SyntaxTree::Literal* > ();
        break;

      case symbol_kind::S_ArrayExpList: // ArrayExpList
      case symbol_kind::S_ExpList: // ExpList
      case symbol_kind::S_CommaExpList: // CommaExpList
        yylhs.value.emplace< SyntaxTree::PtrList<SyntaxTree::Expr> > ();
        break;

      case symbol_kind::S_FParamList: // FParamList
      case symbol_kind::S_CommaFParamList: // CommaFParamList
        yylhs.value.emplace< SyntaxTree::PtrList<SyntaxTree::FuncParam> > ();
        break;

      case symbol_kind::S_GlobalDecl: // GlobalDecl
        yylhs.value.emplace< SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ();
        break;

      case symbol_kind::S_BlockItemList: // BlockItemList
      case symbol_kind::S_BlockItem: // BlockItem
        yylhs.value.emplace< SyntaxTree::PtrList<SyntaxTree::Stmt> > ();
        break;

      case symbol_kind::S_ConstDecl: // ConstDecl
      case symbol_kind::S_ConstDefList: // ConstDefList
      case symbol_kind::S_VarDecl: // VarDecl
      case symbol_kind::S_VarDefList: // VarDefList
        yylhs.value.emplace< SyntaxTree::PtrList<SyntaxTree::VarDef> > ();
        break;

      case symbol_kind::S_Stmt: // Stmt
      case symbol_kind::S_IfStmt: // IfStmt
        yylhs.value.emplace< SyntaxTree::Stmt* > ();
        break;

      case symbol_kind::S_BType: // BType
        yylhs.value.emplace< SyntaxTree::Type > ();
        break;

      case symbol_kind::S_ConstDef: // ConstDef
      case symbol_kind::S_VarDef: // VarDef
        yylhs.value.emplace< SyntaxTree::VarDef* > ();
        break;

      case symbol_kind::S_FLOATCONST: // FLOATCONST
        yylhs.value.emplace< float > ();
        break;

      case symbol_kind::S_INTCONST: // INTCONST
        yylhs.value.emplace< int > ();
        break;

      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_STRINGCONST: // STRINGCONST
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // Begin: CompUnit END
#line 107 "../grammar/MHSJParser.yy"
                    {
    yystack_[1].value.as < SyntaxTree::Assembly* > ()->loc = yylhs.location;
    driver.root = yystack_[1].value.as < SyntaxTree::Assembly* > ();
    return 0;
  }
#line 1026 "MHSJParser.cpp"
    break;

  case 3: // CompUnit: CompUnit GlobalDecl
#line 114 "../grammar/MHSJParser.yy"
                            {
		yystack_[1].value.as < SyntaxTree::Assembly* > ()->global_defs.insert(yystack_[1].value.as < SyntaxTree::Assembly* > ()->global_defs.end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().end());
		yylhs.value.as < SyntaxTree::Assembly* > ()=yystack_[1].value.as < SyntaxTree::Assembly* > ();
	}
#line 1035 "MHSJParser.cpp"
    break;

  case 4: // CompUnit: GlobalDecl
#line 118 "../grammar/MHSJParser.yy"
                    {
		yylhs.value.as < SyntaxTree::Assembly* > ()=new SyntaxTree::Assembly();
		yylhs.value.as < SyntaxTree::Assembly* > ()->global_defs.insert(yylhs.value.as < SyntaxTree::Assembly* > ()->global_defs.end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().end());
  }
#line 1044 "MHSJParser.cpp"
    break;

  case 5: // GlobalDecl: ConstDecl
#line 124 "../grammar/MHSJParser.yy"
                    {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ()=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().insert(yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().end());
  }
#line 1053 "MHSJParser.cpp"
    break;

  case 6: // GlobalDecl: VarDecl
#line 128 "../grammar/MHSJParser.yy"
                 {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ()=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().insert(yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().end());
  }
#line 1062 "MHSJParser.cpp"
    break;

  case 7: // GlobalDecl: FuncDef
#line 132 "../grammar/MHSJParser.yy"
           {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ()=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::GlobalDef> > ().push_back(SyntaxTree::Ptr<SyntaxTree::GlobalDef>(yystack_[0].value.as < SyntaxTree::FuncDef* > ()));
  }
#line 1071 "MHSJParser.cpp"
    break;

  case 8: // ConstDecl: CONST BType ConstDefList SEMICOLON
#line 138 "../grammar/MHSJParser.yy"
                                            {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()=yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ();
    for (auto &node : yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()) {
      node->btype = yystack_[2].value.as < SyntaxTree::Type > ();
    }
  }
#line 1082 "MHSJParser.cpp"
    break;

  case 9: // ConstDefList: ConstDefList COMMA ConstDef
#line 145 "../grammar/MHSJParser.yy"
                                        {
    yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>(yystack_[0].value.as < SyntaxTree::VarDef* > ()));
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()=yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ();
  }
#line 1091 "MHSJParser.cpp"
    break;

  case 10: // ConstDefList: ConstDef
#line 149 "../grammar/MHSJParser.yy"
                  {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()=SyntaxTree::PtrList<SyntaxTree::VarDef>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>(yystack_[0].value.as < SyntaxTree::VarDef* > ()));
  }
#line 1100 "MHSJParser.cpp"
    break;

  case 11: // BType: INT
#line 155 "../grammar/MHSJParser.yy"
         {
  yylhs.value.as < SyntaxTree::Type > ()=SyntaxTree::Type::INT;
}
#line 1108 "MHSJParser.cpp"
    break;

  case 12: // ConstDef: IDENTIFIER ArrayExpList ASSIGN InitVal
#line 161 "../grammar/MHSJParser.yy"
                                               {//TODO:ADD ARRAY SUPPORT
    yylhs.value.as < SyntaxTree::VarDef* > ()=new SyntaxTree::VarDef();
    yylhs.value.as < SyntaxTree::VarDef* > ()->is_constant = true;
    yylhs.value.as < SyntaxTree::VarDef* > ()->is_inited = true;
    yylhs.value.as < SyntaxTree::VarDef* > ()->name=yystack_[3].value.as < std::string > ();
    yylhs.value.as < SyntaxTree::VarDef* > ()->array_length = yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
    yylhs.value.as < SyntaxTree::VarDef* > ()->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>(yystack_[0].value.as < SyntaxTree::InitVal* > ());
    yylhs.value.as < SyntaxTree::VarDef* > ()->loc = yylhs.location;
  }
#line 1122 "MHSJParser.cpp"
    break;

  case 13: // VarDecl: BType VarDefList SEMICOLON
#line 172 "../grammar/MHSJParser.yy"
                                  {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()=yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ();
    for (auto &node : yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()) {
      node->btype = yystack_[2].value.as < SyntaxTree::Type > ();
    }
  }
#line 1133 "MHSJParser.cpp"
    break;

  case 14: // VarDefList: VarDefList COMMA VarDef
#line 180 "../grammar/MHSJParser.yy"
                                  {
    yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>(yystack_[0].value.as < SyntaxTree::VarDef* > ()));
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()=yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ();
  }
#line 1142 "MHSJParser.cpp"
    break;

  case 15: // VarDefList: VarDef
#line 184 "../grammar/MHSJParser.yy"
                {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ()=SyntaxTree::PtrList<SyntaxTree::VarDef>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>(yystack_[0].value.as < SyntaxTree::VarDef* > ()));
  }
#line 1151 "MHSJParser.cpp"
    break;

  case 16: // VarDef: IDENTIFIER ArrayExpList
#line 190 "../grammar/MHSJParser.yy"
                              {
    yylhs.value.as < SyntaxTree::VarDef* > ()=new SyntaxTree::VarDef();
    yylhs.value.as < SyntaxTree::VarDef* > ()->name=yystack_[1].value.as < std::string > ();
    yylhs.value.as < SyntaxTree::VarDef* > ()->array_length = yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
    yylhs.value.as < SyntaxTree::VarDef* > ()->is_inited = false;
    yylhs.value.as < SyntaxTree::VarDef* > ()->loc = yylhs.location;
  }
#line 1163 "MHSJParser.cpp"
    break;

  case 17: // VarDef: IDENTIFIER ArrayExpList ASSIGN InitVal
#line 197 "../grammar/MHSJParser.yy"
                                                {
    yylhs.value.as < SyntaxTree::VarDef* > () = new SyntaxTree::VarDef();
    yylhs.value.as < SyntaxTree::VarDef* > ()->name = yystack_[3].value.as < std::string > ();
    yylhs.value.as < SyntaxTree::VarDef* > ()->array_length = yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
    yylhs.value.as < SyntaxTree::VarDef* > ()->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>(yystack_[0].value.as < SyntaxTree::InitVal* > ());
    yylhs.value.as < SyntaxTree::VarDef* > ()->is_inited = true;
    yylhs.value.as < SyntaxTree::VarDef* > ()->loc = yylhs.location;
  }
#line 1176 "MHSJParser.cpp"
    break;

  case 18: // ArrayExpList: ArrayExpList LBRACKET Exp RBRACKET
#line 207 "../grammar/MHSJParser.yy"
                                               {
    yystack_[3].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ().push_back(SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[1].value.as < SyntaxTree::Expr* > ()));
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ()=yystack_[3].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
  }
#line 1185 "MHSJParser.cpp"
    break;

  case 19: // ArrayExpList: %empty
#line 211 "../grammar/MHSJParser.yy"
                {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ()=SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
#line 1193 "MHSJParser.cpp"
    break;

  case 20: // InitVal: Exp
#line 216 "../grammar/MHSJParser.yy"
            {//TODO:CHECK?
    //TODO:Initializer cheking for scalar to array
    yylhs.value.as < SyntaxTree::InitVal* > () = new SyntaxTree::InitVal();
    yylhs.value.as < SyntaxTree::InitVal* > ()->isExp = true;
    yylhs.value.as < SyntaxTree::InitVal* > ()->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    //$$->elementList = std::vector<SyntaxTree::Ptr<SyntaxTree::InitVal>>();
    yylhs.value.as < SyntaxTree::InitVal* > ()->expr = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::InitVal* > ()->loc = yylhs.location;
  }
#line 1207 "MHSJParser.cpp"
    break;

  case 21: // InitVal: LBRACE InitValList RBRACE
#line 225 "../grammar/MHSJParser.yy"
                             {
    yylhs.value.as < SyntaxTree::InitVal* > () = yystack_[1].value.as < SyntaxTree::InitVal* > ();
  }
#line 1215 "MHSJParser.cpp"
    break;

  case 22: // InitValList: CommaInitValList InitVal
#line 230 "../grammar/MHSJParser.yy"
                                     {
    yystack_[1].value.as < SyntaxTree::InitVal* > ()->elementList.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>(yystack_[0].value.as < SyntaxTree::InitVal* > ()));
    yylhs.value.as < SyntaxTree::InitVal* > () = yystack_[1].value.as < SyntaxTree::InitVal* > ();
  }
#line 1224 "MHSJParser.cpp"
    break;

  case 23: // InitValList: %empty
#line 234 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::InitVal* > () = new SyntaxTree::InitVal();
    yylhs.value.as < SyntaxTree::InitVal* > ()->isExp = false;
    yylhs.value.as < SyntaxTree::InitVal* > ()->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    //$$->elementList = std::vector<SyntaxTree::Ptr<SyntaxTree::InitVal>>();
    yylhs.value.as < SyntaxTree::InitVal* > ()->expr = nullptr;
    yylhs.value.as < SyntaxTree::InitVal* > ()->loc = yylhs.location;
  }
#line 1237 "MHSJParser.cpp"
    break;

  case 24: // CommaInitValList: CommaInitValList InitVal COMMA
#line 244 "../grammar/MHSJParser.yy"
                                                {
    yystack_[2].value.as < SyntaxTree::InitVal* > ()->elementList.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>(yystack_[1].value.as < SyntaxTree::InitVal* > ()));
    yylhs.value.as < SyntaxTree::InitVal* > () = yystack_[2].value.as < SyntaxTree::InitVal* > ();
  }
#line 1246 "MHSJParser.cpp"
    break;

  case 25: // CommaInitValList: %empty
#line 248 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::InitVal* > () = new SyntaxTree::InitVal();
    yylhs.value.as < SyntaxTree::InitVal* > ()->isExp = false;
    yylhs.value.as < SyntaxTree::InitVal* > ()->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    //$$->elementList = std::vector<SyntaxTree::Ptr<SyntaxTree::InitVal>>();
    yylhs.value.as < SyntaxTree::InitVal* > ()->expr = nullptr;
    yylhs.value.as < SyntaxTree::InitVal* > ()->loc = yylhs.location;
  }
#line 1259 "MHSJParser.cpp"
    break;

  case 26: // ExpList: CommaExpList Exp
#line 258 "../grammar/MHSJParser.yy"
                        {
    yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ().push_back(SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ()));
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > () = yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
  }
#line 1268 "MHSJParser.cpp"
    break;

  case 27: // ExpList: %empty
#line 262 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > () = SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
#line 1276 "MHSJParser.cpp"
    break;

  case 28: // CommaExpList: CommaExpList Exp COMMA
#line 267 "../grammar/MHSJParser.yy"
                                   {
    yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ().push_back(SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[1].value.as < SyntaxTree::Expr* > ()));
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > () = yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
  }
#line 1285 "MHSJParser.cpp"
    break;

  case 29: // CommaExpList: %empty
#line 271 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > () = SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
#line 1293 "MHSJParser.cpp"
    break;

  case 30: // FuncFParam: BType IDENTIFIER ArrayExpList
#line 277 "../grammar/MHSJParser.yy"
                                        {
  yylhs.value.as < SyntaxTree::FuncParam* > () = new SyntaxTree::FuncParam();
  yylhs.value.as < SyntaxTree::FuncParam* > ()->param_type = yystack_[2].value.as < SyntaxTree::Type > ();
  yylhs.value.as < SyntaxTree::FuncParam* > ()->name = yystack_[1].value.as < std::string > ();
  yylhs.value.as < SyntaxTree::FuncParam* > ()->array_index = yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
  yylhs.value.as < SyntaxTree::FuncParam* > ()->loc = yylhs.location;
}
#line 1305 "MHSJParser.cpp"
    break;

  case 31: // FuncFParam: BType IDENTIFIER LRBRACKET ArrayExpList
#line 284 "../grammar/MHSJParser.yy"
                                         {
   yylhs.value.as < SyntaxTree::FuncParam* > () = new SyntaxTree::FuncParam();
   yylhs.value.as < SyntaxTree::FuncParam* > ()->param_type = yystack_[3].value.as < SyntaxTree::Type > ();
   yylhs.value.as < SyntaxTree::FuncParam* > ()->name = yystack_[2].value.as < std::string > ();
   yylhs.value.as < SyntaxTree::FuncParam* > ()->array_index = yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
   yylhs.value.as < SyntaxTree::FuncParam* > ()->array_index.insert(yylhs.value.as < SyntaxTree::FuncParam* > ()->array_index.begin(),nullptr);
   yylhs.value.as < SyntaxTree::FuncParam* > ()->loc = yylhs.location;
}
#line 1318 "MHSJParser.cpp"
    break;

  case 32: // FParamList: CommaFParamList FuncFParam
#line 294 "../grammar/MHSJParser.yy"
                                      {
  yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > ().push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>(yystack_[0].value.as < SyntaxTree::FuncParam* > ()));
  yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > () = yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > ();
}
#line 1327 "MHSJParser.cpp"
    break;

  case 33: // FParamList: %empty
#line 298 "../grammar/MHSJParser.yy"
        {
  yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > () = SyntaxTree::PtrList<SyntaxTree::FuncParam>();
}
#line 1335 "MHSJParser.cpp"
    break;

  case 34: // CommaFParamList: CommaFParamList FuncFParam COMMA
#line 303 "../grammar/MHSJParser.yy"
                                                {
  yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > ().push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>(yystack_[1].value.as < SyntaxTree::FuncParam* > ()));
  yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > () = yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > ();
}
#line 1344 "MHSJParser.cpp"
    break;

  case 35: // CommaFParamList: %empty
#line 307 "../grammar/MHSJParser.yy"
        {
  yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > () = SyntaxTree::PtrList<SyntaxTree::FuncParam>();
}
#line 1352 "MHSJParser.cpp"
    break;

  case 36: // FuncDef: BType IDENTIFIER LPARENTHESE FParamList RPARENTHESE Block
#line 311 "../grammar/MHSJParser.yy"
                                                                 {
    yylhs.value.as < SyntaxTree::FuncDef* > () = new SyntaxTree::FuncDef();
    yylhs.value.as < SyntaxTree::FuncDef* > ()->ret_type = yystack_[5].value.as < SyntaxTree::Type > ();
    yylhs.value.as < SyntaxTree::FuncDef* > ()->name = yystack_[4].value.as < std::string > ();
    auto tmp = new SyntaxTree::FuncFParamList();
    tmp->params = yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > ();
    yylhs.value.as < SyntaxTree::FuncDef* > ()->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(tmp);
    yylhs.value.as < SyntaxTree::FuncDef* > ()->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>(yystack_[0].value.as < SyntaxTree::BlockStmt* > ());
    yylhs.value.as < SyntaxTree::FuncDef* > ()->loc = yylhs.location;
  }
#line 1367 "MHSJParser.cpp"
    break;

  case 37: // FuncDef: VOID IDENTIFIER LPARENTHESE FParamList RPARENTHESE Block
#line 321 "../grammar/MHSJParser.yy"
                                                            {
    yylhs.value.as < SyntaxTree::FuncDef* > () = new SyntaxTree::FuncDef();
    yylhs.value.as < SyntaxTree::FuncDef* > ()->ret_type = SyntaxTree::Type::VOID;
    yylhs.value.as < SyntaxTree::FuncDef* > ()->name = yystack_[4].value.as < std::string > ();
    auto tmp = new SyntaxTree::FuncFParamList();
    tmp->params = yystack_[2].value.as < SyntaxTree::PtrList<SyntaxTree::FuncParam> > ();
    yylhs.value.as < SyntaxTree::FuncDef* > ()->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(tmp);
    yylhs.value.as < SyntaxTree::FuncDef* > ()->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>(yystack_[0].value.as < SyntaxTree::BlockStmt* > ());
    yylhs.value.as < SyntaxTree::FuncDef* > ()->loc = yylhs.location;
  }
#line 1382 "MHSJParser.cpp"
    break;

  case 38: // Block: LBRACE BlockItemList RBRACE
#line 333 "../grammar/MHSJParser.yy"
                                 {
    yylhs.value.as < SyntaxTree::BlockStmt* > () = new SyntaxTree::BlockStmt();
    yylhs.value.as < SyntaxTree::BlockStmt* > ()->body = yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ();
    yylhs.value.as < SyntaxTree::BlockStmt* > ()->loc = yylhs.location;
  }
#line 1392 "MHSJParser.cpp"
    break;

  case 39: // BlockItemList: BlockItemList BlockItem
#line 340 "../grammar/MHSJParser.yy"
                                     {
    yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().insert(yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().end());
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > () = yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ();
  }
#line 1401 "MHSJParser.cpp"
    break;

  case 40: // BlockItemList: %empty
#line 344 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > () = SyntaxTree::PtrList<SyntaxTree::Stmt>();
  }
#line 1409 "MHSJParser.cpp"
    break;

  case 41: // BlockItem: VarDecl
#line 349 "../grammar/MHSJParser.yy"
                 {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > () = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().insert(yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().end());
  }
#line 1418 "MHSJParser.cpp"
    break;

  case 42: // BlockItem: ConstDecl
#line 353 "../grammar/MHSJParser.yy"
             {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > () = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().insert(yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().end(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().begin(), yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::VarDef> > ().end());
  }
#line 1427 "MHSJParser.cpp"
    break;

  case 43: // BlockItem: Stmt
#line 357 "../grammar/MHSJParser.yy"
        {
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > () = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    yylhs.value.as < SyntaxTree::PtrList<SyntaxTree::Stmt> > ().push_back(SyntaxTree::Ptr<SyntaxTree::Stmt>(yystack_[0].value.as < SyntaxTree::Stmt* > ()));
  }
#line 1436 "MHSJParser.cpp"
    break;

  case 44: // Stmt: LVal ASSIGN Exp SEMICOLON
#line 363 "../grammar/MHSJParser.yy"
                              {
    auto temp = new SyntaxTree::AssignStmt();
    temp->target = SyntaxTree::Ptr<SyntaxTree::LVal>(yystack_[3].value.as < SyntaxTree::LVal* > ());
    temp->value = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[1].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Stmt* > () = temp;
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1448 "MHSJParser.cpp"
    break;

  case 45: // Stmt: Exp SEMICOLON
#line 370 "../grammar/MHSJParser.yy"
                 {
    auto temp = new SyntaxTree::ExprStmt();
    temp->exp = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[1].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Stmt* > () = temp;
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1459 "MHSJParser.cpp"
    break;

  case 46: // Stmt: RETURN OptionRet SEMICOLON
#line 376 "../grammar/MHSJParser.yy"
                              {
    auto temp = new SyntaxTree::ReturnStmt();
    temp->ret = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[1].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Stmt* > () = temp;
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1470 "MHSJParser.cpp"
    break;

  case 47: // Stmt: Block
#line 382 "../grammar/MHSJParser.yy"
         {
    yylhs.value.as < SyntaxTree::Stmt* > () = yystack_[0].value.as < SyntaxTree::BlockStmt* > ();
  }
#line 1478 "MHSJParser.cpp"
    break;

  case 48: // Stmt: WHILE LPARENTHESE CondExp RPARENTHESE Stmt
#line 385 "../grammar/MHSJParser.yy"
                                              {
    auto temp = new SyntaxTree::WhileStmt();
    temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->statement = SyntaxTree::Ptr<SyntaxTree::Stmt>(yystack_[0].value.as < SyntaxTree::Stmt* > ());
    yylhs.value.as < SyntaxTree::Stmt* > () = temp;
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1490 "MHSJParser.cpp"
    break;

  case 49: // Stmt: IfStmt
#line 392 "../grammar/MHSJParser.yy"
           {
    yylhs.value.as < SyntaxTree::Stmt* > () = yystack_[0].value.as < SyntaxTree::Stmt* > ();
  }
#line 1498 "MHSJParser.cpp"
    break;

  case 50: // Stmt: BREAK SEMICOLON
#line 395 "../grammar/MHSJParser.yy"
                    {
    yylhs.value.as < SyntaxTree::Stmt* > () = new SyntaxTree::BreakStmt();
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1507 "MHSJParser.cpp"
    break;

  case 51: // Stmt: CONTINUE SEMICOLON
#line 399 "../grammar/MHSJParser.yy"
                       {
    yylhs.value.as < SyntaxTree::Stmt* > () = new SyntaxTree::ContinueStmt();
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1516 "MHSJParser.cpp"
    break;

  case 52: // Stmt: SEMICOLON
#line 403 "../grammar/MHSJParser.yy"
             {
    yylhs.value.as < SyntaxTree::Stmt* > () = new SyntaxTree::EmptyStmt();
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1525 "MHSJParser.cpp"
    break;

  case 53: // IfStmt: IF LPARENTHESE CondExp RPARENTHESE Stmt
#line 409 "../grammar/MHSJParser.yy"
                                               {
    auto temp = new SyntaxTree::IfStmt();
    temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>(yystack_[0].value.as < SyntaxTree::Stmt* > ());
    temp->else_statement = nullptr;
    yylhs.value.as < SyntaxTree::Stmt* > () = temp;
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1538 "MHSJParser.cpp"
    break;

  case 54: // IfStmt: IF LPARENTHESE CondExp RPARENTHESE Stmt ELSE Stmt
#line 417 "../grammar/MHSJParser.yy"
                                                      {
    auto temp = new SyntaxTree::IfStmt();
    temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[4].value.as < SyntaxTree::Expr* > ());
    temp->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>(yystack_[2].value.as < SyntaxTree::Stmt* > ());
    temp->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>(yystack_[0].value.as < SyntaxTree::Stmt* > ());
    yylhs.value.as < SyntaxTree::Stmt* > () = temp;
    yylhs.value.as < SyntaxTree::Stmt* > ()->loc = yylhs.location;
  }
#line 1551 "MHSJParser.cpp"
    break;

  case 55: // OptionRet: Exp
#line 427 "../grammar/MHSJParser.yy"
             {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Expr* > ();
  }
#line 1559 "MHSJParser.cpp"
    break;

  case 56: // OptionRet: %empty
#line 430 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::Expr* > () = nullptr;
  }
#line 1567 "MHSJParser.cpp"
    break;

  case 57: // LVal: IDENTIFIER ArrayExpList
#line 435 "../grammar/MHSJParser.yy"
                            {
    yylhs.value.as < SyntaxTree::LVal* > () = new SyntaxTree::LVal();
    yylhs.value.as < SyntaxTree::LVal* > ()->name = yystack_[1].value.as < std::string > ();
    yylhs.value.as < SyntaxTree::LVal* > ()->array_index = yystack_[0].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
    yylhs.value.as < SyntaxTree::LVal* > ()->loc = yylhs.location;
  }
#line 1578 "MHSJParser.cpp"
    break;

  case 58: // Exp: PLUS Exp
#line 447 "../grammar/MHSJParser.yy"
                        {
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::PLUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1590 "MHSJParser.cpp"
    break;

  case 59: // Exp: MINUS Exp
#line 454 "../grammar/MHSJParser.yy"
                          {
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::MINUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1602 "MHSJParser.cpp"
    break;

  case 60: // Exp: NOT Exp
#line 461 "../grammar/MHSJParser.yy"
                      {
    auto temp = new SyntaxTree::UnaryCondExpr();
    temp->op = SyntaxTree::UnaryCondOp::NOT;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1614 "MHSJParser.cpp"
    break;

  case 61: // Exp: Exp PLUS Exp
#line 468 "../grammar/MHSJParser.yy"
                {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::PLUS;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1627 "MHSJParser.cpp"
    break;

  case 62: // Exp: Exp MINUS Exp
#line 476 "../grammar/MHSJParser.yy"
                 {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MINUS;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1640 "MHSJParser.cpp"
    break;

  case 63: // Exp: Exp MULTIPLY Exp
#line 484 "../grammar/MHSJParser.yy"
                    {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MULTIPLY;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1653 "MHSJParser.cpp"
    break;

  case 64: // Exp: Exp DIVIDE Exp
#line 492 "../grammar/MHSJParser.yy"
                  {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::DIVIDE;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1666 "MHSJParser.cpp"
    break;

  case 65: // Exp: Exp MODULO Exp
#line 500 "../grammar/MHSJParser.yy"
                  {
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MODULO;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1679 "MHSJParser.cpp"
    break;

  case 66: // Exp: LPARENTHESE Exp RPARENTHESE
#line 508 "../grammar/MHSJParser.yy"
                               {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[1].value.as < SyntaxTree::Expr* > ();
  }
#line 1687 "MHSJParser.cpp"
    break;

  case 67: // Exp: IDENTIFIER LPARENTHESE ExpList RPARENTHESE
#line 511 "../grammar/MHSJParser.yy"
                                               {
    auto temp = new SyntaxTree::FuncCallStmt();
    temp->name = yystack_[3].value.as < std::string > ();
    temp->params = yystack_[1].value.as < SyntaxTree::PtrList<SyntaxTree::Expr> > ();
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1699 "MHSJParser.cpp"
    break;

  case 68: // Exp: LVal
#line 518 "../grammar/MHSJParser.yy"
        {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::LVal* > ();
  }
#line 1707 "MHSJParser.cpp"
    break;

  case 69: // Exp: Number
#line 521 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Literal* > ();
  }
#line 1715 "MHSJParser.cpp"
    break;

  case 70: // Exp: String
#line 524 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Literal* > ();
  }
#line 1723 "MHSJParser.cpp"
    break;

  case 71: // RelExp: RelExp LT Exp
#line 529 "../grammar/MHSJParser.yy"
                    {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LT;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1736 "MHSJParser.cpp"
    break;

  case 72: // RelExp: RelExp LTE Exp
#line 537 "../grammar/MHSJParser.yy"
                 {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LTE;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1749 "MHSJParser.cpp"
    break;

  case 73: // RelExp: RelExp GT Exp
#line 545 "../grammar/MHSJParser.yy"
                {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::GT;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1762 "MHSJParser.cpp"
    break;

  case 74: // RelExp: RelExp GTE Exp
#line 553 "../grammar/MHSJParser.yy"
                 {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::GTE;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1775 "MHSJParser.cpp"
    break;

  case 75: // RelExp: Exp
#line 561 "../grammar/MHSJParser.yy"
       {//FIXME:type transfer
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Expr* > ();
  }
#line 1783 "MHSJParser.cpp"
    break;

  case 76: // EqExp: EqExp EQ RelExp
#line 566 "../grammar/MHSJParser.yy"
                     {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::EQ;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1796 "MHSJParser.cpp"
    break;

  case 77: // EqExp: EqExp NEQ RelExp
#line 574 "../grammar/MHSJParser.yy"
                   {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::NEQ;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1809 "MHSJParser.cpp"
    break;

  case 78: // EqExp: RelExp
#line 582 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Expr* > ();
  }
#line 1817 "MHSJParser.cpp"
    break;

  case 79: // LAndExp: LAndExp LOGICAND EqExp
#line 587 "../grammar/MHSJParser.yy"
                               {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LAND;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1830 "MHSJParser.cpp"
    break;

  case 80: // LAndExp: EqExp
#line 595 "../grammar/MHSJParser.yy"
        {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Expr* > ();
  }
#line 1838 "MHSJParser.cpp"
    break;

  case 81: // LOrExp: LOrExp LOGICOR LAndExp
#line 600 "../grammar/MHSJParser.yy"
                              {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LOR;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[2].value.as < SyntaxTree::Expr* > ());
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>(yystack_[0].value.as < SyntaxTree::Expr* > ());
    yylhs.value.as < SyntaxTree::Expr* > () = temp;
    yylhs.value.as < SyntaxTree::Expr* > ()->loc = yylhs.location;
  }
#line 1851 "MHSJParser.cpp"
    break;

  case 82: // LOrExp: LAndExp
#line 608 "../grammar/MHSJParser.yy"
          {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Expr* > ();
  }
#line 1859 "MHSJParser.cpp"
    break;

  case 83: // CondExp: LOrExp
#line 613 "../grammar/MHSJParser.yy"
              {
    yylhs.value.as < SyntaxTree::Expr* > () = yystack_[0].value.as < SyntaxTree::Expr* > ();
  }
#line 1867 "MHSJParser.cpp"
    break;

  case 84: // Number: INTCONST
#line 618 "../grammar/MHSJParser.yy"
                 {
    yylhs.value.as < SyntaxTree::Literal* > () = new SyntaxTree::Literal();
    yylhs.value.as < SyntaxTree::Literal* > ()->is_int = true;
    yylhs.value.as < SyntaxTree::Literal* > ()->int_const = yystack_[0].value.as < int > ();
    yylhs.value.as < SyntaxTree::Literal* > ()->loc = yylhs.location;
  }
#line 1878 "MHSJParser.cpp"
    break;

  case 85: // String: STRINGCONST
#line 626 "../grammar/MHSJParser.yy"
                    {
  yylhs.value.as < SyntaxTree::Literal* > () = new SyntaxTree::Literal();
  yylhs.value.as < SyntaxTree::Literal* > ()->is_int = false;
  yylhs.value.as < SyntaxTree::Literal* > ()->str = yystack_[0].value.as < std::string > ();
  yylhs.value.as < SyntaxTree::Literal* > ()->loc = yylhs.location;
}
#line 1889 "MHSJParser.cpp"
    break;


#line 1893 "MHSJParser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  MHSJParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  MHSJParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  MHSJParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // MHSJParser::context.
  MHSJParser::context::context (const MHSJParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  MHSJParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        int yychecklim = yylast_ - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }



  int
  MHSJParser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  MHSJParser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short MHSJParser::yypact_ninf_ = -129;

  const signed char MHSJParser::yytable_ninf_ = -36;

  const short
  MHSJParser::yypact_[] =
  {
       0,  -129,   -26,    -4,    29,    53,  -129,  -129,    16,  -129,
    -129,    17,    19,  -129,  -129,  -129,    41,    36,  -129,    56,
    -129,    92,  -129,    56,     5,  -129,    31,    67,    -4,    54,
    -129,    19,    74,    25,    18,  -129,  -129,    83,    88,   105,
      25,  -129,    83,    18,    18,    18,   102,   111,  -129,    18,
    -129,  -129,  -129,   167,  -129,  -129,   106,  -129,  -129,    75,
    -129,  -129,  -129,  -129,  -129,   129,   109,    25,   128,   130,
    -129,    18,    18,    18,    18,    18,  -129,    62,  -129,   130,
    -129,  -129,   134,   133,    18,     6,     6,  -129,  -129,  -129,
    -129,  -129,   135,    18,   147,   151,   152,  -129,    31,  -129,
    -129,  -129,  -129,  -129,   154,   150,   130,  -129,  -129,   137,
      18,   161,   167,    18,  -129,  -129,    18,  -129,  -129,   167,
       7,    57,   125,   132,   160,  -129,   162,   155,    18,    18,
      18,    18,    18,    18,    18,    18,   101,   101,  -129,   167,
     167,   167,   167,     7,     7,    57,   125,   156,  -129,   101,
    -129
  };

  const signed char
  MHSJParser::yydefact_[] =
  {
       0,    11,     0,     0,     0,     0,     4,     5,     0,     6,
       7,     0,     0,     1,     2,     3,    19,     0,    15,    33,
      19,     0,    10,    33,    16,    13,     0,     0,     0,     0,
       8,     0,     0,     0,     0,    19,    14,     0,     0,    32,
       0,     9,     0,     0,     0,     0,    25,    19,    84,     0,
      85,    17,    68,    20,    69,    70,     0,    40,    37,    19,
      34,    12,    36,    58,    59,     0,     0,     0,    29,    57,
      60,     0,     0,     0,     0,     0,    18,     0,    19,    30,
      66,    21,    22,     0,     0,    61,    62,    63,    64,    65,
      52,    38,     0,    56,     0,     0,     0,    42,     0,    41,
      47,    39,    43,    49,    68,     0,    31,    24,    67,    26,
       0,     0,    55,     0,    50,    51,     0,    45,    28,    75,
      78,    80,    82,    83,     0,    46,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    44,    72,
      73,    74,    71,    76,    77,    79,    81,    53,    48,     0,
      54
  };

  const short
  MHSJParser::yypgoto_[] =
  {
    -129,  -129,  -129,   177,   107,  -129,    -1,   157,   108,  -129,
     163,   -14,   -35,  -129,  -129,  -129,  -129,  -129,   164,  -129,
    -129,    35,  -129,  -129,  -128,  -129,  -129,   -74,   -33,   -12,
      49,    51,  -129,    77,  -129,  -129
  };

  const signed char
  MHSJParser::yydefgoto_[] =
  {
       0,     4,     5,     6,     7,    21,     8,    22,     9,    17,
      18,    24,    51,    66,    67,    83,    84,    39,    27,    28,
      10,   100,    77,   101,   102,   103,   111,    52,   119,   120,
     121,   122,   123,   124,    54,    55
  };

  const short
  MHSJParser::yytable_[] =
  {
      53,    56,    12,   104,    11,    61,    29,    53,   147,   148,
      63,    64,    65,    73,    74,    75,    70,   128,   129,   130,
      33,   150,     1,    43,    44,    34,     1,    38,     2,    13,
      43,    44,    82,    69,    53,    19,    45,     3,    85,    86,
      87,    88,    89,    45,   105,    79,    16,    46,    47,    20,
      48,   109,    25,    26,   131,    47,    14,    48,    49,    23,
     112,    35,   104,   104,   106,    49,    50,    43,    44,    40,
     132,   133,    58,    50,    34,   104,    98,    62,    90,     1,
      45,     2,   -35,   127,    57,    91,    37,    92,     1,    93,
       3,    94,    47,    42,    48,   139,   140,   141,   142,     3,
      95,    96,    49,   105,   105,    57,    43,    44,    30,    31,
      50,    71,    72,    73,    74,    75,   105,    90,    59,    45,
     143,   144,    60,    57,    78,   -23,    92,    76,    93,    68,
      94,    47,    81,    48,    71,    72,    73,    74,    75,    95,
      96,    49,    71,    72,    73,    74,    75,   -27,    80,    50,
      34,   107,   108,   110,   118,    71,    72,    73,    74,    75,
      71,    72,    73,    74,    75,   113,   117,   114,   115,   116,
     134,   138,    71,    72,    73,    74,    75,   125,   135,   136,
     149,   137,    15,   145,    97,    99,   146,    32,    41,    36,
     126
  };

  const unsigned char
  MHSJParser::yycheck_[] =
  {
      33,    34,     3,    77,    30,    40,    20,    40,   136,   137,
      43,    44,    45,     7,     8,     9,    49,    10,    11,    12,
      15,   149,    26,     5,     6,    20,    26,    28,    28,     0,
       5,     6,    67,    47,    67,    18,    18,    37,    71,    72,
      73,    74,    75,    18,    77,    59,    30,    22,    30,    30,
      32,    84,    16,    17,    47,    30,     3,    32,    40,    18,
      93,    30,   136,   137,    78,    40,    48,     5,     6,    15,
      13,    14,    37,    48,    20,   149,    77,    42,    16,    26,
      18,    28,    26,   116,    22,    23,    19,    25,    26,    27,
      37,    29,    30,    19,    32,   128,   129,   130,   131,    37,
      38,    39,    40,   136,   137,    22,     5,     6,    16,    17,
      48,     5,     6,     7,     8,     9,   149,    16,    30,    18,
     132,   133,    17,    22,    49,    23,    25,    21,    27,    18,
      29,    30,    23,    32,     5,     6,     7,     8,     9,    38,
      39,    40,     5,     6,     7,     8,     9,    19,    19,    48,
      20,    17,    19,    18,    17,     5,     6,     7,     8,     9,
       5,     6,     7,     8,     9,    18,    16,    16,    16,    15,
      45,    16,     5,     6,     7,     8,     9,    16,    46,    19,
      24,    19,     5,   134,    77,    77,   135,    23,    31,    26,
     113
  };

  const signed char
  MHSJParser::yystos_[] =
  {
       0,    26,    28,    37,    54,    55,    56,    57,    59,    61,
      73,    30,    59,     0,     3,    56,    30,    62,    63,    18,
      30,    58,    60,    18,    64,    16,    17,    71,    72,    64,
      16,    17,    71,    15,    20,    30,    63,    19,    59,    70,
      15,    60,    19,     5,     6,    18,    22,    30,    32,    40,
      48,    65,    80,    81,    87,    88,    81,    22,    74,    30,
      17,    65,    74,    81,    81,    81,    66,    67,    18,    64,
      81,     5,     6,     7,     8,     9,    21,    75,    49,    64,
      19,    23,    65,    68,    69,    81,    81,    81,    81,    81,
      16,    23,    25,    27,    29,    38,    39,    57,    59,    61,
      74,    76,    77,    78,    80,    81,    64,    17,    19,    81,
      18,    79,    81,    18,    16,    16,    15,    16,    17,    81,
      82,    83,    84,    85,    86,    16,    86,    81,    10,    11,
      12,    47,    13,    14,    45,    46,    19,    19,    16,    81,
      81,    81,    81,    82,    82,    83,    84,    77,    77,    24,
      77
  };

  const signed char
  MHSJParser::yyr1_[] =
  {
       0,    53,    54,    55,    55,    56,    56,    56,    57,    58,
      58,    59,    60,    61,    62,    62,    63,    63,    64,    64,
      65,    65,    66,    66,    67,    67,    68,    68,    69,    69,
      70,    70,    71,    71,    72,    72,    73,    73,    74,    75,
      75,    76,    76,    76,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    78,    78,    79,    79,    80,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    82,    82,    82,    82,    82,    83,    83,    83,    84,
      84,    85,    85,    86,    87,    88
  };

  const signed char
  MHSJParser::yyr2_[] =
  {
       0,     2,     2,     2,     1,     1,     1,     1,     4,     3,
       1,     1,     4,     3,     3,     1,     2,     4,     4,     0,
       1,     3,     2,     0,     3,     0,     2,     0,     3,     0,
       3,     4,     2,     0,     3,     0,     6,     6,     3,     2,
       0,     1,     1,     1,     4,     2,     3,     1,     5,     1,
       2,     2,     1,     5,     7,     1,     0,     2,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     4,     1,     1,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       1,     3,     1,     1,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const MHSJParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "END", "ERROR", "PLUS",
  "MINUS", "MULTIPLY", "DIVIDE", "MODULO", "LTE", "GT", "GTE", "EQ", "NEQ",
  "ASSIGN", "SEMICOLON", "COMMA", "LPARENTHESE", "RPARENTHESE", "LBRACKET",
  "RBRACKET", "LBRACE", "RBRACE", "ELSE", "IF", "INT", "RETURN", "VOID",
  "WHILE", "IDENTIFIER", "FLOATCONST", "INTCONST", "LETTER", "EOL",
  "COMMENT", "BLANK", "CONST", "BREAK", "CONTINUE", "NOT", "AND", "OR",
  "MOD", "FLOAT", "LOGICAND", "LOGICOR", "LT", "STRINGCONST", "LRBRACKET",
  "UPLUS", "UMINUS", "UNOT", "$accept", "Begin", "CompUnit", "GlobalDecl",
  "ConstDecl", "ConstDefList", "BType", "ConstDef", "VarDecl",
  "VarDefList", "VarDef", "ArrayExpList", "InitVal", "InitValList",
  "CommaInitValList", "ExpList", "CommaExpList", "FuncFParam",
  "FParamList", "CommaFParamList", "FuncDef", "Block", "BlockItemList",
  "BlockItem", "Stmt", "IfStmt", "OptionRet", "LVal", "Exp", "RelExp",
  "EqExp", "LAndExp", "LOrExp", "CondExp", "Number", "String", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  MHSJParser::yyrline_[] =
  {
       0,   107,   107,   114,   118,   124,   128,   132,   138,   145,
     149,   155,   161,   172,   180,   184,   190,   197,   207,   211,
     216,   225,   230,   234,   244,   248,   258,   262,   267,   271,
     277,   284,   294,   298,   303,   307,   311,   321,   333,   340,
     344,   349,   353,   357,   363,   370,   376,   382,   385,   392,
     395,   399,   403,   409,   417,   427,   430,   435,   447,   454,
     461,   468,   476,   484,   492,   500,   508,   511,   518,   521,
     524,   529,   537,   545,   553,   561,   566,   574,   582,   587,
     595,   600,   608,   613,   618,   626
  };

  void
  MHSJParser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  MHSJParser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 2474 "MHSJParser.cpp"

#line 634 "../grammar/MHSJParser.yy"


// Register errors to the driver:
void yy::MHSJParser::error (const location_type& l,
                          const std::string& m)
{
    driver.error(l, m);
}
