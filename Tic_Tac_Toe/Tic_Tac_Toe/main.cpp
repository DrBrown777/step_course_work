#include <iostream>
#include <iomanip>

using namespace std;

bool game_over;
int field_size = 9;
char* pField = new char[field_size];
char* pFieldVar = new char[field_size];


void name()
{
	cout << "\t" << "<-xxx Крестики-нолики ooo->" << "\n\n";
}

void setup()
{
	game_over = false;

}

void gen_field(char* pField, char* pFieldVar, int field_size)
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
	name();
	
	for (int i = 0; i < 9; i++)
	{
		if (i == 2 || i == 5 || i == 8)
			cout << "\t" << setw(2) << "-" << pField[i] << "-" << endl;
		else
			cout << "\t" << setw(2) << "-" << pField[i] << "-  |";
	}
	cout << "\n\n\n";
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
	do 
	{
		cout << "Сделайте ход (1-9) -> ";
		cin >> move;
	} while (move < 1 && move > 9);

	return move;
}

void game_logic(int type_game, int move, char symbol_player, char symbol_pc)
{
	pField[move - 1] = symbol_player;
	pFieldVar[move - 1] = '-';
}

int main()
{
	setlocale(LC_ALL, "Russian");

	int menu, type_player, move;
	char symbol_player, symbol_pc;

	setup(); name();
	
	cout << "\tВыберите вариант игры:" << endl;
	cout << "\t1 - Против компьютера \"Random\" стратегия" << endl;
	cout << "\t2 - Против компьютера \"Self Learnig\" стратегия" << endl;
	cout << "\t3 - Играть в двоем" << endl;
	cout << "\t0 - Выход" << endl;
	cin >> menu;
	switch (menu)
	{
	case 1:
		type_player = 1;
		break;
	case 2:
		type_player = 2;
		break;
	case 3:
		type_player = 3;
		break;
	default:
		game_over = true;
		break;
	}

	cout << "\tВыберите чем играть (X или O): ";
	cin >> symbol_player;
	
	if (symbol_player == 'x')
	{
		symbol_player = 'X';
		symbol_pc = 'O';
	}
	else if (symbol_player == 'o' || symbol_player == '0')
	{
		symbol_player = 'O';
		symbol_pc = 'X';
	}

	gen_field(pField, pFieldVar, field_size);

	while (game_over != true)
	{
		draw_field();
		
		move = input_events();
		
		game_logic(type_player, move, symbol_player, symbol_pc);
	}

	delete[] pField;
	delete[] pFieldVar;
	return 0;
}