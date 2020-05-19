/* 
基本量的定义 

词法分析 
*/ 
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
//保留字表  和操作码表optab 
map<string,int> reserve,optab; 
map<int,string> kw;//单词种别及其对应的单词 符号 
//cout<<"******test  \n";
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

void semanticserror();
void parserror();
class ptree;

//string数据转换为int型
int strtoi(string x){
	if(x=="")	return -1;
	int s=0,l=x.length();
	for(int i=0;i<l;i++)
		s=s*10+x[i]-'0';
	return s;
} 

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

//产生table表项函数 
tableitem* mkitem(string NAME,int kind,int l,int value=-1,int address=-1){
	tableitem* the=new tableitem(NAME,kind,l,value,address);
	return the;
}
//table表 
class symtab{ 
public:
	string name; // 表名 ，
	tableitem* item; //表项链表 
	int level;//每一个table表的层次 
	symtab* upfloor;//上层表 
	symtab* next;//其下层表可能有若干并列，链表存储
	  
	symtab(string NAME,int l,symtab *FATHER=NULL){
		name=NAME;
		level=l;
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
	//添加表项 
	additem(tableitem* next){
		if(!item)	item=next;
		else{
			tableitem* cur=item;
			while(cur->next)	cur=cur->next;
			cur->next=next;
		}
	}
}
*table; 

//从当前table表即tabptr.top() 中，按名查找某一项 并返回 
//从当前table表或其上层表中查找 
tableitem* findintab(symtab* com,string xname){
	symtab* now=com;
	tableitem* it=now->item;
	symtab* x=now->upfloor;
	while(it){/*
		cout.width(10);cout<<it->name ;
		cout.width(20);cout<<kw[it->k];
		cout.width(10);cout<<it->level ;
		if(it->val==-1)	cout.width(10),cout<<" 	 ";
			else cout.width(10),cout<<it->val ; 
		if(it->ADR==-1)	 cout<<"\n";
			else cout.width(10),cout<<it->ADR<<"\n"; 
			*/
		if(it->name==xname)		return it;
		it=it->next;
	}	
	//当前表查找完毕，若没有找到，去其上层表中查找
	if(x!=NULL) {
		tableitem* itt=findintab(x,xname);
		return itt;
	}	
//table中查无此项 
	return NULL;
} 
//输出table表
void showtab(symtab* com){
	symtab* now=com;
	tableitem* it=now->item;
	if(com==table)
		cout<<"name:	  kind:		    level:	value:	 ADR:\n";
	cout.setf(std::ios::left);
	symtab* x=now->next;
	while(it){
		cout.width(10);cout<<it->name ;
		cout.width(20);cout<<kw[it->k];
		cout.width(10);cout<<it->level ;
		if(it->val==-1)	cout.width(10),cout<<" 	 ";
			else cout.width(10),cout<<it->val ; 
		if(it->ADR==-1)	 cout<<"\n";
			else cout.width(10),cout<<it->ADR<<"\n"; 
		if(kw[it->k]=="procedure"){
			cout<<"访问子表\n"; 
			showtab(x);
			x=x->next;
		}
		it=it->next;
	}
} 

//操作码
#define jnum 9
string opcode[]{//"_",	  
	"j","j<","j<=","j>","j>=",//5
	"j=","j#","jz","jnz",//4 当操作码小于 9 时表明是跳转语句 
	"+","-","*","/",//4
	":=","minus"//2  
//	"","","",
//	"","","",
}; int opnum=15;

const int maxqnum=1000;
//四元式 存储结构
class quat{
public:
	int op;//操作码
	ptree *r1,*r2,*r3;
	int n3; 
	quat(int option,ptree* x1=NULL,ptree* x2=NULL,ptree* x3=NULL){
		op=option;
		r1=x1;r2=x2;r3=x3;
		n3=-1;
	}
	quat(int option,ptree* x1=NULL,ptree* x2=NULL,int q=-1){
		op=option;
		r1=x1;r2=x2;
		n3=q;
		r3=NULL;
	}
	//两个操作数 是一个标识符或者数字 
/*	string r1,r2;
	bool isnum1,isnum2;//操作数是一个数 还是变量或者常量 
	string r3;//目的地址，一个数；目的操作数，一个变量名 ,临时地址空间 
	bool isnum3;          //默认都是变量名 
	quat(int option,string s1="",string s2="",string s3="",
			bool a=false,bool b=false,bool c=false){
		op=option;
		r1=s1;r2=s2;r3=s3;
		isnum1=a;
		isnum2=b;
		isnum3=c;
	}*/

}
*assembly[maxqnum];
int nextquad=0; 


struct  semantics_list{
	int seqnum;
	semantics_list* next;
	semantics_list(int x,semantics_list* y=NULL)
	{seqnum=x;next=y;}
};

semantics_list *mkslist(int x){
	semantics_list *the=new semantics_list(x);
	return the;
} 

void backpatch(semantics_list *x,int thequad ){
	while(x){
		if(assembly[x->seqnum]->op <jnum){
			assembly[x->seqnum]->n3=thequad;
			x=x->next;
		}
		else semanticserror();
	}
}

semantics_list *mergelist(semantics_list *a,semantics_list *b){
	if(!a && !b)	return NULL;
	if(!a)	return b;
	if(!b)	return a;
	semantics_list *last;
	last=a;
	while(last->next)	last=last->next;
	if(b)	last->next=b;
	return a;
}

int nodeseqnum=0; 
//创建语法树的存储结构
class ptree{//语法树节点结构体 
public:
	int nodeseq;//节点的唯一标识 
	string name;//非终结符 
	seq* wnode;//单词符号指针  （叶子节点才是单词） 
	int level;//节点所在层次 
	ptree* father;
	ptree* firstchild; 
	ptree* nextchild;
	//用于语义分析的节点数据,
//每一个节点是一个非终结符或者终结符，有些具有数值属性
	int value; 
	int begin,next;
	int totrue,tofalse; 
	void* code; 
	int quad; 
	semantics_list *truelist,
	*falselist,
	*nextlist;//链中，为当前节点结束之后去往的下一个位置 
	
	ptree(){
		name=""; 
		wnode=NULL;
		father=firstchild=nextchild=NULL;
		nodeseq=nodeseqnum++;
	} 
	ptree(ptree* FATHER,string NAME){//创建一个非终结符节点 
		name=NAME;
		father=FATHER;
		wnode=NULL;
		firstchild=nextchild=NULL;
		nodeseq=nodeseqnum++;
	}
	ptree(seq* n1,ptree* n2){//创建一个终结符节点 
		wnode=n1;
		father=n2;
		firstchild=nextchild=NULL;
		nodeseq=nodeseqnum++;
	}
}
*root,*curnode,//当进入子程序时，保证curnode为当前子程序的节点 
*leaf,*curleaf,
*fnode; 			//node表示从子程序出去,仍指向这个节点 
//定义一个链表，链接所有的叶子

//输出四元式 
void showass(){
	cout.setf(std::ios::left);
	cout<<"中间代码四元式：\n";
	cout<<"/*操作数1、2的'-'表示无此操作数\n   操作数3的'-'表示临时地址空间，后面为其节点编号*/\n"; 
	cout<<"四元式变换	  操作码	操作数 1 	 2 	   3 \n";	
	
	for(int i=0;i<maxqnum;i++){
		if(assembly[i]){
			cout.width(18),cout<<i;
			cout.width(5),cout<<opcode[assembly[i]->op];
			cout.width(16),cout<<assembly[i]->op;
//操作数1			
			if(assembly[i]->r1)	{
				//判断操作数节点，！！--实际上--！！是否是叶子   //这里不可以，因为叶子都规约上去变成非叶子了 
				cout.width(10); 
				if(assembly[i]->r1->wnode){//叶子节点 
					if(assembly[i]->r1->wnode->k==constant){
						cout<<num[strtoi(assembly[i]->r1->wnode->v)];
					}else 	cout<<assembly[i]->r1->wnode->v; 
				}else{ //非叶子  ，临时地址空间 
					cout<<assembly[i]->r1->nodeseq;//value ; 
				} 
			} else cout.width(10),cout<<"-";//没有该操作数 
//操作数2 
			if(assembly[i]->r2){////判断操作数节点是否是叶子 同上	
				cout.width(10); 
				if(assembly[i]->r2->wnode){//叶子节点 
					if(assembly[i]->r2->wnode->k==constant){
						cout<<num[strtoi(assembly[i]->r2->wnode->v)];
					}else 	cout<<assembly[i]->r2->wnode->v;  
				}else{ //非叶子  ，临时地址空间 
					cout<<assembly[i]->r2->nodeseq;//value ; 
				} 
			}else cout.width(10),cout<<"-";//没有该操作数 
			//
//操作数3 
			if(assembly[i]->op<jnum){	//说明这是一个跳转指令，则目的操作数为一个数字，即n3 
				cout.width(10);
				if(assembly[i]->n3==-1)	cout<<"-";	 else cout<<assembly[i]->n3;
			}else {//非跳转指令   目的操作数应该是一个待操作的数 
			//目的操作时，要么是一个临时地址空间，要么是一个变量 
			//	if(assembly[i]->r3)	cout.width(10),cout<<assembly[i]->r3->value ; 
			//	else 
				if(opcode[assembly[i]->op]==":=")	//赋值语句，操作数是一个变量,节点是一个叶子节点 
					cout.width(10),cout<<assembly[i]->r3->wnode->v; 
				else if(opcode[assembly[i]->op]=="minus"){
					cout.width(10);
					if(assembly[i]->r3->wnode)
						cout<<assembly[i]->r3->wnode->v;
					else cout<<"-"<<assembly[i]->r3->nodeseq;
				}else{//其他语句， 目的操作数应该是一个临时地址空间， 
					cout.width(10),
					cout<<"-"<<assembly[i]->r3->nodeseq;
				} 
			}
			cout<<endl;
		}else break;
	}
}

// 创建保留字表reserve(该表同样兼具单词编码表)，
//还有编码对应单词表kw 
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
//操作码表
	for(int i=1;i<opnum;i++) {
		optab[opcode[i]]=i;
	}
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

//输出显示单词类别表 和操作码表 
void show_word_coding_table(){
	cout<<"    单词	类别码\n";
	for(int i=0;i<kwsize;i++)
		cout<<setw(10)<<keyword[i]<<"	 "<< reserve[keyword[i]]<<endl;
	cout<<"  identifier  	 "<<identifier<<endl;
	cout<<"  constant 	 "<<constant<<endl;
	//操作码表
	cout<<" 操作		操作码\n";
	for(int i=1;i<=opnum;i++) {
		cout<<setw(3)<<opcode[i]<<"		"<<i<<endl;
	}
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


