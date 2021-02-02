//
// Created by poorpool on 2021/1/27.
//

#ifndef MINIC_FORMAT_H
#define MINIC_FORMAT_H
#include <stdio.h>
#include <stdlib.h>
#include "scan.h"

typedef enum {
    UNKNOWN, // 并不关心
    EXT_DEF_LIST, // 外部定义序列
    EXT_INCLUDE, // 外部定义 include
    VAR_LIST, // 外部变量序列int a, b, c;（不含）
    // FORMAL_ARG_LIST,  形式参数序列 int a, float b, char c 似乎没用到
    EXT_FUN_STATEMEMT, // 外部函数声明。int fun_name(形参序列);
    EXT_FUN_DEFINITION, // 外部函数定义，int fun_name(形参序列) 复合语句
    COMPOUND_STATEMENT, // 复合语句，{ 许多变量定义 许多语句 }
    IF_STATEMENT,
    ELSE_IF_STATEMENT,
    ELSE_STATEMENT,

    // 下面是表达式解析用到的 type，具体看文档
    ACTUAL_EXPRESSION,
    OR_EXPRESSION,
    OR_EXPRESSION_,
    AND_EXPRESSION,
    AND_EXPRESSION_,
    EQUAL_EXPRESSION,
    EQUAL_EXPRESSION_,
    COMPARE_EXPRESSION,
    COMPARE_EXPRESSION_,
    EXPRESSION,
    EXPRESSION_,
    TERM,
    TERM_,
    FACTOR
} ASTType;

struct astnode {
    ASTType type;
    int num; // 子节点个数
    struct astnode ** son;
    char * text;
};

typedef struct astnode AstNode;

void format(FILE *fp, FILE *outfp);

AstNode * allocSons(AstNode *p, int n);

AstNode * allocSonsWithoutMalloc(AstNode *p, int n);

AstNode * setAstNodeText(AstNode *p, const char * source);

AstNode * newNode();

void freeNode(AstNode *p);

AstNode * processExtDef(FILE *fp);

AstNode * processExtDefList(FILE *fp);

AstNode * processVarList(FILE *fp, AstNode *ret, TokenKind kind);

AstNode * processFormalArgList(FILE *fp, AstNode *ret, TokenKind kind);

AstNode * processIf(FILE *fp, AstNode *ret, TokenKind kind);

AstNode * processSentence(FILE *fp, AstNode *ret, TokenKind kind);

AstNode * getLotsOFVarList(FILE *fp, AstNode *ret);

AstNode * getLotsOfSentence(FILE *fp, AstNode *ret, TokenKind kind);

AstNode * processCompoundStatement(FILE *fp, AstNode *ret);

#endif //MINIC_FORMAT_H
