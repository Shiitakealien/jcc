#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// kind of token
typedef enum {
    TK_RESERVED,    // reserved
    TK_NUM,         // numebr token
    TK_EOF,         // token reprsenting end of file
} TokenKind;

typedef struct Token Token;

// token type
struct Token {
    TokenKind kind; // token type
    Token *next;    // next token
    int val;        // if kind is TK_NUM, its value
    char *str;      // token string
};

// current token
Token *token;

// function to report error
// same argument type as printf
void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// if next token is expected one, then progress one token and return true
// otherwise, return false
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// if next token is expected one, then progress one token
// otherwise, report error
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error("unexpected token, expected:'%c'", op);
    token = token->next;
}

// if next token is number, then progress one token and retrun its value
// otherwise, report error
int expect_number() {
    if (token->kind != TK_NUM)
        error("not number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// create a new token and connect it to cur
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// tokenize input string p and return gotten token
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // skip space
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur-> val = strtol(p, &p, 10);
            continue;
        }

        error("cannot tokenize");
    }

    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "invalid number of arguments\n");
        return 1;
    }

    // tokenize
    token = tokenize(argv[1]);

    // print header of assembly
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    // check if the start of statements is a number, and 
    // print the first mov instruction
    printf("  mov rax, %d\n", expect_number());

    // consume sequence of tokens `+ <num>` or `- <num>`, and
    // print assembly 
    while (!at_eof()) {
        if (consume('+')) {
            printf("  add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("  sub rax, %d\n", expect_number());
    }

    printf("  ret\n");
    return 0;
}
