#include<iostream>
 //#include "pl0.h"
#include "parse.h"
 
char *name="E:\\My_project\\Lesson\\compiler_PTT\\lab\\PL0_code\\PL0_code.in";
int main()
{
/* */
//���������ֱ�reserve(�ñ�ͬ����ߵ��ʱ����)�����б����Ӧ���ʱ�kw 
	create_keytable();
	//��ȡPL0Դ�����ļ�	 
	string t=fr(name);
	cout<<t<<endl;
	//	 t=preprocessing(t);//��Դ������Ԥ���� 	
	//	show_word_coding_table();//��Ļ����������� 
//�ʷ��������򣬲���������������head 
	lexer(t); 
	//���������������head 
	show(head);
//�﷨�������ݹ��ӳ��� 
	P(); 
//��������﷨����
//	postorder(root);
	levelorder(root);

//	���table�� 
	showtab(table);		
	return 0;
}

