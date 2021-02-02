//
// Created by poorpool on 2021/1/31.
//

#include <string.h>
#include "expression.h"
#include "print.h"
extern int lineNumber;
extern TokenKind tokenKind;
extern char tokenText[];

/*
 * 所有 *_ 函数都会在最后多读一个
 * 非 _ 函数因为最后调用 _ 所以也会多读一个
 * */

AstNode * actualExpression(FILE *fp, AstNode * ret, TokenKind kind) {
    AstNode * orExp = orExpression(fp, newNode(), kind);

    if (tokenKind == ASSIGN) {
        ret = allocSons(ret, 3);
        ret->type = ACTUAL_EXPRESSION;
        ret->son[0] = orExp;
        ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(ASSIGN));
        tokenKind = getToken(fp);
        ret->son[2] = actualExpression(fp, ret->son[2], tokenKind);
        return ret;
    } else {
        return orExp;
    }
}

AstNode * orExpression(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->type = OR_EXPRESSION;
    ret->son[0] = andExpression(fp, ret->son[0], kind);
    ret->son[1] = orExpression_(fp, ret->son[1], tokenKind);
    return ret;
}

AstNode * orExpression_(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 3);
    ret->type = OR_EXPRESSION_;
    switch (kind) {
        case LOGIC_OR:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LOGIC_OR));
            tokenKind = getToken(fp);
            ret->son[1] = andExpression(fp, ret->son[1], tokenKind);
            ret->son[2] = orExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case SEMI:
        case RP:
        case COMMA:
        case RBRACKET:
        case ASSIGN:
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling andExpression!");
            return NULL;
    }
}

AstNode * andExpression(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->type = AND_EXPRESSION;
    ret->son[0] = equalExpression(fp, ret->son[0], kind);
    ret->son[1] = andExpression_(fp, ret->son[1], tokenKind);
    return ret;
}

AstNode * andExpression_(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 3);
    ret->type = AND_EXPRESSION_;
    switch (kind) {
        case LOGIC_AND:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LOGIC_AND));
            tokenKind = getToken(fp);
            ret->son[1] = equalExpression(fp, ret->son[1], tokenKind);
            ret->son[2] = andExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case LOGIC_OR:
        case SEMI:
        case RP:
        case COMMA:
        case RBRACKET:
        case ASSIGN:
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling andExpression!");
            return NULL;
    }
}

AstNode * equalExpression(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->type = EQUAL_EXPRESSION;
    ret->son[0] = compareExpression(fp, ret->son[0], kind);
    ret->son[1] = equalExpression_(fp, ret->son[1], tokenKind);
    return ret;
}

AstNode * equalExpression_(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 3);
    ret->type = EQUAL_EXPRESSION_;
    switch (kind) {
        case EQUAL:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(EQUAL));
            tokenKind = getToken(fp);
            ret->son[1] = compareExpression(fp, ret->son[1], tokenKind);
            ret->son[2] = equalExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case NEQ:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(NEQ));
            tokenKind = getToken(fp);
            ret->son[1] = compareExpression(fp, ret->son[1], tokenKind);
            ret->son[2] = equalExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case LOGIC_AND:
        case LOGIC_OR:
        case SEMI:
        case RP:
        case COMMA:
        case RBRACKET:
        case ASSIGN:
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling equalExpression!");
            return NULL;
    }
}

AstNode * compareExpression(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->type = COMPARE_EXPRESSION;
    ret->son[0] = expression(fp, ret->son[0], kind);
    ret->son[1] = compareExpression_(fp, ret->son[1], tokenKind);
    return ret;
}

AstNode * compareExpression_(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 3);
    ret->type = COMPARE_EXPRESSION_;
    switch (kind) {
        case GREAT:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(GREAT));
            tokenKind = getToken(fp);
            ret->son[1] = expression(fp, ret->son[1], tokenKind);
            ret->son[2] = compareExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case GEQ:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(GEQ));
            tokenKind = getToken(fp);
            ret->son[1] = expression(fp, ret->son[1], tokenKind);
            ret->son[2] = compareExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case LESS:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LESS));
            tokenKind = getToken(fp);
            ret->son[1] = expression(fp, ret->son[1], tokenKind);
            ret->son[2] = compareExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case LEQ:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LEQ));
            tokenKind = getToken(fp);
            ret->son[1] = expression(fp, ret->son[1], tokenKind);
            ret->son[2] = compareExpression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case EQUAL:
        case NEQ:
        case LOGIC_AND:
        case LOGIC_OR:
        case SEMI:
        case RP:
        case COMMA:
        case RBRACKET:
        case ASSIGN:
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling compareExpression!");
            return NULL;
    }
}

AstNode * expression(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->type = EXPRESSION;
    ret->son[0] = term(fp, ret->son[0], kind);
    ret->son[1] = expression_(fp, ret->son[1], tokenKind);
    return ret;
}

AstNode * expression_(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 3);
    ret->type = EXPRESSION_;
    switch (kind) {
        case PLUS:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(PLUS));
            tokenKind = getToken(fp);
            ret->son[1] = term(fp, ret->son[1], tokenKind);
            ret->son[2] = expression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case MINUS:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(MINUS));
            tokenKind = getToken(fp);
            ret->son[1] = term(fp, ret->son[1], tokenKind);
            ret->son[2] = expression_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case GREAT:
        case GEQ:
        case LESS:
        case LEQ:
        case EQUAL:
        case NEQ:
        case LOGIC_AND:
        case LOGIC_OR:
        case SEMI:
        case RP:
        case COMMA:
        case RBRACKET:
        case ASSIGN:
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling expression!");
            return NULL;
    }
}

AstNode * term(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 2);
    ret->type = TERM;
    ret->son[0] = factor(fp, ret->son[0], kind);
    ret->son[1] = term_(fp, ret->son[1], tokenKind);
    return ret;
}

AstNode * term_(FILE *fp, AstNode * ret, TokenKind kind) {
    ret = allocSons(ret, 3);
    ret->type = TERM_;
    switch (kind) {
        case TIME:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(TIME));
            tokenKind = getToken(fp);
            ret->son[1] = factor(fp, ret->son[1], tokenKind);
            ret->son[2] = term_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case DIVIDE:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(DIVIDE));
            tokenKind = getToken(fp);
            ret->son[1] = factor(fp, ret->son[1], tokenKind);
            ret->son[2] = term_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case MOD:
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(MOD));
            tokenKind = getToken(fp);
            ret->son[1] = factor(fp, ret->son[1], tokenKind);
            ret->son[2] = term_(fp, ret->son[2], tokenKind);
            return ret;
            break;
        case PLUS:
        case MINUS:
        case GREAT:
        case GEQ:
        case LESS:
        case LEQ:
        case EQUAL:
        case NEQ:
        case LOGIC_AND:
        case LOGIC_OR:
        case SEMI:
        case RP:
        case COMMA:
        case RBRACKET:
        case ASSIGN:
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling term!");
            return NULL;
    }
}

char tmp[1024];

AstNode * factor(FILE *fp, AstNode * ret, TokenKind kind) {
    ret->type = FACTOR;
    switch (kind) {
        case LP:
            ret = allocSons(ret, 3);
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LP));
            tokenKind = getToken(fp);
            ret->son[1] = actualExpression(fp, ret->son[1], tokenKind);
            if (tokenKind != RP) {
                panic("Want a )!");
                return NULL;
            }
            ret->son[2] = setAstNodeText(ret->son[2], getTokenKindStr(RP));
            break;
        case IDENT:
            strcpy(tmp, tokenText);
            tokenKind = getToken(fp);
            if (tokenKind == LP) {
                ret = allocSons(ret, 4);
                ret->son[0] = setAstNodeText(ret->son[0], tmp);
                ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(LP));
                ret->son[2] = getLotsOFExpressionList(fp, ret->son[2]);
                if (tokenKind != RP) {
                    panic("factor want a )!");
                    return NULL;
                }
                ret->son[3] = setAstNodeText(ret->son[3], getTokenKindStr(RP));
            } else if (tokenKind == LBRACKET) {
                ret = allocSons(ret, 4);
                ret->son[0] = setAstNodeText(ret->son[0], tmp);
                ret->son[1] = setAstNodeText(ret->son[1], getTokenKindStr(LBRACKET));
                tokenKind = getToken(fp);
                ret->son[2] = actualExpression(fp, ret->son[2], tokenKind);
                if (tokenKind != RBRACKET) {
                    panic("factor want a ]!");
                    return NULL;
                }
                ret->son[3] = setAstNodeText(ret->son[3], getTokenKindStr(RBRACKET));
            } else {
                unGetToken(fp, tokenKind);
                ret = setAstNodeText(ret, tmp);
            }
            break;
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
            ret = setAstNodeText(ret, tokenText);
            break;
    }
    tokenKind = getToken(fp);
    return ret;
}