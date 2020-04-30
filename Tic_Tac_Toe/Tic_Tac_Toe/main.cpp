/*v.1.18 by dr_brown*/
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <set>

using namespace sf;

const int FIELD_SIZE = 9; /*размер игрового поля*/
const int INIT_WEIGHT = 100; /*Начальный вес матрицы весов*/
const int PRECISION_COEF = 50; /*Точность генератора хода для Smart игрока*/
const double STEP_COEF = 0.65; /*Коэфициент обучения*/
const int STEP_LEARN = 20; /*Шаг обучения*/
const int NUMBER_OF_GAMES = 100000; /*Кол-во игр которые должен сыграть Smart игрок для обучения*/

char Field[FIELD_SIZE]; /*массив под игровое поле*/

int size_database_X = 0;/*Размер базы данных X*/
int size_database_O = 0;/*Размер базы данных O*/

struct DataBase /*База знаний smart игрока*/
{
	char MyField[9]; //Ситуация на поле
	int MyWeight[9]; //Матрица весов
	DataBase() {
		std::fill(MyWeight, MyWeight + FIELD_SIZE, INIT_WEIGHT);
	}
};

DataBase* Collections_X = new DataBase[size_database_X];/*База для X комбинаций*/
DataBase* Collections_O = new DataBase[size_database_O];/*База для O комбинаций*/

struct Stack /*История выполненых ходов Smart игрока в текущеей партии для функции Learn*/
{
	int current_move;
	int index_weight;
};

struct Wins /*Содержит победный результат*/
{
	int mas[3] = { 0 };
	char xod;
};

void start_game(); /*Отображает стартовое меню игры и определяет последовательность вызова функций в зависимоти от типа игры*/
void setup(int*, bool*); /*Функция инициализации флаговых переменных*/
void type_symbol(bool*, char*, char*, int); /*Функция рандомно определяет кто будет играть за X а кто за O*/
void clear_field(); /*Функция очищает игровые поля*/
int random_player(); /*Функция возвращает возможный ход, случайно, в стратегии Random*/
int input_events(bool*, int, Stack**, int*, char, char); /*Функция возвращает ход сделанный пользователем с клавиатуры, и выводит оставшиеся варианты хода*/
void check_wins(int*, int, int*, int*, int*, bool*, Wins*); /*Функция проверяет на победу после каждого хода*/
void game_logic(int, int*, bool*, int, char, char, int*, int*, int*, bool*, Stack**, int*, Wins*); /*Функция логики игры*/

DataBase* push_database(DataBase*, int*); /*Добавляет в базу данных неизвестную ситуацию на поле*/
Stack* push_stack(int, int, Stack*, int*); /*Добавляет в Stack текущий ход игрока Smart*/
int get_situation(DataBase*, int); /*Ищет в базе сложившнюся ситуацию на поле*/
int get_smart_random(int, DataBase*); /*Генерирует ход Smart игрока на основании матрицы весов*/
void smart_learn(Stack*, int, int, DataBase*); /*Рекурсивная функция обучения, уменьшает вес хода в случае проигрыша, и увеличивет наооборот, ничья нейтрально*/

void display_statistic(RenderWindow&, int, int, int); /*Выводит статистику*/
void menu_graph(RenderWindow&, int*, Event, bool game_over = false); /*Выводит меню игры*/
void display_field(RenderWindow&, char, char, bool game_over = false); /*Выводит игровое полу*/
int move_human(RenderWindow&); /*Возвращает ход человека*/
void wins_victory(RenderWindow&, Wins*); /*Выводит полосу при победе*/

int main()
{
	setlocale(0, "");
	srand(unsigned(time(0)));

	start_game();

	delete[] Collections_X;
	delete[] Collections_O;
	return 0;
}

void start_game()
{
	RenderWindow window(VideoMode(800, 600), L"Крестики-Нолики by dr_brown ver.1.18");
	Image icon;
	icon.loadFromFile("tic.png");
	window.setIcon(32, 32, icon.getPixelsPtr());
	Texture textureFon;
	Sprite spritefon;
	textureFon.loadFromFile("fon.png");
	spritefon.setTexture(textureFon);
	spritefon.setPosition(0, 0);

	int type_game = 0; /*тип игры, Random или Smart*/
	int x_wins = 0, o_wins = 0, d_wins = 0; /*накопительные переменные для статы*/

	int move = -1; /*Переменная содержит ход*/
	bool turn = NULL; /*Очередь хода*/
	char player_1 = NULL, player_2 = NULL; /*символьные переменные кто за что играет*/
	int draw = 0; /*Сумма ходов двух игроков*/
	bool game_over = false; /*Флаговая: конец игры*/
	int stack_size = 0; /*Размер Стека ходов*/
	Stack* Hystory = nullptr;
	Wins* Win = new Wins;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		if (type_game == 0)
		{
			window.clear();
			window.draw(spritefon);
			if (game_over == true) { display_field(window, player_1, player_2, game_over); wins_victory(window, Win); }
			menu_graph(window, &type_game, event, game_over);
			display_statistic(window, x_wins, o_wins, d_wins);
			window.display();
			if (type_game == 1 || type_game == 2)
			{
				setup(&draw, &game_over);
				type_symbol(&turn, &player_1, &player_2, type_game);
				window.clear();
				window.draw(spritefon);
			}
		}
		if (type_game == 3)
		{
			for (int i = 0; i < NUMBER_OF_GAMES; i++)
			{
				setup(&draw, &game_over);
				type_symbol(&turn, &player_1, &player_2, type_game);
				while (game_over != true)
				{
					move = input_events(&turn, type_game, &Hystory, &stack_size, player_1, player_2);
					draw++;
					game_logic(move, &draw, &turn, type_game, player_1, player_2, &x_wins, &o_wins, &d_wins, &game_over, &Hystory, &stack_size, Win);
				}
				stack_size = 0; Hystory = nullptr;
			}
			type_game = 0;
		}
		else if (type_game == 1 || type_game == 2)
		{
			window.clear();
			window.draw(spritefon);
			display_field(window, player_1, player_2);
			display_statistic(window, x_wins, o_wins, d_wins);
			window.display();
			if (turn)
			{
				move = -1;
				do
				{
					move = move_human(window);
					
				} while (move == -1 || Field[move] == 'X' || Field[move] == 'O');
			}
			else
			{
				move = input_events(&turn, type_game, &Hystory, &stack_size, player_1, player_2);
			}
			draw++;
			game_logic(move, &draw, &turn, type_game, player_1, player_2, &x_wins, &o_wins, &d_wins, &game_over, &Hystory, &stack_size, Win);
			if (game_over == true)
			{
				if (type_game == 2) stack_size = 0; Hystory = nullptr;
				type_game = 0;
			}
		}
	}
	delete[] Hystory; delete Win;
}

int input_events(bool* turn, int type_game, Stack** Hystory, int* stack_size, char player_1, char player_2)
{
	int move = 0, index;

	if ((*turn && type_game == 3) || type_game == 2)
	{
		if (player_1 == 'X' && player_2 == 'O' && type_game == 2)
		{
			if (get_situation(Collections_O, size_database_O) == -1)
				Collections_O = push_database(Collections_O, &size_database_O);

			index = get_situation(Collections_O, size_database_O);

			move = get_smart_random(index, Collections_O);
		}
		else if (player_1 == 'O' && player_2 == 'X' && type_game == 2)
		{
			if (get_situation(Collections_X, size_database_X) == -1)
				Collections_X = push_database(Collections_X, &size_database_X);

			index = get_situation(Collections_X, size_database_X);

			move = get_smart_random(index, Collections_X);
		}
		else if (player_1 == 'O' && player_2 == 'X' && type_game == 3)
		{
			if (get_situation(Collections_O, size_database_O) == -1)
				Collections_O = push_database(Collections_O, &size_database_O);

			index = get_situation(Collections_O, size_database_O);

			move = get_smart_random(index, Collections_O);
		}
		else if (player_1 == 'X' && player_2 == 'O' && type_game == 3)
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

void game_logic(int move, int* draw, bool* turn, int type_game, char player_1, char player_2,
	int* x_wins, int* o_wins, int* d_wins, bool* game_over, Stack** Hystory, int* stack_size, Wins* Win)
{
	bool wins = false;
	if ((*turn && type_game == 1) || (*turn && type_game == 2))/*Ветка Человека*/
	{
		Field[move] = player_1;
		check_wins(draw, type_game, x_wins, o_wins, d_wins, &wins, Win);
		if (wins)
		{
			if (Win->xod != 'D' && type_game == 2 && player_1 == 'X')
			{
				smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_O);
			}
			else if (Win->xod != 'D' && type_game == 2 && player_1 == 'O')
			{
				smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_X);
			}
			*game_over = true;
			return;
		}
		*turn = false;
	}
	else if ((*turn && type_game == 3) || type_game == 2) /*Ветка Smart игрока*/
	{
		if (type_game == 2)
		{
			Field[move] = player_2;
		}
		else
		{
			Field[move] = player_1;
		}
		check_wins(draw, type_game, x_wins, o_wins, d_wins, &wins, Win);
		if (wins)
		{
			if (Win->xod != 'D' && player_2 == 'X' && type_game == 2)
			{
				smart_learn(*Hystory, *stack_size - 1, STEP_LEARN, Collections_X);
			}
			else if (Win->xod != 'D' && player_2 == 'O' && type_game == 2)
			{
				smart_learn(*Hystory, *stack_size - 1, STEP_LEARN, Collections_O);
			}
			else if (Win->xod != 'D' && player_1 == 'X' && type_game == 3)
			{
				smart_learn(*Hystory, *stack_size - 1, STEP_LEARN, Collections_X);
			}
			else if (Win->xod != 'D' && player_1 == 'O' && type_game == 3)
			{
				smart_learn(*Hystory, *stack_size - 1, STEP_LEARN, Collections_O);
			}
			*game_over = true;
			return;
		}
		type_game == 2 ? *turn = true : *turn = false;
	}
	else /*Ветка Random игрока*/
	{
		Field[move] = player_2;
		check_wins(draw, type_game, x_wins, o_wins, d_wins, &wins, Win);
		if (wins)
		{
			if (Win->xod != 'D' && type_game != 1 && player_2 == 'X')
			{
				smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_O);
			}
			else if (Win->xod != 'D' && type_game != 1 && player_2 == 'O')
			{
				smart_learn(*Hystory, *stack_size - 1, -STEP_LEARN, Collections_X);
			}
			*game_over = true;
			return;
		}
		*turn = true;
	}
}

void check_wins(int* draw, int type_game, int* x_wins, int* o_wins, int* d_wins, bool* wins, Wins* Win)
{
	int victory[8][3] = { {0, 1 , 2}, {3, 4, 5}, {6, 7, 8},
	{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

	for (int i = 0; i < FIELD_SIZE - 1; i++)
	{
		if (Field[victory[i][0]] == Field[victory[i][1]] &&
			Field[victory[i][0]] == Field[victory[i][2]] &&
			Field[victory[i][0]] != ' ')
		{
			*wins = true;
			if (type_game != 3) Field[victory[i][0]] == 'X' ? (*x_wins)++ : (*o_wins)++;
			for (int j = 0, k = 0; j < 3; j++, k++)
			{
				Win->mas[j] = victory[i][k];
			}
			Field[victory[i][0]] == 'X' ? Win->xod = 'X' : Win->xod = 'O';
			return;
		}
	}
	if (*draw == 9)
	{
		*wins = true;
		if (type_game != 3) (*d_wins)++;
		for (int i = 0; i < 3; i++) { Win->mas[i] = 0; }
		Win->xod = 'D';
		return;
	}
	Win->xod = 'U';
}

void smart_learn(Stack* Hystory, int iterator, int STEP_LEARN, DataBase* Collections)
{
	if (iterator < 0)
		return;

	Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move] += STEP_LEARN;

	if ((Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move]) < 0)
	{
		Collections[Hystory[iterator].index_weight].MyWeight[Hystory[iterator].current_move] = 0;
	}

	smart_learn(Hystory, --iterator, STEP_LEARN * STEP_COEF, Collections);
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

	for (int i = 0; i < FIELD_SIZE; i++)
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

	for (int i = 0; i < FIELD_SIZE; i++)
	{
		summ += Collections[index].MyWeight[i];
		mas_temp[i] = Collections[index].MyWeight[i];
	}
	if (summ == 0) return random_player();

	for (int i = 0; i < FIELD_SIZE; i++)
	{
		mas_temp[i] = (mas_temp[i] / (double)summ) * PRECISION_COEF;
	}
	for (int i = 0; i < FIELD_SIZE; i++)
	{
		count += mas_temp[i];
	}

	int* new_mas = new int[count];

	for (int i = 0, j = 0; i < FIELD_SIZE; i++)
	{
		int tmp = mas_temp[i];
		while (tmp != 0)
		{
			new_mas[j] = i;
			j++;
			tmp--;
		}
	}

	//std::random_shuffle(new_mas, new_mas + count);

	move = new_mas[rand() % count];

	delete[] new_mas;

	return move;
}

int random_player()
{
	int move = 0, count = 0;

	for (int i = 0; i < FIELD_SIZE; i++)
	{
		if (Field[i] == ' ') count++;
	}

	int* pTmp = new int[count];

	for (int i = 0, j = 0; i < FIELD_SIZE; i++)
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

void setup(int* draw, bool* game_over)
{
	*game_over = false;
	*draw = 0;
	clear_field();
}

void type_symbol(bool* turn, char* player_1, char* player_2, int type_game)
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
}

void clear_field()
{
	for (int i = 0; i < FIELD_SIZE; i++)
	{
		Field[i] = ' ';
	}
}

void display_statistic(RenderWindow& window, int x_wins, int o_wins, int d_wins)
{
	Text text_2, text_3, text_4, text_5, text_6, text_7, text_8, text_9;
	Font font;
	font.loadFromFile("comic.ttf");

	char x[7]; char o[7]; char d[7]; char s[7];

	int summ = x_wins + o_wins + d_wins;

	for (auto it : { &text_2, &text_3, &text_4, &text_8, &text_9 })
	{
		it->setFont(font);
		it->setCharacterSize(25);
	}

	for (auto it : { &text_5, &text_6, &text_7 })
	{
		it->setFont(font);
		it->setCharacterSize(35);
		it->setColor(Color(65, 105, 255));
	}

	for (auto it : { &text_2, &text_3, &text_4 })
	{
		it->setColor(Color(65, 105, 255));
		it->setStyle(Text::Underlined);
	}

	text_3.setColor(Color(220, 20, 60));
	text_6.setColor(Color(220, 20, 60));
	text_8.setColor(Color(34, 139, 34));
	text_9.setColor(Color(34, 139, 34));

	text_2.setString(L"Победы X");
	text_3.setString(L"Победы O");
	text_4.setString(L"Ничьи");
	text_9.setString(L"Всего провели игр: ");
	text_5.setString(itoa(x_wins, x, 10));
	text_6.setString(itoa(o_wins, o, 10));
	text_7.setString(itoa(d_wins, d, 10));
	text_8.setString(itoa(summ, s, 10));

	text_2.setPosition(75, 400);
	text_3.setPosition(355, 400);
	text_4.setPosition(650, 400);
	text_5.setPosition(75, 450);
	text_6.setPosition(355, 450);
	text_7.setPosition(650, 450);
	text_8.setPosition(500, 520);
	text_9.setPosition(250, 520);

	window.draw(text_9); window.draw(text_2);
	window.draw(text_3); window.draw(text_4);
	window.draw(text_5); window.draw(text_6);
	window.draw(text_7); window.draw(text_8);
}

void menu_graph(RenderWindow& window, int* type_game, Event event, bool game_over)
{
	/*Вместо лого вывести ProgressBar при обучении Smart*/
	Texture logo;
	Sprite spritelogo;
	logo.loadFromFile("logo.png");
	spritelogo.setTexture(logo);
	spritelogo.setPosition(100, 80);

	Text menu_0, menu_1, menu_2, menu_3, menu_4;
	Font font;
	font.loadFromFile("comic.ttf");

	for (auto it : { &menu_0, &menu_1, &menu_2, &menu_3, &menu_4 })
	{
		it->setFont(font);
		it->setCharacterSize(30);
		it->setColor(Color(255, 140, 0));
	}

	menu_0.setColor(Color::Black);
	menu_4.setColor(Color(255, 20, 147));

	menu_0.setStyle(Text::Underlined);

	menu_0.setString(L"Выберите вариант игры");
	menu_1.setString(L"Пробная игра Random");
	menu_2.setString(L"Против Smart игрока");
	menu_3.setString(L"Обучить Smart игрока");
	menu_4.setString(L"Выход");

	menu_0.setPosition(430, 70);
	menu_1.setPosition(450, 130);
	menu_2.setPosition(450, 190);
	menu_3.setPosition(450, 250);
	menu_4.setPosition(560, 300);

	if (IntRect(450, 130, 320, 30).contains(Mouse::getPosition(window)))
	{
		menu_1.setColor(Color::Blue);
		if (event.type == Event::MouseButtonReleased)
			if (event.mouseButton.button == Mouse::Left) *type_game = 1;
	}

	if (IntRect(450, 190, 320, 30).contains(Mouse::getPosition(window)))
	{
		menu_2.setColor(Color::Blue);
		if (event.type == Event::MouseButtonReleased)
			if (event.mouseButton.button == Mouse::Left) *type_game = 2;
	}
	if (IntRect(450, 250, 320, 30).contains(Mouse::getPosition(window)))
	{
		menu_3.setColor(Color::Blue);
		if (event.type == Event::MouseButtonReleased)
			if (event.mouseButton.button == Mouse::Left) *type_game = 3;
	}
	if (IntRect(560, 300, 100, 30).contains(Mouse::getPosition(window)))
	{
		menu_4.setColor(Color::Black);
		if (event.type == Event::MouseButtonReleased)
			if (event.mouseButton.button == Mouse::Left) window.close();
	}

	window.draw(menu_0); window.draw(menu_1);
	window.draw(menu_2); window.draw(menu_3);
	window.draw(menu_4);
	if (!game_over)
	{
		window.draw(spritelogo);
	}
}

void display_field(RenderWindow& window, char player_1, char player_2, bool game_over)
{
	RectangleShape line_1(Vector2f(300.f, 3.f)), line_2(Vector2f(300.f, 3.f));
	RectangleShape line_3(Vector2f(300.f, 3.f)), line_4(Vector2f(300.f, 3.f));

	for (auto it : { &line_1, &line_2, &line_3, &line_4 })
	{
		it->setFillColor(Color::Black);
	}

	line_1.rotate(90); line_2.rotate(90);

	line_1.move(150, 50); line_2.move(250, 50);
	line_3.move(50, 150); line_4.move(50, 250);

	window.draw(line_1); window.draw(line_2);
	window.draw(line_3); window.draw(line_4);

	Text X, O, type_char_1, type_char_2, text_1, text_2, text_3;
	Font font;
	font.loadFromFile("comic.ttf");

	text_1.setFont(font);
	text_1.setCharacterSize(25);
	text_1.setString(L"Определено, что вы играете за ");
	text_2.setFont(font);
	text_2.setCharacterSize(25);
	text_2.setString(L"Противник играет за ");
	text_3.setFont(font);
	text_3.setCharacterSize(25);
	text_3.setString(L"Крестики ходят первыми!");
	text_3.setColor(Color(65, 105, 255));

	type_char_1.setFont(font);
	type_char_1.setCharacterSize(75);
	type_char_1.setString(player_1);
	type_char_2.setFont(font);
	type_char_2.setCharacterSize(75);
	type_char_2.setString(player_2);

	if (player_1 == 'X') {
		type_char_1.setColor(Color(65, 105, 255));
		type_char_2.setColor(Color(220, 20, 60));
		text_1.setColor(Color(65, 105, 255));
		text_2.setColor(Color(220, 20, 60));
	}
	else {
		type_char_1.setColor(Color(220, 20, 60));
		type_char_2.setColor(Color(65, 105, 255));
		text_1.setColor(Color(220, 20, 60));
		text_2.setColor(Color(65, 105, 255));
	}
	text_1.setPosition(400, 75);
	type_char_1.setPosition(550, 105);
	text_2.setPosition(450, 190);
	type_char_2.setPosition(550, 220);
	text_3.setPosition(425, 310);

	if (!game_over)
	{
		window.draw(type_char_1);
		window.draw(type_char_2);
		window.draw(text_1);
		window.draw(text_2);
		window.draw(text_3);
	}

	for (auto it : { &X, &O })
	{
		it->setFont(font);
		it->setCharacterSize(100);
	}

	X.setColor(Color(65, 105, 255));
	O.setColor(Color(220, 20, 60));

	X.setString("X"); O.setString("O");

	for (int i = 0; i < FIELD_SIZE; i++)
	{
		if (Field[i] == 'X')
		{
			if (i == 0) { X.setPosition(60, 35); window.draw(X); }
			if (i == 1) { X.setPosition(160, 35); window.draw(X); }
			if (i == 2) { X.setPosition(260, 35); window.draw(X); }

			if (i == 3) { X.setPosition(60, 135); window.draw(X); }
			if (i == 4) { X.setPosition(160, 135); window.draw(X); }
			if (i == 5) { X.setPosition(260, 135); window.draw(X); }

			if (i == 6) { X.setPosition(60, 235); window.draw(X); }
			if (i == 7) { X.setPosition(160, 235); window.draw(X); }
			if (i == 8) { X.setPosition(260, 235); window.draw(X); }
		}
		else if (Field[i] == 'O')
		{
			if (i == 0) { O.setPosition(60, 35); window.draw(O); }
			if (i == 1) { O.setPosition(160, 35); window.draw(O); }
			if (i == 2) { O.setPosition(260, 35); window.draw(O); }

			if (i == 3) { O.setPosition(60, 135); window.draw(O); }
			if (i == 4) { O.setPosition(160, 135); window.draw(O); }
			if (i == 5) { O.setPosition(260, 135); window.draw(O); }

			if (i == 6) { O.setPosition(60, 235); window.draw(O); }
			if (i == 7) { O.setPosition(160, 235); window.draw(O); }
			if (i == 8) { O.setPosition(260, 235); window.draw(O); }
		}
	}
}

int move_human(RenderWindow& window)
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed)
			window.close();
	}
	if (IntRect(60, 60, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 0;
	if (IntRect(160, 60, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 1;
	if (IntRect(260, 60, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 2;
	if (IntRect(60, 160, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 3;
	if (IntRect(160, 160, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 4;
	if (IntRect(260, 160, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 5;
	if (IntRect(60, 260, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 6;
	if (IntRect(160, 260, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 7;
	if (IntRect(260, 260, 80, 80).contains(Mouse::getPosition(window)))
		if (Mouse::isButtonPressed(Mouse::Left)) return 8;

	return -1;
}

void wins_victory(RenderWindow& window, Wins* Win)
{
	RectangleShape line(Vector2f(300.f, 5.f));
	RectangleShape line_2(Vector2f(425.f, 5.f));

	line.setFillColor(Color(0, 153, 102));
	line_2.setFillColor(Color(0, 153, 102));

	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{0, 1, 2})
	{
		line.setPosition(50, 100);
		window.draw(line);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{3, 4, 5})
	{
		line.setPosition(50, 200);
		window.draw(line);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{6, 7, 8})
	{
		line.setPosition(50, 300);
		window.draw(line);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{0, 3, 6})
	{
		line.rotate(90); line.setPosition(100, 50);
		window.draw(line);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{1, 4, 7})
	{
		line.rotate(90); line.setPosition(200, 50);
		window.draw(line);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{2, 5, 8})
	{
		line.rotate(90); line.setPosition(300, 50);
		window.draw(line);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{0, 4, 8})
	{
		line_2.rotate(45); line_2.setPosition(50, 50);
		window.draw(line_2);
	}
	if (std::set<int>(Win->mas, Win->mas + 3) == std::set<int>{2, 4, 6})
	{
		line_2.rotate(135); line_2.setPosition(350, 50);
		window.draw(line_2);
	}
}