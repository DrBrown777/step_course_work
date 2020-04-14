#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <algorithm>

using namespace std;

const int field_size = 9; /*размер игрового поля*/
char Field[field_size]; /*массив под игровое поле*/
char FieldVar[field_size]; /*массив под поле с вариантами хода*/

const int init_weight = 100; /*Начальный вес матрицы весов*/
const int precision_coef = 50; /*Точность генератора хода для Smart игрока*/
const double step_coef = 0.65; /*Коэфициент обучения*/
const int step_learn = 10; /*Шаг обучения*/
const int number_of_games = 1000; /*Кол-во игр которые должен сыграть Smart игрок для обучения*/

bool game_over, wins; /*Флаговые: конец игры, победа*/
int x_wins = 0, o_wins = 0, d_wins = 0; /*накопительные переменные для статы*/

struct DataBase /*База знаний smart игрока*/
{
	char MyField[9];
	int MyWeight[9];
	DataBase() {
		fill(MyWeight, MyWeight + field_size, init_weight);
	}
};

void welcome(); /*Отображает приветствие*/
void start_game(); /*Отображает стартовое меню игры и определяет последовательность вызова функций в зависимоти от типа игры*/
void setup(int*); /*Функция инициализации флаговых переменных*/
void type_symbol(bool*, char*, char*, int*); /*Функция рандомно определяет кто будет играть за X а кто за O*/
void clear_field(); /*Функция очищает игровые поля*/
void display_field(); /*Функция выводит игровое поле*/
int random_player(); /*Функция возвращает возможный ход, случайно, в стратегии Random*/
int input_events(bool*, int*, DataBase*, int*); /*Функция возвращает ход сделанный пользователем с клавиатуры, и выводит оставшиеся варианты хода*/
char check_wins(int*, int*); /*Функция проверяет на победу после каждого хода*/
void wins_stat(char, int*); /*Функция выводит поздравление о выиграше*/
void game_logic(int, int*, bool*, int*, char*, char*); /*Функция логики игры*/
void play_game(int*, DataBase*, int*); /*Функция loop цикла 1 партии*/

DataBase* push_database(DataBase*, int*);
int get_situation(DataBase*, int*);
int get_smart_random(int*);
void smart_learn(); /*Рекурсивная функция обучения*/

int main()
{
	setlocale(0, "");
	srand(unsigned(time(0)));

	welcome();
	start_game();
	return 0;
}

void welcome()
{
	cout << "\t" << "*Добро пожаловать в Tic-Tac-Toe!*" << "\n\n";
}

void start_game()
{
	int menu;
	bool loop = true;
	int type_game; /*тип игры, Random или Smart*/
	
	int size_database = 0;
	DataBase* Collections = new DataBase[size_database];

	while (loop == true)
	{
		cout << endl;
		cout << "\tВыберите вариант игры:" << endl;
		cout << "\t1 - Против компьютера \"Random\" стратегия" << endl;
		cout << "\t2 - Против компьютера \"Smart\" стратегия" << endl;
		cout << "\t3 - Обучить \"Smart\" игрока" << endl;
		cout << "\t0 - Выход" << endl;

		cin >> menu;

		switch (menu)
		{
		case 1:
			type_game = 1;
			play_game(&type_game, Collections, &size_database);
			break;
		case 2:
		{
			type_game = 2;
			play_game(&type_game, Collections, &size_database);
			break;
		}
		case 3:
			type_game = 3;
			for (int i = 0; i < number_of_games; i++)
			{
				play_game(&type_game, Collections, &size_database);
			}
			//cout << "Игрок Smart теперь очень умный!" << "\n\n";
			cout << "Победы X: " << x_wins << " Победы O: " << o_wins << " Ничьи: " << d_wins << "\n\n";
			for (size_t i = 0; i < size_database; i++)
			{
				cout << Collections[i].MyField << endl;
				
				for (size_t j = 0; j < size_database; j++)
				{
					cout << Collections[i].MyField[j] << endl;
				}
				cout << endl;
			}
			break;
		default:
			break;
		}
		cout << "Играем еще? (1 / 0 - Да / Нет) ";
		cin >> loop;
	}
}

void play_game(int* type_game, DataBase* Collections, int* size_database)
{
	int move; /*Переменная содержит ход*/
	bool turn; /*Очередь хода*/
	char player_1, player_2; /*символьные переменные кто за что играет*/
	int draw; /*Проверка на ничью*/

	setup(&draw);
	type_symbol(&turn, &player_1, &player_2, type_game);

	while (game_over != true)
	{
		if (*type_game == 1 || *type_game == 2)
			display_field();
		move = input_events(&turn, type_game, Collections, size_database);
		draw++;
		game_logic(move, &draw, &turn, type_game, &player_1, &player_2);
	}
}

int input_events(bool* turn, int* type_game, DataBase* Collections, int* size_database)
{
	int move;
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
		if (get_situation(Collections, size_database) != 0)
			Collections = push_database(Collections, size_database);
		
		int index = get_situation(Collections, size_database);

		return get_smart_random(Collections[index].MyWeight);
	}
	else
	{
		return random_player();
	}
	return move;
}

void game_logic(int move, int* draw, bool* turn, int* type_game, char* player_1, char* player_2)
{
	char XOD;
	if ((*turn && *type_game == 1) || (*turn && *type_game == 2))
	{
		Field[move - 1] = *player_1;
		FieldVar[move - 1] = '-';
		XOD = check_wins(draw, type_game);
		if (wins)
		{
			wins_stat(XOD, type_game);
			return;
		}
		*turn = false;
	}
	else if ((*turn && *type_game == 3) || *type_game == 2)
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
		XOD = check_wins(draw, type_game);
		if (wins)
		{
			wins_stat(XOD, type_game);
			return;
		}
		*type_game == 2 ? *turn = true : *turn = false;
	}
	else
	{
		Field[move] = *player_2;
		if (*type_game == 1 || *type_game == 2) FieldVar[move] = '-';
		XOD = check_wins(draw, type_game);
		if (wins)
		{
			wins_stat(XOD, type_game);
			return;
		}
		*turn = true;
	}
}

void setup(int* draw)
{	
	game_over = false;
	wins = false;
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

char check_wins(int* draw, int* type_game)
{
	int victory[8][3] = { {0, 1 , 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

	for (int i = 0; i < field_size - 1; i++)
	{
		if (Field[victory[i][0]] == Field[victory[i][1]] &&
			Field[victory[i][0]] == Field[victory[i][2]] &&
			Field[victory[i][0]] != ' ')
		{
			wins = true;
			/*if (*type_game != 3)*/ Field[victory[i][0]] == 'X' ? x_wins++ : o_wins++;
			return Field[victory[i][0]] == 'X' ? 'X' : 'O';
		}
	}
	if (*draw == 9)
	{
		wins = true;
		/*if (*type_game != 3)*/ d_wins++;
		return 'D';
	}
	return 'U';
}

void wins_stat(char XOD, int* type_game)
{
	if (*type_game == 1 || *type_game == 2)
	{
		display_field();
		if (XOD != 'D')
			cout << "\t" << "Поздравляем, победили - " << XOD << " !\n\n";
		else
			cout << "\t" << "Спасибо за игру, это ничья!" << "\n\n";
		cout << "Победы X: " << x_wins << " Победы O: " << o_wins << " Ничьи: " << d_wins << "\n\n";
	}

	game_over = true;
}

DataBase* push_database(DataBase* Collect, int* size)
{
	DataBase* Temp = new DataBase[*size + 1];

	for (int i = 0; i < *size; i++)
	{
		Temp[i] = Collect[i];
	}

	strcpy(Temp[*size].MyField, Field);

	for (int i = 0; i < 9; i++)
	{
		if (strncmp(&Temp[*size].MyField[i], " ", 1) != 0)
		{
			Temp[*size].MyWeight[i] = 0;
		}
	}

	//delete[] Collect;

	(*size)++;

	return Temp;
}

int get_situation(DataBase* Collect, int* size)
{

	for (int i = 0; i < *size; i++)
	{
		if (strcmp(Collect[i].MyField, Field) == 0)
		{
			return i;
		}
	}
	return -1;
}

int get_smart_random(int* mas)
{
	int summ = 0, count = 0, move = 0;
	int mas_temp[9];

	for (int i = 0; i < 9; i++)
	{
		summ += mas[i];
		mas_temp[i] = mas[i];
	}

	/*Дописать проверку на summ == 0 когда все хода одинаково плохи*/

	for (int i = 0; i < 9; i++)
	{
		mas_temp[i] = (mas_temp[i] / (double)summ) * precision_coef; //нормализация весов
	}

	for (int i = 0; i < 9; i++)
	{
		count += mas_temp[i];
	}

	int* new_mas = new int[count];

	for (int i = 0, j = 0; i < 9; i++)
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

	return move; //return хода на основании матрицы весов 
}

void smart_learn()
{

}