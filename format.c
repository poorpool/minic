//
// Created by poorpool on 2021/1/27.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scan.h"
#include "format.h"
#include "print.h"
#include "expression.h"

extern int lineNumber;
extern TokenKind tokenKind;
extern char tokenText[];

char tokenTextTmp[1024];

AstNode *root;

// 给 p 分配 n 个儿子，并把这 n 个儿子创建好
AstNode * allocSons(AstNode *p, int n) {
    if (p == NULL) {
        p = newNode();
    }
    p->num = n;
    p->son = (AstNode **)malloc(n*sizeof(AstNode *));
    for (int i = 0; i < n; i++) {
        p->son[i] = newNode();
    }
    return p;
}

// 给 p 分配 n 个儿子，不创建儿子
AstNode * allocSonsWithoutMalloc(AstNode *p, int n) {
    if (p == NULL) {
        p = newNode();
    }
    p->num = n;
    p->son = (AstNode **)malloc(n*sizeof(AstNode *));
    for (int i = 0; i < n; i++) {
        p->son[i] = NULL;
    }
    return p;
}

// 设置 p 的文字
AstNode * setAstNodeText(AstNode *p, const char * source) {
    int len = strlen(source);
    if (p->text != NULL)    free(p->text);
    p->text = (char *)malloc(sizeof(char)*(len + 1));
    strcpy(p->text, source);
    return p;
}

// 创建结点
AstNode * newNode() {
    AstNode * ret = (AstNode*)malloc(sizeof(AstNode));
    ret->num = 0;
    ret->son = NULL;
    ret->type = UNKNOWN;
    ret->text = NULL;
    return ret;
}

// 释放结点
void freeNode(AstNode *p) {
    if (p == NULL) {
        return ;
    }
    if (p->text != NULL) {
        free(p->text);
        p->text = NULL;
    }
    for (int i = 0; i < p->num; i++) {
        freeNode(p->son[i]);
        p->son[i] = NULL;
    }
    if (p->son != NULL) {
        free(p->son);
        p->son = NULL;
    }
    free(p);
}

// 处理变量序列，要传进来一个当前读到的 TokenKind，要是分号或者是逗号，并且把标识符放到 tokenTextTmp 里
// 注意不含类型（int, float, char）
// 不会多读
AstNode * processVarList(FILE *fp, AstNode *ret, TokenKind kind) {
    if (kind == COMMA) {
        ret = allocSons(ret, 3);
        ret->son[0] = setAstNodeText(ret->son[0], tokenTextTmp);
        ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(COMMA));
        tokenKind = getToken(fp);
        if (tokenKind != IDENT) {
            panic("Doesn't get an identifier!");
            return NULL;
        }
        strcpy(tokenTextTmp, tokenText);
        tokenKind = getToken(fp);
        if (tokenKind == COMMA || tokenKind == SEMI) {
            ret->son[2] = processVarList(fp, ret->son[2], tokenKind);
            return ret;
        }
    } else if (tokenKind == SEMI) {
        ret = allocSons(ret, 2);
        ret->son[0] = setAstNodeText(ret->son[0], tokenTextTmp);
        ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(SEMI));
        return ret;
    } else {
        panic("Var list cannot end!");
        return NULL;
    }
    return ret;
}

// 处理形式参数序列
// 要传一个 kind 且为 int/float/char
// 会多读一个
AstNode * processFormalArgList(FILE *fp, AstNode *ret, TokenKind kind) {
    if (kind == INT || kind == FLOAT || kind == CHAR) {
        tokenKind = getToken(fp);
        if (tokenKind != IDENT) {
            panic("Confusing data type!\n");
            return NULL;
        }
        strcpy(tokenTextTmp, tokenText);
        tokenKind = getToken(fp);
        if (tokenKind == COMMA) {
            ret = allocSons(ret, 4);
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(kind));
            ret->son[1] = setAstNodeText(ret->son[1], tokenTextTmp);
            ret->son[2] = setAstNodeText(ret->son[2], getTokenKindStr(COMMA));
            tokenKind = getToken(fp);
            ret->son[3] = processFormalArgList(fp, ret->son[3], tokenKind);
        } else {
            ret = allocSons(ret, 2);
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(kind));
            ret->son[1] = setAstNodeText(ret->son[1], tokenTextTmp);
        }
        return ret;
    } else {
        return NULL;
    }
}

// 得到许多变量序列
// 最后会多读一个
AstNode * getLotsOFVarList(FILE *fp, AstNode *ret) {
    tokenKind = getToken(fp);
    ret = allocSons(ret, 2);
    ret->son[0] = allocSons(ret->son[0], 2);
    ret->son[0]->son[0] = setAstNodeText(ret->son[0]->son[0], getTokenKindStr(tokenKind));
    if (tokenKind != INT && tokenKind != FLOAT && tokenKind != CHAR) {
        freeNode(ret);
        return NULL;
    }
    tokenKind = getToken(fp);
    strcpy(tokenTextTmp, tokenText);
    if (tokenKind != IDENT) {
        panic("Want an identifier!\n");
        return NULL;
    }
    tokenKind = getToken(fp);
    ret->son[0]->son[1] = processVarList(fp, ret->son[0]->son[1], tokenKind);
    ret->son[1] = getLotsOFVarList(fp, ret->son[1]);
    return ret;
}

// 处理一条语句
// 不会多读
AstNode * processSentence(FILE *fp, AstNode *ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->son[0] = expression(fp, ret->son[0], kind);
    if (tokenKind != SEMI) {
        panic("Want a ;!");
        return NULL;
    }
    ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(SEMI));
    return ret;
}

// 得到许多语句
// 会多读
AstNode * getLotsOfSentence(FILE *fp, AstNode *ret, TokenKind kind) {
    if (kind == RBRACE) {
        return NULL;
    } else {
        ret = allocSons(ret, 2);
        ret->son[0] = processSentence(fp, ret->son[0], kind);
        tokenKind = getToken(fp);
        ret->son[1] = getLotsOfSentence(fp, ret->son[1], tokenKind);
        return ret;
    }
}

// 得到 {} 包裹的复合语句
// 此时 tokenKind 一定是 LBRACE
// 不会多读
AstNode * processCompoundStatement(FILE *fp, AstNode *ret) {
    ret->type = COMPOUND_STATEMENT;
    ret = allocSons(ret, 4);
    ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LBRACE));
    ret->son[1] = getLotsOFVarList(fp, ret->son[1]);
    ret->son[2] = getLotsOfSentence(fp, ret->son[2], tokenKind);
    if (tokenKind != RBRACE) {
        panic("Want a }!");
        return NULL;
    }
    ret->son[3] = setAstNodeText(ret->son[3], getTokenKindStr(RBRACE));
    return ret;
}

// 得到外部定义，有多种情况
AstNode * processExtDef(FILE *fp) {
    tokenKind = getToken(fp);
    AstNode * ret = newNode();
    switch (tokenKind) {
        case ENDOFFILE:
            return NULL;
        case SHARP:
            tokenKind = getToken(fp);
            if (tokenKind == INCLUDE) {
                ret->type = EXT_INCLUDE;
                ret = allocSons(ret, 3);
                ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(SHARP));
                ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(INCLUDE));
                tokenKind = getToken(fp);
                if (tokenKind == STRING) {
                    ret->son[2] = setAstNodeText(ret->son[2], tokenText);
                    return ret;
                } else {
                    freeNode(ret);
                    panic("Include doesn't complete!");
                    return NULL;
                }
            } else {
                freeNode(ret);
                panic("Confusing #!");
                return NULL;
            }
            break;
        case INT:
        case FLOAT:
        case CHAR:
            ret = allocSons(ret, 2);
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(tokenKind));
            tokenKind = getToken(fp);
            if (tokenKind != IDENT) {
                freeNode(ret);
                panic("Doesn't get an identifier!");
                return NULL;
            }
            strcpy(tokenTextTmp, tokenText);
            tokenKind = getToken(fp);
            if (tokenKind == COMMA || tokenKind == SEMI) {
                ret->type = VAR_LIST;
                ret->son[1] = processVarList(fp, ret->son[1], tokenKind);
                return ret;
            } else if (tokenKind == LP) {
                ret->son[1] = allocSons(ret->son[1], 5);
                ret->son[1]->son[0] = setAstNodeText(ret->son[1]->son[0], tokenTextTmp);
                ret->son[1]->son[1] = setAstNodeText(ret->son[1]->son[1], getTokenKindStr(LP));
                tokenKind = getToken(fp);
                ret->son[1]->son[2] = processFormalArgList(fp, ret->son[1]->son[2], tokenKind);
                // 这里不需要 getToken，因为 processFormalArgList 已经 get 过了
                if (tokenKind != RP) {
                    freeNode(ret);
                    panic("Doesn't get a )!");
                    return NULL;
                }
                ret->son[1]->son[3] = setAstNodeText(ret->son[1]->son[3], getTokenKindStr(RP));
                tokenKind = getToken(fp);
                if (tokenKind == SEMI) {
                    ret->type = EXT_FUN_STATEMEMT;
                    ret->son[1]->son[4] = setAstNodeText(ret->son[1]->son[4], getTokenKindStr(SEMI));
                } else if (tokenKind == LBRACE) {
                    ret->type = EXT_FUN_DEFINITION;
                    ret->son[1]->son[4] = processCompoundStatement(fp, ret->son[1]->son[4]);
                } else {
                    freeNode(ret);
                    panic("Doesn't get a ;!");
                    return NULL;
                }
                return ret;
            } else {
                freeNode(ret);
                panic("Confusing data type!\n");
                return NULL;
            }
        default:
            panic("还没处理呢!");
            return NULL;
    }
}

AstNode * processExtDefList(FILE *fp) {
    AstNode *ret = newNode();
    ret->type = EXT_DEF_LIST;
    ret = allocSonsWithoutMalloc(ret, 2);
    ret->son[0] = processExtDef(fp);
    if (ret->son[0] == NULL) {
        freeNode(ret);
        return NULL;
    }
    ret->son[1] = processExtDefList(fp);
    return ret;
}

void format(FILE *fp, FILE *outfp) {
    root = processExtDefList(fp);
    printNode(outfp, root, 0);
}

