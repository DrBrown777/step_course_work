﻿#include <iostream>
#include <iomanip>
#include <ctime>

using namespace std;

bool game_over, step, wins;

int field_size = 9;
char* pField = new char[field_size];
char* pFieldVar = new char[field_size];


void name()
{
	cout << "\t" << "*Добро пожаловать в Tic-Tac-Toe!*" << "\n\n";
}

void setup()
{
	game_over = false;
	wins = false; /*Дописать вывод позравления о выиграше и стату*/
}

void type_symbol(char* player_1, char* player_2)
{
	int type_symbol = rand() % 2 + 1;

	if (type_symbol == 2)
	{
		cout << "\tСлучайным образом определено что Вы играете за нолики O." << endl;
		cout << "\tПротивник играет за крестики Х (крестики ходят первыми)." << endl;
		*player_1 = 'O'; *player_2 = 'X';
		step = false;
		cout << endl;
		system("pause");
	}
	else
	{
		cout << "\tСлучайным образом определено что Вы играете за крестики Х" << endl;
		cout << "\t(крестики ходят первыми). Противник играет за нолики O." << endl;
		*player_1 = 'X'; *player_2 = 'O';
		step = true;
		cout << endl;
		system("pause");
	}
}

void clear_field(char* pField, char* pFieldVar, int field_size)
{
	for (int i = 0; i < field_size; i++)
	{
		pField[i] = ' ';
		pFieldVar[i] = (i+1) + '0';
	}
}

void draw_field()
{
	system("cls");
	if (step)
		cout << "\t\t*Ходит игрок*" << "\n\n";
	else
		cout << "\t\t*Ходит противник*" << "\n\n";
	for (int i = 0; i < field_size; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << setw(2) << "-" << pField[i] << "-" << endl;
		else
			cout << "\t" << setw(2) << "-" << pField[i] << "-  |";
	}
	cout << "\n\n";
}

int random_player()/*Не работает*/
{
	int move;
	int count = 0;
	for (int i = 0; i < field_size; i++)
	{
		if (pFieldVar[i] == '-') count++;
	}
	int* tmp = new int[count];
	for (int i = 0; i < field_size; i++)
	{
		if (pFieldVar[i] == '-') tmp[i] = i;
	}
	int i = rand() % count + 1;
	move = tmp[i];
	cout << move << endl;
	//delete[]tmp;
	system("pause");
	return move;
}

int input_events()
{
	int move;
	cout << "Вариант хода:" << "\n\n";
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

void game_logic(int type_game, int move, char symbol_player_1, char symbol_player_2)
{
	if (step)
	{
		pField[move - 1] = symbol_player_1;
		pFieldVar[move - 1] = '-';
		/*Нужна проверка на выигрыш*/
		step = false;
	}
	else
	{
		system("pause");
		pField[move - 1] = symbol_player_2;
		pFieldVar[move - 1] = '-';
		/*Нужна проверка на выигрыш*/
		step = true;
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	srand(unsigned(time(0)));

	int menu, type_game, move;
	char symbol_player_1, symbol_player_2;

	setup(); 
	
	name();
	cout << "\tВыберите вариант игры:" << endl;
	cout << "\t1 - Против компьютера \"Random\" стратегия" << endl;
	cout << "\t2 - Против компьютера \"Smart\" стратегия" << endl;
	cout << "\t0 - Выход" << endl;
	cin >> menu;
	switch (menu)
	{
	case 1:
		type_game = 1;
		type_symbol(&symbol_player_1, &symbol_player_2);
		break;
	case 2:
		type_game = 2;
		type_symbol(&symbol_player_1, &symbol_player_2);
		break;
	default:
		game_over = true;
		break;
	}

	clear_field(pField, pFieldVar, field_size);

	while (game_over != true)
	{
		draw_field();
		
		move = input_events();
		
		game_logic(type_game, move, symbol_player_1, symbol_player_2);
	}

	delete[] pField;
	delete[] pFieldVar;
	return 0;
}