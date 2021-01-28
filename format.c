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
    }
    for (int i = 0; i < p->num; i++) {
        freeNode(p->son[i]);
    }
    if (p->son != NULL) {
        free(p->son);
    }
    free(p);
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
        default:
            for (int i = 0; i < p->num; i++) {
                printNode(outfp, p->son[i]);
            }
    }
}

AstNode * processExtDef(FILE *fp) {
    tokenKind = getToken(fp);
    printf("get %s\n", getTokenKindStr(tokenKind));
    AstNode * ret = newNode();
    switch (tokenKind) {
        case ENDOFFILE:
            return NULL;
        case SHARP:
            tokenKind = getToken(fp);
            printf("get %s %d\n", getTokenKindStr(tokenKind), tokenKind == INCLUDE);
            if (tokenKind == INCLUDE) {

                ret->type = EXT_INCLUDE;
                ret = allocSons(ret, 3);
                ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(SHARP));
                ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(INCLUDE));
                printf("setting\n");
                tokenKind = getToken(fp);
                printf("get %s %s %d\n", getTokenKindStr(tokenKind), tokenText, tokenKind == STRING);
                if (tokenKind == STRING) {
                    ret->son[2] = setAstNodeText(ret->son[2], tokenText);
                    printf("%s\n", tokenText);
                    return ret;
                } else {
                    freeNode(ret);
                    panic("Include doesn't complete!");
                    return NULL;
                }
            }

    }
}

AstNode * processExtDefList(FILE *fp) {
    AstNode *ret = newNode();
    ret->type = EXT_DEF_LIST;
    ret = allocSonsWithoutMalloc(ret, 2);
    printf("okk\n");
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

