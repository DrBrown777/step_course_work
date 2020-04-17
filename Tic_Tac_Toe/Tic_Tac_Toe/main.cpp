/*v.1.16 by dr_brown777*/

#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <algorithm>

//#define DEBUG_yes

using namespace std;

const int field_size = 9; /*размер игрового поля*/
const int init_weight = 100; /*Начальный вес матрицы весов*/
const int precision_coef = 50; /*Точность генератора хода для Smart игрока*/
const double step_coef = 0.65; /*Коэфициент обучения*/
const int step_learn = 20; /*Шаг обучения*/
const int number_of_games = 100000; /*Кол-во игр которые должен сыграть Smart игрок для обучения*/

char Field[field_size]; /*массив под игровое поле*/
char FieldVar[field_size]; /*массив под поле с вариантами хода*/

int size_database_X = 0;/*Размер базы данных X*/
int size_database_O = 0;/*Размер базы данных O*/

struct DataBase /*База знаний smart игрока*/
{
	char MyField[9]; //Ситуация на поле
	int MyWeight[9]; //Матрица весов
	DataBase() {
		fill(MyWeight, MyWeight + field_size, init_weight);
	}
};

DataBase* Collections_X = new DataBase[size_database_X];/*База для X комбинаций*/
DataBase* Collections_O = new DataBase[size_database_O];/*База для O комбинаций*/

struct Stack /*История выполненых ходов Smart игрока в текущеей партии для функции Learn*/
{
	int current_move;
	int index_weight;
};

void welcome(); /*Отображает приветствие*/
void start_game(); /*Отображает стартовое меню игры и определяет последовательность вызова функций в зависимоти от типа игры*/
void setup(int*, bool*, bool*); /*Функция инициализации флаговых переменных*/
void type_symbol(bool*, char*, char*, int*); /*Функция рандомно определяет кто будет играть за X а кто за O*/
void clear_field(); /*Функция очищает игровые поля*/
void display_field(); /*Функция выводит игровое поле*/
int random_player(); /*Функция возвращает возможный ход, случайно, в стратегии Random*/
int input_events(bool*, int*, Stack**, int*, char*, char*); /*Функция возвращает ход сделанный пользователем с клавиатуры, и выводит оставшиеся варианты хода*/
char check_wins(int*, int*, int*, int*, int*, bool*); /*Функция проверяет на победу после каждого хода*/
void wins_stat(char, int*, int*, int*, int*, bool*); /*Функция выводит поздравление о выиграше*/
void game_logic(int, int*, bool*, int*, char*, char*, int*, int*, int*, bool*, bool*, Stack**, int*); /*Функция логики игры*/
void play_game(int*, int*, int*, int*); /*Функция loop цикла 1 партии*/

DataBase* push_database(DataBase*, int*); /*Добавляет в базу данных неизвестную ситуацию на поле*/
Stack* push_stack(int, int, Stack*, int*); /*Добавляет в Stack текущий ход игрока Smart*/
int get_situation(DataBase*, int); /*Ищет в базе сложившнюся ситуацию на поле*/
int get_smart_random(int, DataBase*); /*Генерирует ход Smart игрока на основании матрицы весов*/
void smart_learn(Stack*, int, int, DataBase*); /*Рекурсивная функция обучения, уменьшает вес хода в случае проигрыша, и увеличивет наооборот, ничья нейтрально*/

int main()
{
	setlocale(0, "");
	srand(unsigned(time(0)));

	welcome();
	start_game();

	delete[] Collections_X;
	delete[] Collections_O;
	return 0;
}

void start_game()
{
	int menu;
	bool loop = true;
	int type_game; /*тип игры, Random или Smart*/
	int x_wins = 0, o_wins = 0, d_wins = 0; /*накопительные переменные для статы*/

	while (loop == true)
	{
		cout << endl;
		cout << "\tВыберите вариант игры:" << endl;
		cout << "\t1 - Тренеровочная партия \"Random\" стратегия" << endl;
		cout << "\t2 - Против компьютера \"Smart\" стратегия" << endl;
		cout << "\t3 - Обучить \"Smart\" игрока" << endl;
		cout << "\t0 - Выход" << endl;

		cin >> menu;

		switch (menu)
		{
		case 1:
			type_game = 1;
			play_game(&type_game, &x_wins, &o_wins, &d_wins);
			break;
		case 2:
		{
			type_game = 2;
			play_game(&type_game, &x_wins, &o_wins, &d_wins);
#ifdef DEBUG_yes
			for (size_t i = 0; i < size_database_O; i++)
			{
				cout << Collections_O[i].MyField << endl;

				for (size_t j = 0; j < 9; j++)
				{
					cout << Collections_O[i].MyWeight[j] << " ";
				}
				cout << endl;
			}
#endif // DEBUG_yes	
			break;
		}
		case 3:
			type_game = 3;
			for (int i = 0; i < number_of_games; i++)
			{
				play_game(&type_game, &x_wins, &o_wins, &d_wins);
			}
			cout << "Игрок Smart теперь очень умный!" << "\n\n";
#ifdef DEBUG_yes
			for (size_t i = 0; i < size_database_O; i++)
			{
				cout << Collections_O[i].MyField << endl;
				
				for (size_t j = 0; j < 9; j++)
				{
					cout << Collections_O[i].MyWeight[j] << " ";
				}
				cout << endl;
			}
#endif // DEBUG_yes			
			break;
		default:
			break;
		}
		cout << "Играем еще? (1 / 0 - Да / Нет) ";
		cin >> loop;
	}
}

void play_game(int* type_game, int* x_wins, int* o_wins, int* d_wins)
{
	int move; /*Переменная содержит ход*/
	bool turn; /*Очередь хода*/
	char player_1, player_2; /*символьные переменные кто за что играет*/
	int draw; /*Проверка на ничью*/
	bool game_over, wins; /*Флаговые: конец игры, победа*/
	
	int stack_size = 0; /*Размер Стека ходов*/
	Stack* Hystory = new Stack[stack_size]; /*Стек ходов текущей партии*/

	setup(&draw, &game_over, &wins);
	
	type_symbol(&turn, &player_1, &player_2, type_game);

	while (game_over != true)
	{
		if (*type_game == 1 || *type_game == 2)
			display_field();
		move = input_events(&turn, type_game, &Hystory, &stack_size, &player_1, &player_2);
		draw++;
		game_logic(move, &draw, &turn, type_game, &player_1, &player_2, x_wins, o_wins, d_wins, &game_over, &wins, &Hystory, &stack_size);
	}
#ifdef DEBUG_yes
	for (size_t i = 0; i < stack_size; i++)
	{
		cout << Hystory[i].current_move << " -> " << Hystory[i].index_weight << endl;
	}
#endif // DEBUG_yes
	delete[] Hystory;
}

int input_events(bool* turn, int* type_game, Stack** Hystory, int* stack_size, char* player_1, char* player_2)
{
	int move, index;

	if ((*turn && *type_game == 1) || (*turn && *type_game == 2))
	{
		cout << "Варианты хода:" << "\n\n";
		for (int i = 0; i < field_size; i++)
		{
			if (i == 2 || i == 5 || i == 8)
				cout << "\t" << setw(2) << "-" << FieldVar[i] << "-" << endl;
			else
				cout << "\t" << setw(2) << "-" << FieldVar[i] << "-  |";
		}
		cout << endl;
		do
		{
			cout << "Сделайте ход (1-9) -> ";
			cin >> move;
		} while (move < 1 || move > 9 || Field[move - 1] == 'X' || Field[move - 1] == 'O');
	}
	else if ((*turn && *type_game == 3) || *type_game == 2)
	{
		if (*player_1 == 'X' && *player_2 == 'O' && *type_game == 2)
		{
			if (get_situation(Collections_O, size_database_O) == -1)
				Collections_O = push_database(Collections_O, &size_database_O);

			index = get_situation(Collections_O, size_database_O);

			move = get_smart_random(index, Collections_O);
		}
		else if (*player_1 == 'O' && *player_2 == 'X' && *type_game == 2)
		{
			if (get_situation(Collections_X, size_database_X) == -1)
				Collections_X = push_database(Collections_X, &size_database_X);

			index = get_situation(Collections_X, size_database_X);

			move = get_smart_random(index, Collections_X);
		}
		else if (*player_1 == 'O' && *player_2 == 'X' && *type_game == 3)
		{
			if (get_situation(Collections_O, size_database_O) == -1)
				Collections_O = push_database(Collections_O, &size_database_O);

			index = get_situation(Collections_O, size_database_O);

			move = get_smart_random(index, Collections_O);
		}
		else if (*player_1 == 'X' && *player_2 == 'O' && *type_game == 3)
		{
			if (get_situation(Collections_X, size_database_X) == -1)
				Collections_X = push_database(Collections_X, &size_database_X);

			index = get_situation(Collections_X, size_database_X);

			move = get_smart_random(index, Collections_X);
		}

		*Hystory = push_stack(move, index, *Hystory, stack_size);

		return move;
	}
	else
	{
		return random_player();
	}
	return move;
}

void game_logic(int move, int* draw, bool* turn, int* type_game, char* player_1, char* player_2,
	int* x_wins, int* o_wins, int* d_wins, bool* game_over, bool* wins, Stack** Hystory, int* stack_size)
{
	char XOD;
	if ((*turn && *type_game == 1) || (*turn && *type_game == 2))/*Ветка Человека*/
	{
		Field[move - 1] = *player_1;
		FieldVar[move - 1] = '-';
		XOD = check_wins(draw, type_game, x_wins, o_wins, d_wins, wins);
		if (*wins)
		{
			if (XOD != 'D' && *type_game == 2 && *player_1 == 'X')
			{
				smart_learn(*Hystory, *stack_size - 1, -step_learn, Collections_O);
			}
			else if (XOD != 'D' && *type_game == 2 && *player_1 == 'O')
			{
				smart_learn(*Hystory, *stack_size - 1, -step_learn, Collections_X);
			}
			wins_stat(XOD, type_game, x_wins, o_wins, d_wins, game_over);
			return;
		}
		*turn = false;
	}
	else if ((*turn && *type_game == 3) || *type_game == 2) /*Ветка Smart игрока*/
	{
		if (*type_game == 2)
		{
			Field[move] = *player_2;
			FieldVar[move] = '-';
		}
		else
		{
			Field[move] = *player_1;
		}
		XOD = check_wins(draw, type_game, x_wins, o_wins, d_wins, wins);
		if (*wins)
		{
			if (XOD != 'D' && *player_2 == 'X' && *type_game == 2)
			{
				smart_learn(*Hystory, *stack_size - 1, step_learn, Collections_X);
			}
			else if (XOD != 'D' && *player_2 == 'O' && *type_game == 2)
			{
				smart_learn(*Hystory, *stack_size - 1, step_learn, Collections_O);
			}
			else if (XOD != 'D' && *player_1 == 'X' && *type_game == 3)
			{
				smart_learn(*Hystory, *stack_size - 1, step_learn, Collections_X);
			}
			else if (XOD != 'D' && *player_1 == 'O' && *type_game == 3)
			{
				smart_learn(*Hystory, *stack_size - 1, step_learn, Collections_O);
			}
			wins_stat(XOD, type_game, x_wins, o_wins, d_wins, game_over);
			return;
		}
		*type_game == 2 ? *turn = true : *turn = false;
	}
	else /*Ветка Random игрока*/
	{
		Field[move] = *player_2;
		if (*type_game == 1 || *type_game == 2) FieldVar[move] = '-';
		XOD = check_wins(draw, type_game, x_wins, o_wins, d_wins, wins);
		if (*wins)
		{
			if (XOD != 'D' && *type_game != 1 && *player_2 == 'X')
			{
				smart_learn(*Hystory, *stack_size - 1, -step_learn, Collections_O);
			}
			else if (XOD != 'D' && *type_game != 1 && *player_2 == 'O')
			{
				smart_learn(*Hystory, *stack_size - 1, -step_learn, Collections_X);
			}
			wins_stat(XOD, type_game, x_wins, o_wins, d_wins, game_over);
			return;
		}
		*turn = true;
	}
}

void smart_learn(Stack* Hystory, int iterator, int step_learn, DataBase* Collections)
{	
	if (iterator < 0)
		return;

	Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move] += step_learn;

	if ((Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move]) < 0)
	{
		Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move] = 0;
	}

	smart_learn(Hystory, --iterator, step_learn * step_coef, Collections);
}

Stack* push_stack(int move, int index, Stack* Hystory, int* stack_size)
{
	Stack* Temp = new Stack[*stack_size + 1];

	for (int i = 0; i < *stack_size; i++)
	{
		Temp[i] = Hystory[i];
	}

	Temp[*stack_size].current_move = move;
	Temp[*stack_size].index_weight = index;
	
	delete[] Hystory;

	(*stack_size)++;

	return Temp;
}

DataBase* push_database(DataBase* Collections, int* size_database)
{
	DataBase* Temp = new DataBase[*size_database + 1];

	for (int i = 0; i < *size_database; i++)
	{
		Temp[i] = Collections[i];
	}

	strcpy(Temp[*size_database].MyField, Field);

	for (int i = 0; i < field_size; i++)
	{
		if (strncmp(&Temp[*size_database].MyField[i], " ", 1) != 0)
		{
			Temp[*size_database].MyWeight[i] = 0;
		}
	}

	delete[] Collections;

	(*size_database)++;

	return Temp;
}

int get_situation(DataBase* Collections, int size_database)
{

	for (int i = 0; i < size_database; i++)
	{
		if (strcmp(Collections[i].MyField, Field) == 0)
		{
			return i;
		}
	}
	return -1;
}

int get_smart_random(int index, DataBase* Collections)
{
	int summ = 0, count = 0, move = 0;
	int mas_temp[9];

	for (int i = 0; i < field_size; i++)
	{
		summ += Collections[index].MyWeight[i];
		mas_temp[i] = Collections[index].MyWeight[i];
	}
	if (summ == 0) return random_player();

	for (int i = 0; i < field_size; i++)
	{
		mas_temp[i] = (mas_temp[i] / (double)summ) * precision_coef;
	}
	for (int i = 0; i < field_size; i++)
	{
		count += mas_temp[i];
	}

	int* new_mas = new int[count];

	for (int i = 0, j = 0; i < field_size; i++)
	{
		int tmp = mas_temp[i];
		while (tmp != 0)
		{
			new_mas[j] = i;
			j++;
			tmp--;
		}
	}

	random_shuffle(new_mas, new_mas + count);

	move = new_mas[rand() % count];

	delete[] new_mas;

	return move;
}

int random_player()
{
	int move = 0, count = 0;

	for (int i = 0; i < field_size; i++)
	{
		if (Field[i] == ' ') count++;
	}

	int* pTmp = new int[count];

	for (int i = 0, j = 0; i < field_size; i++)
	{
		if (Field[i] == ' ')
		{
			pTmp[j] = i;
			j++;
		}
	}

	move = pTmp[rand() % count];

	delete[] pTmp;

	return move;
}

void setup(int* draw, bool* game_over, bool* wins)
{
	*game_over = false;
	*wins = false;
	*draw = 0;
	clear_field();
}

void type_symbol(bool* turn, char* player_1, char* player_2, int* type_game)
{
	int type_symbol = rand() % 2 + 1;

	if (type_symbol == 2)
	{
		*player_1 = 'O'; *player_2 = 'X';
		*turn = false;
	}
	else
	{
		*player_1 = 'X'; *player_2 = 'O';
		*turn = true;
	}

	if ((type_symbol == 2 && *type_game == 1) || (type_symbol == 2 && *type_game == 2))
	{
		cout << "\tСлучайным образом определено что Вы играете за нолики O." << endl;
		cout << "\tПротивник играет за крестики Х (крестики ходят первыми)." << endl;
		cout << endl;
		system("pause");
	}
	else if ((type_symbol == 1 && *type_game == 1) || (type_symbol == 1 && *type_game == 2))
	{
		cout << "\tСлучайным образом определено что Вы играете за крестики Х" << endl;
		cout << "\t(крестики ходят первыми). Противник играет за нолики O." << endl;
		cout << endl;
		system("pause");
	}
}

void clear_field()
{
	for (int i = 0; i < field_size; i++)
	{
		Field[i] = ' ';
		FieldVar[i] = (i + 1) + '0';
	}
}

void display_field()
{
	system("cls");
	cout << "\n\n";
	for (int i = 0; i < field_size; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << setw(2) << "-" << Field[i] << "-" << endl;
		else
			cout << "\t" << setw(2) << "-" << Field[i] << "-  |";
	}
	cout << "\n\n";
}

char check_wins(int* draw, int* type_game, int* x_wins, int* o_wins, int* d_wins, bool* wins)
{
	int victory[8][3] = { {0, 1 , 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

	for (int i = 0; i < field_size - 1; i++)
	{
		if (Field[victory[i][0]] == Field[victory[i][1]] &&
			Field[victory[i][0]] == Field[victory[i][2]] &&
			Field[victory[i][0]] != ' ')
		{
			*wins = true;
			if (*type_game != 3) Field[victory[i][0]] == 'X' ? (*x_wins)++ : (*o_wins)++;
			return Field[victory[i][0]] == 'X' ? 'X' : 'O';
		}
	}
	if (*draw == 9)
	{
		*wins = true;
		if (*type_game != 3) (*d_wins)++;
		return 'D';
	}
	return 'U';
}

void wins_stat(char XOD, int* type_game, int* x_wins, int* o_wins, int* d_wins, bool* game_over)
{
	if (*type_game == 1 || *type_game == 2)
	{
		display_field();
		if (XOD != 'D')
			cout << "\t" << "Поздравляем, победили - " << XOD << " !\n\n";
		else
			cout << "\t" << "Спасибо за игру, это ничья!" << "\n\n";
		cout << "Победы X: " << *x_wins << " Победы O: " << *o_wins << " Ничьи: " << *d_wins << "\n\n";
	}

	*game_over = true;
}

void welcome()
{
	cout << "\t" << "*Добро пожаловать в Tic-Tac-Toe!*" << "\n\n";
}