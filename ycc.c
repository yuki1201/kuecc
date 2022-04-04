#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
	TK_RESERVED,//演算子
	TK_NUM,//数字
	TK_EOF,//EOF
}TokenKind;

typedef struct Token Token;

struct Token{
	TokenKind kind;//型
	Token *next;//次tokenのポインタ
	int val;//数値
	char *str;//token文字列
};

typedef enum {
	ND_ADD, // +
	ND_SUB, // -
	ND_MUL, // *
	ND_DIV, // /
	ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs; // 左辺
	Node *rhs; // 右辺
	int val; // kindがND_NUMの場合のみ使う
};

Token *token;//global宣言
char *user_input;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

void error_at(char *loc, char *fmt,...){
	va_list ap;
	va_start(ap,fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos," ");
	fprintf(stderr, "^ ");
	vfprintf(stderr,fmt,ap);
	fprintf(stderr, "\n");
	exit(1);
}

void error(char *fmt,...){
	va_list ap;
	va_start(ap,fmt);
	vfprintf(stderr,fmt,ap);
	fprintf(stderr, "\n");
	exit(1);
}

bool consume(char op){
	if(token->str[0]==op){
		token=token->next;
		return true;
	}
	else{
		return false;
	}
}

int expect_number(){
	if(token->kind!=TK_NUM){
		error_at(token->str, "NaN");
	}
	int val=token->val;
	token=token->next;
	return val;
}

bool at_eof(){
	return token->kind==TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

Token *tokenize(char *p) {
	Token head;
	head.next = NULL;
	Token *cur = &head;
	while (*p) {
		
		if (isspace(*p)) {// 空白文字をスキップ
			p++;
			continue;
		}
		if (*p == '+' || *p == '-'||*p == '*' || *p == '/'||*p == '(' || *p == ')') {
			cur = new_token(TK_RESERVED, cur, p++);
			continue;
		}
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}
		error("can not tokenize");
	}
	new_token(TK_EOF, cur, p);
	return head.next;
}

Node *expr();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
	Node *node = mul();
	while(true) {
		if (consume('+')){
			node = new_node(ND_ADD, node, mul());
		}
		else if (consume('-')){
			node = new_node(ND_SUB, node, mul());
		}
		else{
			return node;
		}
	}
}

Node *mul() {
	Node *node = unary();
	while(true) {
		if (consume('*')){
			node = new_node(ND_MUL, node, unary());
		}
		else if (consume('/')){
			node = new_node(ND_DIV, node, unary());
		}
		else{
			return node;
		}
	}
}

Node *unary() {
	if (consume('+'))
		return primary();
	if (consume('-'))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}


Node *primary() {
	// 次のトークンが"("なら、"(" expr ")"のはず
	if (consume('(')) {
		Node *node = expr();
		consume(')');
		return node;
	}
	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

void gen(Node *node) {
	if (node->kind == ND_NUM) {
		printf(" push %d\n", node->val);
		return;
	}
	gen(node->lhs);
	gen(node->rhs);
	printf("	pop rdi\n");
	printf("	pop rax\n");
	switch (node->kind) {
		case ND_ADD:
			printf("	add rax, rdi\n");
			break;
		case ND_SUB:
			printf("	sub rax, rdi\n");
			break;
		case ND_MUL:
			printf("	imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
	}
	printf(" push rax\n");
}

int main(int argc, char **argv){
	if (argc!=2){
		fprintf(stderr,"arguments number error");
		return 1;
	}
	token=tokenize(argv[1]);
	user_input=argv[1];
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	//printf("	mov rax, %d\n",expect_number());

	gen(node);
	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
