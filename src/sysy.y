%{
#include <iostream>
#include <vector>
#include <string>
#include "Ast.h"
#include "Type.h"
/* 链接到Flex的外部声明 */
extern int yylineno;
extern int yylex();
void yyerror(const char* s);
/* 指向AST根节点的指针 */
CompUnit* root = nullptr;
%}

%code requires {
    #include "Ast.h"
    #include "Type.h"
}


/* 联合体 */
%union {
    int intVal;
    float floatVal;
    std::string* strVal;
    Node* node;
    CompUnit* compUnit;
    Decl* decl;
    FuncDef* funcDef;
    Type* type;
    VarDef* varDef;
    VarDef::InitVal* initVal;
    FuncFParam* funcFParam;
    BlockStmt* block;
    Stmt* stmt;
    Exp* exp;
    LVal* lVal;

    std::vector<Node*>* vecNode;
    std::vector<VarDef*>* vecVarDef;
    std::vector<VarDef::InitVal*>* vecInitVal;
    std::vector<FuncFParam*>* vecFuncFParam;
    std::vector<Exp*>* vecExp;
}


%token <intVal> INT_CONST
%token <floatVal> FLOAT_CONST
%token <strVal> IDENT
/* 关键字和类型 */
%token INT FLOAT VOID CONST RETURN IF ELSE WHILE BREAK CONTINUE
/* 分隔符和运算符 */
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE COMMA SEMICOLON ASSIGN
%token PLUS MINUS MUL DIV MOD NOT EQ NE LT GT LE GE AND OR


%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV MOD
%right NOT UMINUS UPLUS 
/* 悬挂Else二义性 */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


/* 非终结符类型定义 */
%type <compUnit> CompUnit
%type <vecNode> CompUnitList BlockItemList
%type <node> BlockItem
%type <decl> Decl ConstDecl VarDecl
%type <type> BType
%type <vecVarDef> ConstDefList VarDefList
%type <varDef> ConstDef VarDef
%type <vecExp> ConstExpArrayList ExpArrayList FuncRParams
%type <initVal> ConstInitVal InitVal
%type <vecInitVal> ConstInitValList InitValList
%type <funcDef> FuncDef
%type <vecFuncFParam> FuncFParams
%type <funcFParam> FuncFParam
%type <block> Block
%type <stmt> Stmt
%type <exp> Exp Cond PrimaryExp UnaryExp MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp
%type <lVal> LVal 

%%




/* 语法规则、翻译模式 */


/* 顶层结构 */
CompUnit
    : CompUnitList { root = new CompUnit(*$1); delete $1; }
    ;

CompUnitList
    : CompUnitList Decl     { $$ = $1; $$->push_back($2); }
    | CompUnitList FuncDef  { $$ = $1; $$->push_back($2); }
    | Decl                  { $$ = new std::vector<Node*>(); $$->push_back($1); }
    | FuncDef               { $$ = new std::vector<Node*>(); $$->push_back($1); }
    ;



/* 声明*/
Decl
    : ConstDecl { $$ = $1; }
    | VarDecl   { $$ = $1; }
    ;

/* 基本类型 */
BType
    : INT   { $$ = Type::getIntType(); }
    | FLOAT { $$ = Type::getFloatType(); }
    ;

/* 常量声明 */
ConstDecl
    : CONST BType ConstDefList SEMICOLON { 
        $$ = new Decl(true, $2, *$3); 
        delete $3; 
    }
    ;

ConstDefList
    : ConstDefList COMMA ConstDef { $$ = $1; $$->push_back($3); }
    | ConstDef                    { $$ = new std::vector<VarDef*>(); $$->push_back($1); }
    ;

ConstDef
    : IDENT ConstExpArrayList ASSIGN ConstInitVal {
        $$ = new VarDef(*$1, *$2, $4);
        delete $1; delete $2; 
        $$->setLine(yylineno);
    }
    ;

ConstExpArrayList
    : ConstExpArrayList LBRACKET ConstExp RBRACKET { $$ = $1; $$->push_back($3); }
    |                                              { $$ = new std::vector<Exp*>(); }
    ;

ConstInitVal
    : ConstExp { 
        $$ = new VarDef::InitVal(); 
        $$->isList = false; 
        $$->expr = $1; 
    }
    | LBRACE ConstInitValList RBRACE {
        $$ = new VarDef::InitVal();
        $$->isList = true;
        $$->list = *$2;
        delete $2;
    }
    | LBRACE RBRACE {
        $$ = new VarDef::InitVal();
        $$->isList = true;
    }
    ;

ConstInitValList
    : ConstInitValList COMMA ConstInitVal { $$ = $1; $$->push_back($3); }
    | ConstInitVal                        { $$ = new std::vector<VarDef::InitVal*>(); $$->push_back($1); }
    ;

/* 变量声明  */
VarDecl
    : BType VarDefList SEMICOLON { 
        $$ = new Decl(false, $1, *$2); 
        delete $2; 
    }
    ;

VarDefList
    : VarDefList COMMA VarDef { $$ = $1; $$->push_back($3); }
    | VarDef                  { $$ = new std::vector<VarDef*>(); $$->push_back($1); }
    ;

VarDef
    : IDENT ConstExpArrayList {
        $$ = new VarDef(*$1, *$2, nullptr);
        delete $1; delete $2;
        $$->setLine(yylineno);
    }
    | IDENT ConstExpArrayList ASSIGN InitVal {
        $$ = new VarDef(*$1, *$2, $4);
        delete $1; delete $2;
        $$->setLine(yylineno);
    }
    ;

InitVal
    : Exp {
        $$ = new VarDef::InitVal();
        $$->isList = false;
        $$->expr = $1;
    }
    | LBRACE InitValList RBRACE {
        $$ = new VarDef::InitVal();
        $$->isList = true;
        $$->list = *$2;
        delete $2;
    }
    | LBRACE RBRACE {
        $$ = new VarDef::InitVal();
        $$->isList = true;
    }
    ;

InitValList
    : InitValList COMMA InitVal { $$ = $1; $$->push_back($3); }
    | InitVal                   { $$ = new std::vector<VarDef::InitVal*>(); $$->push_back($1); }
    ;




/* 函数 */
FuncDef
    : BType IDENT LPAREN FuncFParams RPAREN Block {
        $$ = new FuncDef($1, *$2, *$4, $6);
        delete $2; delete $4;
        $$->setLine(yylineno);
    }
    | BType IDENT LPAREN RPAREN Block {
        $$ = new FuncDef($1, *$2, std::vector<FuncFParam*>(), $5);
        delete $2;
        $$->setLine(yylineno);
    }
    | VOID IDENT LPAREN FuncFParams RPAREN Block {
        $$ = new FuncDef(Type::getVoidType(), *$2, *$4, $6);
        delete $2; delete $4;
        $$->setLine(yylineno);
    }
    | VOID IDENT LPAREN RPAREN Block {
        $$ = new FuncDef(Type::getVoidType(), *$2, std::vector<FuncFParam*>(), $5);
        delete $2;
        $$->setLine(yylineno);
    }
    ;

/* 函数形式参数 */
FuncFParams
    : FuncFParams COMMA FuncFParam { $$ = $1; $$->push_back($3); }
    | FuncFParam                   { $$ = new std::vector<FuncFParam*>(); $$->push_back($1); }
    ;

FuncFParam
    : BType IDENT {
        $$ = new FuncFParam($1, *$2, false, std::vector<Exp*>());
        delete $2;
        $$->setLine(yylineno);
    }
    | BType IDENT LBRACKET RBRACKET ExpArrayList {
        $$ = new FuncFParam($1, *$2, true, *$5);
        delete $2; delete $5;
        $$->setLine(yylineno);
    }
    ;

ExpArrayList
    : ExpArrayList LBRACKET Exp RBRACKET { $$ = $1; $$->push_back($3); }
    |                                    { $$ = new std::vector<Exp*>(); }
    ;

/* 代码块 */
Block
    : LBRACE BlockItemList RBRACE {
        $$ = new BlockStmt(*$2);
        delete $2;
    }
    ;

BlockItemList
    : BlockItemList BlockItem { $$ = $1; $$->push_back($2); }
    |                         { $$ = new std::vector<Node*>(); }
    ;

BlockItem
    : Decl { $$ = $1; }
    | Stmt { $$ = $1; }
    ;

/* 语句 */
Stmt
    : LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
        $$->setLine(yylineno);
    }
    | Exp SEMICOLON {
        $$ = new ExpStmt($1);
        $$->setLine(yylineno);
    }
    | SEMICOLON {
        $$ = new ExpStmt(nullptr); 
    }
    | Block {$$ = $1; }
    | IF LPAREN Cond RPAREN Stmt %prec LOWER_THAN_ELSE {
        /* 无Else的If，使用 %prec 解决移进归约冲突 */
        $$ = new IfStmt($3, $5);
        $$->setLine(yylineno);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfStmt($3, $5, $7);
        $$->setLine(yylineno);
    }
    | WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3, $5);
        $$->setLine(yylineno);
    }
    | BREAK SEMICOLON {
        $$ = new BreakStmt();
        $$->setLine(yylineno);
    }
    | CONTINUE SEMICOLON {
        $$ = new ContinueStmt();
        $$->setLine(yylineno);
    }
    | RETURN Exp SEMICOLON {
        $$ = new ReturnStmt($2);
        $$->setLine(yylineno);
    }
    | RETURN SEMICOLON {
        $$ = new ReturnStmt(nullptr);
        $$->setLine(yylineno);
    }
    ;

Exp : AddExp { $$ = $1; } ;
Cond : LOrExp { $$ = $1; } ;
ConstExp : AddExp { $$ = $1; } ;

LVal
    : IDENT ExpArrayList {
        $$ = new LVal(*$1, *$2);
        delete $1; delete $2;
        $$->setLine(yylineno);
    }
    ;

PrimaryExp
    : LPAREN Exp RPAREN { $$ = $2; }
    | LVal              { $$ = $1; }
    | INT_CONST         { $$ = new IntConst($1); $$->setLine(yylineno); }
    | FLOAT_CONST       { $$ = new FloatConst($1); $$->setLine(yylineno); }
    ;

UnaryExp
    : PrimaryExp { $$ = $1; }
    | IDENT LPAREN FuncRParams RPAREN {
        /* 带参数的函数调用 */
        $$ = new CallExp(*$1, *$3);
        delete $1; delete $3;
        $$->setLine(yylineno);
    }
    | IDENT LPAREN RPAREN {
        /* 无参数的函数调用 */
        $$ = new CallExp(*$1, std::vector<Exp*>());
        delete $1;
        $$->setLine(yylineno);
    }
    | PLUS UnaryExp %prec UPLUS {
        $$ = new UnaryExp(UnaryExp::PLUS, $2);
        $$->setLine(yylineno);
    }
    | MINUS UnaryExp %prec UMINUS {
        $$ = new UnaryExp(UnaryExp::MINUS, $2);
        $$->setLine(yylineno);
    }
    | NOT UnaryExp {
        $$ = new UnaryExp(UnaryExp::NOT, $2);
        $$->setLine(yylineno);
    }
    ;

/* 函数实参 */
FuncRParams
    : FuncRParams COMMA Exp { $$ = $1; $$->push_back($3); }
    | Exp                   { $$ = new std::vector<Exp*>(); $$->push_back($1); }
    ;

/* 运算 */
MulExp
    : UnaryExp { $$ = $1; }
    | MulExp MUL UnaryExp { $$ = new BinaryExp(BinaryExp::MUL, $1, $3); $$->setLine(yylineno); }
    | MulExp DIV UnaryExp { $$ = new BinaryExp(BinaryExp::DIV, $1, $3); $$->setLine(yylineno); }
    | MulExp MOD UnaryExp { $$ = new BinaryExp(BinaryExp::MOD, $1, $3); $$->setLine(yylineno); }
    ;

AddExp
    : MulExp { $$ = $1; }
    | AddExp PLUS MulExp  { $$ = new BinaryExp(BinaryExp::ADD, $1, $3); $$->setLine(yylineno); }
    | AddExp MINUS MulExp { $$ = new BinaryExp(BinaryExp::SUB, $1, $3); $$->setLine(yylineno); }
    ;

RelExp
    : AddExp { $$ = $1; }
    | RelExp LT AddExp { $$ = new BinaryExp(BinaryExp::LT, $1, $3); $$->setLine(yylineno); }
    | RelExp GT AddExp { $$ = new BinaryExp(BinaryExp::GT, $1, $3); $$->setLine(yylineno); }
    | RelExp LE AddExp { $$ = new BinaryExp(BinaryExp::LE, $1, $3); $$->setLine(yylineno); }
    | RelExp GE AddExp { $$ = new BinaryExp(BinaryExp::GE, $1, $3); $$->setLine(yylineno); }
    ;

EqExp
    : RelExp { $$ = $1; }
    | EqExp EQ RelExp { $$ = new BinaryExp(BinaryExp::EQ, $1, $3); $$->setLine(yylineno); }
    | EqExp NE RelExp { $$ = new BinaryExp(BinaryExp::NE, $1, $3); $$->setLine(yylineno); }
    ;

/* 逻辑运算 */
LAndExp
    : EqExp { $$ = $1; }
    | LAndExp AND EqExp { $$ = new BinaryExp(BinaryExp::AND, $1, $3); $$->setLine(yylineno); }
    ;

LOrExp
    : LAndExp { $$ = $1; }
    | LOrExp OR LAndExp { $$ = new BinaryExp(BinaryExp::OR, $1, $3); $$->setLine(yylineno); }
    ;

%%

void yyerror(const char* s) {
    std::cerr << "Parser Error: " << s << " at line " << yylineno << std::endl;
}
