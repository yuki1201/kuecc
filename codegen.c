#include "ycc.h"

int labseq=0;

void gen_lval(Node *node) {
	if (node->kind != ND_LVAR)
		error("left value is not a var");
	printf("LD ACC 300H\n");
	printf("SUB ACC, %d\n", node->offset);
	printf("PSH ACC\n");
}
void gen(Node *node) {
	switch (node->kind) {
		case ND_FOR:{
			int seq = labseq++;
			gen(node->init);
			printf("LLbegin%d:\n", seq);
			gen(node->cond);
			printf("POP ACC\n");
			printf("CMP ACC, 0\n");
			printf("BZ  LLend%d\n", seq);
			gen(node->then);
			gen(node->cntr);
			printf("BA  LLbegin%d\n", seq);
			printf("LLend%d:\n", seq);
		}
		case ND_WHILE:{
			int seq = labseq++;
			printf("LLbegin%d:\n", seq);
			gen(node->cond);
			printf("POP ACC\n");
			printf("CMP ACC, 0\n");
			printf("BZ  LLend%d\n", seq);
			gen(node->then);
			printf("BA  LLbegin%d\n", seq);
			printf("LLend%d:\n", seq);
		}
		case ND_IF: {
			int seq = labseq++;
			if (node->els) {
				gen(node->cond);
				printf("POP ACC\n");
				printf("CMP ACC, 0\n");
				printf("BZ  LLelse%d\n", seq);
				gen(node->then);
				printf("BA LLend%d\n", seq);
				printf("LLelse%d:\n", seq);
				gen(node->els);
				printf("LLend%d:\n", seq);
			} 
			else {
				gen(node->cond);
				printf("POP ACC\n");
				printf("CMP ACC, 0\n");
				printf("BZ  LLend%d\n", seq);
				gen(node->then);
				printf("LLend%d:\n", seq);
			}
			return;
		}
		case ND_BLOCK:
			for(Node *n=node->body;n;n=n->next){
				gen(n);
			}
			return;
		case ND_RETURN:
			gen(node->lhs);
	 		printf("POP ACC\n");
			printf("LD SP, 300H\n");
			printf("POP ACC\n");
			printf("OUT\n");
			printf("RET\n");
			return;
		case ND_NUM:
			printf("LD ACC %d\n", node->val);
			printf("PSH ACC\n");
			return;
		case ND_LVAR:
			gen_lval(node);
			printf("POP IX\n");
			printf("LD ACC, [IX]\n");
			printf("PSH ACC\n");
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs);
			gen(node->rhs);
			printf("POP ACC\n");
			printf("POP IX\n");
			printf("ST ACC [IX]\n");
			printf("PSH ACC\n");
			return;
		case ND_GOTO:
			printf("BA LLgoto%d\n",node->str[0]);
			return;
		case ND_LABEL:
			printf("LLgoto%d:\n",node->str[0]);
			return;
}

	gen(node->lhs);
	gen(node->rhs);
	printf("POP IX\n");
	printf("POP ACC\n");
	int seq = labseq++;
	switch (node->kind) {
		case ND_ADD:
			printf("ADD ACC, IX\n");
			break;
		case ND_SUB:
			printf("SUB ACC, IX\n");
			break;
		case ND_MUL:
			printf("imul ACC, IX\n");
			break;
		case ND_DIV:
			printf("cqo\n");
			printf("idiv IX\n");
			break;
		case ND_EQ:
			
			printf("CMP ACC, IX\n");
			printf("BZ ND_EQ_JP1%d\n",seq);
			printf("LD ACC 0\n");
			printf("BA ND_EQ_JP2%d\n",seq);
			printf("ND_EQ_JP1%d:\n",seq);
			printf("LD ACC 1\n");
			printf("ND_EQ_JP2%d:\n",seq);
			break;
		case ND_NE:
			printf("CMP ACC, IX\n");
			printf("setne al\n");
			printf("LDzb ACC, al\n");
			break;
		case ND_LT:
			printf("CMP ACC, IX\n");
			printf("setl al\n");
			printf("LDzb ACC, al\n");
			break;
		case ND_LE:
			printf("CMP ACC, IX\n");
			printf("setle al\n");
			printf("LDzb ACC, al\n");
			break;
	}
	printf("PSH ACC\n");
}