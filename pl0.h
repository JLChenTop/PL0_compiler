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
//全局变量 
const int maxnum=1000;//程序中出现的整型变量的最大个数 
string 
	keyword[]={
	//关键字13
	"const","var","procedure","begin","end",	"odd","if","then","call","while","do","read","write",
	//运算符 11
	"=",":=","+","-","*","/","#","<","<=",">",">=",
	//分界符 4
	",",";","(",")"
	//  2 
//	"identifier","constant"
	};
	//常量，
	//标识符    <标识符> → <字母>{<字母>|<数字>}
	/*
字母（不区分大小写）：isalpha();大写字母：isupper();
小写字母：islower();数字：isdigit();字母和数字：isalnum(); 
	*/ 
ll len;
int i,vnum,//标识符数目
	level; //当前程序所处的层次 
//保留字表 
map<string,int> reserve; 
map<int,string> kw;//单词种别及其对应的单词 符号 


//源程序经词法分析后产生的单词序列 
struct seq{
	int k;//单词种别
	string v;//单词自身值 
	seq* next;
	int sn;
	seq(){k=-1;v="";next=NULL;} 
}*head,*tail;
int snn;
//建立常量值表num ,及下一个可以放置常量的地址 
int num[maxnum],ci;

//要创建一个table表，存储源程序中出现的标识符
// 包括，常量名，变量名，过程名
//table的格式，name; kind; val(常量);level;ADR(变量);
//这里的ADR是存储常量实际值的指针，即num中的偏移量 
struct tableitem{//table表项 
	string name;//单词名词 
	int k;  //单词类别 
	int level;//量的层次 
	int val; //常量值 ，只用来存储一个常量的值 
//突然想到，并列层次的过程中定义的量应该是不能共享访问的 
	int ADR; //变量值的地址 ，存储变量 
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
class symtab{ //table表 
public:
	string name; // 表名 ，
	tableitem* item; //表项链表 
	symtab* upfloor;//上层表 
	symtab* next;//其下层表可能有若干并列，链表存储
	  
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


//string数据转换为int型
int strtoi(string x){
	if(x=="")	return -1;
	int s=0,l=x.length();
	for(int i=0;i<l;i++)
		s=s*10+x[i]-'0';
	return s;
} 

// 创建保留字表reserve(该表同样兼具单词编码表)，还有编码对应单词表kw 
void create_keytable(){
//保留字表  ,也是保留字的单词编码表，
//           单词编码还有另外的两类单词，标识符和常量，
//						其类别分别使用identifier ,constant 
	for(int i=0;i<kwsize;i++)
		reserve[keyword[i]]=i+1;
//单词符号表 
//	关键字一字一类 
//标识符统归一类
//常数，统归一类；（可按类型，整型，实型，布尔型
	for(int i=1;i<=kwsize;i++)
		kw[i]=keyword[i-1];	
	kw[identifier]="identifier";
	kw[constant]="constant";
}

//读取PL0语言源程序 
string fr(char *name){
	FILE *fp;
	if ((fp = fopen(name, "r")) == NULL) {	 
		printf("couldn't open input file %s\n", name);
		return 0;
    }
	fseek(fp, 0, 2);		//定位指针到尾部 
    int fileLength = ftell(fp);
    fseek(fp, 0, 0);
    cout<<"file xlength:"<<fileLength<<endl;
    char *buff=new char[fileLength];
    int amount=fread(buff,sizeof(bool),fileLength,fp);    
    return buff;
}

//	预处理子程序，剔除多余空格，tab，回车，换行、注释
string preprocessing(string s)
{
	int index = 0;
    if( !s.empty())
    {//这里不对，不是去除全部空格，而是去除多余空格 
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

//错误处理 
void werror(int i=0,string s=""){
	cout<<"lexing ERROR! \n";
	if(i){
		cout<<s[i]<<s[i+1]<<s[i+2]<<s[i+3]<<s[i+4]<<s[i+5]<<s[i+6]<<s[i+7]<<s[i+8]<<s[i+9]; 
	}
	exit(0);
} 

//输出显示单词类别表 
void show_word_coding_table(){
	for(int i=0;i<kwsize;i++)
		cout<<keyword[i]<<" "<< reserve[keyword[i]]<<endl;
	cout<<"identifier "<<identifier<<endl;
	cout<<"constant "<<constant<<endl;
}

//屏幕输出单词序列 
void show(seq *head){
	seq* cur=head;
	while(cur){
		cout<<"< ";
		cout.width(3);  cout<<cur->sn<<",	";
		cout.width(10);  cout<<kw[cur->k]<<"	,	";
		cout.width(5);  cout<<cur->k<<"	,	";
		cout.width(5);  //当前单词类型为常量，且常量值不是-1； 
		//	if(cur->k==constant &&strtoi(cur->v)!=-1)	cout<<num[strtoi(cur->v)] <<" >\n";
			if(cur->k==constant )	cout<<num[strtoi(cur->v)] <<" >\n";
			else cout<<cur->v <<" >\n";
		cur=cur->next;
	}
}

//把识别出来的单词，存进源程序的单词序列中去 
void gens(string s,bool v1=0,bool v2=0){
//	v1 =0, v2=0,关键字 
//	v1 =0, v2=1 ，标识符 
//	v1 =1, v2=1 ，常数 
	if(!head )	tail=new seq;
	tail->next=new seq;
	if(!head )	head=tail,delete head,head=tail->next;
	tail=tail->next;
	if(reserve[s]){	//关键字： 
		tail->k=reserve[s];
	}
	else if(!v1&&v2){//是标识符   要构建标识符的符号表 
		tail->k=identifier;
		tail->v=s;
		//存进table表  ，还是把这一步骤放在语法分析里吧 
	}
	else if(v1&&v2){//是常数 存进常数表 
		tail->k=constant;
		tail->v=to_string(ci);
		//要使用常数的值时，需要把string数据转换为int型 
		//存进num表
		num[ci++]=strtoi(s); 
	}
	tail->sn=snn++; 
}


//词法分析器产生，单词符号的二元式序列 
//参数是一个源程序长字符串  
void lexer(string s){
	//构建DFA 
	string c="";
	int i=0,len=s.length();
	while(i<len){
		while(s[i]==' '||s[i]=='\n'||s[i]=='	')	i++;
		if(isalpha(s[i])){
			c+=s[i++];
			while(isalnum(s[i]))	
				c+=s[i++];
		//	i--;//回退不需要 
			gens(c,0,1);//存储单词 
			c="";//c清空 
		}else if(isdigit(s[i])){
			int val=0;
			val=val*10+s[i]-'0';
			c+=s[i++];
			while(isdigit(s[i]))
			{
				val=val*10+s[i]-'0';	
				c+=s[i++];
			}
		//	i--;//回退 不需要 
			gens(c,1,1);//存储单词 
			c="";//c清空 
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

//语法分析器， 首先确定输出结果的存储格式 一般是语法分析树、抽象语法树 
void parser(){
	
	
} 


