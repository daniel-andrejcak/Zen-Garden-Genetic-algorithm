#pragma once


class Garden
{
public:
	
	std::vector<std::vector<unsigned short>> grid;

	Garden(unsigned short x, unsigned short y)
	{
		for (size_t i = 0; i < x; i++)
		{
			std::vector<unsigned short> xAxis(y);
			
			grid.push_back(xAxis);
		}
	}

	~Garden() {};


	void placeStone(unsigned short x, unsigned short y)
	{
		grid[x][y] = -1;
	}
};

class Genes
{
public:
	Genes() {};
	~Genes() {};


};