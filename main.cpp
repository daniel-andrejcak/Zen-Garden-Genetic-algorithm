#include<iostream>
#include<vector>
#include<algorithm>

#include"Header.h"


unsigned short X = 0;
unsigned short Y = 0;


//vypis zahrady
std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<unsigned short>>& vec)
{
	for (size_t y = 0; y < Y; y++)
	{
		for (size_t x = 0; x < X; x++)
		{
			os << vec[x][y] << ' ';
		}

		os << std::endl;
	}

	return os;
}


unsigned char fitnes(const Garden& garden, const Genes& genes)
{
	return 0;
}


/*Ak v�ak pr�de k prek�ke � kame�u alebo u� pohraban�mu piesku � mus� sa oto�i�, ak m� kam.
Ak m� vo�n� smery v�avo aj vpravo, je jeho vec, kam sa oto��. Ak m� vo�n� len jeden smer, oto�� sa tam.
Ak sa nem� kam oto�i�, je koniec hry.*/
int main()
{


	//tak ako v zadani
	X = 12;
	Y = 10;
	
	Garden garden = Garden(X, Y);

	garden.placeStone(1, 2);
	garden.placeStone(2, 4);
	garden.placeStone(4, 3);
	garden.placeStone(5, 1);
	garden.placeStone(8, 6);
	garden.placeStone(9, 6);

	
	
	
	
	std::cout << garden.grid << std::endl;

	return 0;
}