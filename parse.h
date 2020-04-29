/*
递归下降法，语法分析 
依据各产生式，定义递归子程序

输入，词法分析产生的单词序列
head  
*/ 
#include "pl0.h"

//访问单词序列的迭代器 
seq* cur;
int lev;//全局变量，标明当前数据的level 
stack<symtab*> tabptr;//存放table的层次 
stack<int> offset;//记录一个过程中变量的相对地址初值定为3，（见指导书 
//创建语法树的存储结构
struct ptree{//语法树节点结构体 
	string name;//非终结符 
	seq* node;//单词符号指针  （叶子节点才是单词） 
	int level;//节点所在层次 
	ptree* father;
	ptree* firstchild; 
	ptree* nextchild;
	
	ptree(){
		name=""; 
		node=NULL;
		father=firstchild=nextchild=NULL;
	} 
	ptree(ptree* FATHER,string NAME){//创建一个非终结符节点 
		name=NAME;
		father=FATHER;
		node=NULL;
		firstchild=nextchild=NULL;
	}
	ptree(seq* n1,ptree* n2){//创建一个终结符节点 
		node=n1;
		father=n2;
		firstchild=nextchild=NULL;
	}
}
*root,*curnode,//当进入子程序时，保证curnode为当前子程序的节点 
*leaf,*curleaf; 
//定义一个链表，链接所有的叶子

void printnode(ptree*nd){ //打印语法树的一个节点 
	if(nd->name=="" && nd->node==NULL)	cout<<"   ";//一个子树 
	else if(nd->firstchild){
		cout<<nd->name<<" ";
	}else {//为终结符 
		if(nd->node->k ==identifier||nd->node->k ==constant){
			if(nd->node->k==constant &&strtoi(nd->node->v)!=-1)	
				cout<<num[strtoi(nd->node->v)] <<"  ";
			else cout<<nd->node->v <<"  ";	
		}else  
			cout<<kw[nd->node->k]<<" ";
	}
}
 
//语法树的遍历，后序遍历（左右根） 
void postorder(ptree *root){
	ptree *cur;
	if(root){
		cur=root->firstchild;
		//遍历孩子 
		if(!cur){//无孩子，是叶子 
			if(root->node->k ==(identifier|constant)){
				if(root->node->k==constant &&strtoi(root->node->v)!=-1)	
					cout<<num[strtoi(root->node->v)] <<" >\n";
				else cout<<root->node->v <<" >\n";	
			}else  cout<<kw[root->node->k]<<" ";
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
//这里表示把一个非终结符的所有孩子入队 应该空一大格子	t=new ptree;q.push(t); 
			}
		}
		cout<<endl;
		p.swap(q);
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
			showtab(x);
			x=x->next;
		}
		it=it->next;
	}
	
} 

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

//语法分析，错误提示函数 
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

void link2father(ptree*node ){//链接父亲节点node指向孩子curnode 
	if(!node->firstchild){//当前节点还没有孩子 
		node->firstchild=curnode; 
	}else{
		ptree *lastnode=node->firstchild;
		while(lastnode->nextchild)	lastnode=lastnode->nextchild;
		lastnode->nextchild=curnode;
	}
}

void P(){//主程序 
	root=new ptree(NULL,"P");//创建语法树的根
	
	table=new symtab("main");//创建主table表 
	tabptr.push(table); 
	offset.push(3); 
	cur=head; 				//单词序列迭代器初始化 
	
	curnode=new ptree(root,"SP");
	link2father(root);
	SP();
//到这里，主过程过程结束了，需要出栈
	tabptr.pop();
	offset.pop(); 	
	
	cout<<"success!!!\n";
}
void SP(){//分程序 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k] =="const"){//存在常量说明部分  
		curnode=new ptree(node,"C");//curnode为node的孩子 
		link2father(node);
		C();	
	} 
	if(kw[cur->k]=="var"){//存在变量说明部分
		curnode=new ptree(node,"V");//curnode为node的孩子 
		link2father(node);
		V();
	} 
	if(kw[cur->k]=="procedure"){//存在过程说明部分
		curnode=new ptree(node,"PRO");//curnode为node的孩子 
		link2father(node);
		PRO();
	} 
	curnode=new ptree(node,"SEN");//curnode为node的孩子 
	link2father(node);
	SEN(); 
	//规约 
}


void C(){//常量说明部分 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k] =="const"){
		curnode=new ptree(cur,node);//创建一个叶子节点
		link2father(node);
		cur=cur->next;
		
		curnode=new ptree(node,"CD");//创建一个CD节点 
		link2father(node);
		CD();
		while(kw[cur->k]==",") {
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			curnode=new ptree(node,"CD");//创建一个CD节点 
			link2father(node);
			CD();
		}
		if(kw[cur->k]==";"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
		} 
		else parserror();
	}else parserror();		
} 
void CD(){//常量定义 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		string cdname=cur->v;	// 记下该常量的名字 
		cur=cur->next;
		if(kw[cur->k]=="=")	{
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			if(kw[cur->k]=="constant"){
				curnode=new ptree(cur,node);//创建一个叶子节点,
				link2father(node);
				//得到一个完整的常量定义，把它加入当前table表中
				tabptr.top()->additem(mkitem(cdname,cur->k,lev,num[strtoi(cur->v)] )) ;
				cur=cur->next;
			}else parserror();
		}else parserror();
	}else parserror(); 
} 

void V(){//变量说明部分 ,关于变量层次说明，放在语义分析时再对程序进行修改 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="var"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		int kind=cur->k;//记录类别 
		cur=cur->next;
		if(kw[cur->k]=="identifier"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
	//得到一个完整的变量定义，把它加入当前table表中
			tabptr.top()->additem(mkitem(cur->v,kind,lev,0, offset.top())) ;
			offset.top()+=4;//4表示当前变量的字节宽度 
			cur=cur->next;
			while(kw[cur->k]==","){
					curnode=new ptree(cur,node);//创建一个叶子节点,
					link2father(node);
					cur=cur->next;
				if(kw[cur->k]=="identifier"){
					curnode=new ptree(cur,node);//创建一个叶子节点,
					link2father(node);
		//得到一个完整的变量定义，把它加入当前table表中
					tabptr.top()->additem(mkitem(cur->v,kind,lev,0, offset.top()));
					offset.top()+=4;//4表示当前变量的字节宽度 	
					cur=cur->next;
				}
				else parserror();
			}
			if(kw[cur->k]==";"){
				curnode=new ptree(cur,node);//创建一个叶子节点,
				link2father(node);
				cur=cur->next;
			}
			else parserror();
		}else parserror();
	}else parserror();
} 

void PRO(){//过程说明部分 

	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	curnode=new ptree(node,"PRPHEAD");//创建一个非终结符节点,
	link2father(node);
	PRPHEAD();
	curnode=new ptree(node,"SP");//创建一个非终结符节点,
	link2father(node);
	SP();
	if(kw[cur->k]==";"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}
	else parserror();
//到这里，一个过程结束了，需要出栈
	tabptr.pop();
	offset.pop(); 
	lev-=1;//层次-1； 
	while(kw[cur->k]=="procedure"){
			curnode=new ptree(node,"PRO");//创建一个非终结符节点,
			link2father(node);
			PRO();
	} 
} 

void PRPHEAD(){//过程首部 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="procedure"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		int kind=cur->k;//记录类型 
		cur=cur->next;
		if(kw[cur->k]=="identifier"){
			//一个过程名 
	//得到一个完整的过程名称定义，把它加入当前table表中
	//对于过程名的ADR域，是在过程体的目标代码生成后返填过程体的入口地址。
	//所以这里暂时不写 
			tabptr.top()->additem(mkitem(cur->v,kind,lev)) ;
		//	offset.top()+=4;//4表示当前变量的字节宽度 
	
	//then进入下一个过程，新建一个表 
			symtab* newtab=new symtab(cur->v,tabptr.top());
	//并在上层表中建立指针指向该表；
			tabptr.top()->ptrnext(newtab);
	//进栈 
			lev+=1;//进入新的过程 
			tabptr.push(newtab);
			offset.push(3);	 
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			if(kw[cur->k]==";"){
				curnode=new ptree(cur,node);//创建一个叶子节点,
				link2father(node);
				cur=cur->next;
			}else parserror();
		}else parserror();	
	}else parserror();
	
} 

void SEN(){//语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(node,"ASS");//创建一个非终结符节点,
		link2father(node);
		ASS();
	}else if(kw[cur->k]=="begin"){
		curnode=new ptree(node,"FH");//创建一个非终结符节点,
		link2father(node);
		FH();
	}else if(kw[cur->k]=="if"){
		curnode=new ptree(node,"COND");//创建一个非终结符节点,
		link2father(node);
		COND();
	}else if(kw[cur->k]=="call"){
		curnode=new ptree(node,"PROCALL");//创建一个非终结符节点,
		link2father(node);
		PROCALL();
	}else if(kw[cur->k]=="while"){
		curnode=new ptree(node,"LOOP");//创建一个非终结符节点,
		link2father(node);
		LOOP();
	}else if(kw[cur->k]=="read"){
		curnode=new ptree(node,"DU");//创建一个非终结符节点,
		link2father(node);
		DU();
	}else if(kw[cur->k]=="write"){
		curnode=new ptree(node,"XIE");//创建一个非终结符节点,
		link2father(node);
		XIE();
	}else {//这里表明这个语句推空 
		cout<<"kong..."<<kw[cur->k]<<"...."<<endl;
		curnode=new ptree;//创建一个空节点,
		link2father(node);
	}
} 

void ASS(){//赋值语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]==":="){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"BDS");//创建一个非终结符节点,
			link2father(node);
			BDS();
		}else parserror();
	}else parserror();
} 

void FH(){//复合语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="begin"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"SEN");//创建一个非终结符节点,
		link2father(node);
		SEN();
		while(kw[cur->k]==";"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"SEN");//创建一个非终结符节点,
			link2father(node);
			SEN();	
		}
		if(kw[cur->k]=="end"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
		}else parserror();
	}else parserror();
} 

void CDI(){//条件 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="odd"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();
	}else {
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();	
		curnode=new ptree(node,"RELA");//创建一个非终结符节点,
		link2father(node);
		RELA();
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();
	}
} 

void BDS(){//表达式 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="+"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if( kw[cur->k]=="-"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}	
	curnode=new ptree(node,"TERM");//创建一个非终结符节点,
	link2father(node);
	TERM();
	while(kw[cur->k]=="+"| kw[cur->k]=="-"){
		curnode=new ptree(node,"PS");//创建一个非终结符节点,
		link2father(node);
		PS();
		curnode=new ptree(node,"TERM");//创建一个非终结符节点,
		link2father(node);
		TERM();
	}
} 

void TERM(){//项 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	curnode=new ptree(node,"FAC");//创建一个非终结符节点,
	link2father(node);
	FAC();
	while(kw[cur->k]=="*"| kw[cur->k]=="/"){
		curnode=new ptree(node,"MULD");//创建一个非终结符节点,
		link2father(node);
		MULD();
		curnode=new ptree(node,"FAC");//创建一个非终结符节点,
		link2father(node);
		FAC(); 
	} 
} 

void FAC(){//因子 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="constant"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="("){
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();
		if(kw[cur->k]==")"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
		}
		else parserror();
	}else parserror();
} 

void COND(){//条件语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="if"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"CDI");//创建一个非终结符节点,
		link2father(node);
		CDI();
		if(kw[cur->k]=="then"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"SEN");//创建一个非终结符节点,
			link2father(node);
			SEN();
		}else parserror();
	}else parserror();
} 

void PROCALL(){//过程调用语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="call"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]=="identifier"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
		}
		else parserror();
	}else parserror();
} 

void LOOP(){//当型循环语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="while"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"CDI");//创建一个非终结符节点,
		link2father(node);
		CDI();
		if(kw[cur->k]=="do"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"SEN");//创建一个非终结符节点,
			link2father(node);
			SEN();
		}else parserror();
	}else parserror();
} 

void DU(){//读语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="read"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]=="("){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			if(kw[cur->k]=="identifier"){
				curnode=new ptree(cur,node);//创建一个叶子节点,
				link2father(node);
				cur=cur->next;
				while(kw[cur->k]==","){
					curnode=new ptree(cur,node);//创建一个叶子节点,
					link2father(node);
					cur=cur->next;
					if(kw[cur->k]=="identifier"){
						curnode=new ptree(cur,node);//创建一个叶子节点,
						link2father(node);
						cur=cur->next;
					}else parserror();
				}
				if(kw[cur->k]==")"){
					curnode=new ptree(cur,node);//创建一个叶子节点,
					link2father(node);
					cur=cur->next;
				}else parserror();
			}else parserror();
		}else parserror();
	}else parserror();
} 
void XIE(){//写语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="write"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		if(kw[cur->k]=="("){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"BDS");//创建一个非终结符节点,
			link2father(node);
			BDS(); 
			while(kw[cur->k]==","){
				curnode=new ptree(cur,node);//创建一个叶子节点,
				link2father(node);
				cur=cur->next;
				curnode=new ptree(node,"BDS");//创建一个非终结符节点,
				link2father(node);
				BDS(); 
			}
			if(kw[cur->k]==")"){
				curnode=new ptree(cur,node);//创建一个叶子节点,
				link2father(node);
				cur=cur->next;
			}else parserror();
		}else parserror();
	}else parserror();
} 

void PS(){//加减运算符 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="+"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="-"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else parserror();
} 
void MULD(){//乘除运算符 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="*"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="/"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else parserror();
} 

void RELA(){//关系运算符 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="="){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="#"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]==">"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]==">="){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="<"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if(kw[cur->k]=="<="){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else parserror();
} 













