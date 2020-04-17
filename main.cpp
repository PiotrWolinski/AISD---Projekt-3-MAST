#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Tree* alloc() {
	return (Tree*)malloc(sizeof(Tree));
}

Tree read_NEWICK(char* input) {
	Tree* tmp_tree = alloc();
	init_Tree(tmp_tree);

	int i = 0;
	do {
		if (input[i] == '(') {
			tmp_tree->son = alloc();
			init_Tree(tmp_tree->son);
			tmp_tree->son->parent = tmp_tree;
			tmp_tree = tmp_tree->son;
		}
		else if (input[i] == ',') {
			tmp_tree->brother = alloc();
			init_Tree(tmp_tree->brother);
			tmp_tree->brother->parent = tmp_tree->parent;
			tmp_tree = tmp_tree->brother;
		}
		else if (input[i] == ')') {
			tmp_tree = tmp_tree->parent;
		}
		else {
			//liczba dwucyfrowa
			if (input[i] == '1' && input[i + 1] > 48 && input[i + 1] < 58) {
				tmp_tree->value = 10 + input[i + 1] - 48;
				i++;
			} 
			else {
				tmp_tree->value = input[i] - 48;
			}
		}
		i++;
	} while (input[i] != ';');
	
	return *tmp_tree;
}

int main() {
	int size;
	std::cin >> size;

	Tree* array = (Tree*)malloc(size * sizeof(Tree));
	char** input = (char**)malloc(size * sizeof(char*));
	int i = 0;
	int blank = getchar();
	do
	{
		unsigned int arr_size = 1;
		input[i] = (char*)malloc(sizeof(char));

		if (input[i] != NULL)
		{
			char q;
			unsigned int j = 0;
			while ((q = getchar()) != '\n')
			{
				input[i][j++] = q;
				if (j == arr_size)
				{
					arr_size = j + 1;
					if (char* tmp = (char*)realloc(input[i], arr_size))
					{
						input[i] = tmp;
					}
				}
			}
			input[i][j] = '\0';
			i++;
		}
	} while (i < size);
	for (int i = 0; i < size; i++) {
		array[i] = read_NEWICK(input[i]);
	}


	for (int i = 0; i < size; i++)
	{
		printf("%s\n", input[i]);
	}

	for (int i = 0; i < size; i++) {
		free(input[i]);
	}
	return 0;
}