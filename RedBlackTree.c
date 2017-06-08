#include <stdio.h>
#include <malloc.h>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string.h>

int BN = 0;

typedef struct NODE* NodePtr;

struct NODE {
	int key;
	struct NODE *left;
	struct NODE *right;
	struct NODE *parent;

	enum { RED = 0, BLACK = 1 } color;
};

typedef struct RBT* RBTPtr;
struct RBT {
	NodePtr r;
	NodePtr nil;
};

NodePtr node_alloc(int val) {
	NodePtr self = (NodePtr)malloc(sizeof(struct NODE));
	self->key = val;
	self->left = NULL;
	self->right = NULL;
	self->parent = NULL;

	return self;
}

RBTPtr rbt_alloc() {
	RBTPtr self = (RBTPtr)malloc(sizeof(struct RBT));
	self->nil = (NodePtr)malloc(sizeof(struct NODE));
	(self->nil)->color = BLACK;
	self->r = self->nil;

	return self;
}

void left_rotate(RBTPtr r, NodePtr x) {
	NodePtr temp;

	temp = x->right;

	x->right = temp->left;

	if (temp->left != r->nil) {
		(temp->left)->parent = x;
	}

	temp->parent = x->parent;

	if (x->parent == r->nil) {
		r->r = temp;
	}
	else if (x == (x->parent)->left) {
		(x->parent)->left = temp;
	}
	else {
		(x->parent)->right = temp;
	}

	temp->left = x;
	x->parent = temp;
}

void right_rotate(RBTPtr r, NodePtr y) {
	NodePtr temp;
	temp = y->left;
	y->left = temp->right;

	if (temp->right != r->nil) {
		(temp->right)->parent = y;
	}

	temp->parent = y->parent;

	if (y->parent == r->nil) {
		r->r = temp;
	}
	else if (y == (y->parent)->left) {
		(y->parent)->left = temp;
	}
	else {
		(y->parent)->right = temp;
	}

	temp->right = y;
	y->parent = temp;
}

void insert_fixup(RBTPtr r, NodePtr x) {
	NodePtr u = NULL; //uncle node

	while ((x->parent)->color == RED) {
		if (x->parent == (x->parent->parent)->left) {
			u = (x->parent->parent)->right;

			if (u->color == RED) {
				(x->parent)->color = BLACK;
				u->color = BLACK;

				(x->parent->parent)->color = RED;

				x = x->parent->parent;
			}
			else {
				if (x == (x->parent)->right) {
					x = x->parent;
					left_rotate(r, x);

				}

				(x->parent)->color = BLACK;
				(x->parent->parent)->color = RED;

				right_rotate(r, (x->parent->parent));

			}
		}
		else {
			u = (x->parent->parent)->left;

			if (u->color == RED) {
				(x->parent)->color = BLACK;
				u->color = BLACK;

				(x->parent->parent)->color = RED;

				x = x->parent->parent;
			}
			else {
				if (x == (x->parent)->left) {
					x = (x->parent);

					right_rotate(r, x);
				}
				(x->parent)->color = BLACK;
				(x->parent->parent)->color = RED;

				left_rotate(r, (x->parent->parent));
			}
		}
	}

	(r->r)->color = BLACK;
}

void insert_node(RBTPtr r, int key) {

	NodePtr n = (NodePtr)malloc(sizeof(struct NODE));
	NodePtr temp = r->r;
	NodePtr p = r->nil;

	n->left = r->nil;
	n->right = r->nil;
	n->color = RED;
	n->key = key;


		while (temp != r->nil) {
			p = temp;

			if (key > temp->key) {
				temp = temp->right;
			}
			else {
				temp = temp->left;
			}
		}

		n->parent = p;

		if (p == r->nil) {
			r->r = n;
		}
		else if (key > p->key) {
			p->right = n;
		}
		else {
			p->left = n;
		}

		insert_fixup(r, n);
	
}

void tree_transplant(RBTPtr r, NodePtr t, NodePtr c) {
	if (t->parent == r->nil) {
		r->r = c;
	}
	else if (t == (t->parent)->left) {
		(t->parent)->left = c;
	}
	else {
		(t->parent)->right = c;
	}

	c->parent = t->parent;
}

void delete_fixup(RBTPtr r, NodePtr x) {
	NodePtr s = NULL; //sibling

	while ((x != r->r) && (x->color == BLACK)) {
		if (x == (x->parent)->left) {
			s = (x->parent)->right;

			if (s->color == RED) {
				s->color = BLACK;
				(x->parent)->color = RED;
				left_rotate(r, x->parent);

				s = (x->parent)->right;
			}

			if ((s->left)->color == BLACK && (s->right)->color == BLACK) {
				s->color = RED;
				x = x->parent;
			}
			else if ((s->left)->color == RED && (s->right)->color == BLACK) {
				s->color = RED;
				(s->left)->color = BLACK;
				right_rotate(r, s);

				s = (x->parent)->right;
			}

			if ((s->right)->color == RED) {
				s->color = (x->parent)->color;
				(s->right)->color = BLACK;
				(x->parent)->color = BLACK;
				left_rotate(r, x->parent);

				x = r->r;
			}
		}
		else {
			s = (x->parent)->left;

			if (s->color == RED) {
				s->color = BLACK;
				(x->parent)->color = RED;
				right_rotate(r, x->parent);

				s = (x->parent)->left;
			}

			if ((s->left)->color == BLACK && (s->right)->color == BLACK) {
				s->color = RED;
				x = x->parent;
			}

			else if ((s->right)->color == RED && (s->left)->color == BLACK) {
				s->color = RED;
				(s->right)->color = BLACK;
				left_rotate(r, s);

				s = (x->parent)->left;
			}

			if ((s->left)->color == RED) {
				s->color = (x->parent)->color;
				(s->left)->color = BLACK;
				(x->parent)->color = BLACK;
				right_rotate(r, x->parent);

				x = r->r;
			}
		}
	}

	x->color = BLACK;
}

void delete_node(RBTPtr r, int key) {
	NodePtr target = r->r;
	NodePtr temp = NULL;
	NodePtr x = NULL;
	char t_clr;


	while (key != target->key) {
		if (target->key > key) {
			target = target->left;
		}
		else {
			target = target->right;
		}
	}
	t_clr = target->color;

	if (target->left == r->nil) {
		x = target->right;
		tree_transplant(r, target, target->right);
	}
	else if (target->right == r->nil) {
		x = target->left;
		tree_transplant(r, target, target->left);
	}
	else {
		temp = target->right;

		while (temp->left != r->nil) {
			temp = temp->left;
		}

		t_clr = temp->color;
		x = temp->right;

		tree_transplant(r, temp, temp->right);
		temp->right = target->right;
		(temp->right)->parent = temp;

		tree_transplant(r, target, temp);
		temp->left = target->left;
		(temp->left)->parent = temp;
		temp->color = target->color;
	}

	if (t_clr == BLACK) {
		delete_fixup(r, x);
	}

	free(target);
}

int search(RBTPtr r, int target) {
	NodePtr current = r->r;
	int check = 1;

	if (current == r->nil) {
		check = 0;
		return check;
	}
	while (current->key != target) {
		if (current->key > target) {
			if (current->left != r->nil) {
				current = current->left;
			}
			else {
				check = 0;
				return check;
			}
		}
		else {
			if (current->right != r->nil) {
				current = current->right;
			}
			else {
				check = 0;
				return check;
			}
		}
	}

	return check = 1;
}

int BlackNode_Height(RBTPtr r, NodePtr tree) {
	int h = 0;

	while (tree != r->nil){
		if (tree->color == BLACK) {
			h++;
		}
		tree = tree->right;
	}

	return h;
}

void inorder_traversal(RBTPtr r, NodePtr tree) {
	if (tree == r->nil) {
		return;
	}
	else{
		inorder_traversal(r ,tree->left);
		printf("%d %c \n", tree->key, tree->color ==  1 ? 'B' : 'R');
		inorder_traversal(r, tree->right);
	}
}

int HowManyBN(RBTPtr r, NodePtr tree) {
	if (tree == r->nil) {
		return 0;
	}
	else {
		if (tree->color == BLACK) {
			BN++;
		}
		HowManyBN(r, tree->left);
		HowManyBN(r, tree->right);
	}
	return BN;
}

void main(int argc, char **argv) {
	FILE *fp;
	char filename[] = "test01.txt";

	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	int data;

	int Node_num = 0;
	int insert = 0;
	int miss = 0;
	int del = 0;

	RBTPtr rbt = rbt_alloc();

	fp = fopen("test01.txt", "r");


	while (fscanf(fp, "%d", &data) != EOF) {
		if (data == 0) break;
		else if (data > 0) {
			insert_node(rbt, data);
			Node_num += 1;
			insert += 1;
		}
		else {
			data = data * (-1);
			
			if (search(rbt, data) == 1) {
				delete_node(rbt, data);
				Node_num -= 1;
				del += 1;
			}
			else {
				//printf("%d does not exist.\n", data);
				miss += 1;
			}
		}
	}
	fclose(fp);

	StringCchCopy(szDir, MAX_PATH, TEXT("."));

	//_tprintf(TEXT("\nTarget directory is %s\n\n"), szDir);

	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name.


	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	// Find the first file in the directory.

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		printf("Error: FindFirstFile\n");

	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
		}
		else
		{ 
			_tprintf(TEXT("filename = %s\n"), ffd.cFileName);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		printf("Error: FindFirstFile\n");
	}

	FindClose(hFind);

	//printf("filename = %s \n", filename);

	printf("total = %d \n", Node_num);
	printf("insert = %d \n", insert);
	printf("deleted = %d \n", del);
	printf("miss = %d \n", miss);

	printf("nb = %d \n", HowManyBN(rbt, rbt->r));
	printf("bh = %d \n", BlackNode_Height(rbt, rbt->r));

	inorder_traversal(rbt, rbt->r);
	return;
}