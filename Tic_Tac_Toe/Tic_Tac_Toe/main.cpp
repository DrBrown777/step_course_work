#include <iostream>
#include <iomanip>
#include <ctime>

using namespace std;

bool game_over, wins, step; /*Флаговые: конец игры, смена хода, победа*/

int turn; /*Проверка на ничью*/
int type_game;
int x_wins = 0, o_wins = 0, d_wins = 0; /*накопительные переменные для статы*/
char symbol_player_1, symbol_player_2;

int field_size = 9; /*размер игрового поля*/
char* pField = new char[field_size]; /*массив под игровое поле*/
char* pFieldVar = new char[field_size]; /*массив под поле с вариантами хода*/


void name() /*Отображает приветствие*/
{
	cout << "\t" << "*Добро пожаловать в Tic-Tac-Toe!*" << "\n\n";
}

void setup() /*Функция инициализации флаговых переменных*/
{
	game_over = false;
	wins = false;
	turn = 0;
	type_game = 0;
}

void type_symbol() /*Функция рандомно определяет кто будет играть за X а кто за O*/
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

void clear_field(char* pField, char* pFieldVar, int field_size) /*Функция очищает игровые поля*/
{
	for (int i = 0; i < field_size; i++)
	{
		pField[i] = ' ';
		pFieldVar[i] = (i+1) + '0';
	}
}

void draw_field() /*Функция выводит игровое поле*/
{
	system("cls");
	cout << "\n\n";
	for (int i = 0; i < field_size; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << setw(2) << "-" << pField[i] << "-" << endl;
		else
			cout << "\t" << setw(2) << "-" << pField[i] << "-  |";
	}
	cout << "\n\n";
}

int random_player() /*Функция возвращает возможный ход, случайно, в стратегии Random*/
{
	int move = 0, count = 0;
	
	for (int i = 0; i < field_size; i++)
	{
		if (pField[i] == ' ') count++;
	}
	
	int* pTmp = new int[count];
	
	for (int i = 0, j = 0; i < field_size; i++)
	{
		if (pField[i] == ' ')
		{
			pTmp[j] = i;
			j++;
		}
	}

	int i = rand() % count;
	
	move = pTmp[i];
	
	delete[] pTmp;

	return move;
}

int input_events() /*Функция возвращает ход сделанный пользователем с клавиатуры, и выводит оставшиеся варианты хода*/
{
	int move;
	cout << "Варианты хода:" << "\n\n";
	for (int i = 0; i < 9; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << setw(2) << "-" << pFieldVar[i] << "-" << endl;
		else
			cout << "\t" << setw(2) << "-" << pFieldVar[i] << "-  |";
	}
	cout << endl;
	if (step)
	{
		do
		{
			cout << "Сделайте ход (1-9) -> ";
			cin >> move;
		} while (move < 1 || move > 9 || pField[move - 1] == 'X' || pField[move - 1] == 'O');
	}
	else
	{
		move = random_player();
	}

	return move;
}

char check_wins() /*Функция проверяет на победу после каждого хода*/
{
	int victory[8][3] = {{0, 1 , 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6}};
	turn++;
	for (int i = 0; i < 8; i++)
	{
		if (pField[victory[i][0]] == pField[victory[i][1]] &&
			pField[victory[i][0]] == pField[victory[i][2]] &&
			pField[victory[i][0]] != ' ')
		{
			wins = true;
			pField[victory[i][0]] == 'X' ? x_wins++ : o_wins++;
			return pField[victory[i][0]] == 'X' ? 'X' : 'O';
		}
		else if (turn == 9)
		{
			wins = true;
			d_wins++;
			return 'D';
		}
	}
}

void start_game()
{
	int menu;

	setup();

	cout << "\tВыберите вариант игры:" << endl;
	cout << "\t1 - Против компьютера \"Random\" стратегия" << endl;
	cout << "\t2 - Против компьютера \"Smart\" стратегия" << endl;
	cout << "\t0 - Выход" << endl;

	cin >> menu;

	switch (menu)
	{
	case 1:
		type_game = 1;
		type_symbol();
		break;
	case 2:
		type_game = 2;
		type_symbol();
		break;
	default:
		game_over = true;
		break;
	}

	clear_field(pField, pFieldVar, field_size);
}

void wins_stat(char XOD) /*Функция выводит поздравление о выиграше*/
{
	char val;
	draw_field();
	if (XOD != 'D')
		cout << "\t" << "Поздравляем, победили - " << XOD << " !\n\n";
	else
		cout << "\t" << "Спасибо за игру, это ничья!" << "\n\n";

	cout << "Победы X: " << x_wins << " Победы O: " << o_wins << " Ничьи: "<< d_wins << "\n\n";
	cout << "Сыграем еще? ";
	cin >> val;
	if (val != 'y')
	{
		game_over = true;
	}
	else
	{
		start_game();
	}
}

void game_logic(int move) /*Функция логики игры*/
{
	char XOD;
	if (step)
	{
		pField[move - 1] = symbol_player_1;
		pFieldVar[move - 1] = '-';
		XOD = check_wins();
		if (wins)
		{
			wins_stat(XOD);
		}
		step = false;
	}
	else
	{
		pField[move] = symbol_player_2;
		pFieldVar[move] = '-';
		XOD = check_wins();
		if (wins)
		{
			wins_stat(XOD);
		}
		step = true;
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	srand(unsigned(time(0)));

	int move;

	name();

	start_game();

	while (game_over != true && type_game == 1)
	{
		draw_field();
		move = input_events();
		game_logic(move);
	}

	delete[] pField;
	delete[] pFieldVar;
	return 0;
}