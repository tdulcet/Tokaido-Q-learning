// Copyright © 2017 Teal Dulcet and Brent McManus

#include <iostream>
#include <cctype>
// #include <fstream>
// #include <cstring>
// #include <algorithm>
#include <climits>
#include <cfloat>
// #include <cstdlib>
#include <cmath>
#include <unistd.h>

const short inn = 1; //The inn
const short shop = 2; //The shop/village
const short shrine = 3; //The shrine/temple
const short encounter = 4; //An encounter
const short field = 5; //A rice field
const short springs = 6; //The hot springs
const short mountain = 7; //The mountain
const short farm = 8; //A farm
const short seaside = 9; //The seaside

const short objects = 0; //small objects
const short clothing = 1; //clothing
const short art = 2; //art
const short fooddrinks = 3; //food & drinks

const short board[54] = {inn, shop, shrine, encounter, field, springs, mountain, farm, shop, shrine, encounter, seaside, mountain, springs,
						 inn, seaside, shrine, farm, field, mountain, encounter, shrine, springs, mountain, seaside, shop, farm,
						 inn, field, shop, encounter, farm, mountain, springs, seaside, field, shrine, farm, encounter, seaside, shop,
						 inn, springs, shrine, encounter, shop, seaside, springs, encounter, mountain, field, seaside, shop,
						 inn};

const bool twospaces[54] = {false, true, false, false, false, true, true, true, false, true, false, true, false, false,
							false, false, false, true, true, true, true, false, true, false, true, false, false,
							false, false, false, true, false, true, false, true, false, true, true, false, false, true,
							false, false, true, false, true, false, true, false, false, true, true, false,
							false};

const short mealspecialties[25] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 11, 12, 13, 14};
const short mealcost[14] = {1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3};

struct player
{
	short locationy;
	short locationx;
	int totalreward;
	short achievements;
	short coins;
	short field;
	short mountain;
	short seaside;
	short souvenirs[4];
	short meal;
	short specialties[14];
	short shrine;
	short springs;
	short encounter;
	player(void);
	~player(void);
};

struct meal
{
	short cost;
	short specialty;
	bool available;
	meal(void);
	~meal(void);
};

void sensor(short numplayers, player *players, short player, short nextinn, short sensors[]);
int state(short numplayers, player *players, short player, short nextinn);
short nextplayer(short numplayers, player *players);
void achievement(short numplayers, player *players);
short reward(short numplayers, player *players, short player, meal meals[][4]);
bool goal(short numplayers, player *players);
bool invalidaction(short numplayers, player *players, short player, short action);
void outputstate(short numplayers, player *players, short player, short action, short nextinn);
void outputreward(short reward, int totalreward);
