#include "AVLTree.h"

int main(void) {
	ifstream insertion("insert.txt");
	ifstream deletion("delete.txt");
	ofstream insert_result("insert_result.txt");
	ofstream deleted_keys("deleted_keys.txt");
	ofstream delete_result("delete_result.txt");
	if (!insertion.is_open() || !deletion.is_open() || !insert_result.is_open() || !deleted_keys.is_open() || !delete_result.is_open()) {
		cerr << "Open failed.\n";
		return EXIT_FAILURE;
	}

	AVLTree* tree = new AVLTree;
	int data, i;

	cout << "<Insert>\n";

	for (i = 0; i < 1000000; i++) {
		insertion >> data;
		tree->Insert(data);
	}

	insertion.close();
	cout << "Insertion session was over.\n";
	cout << "------------------------------\n";

	cout << "<Search>" << endl;
	ifstream searching_1("insert.txt");
	if (!searching_1.is_open()) {
		cerr << "Open failed.\n";
		return EXIT_FAILURE;
	}

	for (i = 0; i < 1000000; i++) {
		searching_1 >> data;
		tree->Search(data, &insert_result);
	}

	searching_1.close();
	insert_result.close();
	cout << "Searching session was over.\n\n";
	cout << "------------------------------\n";

	cout << "<Delete>" << endl;

	for (i = 0; i < 500000; i++) {
		deletion >> data;
		tree->Delete(data, &deleted_keys);
	}

	deletion.close();
	deleted_keys.close();
	cout << "Deletion session was over.\n\n";
	cout << "------------------------------\n";

	ofstream remained_keys("remained_keys.txt");
	if (!remained_keys.is_open()) {
		cerr << "Open failed.\n";
		return EXIT_FAILURE;
	}
	tree->InorderPrint(&remained_keys);
	remained_keys.close();

	cout << "<Search>" << endl;
	ifstream searching_2("remained_keys.txt");
	if (!searching_2.is_open()) {
		cerr << "Open failed.\n";
		return EXIT_FAILURE;
	}
	
	while (!searching_2.eof()) {
		searching_2 >> data;
		if (searching_2.eof())
			break;
		tree->Search(data, &delete_result);
	}

	searching_2.close();
	delete_result.close();
	cout << "Searching session was over.\n\n";
	cout << "------------------------------\n";

	cout << "The program will be terminated. Good bye.\n";
	return 0;
}