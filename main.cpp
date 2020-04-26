#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//tylko na potrzeby prezentacji tabelki - do usuniecia
#include <iomanip>

#define LEAVES 10

struct Tree {
	short id;					//mala wartosc do samego zapisywania numeru wierzcholka, w kolejnosci dodawania
	int value;					//jesli dany wierzcholek jest lisciem, to tu jest jego wartosc, jesli nie jest lisciem, to jest to NULL
	Tree* parent;				
	Tree* son;
	Tree* brother;
};

struct Root {
	Tree* tree;					//zwykly korzen, ktory bedzie zaczynal drzewo
	Tree** order;				//tablica, ktora bedzie przetrzymywala wezly i liscie w kolejnosci dodania
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
	init_Tree(root->tree);
	root->order = NULL;
	root->internal_vert = NULL;
	root->inner = NULL;
}

Tree* alloc() {
	return (Tree*)malloc(sizeof(Tree));
}

Tree* read_NEWICK(char* input, int& internal_vertices, Tree** &order) {
	Tree* tmp = alloc();
	init_Tree(tmp);
	order = (Tree**)malloc(sizeof(Tree*));
	internal_vertices = 0;

	int i = 0;
	do {
		if (input[i] == '(') {
			++internal_vertices;
			tmp->son = alloc();
			init_Tree(tmp->son);
			order[internal_vertices - 1] = tmp;
			tmp->id = 10 + internal_vertices;
			if (Tree** real = (Tree**)realloc(order, (internal_vertices + 1)* sizeof(Tree*))) {
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
	
	return tmp;
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


//sprawdzanie czy konkretny wierzcholek zawiera okreslone liscie
void check_leaves_vert(int** arr, int index, Tree* &node) {
	Tree* tmp = node->son;
	Tree* tmp2 = node;

	while (tmp != NULL) {

		if (tmp->value != NULL) {
			arr[index][tmp->value - 1] = 1;
		}
		else if (tmp->value == NULL) {
			for (int i = 0; i < 10; i++) {
				arr[index][i] += arr[tmp->id - 1][i];
			}
		}
		tmp = tmp->brother;
		//jesli tmp->value to NULL, to trzeba zajrzec do odpowiadajacego wiersza w tabeli
	}
	//jesli value to NULL, to zajrzyj 
	node = tmp2;
}

//sprawdzanie czy konkretny wierzcholek zawiera okreslone liscie
void check_leaves_hori(int** arr, int index, Tree*& node) {
	Tree* tmp = node->son;
	Tree* tmp2 = node;

	while (tmp != NULL) {

		if (tmp->value != NULL) {
			arr[tmp->value - 1][index] = 1;
		}
		else if (tmp->value == NULL) {
			for (int i = 0; i < 10; i++) {
				arr[i][index] += arr[i][tmp->id - 1];
			}
		}
		tmp = tmp->brother;
		//jesli tmp->value to NULL, to trzeba zajrzec do odpowiadajacego wiersza w tabeli
	}
	//jesli value to NULL, to zajrzyj 
	node = tmp2;
}


//funkcja sprawdza czy wierzcholki danego drzewa zawieraja konkretne liscie
void check_vert(Tree** order, int** arr, int size_y) {
	for (int i = size_y - 1; i >= 10; i--) {
		check_leaves_vert(arr, i, order[i - 10]);
	}
}

//sprawdzam czy konkretny lisc znajduje sie w konkrentym wierzcholku
int check_leaf(int** arr, int index, int leaf) {
	if (arr[index - 1][leaf - 1] == 1) {
		return 1;
	}
	else {
		return 0;
	}
}

//funkcja sprawdza czy wierzcholki danego drzewa zawieraja konkretne liscie
void check_hori(Tree** order, int** arr, int size_x) {
	for (int i = size_x - 1; i >= 10; i--) {
		check_leaves_hori(arr, i, order[i - 10]);
	}
}

//zwraca liczbe synow (tylko bezposrednich) danego wierzcholka
int count_sons(Tree* tree) {
	Tree* tmp = tree->son;
	int counter = 0;

	while (tmp != NULL) {
		counter++;
		tmp = tmp->brother;
	}
	return counter;
}

//sprawdza dwa podane wierzcholki
int compare(int** arr, Tree* node1, Tree* node2) {
	//obydwa wierzcholki to liscie
	if (!node1->id && !node2->id) {
		if (node1->value == node2->value) {
			return 1;
		}
		else {
			return 0;
		}
	}
	//wierzcholek 1 to rozgalezienie, a drugi to lisc, wiec sprawdzam czy ten wierzcholek zawiera tego liscia
	else if (node1->id > 10 && !node2->id) {
		return check_leaf(arr, node1->id, node2->value);
	}
	//wierzcholek 2 to rozgalezienie, a pierwszy to lisc, wiec sprawdzam czy ten wierzcholek zawiera tego liscia
	else if (!node1->id && node2->id > 10) {
		return check_leaf(arr, node1->value, node2->id);
	}
	//obydwa wierzcholki to rozgalezienia, wiec zagladam do tabelki w czwartej cwiartce
	else {
		return arr[node1->id - 1][node2->id - 1];
	}
}

int check_id(int* arr, int size, int value) {
	for (int i = 0; i < size; i++) {
		if (arr[i] == value) {
			return 0;
		}
	}
	return 1;
}

void exclude_row(int** arr, int row, int size) {
	for (int i = 0; i < size; i++) {
		arr[row][i] = 1;
	}
}

void exclude_column(int** arr, int column, int size) {
	for (int i = 0; i < size; i++) {
		arr[i][column] = 1;
	}
}

int find_max(int** arr, int size_Y, int size_X, int** pom) {
	int max = 0;
	int max_x = 0;
	int max_y = 0;
	for (int i = 0; i < size_Y; i++) {
		for (int j = 0; j < size_X; j++) {
			if (arr[i][j] > max && !pom[i][j]) {
				max = arr[i][j];
				max_y = i;
				max_x = j;
			}
		}
	}

	exclude_column(pom, max_x, size_Y);
	exclude_row(pom, max_y, size_X);
	return max;
}

int compare_inner(int** arr, Tree* order1, Tree* order2) {
	int size_Y = count_sons(order1);
	int size_X = count_sons(order2);

	Tree** t1_sons = (Tree**)malloc(size_Y * sizeof(Tree*));
	Tree** t2_sons = (Tree**)malloc(size_X * sizeof(Tree*));

	Tree* tmp = order1->son;
	int l = 0;
	while (tmp != NULL) {
		t1_sons[l] = tmp;
		tmp = tmp->brother;
		l++;
	}

	tmp = order2->son;
	l = 0;
	while (tmp != NULL) {
		t2_sons[l] = tmp;
		tmp = tmp->brother;
		l++;
	}

	int** tmp_arr = (int**)malloc(size_Y * sizeof(int*));
	int** pom_arr = (int**)malloc(size_Y * sizeof(int*));
	for (int i = 0; i < size_Y; i++) {
		tmp_arr[i] = (int*)malloc(size_X * sizeof(int));
		pom_arr[i] = (int*)malloc(size_X * sizeof(int));
	}

	for (int i = 0; i < size_Y; i++) {
		for (int j = 0; j < size_X;j++) {
			tmp_arr[i][j] = compare(arr, t1_sons[i], t2_sons[j]);
			pom_arr[i][j] = 0;
		}
	}

	//szukanie sumy maksymalnych elementow z danych rzedow/kolumn
	int max = 0;

	for (int i = 0; i < size_Y; i++) {
		for (int j = 0; j < size_X; j++) {
			max += find_max(tmp_arr, size_Y, size_X, pom_arr);
		}
	}


	int* pom_t1 = (int*)malloc(size_X * sizeof(int));
	int* pom_t2 = (int*)malloc(size_Y * sizeof(int));

	for (int i = 0; i < size_X; i++) {
		pom_t1[i] = compare(arr, order1, t2_sons[i]);
		if (pom_t1[i] > max) {
			max = pom_t1[i];
		}
	}

	for (int i = 0; i < size_Y; i++) {
		pom_t2[i] = compare(arr, t1_sons[i], order2);
		if (pom_t2[i] > max) {
			max = pom_t2[i];
		}
	}
	return max;
}

//dla kazdej pary drzew sprawdzam pokrywanie sie ich wierzcholkow
void check_inner(Tree** order1, Tree** order2, int** arr, int size_y, int size_x) {
	for (int i = size_y - 1; i >= 10; i--) {
		for (int j = size_x - 1; j >= 10; j--) {
			arr[i][j] = compare_inner(arr, order1[i - 10], order2[j - 10]);
		}
	}
}

int main() {
	int size;
	std::cin >> size;

	Root* arr = (Root*)malloc(size * sizeof(Root));
	char** input = (char**)malloc(size * sizeof(char*));
	int x = 0;
	int blank = getchar();
	do
	{
		unsigned int arr_size = 1;
		input[x] = (char*)malloc(sizeof(char));

		if (input[x] != NULL)
		{
			char q;
			unsigned int j = 0;
			while ((q = getchar()) != '\n')
			{
				input[x][j++] = q;
				if (j == arr_size)
				{
					arr_size = j + 1;
					if (char* tmp = (char*)realloc(input[x], arr_size))
					{
						input[x] = tmp;
					}
				}
			}
			input[x][j] = '\0';
			x++;
		}
	} while (x < size);

	for (int i = 0; i < size; i++) {
		arr[i].tree = alloc();
		init_Root(&arr[i]);
		arr[i].tree = read_NEWICK(input[i], arr[i].internal_vert, arr[i].order);
	}

	//petla robiaca porownanie kazdy z kazdym
	int comp = 1;
	for (int i = 0; comp < size; i++) {
		for (int j = 1; j + comp <= size; j++) {
			//w tym miejscu comp i comp+j maja odpowiednie wartosci, zeby je zaczac ze soba porownywac
			int size_Y = arr[comp - 1].internal_vert + 10;
			int size_X = arr[comp + j - 1].internal_vert + 10;

			int** main_arr = alloc_table(size_Y, size_X);
			init_table(main_arr, size_Y, size_X);

			//wypelnienie tabelki - I cwiartka
			init_leaves(main_arr, size_Y, size_X);
			
			check_vert(arr[comp - 1].order, main_arr, size_Y);

			check_hori(arr[comp + j - 1].order, main_arr, size_X);

			int t = count_sons(arr[comp - 1].tree);

			check_inner(arr[comp - 1].order, arr[comp + j - 1].order, main_arr, size_Y, size_X);

			//wyswietlanie tabelki
			/*for (int t2 = 0; t2 <= size_Y; t2++) {
				for (int t1 = 0; t1 <= size_X; t1++) {
					if (t2 == 0 && t1 == 0) std::cout << std::setw(2) << 0 << ' ';
					if (t2 == 0 && t1 != 0) std::cout << std::setw(2) << t1 << ' ';
					if (t1 == 0 && t2 != 0) std::cout << std::setw(2) << t2 << ' ';
					if (t1 >= 1 && t2 >= 1 ) {
						std::cout << std::setw(2) << main_arr[t2 - 1][t1 - 1] << ' ';
					}
				}
				std::cout << '\n';
			}*/

			int max = 0;
			for (int m = 10; m < size_Y; m++) {
				for (int n = 10; m < size_X; n++) {
					if (main_arr[m][n] > max) {
						max = main_arr[m][n];
					}
				}
			}

			std::cout << max;

			for (int f = 0; f < size_Y; f++) {
				free(main_arr[f]);
			}
			//std::cout << comp << '-' << comp + j << '\n';
		}
		comp++;
	}
	
	for (int i = 0; i < size; i++) {
		free(input[i]);
	}
	return 0;
}