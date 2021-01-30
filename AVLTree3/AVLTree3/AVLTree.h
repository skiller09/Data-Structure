#include <iostream>
#include <fstream>
#include <queue>
using namespace std;

struct node {
	int data, height;
	node* left;
	node* right;
	node(int x) : data(x), height(1), left(NULL), right(NULL) {}
};

class AVLTree {
private:
	node* root = NULL;
	int n = 0;
	queue<int> q;
	bool find = false;

	int GetHeight(node* pNode) {
		if (pNode == NULL)
			return 0;
		return pNode->height;
	}

	node* rotateR(node* pNode) {
		node* newNode = pNode->left;
		if (newNode != NULL) {
			pNode->left = newNode->right;
			newNode->right = pNode;
			pNode->height = 1 + max(GetHeight(pNode->left), GetHeight(pNode->right));
			newNode->height = 1 + max(GetHeight(newNode->left), GetHeight(newNode->right));
			return newNode;
		}
		return pNode;
	}

	node* rotateL(node* pNode) {
		node* newNode = pNode->right;
		if (newNode != NULL) {
			pNode->right = newNode->left;
			newNode->left = pNode;
			pNode->height = 1 + max(GetHeight(pNode->left), GetHeight(pNode->right));
			newNode->height = 1 + max(GetHeight(newNode->left), GetHeight(newNode->right));
			return newNode;
		}
		return pNode;
	}

	void inorderPrint(node* pNode, ofstream* output) {
		if (pNode == NULL)
			return;

		inorderPrint(pNode->left, output);
		*output << pNode->data << " ";
		inorderPrint(pNode->right, output);
	}

	node* Insert(node* pNode, int x) {
		if (pNode == NULL) {
			n += 1;
			node* temp = new node(x);
			return temp;
		}

		if (x == pNode->data)
			cout << "Insertion failed: " << x << " already exists in the tree.\n";

		else if (x < pNode->data)
			pNode->left = Insert(pNode->left, x);

		else if (x > pNode->data)
			pNode->right = Insert(pNode->right, x);

		pNode->height = 1 + max(GetHeight(pNode->left), GetHeight(pNode->right));
		int diff = GetHeight(pNode->left) - GetHeight(pNode->right);
		if (diff > 1) {
			if (x < pNode->left->data)
				return rotateR(pNode);
			else {
				pNode->left = rotateL(pNode->left);
				return rotateR(pNode);
			}
		}
		else if (diff < -1) {
			if (x > pNode->right->data)
				return rotateL(pNode);
			else {
				pNode->right = rotateR(pNode->right);
				return rotateL(pNode);
			}
		}
		return pNode;
	}

	node* Delete(node* pNode, int x) {
		if (pNode == NULL) {
			cout << "Deletion failed: There is no "<< x <<" in this tree.\n";
			return NULL;
		}

		if (x < pNode->data)
			pNode->left = Delete(pNode->left, x);
		else if (x > pNode->data)
			pNode->right = Delete(pNode->right, x);

		else {
			node* r_child = pNode->right;
			if (pNode->right == NULL) {
				node* l_child = pNode->left;
				delete(pNode);
				pNode = l_child;
			}
			else if (pNode->left == NULL) {
				delete(pNode);
				pNode = r_child;
			}
			else {
				while (r_child->left != NULL)
					r_child = r_child->left;
				pNode->data = r_child->data;
				pNode->right = Delete(pNode->right, r_child->data);
			}
		}

		if (pNode == NULL)
			return pNode;

		pNode->height = 1 + max(GetHeight(pNode->left), GetHeight(pNode->right));
		int diff = GetHeight(pNode->left) - GetHeight(pNode->right);
		if (diff > 1) {
			if (x > pNode->left->data)
				return rotateR(pNode);
			else {
				pNode->left = rotateL(pNode->left);
				return rotateR(pNode);
			}
		}
		else if (diff < -1) {
			if (x < pNode->right->data)
				return rotateL(pNode);
			else {
				pNode->right = rotateR(pNode->right);
				return rotateL(pNode);
			}
		}
		return pNode;
	}

	node* Search(node* pNode, int x) {
		if (pNode == NULL)
			return NULL;
		q.push(pNode->data);
		int k = pNode->data;
		if (k == x){
			find = true;
			return pNode;
		}
		if (k > x)
			return Search(pNode->left, x);
		if (k < x)
			return Search(pNode->right, x);
	}

public:
	void Insert(int x) {
		root = Insert(root, x);
	}
	void Delete(int x, ofstream* output) {
		root = Delete(root, x);
		if (root != NULL)
			*output << x << endl;
	}
	node* Search(int x, ofstream* output) {
		while (!q.empty())
			q.pop();
		find = false;
		node* target = Search(root, x);
		if (find) {
			*output << "Searching successed: ";
			while (q.front() != x) {
				*output << q.front() << " -> ";
				q.pop();
			}
			q.pop();
			*output << x << endl;
		}
		else
			cout << "Searching failed: There is no " << x << " in this tree.\n";

		return target;
	}
	void InorderPrint(ofstream* output) {
		inorderPrint(root, output);
		*output << endl;
	}
};

//reference: https://github.com/Ekan5h/AVLtree/blob/master/AVL.cpp