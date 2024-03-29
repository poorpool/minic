//
// Created by poorpool on 2021/1/27.
//

#include <ctype.h>
#include <string.h>
#include "scan.h"

int lineNumber = 1; // 当前行号为 1
int tokenLen = 0;
TokenKind tokenKind;
char tokenText[1024];

int wantChar(FILE *fp, int want) {
    int c = fgetc(fp);
    if (c != want) {
        ungetc(c, fp);
        return 0;
    }
    return 1;
}

void getTextUntilNewLine(FILE *fp) {
    tokenLen = 0;
    tokenText[0] = '\0';
    int c;
    while ((c = fgetc(fp))) {
        if (feof(fp)) {
            return ;
        }
        if (!isascii(c)) {
            panic("Not ascii character!");
            return ;
        }
        if (isspace(c)) {
            if (c == '\n') {
                lineNumber++;
            }
        } else {
            tokenText[tokenLen++] = c;
            break;
        }
    }
    while ((c = fgetc(fp))) {
        if (feof(fp)) {
            return ;
        }
        if (!isascii(c)) {
            panic("Not ascii character!");
            return ;
        }
        if (isspace(c)) {
            if (c == '\n') {
                ungetc(c, fp);
                tokenText[tokenLen] = '\0';
                return ;
            }
        }
        tokenText[tokenLen++] = c;
    }
    tokenText[tokenLen] = '\0';
}

TokenKind gettoken(FILE *fp) {
    if (feof(fp)) {
        return ENDOFFILE;
    }
    int c;
    while ((c = fgetc(fp))) {
        if (feof(fp)) {
            return ENDOFFILE;
        }
        if (!isascii(c)) {
            panic("Not ascii character!");
            return ERROR_TOKEN;
        }
        if (isspace(c)) {
            if (c == '\n') {
                lineNumber++;
            }
        } else {
            break;
        }
    }

    switch (c) {
        case '=':
            if (wantChar(fp, '=')) {
                return EQUAL;
            } else {
                return ASSIGN;
            }
            break;
        case '<':
            if (wantChar(fp, '=')) {
                return LEQ;
            } else {
                return LESS;
            }
            break;
        case '>':
            if (wantChar(fp, '=')) {
                return GEQ;
            } else {
                return GREAT;
            }
            break;
        case '#':
            return SHARP;
            break;
        case '!':
            if (wantChar(fp, '=')) {
                return NEQ;
            } else {
                panic("Get ! alone");
                return ERROR_TOKEN;
            }
            break;
        case '(':
            return LP;
            break;
        case ')':
            return RP;
            break;
        case ';':
            return SEMI;
            break;
        case ',':
            return COMMA;
            break;
        case '+':
            return PLUS;
            break;
        case '-':
            return MINUS;
            break;
        case '*':
            return TIME;
            break;
        case '/':
            if (wantChar(fp, '/')) {
                tokenLen = 2;
                tokenText[0] = tokenText[1] = '/';
                while ((c = fgetc(fp))) {
                    if (c == EOF) {
                        break;
                    }
                    if (c == '\n') {
                        ungetc(c, fp);
                        break;
                    }
                    tokenText[tokenLen++] = c;
                }
                tokenText[tokenLen] = '\0';
                return LINE_COMMENT;
            } else if (wantChar(fp, '*')) {
                tokenLen = 2;
                tokenText[0] = '/';
                tokenText[1] = '*';
                while ((c = fgetc(fp))) {
                    if (c == EOF) {
                        panic("Comment does not close!");
                        return ERROR_TOKEN;
                    }
                    if (c == '\n') {
                        lineNumber++;
                    }
                    tokenText[tokenLen++] = c;
                    if (c == '/' && tokenLen >= 4 && tokenText[tokenLen-2] == '*') {
                        break;
                    }
                }
                tokenText[tokenLen] = '\0';
                return BLOCK_COMMENT;
            } else {
                return DIVIDE;
            }
            break;
        case '%':
            return MOD;
            break;
        case '&':
            if (wantChar(fp, '&')) {
                return LOGIC_AND;
            } else {
                panic("Get & alone");
                return ERROR_TOKEN;
            }
            break;
        case '|':
            if (wantChar(fp, '|')) {
                return LOGIC_OR;
            } else {
                panic("Get & alone");
                return ERROR_TOKEN;
            }
            break;
        case '"':
            tokenLen = 1;
            tokenText[0] = c;
            while ((c = fgetc(fp))) {
                if (c == EOF) {
                    panic("String has no end");
                    return ERROR_TOKEN;
                } else if (c == '\n') {
                    panic("String gets a new line");
                    return ERROR_TOKEN;
                } else {
                    tokenText[tokenLen++] = c;
                }
                if (c == '"') {
                    break;
                }
            }
            tokenText[tokenLen] = '\0';
            return STRING;
            break;
        case '[':
            return LBRACKET;
            break;
        case ']':
            return RBRACKET;
            break;
        case '{':
            return LBRACE;
            break;
        case '}':
            return RBRACE;
            break;
        default:
            ;
    }
    if (c == '\'') {
        if (wantChar(fp, '\\')) {
            c = fgetc(fp);
            if (!isascii(c)) {
                panic("Illegal char (not ascii)!");
                return ERROR_TOKEN;
            }
            if (!wantChar(fp, '\'')) {
                panic("Illegal char!");
                return ERROR_TOKEN;
            }
            tokenText[0] = '\'';
            tokenText[1] = '\\';
            tokenText[2] = c;
            tokenText[3] = '\'';
            tokenText[4] = '\0';
            tokenLen = 4;
        } else {
            c = fgetc(fp);
            if (!isascii(c)) {
                panic("Illegal char (not ascii)!");
                return ERROR_TOKEN;
            }
            if (!wantChar(fp, '\'')) {
                panic("Illegal char!");
                return ERROR_TOKEN;
            }
            tokenText[0] = '\'';
            tokenText[1] = c;
            tokenText[2] = '\'';
            tokenText[3] = '\0';
            tokenLen = 3;
        }
        return CHAR_CONST;
    }
    if (!isalpha(c) && !isdigit(c) && c != '_' && c != '.') {
        printf("%c %d\n", c, c);
        panic("Illegal character!");
        return ERROR_TOKEN;
    }

    if (isalpha(c) || c == '_') {
        tokenLen = 1;
        tokenText[0] = c;
        while ((c = fgetc(fp))) {
            if (!isalpha(c) && !isdigit(c) && c != '_') {
                ungetc(c, fp);
                break;
            }
            tokenText[tokenLen++] = c;
        }
        tokenText[tokenLen] = '\0';
        if (strcmp(tokenText, "int") == 0) {
            return INT;
        } else if (strcmp(tokenText, "float") == 0) {
            return FLOAT;
        } else if (strcmp(tokenText, "char") == 0) {
            return CHAR;
        } else if (strcmp(tokenText, "if") == 0) {
            return IF;
        } else if (strcmp(tokenText, "else") == 0) {
            return ELSE;
        } else if (strcmp(tokenText, "for") == 0) {
            return FOR;
        } else if (strcmp(tokenText, "while") == 0) {
            return WHILE;
        } else if (strcmp(tokenText, "return") == 0) {
            return RETURN;
        } else if (strcmp(tokenText, "continue") == 0) {
            return CONTINUE;
        } else if (strcmp(tokenText, "extern") == 0) {
            return EXTERN;
        } else if (strcmp(tokenText, "static") == 0) {
            return STATIC;
        } else if (strcmp(tokenText, "break") == 0) {
            return BREAK;
        } else if (strcmp(tokenText, "include") == 0) {
            return INCLUDE;
        } else if (strcmp(tokenText, "define") == 0) {
            return DEFINE;
        } else {
            return IDENT;
        }
    } else {
        tokenLen = 1;
        tokenText[0] = c;
        TokenKind ret = c == '.' ? FLOAT_CONST : INT_CONST;
        while ((c = fgetc(fp))) {
            if (isdigit(c)) {
                tokenText[tokenLen++] = c;
            } else if (c == '.') {
                if (ret == FLOAT_CONST) {
                    panic("Illegal float!");
                    return ERROR_TOKEN;
                }
                if (tokenLen > 1 && tokenText[1] == '0') {
                    panic("Hex number cannot be float!");
                    return ERROR_TOKEN;
                }
                ret = FLOAT_CONST;
                tokenText[tokenLen++] = c;
            } else if (c == 'x') {
                if (tokenLen != 1 || tokenText[0] != '0') {
                    panic("Confusing x!");
                    return ERROR_TOKEN;
                }
                tokenText[tokenLen++] = c;
            } else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')){
                if (tokenLen <= 1 || tokenText[0] != '0') {
                    panic("Confusing alpha after number!");
                    return ERROR_TOKEN;
                }
                tokenText[tokenLen++] = c;
            } else {
                ungetc(c, fp);
                break;
            }
        }
        tokenText[tokenLen] = '\0';
        if (tokenLen == 1 && tokenText[0] == '.') {
            panic("Get . alone");
            return ERROR_TOKEN;
        }
        return ret;
    }
}

TokenKind getToken(FILE *fp) {
    TokenKind kind = gettoken(fp);
#ifdef SHOW_TOKEN
    printf("%-30s%s\n", getTokenKindType(kind), getTokenKindStr(kind));
#endif
    return kind;
}


void unGetTokenKindStr(FILE *fp, const char * s) {
    ungetc(' ', fp);
    int len = strlen(s);
    for (int i = len - 1; i >= 0; i--) {
        ungetc(s[i], fp);
    }
}

void unGetToken(FILE *fp, TokenKind kind) {
#ifdef SHOW_TOKEN
    printf("UNGET %-24s%s\n", getTokenKindType(kind), getTokenKindStr(kind));
#endif
    switch (kind) {
        case ERROR_TOKEN:
        case ENDOFFILE:
            ;
        case IDENT:
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
        case STRING:
        case LINE_COMMENT:
        case BLOCK_COMMENT:
            unGetTokenKindStr(fp, tokenText);
            break;
        default:
            unGetTokenKindStr(fp, getTokenKindStr(kind));
            break;
    }
}

const char * getTokenKindStr(TokenKind kind) {
    switch (kind) {
        case ERROR_TOKEN:
            return "ERROR";
        case ENDOFFILE:
            return "";
        case IDENT:
        case INT_CONST:
        case FLOAT_CONST:
        case CHAR_CONST:
        case STRING:
        case LINE_COMMENT:
        case BLOCK_COMMENT:
            return tokenText;
        case INT:
            return "int";
        case FLOAT:
            return "float";
        case CHAR:
            return "char";
        case IF:
            return "if";
        case ELSE:
            return "else";
        case EQUAL:
            return "==";
        case LESS:
            return "<";
        case LEQ:
            return "<=";
        case GREAT:
            return ">";
        case GEQ:
            return ">=";
        case NEQ:
            return "!=";
        case ASSIGN:
            return "=";
        case LP:
            return "(";
        case RP:
            return ")";
        case SEMI:
            return ";";
        case COMMA:
            return ",";
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case TIME:
            return "*";
        case DIVIDE:
            return "/";
        case MOD:
            return "%";
        case LOGIC_AND:
            return "&&";
        case LOGIC_OR:
            return "||";
        case FOR:
            return "for";
        case WHILE:
            return "while";
        case RETURN:
            return "return";
        case CONTINUE:
            return "continue";
        case EXTERN:
            return "extern";
        case STATIC:
            return "static";
        case BREAK:
            return "break";
        case SHARP:
            return "#";
        case INCLUDE:
            return "include";
        case DEFINE:
            return "define";
        case LBRACKET:
            return "[";
        case RBRACKET:
            return "]";
        case LBRACE:
            return "{";
        case RBRACE:
            return "}";
        default:
            return "";
    }
}

const char * getTokenKindType(TokenKind kind) {
    switch (kind) {
        case ERROR_TOKEN:
            return "error_token";
        case ENDOFFILE:
            return "eof";
        case IDENT:
            return "identifier";
        case INT_CONST:
            return "int value";
        case FLOAT_CONST:
            return "float value";
        case CHAR_CONST:
            return "char value";
        case STRING:
            return "string";
        case LINE_COMMENT:
            return "line comment";
        case BLOCK_COMMENT:
            return "block comment";
        case INT:
            return "int type";
        case FLOAT:
            return "float type";
        case CHAR:
            return "char type";
        case IF:
            return "if keyword";
        case ELSE:
            return "else keyword";
        case EQUAL:
            return "equal";
        case LESS:
            return "less";
        case LEQ:
            return "less or equal";
        case GREAT:
            return "great";
        case GEQ:
            return "great or equal";
        case NEQ:
            return "not equal";
        case ASSIGN:
            return "assign";
        case LP:
            return "left parenthesis";
        case RP:
            return "right parenthesis";
        case SEMI:
            return "semicolon";
        case COMMA:
            return "comma";
        case PLUS:
            return "plus";
        case MINUS:
            return "minus";
        case TIME:
            return "times";
        case DIVIDE:
            return "division";
        case MOD:
            return "mod";
        case LOGIC_AND:
            return "and";
        case LOGIC_OR:
            return "or";
        case FOR:
            return "for keyword";
        case WHILE:
            return "while keyword";
        case RETURN:
            return "return keyword";
        case CONTINUE:
            return "continue keyword";
        case EXTERN:
            return "extern keyword";
        case STATIC:
            return "static keyword";
        case BREAK:
            return "break keyword";
        case SHARP:
            return "sharp";
        case INCLUDE:
            return "include keyword";
        case DEFINE:
            return "define keyword";
        case LBRACKET:
            return "left bracket";
        case RBRACKET:
            return "right bracket";
        case LBRACE:
            return "left brace";
        case RBRACE:
            return "right brace";
        default:
            return "";
    }
}