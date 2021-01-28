//
// Created by poorpool on 2021/1/27.
//

#ifndef MINIC_SCAN_H
#define MINIC_SCAN_H

#include <stdio.h>
#include "main.h"

enum TokenKind {
    ERROR_TOKEN,
    ENDOFFILE, // 代表结束
    IDENT, // 标识符
    INT_CONST,
    FLOAT_CONST,
    CHAR_CONST,
    INT,
    FLOAT,
    CHAR,
    IF,
    ELSE,
    EQUAL, // ==
    LESS, // <
    LEQ, // <=
    GREAT, // >
    GEQ, // >=
    NEQ, // !=
    ASSIGN, // =
    LP, // (
    RP, // )
    SEMI, // ;
    COMMA, // ,
    PLUS, // +
    MINUS, // -
    TIME, // *
    DIVIDE, // /
    MOD, // %
    LOGIC_AND, // &&
    LOGIC_OR, // ||
    FOR,
    WHILE,
    RETURN,
    CONTINUE,
    EXTERN,
    STATIC,
    BREAK,
    SHARP, // #
    INCLUDE,
    DEFINE,
    LINE_COMMENT, // //
    BLOCK_COMMENT,
    STRING, // "text"
    LBRACKET, // [
    RBRACKET, // ]
    LBRACE, // {
    RBRACE, // }
};

enum TokenKind getToken(FILE *fp);

const char * getTokenKindStr(enum TokenKind kind);

#endif //MINIC_SCAN_H
