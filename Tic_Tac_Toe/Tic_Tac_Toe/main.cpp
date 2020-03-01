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

void type_symbol(char* player_1, char* player_2)
{
	int type_symbol = rand() % 2 + 1;

	if (type_symbol == 2)
	{
		cout << "\tСлучайным образом определено что Вы играете за нолики O." << endl;
		cout << "\tПротивник играет за крестики Х (крестики ходят первыми)." << endl;
		*player_1 = 'O'; *player_2 = 'X';
		cout << endl;
		system("pause");
	}
	else
	{
		cout << "\tСлучайным образом определено что Вы играете за крестики Х" << endl;
		cout << "\t(крестики ходят первыми). Противник играет за нолики O." << endl;
		*player_1 = 'X'; *player_2 = 'O';
		cout << endl;
		system("pause");
	}
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
	srand(unsigned(time(0)));

	int menu, type_game, move;
	char symbol_player_1, symbol_player_2;

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
		type_symbol(&symbol_player_1, &symbol_player_2);
		break;
	case 2:
		type_game = 2;
		type_symbol(&symbol_player_1, &symbol_player_2);
		break;
	case 3:
		type_game = 3;
		type_symbol(&symbol_player_1, &symbol_player_2);
		break;
	default:
		game_over = true;
		break;
	}

	gen_field(pField, pFieldVar, field_size);

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