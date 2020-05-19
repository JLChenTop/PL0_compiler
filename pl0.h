/* 
�������Ķ��� 

�ʷ����� 
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
//�����ֱ�  �Ͳ������optab 
map<string,int> reserve,optab; 
map<int,string> kw;//�����ֱ����Ӧ�ĵ��� ���� 
//cout<<"******test  \n";
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

void semanticserror();
void parserror();
class ptree;

//string����ת��Ϊint��
int strtoi(string x){
	if(x=="")	return -1;
	int s=0,l=x.length();
	for(int i=0;i<l;i++)
		s=s*10+x[i]-'0';
	return s;
} 

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

//����table����� 
tableitem* mkitem(string NAME,int kind,int l,int value=-1,int address=-1){
	tableitem* the=new tableitem(NAME,kind,l,value,address);
	return the;
}
//table�� 
class symtab{ 
public:
	string name; // ���� ��
	tableitem* item; //�������� 
	int level;//ÿһ��table��Ĳ�� 
	symtab* upfloor;//�ϲ�� 
	symtab* next;//���²����������ɲ��У�����洢
	  
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
	//��ӱ��� 
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

//�ӵ�ǰtable��tabptr.top() �У���������ĳһ�� ������ 
//�ӵ�ǰtable������ϲ���в��� 
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
	//��ǰ�������ϣ���û���ҵ���ȥ���ϲ���в���
	if(x!=NULL) {
		tableitem* itt=findintab(x,xname);
		return itt;
	}	
//table�в��޴��� 
	return NULL;
} 
//���table��
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
			cout<<"�����ӱ�\n"; 
			showtab(x);
			x=x->next;
		}
		it=it->next;
	}
} 

//������
#define jnum 9
string opcode[]{//"_",	  
	"j","j<","j<=","j>","j>=",//5
	"j=","j#","jz","jnz",//4 ��������С�� 9 ʱ��������ת��� 
	"+","-","*","/",//4
	":=","minus"//2  
//	"","","",
//	"","","",
}; int opnum=15;

const int maxqnum=1000;
//��Ԫʽ �洢�ṹ
class quat{
public:
	int op;//������
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
	//���������� ��һ����ʶ���������� 
/*	string r1,r2;
	bool isnum1,isnum2;//��������һ���� ���Ǳ������߳��� 
	string r3;//Ŀ�ĵ�ַ��һ������Ŀ�Ĳ�������һ�������� ,��ʱ��ַ�ռ� 
	bool isnum3;          //Ĭ�϶��Ǳ����� 
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
//�����﷨���Ĵ洢�ṹ
class ptree{//�﷨���ڵ�ṹ�� 
public:
	int nodeseq;//�ڵ��Ψһ��ʶ 
	string name;//���ս�� 
	seq* wnode;//���ʷ���ָ��  ��Ҷ�ӽڵ���ǵ��ʣ� 
	int level;//�ڵ����ڲ�� 
	ptree* father;
	ptree* firstchild; 
	ptree* nextchild;
	//������������Ľڵ�����,
//ÿһ���ڵ���һ�����ս�������ս������Щ������ֵ����
	int value; 
	int begin,next;
	int totrue,tofalse; 
	void* code; 
	int quad; 
	semantics_list *truelist,
	*falselist,
	*nextlist;//���У�Ϊ��ǰ�ڵ����֮��ȥ������һ��λ�� 
	
	ptree(){
		name=""; 
		wnode=NULL;
		father=firstchild=nextchild=NULL;
		nodeseq=nodeseqnum++;
	} 
	ptree(ptree* FATHER,string NAME){//����һ�����ս���ڵ� 
		name=NAME;
		father=FATHER;
		wnode=NULL;
		firstchild=nextchild=NULL;
		nodeseq=nodeseqnum++;
	}
	ptree(seq* n1,ptree* n2){//����һ���ս���ڵ� 
		wnode=n1;
		father=n2;
		firstchild=nextchild=NULL;
		nodeseq=nodeseqnum++;
	}
}
*root,*curnode,//�������ӳ���ʱ����֤curnodeΪ��ǰ�ӳ���Ľڵ� 
*leaf,*curleaf,
*fnode; 			//node��ʾ���ӳ����ȥ,��ָ������ڵ� 
//����һ�������������е�Ҷ��

//�����Ԫʽ 
void showass(){
	cout.setf(std::ios::left);
	cout<<"�м������Ԫʽ��\n";
	cout<<"/*������1��2��'-'��ʾ�޴˲�����\n   ������3��'-'��ʾ��ʱ��ַ�ռ䣬����Ϊ��ڵ���*/\n"; 
	cout<<"��Ԫʽ�任	  ������	������ 1 	 2 	   3 \n";	
	
	for(int i=0;i<maxqnum;i++){
		if(assembly[i]){
			cout.width(18),cout<<i;
			cout.width(5),cout<<opcode[assembly[i]->op];
			cout.width(16),cout<<assembly[i]->op;
//������1			
			if(assembly[i]->r1)	{
				//�жϲ������ڵ㣬����--ʵ����--�����Ƿ���Ҷ��   //���ﲻ���ԣ���ΪҶ�Ӷ���Լ��ȥ��ɷ�Ҷ���� 
				cout.width(10); 
				if(assembly[i]->r1->wnode){//Ҷ�ӽڵ� 
					if(assembly[i]->r1->wnode->k==constant){
						cout<<num[strtoi(assembly[i]->r1->wnode->v)];
					}else 	cout<<assembly[i]->r1->wnode->v; 
				}else{ //��Ҷ��  ����ʱ��ַ�ռ� 
					cout<<assembly[i]->r1->nodeseq;//value ; 
				} 
			} else cout.width(10),cout<<"-";//û�иò����� 
//������2 
			if(assembly[i]->r2){////�жϲ������ڵ��Ƿ���Ҷ�� ͬ��	
				cout.width(10); 
				if(assembly[i]->r2->wnode){//Ҷ�ӽڵ� 
					if(assembly[i]->r2->wnode->k==constant){
						cout<<num[strtoi(assembly[i]->r2->wnode->v)];
					}else 	cout<<assembly[i]->r2->wnode->v;  
				}else{ //��Ҷ��  ����ʱ��ַ�ռ� 
					cout<<assembly[i]->r2->nodeseq;//value ; 
				} 
			}else cout.width(10),cout<<"-";//û�иò����� 
			//
//������3 
			if(assembly[i]->op<jnum){	//˵������һ����תָ���Ŀ�Ĳ�����Ϊһ�����֣���n3 
				cout.width(10);
				if(assembly[i]->n3==-1)	cout<<"-";	 else cout<<assembly[i]->n3;
			}else {//����תָ��   Ŀ�Ĳ�����Ӧ����һ������������ 
			//Ŀ�Ĳ���ʱ��Ҫô��һ����ʱ��ַ�ռ䣬Ҫô��һ������ 
			//	if(assembly[i]->r3)	cout.width(10),cout<<assembly[i]->r3->value ; 
			//	else 
				if(opcode[assembly[i]->op]==":=")	//��ֵ��䣬��������һ������,�ڵ���һ��Ҷ�ӽڵ� 
					cout.width(10),cout<<assembly[i]->r3->wnode->v; 
				else if(opcode[assembly[i]->op]=="minus"){
					cout.width(10);
					if(assembly[i]->r3->wnode)
						cout<<assembly[i]->r3->wnode->v;
					else cout<<"-"<<assembly[i]->r3->nodeseq;
				}else{//������䣬 Ŀ�Ĳ�����Ӧ����һ����ʱ��ַ�ռ䣬 
					cout.width(10),
					cout<<"-"<<assembly[i]->r3->nodeseq;
				} 
			}
			cout<<endl;
		}else break;
	}
}

// ���������ֱ�reserve(�ñ�ͬ����ߵ��ʱ����)��
//���б����Ӧ���ʱ�kw 
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
//�������
	for(int i=1;i<opnum;i++) {
		optab[opcode[i]]=i;
	}
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

//�����ʾ�������� �Ͳ������ 
void show_word_coding_table(){
	cout<<"    ����	�����\n";
	for(int i=0;i<kwsize;i++)
		cout<<setw(10)<<keyword[i]<<"	 "<< reserve[keyword[i]]<<endl;
	cout<<"  identifier  	 "<<identifier<<endl;
	cout<<"  constant 	 "<<constant<<endl;
	//�������
	cout<<" ����		������\n";
	for(int i=1;i<=opnum;i++) {
		cout<<setw(3)<<opcode[i]<<"		"<<i<<endl;
	}
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


