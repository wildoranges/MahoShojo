%skeleton "lalr1.cc" /* -*- c++ -*- */
%require "3.0"
%defines
//%define parser_class_name {MHSJParser}
%define api.parser.class {MHSJParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires
{
#include <string>
#include "SyntaxTree.h"
class MHSJDriver;
}

// The parsing context.
%param { MHSJDriver& driver }

// Location tracking
%locations
%initial-action
{
// Initialize the initial location.
@$.begin.filename = @$.end.filename = &driver.file;
};

// Enable tracing and verbose errors (which may be wrong!)
%define parse.trace
%define parse.error verbose

// Parser needs to know about the driver:
%code
{
#include "MHSJDriver.h"
#define yylex driver.lexer.yylex
}

// Tokens:
%define api.token.prefix {TOK_}

%token END
%token ERROR 258 PLUS 259 MINUS 260 MULTIPLY 261 DIVIDE 262 MODULO 263 LTE 264
%token GT 265 GTE 266 EQ 267 NEQ 268 ASSIGN 269 SEMICOLON 270 
%token COMMA 271 LPARENTHESE 272 RPARENTHESE 273 LBRACKET 274 
%token RBRACKET 275 LBRACE 276 RBRACE 277 ELSE 278 IF 279
%token INT 280 RETURN 281 VOID 282 WHILE 283 
%token <std::string>IDENTIFIER 284
%token <float>FLOATCONST 285
%token <int>INTCONST 286
%token LETTER 287 EOL 288 COMMENT 289 
%token BLANK 290 CONST 291 BREAK 292 CONTINUE 293 NOT 294
%token AND 295 OR 296 MOD 297
%token FLOAT 298
%token LOGICAND 299
%token LOGICOR 300
%token LT 301
%token <std::string>STRINGCONST 302
%token LRBRACKET 303
// Use variant-based semantic values: %type and %token expect genuine types

%type <SyntaxTree::Assembly*>CompUnit
%type <SyntaxTree::PtrList<SyntaxTree::GlobalDef>>GlobalDecl
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>ConstDecl
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>ConstDefList
%type <SyntaxTree::Type>BType
%type <SyntaxTree::VarDef*>ConstDef
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDecl
%type <SyntaxTree::PtrList<SyntaxTree::VarDef>>VarDefList
%type <SyntaxTree::VarDef*>VarDef
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>ArrayExpList
%type <SyntaxTree::InitVal*>InitVal
%type <SyntaxTree::InitVal*>InitValList
%type <SyntaxTree::InitVal*>CommaInitValList
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>ExpList
%type <SyntaxTree::PtrList<SyntaxTree::Expr>>CommaExpList
%type <SyntaxTree::FuncDef*>FuncDef
%type <SyntaxTree::BlockStmt*>Block
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItemList
%type <SyntaxTree::PtrList<SyntaxTree::Stmt>>BlockItem
%type <SyntaxTree::Stmt*>Stmt
%type <SyntaxTree::Expr*>OptionRet
%type <SyntaxTree::LVal*>LVal
%type <SyntaxTree::Expr*>Exp
%type <SyntaxTree::Literal*>Number
%type <SyntaxTree::Literal*>String

%type <SyntaxTree::FuncParam*>FuncFParam
%type <SyntaxTree::PtrList<SyntaxTree::FuncParam>>FParamList
%type <SyntaxTree::PtrList<SyntaxTree::FuncParam>>CommaFParamList
%type <SyntaxTree::Expr*>RelExp
%type <SyntaxTree::Expr*>EqExp
%type <SyntaxTree::Expr*>LAndExp
%type <SyntaxTree::Expr*>LOrExp
%type <SyntaxTree::Expr*>CondExp
%type <SyntaxTree::Stmt*>IfStmt

// No %destructors are needed, since memory will be reclaimed by the
// regular destructors.
/* %printer { yyoutput << $$; } <*>; TODO:trace_parsing*/   

// Grammar:
%start Begin 

%%
Begin: CompUnit END {
    $1->loc = @$;
    driver.root = $1;
    return 0;
  }
  ;

CompUnit:CompUnit GlobalDecl{
		$1->global_defs.insert($1->global_defs.end(), $2.begin(), $2.end());
		$$=$1;
	} 
	| GlobalDecl{
		$$=new SyntaxTree::Assembly();
		$$->global_defs.insert($$->global_defs.end(), $1.begin(), $1.end());
  }
	;

GlobalDecl:ConstDecl{
    $$=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
	| VarDecl{
    $$=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | FuncDef{
    $$=SyntaxTree::PtrList<SyntaxTree::GlobalDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::GlobalDef>($1));
  }
	;

ConstDecl:CONST BType ConstDefList SEMICOLON{
    $$=$3;
    for (auto &node : $$) {
      node->btype = $2;
    }
  }
	;
ConstDefList:ConstDefList COMMA ConstDef{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
    $$=$1;
  }
	| ConstDef{
    $$=SyntaxTree::PtrList<SyntaxTree::VarDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
  }
	;

BType:INT{
  $$=SyntaxTree::Type::INT;
}
;


ConstDef:IDENTIFIER ArrayExpList ASSIGN InitVal{//TODO:ADD ARRAY SUPPORT
    $$=new SyntaxTree::VarDef();
    $$->is_constant = true;
    $$->is_inited = true;
    $$->name=$1;
    $$->array_length = $2;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($4);
    $$->loc = @$;
  }
	;

VarDecl:BType VarDefList SEMICOLON{
    $$=$2;
    for (auto &node : $$) {
      node->btype = $1;
    }
  }
	;

VarDefList:VarDefList COMMA VarDef{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($3));
    $$=$1;
  }
	| VarDef{
    $$=SyntaxTree::PtrList<SyntaxTree::VarDef>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::VarDef>($1));
  }
	;

VarDef:IDENTIFIER ArrayExpList{
    $$=new SyntaxTree::VarDef();
    $$->name=$1;
    $$->array_length = $2;
    $$->is_inited = false;
    $$->loc = @$;
  }
	| IDENTIFIER ArrayExpList ASSIGN InitVal{
    $$ = new SyntaxTree::VarDef();
    $$->name = $1;
    $$->array_length = $2;
    $$->initializers = SyntaxTree::Ptr<SyntaxTree::InitVal>($4);
    $$->is_inited = true;
    $$->loc = @$;
  }
	;

ArrayExpList:ArrayExpList LBRACKET Exp RBRACKET{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($3));
    $$=$1;
  }
	| %empty{
    $$=SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
  ;

InitVal: Exp{//TODO:CHECK?
    //TODO:Initializer cheking for scalar to array
    $$ = new SyntaxTree::InitVal();
    $$->isExp = true;
    $$->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    //$$->elementList = std::vector<SyntaxTree::Ptr<SyntaxTree::InitVal>>();
    $$->expr = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$->loc = @$;
  }
  | LBRACE InitValList RBRACE{
    $$ = $2;
  }
  ;

InitValList: CommaInitValList InitVal{
    $1->elementList.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($2));
    $$ = $1;
  }
  | %empty{
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    //$$->elementList = std::vector<SyntaxTree::Ptr<SyntaxTree::InitVal>>();
    $$->expr = nullptr;
    $$->loc = @$;
  }
  ;

CommaInitValList: CommaInitValList InitVal COMMA{
    $1->elementList.push_back(SyntaxTree::Ptr<SyntaxTree::InitVal>($2));
    $$ = $1;
  }
  | %empty{
    $$ = new SyntaxTree::InitVal();
    $$->isExp = false;
    $$->elementList = SyntaxTree::PtrList<SyntaxTree::InitVal>();
    //$$->elementList = std::vector<SyntaxTree::Ptr<SyntaxTree::InitVal>>();
    $$->expr = nullptr;
    $$->loc = @$;
  }
  ;

ExpList:CommaExpList Exp{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($2));
    $$ = $1;
  }
  | %empty{
    $$ = SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
	;

CommaExpList:CommaExpList Exp COMMA{
    $1.push_back(SyntaxTree::Ptr<SyntaxTree::Expr>($2));
    $$ = $1;
  }
  | %empty{
    $$ = SyntaxTree::PtrList<SyntaxTree::Expr>();
  }
  ;


FuncFParam:BType IDENTIFIER ArrayExpList{
  $$ = new SyntaxTree::FuncParam();
  $$->param_type = $1;
  $$->name = $2;
  $$->array_index = $3;
  $$->loc = @$;
}
| BType IDENTIFIER LRBRACKET ArrayExpList{
   $$ = new SyntaxTree::FuncParam();
   $$->param_type = $1;
   $$->name = $2;
   $$->array_index = $4;
   $$->array_index.insert($$->array_index.begin(),nullptr);
   $$->loc = @$;
}
;

FParamList: CommaFParamList FuncFParam{
  $1.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($2));
  $$ = $1;
}
| %empty{
  $$ = SyntaxTree::PtrList<SyntaxTree::FuncParam>();
}
;

CommaFParamList:CommaFParamList FuncFParam COMMA{
  $1.push_back(SyntaxTree::Ptr<SyntaxTree::FuncParam>($2));
  $$ = $1;
}
| %empty{
  $$ = SyntaxTree::PtrList<SyntaxTree::FuncParam>();
}
;
FuncDef:BType IDENTIFIER LPARENTHESE FParamList RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = $1;
    $$->name = $2;
    auto tmp = new SyntaxTree::FuncFParamList();
    tmp->params = $4;
    $$->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(tmp);
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($6);
    $$->loc = @$;
  }
  | VOID IDENTIFIER LPARENTHESE FParamList RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = SyntaxTree::Type::VOID;
    $$->name = $2;
    auto tmp = new SyntaxTree::FuncFParamList();
    tmp->params = $4;
    $$->param_list = SyntaxTree::Ptr<SyntaxTree::FuncFParamList>(tmp);
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($6);
    $$->loc = @$;
  }
  ;

Block:LBRACE BlockItemList RBRACE{
    $$ = new SyntaxTree::BlockStmt();
    $$->body = $2;
    $$->loc = @$;
  }
  ;

BlockItemList:BlockItemList BlockItem{
    $1.insert($1.end(), $2.begin(), $2.end());
    $$ = $1;
  }
  | %empty{
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
  }
  ;

BlockItem:VarDecl{
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | ConstDecl{
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.insert($$.end(), $1.begin(), $1.end());
  }
  | Stmt{
    $$ = SyntaxTree::PtrList<SyntaxTree::Stmt>();
    $$.push_back(SyntaxTree::Ptr<SyntaxTree::Stmt>($1));
  }
  ;

Stmt:LVal ASSIGN Exp SEMICOLON{
    auto temp = new SyntaxTree::AssignStmt();
    temp->target = SyntaxTree::Ptr<SyntaxTree::LVal>($1);
    temp->value = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | Exp SEMICOLON{
    auto temp = new SyntaxTree::ExprStmt();
    temp->exp = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    $$ = temp;
    $$->loc = @$;
  }
  | RETURN OptionRet SEMICOLON{
    auto temp = new SyntaxTree::ReturnStmt();
    temp->ret = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
  }
  | Block{
    $$ = $1;
  }
  | WHILE LPARENTHESE CondExp RPARENTHESE Stmt{
    auto temp = new SyntaxTree::WhileStmt();
    temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp->statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    $$ = temp;
    $$->loc = @$;
  }
  | IfStmt {
    $$ = $1;
  }
  | BREAK SEMICOLON {
    $$ = new SyntaxTree::BreakStmt();
    $$->loc = @$;
  }
  | CONTINUE SEMICOLON {
    $$ = new SyntaxTree::ContinueStmt();
    $$->loc = @$;
  }
  | SEMICOLON{
    $$ = new SyntaxTree::EmptyStmt();
    $$->loc = @$;
  }
  ;

IfStmt:IF LPARENTHESE CondExp RPARENTHESE Stmt {
    auto temp = new SyntaxTree::IfStmt();
    temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    temp->else_statement = nullptr;
    $$ = temp;
    $$->loc = @$;
  }
  | IF LPARENTHESE CondExp RPARENTHESE Stmt ELSE Stmt {
    auto temp = new SyntaxTree::IfStmt();
    temp->cond_exp = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    temp->if_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($5);
    temp->else_statement = SyntaxTree::Ptr<SyntaxTree::Stmt>($7);
    $$ = temp;
    $$->loc = @$;
  }
  ;

OptionRet:Exp{
    $$ = $1;
  }
  | %empty{
    $$ = nullptr;
  }
  ;

LVal:IDENTIFIER ArrayExpList{
    $$ = new SyntaxTree::LVal();
    $$->name = $1;
    $$->array_index = $2;
    $$->loc = @$;
  }
  ;

%left PLUS MINUS;
%left MULTIPLY DIVIDE MODULO;
%precedence UPLUS UMINUS UNOT;

Exp:PLUS Exp %prec UPLUS{
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::PLUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
  }
  | MINUS Exp %prec UMINUS{
    auto temp = new SyntaxTree::UnaryExpr();
    temp->op = SyntaxTree::UnaryOp::MINUS;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
  }
  | NOT Exp %prec UNOT{
    auto temp = new SyntaxTree::UnaryCondExpr();
    temp->op = SyntaxTree::UnaryCondOp::NOT;
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($2);
    $$ = temp;
    $$->loc = @$;
  }
  | Exp PLUS Exp{
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::PLUS;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | Exp MINUS Exp{
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MINUS;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | Exp MULTIPLY Exp{
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MULTIPLY;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | Exp DIVIDE Exp{
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::DIVIDE;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | Exp MODULO Exp{
    auto temp = new SyntaxTree::BinaryExpr();
    temp->op = SyntaxTree::BinOp::MODULO;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  | LPARENTHESE Exp RPARENTHESE{
    $$ = $2;
  }
  | IDENTIFIER LPARENTHESE ExpList RPARENTHESE {
    auto temp = new SyntaxTree::FuncCallStmt();
    temp->name = $1;
    temp->params = $3;
    $$ = temp;
    $$->loc = @$;
  }
  | LVal{
    $$ = $1;
  }
  | Number{
    $$ = $1;
  }
  | String{
    $$ = $1;
  }
  ;

RelExp:RelExp LT Exp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LT;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |RelExp LTE Exp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LTE;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |RelExp GT Exp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::GT;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |RelExp GTE Exp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::GTE;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |Exp {//FIXME:type transfer
    $$ = $1;
  }
  ;

EqExp:EqExp EQ RelExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::EQ;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |EqExp NEQ RelExp{
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::NEQ;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |RelExp {
    $$ = $1;
  }
  ;

LAndExp:LAndExp LOGICAND EqExp {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LAND;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |EqExp{
    $$ = $1;
  }
  ;

LOrExp:LOrExp LOGICOR LAndExp {
    auto temp = new SyntaxTree::BinaryCondExpr();
    temp->op = SyntaxTree::BinaryCondOp::LOR;
    temp->lhs = SyntaxTree::Ptr<SyntaxTree::Expr>($1);
    temp->rhs = SyntaxTree::Ptr<SyntaxTree::Expr>($3);
    $$ = temp;
    $$->loc = @$;
  }
  |LAndExp{
    $$ = $1;
  }
  ;

CondExp:LOrExp{
    $$ = $1;
  }
  ;

Number: INTCONST {
    $$ = new SyntaxTree::Literal();
    $$->is_int = true;
    $$->int_const = $1;
    $$->loc = @$;
  }
  ;

String: STRINGCONST {
  $$ = new SyntaxTree::Literal();
  $$->is_int = false;
  $$->str = $1;
  $$->loc = @$;
}
;

%%

// Register errors to the driver:
void yy::MHSJParser::error (const location_type& l,
                          const std::string& m)
{
    driver.error(l, m);
}
