#include "ycc.h"

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
		if(headstrcmp(p,"return")&&!is_alnum(p[6])){
			cur = new_token(TK_RETURN, cur ,p,6);
			p+=6;
			continue;
		}
		if(headstrcmp(p,"if")&&!is_alnum(p[2])){
			cur = new_token(TK_IF, cur, p, 2);
			p+=2;
			continue;
		}
		if(headstrcmp(p,"else")&&!is_alnum(p[4])){
			cur = new_token(TK_RESERVED, cur, p, 4);
			p+=4;
			continue;
		}
		if(headstrcmp(p,"while")&&!is_alnum(p[5])){
			cur = new_token(TK_WHILE, cur, p, 5);
			p+=5;
			continue;
		}
		if(headstrcmp(p,"for")&&!is_alnum(p[3])){
			cur = new_token(TK_FOR, cur, p, 3);
			p+=3;
			continue;
		}
		if(headstrcmp(p,"goto")&&!is_alnum(p[4])){
			cur = new_token(TK_FOR, cur, p, 4);
			p+=4;
			continue;
		}
		if(headstrcmp(p,"<=")||headstrcmp(p,">=")||headstrcmp(p,"==")||headstrcmp(p,"!=")){
			cur = new_token(TK_RESERVED, cur, p,2);
			p+=2;
			continue;
		}
		if(*p == '+' || *p == '-'||*p == '*' || *p == '/'||*p == '(' || *p == ')'||*p == '>' || *p == '<'||*p == '=' || *p == ';'|| *p == '{'|| *p == '}'|| *p == ':') {
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
		
		if ('A' <= *p && *p <= 'Z') {
			cur = new_token(TK_LABEL, cur, p++,1);
			continue;
		}

		printf("%s\n",p);
		error("can not tokenize");
	}
	new_token(TK_EOF, cur, p, 0);
	return head.next;
}