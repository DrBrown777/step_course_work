#include <iostream>

using namespace std;

bool game_over;

char field[9] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
char field_var[9] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };


void name()
{
	cout << "\t" << "<-xxx Крестики-нолики ooo->" << "\n\n";
}

void setup()
{
	game_over = false;

}

void draw_field()
{
	system("cls");
	name();
	
	for (int i = 0; i < 9; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << "-" << field[i] << "-" << endl;
		else
			cout << "\t" << "-" << field[i] << "-  |";
	}
	cout << "\n\n\n";
}

int input_events()
{
	int move;
	cout << "Вариант хода:" << endl;
	for (int i = 0; i < 9; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << "-" << field_var[i] << "-" << endl;
		else
			cout << "\t" << "-" << field_var[i] << "-  |";
	}
	cout << endl;
	cout << "Сделайте ход (1-9) -> ";
	cin >> move;
	field[move - 1] = 'X';
	field_var[move - 1] = '-';
	return move;
}

void game_logic(int type_game, int move)
{

}

int main()
{
	setlocale(0, "");

	int menu, type_game, move;

	setup();

	name();

	cout << "\tВыберите вариант игры:" << endl;
	cout << "\t1 - Против компьютера \"Random\" стратегия" << endl;
	cout << "\t2 - Против компьютера \"Self Learnig\" стратегия" << endl;
	cout << "\t3 - Играть в двоем" << endl;
	cout << "\t0 - Выход" << endl;
	cin >> menu;
	switch (menu)
	{
	case 1:
		type_game = 1;
		break;
	case 2:
		type_game = 2;
		break;
	case 3:
		type_game = 3;
		break;
	default:
		game_over = true;
		break;
	}

	while (game_over != true)
	{

		draw_field();
		
		move = input_events();
		
		game_logic(type_game, move);
	}

	return 0;
}