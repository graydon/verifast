struct Node {
  struct Node * left;
	struct Node * right;
	int value;
};

struct Tree {
  struct Node * nullNode;
	struct Node * root;
};

/*@
inductive inttree = | nil | cons(int, inttree, inttree);

fixpoint inttree doInsert(inttree v, int x) {
  switch (v) {
    case nil: return cons(x, nil, nil);
    case cons(i, t1, t2): return x <= i ? cons(i, doInsert(t1, x), t2) : cons(i, t1, doInsert(t2, x));
  }
}
@*/

/*@
predicate node(struct Node * root, struct Node * l, struct Node * r, int value)
  requires root->left |-> l &*& root->right |-> r &*& root->value |-> value &*&  malloc_block_Node(root);
@*/

/*@
predicate tree(struct Tree* t, struct Node* nullNode, inttree tv)
  requires t->root |-> ?_root &*& t->nullNode |-> nullNode &*& node(nullNode, ?_l, ?_r, ?_v) &*& (_root == nullNode ? (emp &*& tv == nil): treen(_root, nullNode, tv)) &*& malloc_block_Tree(t);
@*/ 

/*@
predicate treen(struct Node* root, struct Node* nullNode, inttree tv)
  requires node(root, ?_l, ?_r, ?_v) &*& (_l == nullNode ? (_r == nullNode ? emp &*& tv == cons(_v, nil, nil) : treen(_r, nullNode, ?_rtv) &*& tv == cons(_v, nil, _rtv)) : treen(_l, nullNode, ?_ltv) &*& (_r == nullNode ? emp &*& tv==cons(_v, _ltv, nil) : treen(_r, nullNode, ?_rtv) &*& tv==cons(_v, _ltv, _rtv)));
@*/ 

/*@
predicate treeseg(struct Node* root, struct Node* nullNode, struct Node* stop)
  requires root == stop ? 
	  emp :
		node(root, ?_l, ?_r, ?_v) &*& (_l == nullNode ? (_r == nullNode ? emp : treeseg(_r, nullNode, stop) ) : treeseg(_l, nullNode, stop) &*& (_r == nullNode ? emp  : treeseg(_r, nullNode, stop) ));
@*/

/*@
lemma void distinctNodes(struct Node * node1, struct Node * node2)
  requires node(node1, ?_l1, ?_r1, ?_v1) &*& node(node2, ?_l2, ?_r2, ?_v2);
  ensures node(node1, _l1, _r1, _v1) &*& node(node2, _l2, _r2, _v2) &*& node1 != node2;
{
  open node(node1, _l1, _r1, _v1);
  open node(node2, _l2, _r2, _v2);
  close node(node1, _l1, _r1, _v1);
  close node(node2, _l2, _r2, _v2);
}
@*/

struct Node * createNode()
  //@ requires emp;
	//@ ensures node(result, _, _, _);
{
	struct Node * n = malloc(sizeof(struct Node));
	//@ struct node *l = n->left;
	//@ struct node *r = n->right;
	//@ int x = n->value;
	//@ close node(n, l, r, x);
	return n;
}

struct Tree * createTree()
  //@ requires emp;
	//@ ensures tree(result, _, nil);
{
	struct Tree * t = malloc(sizeof(struct Tree));
	struct Node * nullNode = createNode();
	t->nullNode = nullNode;
	t->root = nullNode;
	//@ close tree(t, nullNode, nil);
	return t;
}

void insert(struct Tree * t, int x)
  //@ requires tree(t, ?_nullNode, ?_tv);
	//@ ensures tree(t, _nullNode, doInsert(_tv, x));
{
	//@ open tree(t, _nullNode, _);
	struct Node * myroot = t->root;
	struct Node * mynull = t->nullNode;
	if(myroot == mynull){
	  struct Node * newNode = createNode();
		//@ open node(newNode, _, _, _);
		newNode->value = x;
		newNode->left = mynull;
		newNode->right = mynull;
		//@ close node(newNode, mynull, mynull, x);
		//@ distinctNodes(newNode, mynull);
		//@ close treen(newNode, mynull, cons(x, nil, nil));
		t->root = newNode;
	} else {
    insertNode(myroot, mynull, x);
	}
	//@ close tree(t, mynull, doInsert(_tv, x));
}


/*
 * Recursive insert.
 */ 
void insertNode(struct Node * root, struct Node * nullNode, int x)
  //@ requires treen(root, nullNode, ?_tv) &*& node(nullNode, ?_nl, ?_nr, ?_nv);
	//@ ensures treen(root, nullNode, doInsert(_tv, x)) &*& node(nullNode, _nl, _nr, _nv);
{
	//@ open treen(root, nullNode, _tv);
	//@ open node(root, _, _, _); 
	int myval = root->value;
	struct Node * myright = root->right;
	struct Node * myleft = root->left;

	if(x<=myval){
	  if(myleft == nullNode){
			struct Node * newNode = createNode();
		  //@ open node(newNode, _, _, _);
		  newNode->value = x;
		  newNode->left = nullNode;
		  newNode->right = nullNode;
		  //@ close node(newNode, nullNode, nullNode, x);
		  //@ distinctNodes(newNode, nullNode);
			root->left = newNode;
			//@ close treen(newNode, nullNode, cons(x, nil, nil));
			//@ close node(root, newNode, myright, myval);
		} else {
			//@ close node(root, myleft, myright, myval);
		  insertNode(myleft, nullNode, x);
		}
	} else {
		if(myright == nullNode){
			struct Node * newNode = createNode();
		  //@ open node(newNode, _, _, _);
		  newNode->value = x;
		  newNode->left = nullNode;
		  newNode->right = nullNode;
		  //@ close node(newNode, nullNode, nullNode, x);
		  //@ distinctNodes(newNode, nullNode);
			root->right = newNode;
			//@ close treen(newNode, nullNode, cons(x, nil, nil));
			//@ close node(root, myleft, newNode, myval);
		} else {
			//@ close node(root, myleft, myright, myval);
		  insertNode(myright, nullNode, x);
		}
	}
	//@ close treen(root, nullNode, doInsert(_tv, x));
}

/*@
lemma void treen2treeseg(struct Node* root, struct Node * nullNode, struct Node * stop) 
  requires treen(root, nullNode, ?_rvalue) &*& node(nullNode, ?_nl, ?_nr, ?_nv) &*& node(stop, ?_stopleft, ?_stopright, ?_stopvalue);
  ensures treeseg(root, nullNode, stop) &*& node(nullNode, ?_nl, ?_nr, ?_nv) &*& node(stop, ?_stopleft, ?_stopright, ?_stopvalue);
{
  open treen(root, nullNode, _rvalue);
	distinctNodes(root, stop);
	distinctNodes(root, nullNode);
	open node(root, _, _, _);
	int myvalue = root->value;
	struct Node * myleft = root->left;
	struct Node * myright = root->right;
	
  if (myleft != nullNode) {
	  treen2treeseg(myleft, nullNode, stop);
	} else {}
	if (myright != nullNode) {
	  treen2treeseg(myright, nullNode, stop);
	} else {}
	close node(root, myleft, myright, myvalue);
	close treeseg(root, nullNode, stop);
}
@*/


/*@
lemma void combineTreeSegL(struct Node * root, struct Node * nullNode, struct Node * oldCurrent, struct Node * newCurrent, struct Node * oldcurrentleft)
  requires treeseg(root, nullNode, oldCurrent) &*&
	         node(nullNode, ?_nl, ?_nr, ?_nv) &*&
					 node(oldCurrent, oldcurrentleft, newCurrent, ?ocv) &*&
           node(newCurrent, ?_ncl, ?_ncr, ?_ncv) &*&
           (oldcurrentleft == nullNode? emp : treeseg(oldcurrentleft, nullNode, newCurrent));
	ensures treeseg(root, nullNode, newCurrent) &*& node(nullNode, _nl, _nr, _nv) &*& node(newCurrent, _ncl, _ncr, _ncv);
{
  open treeseg(root, nullNode, oldCurrent);
	distinctNodes(oldCurrent, nullNode);
	distinctNodes(newCurrent, nullNode);
	distinctNodes(oldCurrent, newCurrent);
	if(root == oldCurrent){
	  close treeseg(newCurrent, nullNode, newCurrent);
		close treeseg(oldCurrent, nullNode, newCurrent);
	} else {
		distinctNodes(oldCurrent, root);
	  distinctNodes(newCurrent, root);
	  distinctNodes(oldCurrent, root);
		open node(root, _, _, _);
	  int rootvalue = root->value;
	  struct Node * rootleft = root->left;
	  struct Node * rootright = root->right;
		
		if(rootleft != nullNode){
		  combineTreeSegL(rootleft, nullNode, oldCurrent, newCurrent, oldcurrentleft);
		} else {}
		if(rootright != nullNode){
			combineTreeSegL(rootright, nullNode, oldCurrent, newCurrent, oldcurrentleft);
		} else {}
		close node(root, rootleft, rootright, rootvalue);
		close treeseg(root, nullNode, newCurrent);
	}
}
@*/

/*@
lemma void combineTreeSegR(struct Node * root, struct Node * nullNode, struct Node * oldCurrent, struct Node * newCurrent, struct Node * oldcurrentright)
  requires treeseg(root, nullNode, oldCurrent) &*&
	         node(nullNode, ?_nl, ?_nr, ?_nv) &*&
					 node(oldCurrent, newCurrent, oldcurrentright, ?_ocv) &*&
           node(newCurrent, ?_ncl, ?_ncr, ?_ncv) &*&
           (oldcurrentright == nullNode ? emp : treeseg(oldcurrentright, nullNode, newCurrent));
	ensures treeseg(root, nullNode, newCurrent) &*& node(nullNode, _nl, _nr, _nv) &*& node(newCurrent, _ncl, _ncr, _ncv);
{
  open treeseg(root, nullNode, oldCurrent);
	if(root == oldCurrent){
		distinctNodes(oldCurrent, nullNode);
		distinctNodes(newCurrent, nullNode);
		distinctNodes(oldCurrent, newCurrent);
	  close treeseg(newCurrent, nullNode, newCurrent);
		close treeseg(oldCurrent, nullNode, newCurrent);
	} else {
		open node(root, _, _, _);
	  int rootvalue = root->value;
	  struct Node * rootleft = root->left;
	  struct Node * rootright = root->right;
		
		if(l!=nullNode){
		  combineTreeSegR(rootleft, nullNode, oldCurrent, newCurrent, oldcurrentright);
		} else {}
		if(r!=nullNode){
			combineTreeSegR(rootright, nullNode, oldCurrent, newCurrent, oldcurrentright);
		} else {}
		close node(root, rootleft, rootright, rootvalue);
		close treeseg(root, nullNode, newCurrent);
	}
}
@*/

/*@
lemma void maketreeseg(struct Node * root, struct Node * nullNode, struct Node * oldCurrent, struct Node * newCurrent, struct Node * otherhalf)
  requires treeseg(root, nullNode, oldCurrent)&*&
	             node(nullNode, ?_nl, ?_nr, ?_nv) &*&
							 node(oldCurrent, ?_ocl, ?_ocr, ?_ocv) &*&
							 (otherhalf == nullNode ? emp : treen(otherhalf, _, _)) &*&
							 (newCurrent == _ocr ? _ocl == otherhalf : (otherhalf == _ocr &*& newCurrent == _ocl)) &*&
							 node(newCurrent, ?_ncl, ?_ncr, ?_ncv);
	ensures (otherhalf == _ocl ? treeseg(root, nullNode, _ocr) : treeseg(root, nullNode, _ocl)) &*& node(newCurrent, _ncl, _ncr, _ncv) &*& node(nullNode, _nl, _nr, _nv);
{
  open node(oldCurrent, _, _, _);
	int oldcurrentvalue = oldCurrent->value;
	struct Node * oldcurrentleft = oldCurrent->left;
	struct Node * oldcurrentright = oldCurrent->right;
	close node(oldCurrent, oldcurrentleft, oldcurrentright, oldcurrentvalue);
	
	if(otherhalf == oldcurrentleft){
	  // treeseg(root, nullNode, oldCurrent)
		// node(oldCurrent, ?_ocl, ?_ocr, ?_ocv)
		// treen(oldcurrentleft, _, _)
		// node (newCurrent, _, _, _)
		if(oldcurrentleft != nullNode){
		  treen2treeseg(oldcurrentleft, nullNode, newCurrent);
		} else {}
		// treeseg(root, nullNode, oldCurrent)
		// node(oldCurrent, oldcurrentLeft, newCurrent, ?_ocv)
		// treeseg(oldcurrentleft, _, newCurrent)
		// node (newCurrent, _, _, _)
		combineTreeSegL(root, nullNode, oldCurrent, newCurrent, oldcurrentleft);
	} else {
		if(oldcurrentright != nullNode) {
		  treen2lseg(oldcurrentright, nullNode, newCurrent);
		} else {}
		combineTreeSegR(root, nullNode, oldCurrent, newCurrent, oldcurrentleft);
	}
}
@*/



