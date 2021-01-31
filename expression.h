//
// Created by poorpool on 2021/1/31.
//

#ifndef MINIC_EXPRESSION_H
#define MINIC_EXPRESSION_H

#include "format.h"

AstNode * actualExpression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * orExpression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * orExpression_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * andExpression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * andExpression_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * equalExpression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * equalExpression_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * compareExpression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * compareExpression_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * expression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * expression_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * term(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * term_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * factor(FILE *fp, AstNode * ret, TokenKind kind);

#endif //MINIC_EXPRESSION_H
