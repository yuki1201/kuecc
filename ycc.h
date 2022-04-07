#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
	TK_RESERVED,//演算子
	TK_IDENT,//識別子
	TK_NUM,//数字
	TK_EOF,//EOF
	TK_RETURN,//return
	TK_IF,//if
	TK_WHILE,//while
	TK_FOR,//for
	TK_GOTO,//goto
	TK_LABEL,//label
}TokenKind;

extern char *user_input;
typedef struct Token Token;

struct Token{
	TokenKind kind;//型
	Token *next;//次tokenのポインタ
	int val;//数値
	char *str;//token文字列
	int len;//長さ
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
//int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

extern char *user_input;
extern Token *token;


typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_EQ,//==
	ND_NE,//!=
	ND_LT,//<
	ND_LE,//<=
	ND_ASSIGN,//代入
	ND_LVAR,//ローカル変数
	ND_NUM, // 整数
	ND_RETURN,//return
	ND_IF,//if
	ND_WHILE,//while
	ND_FOR,//for
	ND_BLOCK,//{}
	ND_GOTO,//goto
	ND_LABEL,//label
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs; // 左辺
	Node *rhs; // 右辺
	int val; // kindがND_NUMの場合のみ使う
	char *str;//label
	int offset;
	Node *cond;//条件
	Node *then;//if true
	Node *els;//if else
	Node *init;
	Node *cntr;
	Node *next;
	Node *body;
};

extern Node *code[100];
Token *token;//global宣言

Node *expr();
void program();

extern int labseq;

void codegen(Node *node);
void gen(Node *node);