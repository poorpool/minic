//
// Created by poorpool on 2021/1/31.
//

#ifndef MINIC_EXPRESSION_H
#define MINIC_EXPRESSION_H

#include "format.h"

AstNode * expression(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * expression_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * term(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * term_(FILE *fp, AstNode * ret, TokenKind kind);

AstNode * factor(FILE *fp, AstNode * ret, TokenKind kind);

#endif //MINIC_EXPRESSION_H
