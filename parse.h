/*
递归下降法，语法分析 
依据各产生式，定义递归子程序

输入，词法分析产生的单词序列
head  

语义分析 
*/ 
#include "pl0.h"

//访问单词序列的迭代器 
seq* cur;
int lev;//全局变量，标明当前数据的level 
stack<symtab*> tabptr;//存放table的层次 
//tabptr.top()  指向当前所在的table表 
stack<int> offset;//记录一个过程中变量的相对地址初值定为3，（见指导书 


void printnode(ptree*nd){ //打印语法树的一个节点 
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

//语义分析error 
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

void link2father(ptree *fathernode ){//链接父亲节点node指向孩子（默认）curnode 
	if(!fathernode->firstchild){//当前节点还没有孩子 
		fathernode->firstchild=curnode; 
	}else{
		ptree *lastnode=fathernode->firstchild;
		while(lastnode->nextchild)	lastnode=lastnode->nextchild;
		lastnode->nextchild=curnode;
	}
}

void P(){//主程序 
	root=new ptree(NULL,"P");//创建语法树的根
	table=new symtab("main",lev);//创建主table表 
	tabptr.push(table); //存放table的层次 
	offset.push(3); 
//	cout<<"test!!   \n"; 
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
	
	fnode=node;
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
	//规约 
	
	fnode=node;		
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
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
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
			symtab* newtab=new symtab(cur->v,lev+1,tabptr.top());
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
	//规约 
	
	fnode=node;	
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
		cout<<"语句推空"<<cur->sn<<" "<<kw[cur->k]<<"...."<<endl;
		curnode=new ptree;//创建一个空节点,
		link2father(node);
	}
	//规约 
	node->nextlist=NULL; 
	
	fnode=node;
} 

void ASS(){//赋值语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="identifier"){
//翻译赋值语句：
		//首先查找标识符是否存在 到table表中查找 
		tableitem* id= findintab(tabptr.top(),cur->v);
		if(id==NULL)	semanticserror(); 
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		ptree *idnode=curnode;//指向叶子节点 
		if(kw[cur->k]==":="){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			curnode=new ptree(node,"BDS");//创建一个非终结符节点,
			link2father(node);
			BDS();
		//调用了BDS，然后我们这个节点，是fnode,表达式的值存在其中 
		
		//给出一条中间代码
			assembly[nextquad++]=
			new quat(optab[":="],fnode,NULL,idnode);	
		}else parserror();
	}else parserror();
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
} 

void CDI(){//条件   这里的布尔表达式，逻辑简单只有一个逻辑判断 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="odd"){//表示bds值
	//odd bds，即单表达式，非0为真，否则即为0，为假 
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();
//翻译
		node->truelist=mkslist(nextquad);
		node->falselist=mkslist(nextquad+1);
	//给出中间代码    //这里要跳往何处暂时不知道，
		assembly[nextquad++]=new quat(optab["jnz"],fnode,NULL,-1);
		assembly[nextquad++]=new quat(optab["j"],NULL,NULL,-1);		
	}else {
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();	
		ptree *s1=fnode; 
		curnode=new ptree(node,"RELA");//创建一个非终结符节点,
		link2father(node);
		RELA();
		ptree *relopnode=fnode; 
		//新加一个空节点  这里不需要，当多个逻辑判断组合时使用 
	//	curnode=new ptree;//创建一个空节点,
	//	link2father(node);
	//	curnode->quad=nextquad;
		 
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();
		ptree *s2=fnode; 
//翻译
		node->truelist=mkslist(nextquad);
		node->falselist=mkslist(nextquad+1);
	//给出中间代码    //这里要跳往何处暂时不知道，
		string jump="j"+kw[relopnode->firstchild->wnode->k];
		assembly[nextquad++]=new quat(optab[jump],s1,s2,-1);
		assembly[nextquad++]=new quat(optab["j"],NULL,NULL,-1);	
	}
	//规约 
	
	fnode=node;
} 

void BDS(){//表达式 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	bool neg=false; //cout<<"test !!!\n";
	if(kw[cur->k]=="+"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
	}else if( kw[cur->k]=="-"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		//说明下一项是一个负项即需要乘以-1
		neg=true; 
	}	
	curnode=new ptree(node,"TERM");//创建一个非终结符节点,
	link2father(node);
	TERM(); 
	ptree *n1=fnode;
	bool isleaf=true;//标明这个表达式是否直接指向一个TERM 
	if(neg){//负项  对fnode即n1取负给node     !!!!!!!!这里还需要再思考一下，把bds指向叶子后，这个负项怎么做 
	//	node->value=n1->value*(-1);
		if(n1->wnode){//n1是一个叶子
		 
		}
		assembly[nextquad++]=
		new quat(optab["minus"],n1,NULL,n1);
	} else{
		node->value=n1->value;//当前节点的值	
	}
	bool isonetime=true;//判断这里是否为一个还是多个加减符号
	while(kw[cur->k]=="+"| kw[cur->k]=="-"){
		isleaf=false;  //bds节点不是只有一个孩子 
		curnode=new ptree(node,"PS");//创建一个非终结符节点,
		link2father(node);
		PS();
		ptree *psnode=fnode;//加减符号节点 
		curnode=new ptree(node,"TERM");//创建一个非终结符节点,
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
	if(isleaf){	//这一个表达式实际上只是一个叶子，则不进入上面的while 
//即个bds只含有乘除，无加减，则bds节点 和term 节点实际上开辟同一个空间
//所以这里fnode 就不指向自己，而指向其孩子term 
		node->wnode=n1->wnode; 
		fnode=n1; 
	}else  fnode=node;
	//规约 
} 

void TERM(){//项 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	curnode=new ptree(node,"FAC");//创建一个非终结符节点,
	link2father(node);
	FAC();
	ptree *n1=fnode;
	bool isleaf=false;
	if(n1->wnode)	isleaf=true;//这个fac是个叶子 
	node->value=n1->value;//当前节点的值
	bool isonetime=true;//判断这里是否为一个还是多个乘除符号	
	while(kw[cur->k]=="*"| kw[cur->k]=="/"){
		isleaf=false;
		curnode=new ptree(node,"MULD");//创建一个非终结符节点,
		link2father(node);
		MULD();
		ptree *muldnode=fnode;//乘除符号节点 
		curnode=new ptree(node,"FAC");//创建一个非终结符节点,
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
	if(isleaf)	//这一项实际上只是一个叶子，则不进入上面的while 
		node->wnode=n1->wnode; 
	//规约 
	fnode=node;
} 

void FAC(){//因子 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="identifier"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
	//	cout<<"test  :"<<node->nodeseq<<endl;
		node->wnode=cur;		//因为是个叶子，所以让这个fac也指向叶子 
		//去table中取数据的值 
//首先查找标识符是否存在 到table表中查找 
		tableitem* id= findintab(tabptr.top(),cur->v);
		if(id==NULL)	semanticserror(); 
		node->value=id->val; 
		cur=cur->next;
	}else if(kw[cur->k]=="constant"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		node->wnode=cur;		//因为是个叶子，所以让这个fac也指向叶子 
//首先查找标识符是否存在 到table表中查找 
		node->value=num[strtoi(cur->v)]; 
		cur=cur->next;
	}else if(kw[cur->k]=="("){
		curnode=new ptree(node,"BDS");//创建一个非终结符节点,
		link2father(node);
		BDS();
		node->value=fnode->value;//当前节点值=bds的值 
		if(kw[cur->k]==")"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
		}
		if(fnode->wnode){//如果bds指向一个叶子
			node->wnode=fnode->wnode; 
		}	
		else parserror();
	}else parserror();
	//规约 
	
	fnode=node;
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
		ptree *cdinode=fnode;
		if(kw[cur->k]=="then"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			//新加一个空节点  M
			curnode=new ptree;//创建一个空节点,
			link2father(node);
			curnode->quad=nextquad;
			ptree *mnode=curnode;
			
			curnode=new ptree(node,"SEN");//创建一个非终结符节点,
			link2father(node);
			SEN();
	//翻译
			backpatch(cdinode->truelist,mnode->quad);
			node->nextlist=mergelist(cdinode->falselist,fnode->nextlist);		
		}else parserror();
		
	}else parserror();
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
} 

void LOOP(){//当型循环语句 
	ptree *node=curnode;	//node为调用当前这个子程序时产生的节点 
	if(kw[cur->k]=="while"){
		curnode=new ptree(cur,node);//创建一个叶子节点,
		link2father(node);
		cur=cur->next;
		
		curnode=new ptree;//创建一个空节点,m1
		link2father(node);
		curnode->quad=nextquad;
		ptree *m1node=curnode;
		
		curnode=new ptree(node,"CDI");//创建一个非终结符节点,
		link2father(node);
		CDI();
		ptree *cdinode=fnode;
		if(kw[cur->k]=="do"){
			curnode=new ptree(cur,node);//创建一个叶子节点,
			link2father(node);
			cur=cur->next;
			
			curnode=new ptree;//创建一个空节点,m2
			link2father(node);
			curnode->quad=nextquad;
			ptree *m2node=curnode;
			
			curnode=new ptree(node,"SEN");//创建一个非终结符节点,
			link2father(node);
			SEN();
	//翻译
			backpatch(fnode->nextlist,m1node->quad);
			backpatch(cdinode->truelist,m2node->quad);
			node->nextlist=cdinode->falselist;
			//给出一条中间代码 
			assembly[nextquad++]=new quat(optab["j"],NULL,NULL,m1node->quad);			
		}else parserror();
	}else parserror();
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
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
	//规约 
	
	fnode=node;
} 













