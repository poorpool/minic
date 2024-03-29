//
// Created by poorpool on 2021/1/30.
//

#ifndef MINIC_PRINT_H
#define MINIC_PRINT_H
#include <stdio.h>
#include "format.h"

void printIndentation(FILE *outfp, int iden);

void printCompoundStatement(FILE *outfp, AstNode *p, int iden);

void printExpression(FILE *outfp, AstNode *p);

void printNode(FILE *outfp, AstNode *p, int iden);

#endif //MINIC_PRINT_H
