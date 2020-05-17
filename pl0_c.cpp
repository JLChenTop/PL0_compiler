#include<iostream>
 //#include "pl0.h"
#include "parse.h"
 
char *name=
"E:\\My_project\\Lesson\\compiler_PTT\\lab\\PL0_code\\PL0_code4.in";
int main()
{
/* */
//创建保留字表reserve(该表同样兼具单词编码表)，还有编码对应单词表kw 
	create_keytable(); 
	//读取PL0源程序文件	 
	string t=fr(name);
//	cout<<t<<endl;
	//	 t=preprocessing(t);//对源程序做预处理 	
	//	show_word_coding_table();//屏幕输出单词类别表 和操作码表 
//词法分析程序，产生单词序列链表，head 
	lexer(t); 
	//输出单词序列链表head 
	show(head);
//语法分析，递归子程序法 
	P(); 
//后序遍历语法树：
//	postorder(root);
//	levelorder(root);

//	输出table表 
//	showtab(table);	
//	输出中间代码 
	showass();
	return 0;
}

