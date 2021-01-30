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
} ASTType;

struct astnode {
    ASTType type;
    int num; // 子节点个数
    struct astnode ** son;
    char * text;
};

typedef struct astnode AstNode;
// 读取 fp 程序，输出格式化程序到 outfp
void format(FILE *fp, FILE *outfp);

AstNode * allocSons(AstNode *p, int n);

AstNode * allocSonsWithoutMalloc(AstNode *p, int n);

AstNode * setAstNodeText(AstNode *p, const char * source);

AstNode * newNode();

void freeNode(AstNode *p);

// 打印缩进
void printIndentation(FILE *outfp, int iden);

void printNode(FILE *outfp, AstNode *p, int iden); // 和缩进

// 处理外部定义序列
AstNode * processExtDef(FILE *fp);

// 处理外部定义序列
AstNode * processExtDefList(FILE *fp);

// 处理外部变量序列
AstNode * processVarList(FILE *fp, AstNode *ret, TokenKind kind);

// 处理形式参数序列
AstNode * processFormalArgList(FILE *fp, AstNode *ret, TokenKind kind);

// 处理复合语句
AstNode * processCompoundStatement(FILE *fp, AstNode *ret);


#endif //MINIC_FORMAT_H
