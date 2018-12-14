// Copyright © 2017 Teal Dulcet and Brent McManus

#include <fstream>
#include "tokaido.h"

using namespace std;

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
	char again; //Would you like to start again?

	const int N = 1000000; //Episodes

	srand(time(0));

	const float n = 0.1;
	const float y = 0.9;

	static long double qmatrix[1050][4][5][6][50][14]; //Q-matrix

	for (int i = 0; i < 1050; ++i)
		for (int j = 0; j < 4; ++j)
			for (int k = 0; k < 5; ++k)
				for (int l = 0; l < 6; ++l)
					for (int ii = 0; ii < 50; ++ii)
						for (int ij = 0; ij < 14; ++ij)
							qmatrix[i][j][k][l][ii][ij] = 0;

	float e = 1;

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

			float **trainingtotalreward = new float *[numplayers]; //Total reward gained per training episode

			for (int i = 0; i < numplayers; ++i)
				trainingtotalreward[i] = new float[N];

			//Training

			for (int i = 0; i < N; ++i)
			{
				struct player players[numplayers];

				for (int j = 0; j < numplayers; ++j)
					players[j].locationx = numplayers - j - 1;

				struct meal meals[numplayers + 1][4];

				for (int j = 0; j < 4; ++j)
				{
					for (int k = 0; k < (numplayers + 1); ++k)
					{
						meals[k][j].specialty = mealspecialties[rand() % 25] - 1;
						meals[k][j].cost = mealcost[meals[k][j].specialty];
					}
				}

				struct player oldplayers[numplayers];

				short oldaction[numplayers];
				short oldreward[numplayers];

				int oldstate[numplayers];

				for (int j = 0; j < numplayers; ++j)
				{
					oldaction[j] = 1;
					oldreward[j] = 0;

					oldstate[j] = -1;
				}

				//outputstate(numplayers, players, 0, 0, 0);

				while (goal(numplayers, players) == false)
				{
					short player = nextplayer(numplayers, players);

					short action = 1;  //Action
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

						for (int j = 0; j < 14; ++j)
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

					for (int j = 0; j < numplayers; ++j)
						if (j != player and players[j].locationy == players[player].locationy and players[j].locationx >= players[player].locationx)
							players[player].locationx = players[j].locationx + 1;

					//outputstate(numplayers, players, player, action, nextinn);

					areward = reward(numplayers, players, player, meals);

					//outputreward(areward, players[player].totalreward);

					players[player].totalreward += areward;

					//sleep(2);
				}

				int areward[numplayers];

				for (int j = 0; j < numplayers; ++j)
					areward[j] = players[j].totalreward;

				achievement(numplayers, players);

				for (int j = 0; j < numplayers; ++j)
					qmatrix[oldstate[j]][oldplayers[j].field][oldplayers[j].mountain][oldplayers[j].seaside][oldplayers[j].coins][oldaction[j] - 1] += n * (oldreward[j] + (players[j].totalreward - areward[j]) + (y * 0) - qmatrix[oldstate[j]][oldplayers[j].field][oldplayers[j].mountain][oldplayers[j].seaside][oldplayers[j].coins][oldaction[j] - 1]);

				for (int j = 0; j < numplayers; ++j)
					trainingtotalreward[j][i] = players[j].totalreward;
				
				if (((i + 1) % 1000) == 0)
					cout << "Episode: " << i + 1 << "/" << N << ", " << ((i + 1.0) / N) * 100 << "%\r";
			}

			outfile("qmatrix.csv", numplayers, qmatrix);

			ofstream fout;

			fout.open("training.csv");

			fout << "Training:\n";

			for (int i = 0; i < numplayers; ++i)
			{
				fout << "Player " << i + 1;

				if (i == (numplayers - 1))
					fout << "\n";
				else
					fout << ",";
			}

			for (int j = 0; j < N; ++j)
			{
				for (int i = 0; i < numplayers; ++i)
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

			for (int i = 0; i < numplayers; ++i)
				delete[] trainingtotalreward[i];

			delete[] trainingtotalreward;
		}

		//Test

		/*float **testtotalreward = new float*[numplayers]; //Total reward gained per test episode

		for (int i = 0; i < numplayers; ++i)
			testtotalreward[i] = new float[N];
		
		for (int i = 0; i < numplayers; ++i)
			for (int j = 0; j < N; ++j)
				testtotalreward[i][j] = 0;*/

		//for(int i = 0; i < N; ++i)
		{
			struct player players[numplayers];

			for (int j = 0; j < numplayers; ++j)
				players[j].locationx = numplayers - j - 1;

			struct meal meals[numplayers + 1][4];

			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < (numplayers + 1); ++k)
				{
					meals[k][j].specialty = mealspecialties[rand() % 25] - 1;
					meals[k][j].cost = mealcost[meals[k][j].specialty];
				}
			}

			outputstate(numplayers, players, 0, 0, 0);

			while (goal(numplayers, players) == false)
			{
				short player = nextplayer(numplayers, players);

				short action = 1;  //Action
				short areward = 0; //Reward

				short nextinn = players[player].locationy + 1;

				while (nextinn < 53 and board[nextinn] != inn)
					++nextinn;

				int currentstate = state(numplayers, players, player, nextinn); //Current state

				bool invalid = false;

				if (player == 0)
				{
					long double max = -LDBL_MAX;

					for (int j = 0; j < 14; ++j)
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

				for (int j = 0; j < numplayers; ++j)
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

			/*for (int j = 0; j < numplayers; ++j)
				testtotalreward[j][i] = players[j].totalreward;*/

			int maxreward = 0;
			short winner = 0;

			for (int j = 0; j < numplayers; ++j)
			{
				if (players[j].totalreward > maxreward)
				{
					maxreward = players[j].totalreward;
					winner = j;
				}
			}

			short count = 0;

			for (int j = 0; j < numplayers; ++j)
				if (players[j].totalreward == maxreward)
					++count;

			if (count > 0)
			{
				short maxachievements = 0;

				for (int j = 0; j < numplayers; ++j)
				{
					if (players[j].totalreward == maxreward and players[j].achievements > maxachievements)
					{
						maxachievements = players[j].achievements;
						winner = j;
					}
				}
			}

			cout << "\nTotal Reward:\n";

			for (int j = 0; j < numplayers; ++j)
				cout << "\tPlayer " << j + 1 << ": " << players[j].totalreward << "\n";

			cout << "\nAchievements:\n";

			for (int j = 0; j < numplayers; ++j)
				cout << "\tPlayer " << j + 1 << ": " << players[j].achievements << "\n";

			if (winner == 0)
				cout << "\n\e[1mPlayer " << winner + 1 << " (Computer) won!\e[0m";
			else
				cout << "\nPlayer " << winner + 1 << " won.";

			cout << endl;
		}

		/*double testaverage[numplayers]; //Test-Average
		double teststandarddeviation[numplayers]; //Test-Standard-Deviation

		for (int i = 0; i < numplayers; ++i)
		{
			testaverage[i] = 0;
			teststandarddeviation[i] = 0;
		}
		
		for (int i = 0; i < numplayers; ++i)
			for (int j = 0; j < N; ++j)
				testaverage[i] += testtotalreward[i][j];
		
		for (int i = 0; i < numplayers; ++i)
			testaverage[i] /= N;
		
		for (int i = 0; i < numplayers; ++i)
			for (int j = 0; j < N; ++j)
				teststandarddeviation[i] += pow(testtotalreward[i][j] - testaverage[i], 2);
		
		for (int i = 0; i < numplayers; ++i)
			teststandarddeviation[i] = sqrt(teststandarddeviation[i] / N);

		ofstream fout;
		fout.open("test.csv");
		
		fout << "Test:\n";
		
		for (int i = 0; i < numplayers; ++i)
		{
			fout << "Player " << i + 1;

			if (i == (numplayers - 1))
				fout << "\n";
			else
				fout << ",";
		}

		for (int j = 0; j < N; ++j)
		{
			for (int i = 0; i < numplayers; ++i)
			{
				fout << testtotalreward[i][j];

				if (i == (numplayers - 1))
					fout << "\n";
				else
					fout << ",";
			}
		}
		
		for (int i = 0; i < numplayers; ++i)
		{
			fout << "Player " << i + 1 << "\n";
			
			fout << "Test Average: " << testaverage[i] << "\n";
			
			fout << "Test Standard Deviation: " << teststandarddeviation[i] << "\n";
		}

		fout.close();
		fout.clear();
		
		for (int i = 0; i < numplayers; ++i)
			delete [] testtotalreward[i];

		delete [] testtotalreward;*/

		cout << "Would you like to start again? (y/n) ";

		cin >> again;
		cin.ignore(100, '\n');

		again = tolower(again);

	} while (again == 'y');

	return 0;
}
