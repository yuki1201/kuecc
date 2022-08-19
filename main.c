#include "ycc.h"

int main(int argc, char **argv){
	if(strcmp(argv[1],"-f")==0){
		 FILE *fp;
		 char pgf[100];
		 fp=fopen(argv[2],"r");
		 if(fp==NULL){
		 	return 1;
		 }
		 else{
		 	int lenofpgf=fread(pgf,1,99,fp);
		 	pgf[lenofpgf-1]='\0';
		 	argv[1]=pgf;
		 }
	}
	else if (argc!=2){
		fprintf(stderr,"arguments number error");
		return 1;
	}
	token=tokenize(argv[1]);
	user_input=argv[1];
	program();
	printf("LD SP 1500H\n");
	for (int i = 0; code[i]; i++) {
		gen(code[i]);
	}
	printf("HLT\n");
	return 0;
}
