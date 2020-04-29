#include<iostream>
#include<string> 
#include<string.h>
#include<cstring> 
#include<map> 
#include<algorithm>
#include<stdio.h>
#include<iomanip>
#include<queue>
#include<stack> 
#define ll long long
using namespace std;

#define kwsize 28
#define identifier 29 
#define constant 30 
//ȫ�ֱ��� 
const int maxnum=1000;//�����г��ֵ����ͱ����������� 
string 
	keyword[]={
	//�ؼ���13
	"const","var","procedure","begin","end",	"odd","if","then","call","while","do","read","write",
	//����� 11
	"=",":=","+","-","*","/","#","<","<=",">",">=",
	//�ֽ�� 4
	",",";","(",")"
	//  2 
//	"identifier","constant"
	};
	//������
	//��ʶ��    <��ʶ��> �� <��ĸ>{<��ĸ>|<����>}
	/*
��ĸ�������ִ�Сд����isalpha();��д��ĸ��isupper();
Сд��ĸ��islower();���֣�isdigit();��ĸ�����֣�isalnum(); 
	*/ 
ll len;
int i,vnum,//��ʶ����Ŀ
	level; //��ǰ���������Ĳ�� 
//�����ֱ� 
map<string,int> reserve; 
map<int,string> kw;//�����ֱ����Ӧ�ĵ��� ���� 


//Դ���򾭴ʷ�����������ĵ������� 
struct seq{
	int k;//�����ֱ�
	string v;//��������ֵ 
	seq* next;
	int sn;
	seq(){k=-1;v="";next=NULL;} 
}*head,*tail;
int snn;
//��������ֵ��num ,����һ�����Է��ó����ĵ�ַ 
int num[maxnum],ci;

//Ҫ����һ��table���洢Դ�����г��ֵı�ʶ��
// ����������������������������
//table�ĸ�ʽ��name; kind; val(����);level;ADR(����);
//�����ADR�Ǵ洢����ʵ��ֵ��ָ�룬��num�е�ƫ���� 
struct tableitem{//table���� 
	string name;//�������� 
	int k;  //������� 
	int level;//���Ĳ�� 
	int val; //����ֵ ��ֻ�����洢һ��������ֵ 
//ͻȻ�뵽�����в�εĹ����ж������Ӧ���ǲ��ܹ�����ʵ� 
	int ADR; //����ֵ�ĵ�ַ ���洢���� 
	tableitem* next;
	tableitem(){
		next=NULL;
	}
	tableitem(string NAME,int kind,int l,int value=-1,int address=-1){
		name=NAME;k=kind;level=l;val=value;ADR=address;
	}
}; 
tableitem* mkitem(string NAME,int kind,int l,int value=-1,int address=-1){
	tableitem* the=new tableitem(NAME,kind,l,value,address);
	return the;
}
class symtab{ //table�� 
public:
	string name; // ���� ��
	tableitem* item; //�������� 
	symtab* upfloor;//�ϲ�� 
	symtab* next;//���²����������ɲ��У�����洢
	  
	symtab(string NAME,symtab *FATHER=NULL){
		name=NAME;
		upfloor=FATHER;
		item=NULL;
		next=NULL;
	}
	void ptrnext(symtab* cur){
		if(!next)	next=cur;
		else {
			symtab*iter=next;
			while(iter->next)	iter=iter->next;
			iter->next=cur;
		}
	}
	additem(tableitem* next){
		if(!item)	item=next;
		else{
			tableitem* cur=item;
			while(cur->next)	cur=cur->next;
			cur->next=next;
		}
	}
}*table; 


//string����ת��Ϊint��
int strtoi(string x){
	if(x=="")	return -1;
	int s=0,l=x.length();
	for(int i=0;i<l;i++)
		s=s*10+x[i]-'0';
	return s;
} 

// ���������ֱ�reserve(�ñ�ͬ����ߵ��ʱ����)�����б����Ӧ���ʱ�kw 
void create_keytable(){
//�����ֱ�  ,Ҳ�Ǳ����ֵĵ��ʱ����
//           ���ʱ��뻹����������൥�ʣ���ʶ���ͳ�����
//						�����ֱ�ʹ��identifier ,constant 
	for(int i=0;i<kwsize;i++)
		reserve[keyword[i]]=i+1;
//���ʷ��ű� 
//	�ؼ���һ��һ�� 
//��ʶ��ͳ��һ��
//������ͳ��һ�ࣻ���ɰ����ͣ����ͣ�ʵ�ͣ�������
	for(int i=1;i<=kwsize;i++)
		kw[i]=keyword[i-1];	
	kw[identifier]="identifier";
	kw[constant]="constant";
}

//��ȡPL0����Դ���� 
string fr(char *name){
	FILE *fp;
	if ((fp = fopen(name, "r")) == NULL) {	 
		printf("couldn't open input file %s\n", name);
		return 0;
    }
	fseek(fp, 0, 2);		//��λָ�뵽β�� 
    int fileLength = ftell(fp);
    fseek(fp, 0, 0);
    cout<<"file xlength:"<<fileLength<<endl;
    char *buff=new char[fileLength];
    int amount=fread(buff,sizeof(bool),fileLength,fp);    
    return buff;
}

//	Ԥ�����ӳ����޳�����ո�tab���س������С�ע��
string preprocessing(string s)
{
	int index = 0;
    if( !s.empty())
    {//���ﲻ�ԣ�����ȥ��ȫ���ո񣬶���ȥ������ո� 
        while( (index = s.find(' ',index)) != string::npos)
        {
            s.erase(index,1);
        }
        index=0;
        while( (index = s.find('\n',index)) != string::npos)
        {
            s.erase(index,1);
        }
    }
	return s;
} 

//������ 
void werror(int i=0,string s=""){
	cout<<"lexing ERROR! \n";
	if(i){
		cout<<s[i]<<s[i+1]<<s[i+2]<<s[i+3]<<s[i+4]<<s[i+5]<<s[i+6]<<s[i+7]<<s[i+8]<<s[i+9]; 
	}
	exit(0);
} 

//�����ʾ�������� 
void show_word_coding_table(){
	for(int i=0;i<kwsize;i++)
		cout<<keyword[i]<<" "<< reserve[keyword[i]]<<endl;
	cout<<"identifier "<<identifier<<endl;
	cout<<"constant "<<constant<<endl;
}

//��Ļ����������� 
void show(seq *head){
	seq* cur=head;
	while(cur){
		cout<<"< ";
		cout.width(3);  cout<<cur->sn<<",	";
		cout.width(10);  cout<<kw[cur->k]<<"	,	";
		cout.width(5);  cout<<cur->k<<"	,	";
		cout.width(5);  //��ǰ��������Ϊ�������ҳ���ֵ����-1�� 
		//	if(cur->k==constant &&strtoi(cur->v)!=-1)	cout<<num[strtoi(cur->v)] <<" >\n";
			if(cur->k==constant )	cout<<num[strtoi(cur->v)] <<" >\n";
			else cout<<cur->v <<" >\n";
		cur=cur->next;
	}
}

//��ʶ������ĵ��ʣ����Դ����ĵ���������ȥ 
void gens(string s,bool v1=0,bool v2=0){
//	v1 =0, v2=0,�ؼ��� 
//	v1 =0, v2=1 ����ʶ�� 
//	v1 =1, v2=1 ������ 
	if(!head )	tail=new seq;
	tail->next=new seq;
	if(!head )	head=tail,delete head,head=tail->next;
	tail=tail->next;
	if(reserve[s]){	//�ؼ��֣� 
		tail->k=reserve[s];
	}
	else if(!v1&&v2){//�Ǳ�ʶ��   Ҫ������ʶ���ķ��ű� 
		tail->k=identifier;
		tail->v=s;
		//���table��  �����ǰ���һ��������﷨������� 
	}
	else if(v1&&v2){//�ǳ��� ��������� 
		tail->k=constant;
		tail->v=to_string(ci);
		//Ҫʹ�ó�����ֵʱ����Ҫ��string����ת��Ϊint�� 
		//���num��
		num[ci++]=strtoi(s); 
	}
	tail->sn=snn++; 
}


//�ʷ����������������ʷ��ŵĶ�Ԫʽ���� 
//������һ��Դ�����ַ���  
void lexer(string s){
	//����DFA 
	string c="";
	int i=0,len=s.length();
	while(i<len){
		while(s[i]==' '||s[i]=='\n'||s[i]=='	')	i++;
		if(isalpha(s[i])){
			c+=s[i++];
			while(isalnum(s[i]))	
				c+=s[i++];
		//	i--;//���˲���Ҫ 
			gens(c,0,1);//�洢���� 
			c="";//c��� 
		}else if(isdigit(s[i])){
			int val=0;
			val=val*10+s[i]-'0';
			c+=s[i++];
			while(isdigit(s[i]))
			{
				val=val*10+s[i]-'0';	
				c+=s[i++];
			}
		//	i--;//���� ����Ҫ 
			gens(c,1,1);//�洢���� 
			c="";//c��� 
		}else if(s[i]=='+'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='-'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='*'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='/'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='='){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]==':'){
			c+=s[i++];
			if(s[i]=='='){
				c+=s[i++];
				gens(c);
				c="";
			}else {
			cout<<"---\n";	werror(i,s);
			}
		}else if(s[i]=='#'){
			c+=s[i++];	gens(c);	c=""; 
		}else if(s[i]=='>'){
			c+=s[i++];
			if(s[i]=='=')
				c+=s[i++];
			gens(c);
			c="";
		}else if(s[i]=='<'){
			c+=s[i++];
			if(s[i]=='=')
				c+=s[i++];
			gens(c);
			c="";
		}else if(s[i]=='('){
			c+=s[i++];	gens(c);	c=""; 
		}else if(s[i]==')'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]==','){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]==';'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='.'){
			c+=s[i++];	gens(c);	c="";
			cout<<"\nProgram END\n";
		}else {
			cout<<s[i]<<endl;
			cout<<"...\n";	werror(i,s); 
		}
	}
	
} 

//�﷨�������� ����ȷ���������Ĵ洢��ʽ һ�����﷨�������������﷨�� 
void parser(){
	
	
} 


