//
// Created by poorpool on 2021/1/27.
//
#include <stdio.h>
#include <string.h>
#include "scan.h"
#include "format.h"

extern int lineNumber;

void format(FILE *fp, FILE *outfp) {
    enum TokenKind kind;
    while ((kind = getToken(fp))) {
        if (kind == ENDOFFILE) {
            break;
        }
        printf("Line %d: %s\n", lineNumber, getTokenKindStr(kind));
    }
}

