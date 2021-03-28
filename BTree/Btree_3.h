#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

#define order 32

struct node {
	int n; // # of keys
	pair<int, int>* data;
	node** child;
	bool leaf;

	node(pair<int, int> p) {
		n = 1;
		data = new pair<int, int>[order]; //(order-1)���� ���� ������ full �� ��� insert������ 1ĭ�� ������ �д�.
		data[0] = { p.first, p.second };
		child = new node * [order];
		leaf = true;
	}
};

class BTree {
private:
	node* root = nullptr;

	pair<bool, int> search_node(node* pNode, int x) {//ã�Ҵ��� ���ο� index ��ȯ
		if (pNode == nullptr)//�� ���
			return { false, -1 };

		int i;
		for (i = 0; i < pNode->n; i++) {//index ã��
			if (pNode->data[i].first == x)//ã�Ҵ�
				return { true, i };
			else if (pNode->data[i].first > x)
				break;
		}
		return { false, i };//��ã�Ұ� index �Բ� ��ȯ
	}

	node* Split(node* parent, node* pNode) {//pNode split �� parent ��ȯ
		int i;
		int center = pNode->n / 2;
		int r_start = center + 1;
		pair<int, int> c_data = pNode->data[center];
		pair<int, int> r_data = pNode->data[r_start];

		node* left;
		node* right = new node(r_data);

		//���� ��忡�� right�� ������ key-value ���� ����, �� �κ��� ���� ��忡�� ����
		for (i = 0; i < pNode->n - r_start; i++) {
			right->data[i] = pNode->data[r_start + i];
			pNode->data[r_start + i] = { -1, -1 };
		}
		right->n = pNode->n - r_start;
		pNode->n -= right->n + 1;
		pNode->data[center] = { -1, -1 };


		if (!pNode->leaf) {//���� ��� �ڽ��� ������
			//���� ��忡�� right�� ������ children ���� ����, �� �κ��� ���� ��忡�� ����
			for (i = 0; i < right->n + 1; i++) {
				right->child[i] = pNode->child[r_start + i];
				pNode->child[r_start + i] = nullptr;
			}
			right->leaf = false;
		}

		left = pNode;//���� ���� left�� ��

		//left, right�� ��� ��������� �����ϱ�
		pair<bool, int> p = search_node(parent, c_data.first);
		int index = p.second;

		if (index == -1) {//parent�� nullptr, �� pNode�� root�� ���
			node* temp = new node(c_data);//�� ��� ����� �ڽ� �ޱ�
			temp->child[0] = left;
			temp->child[1] = right;
			temp->leaf = false;
			root = temp;
			return root;
		}

		else {//parent�� �Ϲ����� ����� ���
			for (i = parent->n; i > index; i--)//parent�� index ���� key �� ĭ�� �ڷ� �б�
				parent->data[i] = parent->data[i - 1];
			parent->data[index] = c_data;
			parent->n++;

			for (i = parent->n; i > index + 1; i--)//parent�� index ���� children �� ĭ�� �ڷ� �б�
				parent->child[i] = parent->child[i - 1];

			parent->child[index] = left;//left, right �ޱ�
			parent->child[index + 1] = right;
			return parent;
		}
	}

	void Insert(node* pNode, int key, int value) {//pNode �ȿ� int x�� ����ִ� �Լ�

		//1. pNode nullptr�̸� ���� ���� root�� �ֱ�
		if (pNode == nullptr) {
			pair<int, int> p = { key, value };
			node* temp = new node(p);
			root = temp; //root�� �ٲܱ�? Ȯ�οϷ�. ó���� root�� �Ҵ��� �ȵǾ��־ nullptr�̹Ƿ� pNode = temp�ϸ� �ȵ�.
			return;
		}

		//2. Search
		int index, i;
		pair<bool, int> p = search_node(pNode, key);

		if (p.first) {//2-1. �� ��忡 ����
			cout << key << " already exists in the tree.\n";
			return;
		}

		index = p.second;//2-2. �� ��忡 ����

		//3. Insert(���� insert�� leaf������ �Ͼ)
		if (pNode->leaf) {//3-1. leaf�̹Ƿ� �̹� �� ã�ƺô�. ���� insert
			for (i = pNode->n; i > index; i--)
				pNode->data[i] = pNode->data[i - 1];
			pNode->data[index] = { key, value };
			pNode->n++;
		}
		else {//3-2. �ڽ��� ������ �� Ž��
			Insert(pNode->child[index], key, value);//�ڽ����� ã�Ƶ���

		//4. Overflow ����
			if (pNode->child[index]->n >= order) {//4-1.�ڽĿ��� overflow �߻��ߴ��� check
				pNode = Split(pNode, pNode->child[index]);//�ڽ� split�ϱ�(pNode�� root�� �ƴ� �͸�)
			}
		}

		if (pNode == root) {//root���� Ȯ��
			if (pNode->n >= order) {//4-2.root���� overflow �߻��ߴ��� check(root�� �θ� �����Ƿ� �θ� Ȯ�� ����)
				pNode = Split(nullptr, pNode);//root split�ϱ�
				return;
			}
		}
		return;
	}

	node* Largest(node* pNode) {//���� ū leaf
		if (pNode == nullptr)
			return nullptr;

		while (!pNode->leaf)
			pNode = pNode->child[pNode->n];

		return pNode;

	}

	void rotateR(node* parent, node* pNode, node* sibling, int c_index) {//pNode == parent->child[c_index]
		int i;
		for (i = pNode->n; i > 0; i--)
			pNode->data[i] = pNode->data[i - 1];
		pNode->data[0] = parent->data[c_index - 1];
		parent->data[c_index - 1] = sibling->data[sibling->n - 1];
		sibling->data[sibling->n - 1] = { -1, -1 };
		pNode->n++;
		sibling->n--;

		if (!pNode->leaf) {
			for (i = pNode->n; i > 0; i--)
				pNode->child[i] = pNode->child[i - 1];
			pNode->child[0] = nullptr;
		}
		if (!sibling->leaf) {
			pNode->child[0] = sibling->child[sibling->n + 1];
			sibling->child[sibling->n + 1] = nullptr;
			pNode->leaf = false;
		}
	}

	void rotateL(node* parent, node* pNode, node* sibling, int c_index) {//pNode == parent->child[c_index]
		int i;
		pNode->data[pNode->n] = parent->data[c_index];
		parent->data[c_index] = sibling->data[0];
		for (i = 0; i < sibling->n - 1; i++)
			sibling->data[i] = sibling->data[i + 1];
		sibling->data[sibling->n - 1] = { -1, -1 };
		pNode->n++;
		sibling->n--;

		if (!sibling->leaf) {
			pNode->child[pNode->n] = sibling->child[0];
			pNode->leaf = false;
			for (i = 0; i < sibling->n + 1; i++)
				sibling->child[i] = sibling->child[i + 1];
			sibling->child[i] = nullptr;
		}

	}

	void basic_merge(node* parent, node* left, node* right, int c_index) {//left, parent�� data �� ��, right ���ļ� �ϳ��� ���� �����
		int i;
		left->data[left->n++] = parent->data[c_index];
		left->child[left->n] = right->child[0];
		for (i = 0; i < right->n; i++) {
			left->data[left->n++] = right->data[i];
			left->child[left->n] = right->child[i + 1];
		}
		if (!right->leaf)
			left->leaf = false;

		for (i = c_index; i < parent->n - 1; i++) {
			parent->data[i] = parent->data[i + 1];
			parent->child[i + 1] = parent->child[i + 2];
		}
		parent->child[c_index] = left;
		parent->n--;
		parent->data[parent->n] = { -1,-1 };
		return;
	}

	void Merge(node* parent, node* pNode, int c_index) {//pNode == parent->child[c_index];
		node* sibling, * left, * right;
		//if parent == nullptr

		if (parent == nullptr) {//root�� ��������� merge
			root = pNode->child[c_index];
			delete(pNode);
			return;
		}

		if (c_index == 0) { //ù���
			sibling = parent->child[c_index + 1];
			if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//������ ���� ������ rotateL
				rotateL(parent, pNode, sibling, c_index);
				return;
			}
			else {//���� ������ ��� ��ġ��
				right = sibling;
				left = pNode;
				return basic_merge(parent, left, right, c_index);
			}
		}
		else if (c_index == parent->n) {//�����
			sibling = parent->child[c_index - 1];
			if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//���� ���� ������ rotateR
				rotateR(parent, pNode, sibling, c_index);
				return;
			}
			else {//���� ������ ��� ��ġ��
				right = pNode;
				left = sibling;
				return basic_merge(parent, left, right, c_index-1);
			}
		}
		else {//�߰����
			sibling = parent->child[c_index + 1];
			if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//������ ���� ������ rotateL
				rotateL(parent, pNode, sibling, c_index);
				return;
			}
			else {
				sibling = parent->child[c_index - 1];
				if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//���� ���� ������ rotateR
					rotateR(parent, pNode, sibling, c_index);
					return;
				}
			}
			//���� �� ���� ������ ��� ��ġ��
			right = pNode;
			left = sibling;
			return basic_merge(parent, left, right, c_index-1);
		}
	}

	void Delete(node* pNode, int key, int value) {
		if (pNode == nullptr) {
			cout << "The tree is empty.\n";
			return;
		}

		int index, i;

		pair<bool, int> p = search_node(pNode, key);
		index = p.second;

		if (pNode->leaf) {//�ڽ� ����
			if (p.first && pNode->data[index].second == value) {//ã�Ҵ�
				for (i = index; i < pNode->n - 1; i++)
					pNode->data[i] = pNode->data[i + 1];
				pNode->data[pNode->n - 1] = { -1, -1 };
				pNode->n--;
				return;
			}
			else {//�� Ʈ���� ����
				cout << "There is no <" << key << ", " << value << "> in the tree.\n";
				return;
			}
		}
		else {//�ڽ� �ִ�
			if (p.first && pNode->data[index].second == value) {//ã�Ҵ�
				node* largest_son = Largest(pNode->child[index]);
				int s_key = largest_son->data[largest_son->n - 1].first;
				int s_value = largest_son->data[largest_son->n - 1].second;
				Delete(pNode, s_key, s_value);//���ʿ��� ���� ū leaf ����
				pair<node*, int> deleted = Search(pNode, key);//���ٿ��� ��� leaf�� ���� ������� data �ڸ��� ������
				if (deleted.first != nullptr && deleted.first->data[deleted.second].second == value)
					deleted.first->data[deleted.second] = { s_key, s_value };
			}
			else {//�� ã�Ҵ�
				Delete(pNode->child[index], key, value);//�ڽ� Ž��

				if (pNode->child[index]->n < ceil((double)order / 2) - 1)//�ڽ� underflow �� merge
					Merge(pNode, pNode->child[index], index);

				if (pNode == root) {//root underflow �� merge
					if (pNode->n < 1)
						Merge(nullptr, pNode, 0);
				}
			}
			return;
		}


	}

	pair<node*, int> Search(node* pNode, int x) {//������ �ִ� ���, ������ nullptr ��ȯ
		pair<bool, int> p = search_node(pNode, x);//�� �ܰ迡�� �˻�

		if (p.first)//ã��
			return { pNode, p.second };

		else if (p.second == -1 || pNode->leaf)//��� ������� or ��ã�Ұ� �ڽĵ� ������ nullptr ��ȯ
			return { nullptr, -1 };

		pair<node*, int> temp = Search(pNode->child[p.second], x);//��ã�Ұ� �ڽ� ������ �ڽ� Ž��
		return temp;
	}

	/*void inorderPrint(node* pNode, ofstream* output) {
		if (pNode == NULL)
			return;

		int i;
		if (pNode->leaf) {
			for (i = 0; i < pNode->n; i++) {
				*output << pNode->data[i].first << "\t";
				*output << pNode->data[i].second << endl;
			}
			return;
		}
		for (i = 0; i < pNode->n; i++) {
			inorderPrint(pNode->child[i], output);
			*output << pNode->data[i].first << "\t";
			*output << pNode->data[i].second << endl;
		}
		inorderPrint(pNode->child[i], output);
	}*/

public:
	void Insert(int key, int value) {
		Insert(root, key, value);
		return;
	}

	void Delete(int key, int value) {
		Delete(root, key, value);
		return;
	}

	pair<node*, int> Search(int x) {
		pair<node*, int> temp = Search(root, x);
		return temp;
	}

	/*void InorderPrint(ofstream* output) {
		inorderPrint(root, output);
		*output << endl;
	}*/
};