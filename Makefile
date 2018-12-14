# Copyright © 2017 Teal Dulcet and Brent McManus
# Copyright © 2018 Teal Dulcet, Andreas Natsis and Daniel Ure

all: main1 main2

main1: main1.cpp tokaido.cpp
	g++ -Wall -g main1.cpp tokaido.cpp -o Project1
	
main2: main2.cpp tokaido.cpp
	g++ -Wall -g main2.cpp tokaido.cpp -o Project2
