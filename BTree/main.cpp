#include "Btree_3.h"
#include <string>

int main() {

	BTree* tree = new BTree;
	int key, value, value_2;
	pair<int, int> temp;
	int input;
	string filename;
	bool same;

	cout << "<<Main>>\n";
	cout << "Press the number of the activity which you want to excute.\n";
	cout << "1. Insert\n";
	cout << "2. Delete\n";
	cout << "3. Quit\n";

	while (cin >> input) {
		switch (input) {
		case 1://insert & search
		{
			cout << "------------------------------\n";
			cout << "<Insert>\n";

			cout << "Enter the name of file for insertion.\n";
			cin >> filename;
			ifstream insertion(filename.c_str(), ios::in);
			if (!insertion.is_open()){
				cerr << "Open failed.\n";
				return EXIT_FAILURE;
			}
			cout << "Insertion is in progress. Please wait...\n";

			while (insertion >> key && insertion >> value) {
				tree->Insert(key, value);
			}

			insertion.close();
			cout << "Insertion session was over.\n\n";
			cout << "------------------------------\n";

			cout << "<Search 1>" << endl;
			ifstream searching_1(filename.c_str(), ios::in);
			ofstream insert_result("insert_result.csv");
			if (!searching_1.is_open() || !insert_result.is_open()) {
				cerr << "Open failed.\n";
				return EXIT_FAILURE;
			}
			cout << "Searching after insertion is in progress. Please wait...\n";

			same = true;

			while (searching_1 >> key && searching_1 >> value) {
				pair<node*, int> p = tree->Search(key);
				if (p.first == nullptr) {
					same = false;
					insert_result << key << "\t";
					insert_result << "N/A\n";
				}
				else {
					value_2 = p.first->data[p.second].second;
					insert_result << key << "\t";
					insert_result << value_2 << endl;
					if (value != value_2) {
						same = false;
						insert_result << key << "\t";
						insert_result << "N/A\n";
					}
				}
			}

			if (same)
				cout << "Same result.\n";
			else
				cout << "Not same.\n";

			searching_1.close();
			insert_result.close();
			cout << "Searching 1 session was over.\n\n";
			cout << "------------------------------\n";
			break;
		}
			

		case 2://delete & search
		{
			cout << "------------------------------\n";
			cout << "<Delete>\n";
			cout << "Enter the name of file for insertion.\n";
			cin >> filename;
			ifstream deletion(filename.c_str(), ios::in);
			if (!deletion.is_open()) {
				cerr << "Open failed.\n";
				return EXIT_FAILURE;
			}
			cout << "Deletion is in progress. Please wait...\n";
			while (deletion >> key && deletion >> value) {
				tree->Delete(key, value);
			}

			deletion.close();
			cout << "Deletion session was over.\n\n";
			cout << "------------------------------\n";

			cout << "<Search 2>" << endl;
			ifstream searching_2("delete_result.csv");
			ofstream remained_keys("remained_keys.csv");
			if (!searching_2.is_open() || !remained_keys.is_open()) {
				cerr << "Open failed.\n";
				return EXIT_FAILURE;
			}
			cout << "Searching after deletion is in progress. Please wait...\n";

			string delete_input;
			same = true;

			while (searching_2 >> key && searching_2 >> delete_input) {
				pair<node*, int> p = tree->Search(key);
				remained_keys << key << "\t";
				remained_keys << delete_input << endl;
				if (p.first == nullptr) {//¸øÃ£À½
					if (delete_input != "N/A") {
						same = false;
						cout << key << " : not same here.\n";
					}
				}
				else {
					value_2 = p.first->data[p.second].second;
					value = stoi(delete_input);
					if (value != value_2) {
						same = false;
						cout << key << " : not same here.\n";
					}
				}
			}
			if (same)
				cout << "Same result.\n";
			else
				cout << "Not same.\n";

			searching_2.close();
			remained_keys.close();
			cout << "Searching 2 session was over.\n\n";
			cout << "------------------------------\n";
			break;
		}

		case 3://quit
			cout << "------------------------------\n";
			delete(tree);
			cout << "The program will be terminated. Good bye.\n";
			return 0;
		}

		cout << "<<Main>>\n";
		cout << "Press the number of the activity which you want to excute.\n";
		cout << "1. Insert\n";
		cout << "2. Delete\n";
		cout << "3. Quit\n";
	}

}