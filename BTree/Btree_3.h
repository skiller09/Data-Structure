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
		data = new pair<int, int>[order]; //(order-1)개만 넣을 거지만 full 시 잠깐 insert용으로 1칸의 여분을 둔다.
		data[0] = { p.first, p.second };
		child = new node * [order];
		leaf = true;
	}
};

class BTree {
private:
	node* root = nullptr;

	pair<bool, int> search_node(node* pNode, int x) {//찾았는지 여부와 index 반환
		if (pNode == nullptr)//빈 노드
			return { false, -1 };

		int i;
		for (i = 0; i < pNode->n; i++) {//index 찾기
			if (pNode->data[i].first == x)//찾았다
				return { true, i };
			else if (pNode->data[i].first > x)
				break;
		}
		return { false, i };//못찾았고 index 함께 반환
	}

	node* Split(node* parent, node* pNode) {//pNode split 후 parent 반환
		int i;
		int center = pNode->n / 2;
		int r_start = center + 1;
		pair<int, int> c_data = pNode->data[center];
		pair<int, int> r_data = pNode->data[r_start];

		node* left;
		node* right = new node(r_data);

		//기존 노드에서 right로 오른쪽 key-value 절반 복사, 그 부분은 기존 노드에서 삭제
		for (i = 0; i < pNode->n - r_start; i++) {
			right->data[i] = pNode->data[r_start + i];
			pNode->data[r_start + i] = { -1, -1 };
		}
		right->n = pNode->n - r_start;
		pNode->n -= right->n + 1;
		pNode->data[center] = { -1, -1 };


		if (!pNode->leaf) {//기존 노드 자식이 있으면
			//기존 노드에서 right로 오른쪽 children 절반 복사, 그 부분은 기존 노드에서 삭제
			for (i = 0; i < right->n + 1; i++) {
				right->child[i] = pNode->child[r_start + i];
				pNode->child[r_start + i] = nullptr;
			}
			right->leaf = false;
		}

		left = pNode;//기존 노드는 left가 됨

		//left, right를 어디에 집어넣을지 결정하기
		pair<bool, int> p = search_node(parent, c_data.first);
		int index = p.second;

		if (index == -1) {//parent가 nullptr, 즉 pNode가 root인 경우
			node* temp = new node(c_data);//새 노드 만들고 자식 달기
			temp->child[0] = left;
			temp->child[1] = right;
			temp->leaf = false;
			root = temp;
			return root;
		}

		else {//parent가 일반적인 노드인 경우
			for (i = parent->n; i > index; i--)//parent의 index 이후 key 한 칸씩 뒤로 밀기
				parent->data[i] = parent->data[i - 1];
			parent->data[index] = c_data;
			parent->n++;

			for (i = parent->n; i > index + 1; i--)//parent의 index 이후 children 한 칸씩 뒤로 밀기
				parent->child[i] = parent->child[i - 1];

			parent->child[index] = left;//left, right 달기
			parent->child[index + 1] = right;
			return parent;
		}
	}

	void Insert(node* pNode, int key, int value) {//pNode 안에 int x를 집어넣는 함수

		//1. pNode nullptr이면 새로 만들어서 root로 넣기
		if (pNode == nullptr) {
			pair<int, int> p = { key, value };
			node* temp = new node(p);
			root = temp; //root로 바꿀까? 확인완료. 처음에 root는 할당이 안되어있어서 nullptr이므로 pNode = temp하면 안됨.
			return;
		}

		//2. Search
		int index, i;
		pair<bool, int> p = search_node(pNode, key);

		if (p.first) {//2-1. 이 노드에 있음
			cout << key << " already exists in the tree.\n";
			return;
		}

		index = p.second;//2-2. 이 노드에 없음

		//3. Insert(실제 insert는 leaf에서만 일어남)
		if (pNode->leaf) {//3-1. leaf이므로 이미 다 찾아봤다. 따라서 insert
			for (i = pNode->n; i > index; i--)
				pNode->data[i] = pNode->data[i - 1];
			pNode->data[index] = { key, value };
			pNode->n++;
		}
		else {//3-2. 자식이 있으니 더 탐색
			Insert(pNode->child[index], key, value);//자식으로 찾아들어가기

		//4. Overflow 관리
			if (pNode->child[index]->n >= order) {//4-1.자식에서 overflow 발생했는지 check
				pNode = Split(pNode, pNode->child[index]);//자식 split하기(pNode가 root가 아닌 것만)
			}
		}

		if (pNode == root) {//root인지 확인
			if (pNode->n >= order) {//4-2.root에서 overflow 발생했는지 check(root는 부모가 없으므로 부모가 확인 못함)
				pNode = Split(nullptr, pNode);//root split하기
				return;
			}
		}
		return;
	}

	node* Largest(node* pNode) {//가장 큰 leaf
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

	void basic_merge(node* parent, node* left, node* right, int c_index) {//left, parent의 data 한 개, right 합쳐서 하나의 노드로 만들기
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

		if (parent == nullptr) {//root가 비어있으면 merge
			root = pNode->child[c_index];
			delete(pNode);
			return;
		}

		if (c_index == 0) { //첫노드
			sibling = parent->child[c_index + 1];
			if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//오른쪽 여유 있으면 rotateL
				rotateL(parent, pNode, sibling, c_index);
				return;
			}
			else {//여유 없으면 노드 합치기
				right = sibling;
				left = pNode;
				return basic_merge(parent, left, right, c_index);
			}
		}
		else if (c_index == parent->n) {//끝노드
			sibling = parent->child[c_index - 1];
			if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//왼쪽 여유 있으면 rotateR
				rotateR(parent, pNode, sibling, c_index);
				return;
			}
			else {//여유 없으면 노드 합치기
				right = pNode;
				left = sibling;
				return basic_merge(parent, left, right, c_index-1);
			}
		}
		else {//중간노드
			sibling = parent->child[c_index + 1];
			if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//오른쪽 여유 있으면 rotateL
				rotateL(parent, pNode, sibling, c_index);
				return;
			}
			else {
				sibling = parent->child[c_index - 1];
				if (sibling->n - 1 >= ceil((double)order / 2) - 1) {//왼쪽 여유 있으면 rotateR
					rotateR(parent, pNode, sibling, c_index);
					return;
				}
			}
			//양쪽 다 여유 없으면 노드 합치기
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

		if (pNode->leaf) {//자식 없다
			if (p.first && pNode->data[index].second == value) {//찾았다
				for (i = index; i < pNode->n - 1; i++)
					pNode->data[i] = pNode->data[i + 1];
				pNode->data[pNode->n - 1] = { -1, -1 };
				pNode->n--;
				return;
			}
			else {//이 트리에 없다
				cout << "There is no <" << key << ", " << value << "> in the tree.\n";
				return;
			}
		}
		else {//자식 있다
			if (p.first && pNode->data[index].second == value) {//찾았다
				node* largest_son = Largest(pNode->child[index]);
				int s_key = largest_son->data[largest_son->n - 1].first;
				int s_value = largest_son->data[largest_son->n - 1].second;
				Delete(pNode, s_key, s_value);//왼쪽에서 제일 큰 leaf 비우기
				pair<node*, int> deleted = Search(pNode, key);//윗줄에서 비운 leaf를 원래 지우려는 data 자리에 덮어씌우기
				if (deleted.first != nullptr && deleted.first->data[deleted.second].second == value)
					deleted.first->data[deleted.second] = { s_key, s_value };
			}
			else {//못 찾았다
				Delete(pNode->child[index], key, value);//자식 탐색

				if (pNode->child[index]->n < ceil((double)order / 2) - 1)//자식 underflow 시 merge
					Merge(pNode, pNode->child[index], index);

				if (pNode == root) {//root underflow 시 merge
					if (pNode->n < 1)
						Merge(nullptr, pNode, 0);
				}
			}
			return;
		}


	}

	pair<node*, int> Search(node* pNode, int x) {//있으면 있는 노드, 없으면 nullptr 반환
		pair<bool, int> p = search_node(pNode, x);//이 단계에서 검색

		if (p.first)//찾음
			return { pNode, p.second };

		else if (p.second == -1 || pNode->leaf)//노드 비어있음 or 못찾았고 자식도 없으면 nullptr 반환
			return { nullptr, -1 };

		pair<node*, int> temp = Search(pNode->child[p.second], x);//못찾았고 자식 있으면 자식 탐색
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