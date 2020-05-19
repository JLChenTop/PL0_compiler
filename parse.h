/*
�ݹ��½������﷨���� 
���ݸ�����ʽ������ݹ��ӳ���

���룬�ʷ����������ĵ�������
head  

������� 
*/ 
#include "pl0.h"

//���ʵ������еĵ����� 
seq* cur;
int lev;//ȫ�ֱ�����������ǰ���ݵ�level 
stack<symtab*> tabptr;//���table�Ĳ�� 
//tabptr.top()  ָ��ǰ���ڵ�table�� 
stack<int> offset;//��¼һ�������б�������Ե�ַ��ֵ��Ϊ3������ָ���� 


void printnode(ptree*nd){ //��ӡ�﷨����һ���ڵ� 
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

//�������error 
void semanticserror(){
	cout<<"semantics error!!!\n";
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

void link2father(ptree *fathernode ){//���Ӹ��׽ڵ�nodeָ���ӣ�Ĭ�ϣ�curnode 
	if(!fathernode->firstchild){//��ǰ�ڵ㻹û�к��� 
		fathernode->firstchild=curnode; 
	}else{
		ptree *lastnode=fathernode->firstchild;
		while(lastnode->nextchild)	lastnode=lastnode->nextchild;
		lastnode->nextchild=curnode;
	}
}

void P(){//������ 
	root=new ptree(NULL,"P");//�����﷨���ĸ�
	table=new symtab("main",lev);//������table�� 
	tabptr.push(table); //���table�Ĳ�� 
	offset.push(3); 
//	cout<<"test!!   \n"; 
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
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;		
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
	//��Լ 
	
	fnode=node;
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
			offset.top()+=4;//4��ʾ��ǰ�������ֽڿ�� 
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
					offset.top()+=4;//4��ʾ��ǰ�������ֽڿ�� 	
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
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
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
		//	offset.top()+=4;//4��ʾ��ǰ�������ֽڿ�� 
	
	//then������һ�����̣��½�һ���� 
			symtab* newtab=new symtab(cur->v,lev+1,tabptr.top());
	//�����ϲ���н���ָ��ָ��ñ�
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
	//��Լ 
	
	fnode=node;	
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
		cout<<"����ƿ�"<<cur->sn<<" "<<kw[cur->k]<<"...."<<endl;
		curnode=new ptree;//����һ���սڵ�,
		link2father(node);
	}
	//��Լ 
	node->nextlist=NULL; 
	
	fnode=node;
} 

void ASS(){//��ֵ��� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="identifier"){
//���븳ֵ��䣺
		//���Ȳ��ұ�ʶ���Ƿ���� ��table���в��� 
		tableitem* id= findintab(tabptr.top(),cur->v);
		if(id==NULL)	semanticserror(); 
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		ptree *idnode=curnode;//ָ��Ҷ�ӽڵ� 
		if(kw[cur->k]==":="){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
			link2father(node);
			BDS();
		//������BDS��Ȼ����������ڵ㣬��fnode,���ʽ��ֵ�������� 
		
		//����һ���м����
			assembly[nextquad++]=
			new quat(optab[":="],fnode,NULL,idnode);	
		}else parserror();
	}else parserror();
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
} 

void CDI(){//����   ����Ĳ������ʽ���߼���ֻ��һ���߼��ж� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="odd"){//��ʾbdsֵ
	//odd bds���������ʽ����0Ϊ�棬����Ϊ0��Ϊ�� 
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();
//����
		node->truelist=mkslist(nextquad);
		node->falselist=mkslist(nextquad+1);
	//�����м����    //����Ҫ�����δ���ʱ��֪����
		assembly[nextquad++]=new quat(optab["jnz"],fnode,NULL,-1);
		assembly[nextquad++]=new quat(optab["j"],NULL,NULL,-1);		
	}else {
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();	
		ptree *s1=fnode; 
		curnode=new ptree(node,"RELA");//����һ�����ս���ڵ�,
		link2father(node);
		RELA();
		ptree *relopnode=fnode; 
		//�¼�һ���սڵ�  ���ﲻ��Ҫ��������߼��ж����ʱʹ�� 
	//	curnode=new ptree;//����һ���սڵ�,
	//	link2father(node);
	//	curnode->quad=nextquad;
		 
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();
		ptree *s2=fnode; 
//����
		node->truelist=mkslist(nextquad);
		node->falselist=mkslist(nextquad+1);
	//�����м����    //����Ҫ�����δ���ʱ��֪����
		string jump="j"+kw[relopnode->firstchild->wnode->k];
		assembly[nextquad++]=new quat(optab[jump],s1,s2,-1);
		assembly[nextquad++]=new quat(optab["j"],NULL,NULL,-1);	
	}
	//��Լ 
	
	fnode=node;
} 

void BDS(){//���ʽ 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	bool neg=false; //cout<<"test !!!\n";
	if(kw[cur->k]=="+"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
	}else if( kw[cur->k]=="-"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		//˵����һ����һ�������Ҫ����-1
		neg=true; 
	}	
	curnode=new ptree(node,"TERM");//����һ�����ս���ڵ�,
	link2father(node);
	TERM(); 
	ptree *n1=fnode;
	bool isleaf=true;//����������ʽ�Ƿ�ֱ��ָ��һ��TERM 
	if(neg){//����  ��fnode��n1ȡ����node     !!!!!!!!���ﻹ��Ҫ��˼��һ�£���bdsָ��Ҷ�Ӻ����������ô�� 
	//	node->value=n1->value*(-1);
		if(n1->wnode){//n1��һ��Ҷ��
		 
		}
		assembly[nextquad++]=
		new quat(optab["minus"],n1,NULL,n1);
	} else{
		node->value=n1->value;//��ǰ�ڵ��ֵ	
	}
	bool isonetime=true;//�ж������Ƿ�Ϊһ�����Ƕ���Ӽ�����
	while(kw[cur->k]=="+"| kw[cur->k]=="-"){
		isleaf=false;  //bds�ڵ㲻��ֻ��һ������ 
		curnode=new ptree(node,"PS");//����һ�����ս���ڵ�,
		link2father(node);
		PS();
		ptree *psnode=fnode;//�Ӽ����Žڵ� 
		curnode=new ptree(node,"TERM");//����һ�����ս���ڵ�,
		link2father(node);
		TERM();
	//	cout<<psnode->firstchild->wnode->sn<<"..";
	//	cout<<"test+-  :"<<kw[psnode->firstchild->wnode->k]<<" \n";
		if(isonetime){
			isonetime=false;
			assembly[nextquad++]=
			new quat(optab[kw[psnode->firstchild->wnode->k]],n1,fnode,node);
		} else{
			assembly[nextquad++]=
			new quat(optab[kw[psnode->firstchild->wnode->k]],node,fnode,node);
			}	
		}
	if(isleaf){	//��һ�����ʽʵ����ֻ��һ��Ҷ�ӣ��򲻽��������while 
//����bdsֻ���г˳����޼Ӽ�����bds�ڵ� ��term �ڵ�ʵ���Ͽ���ͬһ���ռ�
//��������fnode �Ͳ�ָ���Լ�����ָ���亢��term 
		node->wnode=n1->wnode; 
		fnode=n1; 
	}else  fnode=node;
	//��Լ 
} 

void TERM(){//�� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	curnode=new ptree(node,"FAC");//����һ�����ս���ڵ�,
	link2father(node);
	FAC();
	ptree *n1=fnode;
	bool isleaf=false;
	if(n1->wnode)	isleaf=true;//���fac�Ǹ�Ҷ�� 
	node->value=n1->value;//��ǰ�ڵ��ֵ
	bool isonetime=true;//�ж������Ƿ�Ϊһ�����Ƕ���˳�����	
	while(kw[cur->k]=="*"| kw[cur->k]=="/"){
		isleaf=false;
		curnode=new ptree(node,"MULD");//����һ�����ս���ڵ�,
		link2father(node);
		MULD();
		ptree *muldnode=fnode;//�˳����Žڵ� 
		curnode=new ptree(node,"FAC");//����һ�����ս���ڵ�,
		link2father(node);
		FAC(); 
		if(isonetime){
			isonetime=false;
			assembly[nextquad++]=
		new quat(optab[kw[muldnode->firstchild->wnode->k]],n1,fnode,node);
		} else
		assembly[nextquad++]=
		new quat(optab[kw[muldnode->firstchild->wnode->k]],node,fnode,node);
	} 
	if(isleaf)	//��һ��ʵ����ֻ��һ��Ҷ�ӣ��򲻽��������while 
		node->wnode=n1->wnode; 
	//��Լ 
	fnode=node;
} 

void FAC(){//���� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
	//	cout<<"test  :"<<node->nodeseq<<endl;
		node->wnode=cur;		//��Ϊ�Ǹ�Ҷ�ӣ����������facҲָ��Ҷ�� 
		//ȥtable��ȡ���ݵ�ֵ 
//���Ȳ��ұ�ʶ���Ƿ���� ��table���в��� 
		tableitem* id= findintab(tabptr.top(),cur->v);
		if(id==NULL)	semanticserror(); 
		node->value=id->val; 
		cur=cur->next;
	}else if(kw[cur->k]=="constant"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		node->wnode=cur;		//��Ϊ�Ǹ�Ҷ�ӣ����������facҲָ��Ҷ�� 
//���Ȳ��ұ�ʶ���Ƿ���� ��table���в��� 
		node->value=num[strtoi(cur->v)]; 
		cur=cur->next;
	}else if(kw[cur->k]=="("){
		curnode=new ptree(node,"BDS");//����һ�����ս���ڵ�,
		link2father(node);
		BDS();
		node->value=fnode->value;//��ǰ�ڵ�ֵ=bds��ֵ 
		if(kw[cur->k]==")"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
		}
		if(fnode->wnode){//���bdsָ��һ��Ҷ��
			node->wnode=fnode->wnode; 
		}	
		else parserror();
	}else parserror();
	//��Լ 
	
	fnode=node;
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
		ptree *cdinode=fnode;
		if(kw[cur->k]=="then"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			//�¼�һ���սڵ�  M
			curnode=new ptree;//����һ���սڵ�,
			link2father(node);
			curnode->quad=nextquad;
			ptree *mnode=curnode;
			
			curnode=new ptree(node,"SEN");//����һ�����ս���ڵ�,
			link2father(node);
			SEN();
	//����
			backpatch(cdinode->truelist,mnode->quad);
			node->nextlist=mergelist(cdinode->falselist,fnode->nextlist);		
		}else parserror();
		
	}else parserror();
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
} 

void LOOP(){//����ѭ����� 
	ptree *node=curnode;	//nodeΪ���õ�ǰ����ӳ���ʱ�����Ľڵ� 
	if(kw[cur->k]=="while"){
		curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
		link2father(node);
		cur=cur->next;
		
		curnode=new ptree;//����һ���սڵ�,m1
		link2father(node);
		curnode->quad=nextquad;
		ptree *m1node=curnode;
		
		curnode=new ptree(node,"CDI");//����һ�����ս���ڵ�,
		link2father(node);
		CDI();
		ptree *cdinode=fnode;
		if(kw[cur->k]=="do"){
			curnode=new ptree(cur,node);//����һ��Ҷ�ӽڵ�,
			link2father(node);
			cur=cur->next;
			
			curnode=new ptree;//����һ���սڵ�,m2
			link2father(node);
			curnode->quad=nextquad;
			ptree *m2node=curnode;
			
			curnode=new ptree(node,"SEN");//����һ�����ս���ڵ�,
			link2father(node);
			SEN();
	//����
			backpatch(fnode->nextlist,m1node->quad);
			backpatch(cdinode->truelist,m2node->quad);
			node->nextlist=cdinode->falselist;
			//����һ���м���� 
			assembly[nextquad++]=new quat(optab["j"],NULL,NULL,m1node->quad);			
		}else parserror();
	}else parserror();
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
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
	//��Լ 
	
	fnode=node;
} 













