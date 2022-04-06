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

Token *consume_tk_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *t = token;
  token = token->next;
  return t;
}

void expect(char *op){
	if(memcmp(token->str,op,strlen(op))!=0){
		error_at(token->str,"Not a Expected Sign");
	}
	else{
		token = token->next;
	}
}

int expect_number(){
	if(token->kind!=TK_NUM){
		error_at(token->str, "Not a Number");
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
	//printf("%s:%s\n",p,q);
  return memcmp(p, q, strlen(q)) == 0;
}

int is_alnum(char c) {
	return ('a' <= c && c <= 'z') ||
	('A' <= c && c <= 'Z') ||
	('0' <= c && c <= '9') ||
	(c == '_');
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
		if(headstrcmp(p,"return")){
			cur = new_token(TK_RETURN, cur ,p++,6);
			p+=5;
			continue;
		}
		if(headstrcmp(p,"<=")||headstrcmp(p,">=")||headstrcmp(p,"==")||headstrcmp(p,"!=")){
			cur = new_token(TK_RESERVED, cur, p++,2);
			p++;
			continue;
		}
		if(*p == '+' || *p == '-'||*p == '*' || *p == '/'||*p == '(' || *p == ')'||*p == '>' || *p == '<'||*p == '=' || *p == ';') {
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
			continue;
		}

		error("can not tokenize");
	}
	new_token(TK_EOF, cur, p, 0);
	return head.next;
}

Node *code[100];
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void program(){
	int i=0;
	while(!at_eof()){
		code[i++]=stmt();
	}
	code[i]=NULL;
}

Node *stmt(){
	Node *node;
	if(consume("return")){
		node=calloc(1,sizeof(Node));
		node->kind=ND_RETURN;
		node->lhs=expr();//error gen
	}
	else{
		node=expr();
	}
	expect(";");
	return node;
	
}

Node *expr() {
	Node *node = assign();
}

Node *assign(){
	Node *node = equality();
	
	if(consume("=")){
		node=new_node(ND_ASSIGN,node,assign());
	}
	return node;
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
	//Token *tok = consume_ident();
	Token *tok = consume_tk_ident();
	if (tok) {
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LVAR;
		node->offset = (tok->str[0] - 'a' + 1) * 8;
		return node;
	}
	// そうでなければ数値のはず

	return new_node_num(expect_number());
}