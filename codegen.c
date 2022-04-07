#include "ycc.h"

int labseq=0;

void gen_lval(Node *node) {
	if (node->kind != ND_LVAR)
		error("left value is not a var");
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");
}
void gen(Node *node) {
	switch (node->kind) {
		case ND_FOR:{
			int seq = labseq++;
			gen(node->init);
			printf(".Lbegin%d:\n", seq);
			gen(node->cond);
			printf("	pop rax\n");
			printf(" 	cmp rax, 0\n");
			printf("	je  .Lend%d\n", seq);
			gen(node->then);
			gen(node->cntr);
			printf("	jmp  .Lbegin%d\n", seq);
			printf(".Lend%d:\n", seq);
		}
		case ND_WHILE:{
			int seq = labseq++;
			printf(".Lbegin%d:\n", seq);
			gen(node->cond);
			printf("	pop rax\n");
			printf(" 	cmp rax, 0\n");
			printf("	je  .Lend%d\n", seq);
			gen(node->then);
			printf("	jmp  .Lbegin%d\n", seq);
			printf(".Lend%d:\n", seq);
		}
		case ND_IF: {
			int seq = labseq++;
			if (node->els) {
				gen(node->cond);
				printf("	pop rax\n");
				printf(" 	cmp rax, 0\n");
				printf(" 	je  .Lelse%d\n", seq);
				gen(node->then);
				printf(" 	jmp .Lend%d\n", seq);
				printf(".Lelse%d:\n", seq);
				gen(node->els);
				printf(".Lend%d:\n", seq);
			} 
			else {
				gen(node->cond);
				printf("	pop rax\n");
				printf(" 	cmp rax, 0\n");
				printf("	je  .Lend%d\n", seq);
				gen(node->then);
				printf(".Lend%d:\n", seq);
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
	 		printf("	pop rax\n");
			printf("	mov rsp, rbp\n");
			printf("	pop rbp\n");
			printf("	ret\n");
			return;
		case ND_NUM:
			printf("	push %d\n", node->val);
			return;
		case ND_LVAR:
			gen_lval(node);
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			return;
		case ND_ASSIGN:
			gen_lval(node->lhs);
			gen(node->rhs);
			printf("	pop rdi\n");
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
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
		case ND_EQ:
			printf("	cmp rax, rdi\n");
			printf("	sete al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_NE:
			printf("	cmp rax, rdi\n");
			printf("	setne al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LT:
			printf("	cmp rax, rdi\n");
			printf("	setl al\n");
			printf("	movzb rax, al\n");
			break;
		case ND_LE:
			printf("	cmp rax, rdi\n");
			printf("	setle al\n");
			printf("	movzb rax, al\n");
			break;
	}
	printf("	push rax\n");
}