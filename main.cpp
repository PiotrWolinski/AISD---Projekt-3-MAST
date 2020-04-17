#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

struct Tree {
	int value;
	Tree* parent;
	Tree* son;
	Tree* brother;
};

void init_Tree(Tree* root) {
	root->value = NULL;
	root->parent = NULL;
	root->brother = NULL;
	root->son = NULL;
}

void read_NEWICK(Tree* root, char* input) {
	init_Tree(root);
	int size = 10;
	input = (char*)malloc(size * sizeof(char));
	Tree* tmp_tree; 

	if (input != NULL) {
		char tmp;
		char last;
		int i = 0;
		while ((tmp = getchar()) != ';') {
			if (tmp != '(') {
				if (tmp_tree->son == NULL) {
					tmp_tree->son = (Tree*)malloc(sizeof(Tree));
				}
				else if (tmp_tree->son != NULL) {
					tmp_tree = tmp_tree->son;
					if (tmp_tree->brother == NULL) {
						tmp_tree->brother = (Tree*)malloc(sizeof(Tree));
					}
					else {
						do {
							tmp_tree = tmp_tree->brother;
						} while (tmp_tree->brother == NULL);
					}
				}
				root->son = (Tree*)malloc(sizeof(Tree));
				tmp_tree = root->son;
				init_Tree(tmp_tree);
			}
			last = tmp;
		}
	}



}

int main() {



	return 0;
}