//
// Created by poorpool on 2021/1/27.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scan.h"
#include "format.h"

extern int lineNumber;
extern TokenKind tokenKind;
extern char tokenText[];

char tokenTextTmp[1024];


AstNode *root;

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

AstNode * setAstNodeText(AstNode *p, const char * source) {
    int len = strlen(source);
    if (p->text != NULL)    free(p->text);
    p->text = (char *)malloc(sizeof(char)*(len + 1));
    strcpy(p->text, source);
    return p;
}

AstNode * newNode() {
    AstNode * ret = (AstNode*)malloc(sizeof(AstNode));
    ret->num = 0;
    ret->son = NULL;
    ret->type = UNKNOWN;
    ret->text = NULL;
    return ret;
}

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

void printExtVarList(FILE *outfp, AstNode *p) {
    fprintf(outfp, "%s", p->son[0]->text);
    fprintf(outfp, "%s", p->son[1]->text);
    fprintf(outfp, " ");
    if (p->num == 3) {
        printExtVarList(outfp, p->son[2]);
    }
}

void printExtFunStatement(FILE *outfp, AstNode *p) {
    fprintf(outfp, "%s", p->son[0]->text);
    fprintf(outfp, " %s", p->son[1]->text);
    if (p->num == 4) {
        fprintf(outfp, "%s", p->son[2]->text);
        fprintf(outfp, " ");
        printExtFunStatement(outfp, p->son[3]);
    }
}

void printNode(FILE *outfp, AstNode *p) {
    if (p == NULL) {
        return ;
    }
    if (p->text != NULL) { // fixme: 这个顺序对吗？
        fprintf(outfp,"%s", p->text);
    }
    switch (p->type) {
        case EXT_INCLUDE:
            printNode(outfp, p->son[0]);
            printNode(outfp, p->son[1]);
            fprintf(outfp, " ");
            printNode(outfp, p->son[2]);
            fprintf(outfp, "\n");
            break;
        case EXT_VAR_LIST:
            printNode(outfp, p->son[0]);
            fprintf(outfp, " ");
            printExtVarList(outfp, p->son[1]);
            fprintf(outfp, "\n");
            break;
        case EXT_FUN_STATEMEMT:
            printNode(outfp, p->son[0]);
            fprintf(outfp, " ");
            printNode(outfp, p->son[1]->son[0]);
            printNode(outfp, p->son[1]->son[1]);
            printExtFunStatement(outfp, p->son[1]->son[2]);
            printNode(outfp, p->son[1]->son[3]);
            printNode(outfp, p->son[1]->son[4]);
            fprintf(outfp, "\n");
            break;
        default:
            for (int i = 0; i < p->num; i++) {
                printNode(outfp, p->son[i]);
            }
    }
}

AstNode * processExtVarList(FILE *fp, AstNode *ret, TokenKind kind) {
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
            ret->son[2] = processExtVarList(fp, ret->son[2], tokenKind);
            return ret;
        }
    } else if (tokenKind == SEMI) {
        ret = allocSons(ret, 2);
        ret->son[0] = setAstNodeText(ret->son[0], tokenTextTmp);
        ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(SEMI));
        return ret;
    }
    return ret;
}

// 得到、留下不是int char float的第一个kind
AstNode * processFormalArgList(FILE *fp, AstNode *ret, TokenKind kind) {
    printf("now kind %s\n", getTokenKindStr(kind));
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
                ret->type = EXT_VAR_LIST;
                ret->son[1] = processExtVarList(fp, ret->son[1], tokenKind);
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
                if (tokenKind != SEMI) {
                    freeNode(ret);
                    panic("Doesn't get a ;!");
                    return NULL;
                } else {
                    ret->type = EXT_FUN_STATEMEMT;
                    ret->son[1]->son[4] = setAstNodeText(ret->son[1]->son[4], getTokenKindStr(SEMI));
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

    printNode(outfp, root);

    /*enum TokenKind kind;
    while ((kind = getToken(fp))) {
        if (kind == ENDOFFILE) {
            break;
        }
        printf("Line %d: %s\n", lineNumber, getTokenKindStr(kind));
    }*/
}

