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
	// アセンブリの前半部分を出力
	// プロローグ
	printf("LD SP 500H\n");
	// 変数26個分の領域を確保する
	// 先頭の式から順にコード生成
	for (int i = 0; code[i]; i++) {
		gen(code[i]);
		// 式の評価結果としてスタックに一つの値が残っている
		// はずなので、スタックが溢れないようにポップしておく
	}
	// エピローグ
	// 最後の式の結果がRAXに残っているのでそれが返り値になる
	printf("	HLT\n");
	return 0;
}
