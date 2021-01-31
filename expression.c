//
// Created by poorpool on 2021/1/31.
//

#include "expression.h"

extern int lineNumber;
extern TokenKind tokenKind;
extern char tokenText[];

// 底下的都会吞一个
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
            freeNode(ret);
            return NULL;
            break;
        default:
            panic("Error in handling expression!");
            return NULL;
    }
}

AstNode * factor(FILE *fp, AstNode * ret, TokenKind kind) {
    ret->type = FACTOR;
    switch (kind) {
        case LP:
            ret = allocSons(ret, 3);
            ret->son[0] = setAstNodeText(ret->son[0], getTokenKindStr(LP));
            tokenKind = getToken(fp);
            ret->son[1] = expression(fp, ret->son[1], tokenKind);
            if (tokenKind != RP) {
                panic("Want a )!");
                return NULL;
            }
            ret->son[2] = setAstNodeText(ret->son[2], getTokenKindStr(RP));
            break;
        case IDENT:
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
            ret = setAstNodeText(ret, tokenText);
            break;
    }
    tokenKind = getToken(fp);
    return ret;
}