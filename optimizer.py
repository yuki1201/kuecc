# coding: utf-8

file1 = open("./tmp.o",'r',encoding='utf-8')
f1 = file1.readlines()
for i in reversed(range(len(f1))):
	if f1[i]=="PSH ACC\n" and f1[i+1]=="POP ACC\n":
		f1.pop(i)
		f1.pop(i)

file1 = open("./tmp.o",'w',encoding='utf-8')
for i in f1:
	file1.write(i)