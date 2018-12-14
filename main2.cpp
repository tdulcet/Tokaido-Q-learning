// Copyright © 2018 Teal Dulcet, Andreas Natsis and Daniel Ure

#include <fstream>
#include "tokaido.h"

using namespace std;

//Sigmoid activation function

long double sigmoid(long double x)
{
	return 1.0 / (1.0 + exp(-x));
}

int main()
{
	char again; //Would you like to start again?

	const int N = 1000000; //Episodes

	srand(time(0));

	const float n = 0.1; //Learning Rate
	// const float y = 0.9;

	const int inputunits = 5;
	const int outputunits = 14;

	const int hiddenunits = 101; //Hidden units

	const float momentum = 0.9; //Momentum

	float weighthidden[hiddenunits][inputunits];  //Hidden weights
	float weightoutput[outputunits][hiddenunits]; //Output weights

	for (int ij = 1; ij < hiddenunits; ++ij)
		for (int j = 0; j < inputunits; ++j)
			weighthidden[ij][j] = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / .1)) - .05;

	for (short ii = 0; ii < outputunits; ++ii)
		for (int ij = 0; ij < hiddenunits; ++ij)
			weightoutput[ii][ij] = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX / .1)) - .05;

	float e = 1;

	do
	{
		short numplayers = 0;

		long double prediction = 0; //Prediction value for current output
		long double value = 0;		//Highest prediction value
		float t = 0;				//t

		//Perceptron

		double aweighthidden[hiddenunits][inputunits];  //Hidden weights
		double aweightoutput[outputunits][hiddenunits]; //Output weights

		for (int ij = 1; ij < hiddenunits; ++ij)
			for (int j = 0; j < inputunits; ++j)
				aweighthidden[ij][j] = weighthidden[ij][j];

		for (short ii = 0; ii < outputunits; ++ii)
			for (int ij = 0; ij < hiddenunits; ++ij)
				aweightoutput[ii][ij] = weightoutput[ii][ij];

		double previousweighthidden[hiddenunits][inputunits];  //Hidden weights
		double previousweightoutput[outputunits][hiddenunits]; //Output weights

		for (int ij = 1; ij < hiddenunits; ++ij)
			for (int j = 0; j < inputunits; ++j)
				previousweighthidden[ij][j] = 0;

		for (short ii = 0; ii < outputunits; ++ii)
			for (int ij = 0; ij < hiddenunits; ++ij)
				previousweightoutput[ii][ij] = 0;

		long double hiddenlayer[hiddenunits]; //Hidden layer

		long double outputlayer[outputunits]; //Output units

		cout << "\nThe Tokaido board game.\n";

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

			// struct player oldplayers[numplayers];

			// short oldaction[numplayers];
			// short oldreward[numplayers];

			int oldstate[numplayers];

			for (int j = 0; j < numplayers; ++j)
			{
				// oldaction[j] = 1;
				// oldreward[j] = 0;

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
					// qmatrix[oldstate[player]][oldplayers[player].field][oldplayers[player].mountain][oldplayers[player].seaside][oldplayers[player].coins][oldaction[player] - 1] += n * (oldreward[player] + (y * qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][action - 1]) - qmatrix[oldstate[player]][oldplayers[player].field][oldplayers[player].mountain][oldplayers[player].seaside][oldplayers[player].coins][oldaction[player] - 1]);

					double input[inputunits] = {currentstate / 1050.0, players[player].field / 4.0, players[player].mountain / 5.0, players[player].seaside / 6.0, players[player].coins / 50.0};

					hiddenlayer[0] = 1;

					for (int ij = 1; ij < hiddenunits; ++ij)
						hiddenlayer[ij] = 0;

					for (short ii = 0; ii < outputunits; ++ii)
						outputlayer[ii] = 0;

					for (int ij = 1; ij < hiddenunits; ++ij)
					{
						long double temp = 0;

						for (int j = 0; j < inputunits; ++j)
						{
							temp += aweighthidden[ij][j] * input[j];
						}

						hiddenlayer[ij] = sigmoid(temp);
					}

					for (short ii = 0; ii < outputunits; ++ii)
					{
						prediction = 0;

						for (int ij = 0; ij < hiddenunits; ++ij)
						{
							prediction += aweightoutput[ii][ij] * hiddenlayer[ij];
						}

						prediction = sigmoid(prediction);

						outputlayer[ii] = prediction;
					}

					double sk[outputunits];

					for (short ii = 0; ii < outputunits; ++ii)
					{
						// if(ii == traininganswers[i])
						t = 0.9;
						// else
						// t = 0.1;

						sk[ii] = outputlayer[ii] * (1 - outputlayer[ii]) * (t - outputlayer[ii]);

						for (int ij = 0; ij < hiddenunits; ++ij)
						{
							double temp = aweightoutput[ii][ij];
							aweightoutput[ii][ij] += (n * sk[ii] * hiddenlayer[ij]) + (momentum * previousweightoutput[ii][ij]);
							previousweightoutput[ii][ij] = aweightoutput[ii][ij] - temp;
						}
					}

					for (int ij = 1; ij < hiddenunits; ++ij)
					{
						double sum = 0;

						for (short ii = 0; ii < outputunits; ++ii)
							sum += aweightoutput[ii][ij] * sk[ii];

						double sj = hiddenlayer[ij] * (1 - hiddenlayer[ij]) * sum;

						for (int j = 0; j < inputunits; ++j)
						{
							double temp = aweighthidden[ij][j];
							aweighthidden[ij][j] += (n * sj * input[j]) + (momentum * previousweighthidden[ij][j]);
							previousweighthidden[ij][j] = aweighthidden[ij][j] - temp;
						}
					}
				}

				// oldstate[player] = currentstate;
				// oldaction[player] = action;
				// oldreward[player] = areward;
				// oldplayers[player].field = players[player].field;
				// oldplayers[player].mountain = players[player].mountain;
				// oldplayers[player].seaside = players[player].seaside;
				// oldplayers[player].coins = players[player].coins;

				if ((i % 10000) == 0 and e > 0.1)
					e -= 0.01;

				float temp = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

				if (temp >= e)
				{
					/*long double max = -LDBL_MAX;

					for (int j = 0; j < 14; ++j)
					{
						if (qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j] > max and invalidaction(numplayers, players, player, j + 1) == false)
						{
							max = qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j];
							action = j + 1;
						}
					}*/

					double input[inputunits] = {currentstate / 1050.0, players[player].field / 4.0, players[player].mountain / 5.0, players[player].seaside / 6.0, players[player].coins / 50.0};

					hiddenlayer[0] = 1;

					for (int ij = 1; ij < hiddenunits; ++ij)
						hiddenlayer[ij] = 0;

					value = LDBL_MIN;

					for (int ij = 1; ij < hiddenunits; ++ij)
					{
						long double temp = 0;

						for (int j = 0; j < inputunits; ++j)
						{
							temp += aweighthidden[ij][j] * input[j];
						}

						hiddenlayer[ij] = sigmoid(temp);
					}

					for (short ii = 0; ii < outputunits; ++ii)
					{
						prediction = 0;

						for (int ij = 0; ij < hiddenunits; ++ij)
						{
							prediction += aweightoutput[ii][ij] * hiddenlayer[ij];
						}

						prediction = sigmoid(prediction);

						if (prediction > value and !invalidaction(numplayers, players, player, ii + 1))
						{
							value = prediction;

							action = ii + 1;
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

			// int areward[numplayers];

			// for (int j = 0; j < numplayers; ++j)
			// areward[j] = players[j].totalreward;

			achievement(numplayers, players);

			for (int k = 0; k < numplayers; ++k)
			{
				// qmatrix[oldstate[j]][oldplayers[j].field][oldplayers[j].mountain][oldplayers[j].seaside][oldplayers[j].coins][oldaction[j] - 1] += n * (oldreward[j] + (players[j].totalreward - areward[j]) + (y * 0) - qmatrix[oldstate[j]][oldplayers[j].field][oldplayers[j].mountain][oldplayers[j].seaside][oldplayers[j].coins][oldaction[j] - 1]);

				double input[inputunits] = {oldstate[k] / 1050.0, players[k].field / 4.0, players[k].mountain / 5.0, players[k].seaside / 6.0, players[k].coins / 50.0};

				hiddenlayer[0] = 1;

				for (int ij = 1; ij < hiddenunits; ++ij)
					hiddenlayer[ij] = 0;

				for (short ii = 0; ii < outputunits; ++ii)
					outputlayer[ii] = 0;

				for (int ij = 1; ij < hiddenunits; ++ij)
				{
					long double temp = 0;

					for (int j = 0; j < inputunits; ++j)
					{
						temp += aweighthidden[ij][j] * input[j];
					}

					hiddenlayer[ij] = sigmoid(temp);
				}

				for (short ii = 0; ii < outputunits; ++ii)
				{
					prediction = 0;

					for (int ij = 0; ij < hiddenunits; ++ij)
					{
						prediction += aweightoutput[ii][ij] * hiddenlayer[ij];
					}

					prediction = sigmoid(prediction);

					outputlayer[ii] = prediction;
				}

				double sk[outputunits];

				for (short ii = 0; ii < outputunits; ++ii)
				{
					// if(ii == traininganswers[i])
					t = 0.9;
					// else
					// t = 0.1;

					sk[ii] = outputlayer[ii] * (1 - outputlayer[ii]) * (t - outputlayer[ii]);

					for (int ij = 0; ij < hiddenunits; ++ij)
					{
						double temp = aweightoutput[ii][ij];
						aweightoutput[ii][ij] += (n * sk[ii] * hiddenlayer[ij]) + (momentum * previousweightoutput[ii][ij]);
						previousweightoutput[ii][ij] = aweightoutput[ii][ij] - temp;
					}
				}

				for (int ij = 1; ij < hiddenunits; ++ij)
				{
					double sum = 0;

					for (short ii = 0; ii < outputunits; ++ii)
						sum += aweightoutput[ii][ij] * sk[ii];

					double sj = hiddenlayer[ij] * (1 - hiddenlayer[ij]) * sum;

					for (int j = 0; j < inputunits; ++j)
					{
						double temp = aweighthidden[ij][j];
						aweighthidden[ij][j] += (n * sj * input[j]) + (momentum * previousweighthidden[ij][j]);
						previousweighthidden[ij][j] = aweighthidden[ij][j] - temp;
					}
				}
			}

			for (int j = 0; j < numplayers; ++j)
				trainingtotalreward[j][i] = players[j].totalreward;

			if (((i + 1) % 1000) == 0)
				cout << "Episode: " << i + 1 << "/" << N << ", " << ((i + 1.0) / N) * 100 << "%\r";
		}

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
					/*long double max = -LDBL_MAX;

					for (int j = 0; j < 14; ++j)
					{
						if (qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j] > max and invalidaction(numplayers, players, player, j + 1) == false)
						{
							max = qmatrix[currentstate][players[player].field][players[player].mountain][players[player].seaside][players[player].coins][j];
							action = j + 1;
						}
					}*/

					double input[inputunits] = {currentstate / 1050.0, players[player].field / 4.0, players[player].mountain / 5.0, players[player].seaside / 6.0, players[player].coins / 50.0};

					hiddenlayer[0] = 1;

					for (int ij = 1; ij < hiddenunits; ++ij)
						hiddenlayer[ij] = 0;

					value = LDBL_MIN;

					for (int ij = 1; ij < hiddenunits; ++ij)
					{
						long double temp = 0;

						for (int j = 0; j < inputunits; ++j)
						{
							temp += aweighthidden[ij][j] * input[j];
						}

						hiddenlayer[ij] = sigmoid(temp);
					}

					for (short ii = 0; ii < outputunits; ++ii)
					{
						prediction = 0;

						for (int ij = 0; ij < hiddenunits; ++ij)
						{
							prediction += aweightoutput[ii][ij] * hiddenlayer[ij];
						}

						prediction = sigmoid(prediction);

						if (prediction > value and !invalidaction(numplayers, players, player, ii + 1))
						{
							value = prediction;

							action = ii + 1;
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
