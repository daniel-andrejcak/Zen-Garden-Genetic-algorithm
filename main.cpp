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


/*Ak však príde k prekážke – kameòu alebo už pohrabanému piesku – musí sa otoèi, ak má kam.
Ak má vo¾né smery v¾avo aj vpravo, je jeho vec, kam sa otoèí. Ak má vo¾ný len jeden smer, otoèí sa tam.
Ak sa nemá kam otoèi, je koniec hry.*/
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