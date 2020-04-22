#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Tree {
	int value;					//jesli dany wierzcholek jest lisciem, to tu jest jego wartosc, jesli nie jest lisciem, to jest to NULL
	Tree* parent;				
	Tree* son;
	Tree* brother;
};

struct Root {
	Tree tree;					//zwykly korzen, ktory bedzie zaczynal drzewo
	Tree* order;				//tablica, ktora bedzie przetrzymywala wezly i liscie w kolejnosci dodania
	int internal_vert;			//zmienna przechowujaca ilosc wierzcholkow wewnetrznych
	int inner;
};

struct Collector {
	Tree tree;
	Collector* next;
};

void init_Tree(Tree* root) {
	root->value = NULL;
	root->parent = NULL;
	root->brother = NULL;
	root->son = NULL;
}

void init_Root(Root* root) {
	init_Tree(&root->tree);
	root->order = NULL;
	root->internal_vert = NULL;
	root->inner = NULL;
}

Tree* alloc() {
	return (Tree*)malloc(sizeof(Tree));
}

Tree read_NEWICK(char* input, int& internal_vertices, Tree* order) {
	Tree* tmp = alloc();
	init_Tree(tmp);
	order = (Tree*)malloc(1*sizeof(Tree));
	internal_vertices = 0;

	int i = 0;
	do {
		if (input[i] == '(') {
			++internal_vertices;
			tmp->son = alloc();
			init_Tree(tmp->son);
			order[internal_vertices - 1] = *tmp;
			if (Tree* real = (Tree*)realloc(order, internal_vertices + 1)) {
				order = real;
			}
			//order = (Tree**)realloc(order, internal_vertices + 1);
			tmp->son->parent = tmp;
			tmp = tmp->son;
		}
		else if (input[i] == ',') {
			tmp->brother = alloc();
			init_Tree(tmp->brother);
			tmp->brother->parent = tmp->parent;
			tmp = tmp->brother;
		}
		else if (input[i] == ')') {
			tmp = tmp->parent;
		}
		else {
			//liczba dwucyfrowa
			if (input[i] == '1' && input[i + 1] >= 48 && input[i + 1] < 58) {
				tmp->value = 10 + input[i + 1] - 48;
				i++;
			} 
			else {
				tmp->value = input[i] - 48;
			}
		}
		i++;
	} while (input[i] != ';');
	
	return *tmp;
}

int main() {
	int size;
	std::cin >> size;

	Root* arr = (Root*)malloc(size * sizeof(Root));
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
		init_Root(&arr[i]);
		arr[i].tree = read_NEWICK(input[i], arr[i].internal_vert, arr[i].order);
	}

	//tu sie zacznie dziac tabela
	int comp = 1;
	for (int i = 0; comp < size; i++) {
		for (int j = 1; j + comp <= size; j++) {
			//w tym miejscu comp i comp+j maja odpowiednie wartosci, zeby je zaczac ze soba porownywac
			int t1_size = arr[comp - 1].internal_vert + 10;
			int t2_size = arr[comp + j - 1].internal_vert + 10;

			int** main_arr = (int**)malloc(t2_size * sizeof(int*));

			//wyswietlanie tabelki
			for (int t2 = 1; t2 <= t2_size; t2++) {
				main_arr[t2-1] = (int*)malloc(t1_size * sizeof(int));
				for (int t1 = 1; t1 <= t1_size; t1++) {
					if (t2 == 1) std::cout << t1 << ' ';
					if (t1 == 1) std::cout << t2 << ' ';
				}
				
				std::cout << '\n';
			}
			for (int f = 0; f < t2_size; f++) {
				free(main_arr[f]);
			}
			std::cout << comp << '-' << comp + j << '\n';
		}
		comp++;
	}
	











	for (int i = 0; i < size; i++) {
		free(input[i]);
	}
	return 0;
}