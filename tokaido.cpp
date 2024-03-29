// Copyright © 2017 Teal Dulcet and Brent McManus

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

		int card = -1;
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
			cout << "\e[1m\e[33mThe inn\t\t";
		else if (board[i] == shop)
			cout << "\e[7mThe shop\t";
		else if (board[i] == shrine)
			cout << "\e[31mThe shrine\t";
		else if (board[i] == encounter)
			cout << "\e[35mAn encounter\t";
		else if (board[i] == field)
			cout << "\e[32mA rice field\t";
		else if (board[i] == springs)
			cout << "\e[36mThe hot springs\t";
		else if (board[i] == mountain)
			cout << "\e[37mThe mountain\t";
		else if (board[i] == farm)
			cout << "\e[33mA farm\t\t";
		else if (board[i] == seaside)
			cout << "\e[34mThe seaside\t";

		if (board[i] == inn)
			cout << "\u2022 \u2022 \u2022 \u2022";
		else if (numplayers > 3 and twospaces[i])
			cout << "—— \u2022 ——";
		else
			cout << "———————";

		cout << " \u2022\e[0m";

		for (int j = 0; j < numplayers; ++j)
		{
			for (int k = 0; k < numplayers; ++k)
			{
				if (players[k].locationy == i and players[k].locationx == j)
				{
					cout << " — ";
					cout << "\U0001F464";
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
					cout << "\e[1m\e[33mThe inn";
				else if (sensors[i] == shop)
					cout << "\e[7mThe shop";
				else if (sensors[i] == shrine)
					cout << "\e[31mThe shrine";
				else if (sensors[i] == encounter)
					cout << "\e[35mAn encounter";
				else if (sensors[i] == field)
					cout << "\e[32mA rice field";
				else if (sensors[i] == springs)
					cout << "\e[36mThe hot springs";
				else if (sensors[i] == mountain)
					cout << "\e[37mThe mountain";
				else if (sensors[i] == farm)
					cout << "\e[33mA farm";
				else if (sensors[i] == seaside)
					cout << "\e[34mThe seaside";

				cout << "\e[0m";
			}

			cout << ")\n";
		}

		cout << "\n\t\t\e[32mA rice field\e[0m:\t" << players[player].field << "\n";
		cout << "\t\t\e[37mThe mountain\e[0m:\t" << players[player].mountain << "\n";
		cout << "\t\t\e[34mThe seaside\e[0m:\t" << players[player].seaside << "\n";
		cout << "\t\t";
		cout << "\U0001F4B0";
		cout << " Coins:\t" << players[player].coins << "\n";

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
