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

//��ǰ���� 


struct  semantics_list; //���� ����������������ṹ������ 
class ptree;             //�﷨�� 
struct tableitem;//���ű���ṹ����ǰ���� 
class symtab;			//���ű� 



string fr(char *name);//��ȡPL0����Դ����
void create_keytable();// ���������ֱ�reserve(�ñ�ͬ����ߵ��ʱ����)��
                        //���б����Ӧ���ʱ�kw ,�������optab 
 
void levelorder(ptree*root);//�﷨����α���


//�ʷ��������﷨����������������������� 
void lexerror(int,string);
void parserror();
void semanticserror();

//��Ļ�����ʾ���� 
void show_word_coding_table();//�����ʾ�������� �Ͳ������ 
void show();//��Ļ����������� 
void showtab();//���table��
void showass();//����м������Ԫʽ 
void showS();   //�������ջ 

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
//��������ֵ��num ,����һ�����Է��ó����ĵ�ַ 
int num[maxnum],ci;

//����-string����ת��Ϊint��
int strtoi(string x){
	if(x=="")	return -1;
	int s=0,l=x.length();
	for(int i=0;i<l;i++)
		s=s*10+x[i]-'0';
	return s;
} 

//************��ȡԴ����******************* 
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
 //   cout<<"file length:"<<fileLength<<endl;
    char *buff=new char[fileLength];
    int amount=fread(buff,sizeof(bool),fileLength,fp);    
    if(fclose(fp) !=0){
    	cout<<"close the file error!\n";
	}
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





// ************�ʷ����� ******************** 
//...............����
//Դ���򾭴ʷ�����������ĵ������� 
struct seq{
	int k;//�����ֱ�
	string v;//��������ֵ 
	seq* next;
	int sn;
	seq(){k=-1;v="";next=NULL;} 
}*head,*tail;
int snn;//���ڱ�ʶ��ǰ�������к� 

//...............ִ�� 
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

//�ʷ�������  ���������ʷ��ŵĶ�Ԫʽ���� 
//������һ��Դ�����ַ���  
void lexer(string s){
	//����DFA 
	string c="";
	int i=0,len=s.length();
	cout<<"\t\t��ʼ�ʷ�����\n";//<<len<<endl; 
	while(i<len){
		while(s[i]==' '||s[i]=='\n'||
		   s[i]=='\t'||s[i]=='\0')	i++;
		if(i>=len)	break;
		if(isalpha(s[i])){
			c+=s[i++];
			while(isalnum(s[i]))	
				c+=s[i++];
		//	i--;//���˲���Ҫ 
			gens(c,0,1);//�洢���� 
			c="";//c��� 
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
		//	i--;//���� ����Ҫ 
			gens(c,1,1);//�洢���� 
			c="";//c��� 
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
	cout<<"\t\t�ʷ�����success!!\n";
} 


//**************************�﷨����**************************************
//.....................����
//�﷨���������õݹ��½��ӳ��� ��Ҫ����һ���﷨�� 
//���������ӳ���
void P(); //������ 
void SP(); //�ֳ��� 
void C(); //����˵������ 
void CD();//�������� 
void V(); //����˵������ 
void PRO(); //����˵������ 
void PRPHEAD();//����ͷ 
void SEN(); //��� 
void ASS();//��ֵ��� 
void FH(); //������� 
void CDI(); //���� 
void BDS(); //���ʽ 
void TERM();//�� 
void FAC(); //���� 
void COND(); //����������� 
void PROCALL(); //���̵������ 
void LOOP();//����ѭ����� 
void DU(); //����� 
void XIE(); //д��� 
void PS();//�Ӽ������ 
void MULD();//�˳������ 
void RELA();//��ϵ�����
 
//����һ����   �����﷨���ڵ㶨�壬������������� 
// struct  semantics_list; 
int nodeseqnum=0; //������ǰ�﷨���ڵ��� 
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
	//���캯�� 
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
*fnode; 			//fnode��ʾ���ӳ����ȥ,��ָ������ڵ� 
//����һ�������������е�Ҷ��
//��ӡ�﷨����һ���ڵ� 
void printnode(ptree*nd){ 
	if(nd->name=="" && nd->wnode==NULL)	cout<<"   ";//һ������ 
	else if(nd->firstchild){
		cout<<nd->name<<" ";
	}else {//Ϊ�ս�� 
		if(nd->wnode->k ==identifier||nd->wnode->k ==constant){
			if(nd->wnode->k==constant &&strtoi(nd->wnode->v)!=-1)	
				cout<<num[strtoi(nd->wnode->v)] <<"  ";
			else cout<<nd->wnode->v <<"  ";	
		}else  
			cout<<kw[nd->wnode->k]<<" ";
	}
}
 
//�﷨���ı�����������������Ҹ��� 
void postorder(ptree *root){
	ptree *cur;
	if(root){
		cur=root->firstchild;
		//�������� 
		if(!cur){//�޺��ӣ���Ҷ�� 
			if(root->wnode->k ==(identifier|constant)){
				if(root->wnode->k==constant &&strtoi(root->wnode->v)!=-1)	
					cout<<num[strtoi(root->wnode->v)] <<" >\n";
				else cout<<root->wnode->v <<" >\n";	
			}else  cout<<kw[root->wnode->k]<<" ";
		}else{	
			//�к��ӣ�������Լ��һ�����ս�� 
			while(cur){
				postorder(cur);
				cur=cur->nextchild;
			}
		} 
		cout<<root->name<<" ";
		cout<<endl;
	}
} 
//�﷨����α��� 
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
//�����ʾ��һ�����ս�������к������ Ӧ�ÿ�һ�����
//	t=new ptree;q.push(t); 
			}
		}
		cout<<endl;
		p.swap(q);
	}
}

//.......................ִ�� (�﷨���� 
//ִ�м�parse.h 

 
//**************************������� ************************************** 
//...................���� 
//Ҫ����һ��table���洢Դ�����г��ֵı�ʶ��
// ����������������������������
//table�ĸ�ʽ��name; kind; val(����);level(���);ADR(����ջ��λ��);
struct tableitem{//table���� 
	string name;//�������� 
	int k;  //������� 
	int level;//���Ĳ�� 
	int val; //����ֵ ��ֻ�����洢һ��������ֵ 
//ͻȻ�뵽�����в�εĹ����ж������Ӧ���ǲ��ܹ�����ʵ� 
	int ADR; //����ֵ�ĵ�ַ ���洢���� 
	tableitem* next;
	tableitem(){next=NULL;}
	tableitem(string NAME,int kind,int l,int value=-1,int address=-1){
		name=NAME;k=kind;level=l;val=value;ADR=address;	}
}; 
tableitem* findintab(symtab* com,string xname,int *l);
//����table����� 
tableitem* mkitem(string NAME,int kind,int l,int value=-1,int address=-1){
	tableitem* the=new tableitem(NAME,kind,l,value,address);
	return the;
}

//table��   �� 
class symtab{ 
public:
	string name; // ���� ��
	tableitem* item; //�������� 
	int level;//ÿһ��table��Ĳ�� 
	symtab* upfloor;//�ϲ�� 
	symtab* next;//���²����������ɲ��У�����洢
	int itemnum;
	symtab(string NAME,int l,symtab *FATHER=NULL){
		name=NAME;
		level=l;
		upfloor=FATHER;
		item=NULL;
		next=NULL;
		itemnum=0;
	}
	void ptrnext(symtab* cur){ //������²�� 
		if(!next)	next=cur;
		else {
			symtab*iter=next;
			while(iter->next)	iter=iter->next;
			iter->next=cur;
		}
	}
	//��ӱ��� 
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
*table; //��table�� 

//�ӵ�ǰtable��tabptr.top() �У���������ĳһ�� ������ �ñ��� 
//�ӵ�ǰtable������ϲ���в��� 
// l���ҵ��ı����뵱ǰ���Ĳ�� 
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
	//��ǰ�������ϣ���û���ҵ���ȥ���ϲ���в���
	if(x!=NULL) {
		(*l)++;
		tableitem* itt=findintab(x,xname,l);
		return itt;
	}	
//table�в��޴��� 
	return NULL;
} 

const int maxqnum=1000;//�м�������� 
//�м���� �洢�ṹ
class quat{
public:
	int op;//������
	int L;//��βҲ������ ��̬�����ϻ��ݵĲ��� 
	int n3; 
	quat(int option,int lc=0,int a=0){
		op=option;
		L=lc;
		n3=a;
	}
}
*assembly[maxqnum];

int nextquad=0; 
stack<int> backjump;//���ڹ��̵������ĳ�ʼ���� 
//stack<int> retstack;//call��䷵�ص�ַջ   Ӧ����ÿһ������һ�����ص�ַջ 

//����ṹ�壬��3�����������ڿ�����䷭��ʱ����ת��ַ������ 
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


//**************����ִ�� **************************************** 
//������
string opcode[]{"_",
	"INT","LIT","LOD","STO",//4
	"call","ret","read","write",//4
	"minus",			// 1 
	"+","-","*","/",//4
	"j","j<","j<=","j>","j>=",//5
	"j=","j#","jz","jnz"//4 
}; int opnum=23;
int IR=0,//��ŵ�ǰָ��ĵ�ַ
	PC,//��һ��ָ��ĵ�ַ
	T=-1,//ջ���Ĵ�����ʼ��ָ��ջ�� 
	B;//����ַ�Ĵ��� 
	
int S[1000*5];//����һ��һά����������Ϊ����ʱջ 
	
	
int	lastpro;//��һ������ 
int  curpro;//��ǰ�����Ĺ�����ڵ�ַ����ʶ��һ����
bool isrecursion;//�Ƿ��ǵݹ���� 
int recursionlen;//�ݹ������ȣ�
int fdglen;//�ǵݹ���� 

void exec(){
	cout<<"��ʼִ��\n";
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
			//  ����һ���µĹ��̣���Ҫ 
		//	������ǰ ����
				lastpro=curpro;
				curpro=IR; 
				T+=ir->n3;
	//			cout<<"���̵��õ���ϵ��Ԫ��  ";showS();  
				break;
			}
			case 2:{// LIT
				S[++T]=ir->n3;
		//		cout<<"ջ����"<<S[T]<<endl;
				break;
			}
			case 3:{// LOD
				// ȡ�������ŵ�ջ��
				int add=B,//һ�����̵Ļ���ַ 
					lv=ir->L;//��� 
		//		if(dglv>0 && lv!=0)
		//			lv+=dglv;
				while(lv!=0){
					add=S[add+1];		//��̬����ȥ����̬��� 
					lv--;
				} 		
				S[T+1]=S[add+ir->n3];//  ȡ�����ŵ�ջ��������ַ+ƫ���� 
				T++;
		//	cout<<"ջ����"<<S[T]<<endl;	showS();
				break;
			}
			case 4:{// STO
				// ջ�� �������
				int add=B,//һ�����̵Ļ���ַ 
					lv=ir->L;//��� 
		//		if(dglv>0 && lv!=0)
	//				lv+=dglv;
				while(lv!=0){
					add=S[add+1];		//��̬����ȥ����̬��� 
					lv--;
				} 
		//		cout<<"\t\t\tdizhi :"<<add<<" "<<ir->n3<<".....";
				S[add+ir->n3]=S[T--];//  ջ���������������ַ+ƫ���� 
	//		cout<<"ջ����"<<S[T]<<endl;	showS();
				break;
			}
			case 5:{// CALL
				//���̵���	
			//��д������ϵ��Ԫ�����ص�ַ����̬������̬�� 
//   ��̬��   �����ù��̵ľ�̬���ģ�����ַ����ʵ���ǻ�ַ�Ĵ���B��ֵ 
//   ��̬��   �����ù��̵ľ�̬���ģ���������ָ�룬��ʵ����ջ��ָ��Ĵ���T��ֵ
//��Ϊ 
//   ��̬��   �����ù����ڶ���ʱ����㣬��Ҫ��ǰ����
//   ��̬��   �����ù��̵ľ�̬���ģ�����ַ����ʵ���ǻ�ַ�Ĵ���B��ֵ 

				S[T+1]=PC;//��ŷ��ص�ַ
		//		S[T+2]=B;//��̬��
		//		S[T+3]=T;//��̬�� 
				int jdl=0;		//�Ǳ����õ���������ڶ���ʱ�������̵���ڵ�ַ 
				bool meetret=false;
				for(int i=ir->n3;i>0;i--){
					if(assembly[i]->op==1 &&!meetret){
						jdl=i;break;
					}else if(assembly[i]->op==1 && meetret){//����һ��ret
						meetret=false;
					}else if(assembly[i]->op==6){//����һ��ret
						meetret=true;
					} 
				} 
				if(jdl==S[B]){//��ʾ���ù��̶���ʱ�����ͱ�����ʱ�������ͬһ�� 
					//���ԣ��侲̬��=������ڵ�ַ������ַB
					S[T+2]=B;//��̬��	 
				}else{//�����䣬��̬���̬���ľ�̬�� 
					S[T+2]=S[B+1];//��̬��
				} 
				S[T+3]=B;//��̬�� 
				// 
				PC= ir->n3;//�����ù��̵�Ŀ����������͸�PC
				B=T+1;    // �����ù��̵Ļ���ֵַ�����ַ�Ĵ���B 
				// Tջ��ָ��Ĵ�����ֵ���ڹ����ڲ������ٿռ�ʱ�޸� 
	//  �ݹ���ò������µĲ�Σ����ȡ����ֵ��������	
		//		dglv++;
	//  �������ַ�����Եݹ���Ч����Ե��������ٵ��ã��ͻ᷸��			
			//	����Ҳ��Ч 
		//		cout<<T<<"  call:"<<S[T+1]<<" - "<<S[T+2]<<" - "<<S[T+3]<<endl;
	//  ���ǻ��д��󣬵��õĲ��		
				break;
			}
			case 6:{// RET
				//  ���̷���
				PC=S[B+0]; 
				T=B-1;//S[B+2];
				B=S[B+2];
			//	dglv--;
				break;
			}
			case 7:{//READ
				cout<<"please input an interger:";
				int x;cin>>x;
				// ����һ���������������
				//��ʵ���Ը�Ϊ����ָ�� ��һ��������һ��sto 
				int add=B,//һ�����̵Ļ���ַ 
					lv=ir->L;//��� 
	//			if(dglv>0 && lv!=0)
	//				lv+=dglv;
				while(lv!=0){
					add=S[add+1];		//��̬����ȥ����̬��� 
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
			//	��������ת 
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

// ���������ֱ�reserve(�ñ�ͬ����ߵ��ʱ����)��
//���б����Ӧ���ʱ�kw ,�������optab 
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


//********************������ *************************************** 
//�ʷ�����------������ 
void lexerror(int i=0,string s=""){
	cout<<"lexing ERROR! \n";
	if(i){
		cout<<"i-6:"<<i-6<<s[i-6]<<s[i-5]<<s[i-4]<<s[i-3]<<s[i-2]<<s[i-1];
		cout<<i<<"--"<<s[i]<<s[i+1]<<s[i+2]<<s[i+3]<<s[i+4]<<s[i+5]<<s[i+6]<<s[i+7]<<s[i+8]<<s[i+9]; 
	}
	exit(0);
} 

//�﷨����------������  
void parserror();
//�������------������ 
void semanticserror();


//*******************�����ʾ ***********************************
//�����ʾ�������� �Ͳ������ 
void show_word_coding_table(){
	cout<<"    ����	�����\n";
	for(int i=0;i<kwsize;i++)
		cout<<setw(10)<<keyword[i]<<"	 "<< reserve[keyword[i]]<<endl;
	cout<<"  identifier  	 "<<identifier<<endl;
	cout<<"  constant 	 "<<constant<<endl;
	//�������
	cout<<" ����		������\n";
	for(int i=1;i<opnum;i++) {
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
	cout<<"����"<<now->name<<"   num:"<<now->itemnum<<endl;
	return;
} 

//����м������Ԫʽ 
void showass(){
	cout.setf(std::ios::left);
	cout<<"�м������Ԫʽ��\n";
	cout<<"/* L ��  aƫ����*/\n"; 
	cout<<"��Ԫʽ�任	  ������	������  l	 a \n";	
	
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

//�������ջ 
void showS(){
	for(int i=0;i<=T;i++) 
		cout<<S[i]<<" ";
	cout<<"\n";
} 




