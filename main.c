#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//エラー関数
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//トークンの種類
typedef enum {
    TK_RESERVED,    //記号
    TK_NUM,         //数字
    TK_EOF,         //入力の終わり
} TokenKind;

//トークン
typedef struct token {
    TokenKind kind;     //トークンの種類
    struct token *next; //次のトークン
    int val;            //トークンの種類が数字の場合、その数字
    char *str;          //トークン文字列
} Token;

//新しいトークンの生成
Token *new_token(TokenKind kind, Token *cur, char *str) {
    Token *tok = calloc(1, sizeof(Token));
    cur->next = tok;
    tok->kind = kind;
    tok->str = str;
    return tok;
}

//トークナイス
Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    // printf("original:%s\n", p);

    while(*p) {
        //空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }
        //数字の場合、新しい数字トークンを作る
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            // printf("NUM:->str = %s\n", cur->str);
            continue;
        }
        //記号の場合、新しい記号トークンを作る
        if (*p == '+' || *p == '-') {
            cur = new_token(TK_RESERVED, cur, p);
            p++;
            // printf("RESERVED:->str = %s\n", cur->str);
            continue;
        }
        error("トークナイズ出来ない文字が含まれます");
    }
    //終わりのトークン
    new_token(TK_EOF, cur, p);

    return head.next;
}

//今注目しているトークン
Token *token;

//トークンのTokenKindを読み込みTK_NUMだったら、その数字をかえしてトークンを次へ
//それ以外はえらー
int expect_number() {
    if (token->kind != TK_NUM) {
        error("数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

//指定の文字を消費してトークンを一つすすめる
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}
//指定の文字だったらトークンを一つすすめる
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        error("%cではありません");
    }
    token = token->next;
}

//終わりの判定
bool at_eof() {
    return token->kind == TK_EOF;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    char *p = argv[1];

    token= tokenize(p);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", expect_number());

    while(!at_eof()){
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }
    printf("    ret\n");
    return 0;
}