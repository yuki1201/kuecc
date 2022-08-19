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

Token *consume_tk_label() {
  if (token->kind != TK_LABEL)
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
	if(consume("if")){
		node=calloc(1,sizeof(Node));
		node->kind=ND_IF;
		expect("(");
		node->cond=expr();
		expect(")");
		node->then=stmt();
		if(consume("else")){
			node->els=stmt();
		}
		return node;//if文に;は不要
	}
	if(consume("while")){
		node=calloc(1,sizeof(Node));
		node->kind=ND_WHILE;
		expect("(");
		node->cond=expr();
		expect(")");
		node->then=stmt();
		return node;
	}
	if(consume("for")){
		node=calloc(1,sizeof(Node));
		node->kind=ND_FOR;
		expect("(");
		node->init=expr();
		expect(";");
		node->cond=expr();
		expect(";");
		node->cntr=expr();
		expect(")");
		node->then=stmt();
		return node;
	}
	if(consume("{")){
		Node head={};
		//head->next=NULL;
		Node *cur=&head;
		while(!consume("}")){
			cur->next = stmt();
			cur=cur->next;
		}
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_BLOCK;
		node->body=head.next;
		return node;
	}
	
	if(consume("goto")){
		node=calloc(1,sizeof(Node));
		node->kind=ND_GOTO;
		node->str=token->str;
		token=token->next;
		expect(";");
		return node;
	}
	Token *tok = consume_tk_label();
	if (tok) {
		Node *node = calloc(1, sizeof(Node));
		node->kind = ND_LABEL;
		node->str=tok->str;
		consume(":");
		return node;
	}
	if(consume("return")){
		node=calloc(1,sizeof(Node));
		node->kind=ND_RETURN;
		node->lhs=expr();
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
	if (consume("++")){
		return new_node(ND_ASSIGN,node,new_node(ND_ADD, node, new_node_num(1)));
		expect(";");
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
		if (consume("<=")){
			node = new_node(ND_LE, node, add());
		}
		else if (consume("<")){
			node = new_node(ND_LT, node, add());
		}
		else if (consume(">=")){
			node = new_node(ND_LE, add(), node);
		}
		else if (consume(">")){
			node = new_node(ND_LT, add(), node);
		}
		else{
			return node;
		}
	}
}

Node *add(){
	Node *node = mul();
	while(true) {
		if (memcmp(token->str,"++",2)!=0&&consume("+")){
			node = new_node(ND_ADD, node, mul());
		}
		else if (memcmp(token->str,"--",2)!=0&&consume("-")){
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
	if (memcmp(token->str,"++",2)!=0&&consume("+"))
		return primary();
	if (memcmp(token->str,"--",2)!=0&&consume("-"))
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
		node->offset = (tok->str[0] - 'a' + 1) * 2;
		return node;
	}
	
	// そうでなければ数値のはず
	return new_node_num(expect_number());
}