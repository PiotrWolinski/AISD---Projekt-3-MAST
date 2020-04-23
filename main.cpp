#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//tylko na potrzeby prezentacji tabelki - do usuniecia
#include <iomanip>

struct Tree {
	short id;					//mala wartosc do samego zapisywania numeru wierzcholka, w kolejnosci dodawania
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

void init_Tree(Tree* tree) {
	tree->id = NULL;
	tree->value = NULL;
	tree->parent = NULL;
	tree->brother = NULL;
	tree->son = NULL;
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

Tree read_NEWICK(char* input, int& internal_vertices, Tree* &order) {
	Tree* tmp = alloc();
	init_Tree(tmp);
	order = (Tree*)malloc(sizeof(Tree));
	internal_vertices = 0;

	int i = 0;
	do {
		if (input[i] == '(') {
			++internal_vertices;
			tmp->son = alloc();
			init_Tree(tmp->son);
			order[internal_vertices - 1] = *tmp;
			tmp->id = 10 + internal_vertices;
			if (Tree* real = (Tree*)realloc(order, (internal_vertices + 1)* sizeof(Tree))) {
				order = real;
			}
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

int** alloc_table(int columns, int rows) {
	int** arr = (int**)malloc(columns * sizeof(int*));
	for (int t2 = 0; t2 < columns; t2++) {
		arr[t2] = (int*)malloc(rows * sizeof(int));
	}
	return arr;
}

//inicjuje cala tablice zerami
void init_table(int** arr, int columns, int rows) {
	for (int t2 = 0; t2 < columns; t2++) {
		for (int t1 = 0; t1 < rows; t1++) {
			arr[t2][t1] = 0;
		}
	}
}

//wypelnia pierwsza cwiartke dla lisci
void init_leaves(int** arr, int columns, int rows) {
	for (int t2 = 0; t2 < columns; t2++) {
		for (int t1 = 0; t1 < rows; t1++) {
			if (t2 == t1 && t1 < 10 && t2 < 10) {
				arr[t2][t1] = 1;
			}
			else {
				arr[t2][t1] = 0;
			}
		}
	}
}

//wypelnianie drugiej cwiartki
void check_for_son(int* row, int size, Tree* tree) {
	Tree* tmp = alloc();
	init_Tree(tmp);
	tmp = tree->son;
	while (tmp != NULL)
	{
		row[tmp->value - 1] = 1;
		tmp = tmp->brother;
	}
}

//nie wiem czy to dobrze dziala z tymi wskaznikami szalonymi
void check_leaves(int** arr, int size, int index, Tree* node) {
	Tree* tmp = alloc();
	Tree* tmp2 = alloc();
	init_Tree(tmp);
	init_Tree(tmp2);
	tmp = node->son;
	tmp2 = node;

	while (tmp != NULL) {

		if (tmp->value != NULL) {
			arr[index][tmp->value - 1] = 1;
		}
		else if (tmp->value == NULL) {
			for (int i = 0; i < size; i++) {
				arr[index][i] += arr[tmp->id - 1][i];
			}
		}
		tmp = tmp->brother;
		//jesli tmp->value to NULL, to trzeba zajrzec do odpowiadajacego wiersza w tabeli
	}
	//jesli value to NULL, to zajrzyj 
	node = tmp2;
	free(tmp2);
	free(tmp);
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

	//petla robiaca porownanie kazdy z kazdym
	int comp = 1;
	for (int i = 0; comp < size; i++) {
		for (int j = 1; j + comp <= size; j++) {
			//w tym miejscu comp i comp+j maja odpowiednie wartosci, zeby je zaczac ze soba porownywac
			int t1_size = arr[comp - 1].internal_vert + 10;
			int t2_size = arr[comp + j - 1].internal_vert + 10;

			int** main_arr = alloc_table(t2_size, t1_size);
			init_table(main_arr, t2_size, t1_size);

			//wypelnienie tabelki - I cwiartka
			init_leaves(main_arr, t2_size, t1_size);
			



			//wyswietlanie tabelki
			for (int t2 = 0; t2 <= t2_size; t2++) {
				for (int t1 = 0; t1 <= t1_size; t1++) {
					if (t2 == 0 && t1 == 0) std::cout << std::setw(2) << 0 << ' ';
					if (t2 == 0 && t1 != 0) std::cout << std::setw(2) << t1 << ' ';
					if (t1 == 0 && t2 != 0) std::cout << std::setw(2) << t2 << ' ';
					if (t1 >= 1 && t2 >= 1 /*&& t1 <11 && t2 <11*/) {
						std::cout << std::setw(2) << main_arr[t2 - 1][t1 - 1] << ' ';
					}
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