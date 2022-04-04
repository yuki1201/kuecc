#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv){
	if (argc!=2){
		fprintf(stderr,"arguments number error");
		return 1;
	}
	char *p = argv[1];

	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	printf("	mov rax, %ld\n",strtol(p,&p,10));

	while(*p){
		if (*p!='+'&&*p!='-'){
			fprintf(stderr,"unexpexted character:'%c'\n",*p);
			return 1;
		}
		if(*p=='+'){
			printf("	add ");
		}
		if(*p=='-'){
			printf("	sub ");
		}
		p++;
		printf("rax, %ld\n", strtol(p,&p,10));
	}
	printf("	ret\n");
	return 0;
}
