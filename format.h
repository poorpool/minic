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
    EXT_VAR_LIST, // 外部变量序列int a, b, c;
    FORMAL_ARG_LIST, // 形式参数序列 int a, float b, char c
    EXT_FUN_STATEMEMT, // 外部函数声明。fun_name (
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

void printNode(FILE *outfp, AstNode *p);

// 处理外部定义序列
AstNode * processExtDef(FILE *fp);

// 处理外部定义序列
AstNode * processExtDefList(FILE *fp);

// 处理外部变量序列
AstNode * processExtVarList(FILE *fp, AstNode *ret, TokenKind kind);

// 处理形式参数序列
AstNode * processFormalArgList(FILE *fp, AstNode *ret, TokenKind kind);


#endif //MINIC_FORMAT_H
