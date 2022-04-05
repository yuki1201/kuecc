#include "ycc.h"

char *user_input;
Token *token;

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

bool consume(char *op){
	if(memcmp(token->str,op,strlen(op))==0){
		//printf("%s:%s",op,token->str);
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

Token *new_token(TokenKind kind, Token *cur, char *str,int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len=len;
	cur->next = tok;
	return tok;
}

bool headstrcmp(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
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
		if(headstrcmp(p,"<=")||headstrcmp(p,">=")||headstrcmp(p,"==")||headstrcmp(p,"!=")){
			cur = new_token(TK_RESERVED, cur, p++,2);
			p++;
			continue;
		}
		if(*p == '+' || *p == '-'||*p == '*' || *p == '/'||*p == '(' || *p == ')'||*p == '>' || *p == '<') {
			cur = new_token(TK_RESERVED, cur, p++,1);
			continue;
		}
		if(isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0);
			cur->val = strtol(p, &p, 10);
			continue;
		}
		if ('a' <= *p && *p <= 'z') {
			cur = new_token(TK_IDENT, cur, p++,1);
			cur->len = 1;
			continue;
		}

		error("can not tokenize");
	}
	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
	Node *node = equality();
}

Node *equality(){
	Node *node = relational();
	while(true) {
		if (consume("==")){
			node = new_node(ND_EQ, node, relational());
		}
		else if (consume("!=")){
			node = new_node(ND_NE, node, relational());
		}
		else{
			return node;
		}
	}
}

Node *relational(){
	Node *node = add();
	while(true) {
		if (consume("<")){
			node = new_node(ND_LT, node, add());
		}
		else if (consume("<=")){
			node = new_node(ND_LE, node, add());
		}
		else if (consume(">")){
			node = new_node(ND_LT, add(), node);
		}
		else if (consume(">=")){
			node = new_node(ND_LE, add(), node);
		}
		else{
			return node;
		}
	}
}

Node *add(){
	Node *node = mul();
	while(true) {
		if (consume("+")){
			node = new_node(ND_ADD, node, mul());
		}
		else if (consume("-")){
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
		if (consume("*")){
			node = new_node(ND_MUL, node, unary());
		}
		else if (consume("/")){
			node = new_node(ND_DIV, node, unary());
		}
		else{
			return node;
		}
	}
}

Node *unary() {
	if (consume("+"))
		return primary();
	if (consume("-"))
		return new_node(ND_SUB, new_node_num(0), primary());
	return primary();
}

Node *primary() {
	// 次のトークンが"("なら、"(" expr ")"のはず
	if (consume("(")) {
		Node *node = expr();
		consume(")");
		return node;
	}
	// そうでなければ数値のはず
	return new_node_num(expect_number());
}