#include <iostream>
#include <iomanip>
#include <ctime>

using namespace std;

bool game_over, wins, step; /*Флаговые: конец игры, смена хода, победа*/

int turn; /*Проверка на ничью*/
int type_game; /*тип игры, Random или Smart*/
int x_wins = 0, o_wins = 0, d_wins = 0; /*накопительные переменные для статы*/
char symbol_player_1, symbol_player_2; /*символьные переменные кто за что играет*/

const int field_size = 9; /*размер игрового поля*/
char Field[field_size]; /*массив под игровое поле*/
char FieldVar[field_size]; /*массив под поле с вариантами хода*/

void welcome(); /*Отображает приветствие*/
void start_game(); /*Отображает стартовое меню игры*/
void setup(); /*Функция инициализации флаговых переменных*/
void type_symbol(); /*Функция рандомно определяет кто будет играть за X а кто за O*/
void clear_field(); /*Функция очищает игровые поля*/
void draw_field(); /*Функция выводит игровое поле*/
int random_player(); /*Функция возвращает возможный ход, случайно, в стратегии Random*/
int input_events(); /*Функция возвращает ход сделанный пользователем с клавиатуры, и выводит оставшиеся варианты хода*/
char check_wins(); /*Функция проверяет на победу после каждого хода*/
void wins_stat(char); /*Функция выводит поздравление о выиграше*/
void game_logic(int); /*Функция логики игры*/
void play_game(); /*Функция loop цикла игры*/

int main()
{
	setlocale(LC_ALL, "Russian");
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
			setup();
			type_symbol();
			clear_field();
			play_game();
			break;
		case 2:
		{
			while ((x_wins + o_wins + d_wins) != 100000)
			{
				type_game = 2;
				setup();
				int type_symbol = rand() % 2 + 1;
				if (type_symbol == 2)
				{
					symbol_player_1 = 'O'; symbol_player_2 = 'X';
					step = false;
				}
				else
				{
					symbol_player_1 = 'X'; symbol_player_2 = 'O';
					step = true;
				}
				clear_field();
				play_game();
			}
			cout << "Победы X: " << x_wins << " Победы O: " << o_wins << " Ничьи: " << d_wins << "\n\n";
			break;
		}
		case 3:
			type_game = 3;
			break;
		default:
			break;
		}
		cout << "Играем еще? (1 / 0 - Да / Нет) ";
		cin >> loop;
	}
}

void setup()
{
	game_over = false;
	wins = false;
	turn = 0;
}

void type_symbol()
{
	int type_symbol = rand() % 2 + 1;

	if (type_symbol == 2)
	{
		cout << "\tСлучайным образом определено что Вы играете за нолики O." << endl;
		cout << "\tПротивник играет за крестики Х (крестики ходят первыми)." << endl;
		symbol_player_1 = 'O'; symbol_player_2 = 'X';
		step = false;
		cout << endl;
		system("pause");
	}
	else
	{
		cout << "\tСлучайным образом определено что Вы играете за крестики Х" << endl;
		cout << "\t(крестики ходят первыми). Противник играет за нолики O." << endl;
		symbol_player_1 = 'X'; symbol_player_2 = 'O';
		step = true;
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

void draw_field()
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

int input_events()
{
	int move;
	if (step && type_game == 1)
	{
		cout << "Варианты хода:" << "\n\n";
		for (int i = 0; i < 9; i++)
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
	else if (step && type_game == 2)
	{
		return random_player();
	}
	else
	{
		return random_player();
	}
	return move;
}

char check_wins()
{
	int victory[8][3] = { {0, 1 , 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

	for (int i = 0; i < 8; i++)
	{
		if (Field[victory[i][0]] == Field[victory[i][1]] &&
			Field[victory[i][0]] == Field[victory[i][2]] &&
			Field[victory[i][0]] != ' ')
		{
			wins = true;
			Field[victory[i][0]] == 'X' ? x_wins++ : o_wins++;
			return Field[victory[i][0]] == 'X' ? 'X' : 'O';
		}
	}
	if (turn == 9)
	{
		wins = true;
		d_wins++;
		return 'D';
	}
}

void wins_stat(char XOD)
{
	if (type_game == 1)
	{
		draw_field();
		if (XOD != 'D')
			cout << "\t" << "Поздравляем, победили - " << XOD << " !\n\n";
		else
			cout << "\t" << "Спасибо за игру, это ничья!" << "\n\n";
		cout << "Победы X: " << x_wins << " Победы O: " << o_wins << " Ничьи: " << d_wins << "\n\n";
	}

	game_over = true;
}

void game_logic(int move)
{
	char XOD;
	if (step && type_game == 1)
	{
		Field[move - 1] = symbol_player_1;
		FieldVar[move - 1] = '-';
		XOD = check_wins();
		if (wins)
		{
			wins_stat(XOD);
			return;
		}
		step = false;
	}
	else if (step && type_game == 2)
	{
		Field[move] = symbol_player_1;
		//FieldVar[move] = '-';
		XOD = check_wins();
		if (wins)
		{
			wins_stat(XOD);
			return;
		}
		step = false;
	}
	else
	{
		Field[move] = symbol_player_2;
		//FieldVar[move] = '-';
		XOD = check_wins();
		if (wins)
		{
			wins_stat(XOD);
			return;
		}
		step = true;
	}
}

void play_game()
{
	int move;

	while (game_over != true)
	{
		if (type_game == 1)
			draw_field();
		move = input_events();
		turn++;
		game_logic(move);
	}
}