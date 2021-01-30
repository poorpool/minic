//
// Created by poorpool on 2021/1/30.
//

#include "print.h"

// 打印变量序列
// 注意不包含类型
void printVarList(FILE *outfp, AstNode *p) {
    fprintf(outfp, "%s", p->son[0]->text);
    fprintf(outfp, "%s", p->son[1]->text);
    fprintf(outfp, " ");
    if (p->num == 3) {
        printVarList(outfp, p->son[2]);
    }
}

// 打印形参序列
void printFormalArgList(FILE *outfp, AstNode *p) {
    fprintf(outfp, "%s", p->son[0]->text);
    fprintf(outfp, " %s", p->son[1]->text);
    if (p->num == 4) {
        fprintf(outfp, "%s", p->son[2]->text);
        fprintf(outfp, " ");
        printFormalArgList(outfp, p->son[3]);
    }
}

// 打印许多变量序列
void printLotsOfVarList(FILE *outfp, AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    printIndentation(outfp, iden);
    printNode(outfp, p->son[0]->son[0], iden);
    fprintf(outfp, " ");
    printVarList(outfp, p->son[0]->son[1]);
    fprintf(outfp, "\n");
    if (p->son[1] != NULL) {
        printLotsOfVarList(outfp, p->son[1], iden);
    }
}

// 打印 {} 包裹的复合语句
void printCompoundStatement(FILE *outfp, AstNode *p, int iden) {
    fprintf(outfp, " %s\n", p->son[0]->text);
    printLotsOfVarList(outfp, p->son[1], iden+1);
    fprintf(outfp, "%s", p->son[3]->text);
}

// 打印缩进
void printIndentation(FILE *outfp, int iden) {
    for (int i = 0; i < 4 * iden; i++) {
        fprintf(outfp, "%c", ' ');
    }
}

// 打印结点
void printNode(FILE *outfp, AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    if (p->text != NULL) { // fixme: 这个顺序对吗？
        fprintf(outfp,"%s", p->text);
    }
    switch (p->type) {
        case EXT_INCLUDE:
            printIndentation(outfp, iden);
            printNode(outfp, p->son[0], iden);
            printNode(outfp, p->son[1], iden);
            fprintf(outfp, " ");
            printNode(outfp, p->son[2], iden);
            fprintf(outfp, "\n");
            break;
        case VAR_LIST:
            printIndentation(outfp, iden);
            printNode(outfp, p->son[0], iden);
            fprintf(outfp, " ");
            printVarList(outfp, p->son[1]);
            fprintf(outfp, "\n");
            break;
        case EXT_FUN_STATEMEMT:
            printIndentation(outfp, iden);
            printNode(outfp, p->son[0], iden);
            fprintf(outfp, " ");
            printNode(outfp, p->son[1]->son[0], iden);
            printNode(outfp, p->son[1]->son[1], iden);
            printFormalArgList(outfp, p->son[1]->son[2]);
            printNode(outfp, p->son[1]->son[3], iden);
            printNode(outfp, p->son[1]->son[4], iden);
            fprintf(outfp, "\n");
            break;
        case EXT_FUN_DEFINITION:
            printIndentation(outfp, iden);
            printNode(outfp, p->son[0], iden);
            fprintf(outfp, " ");
            printNode(outfp, p->son[1]->son[0], iden);
            printNode(outfp, p->son[1]->son[1], iden);
            printFormalArgList(outfp, p->son[1]->son[2]);
            printNode(outfp, p->son[1]->son[3], iden);
            printCompoundStatement(outfp, p->son[1]->son[4], iden);
            fprintf(outfp, "\n");
            break;
        default:
            for (int i = 0; i < p->num; i++) {
                printNode(outfp, p->son[i], iden);
            }
    }
}