//Copyright © 2017 Teal Dulcet and Brent McManus

#include "tokaido.h"

using namespace std;

player::player(void)
{
	locationy = 0;
	locationx = 0;
	totalreward = 0;
	achievements = 0;
	coins = 6;
	field = 0;
	mountain = 0;
	seaside = 0;
	for (int i = 0; i < 4; ++i)
		souvenirs[i] = 0;
	meal = 0;
	for (int i = 0; i < 14; ++i)
		specialties[i] = 0;
	shrine = 0;
	springs = 0;
	encounter = 0;
}

player::~player(void)
{
}

meal::meal(void)
{
	cost = 0;
	specialty = 0;
	available = true;
}

meal::~meal(void)
{
}

//Sensor input
void sensor(short numplayers, player *players, short player, short nextinn, short sensors[])
{
	int j = 0;

	for (int i = players[player].locationy + 1; i <= nextinn; ++i)
	{
		if (invalidaction(numplayers, players, player, j))
			sensors[j] = 0;
		else
			sensors[j] = board[i];

		++j;
	}
}

//State-index
int state(short numplayers, player *players, short player, short nextinn)
{
	int index = 0;
	int num = 1;

	short sensors[nextinn - players[player].locationy];

	sensor(numplayers, players, player, nextinn, sensors);

	for (int i = 0; i < (nextinn - players[player].locationy); ++i)
	{
		index += (num * sensors[i]);

		++num;
	}

	return index;
}

short nextplayer(short numplayers, player *players)
{
	short player = 0;

	for (int i = 0; i < numplayers; ++i)
		if (players[i].locationy < players[player].locationy)
			player = i;

	for (int i = 0; i < numplayers; ++i)
		if (players[i].locationy == players[player].locationy and players[i].locationx > players[player].locationx)
			player = i;

	return player;
}

void achievement(short numplayers, player *players)
{
	short max = 0;

	for (int i = 0; i < numplayers; ++i)
		if (players[i].meal > max)
			max = players[i].meal;

	for (int i = 0; i < numplayers; ++i)
	{
		if (players[i].meal == max)
		{
			players[i].totalreward += 3;
			++players[i].achievements;
		}
	}

	max = 0;

	for (int i = 0; i < numplayers; ++i)
		if (players[i].springs > max)
			max = players[i].springs;

	for (int i = 0; i < numplayers; ++i)
	{
		if (players[i].springs == max)
		{
			players[i].totalreward += 3;
			++players[i].achievements;
		}
	}

	max = 0;

	for (int i = 0; i < numplayers; ++i)
		if (players[i].encounter > max)
			max = players[i].encounter;

	for (int i = 0; i < numplayers; ++i)
	{
		if (players[i].encounter == max)
		{
			players[i].totalreward += 3;
			++players[i].achievements;
		}
	}

	max = 0;

	for (int i = 0; i < numplayers; ++i)
		if ((players[i].souvenirs[objects] + players[i].souvenirs[clothing] + players[i].souvenirs[art] + players[i].souvenirs[fooddrinks]) > max)
			max = players[i].souvenirs[objects] + players[i].souvenirs[clothing] + players[i].souvenirs[art] + players[i].souvenirs[fooddrinks];

	for (int i = 0; i < numplayers; ++i)
	{
		if ((players[i].souvenirs[objects] + players[i].souvenirs[clothing] + players[i].souvenirs[art] + players[i].souvenirs[fooddrinks]) == max)
		{
			players[i].totalreward += 3;
			++players[i].achievements;
		}
	}

	max = SHRT_MAX;

	for (int i = 0; i < numplayers; ++i)
	{
		short amax = 0;

		for (int j = 0; j < numplayers; ++j)
			if (players[j].shrine > amax and players[j].shrine < max)
				amax = players[j].shrine;

		if (amax > 0)
		{
			for (int j = 0; j < numplayers; ++j)
			{
				if (players[j].shrine == amax)
				{
					if (i == 0)
					{
						players[j].totalreward += 10;
					}
					else if (i == 1)
					{
						players[j].totalreward += 7;
					}
					else if (i == 2)
					{
						players[j].totalreward += 4;
					}
					else if (i >= 3)
					{
						players[j].totalreward += 2;
					}
				}
			}
		}

		max = amax;
	}
}

short reward(short numplayers, player *players, short player, meal meals[][4])
{
	short areward = 0; //Reward

	short space = board[players[player].locationy];

	if (space == encounter)
	{
		int temp = rand() % 7;

		if (temp == 0)
		{
			space = shop;
		}
		else if (temp == 1)
		{
			space = field;
		}
		else if (temp == 2)
		{
			space = mountain;
		}
		else if (temp == 3)
		{
			space = seaside;
		}
		else if (temp == 4)
		{
			areward += 3;
		}
		else if (temp == 5)
		{
			players[player].coins += 3;
		}
		else if (temp == 6)
		{
			if (players[player].coins > 0)
			{
				areward += 1;
				--players[player].coins;
			}
		}

		++players[player].encounter;
	}

	if (space == inn)
	{
		short innnum = 0;

		for (int i = 1; i <= players[player].locationy; ++i)
			if (board[i] == inn)
				++innnum;

		--innnum;

		short card = -1;
		short max = 0;

		for (int i = 0; i < (numplayers + 1); ++i)
		{
			if (meals[i][innnum].available and meals[i][innnum].cost > max and players[player].coins >= meals[i][innnum].cost and players[player].specialties[meals[i][innnum].specialty] < 2)
			{
				max = meals[i][innnum].cost;
				card = i;
			}
		}

		if (card >= 0)
		{
			players[player].coins -= meals[card][innnum].cost;
			++players[player].specialties[meals[card][innnum].specialty];

			meals[card][innnum].available = false;

			areward += 6;
			players[player].meal += meals[card][innnum].cost;
		}
	}
	else if (space == shop)
	{
		short type[3];
		short cost[3];

		for (int i = 0; i < 3; ++i)
		{
			type[i] = rand() % 4;
			cost[i] = rand() % 3;
			++cost[i];
		}

		for (int i = 1; i <= 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (cost[j] == i and players[player].coins >= i)
				{
					short set = 0;
					short count = 0;

					players[player].coins -= cost[j];

					if (type[j] == objects)
					{
						++players[i].souvenirs[objects];
						set = players[i].souvenirs[objects];
					}
					else if (type[j] == clothing)
					{
						++players[i].souvenirs[clothing];
						set = players[i].souvenirs[clothing];
					}
					else if (type[j] == art)
					{
						++players[i].souvenirs[art];
						set = players[i].souvenirs[art];
					}
					else if (type[j] == fooddrinks)
					{
						++players[i].souvenirs[fooddrinks];
						set = players[i].souvenirs[fooddrinks];
					}

					for (int k = 0; k < 4; ++k)
						if (players[player].souvenirs[k] >= set)
							++count;

					if (count == 1)
					{
						areward += 1;
					}
					else if (count == 2)
					{
						areward += 3;
					}
					else if (count == 3)
					{
						areward += 5;
					}
					else if (count == 4)
					{
						areward += 7;
					}
				}
			}
		}
	}
	else if (space == shrine)
	{
		if (players[player].coins >= 3)
		{
			players[player].coins -= 3;
			players[player].shrine += 3;
			areward += 3;
		}
		else if (players[player].coins >= 2)
		{
			players[player].coins -= 2;
			players[player].shrine += 2;
			areward += 2;
		}
		else if (players[player].coins >= 1)
		{
			players[player].coins -= 1;
			players[player].shrine += 1;
			areward += 1;
		}
	}
	else if (space == field)
	{
		if (players[player].field == 0)
		{
			areward += 1;
			++players[player].field;
		}
		else if (players[player].field == 1)
		{
			areward += 2;
			++players[player].field;
		}
		else if (players[player].field == 2)
		{
			areward += 3;
			++players[player].field;

			bool achievement = true;

			for (int i = 0; i < numplayers; ++i)
				if (i != player and players[i].field > 2)
					achievement = false;

			if (achievement)
			{
				areward += 3;
				++players[player].achievements;
			}
		}
	}
	else if (space == springs)
	{
		int temp = rand() % 2;

		if (temp == 0)
		{
			areward += 2;
		}
		if (temp == 1)
		{
			areward += 3;
		}

		++players[player].springs;
	}
	else if (space == mountain)
	{
		if (players[player].mountain == 0)
		{
			areward += 1;
			++players[player].mountain;
		}
		else if (players[player].mountain == 1)
		{
			areward += 2;
			++players[player].mountain;
		}
		else if (players[player].mountain == 2)
		{
			areward += 3;
			++players[player].mountain;
		}
		else if (players[player].mountain == 3)
		{
			areward += 4;
			++players[player].mountain;

			bool achievement = true;

			for (int i = 0; i < numplayers; ++i)
				if (i != player and players[i].mountain > 3)
					achievement = false;

			if (achievement)
			{
				areward += 3;
				++players[player].achievements;
			}
		}
	}
	else if (space == farm)
	{
		players[player].coins += 3;
	}
	else if (space == seaside)
	{
		if (players[player].seaside == 0)
		{
			areward += 1;
			++players[player].seaside;
		}
		else if (players[player].seaside == 1)
		{
			areward += 2;
			++players[player].seaside;
		}
		else if (players[player].seaside == 2)
		{
			areward += 3;
			++players[player].seaside;
		}
		else if (players[player].seaside == 3)
		{
			areward += 4;
			++players[player].seaside;
		}
		else if (players[player].seaside == 4)
		{
			areward += 5;
			++players[player].seaside;

			bool achievement = true;

			for (int i = 0; i < numplayers; ++i)
				if (i != player and players[i].seaside > 4)
					achievement = false;

			if (achievement)
			{
				areward += 3;
				++players[player].achievements;
			}
		}
	}

	return areward;
}

bool goal(short numplayers, player *players)
{
	for (int i = 0; i < numplayers; ++i)
		if (players[i].locationy != 53)
			return (false);

	return (true);
}

bool invalidaction(short numplayers, player *players, short player, short action)
{
	if ((players[player].locationy + action) >= 54)
		return (true);

	if (board[players[player].locationy + action] != inn)
	{
		if (numplayers > 3 and twospaces[players[player].locationy + action])
		{
			short anumplayers = 0;

			for (int i = 0; i < numplayers; ++i)
				if (i != player and (players[player].locationy + action) == players[i].locationy)
					++anumplayers;

			if (anumplayers > 1)
				return (true);
		}
		else
		{
			for (int i = 0; i < numplayers; ++i)
				if (i != player and (players[player].locationy + action) == players[i].locationy)
					return (true);
		}
	}

	if (board[players[player].locationy + action] == shop)
	{
		if (players[player].coins < 1)
			return (true);
	}
	else if (board[players[player].locationy + action] == field)
	{
		if (players[player].field >= 3)
			return (true);
	}
	else if (board[players[player].locationy + action] == mountain)
	{
		if (players[player].mountain >= 4)
			return (true);
	}
	else if (board[players[player].locationy + action] == seaside)
	{
		if (players[player].seaside >= 5)
			return (true);
	}

	return (false);
}

void outputstate(short numplayers, player *players, short player, short action, short nextinn)
{
	cout << "\n";

	for (int i = 53; i >= 0; --i)
	{
		if (board[i] == inn)
			cout << "\033[1m\033[33mThe inn\t\t";
		else if (board[i] == shop)
			cout << "\033[7mThe shop\t";
		else if (board[i] == shrine)
			cout << "\033[31mThe shrine\t";
		else if (board[i] == encounter)
			cout << "\033[35mAn encounter\t";
		else if (board[i] == field)
			cout << "\033[32mA rice field\t";
		else if (board[i] == springs)
			cout << "\033[36mThe hot springs\t";
		else if (board[i] == mountain)
			cout << "\033[37mThe mountain\t";
		else if (board[i] == farm)
			cout << "\033[33mA farm\t\t";
		else if (board[i] == seaside)
			cout << "\033[34mThe seaside\t";

		if (board[i] == inn)
			cout << "\u2022 \u2022 \u2022 \u2022";
		else if (numplayers > 3 and twospaces[i])
			cout << "—— \u2022 ——";
		else
			cout << "———————";

		cout << " \u2022\033[0m";

		for (int j = 0; j < numplayers; ++j)
		{
			for (int k = 0; k < numplayers; ++k)
			{
				if (players[k].locationy == i and players[k].locationx == j)
				{
					cout << " — ";
					wcout << "\U0001F464";
					cout << " Player " << k + 1;
				}
			}
		}

		if (i > 0)
			cout << "\n\t\t\t|\n";
		else
			cout << "\n";
	}

	cout << "\n";

	if (action > 0)
	{
		short sensors[nextinn - players[player].locationy];

		sensor(numplayers, players, player, nextinn, sensors);

		cout << "Player " << player + 1 << ":\n";

		cout << "\tSensors:\n";

		for (int i = 0; i < (nextinn - players[player].locationy); ++i)
		{
			cout << "\t\t" << i + 1 << ":\t\t" << sensors[i] << " (";

			if (sensors[i] == 0)
				cout << "Invalid";
			else
			{
				if (sensors[i] == inn)
					cout << "\033[1m\033[33mThe inn";
				else if (sensors[i] == shop)
					cout << "\033[7mThe shop";
				else if (sensors[i] == shrine)
					cout << "\033[31mThe shrine";
				else if (sensors[i] == encounter)
					cout << "\033[35mAn encounter";
				else if (sensors[i] == field)
					cout << "\033[32mA rice field";
				else if (sensors[i] == springs)
					cout << "\033[36mThe hot springs";
				else if (sensors[i] == mountain)
					cout << "\033[37mThe mountain";
				else if (sensors[i] == farm)
					cout << "\033[33mA farm";
				else if (sensors[i] == seaside)
					cout << "\033[34mThe seaside";

				cout << "\033[0m";
			}

			cout << ")\n";
		}

		cout << "\n\t\t\033[32mA rice field\033[0m:\t" << players[player].field << "\n";
		cout << "\t\t\033[37mThe mountain\033[0m:\t" << players[player].mountain << "\n";
		cout << "\t\t\033[34mThe seaside\033[0m:\t" << players[player].seaside << "\n";
		cout << "\t\t";
		wcout << "\U0001F464";
		cout << " Coins:\t\t" << players[player].coins << "\n";

		cout << "\n\tAction: moves " << action << " space";

		if (action > 1)
			cout << "s";

		cout << " \u2191";
	}

	cout << endl;
}

void outputreward(short reward, int totalreward)
{
	cout << "\n\tReward: " << reward << "\tTotal Reward: " << totalreward << endl;
}

bool infile(const char file[], short &numplayers, long double qmatrix[1050][4][5][6][50][14])
{
	ifstream fin;

	fin.open(file);
	if (fin)
	{
		fin >> numplayers;
		fin.ignore(100, '\n');

		for (int i = 0; i < 1050 and !fin.eof(); ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < 5; ++k)
				{
					for (int l = 0; l < 6; ++l)
					{
						for (int ii = 0; ii < 50; ++ii)
						{
							for (int ij = 0; ij < 14; ++ij)
							{
								fin >> qmatrix[i][j][k][l][ii][ij];

								if (j == 3 and k == 4 and l == 5 and ii == 49 and ij == 13)
									fin.ignore(100, '\n');
								else
									fin.ignore(100, ',');
							}
						}
					}
				}
			}
		}

		fin.close();
		fin.clear();
	}
	else
		return false;

	return true;
}

void outfile(const char file[], short numplayers, long double qmatrix[1050][4][5][6][50][14])
{
	ofstream fout;

	fout.open(file);

	fout << numplayers << "\n";

	for (int i = 0; i < 1050; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 5; ++k)
			{
				for (int l = 0; l < 6; ++l)
				{
					for (int ii = 0; ii < 50; ++ii)
					{
						for (int ij = 0; ij < 14; ++ij)
						{
							fout << qmatrix[i][j][k][l][ii][ij];

							if (j == 3 and k == 4 and l == 5 and ii == 49 and ij == 13)
								fout << "\n";
							else
								fout << ",";
						}
					}
				}
			}
		}
	}

	fout.close();
	fout.clear();
}

int main()
{
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int ii = 0;
	int ij = 0;

	char again; //Would you like to start again?

	const int N = 1000000; //Episodes

	srand(time(0));

	const float n = 0.1;
	const float y = 0.9;

	static long double qmatrix[1050][4][5][6][50][14]; //Q-matrix

	for (i = 0; i < 1050; ++i)
		for (j = 0; j < 4; ++j)
			for (k = 0; k < 5; ++k)
				for (l = 0; l < 6; ++l)
					for (ii = 0; ii < 50; ++ii)
						for (ij = 0; ij < 14; ++ij)
							qmatrix[i][j][k][l][ii][ij] = 0;

	float e = 1;

	const short mealspecialties[25] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 11, 12, 13, 14};
	const short mealcost[14] = {1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3};

	do
	{
		short numplayers = 0;

		cout << "\nThe Tokaido board game.\n";

		if (infile("qmatrix.csv", numplayers, qmatrix))
		{
			cout << "\nNumber of players: " << numplayers << "\n";
		}
		else
		{
			do
			{
				cout << "\nPlease enter the number of players: ";

				cin >> numplayers;
				cin.ignore(100, '\n');

				if (numplayers < 1 or numplayers > 5)
					cerr << "Invalid Input." << endl;
			} while (numplayers < 1 or numplayers > 5);

			float **trainingtotalreward = new float*[numplayers]; //Total reward gained per training episode

			for (i = 0; i < numplayers; ++i)
				trainingtotalreward[i] = new float[N];

			//Training

			for (i = 0; i < N; ++i)
			{
				struct player players[numplayers];

				for (j = 0; j < numplayers; ++j)
					players[j].locationx = numplayers - j - 1;

				struct meal meals[numplayers + 1][4];

				for (j = 0; j < 4; ++j)
				{
					for (k = 0; k < (numplayers + 1); ++k)
					{
						meals[k][j].specialty = mealspecialties[rand() % 25];
						meals[k][j].cost = mealcost[meals[k][j].specialty];
					}
				}

				struct player oldplayers[numplayers];

				short oldaction[numplayers];
				short oldreward[numplayers];

				int oldstate[numplayers];

				for (j = 0; j < numplayers; ++j)
				{
					oldaction[j] = 1;
					oldreward[j] = 0;

					oldstate[j] = -1;
				}

				//outputstate(numplayers, players, 0, 0, 0);

				while (goal(numplayers, players) == false)
				{
					short player = nextplayer(numplayers, players);

					short action = 1;	//Action
					short areward = 0; //Reward

					short nextinn = players[player].locationy + 1;

					while (nextinn < 53 and board[nextinn] != inn)
						++nextinn;

					int currentstate = state(numplayers, players, player, nextinn); //Current state

					if (oldstate[player] > 0)
					{
						qmatrix[oldstate[player]][oldplayers[player].field][oldplayers[player].mountain][oldplayers[player].seaside][oldplayers[player].coins][oldaction[player] - 1] += n * (oldreward[player] + (y * qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][action - 1]) - qmatrix[oldstate[player]][oldplayers[player].field][oldplayers[player].mountain][oldplayers[player].seaside][oldplayers[player].coins][oldaction[player] - 1]);
					}

					oldstate[player] = currentstate;
					oldaction[player] = action;
					oldreward[player] = areward;
					oldplayers[player].field = players[player].field;
					oldplayers[player].mountain = players[player].mountain;
					oldplayers[player].seaside = players[player].seaside;
					oldplayers[player].coins = players[player].coins;

					if ((i % 10000) == 0 and e > 0.1)
						e -= 0.01;

					float temp = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

					if (temp >= e)
					{
						long double max = -LDBL_MAX;

						for (j = 0; j < 14; ++j)
						{
							if (qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j] > max and invalidaction(numplayers, players, player, j + 1) == false)
							{
								max = qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j];
								action = j + 1;
							}
						}
					}
					else
					{
						do
						{
							action = rand() % (nextinn - players[player].locationy);
							++action;
						} while (invalidaction(numplayers, players, player, action));
					}

					players[player].locationy += action;
					players[player].locationx = 0;

					for (j = 0; j < numplayers; ++j)
						if (j != player and players[j].locationy == players[player].locationy and players[j].locationx >= players[player].locationx)
							players[player].locationx = players[j].locationx + 1;

					//outputstate(numplayers, players, player, action, nextinn);

					areward = reward(numplayers, players, player, meals);

					//outputreward(areward, players[player].totalreward);

					players[player].totalreward += areward;

					//sleep(2);
				}

				int areward[numplayers];

				for (j = 0; j < numplayers; ++j)
					areward[j] = players[j].totalreward;

				achievement(numplayers, players);

				for (j = 0; j < numplayers; ++j)
					qmatrix[oldstate[j]][oldplayers[j].field][oldplayers[j].mountain][oldplayers[j].seaside][oldplayers[j].coins][oldaction[j] - 1] += n * (oldreward[j] + (players[j].totalreward - areward[j]) + (y * 0) - qmatrix[oldstate[j]][oldplayers[j].field][oldplayers[j].mountain][oldplayers[j].seaside][oldplayers[j].coins][oldaction[j] - 1]);

				for (j = 0; j < numplayers; ++j)
					trainingtotalreward[j][i] = players[j].totalreward;
			}

			outfile("qmatrix.csv", numplayers, qmatrix);

			ofstream fout;

			fout.open("training.csv");

			fout << "Training:\n";

			for (i = 0; i < numplayers; ++i)
			{
				fout << "Player " << i + 1;

				if (i == (numplayers - 1))
					fout << "\n";
				else
					fout << ",";
			}

			for (j = 0; j < N; ++j)
			{
				for (i = 0; i < numplayers; ++i)
				{
					fout << trainingtotalreward[i][j];

					if (i == (numplayers - 1))
						fout << "\n";
					else
						fout << ",";
				}
			}

			fout.close();
			fout.clear();

			delete [] trainingtotalreward;
		}

		//Test

		/*float **testtotalreward = new float*[numplayers]; //Total reward gained per test episode

		for (i = 0; i < numplayers; ++i)
			testtotalreward[i] = new float[N];
		
		for (i = 0; i < numplayers; ++i)
			for (j = 0; j < N; ++j)
				testtotalreward[i][j] = 0;*/

		//for(i = 0; i < N; ++i)
		{
			struct player players[numplayers];

			for (j = 0; j < numplayers; ++j)
				players[j].locationx = numplayers - j - 1;

			struct meal meals[numplayers + 1][4];

			for (j = 0; j < 4; ++j)
			{
				for (k = 0; k < (numplayers + 1); ++k)
				{
					meals[k][j].specialty = mealspecialties[rand() % 25];
					meals[k][j].cost = mealcost[meals[k][j].specialty];
				}
			}

			outputstate(numplayers, players, 0, 0, 0);

			while (goal(numplayers, players) == false)
			{
				short player = nextplayer(numplayers, players);

				short action = 1;	//Action
				short areward = 0; //Reward

				short nextinn = players[player].locationy + 1;

				while (nextinn < 53 and board[nextinn] != inn)
					++nextinn;

				int currentstate = state(numplayers, players, player, nextinn); //Current state

				bool invalid = false;

				if (player == 0)
				{
					long double max = -LDBL_MAX;

					for (j = 0; j < 14; ++j)
					{
						if (qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j] > max and invalidaction(numplayers, players, player, j + 1) == false)
						{
							max = qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j];
							action = j + 1;
						}
					}
				}
				else
				{
					do
					{
						cout << "\nPlayer " << player + 1 << ", please enter the number of spaces to move \u2191: ";

						cin >> action;
						cin.ignore(100, '\n');

						invalid = invalidaction(numplayers, players, player, action);

						if (action < 1 or (players[player].locationy + action) > nextinn or (players[player].locationy + action) > 53)
							invalid = true;

						if (invalid)
							cerr << "Invalid Input." << endl;
					} while (invalid);
				}

				players[player].locationy += action;
				players[player].locationx = 0;

				for (j = 0; j < numplayers; ++j)
					if (j != player and players[j].locationy == players[player].locationy and players[j].locationx >= players[player].locationx)
						players[player].locationx = players[j].locationx + 1;

				outputstate(numplayers, players, player, action, nextinn);

				areward = reward(numplayers, players, player, meals);

				outputreward(areward, players[player].totalreward);

				players[player].totalreward += areward;

				if (player == 0)
					sleep(2);
			}

			achievement(numplayers, players);

			/*for (j = 0; j < numplayers; ++j)
				testtotalreward[j][i] = players[j].totalreward;*/

			int maxreward = 0;
			short winner = 0;

			for (j = 0; j < numplayers; ++j)
			{
				if (players[j].totalreward > maxreward)
				{
					maxreward = players[j].totalreward;
					winner = j;
				}
			}

			short count = 0;

			for (j = 0; j < numplayers; ++j)
				if (players[j].totalreward == maxreward)
					++count;

			if (count > 0)
			{
				short maxachievements = 0;

				for (j = 0; j < numplayers; ++j)
				{
					if (players[j].totalreward == maxreward and players[j].achievements > maxachievements)
					{
						maxachievements = players[j].achievements;
						winner = j;
					}
				}
			}

			cout << "\nTotal Reward:\n";

			for (j = 0; j < numplayers; ++j)
				cout << "\tPlayer " << j + 1 << ": " << players[j].totalreward << "\n";

			cout << "\nAchievements:\n";

			for (j = 0; j < numplayers; ++j)
				cout << "\tPlayer " << j + 1 << ": " << players[j].achievements << "\n";

			if (winner == 0)
				cout << "\n\033[1mPlayer " << winner + 1 << " (Computer) won!\033[0m";
			else
				cout << "\nPlayer " << winner + 1 << " won.";

			cout << endl;
		}

		/*double testaverage[numplayers]; //Test-Average
		double teststandarddeviation[numplayers]; //Test-Standard-Deviation

		for (i = 0; i < numplayers; ++i)
		{
			testaverage[i] = 0;
			teststandarddeviation[i] = 0;
		}
		
		for (i = 0; i < numplayers; ++i)
			for (j = 0; j < N; ++j)
				testaverage[i] += testtotalreward[i][j];
		
		for (i = 0; i < numplayers; ++i)
			testaverage[i] /= N;
		
		for (i = 0; i < numplayers; ++i)
			for (j = 0; j < N; ++j)
				teststandarddeviation[i] += pow(testtotalreward[i][j] - testaverage[i], 2);
		
		for (i = 0; i < numplayers; ++i)
			teststandarddeviation[i] = sqrt(teststandarddeviation[i] / N);

		ofstream fout;
		fout.open("test.csv");
		
		fout << "Test:\n";
		
		for (i = 0; i < numplayers; ++i)
		{
			fout << "Player " << i + 1;

			if (i == (numplayers - 1))
				fout << "\n";
			else
				fout << ",";
		}

		for (j = 0; j < N; ++j)
		{
			for (i = 0; i < numplayers; ++i)
			{
				fout << testtotalreward[i][j];

				if (i == (numplayers - 1))
					fout << "\n";
				else
					fout << ",";
			}
		}
		
		for (i = 0; i < numplayers; ++i)
		{
			fout << "Player " << i + 1 << "\n";
			
			fout << "Test Average: " << testaverage[i] << "\n";
			
			fout << "Test Standard Deviation: " << teststandarddeviation[i] << "\n";
		}

		fout.close();
		fout.clear();

		delete [] testtotalreward;*/

		cout << "Would you like to start again? (y/n) ";

		cin >> again;
		cin.ignore(100, '\n');

		again = tolower(again);

	} while (again == 'y');

	return 0;
}
