/*
�ݹ��½������﷨���� 
���ݸ�����ʽ������ݹ��ӳ���

���룬�ʷ����������ĵ�������
head  
*/ 
#include "pl0.h"

//���ʵ������еĵ����� 
seq* cur;
int lev;//ȫ�ֱ�����������ǰ���ݵ�level 
stack<symtab*> tabptr;//���table�Ĳ�� 
stack<int> offset;//��¼һ�������б�������Ե�ַ��ֵ��Ϊ3������ָ���� 
//�����﷨���Ĵ洢�ṹ
struct ptree{//�﷨���ڵ�ṹ�� 
	string name;//���ս�� 
	seq* node;//���ʷ���ָ��  ��Ҷ�ӽڵ���ǵ��ʣ� 
	int level;//�ڵ����ڲ�� 
	ptree* father;
	ptree* firstchild; 
	ptree* nextchild;
	
	ptree(){
		name=""; 
		node=NULL;
		father=firstchild=nextchild=NULL;
	} 
	ptree(ptree* FATHER,string NAME){//����һ�����ս���ڵ� 
		name=NAME;
		father=FATHER;
		node=NULL;
		firstchild=nextchild=NULL;
	}
	ptree(seq* n1,ptree* n2){//����һ���ս���ڵ� 
		node=n1;
		father=n2;
		firstchild=nextchild=NULL;
	}
}
*root,*curnode,//�������ӳ���ʱ����֤curnodeΪ��ǰ�ӳ���Ľڵ� 
*leaf,*curleaf; 
//����һ���������������е�Ҷ��

void printnode(ptree*nd){ //��ӡ�﷨����һ���ڵ� 
	if(nd->name=="" && nd->node==NULL)	cout<<"   ";//һ������ 
	else if(nd->firstchild){
		cout<<nd->name<<" ";
	}else {//Ϊ�ս�� 
		if(nd->node->k ==identifier||nd->node->k ==constant){
			if(nd->node->k==constant &&strtoi(nd->node->v)!=-1)	
				cout<<num[strtoi(nd->node->v)] <<"  ";
			else cout<<nd->node->v <<"  ";	
		}else  
			cout<<kw[nd->node->k]<<" ";
	}
}
 
//�﷨���ı�����������������Ҹ��� 
void postorder(ptree *root){
	ptree *cur;
	if(root){
		cur=root->firstchild;
		//�������� 
		if(!cur){//�޺��ӣ���Ҷ�� 
			if(root->node->k ==(identifier|constant)){
				if(root->node->k==constant &&strtoi(root->node->v)!=-1)	
					cout<<num[strtoi(root->node->v)] <<" >\n";
				else cout<<root->node->v <<" >\n";	
			}else  cout<<kw[root->node->k]<<" ";
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
//�����ʾ��һ�����ս�������к������ Ӧ�ÿ�һ�����	t=new ptree;q.push(t); 
			}
		}
		cout<<endl;
		p.swap(q);
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
			showtab(x);
			x=x->next;
		}
		it=it->next;
	}
	
} 

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
void BDS(); //����ʽ 
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

//�﷨������������ʾ���� 
void parserror(){
	cout<<"pasing error!!!\n";
	cout<<"< ";
		cout.width(3);  cout<<cur->sn<<",	";
		cout.width(10);  cout<<kw[cur->k]<<"	,	";
		cout.width(5);  cout<<cur->k<<"	,	";
		cout.width(5);  
	if(cur->k==constant &&strtoi(cur->v)!=-1)	
		cout<<num[strtoi(cur->v)] <<" >\n";
	else cout<<cur->v <<" >\n";
	exit(0);
}

void link2father(ptree*node ){//���Ӹ��׽ڵ�nodeָ����curnode 
	if(!node->firstchild){//��ǰ�ڵ㻹û�к��� 
		node->firstchild=curnode; 
	}else{
		ptree *lastnode=node->firstchild;
		while(lastnode->nextchild)	lastnode=lastnode->nextchild;
		lastnode->nextchild=curnode;
	}
}

void P(){//������ 
	root=new ptree(NULL,"P");//�����﷨���ĸ�
	
	table=new symtab("main");//������table�� 
	tabptr.push(table); 
	offset.push(3); 
	cur=head; 				//�������е�������ʼ�� 
	
	curnode=new ptree(root,"SP");
	link2father(root);
	SP();
//����������̹��̽����ˣ���Ҫ��ջ
	tabptr.pop();
	offset.pop(); 	
	
	cout<<"success!!!\n";
}
void SP(){//�ֳ��� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k] =="const"){//���ڳ���˵������  
		curnode=new ptree(node,"C");//curnodeΪnode�ĺ��� 
		link2father(node);
		C();	
	} 
	if(kw[cur->k]=="var"){//���ڱ���˵������
		curnode=new ptree(node,"V");//curnodeΪnode�ĺ��� 
		link2father(node);
		V();
	} 
	if(kw[cur->k]=="procedure"){//���ڹ���˵������
		curnode=new ptree(node,"PRO");//curnodeΪnode�ĺ��� 
		link2father(node);
		PRO();
	} 
	curnode=new ptree(node,"SEN");//curnodeΪnode�ĺ��� 
	link2father(node);
	SEN(); 
	//��Լ 
}


void C(){//����˵������ 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k] =="const"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�
		link2father(node);
		cur=cur->next;
		
		curnode=new ptree(node,"CD");//����һ��CD�ڵ� 
		link2father(node);
		CD();
		while(kw[cur->k]==",") {
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			curnode=new ptree(node,"CD");//����һ��CD�ڵ� 
			link2father(node);
			CD();
		}
		if(kw[cur->k]==";"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
		} 
		else parserror();
	}else parserror();		
} 
void CD(){//�������� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		string cdname=cur->v;	// ���¸ó��������� 
		cur=cur->next;
		if(kw[cur->k]=="=")	{
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			if(kw[cur->k]=="constant"){
				curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
				link2father(node);
				//�õ�һ�������ĳ������壬�������뵱ǰtable����
				tabptr.top()->additem(mkitem(cdname,cur->k,lev,num[strtoi(cur->v)] )) ;
				cur=cur->next;
			}else parserror();
		}else parserror();
	}else parserror(); 
} 

void V(){//����˵������ ,���ڱ������˵���������������ʱ�ٶԳ�������޸� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="var"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		int kind=cur->k;//��¼��� 
		cur=cur->next;
		if(kw[cur->k]=="identifier"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
	//�õ�һ�������ı������壬�������뵱ǰtable����
			tabptr.top()->additem(mkitem(cur->v,kind,lev,0, offset.top())) ;
			offset.top()+=4;//4��ʾ��ǰ�������ֽڿ��� 
			cur=cur->next;
			while(kw[cur->k]==","){
					curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
					link2father(node);
					cur=cur->next;
				if(kw[cur->k]=="identifier"){
					curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
					link2father(node);
		//�õ�һ�������ı������壬�������뵱ǰtable����
					tabptr.top()->additem(mkitem(cur->v,kind,lev,0, offset.top()));
					offset.top()+=4;//4��ʾ��ǰ�������ֽڿ��� 	
					cur=cur->next;
				}
				else parserror();
			}
			if(kw[cur->k]==";"){
				curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
				link2father(node);
				cur=cur->next;
			}
			else parserror();
		}else parserror();
	}else parserror();
} 

void PRO(){//����˵������ 

	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	curnode=new ptree(node,"PRPHEAD");//����һ�����ս���ڵ�,
	link2father(node);
	PRPHEAD();
	curnode=new ptree(node,"SP");//����һ�����ս���ڵ�,
	link2father(node);
	SP();
	if(kw[cur->k]==";"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}
	else parserror();
//�����һ�����̽����ˣ���Ҫ��ջ
	tabptr.pop();
	offset.pop(); 
	lev-=1;//���-1�� 
	while(kw[cur->k]=="procedure"){
			curnode=new ptree(node,"PRO");//����һ�����ս���ڵ�,
			link2father(node);
			PRO();
	} 
} 

void PRPHEAD(){//�����ײ� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="procedure"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		int kind=cur->k;//��¼���� 
		cur=cur->next;
		if(kw[cur->k]=="identifier"){
			//һ�������� 
	//�õ�һ�������Ĺ������ƶ��壬�������뵱ǰtable����
	//���ڹ�������ADR�����ڹ������Ŀ��������ɺ�����������ڵ�ַ��
	//����������ʱ��д 
			tabptr.top()->additem(mkitem(cur->v,kind,lev)) ;
		//	offset.top()+=4;//4��ʾ��ǰ�������ֽڿ��� 
	
	//then������һ�����̣��½�һ���� 
			symtab* newtab=new symtab(cur->v,tabptr.top());
	//�����ϲ���н���ָ��ָ��ñ���
			tabptr.top()->ptrnext(newtab);
	//��ջ 
			lev+=1;//�����µĹ��� 
			tabptr.push(newtab);
			offset.push(3);	 
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			if(kw[cur->k]==";"){
				curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
				link2father(node);
				cur=cur->next;
			}else parserror();
		}else parserror();	
	}else parserror();
	
} 

void SEN(){//��� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(node,"ASS");//����һ�����ս���ڵ�,
		link2father(node);
		ASS();
	}else if(kw[cur->k]=="begin"){
		curnode=new ptree(node,"FH");//����һ�����ս���ڵ�,
		link2father(node);
		FH();
	}else if(kw[cur->k]=="if"){
		curnode=new ptree(node,"COND");//����һ�����ս���ڵ�,
		link2father(node);
		COND();
	}else if(kw[cur->k]=="call"){
		curnode=new ptree(node,"PROCALL");//����һ�����ս���ڵ�,
		link2father(node);
		PROCALL();
	}else if(kw[cur->k]=="while"){
		curnode=new ptree(node,"LOOP");//����һ�����ս���ڵ�,
		link2father(node);
		LOOP();
	}else if(kw[cur->k]=="read"){
		curnode=new ptree(node,"DU");//����һ�����ս���ڵ�,
		link2father(node);
		DU();
	}else if(kw[cur->k]=="write"){
		curnode=new ptree(node,"XIE");//����һ�����ս���ڵ�,
		link2father(node);
		XIE();
	}else {//��������������ƿ� 
		cout<<"kong..."<<kw[cur->k]<<"...."<<endl;
		curnode=new ptree;//����һ���սڵ�,
		link2father(node);
	}
} 

void ASS(){//��ֵ��� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]==":="){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
			link2father(node);
			BDS();
		}else parserror();
	}else parserror();
} 

void FH(){//������� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="begin"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"SEN");//����һ�����ս���ڵ�,
		link2father(node);
		SEN();
		while(kw[cur->k]==";"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"SEN");//����һ�����ս���ڵ�,
			link2father(node);
			SEN();	
		}
		if(kw[cur->k]=="end"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
		}else parserror();
	}else parserror();
} 

void CDI(){//���� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="odd"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();
	}else {
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();	
		curnode=new ptree(node,"RELA");//����һ�����ս���ڵ�,
		link2father(node);
		RELA();
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();
	}
} 

void BDS(){//����ʽ 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="+"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if( kw[cur->k]=="-"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}	
	curnode=new ptree(node,"TERM");//����һ�����ս���ڵ�,
	link2father(node);
	TERM();
	while(kw[cur->k]=="+"| kw[cur->k]=="-"){
		curnode=new ptree(node,"PS");//����һ�����ս���ڵ�,
		link2father(node);
		PS();
		curnode=new ptree(node,"TERM");//����һ�����ս���ڵ�,
		link2father(node);
		TERM();
	}
} 

void TERM(){//�� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	curnode=new ptree(node,"FAC");//����һ�����ս���ڵ�,
	link2father(node);
	FAC();
	while(kw[cur->k]=="*"| kw[cur->k]=="/"){
		curnode=new ptree(node,"MULD");//����һ�����ս���ڵ�,
		link2father(node);
		MULD();
		curnode=new ptree(node,"FAC");//����һ�����ս���ڵ�,
		link2father(node);
		FAC(); 
	} 
} 

void FAC(){//���� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="constant"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="("){
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();
		if(kw[cur->k]==")"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
		}
		else parserror();
	}else parserror();
} 

void COND(){//������� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="if"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"CDI");//����һ�����ս���ڵ�,
		link2father(node);
		CDI();
		if(kw[cur->k]=="then"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"SEN");//����һ�����ս���ڵ�,
			link2father(node);
			SEN();
		}else parserror();
	}else parserror();
} 

void PROCALL(){//���̵������ 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="call"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]=="identifier"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
		}
		else parserror();
	}else parserror();
} 

void LOOP(){//����ѭ����� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="while"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"CDI");//����һ�����ս���ڵ�,
		link2father(node);
		CDI();
		if(kw[cur->k]=="do"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"SEN");//����һ�����ս���ڵ�,
			link2father(node);
			SEN();
		}else parserror();
	}else parserror();
} 

void DU(){//����� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="read"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]=="("){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			if(kw[cur->k]=="identifier"){
				curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
				link2father(node);
				cur=cur->next;
				while(kw[cur->k]==","){
					curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
					link2father(node);
					cur=cur->next;
					if(kw[cur->k]=="identifier"){
						curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
						link2father(node);
						cur=cur->next;
					}else parserror();
				}
				if(kw[cur->k]==")"){
					curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
					link2father(node);
					cur=cur->next;
				}else parserror();
			}else parserror();
		}else parserror();
	}else parserror();
} 
void XIE(){//д��� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="write"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]=="("){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
			link2father(node);
			BDS(); 
			while(kw[cur->k]==","){
				curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
				link2father(node);
				cur=cur->next;
				curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
				link2father(node);
				BDS(); 
			}
			if(kw[cur->k]==")"){
				curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
				link2father(node);
				cur=cur->next;
			}else parserror();
		}else parserror();
	}else parserror();
} 

void PS(){//�Ӽ������ 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="+"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="-"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else parserror();
} 
void MULD(){//�˳������ 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="*"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="/"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else parserror();
} 

void RELA(){//��ϵ����� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="="){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="#"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]==">"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]==">="){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="<"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="<="){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else parserror();
} 












