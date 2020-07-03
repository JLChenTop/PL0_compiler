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

//提前声明 


struct  semantics_list; //用于 语义分析，拉链，结构体链表 
class ptree;             //语法树 
struct tableitem;//符号表项，结构体提前声明 
class symtab;			//符号表 



string fr(char *name);//读取PL0语言源程序
void create_keytable();// 创建保留字表reserve(该表同样兼具单词编码表)，
                        //还有编码对应单词表kw ,操作码表optab 
 
void levelorder(ptree*root);//语法树层次遍历


//词法分析、语法分析、语义分析，错误处理函数 
void lexerror(int,string);
void parserror();
void semanticserror();

//屏幕输出显示函数 
void show_word_coding_table();//输出显示单词类别表 和操作码表 
void show();//屏幕输出单词序列 
void showtab();//输出table表
void showass();//输出中间代码四元式 
void showS();   //输出运行栈 

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
//建立常量值表num ,及下一个可以放置常量的地址 
int num[maxnum],ci;

//函数-string数据转换为int型
int strtoi(string x){
	if(x=="")	return -1;
	int s=0,l=x.length();
	for(int i=0;i<l;i++)
		s=s*10+x[i]-'0';
	return s;
} 

//************读取源程序******************* 
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
 //   cout<<"file length:"<<fileLength<<endl;
    char *buff=new char[fileLength];
    int amount=fread(buff,sizeof(bool),fileLength,fp);    
    if(fclose(fp) !=0){
    	cout<<"close the file error!\n";
	}
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





// ************词法分析 ******************** 
//...............定义
//源程序经词法分析后产生的单词序列 
struct seq{
	int k;//单词种别
	string v;//单词自身值 
	seq* next;
	int sn;
	seq(){k=-1;v="";next=NULL;} 
}*head,*tail;
int snn;//用于标识当前单词序列号 

//...............执行 
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

//词法分析器  产生，单词符号的二元式序列 
//参数是一个源程序长字符串  
void lexer(string s){
	//构建DFA 
	string c="";
	int i=0,len=s.length();
	cout<<"\t\t开始词法分析\n";//<<len<<endl; 
	while(i<len){
		while(s[i]==' '||s[i]=='\n'||
		   s[i]=='\t'||s[i]=='\0')	i++;
		if(i>=len)	break;
		if(isalpha(s[i])){
			c+=s[i++];
			while(isalnum(s[i]))	
				c+=s[i++];
		//	i--;//回退不需要 
			gens(c,0,1);//存储单词 
			c="";//c清空 
		}
		else if(isdigit(s[i])){
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
		}
		else if(s[i]=='+'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='-'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='*'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='/'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='='){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]==':')
		{
			c+=s[i++];
			if(s[i]=='='){
				c+=s[i++];
				gens(c);
				c="";
			}else {
				lexerror(i,s);
			}
		}
		else if(s[i]=='#'){
			c+=s[i++];	gens(c);	c=""; 
		}else if(s[i]=='>')
		{
			c+=s[i++];
			if(s[i]=='=')
				c+=s[i++];
			gens(c);
			c="";
		}
		else if(s[i]=='<')
		{
			c+=s[i++];
			if(s[i]=='=')
				c+=s[i++];
			gens(c);
			c="";
		}
		else if(s[i]=='('){
			c+=s[i++];	gens(c);	c=""; 
		}else if(s[i]==')'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]==','){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]==';'){
			c+=s[i++];	gens(c);	c="";
		}else if(s[i]=='.'){
			c+=s[i++];//	gens(c);
			c="";
		//	cout<<"\n lexer Program END\n";
		}else {
			lexerror(i,s); 
		}
	}
	cout<<"\t\t词法分析success!!\n";
} 


//**************************语法分析**************************************
//.....................定义
//语法分析，采用递归下降子程序法 ，要产生一颗语法树 
//声明所有子程序
void P(); //主程序 
void SP(); //分程序 
void C(); //常量说明部分 
void CD();//常量定义 
void V(); //变量说明部分 
void PRO(); //过程说明部分 
void PRPHEAD();//过程头 
void SEN(); //语句 
void ASS();//赋值语句 
void FH(); //复合语句 
void CDI(); //条件 
void BDS(); //表达式 
void TERM();//项 
void FAC(); //因子 
void COND(); //条件调用语句 
void PROCALL(); //过程调用语句 
void LOOP();//当型循环语句 
void DU(); //读语句 
void XIE(); //写语句 
void PS();//加减运算符 
void MULD();//乘除运算符 
void RELA();//关系运算符
 
//声明一个类   用于语法树节点定义，其用于语义分析 
// struct  semantics_list; 
int nodeseqnum=0; //标明当前语法树节点编号 
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
	//构造函数 
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
*fnode; 			//fnode表示从子程序出去,仍指向这个节点 
//定义一个链表，链接所有的叶子
//打印语法树的一个节点 
void printnode(ptree*nd){ 
	if(nd->name=="" && nd->wnode==NULL)	cout<<"   ";//一个子树 
	else if(nd->firstchild){
		cout<<nd->name<<" ";
	}else {//为终结符 
		if(nd->wnode->k ==identifier||nd->wnode->k ==constant){
			if(nd->wnode->k==constant &&strtoi(nd->wnode->v)!=-1)	
				cout<<num[strtoi(nd->wnode->v)] <<"  ";
			else cout<<nd->wnode->v <<"  ";	
		}else  
			cout<<kw[nd->wnode->k]<<" ";
	}
}
 
//语法树的遍历，后序遍历（左右根） 
void postorder(ptree *root){
	ptree *cur;
	if(root){
		cur=root->firstchild;
		//遍历孩子 
		if(!cur){//无孩子，是叶子 
			if(root->wnode->k ==(identifier|constant)){
				if(root->wnode->k==constant &&strtoi(root->wnode->v)!=-1)	
					cout<<num[strtoi(root->wnode->v)] <<" >\n";
				else cout<<root->wnode->v <<" >\n";	
			}else  cout<<kw[root->wnode->k]<<" ";
		}else{	
			//有孩子，表明规约成一个非终结符 
			while(cur){
				postorder(cur);
				cur=cur->nextchild;
			}
		} 
		cout<<root->name<<" ";
		cout<<endl;
	}
} 
//语法树层次遍历 
void levelorder(ptree*root){
	queue<ptree*> q,p;
	ptree* t;
	p.push(root);
	while(!p.empty()){
	//	cout<<"88";
		while(!p.empty()){
			t=p.front();
			printnode(t);
			p.pop();
			t=t->firstchild;
			if(t){
				q.push(t);
				while(t->nextchild){
					t=t->nextchild;
					q.push(t);
				}
//这里表示把一个非终结符的所有孩子入队 应该空一大格子
//	t=new ptree;q.push(t); 
			}
		}
		cout<<endl;
		p.swap(q);
	}
}

//.......................执行 (语法分析 
//执行见parse.h 

 
//**************************语义分析 ************************************** 
//...................定义 
//要创建一个table表，存储源程序中出现的标识符
// 包括，常量名，变量名，过程名
//table的格式，name; kind; val(常量);level(层次);ADR(变量栈中位置);
struct tableitem{//table表项 
	string name;//单词名词 
	int k;  //单词类别 
	int level;//量的层次 
	int val; //常量值 ，只用来存储一个常量的值 
//突然想到，并列层次的过程中定义的量应该是不能共享访问的 
	int ADR; //变量值的地址 ，存储变量 
	tableitem* next;
	tableitem(){next=NULL;}
	tableitem(string NAME,int kind,int l,int value=-1,int address=-1){
		name=NAME;k=kind;level=l;val=value;ADR=address;	}
}; 
tableitem* findintab(symtab* com,string xname,int *l);
//产生table表项函数 
tableitem* mkitem(string NAME,int kind,int l,int value=-1,int address=-1){
	tableitem* the=new tableitem(NAME,kind,l,value,address);
	return the;
}

//table表   类 
class symtab{ 
public:
	string name; // 表名 ，
	tableitem* item; //表项链表 
	int level;//每一个table表的层次 
	symtab* upfloor;//上层表 
	symtab* next;//其下层表可能有若干并列，链表存储
	int itemnum;
	symtab(string NAME,int l,symtab *FATHER=NULL){
		name=NAME;
		level=l;
		upfloor=FATHER;
		item=NULL;
		next=NULL;
		itemnum=0;
	}
	void ptrnext(symtab* cur){ //添加其下层表 
		if(!next)	next=cur;
		else {
			symtab*iter=next;
			while(iter->next)	iter=iter->next;
			iter->next=cur;
		}
	}
	//添加表项 
	additem(tableitem* next){
		if(kw[next->k]!="procedure" && kw[next->k]!="constant")
			itemnum++;
		int x=0;
		if(findintab(this,next->name,&x)!=NULL)
			semanticserror();	
		if(!item)	item=next;
		else{
			tableitem* cur=item;
			while(cur->next)	cur=cur->next;
			cur->next=next;
		}
	}
}
*table; //主table表 

//从当前table表即tabptr.top() 中，按名查找某一项 并返回 该表项 
//从当前table表或其上层表中查找 
// l查找到的变量与当前层间的层差 
tableitem* findintab(symtab* com,string xname,int *l){
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
		(*l)++;
		tableitem* itt=findintab(x,xname,l);
		return itt;
	}	
//table中查无此项 
	return NULL;
} 

const int maxqnum=1000;//中间代码条数 
//中间代码 存储结构
class quat{
public:
	int op;//操作码
	int L;//层次差，也即，从 动态链往上回溯的层数 
	int n3; 
	quat(int option,int lc=0,int a=0){
		op=option;
		L=lc;
		n3=a;
	}
}
*assembly[maxqnum];

int nextquad=0; 
stack<int> backjump;//用于过程调用语句的初始回填 
//stack<int> retstack;//call语句返回地址栈   应该是每一个过程一个返回地址栈 

//下面结构体，和3个函数，用于控制语句翻译时，跳转地址的生成 
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
		assembly[x->seqnum]->n3=thequad;
		x=x->next;
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


//**************解释执行 **************************************** 
//操作码
string opcode[]{"_",
	"INT","LIT","LOD","STO",//4
	"call","ret","read","write",//4
	"minus",			// 1 
	"+","-","*","/",//4
	"j","j<","j<=","j>","j>=",//5
	"j=","j#","jz","jnz"//4 
}; int opnum=23;
int IR=0,//存放当前指令的地址
	PC,//下一条指令的地址
	T=-1,//栈顶寄存器，始终指向栈顶 
	B;//基地址寄存器 
	
int S[1000*5];//定义一个一维整型数组作为运行时栈 
	
	
int	lastpro;//上一个过程 
int  curpro;//当前所属的过程入口地址，标识这一过程
bool isrecursion;//是否是递归调用 
int recursionlen;//递归调用深度；
int fdglen;//非递归深度 

void exec(){
	cout<<"开始执行\n";
/*
string opcode[]{"_",
	"INT","LIT","LOD","STO",		//4
	"call","ret","read","write",	//4
	"minus",						// 1 
	"+","-","*","/",				//4
	"j","j<","j<=","j>","j>=",		//5
	"j=","j#","jz","jnz"			//4 
};
*/
//	int  dglv=-1; 
	while(assembly[IR]!=NULL){
		PC=IR+1;
		quat *ir=assembly[IR]; 
		
		switch(ir->op){
			case 1:{// INT
			//  进入一个新的过程，需要 
		//	标明当前 过程
				lastpro=curpro;
				curpro=IR; 
				T+=ir->n3;
	//			cout<<"过程调用的联系单元：  ";showS();  
				break;
			}
			case 2:{// LIT
				S[++T]=ir->n3;
		//		cout<<"栈顶："<<S[T]<<endl;
				break;
			}
			case 3:{// LOD
				// 取出变量放到栈顶
				int add=B,//一个过程的基地址 
					lv=ir->L;//层差 
		//		if(dglv>0 && lv!=0)
		//			lv+=dglv;
				while(lv!=0){
					add=S[add+1];		//静态链，去往静态外层 
					lv--;
				} 		
				S[T+1]=S[add+ir->n3];//  取出数放到栈顶，基地址+偏移量 
				T++;
		//	cout<<"栈顶："<<S[T]<<endl;	showS();
				break;
			}
			case 4:{// STO
				// 栈顶 存入变量
				int add=B,//一个过程的基地址 
					lv=ir->L;//层差 
		//		if(dglv>0 && lv!=0)
	//				lv+=dglv;
				while(lv!=0){
					add=S[add+1];		//静态链，去往静态外层 
					lv--;
				} 
		//		cout<<"\t\t\tdizhi :"<<add<<" "<<ir->n3<<".....";
				S[add+ir->n3]=S[T--];//  栈顶存入变量，基地址+偏移量 
	//		cout<<"栈顶："<<S[T]<<endl;	showS();
				break;
			}
			case 5:{// CALL
				//过程调用	
			//填写三个联系单元，返回地址，静态链，动态链 
//   静态链   被调用过程的静态外层的，基地址，其实就是基址寄存器B的值 
//   动态链   被调用过程的静态外层的，最新数据指针，其实就是栈顶指针寄存器T的值
//改为 
//   静态链   被调用过程在定义时的外层，需要往前查找
//   动态链   被调用过程的静态外层的，基地址，其实就是基址寄存器B的值 

				S[T+1]=PC;//存放返回地址
		//		S[T+2]=B;//静态链
		//		S[T+3]=T;//动态链 
				int jdl=0;		//是被调用的这个过程在定义时的外层过程的入口地址 
				bool meetret=false;
				for(int i=ir->n3;i>0;i--){
					if(assembly[i]->op==1 &&!meetret){
						jdl=i;break;
					}else if(assembly[i]->op==1 && meetret){//遇到一个ret
						meetret=false;
					}else if(assembly[i]->op==6){//遇到一个ret
						meetret=true;
					} 
				} 
				if(jdl==S[B]){//表示，该过程定义时的外层和被调用时的外层是同一个 
					//所以，其静态链=外层的入口地址即基地址B
					S[T+2]=B;//静态链	 
				}else{//否则其，静态链填动态外层的静态链 
					S[T+2]=S[B+1];//静态链
				} 
				S[T+3]=B;//动态链 
				// 
				PC= ir->n3;//被调用过程的目标程序的入口送给PC
				B=T+1;    // 被调用过程的基地址值送入基址寄存器B 
				// T栈顶指针寄存器的值，在过程内部，开辟空间时修改 
	//  递归调用产生了新的层次，层差取出数值出了问题	
		//		dglv++;
	//  上面这种方法面对递归生效，面对调用里面再调用，就会犯错			
			//	好像也有效 
		//		cout<<T<<"  call:"<<S[T+1]<<" - "<<S[T+2]<<" - "<<S[T+3]<<endl;
	//  还是会有错误，调用的层次		
				break;
			}
			case 6:{// RET
				//  过程返回
				PC=S[B+0]; 
				T=B-1;//S[B+2];
				B=S[B+2];
			//	dglv--;
				break;
			}
			case 7:{//READ
				cout<<"please input an interger:";
				int x;cin>>x;
				// 读入一个数，存入变量，
				//其实可以改为两条指令 ，一条读数，一条sto 
				int add=B,//一个过程的基地址 
					lv=ir->L;//层差 
	//			if(dglv>0 && lv!=0)
	//				lv+=dglv;
				while(lv!=0){
					add=S[add+1];		//静态链，去往静态外层 
					lv--;
				} 
				S[add+ir->n3]=x;
				break;
			}
			case 8:{//WRITE
				cout<<"ouput :";
				cout<<S[T--]<<endl;
	//			showS();
				break;
			}
			case 9:{//MINUS
				S[T]*=-1;
				break;
			}
			case 10:{//+
				S[T-1]=S[T-1]+S[T];
				T--;
				break;
			}
			case 11:{//-
				S[T-1]=S[T-1]-S[T];
				T--;
				break;
			}
			case 12:{//*
				S[T-1]=S[T-1]*S[T];
				T--;
				break;
			}
			case 13:{//  /
				S[T-1]=S[T-1]/S[T];
				T--;
				break;
			}
			case 14:{// J
			//	无条件跳转 
				PC=ir->n3; 
				break;
			}
			case 15:{// J<
				if(S[T-1] < S[T])
					PC=ir->n3;
				T-=2;
				break;
			}
			case 16:{// J<=
				if(S[T-1] <= S[T])
					PC=ir->n3;
				T-=2;
				break;
			}
			case 17:{// J>
				if(S[T-1] > S[T])
					PC=ir->n3;
				T-=2;
				break;
			}
			case 18:{// J>=
				if(S[T-1] >= S[T])
					PC=ir->n3;
				T-=2;
				break;
			}
			case 19:{// J=
				if(S[T-1] == S[T])
					PC=ir->n3;
				T-=2;
				break;
			}
			case 20:{// J#
				if(S[T-1] != S[T])
					PC=ir->n3;
				T-=2;
				break;
			}
			case 21:{// JZ
				if( S[T] == 0 )
					PC=ir->n3;
				T--;
				break;
			}
			case 22:{//JNZ
				if( S[T] != 0 )
					PC=ir->n3;
				T--;
				break;
			}
			default:
				cout<<"error op\n";	
		}
	//	cout<<"IR "<<IR<<"  PC "<<PC<<endl;
		IR=PC;
	} 
} 

// 创建保留字表reserve(该表同样兼具单词编码表)，
//还有编码对应单词表kw ,操作码表optab 
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


//********************错误处理 *************************************** 
//词法分析------错误处理 
void lexerror(int i=0,string s=""){
	cout<<"lexing ERROR! \n";
	if(i){
		cout<<"i-6:"<<i-6<<s[i-6]<<s[i-5]<<s[i-4]<<s[i-3]<<s[i-2]<<s[i-1];
		cout<<i<<"--"<<s[i]<<s[i+1]<<s[i+2]<<s[i+3]<<s[i+4]<<s[i+5]<<s[i+6]<<s[i+7]<<s[i+8]<<s[i+9]; 
	}
	exit(0);
} 

//语法分析------错误处理  
void parserror();
//语义分析------错误处理 
void semanticserror();


//*******************输出显示 ***********************************
//输出显示单词类别表 和操作码表 
void show_word_coding_table(){
	cout<<"    单词	类别码\n";
	for(int i=0;i<kwsize;i++)
		cout<<setw(10)<<keyword[i]<<"	 "<< reserve[keyword[i]]<<endl;
	cout<<"  identifier  	 "<<identifier<<endl;
	cout<<"  constant 	 "<<constant<<endl;
	//操作码表
	cout<<" 操作		操作码\n";
	for(int i=1;i<opnum;i++) {
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
	cout<<"表项"<<now->name<<"   num:"<<now->itemnum<<endl;
	return;
} 

//输出中间代码四元式 
void showass(){
	cout.setf(std::ios::left);
	cout<<"中间代码四元式：\n";
	cout<<"/* L 层差，  a偏移量*/\n"; 
	cout<<"四元式变换	  操作码	操作数  l	 a \n";	
	
	for(int i=0;i<maxqnum;i++){
		if(assembly[i]){
			cout.width(18),cout<<i;
			cout.width(5),cout<<opcode[assembly[i]->op];
			cout.width(16),cout<<assembly[i]->op;
			cout.width(10);
			if(assembly[i]->L==-1)	cout<<"-";
			else cout<<assembly[i]->L;
			
			cout.width(10);
			if(assembly[i]->n3==-1)	cout<<"-";
			else cout<<assembly[i]->n3;
			cout<<endl;
		}else break;	
	}
}

//输出运行栈 
void showS(){
	for(int i=0;i<=T;i++) 
		cout<<S[i]<<" ";
	cout<<"\n";
} 




