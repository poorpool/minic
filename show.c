//
// Created by poorpool on 2021/2/4.
//

#include "print.h"
#include "show.h"

void showIndentation(int iden) {
    printIndentation(stdout, iden);
}

void showComment(AstNode *p, int iden) {
    showIndentation(iden);
    printf("comment: %s\n", p->text);
}

void showIdentOrArray(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    if (p->num == 2) {
        if (p->son[1] == NULL) {
            showIndentation(iden);
            printf("variable: %s\n", p->son[0]->text);
        } else {
            showIndentation(iden);
            printf("array: %s", p->son[0]->text);
            showIdentOrArray(p->son[1], iden);
        }
    } else {
        printf("[");
        printExpression(stdout, p->son[1]);
        printf("]");
        if (p->son[3] != NULL) {
            showIdentOrArray(p->son[3], iden);
        } else {
            printf("\n");
        }
    }
}

void showVarList(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    showIdentOrArray(p->son[0], iden);
    if (p->son[2] != NULL) {
        showVarList(p->son[2], iden);
    }
}

void showFormalArgs(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    showIndentation(iden);
    printf("type: %s, ", p->son[0]->text);
    showIdentOrArray(p->son[1], 0);
    if (p->num == 4) {
        showFormalArgs(p->son[3], iden);
    }
}

void showLotsOfVarList(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    if (p->son[0]->type == COMMENT_STATEMENT) {
        showComment(p->son[0], iden);
    } else {
        showIndentation(iden);
        printf("variable definition\n");
        showIndentation(iden+1);
        printf("type: %s\n", p->son[0]->son[0]->text);
        showIndentation(iden+1);
        printf("variables:\n");
        showVarList(p->son[0]->son[1], iden+2);
    }
    if (p->son[1] != NULL) {
        showLotsOfVarList(p->son[1], iden);
    }
}

void showIf(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    if (p->type == IF_STATEMENT) {
        showIndentation(iden);
        printf("if statement:\n");
        showIndentation(iden+1);
        printf("condition: ");
        printExpression(stdout, p->son[2]);
        printf("\n");
        showCompoundStatement(p->son[4], iden+1);
        showIf(p->son[5], iden);
    } else if (p->type == ELSE_IF_STATEMENT) {
        showIndentation(iden);
        printf("else if statement:\n");
        showIndentation(iden+1);
        printf("condition: ");
        printExpression(stdout, p->son[2]);
        printf("\n");
        showCompoundStatement(p->son[4], iden+1);
        showIf(p->son[5], iden);
    } else {
        showIndentation(iden);
        printf("else statement:\n");
        showCompoundStatement(p->son[1], iden+1);
    }
}

// 传的 iden 是递归到底的缩进，返回的是当前这一层的缩进
// 这个奇奇怪怪的写法是因为表达式是 LL(1) 得到的
int showExpression(AstNode *p, int iden) {
    if (p == NULL) {
        return iden;
    }
    switch (p->type) {
        case ACTUAL_EXPRESSION:
            showIndentation(iden);
            printf("%s operator:\n", p->son[1]->text);
            showExpression(p->son[0], iden+1);
            showExpression(p->son[2], iden+1);
            return iden;
            break;
        case OR_EXPRESSION:
        case AND_EXPRESSION:
        case EQUAL_EXPRESSION:
        case COMPARE_EXPRESSION:
        case EXPRESSION:
        case TERM:/*
            if (p->son[1] != NULL) {
                iden = showExpression(p->son[1], iden);
            }
            showExpression(p->son[0], iden);
            return iden;*/
            if (p->son[1] == NULL) {
                return showExpression(p->son[0], iden);
            }
            showIndentation(iden);
            printf("calculate as follow:\n");
            showExpression(p->son[0], iden+1);
            showExpression(p->son[1], iden);
            showIndentation(iden);
            printf("calculate end;\n");
            break;
        case OR_EXPRESSION_:
        case AND_EXPRESSION_:
        case EQUAL_EXPRESSION_:
        case COMPARE_EXPRESSION_:
        case EXPRESSION_:
        case TERM_:
            /*if (p->son[2] != NULL) {
                iden = showExpression(p->son[2], iden);
            }
            showIndentation(iden);
            printf("%s operator:\n", p->son[0]->text);
            showExpression(p->son[1], iden+1);
            return iden+1;*/
            showIndentation(iden+1);
            printf("then apply %s with result above and:\n", p->son[0]->text);
            showExpression(p->son[1], iden+1);
            if (p->son[2] != NULL) {
                return showExpression(p->son[2], iden);
            }
            break;
        case FACTOR:
            showIndentation(iden);
            printf("factor: ");
            printExpression(stdout, p);
            printf("\n");
            return iden;
            break;
    }
    return iden;
}

void showSentence(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    switch (p->type) {
        case COMMENT_STATEMENT:
            showComment(p, iden);
            break;
        case IF_STATEMENT:
            showIf(p, iden);
            break;
        case RETURN_STATEMENT:
            showIndentation(iden);
            printf("return: ");
            printExpression(stdout, p->son[1]);
            printf("\n");
            break;
        case BREAK_STATEMENT:
            showIndentation(iden);
            printf("break\n");
            break;
        case CONTINUE_STATEMENT:
            showIndentation(iden);
            printf("continue\n");
            break;
        case WHILE_STATEMENT:
            showIndentation(iden);
            printf("while statement\n");
            showIndentation(iden+1);
            printf("condition: ");
            printExpression(stdout, p->son[2]);
            printf("\n");
            showCompoundStatement(p->son[4], iden+1);
            break;
        case FOR_STATEMENT:
            showIndentation(iden);
            printf("for statement\n");
            showIndentation(iden+1);
            printf("initial expression: ");
            printExpression(stdout, p->son[2]);
            printf("\n");
            showIndentation(iden+1);
            printf("condition expression: ");
            printExpression(stdout, p->son[4]);
            printf("\n");
            showIndentation(iden+1);
            printf("modify expression: ");
            printExpression(stdout, p->son[6]);
            printf("\n");
            showCompoundStatement(p->son[8], iden+1);
            break;
        default:
            if (p->son[0] != NULL) {
                showExpression(p->son[0], iden);
            } else {
                showIndentation(iden);
                printf("empty sentence\n");
            }
    }
}

void showLotsOfSentence(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    showSentence(p->son[0], iden);
    if (p->son[1] != NULL) {
        showLotsOfSentence(p->son[1], iden);
    }
}

void showCompoundStatement(AstNode *p, int iden) {
    showIndentation(iden);
    printf("compound statement:\n");
    if (p->son[1] != NULL) {
        showIndentation(iden+1);
        printf("variable definitions:\n");
        showLotsOfVarList(p->son[1], iden+2);
    }
    if (p->son[2] != NULL) {
        showIndentation(iden+1);
        printf("sentences:\n");
        showLotsOfSentence(p->son[2], iden+2);
    }
}

void showNode(AstNode *p, int iden) {
    if (p == NULL) {
        return ;
    }
    switch (p->type) {
        case EXT_INCLUDE:
            printf("include statement\n");
            showIndentation(1);
            printf("filename: %s\n", p->son[2]->text);
            break;
        case EXT_DEFINE:
            printf("define statement\n");
            showIndentation(1);
            printf("defining：%s\n", p->son[2]->text);
            showIndentation(1);
            printf("to: %s\n", p->son[3]->text);
            break;
        case VAR_LIST:
            showIndentation(iden);
            printf("variable definition\n");
            showIndentation(iden+1);
            printf("type: %s\n", p->son[0]->text);
            showIndentation(iden+1);
            printf("variables:\n");
            showVarList(p->son[1], iden+2);
            break;
        case EXT_FUN_STATEMEMT:
            showIndentation(iden);
            printf("function statement\n");
            showIndentation(iden+1);
            printf("return type: %s\n", p->son[0]->text);
            showIndentation(iden+1);
            printf("function name: %s\n", p->son[1]->son[0]->son[0]->text);
            if (p->son[1]->son[2] != NULL) {
                showIndentation(iden+1);
                printf("formal arguments:\n");
                showFormalArgs(p->son[1]->son[2], iden+2);
            }
            break;
        case EXT_FUN_DEFINITION:
            showIndentation(iden);
            printf("function definition\n");
            showIndentation(iden+1);
            printf("return type: %s\n", p->son[0]->text);
            showIndentation(iden+1);
            printf("function name: %s\n", p->son[1]->son[0]->son[0]->text);
            if (p->son[1]->son[2] != NULL) {
                showIndentation(iden+1);
                printf("formal arguments:\n");
                showFormalArgs(p->son[1]->son[2], iden+2);
            }
            showCompoundStatement(p->son[1]->son[4], iden+1);
            break;
        case COMMENT_STATEMENT:
            showComment(p, iden);
            break;
        default:
            for (int i = 0; i < p->num; i++) {
                showNode(p->son[i], iden);
            }
    }
}