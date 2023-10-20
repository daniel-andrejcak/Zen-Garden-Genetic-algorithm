#pragma once

unsigned short X = 0;
unsigned short Y = 0;
unsigned short stoneCount = 0;
unsigned short populationSize = 50;
unsigned short individualSize = 0;
unsigned short desiredFitnesValue = 0;

class Individual
{
public:
	Individual()
	{
		genes = std::vector<unsigned short>(X + Y + stoneCount);
	};


	~Individual() {};

	unsigned short fitnesValue = 0;

	std::vector<unsigned short> genes;


	bool operator!=(const Individual& other) const {
		return this->fitnesValue != other.fitnesValue;
	}

};


class Garden
{
public:
	
	std::vector<std::vector<short>> grid;

	Garden(){}

	Garden(short x, short y)
	{
		for (size_t i = 0; i < x; i++)
		{
			std::vector<short> xAxis(y);
			
			grid.push_back(xAxis);
		}
	}

	~Garden() {};


	void moveDown(unsigned short x, unsigned short yStart, unsigned short moveNumber, const Individual& individual)
	{
		for (unsigned short y = yStart; y < Y; y++)
		{
			if (!grid[x - 1][y])
			{
				grid[x - 1][y] = moveNumber;
			}
			else
			{
				if(individual.genes[X + Y + ((moveNumber + 0) % stoneCount)] == 0 && x != X && !grid[x][y - 1])
				{
					moveRight(x, y, moveNumber, individual);
				}
				else if(x - 1 != 0 && !grid[x - 2][y - 1])
				{
					moveLeft(x - 2, y, moveNumber, individual);
				}

				return;
			}
		}
	}

	void moveUp(unsigned short x, unsigned short yStart, unsigned short moveNumber, const Individual& individual)
	{
		for (short y = yStart; y >= 0; y--)
		{
			if (!grid[x - 1][y])
			{
				grid[x - 1][y] = moveNumber;
			}
			else
			{
				if (individual.genes[X + Y + ((moveNumber + 1) % stoneCount)] == 0 && x - 1 != 0 && !grid[x - 2][y + 1])
				{
					moveLeft(x - 2, y + 2, moveNumber, individual);
				}
				else if (x != X && !grid[x][y + 1])
				{
					moveRight(x, y + 2, moveNumber, individual);
				}

				return;
			}
		}
	}

	void moveLeft(unsigned short xStart, unsigned short y, unsigned short moveNumber, const Individual& individual)
	{
		for (short x = xStart; x >= 0; x--)
		{
			if (!grid[x][y - 1])
			{
				grid[x][y - 1] = moveNumber;
			}
			else
			{
				if (individual.genes[X + Y + ((moveNumber + 2) % stoneCount)] == 0 && y != Y && !grid[x + 1][y])
				{
					moveDown(x + 2, y, moveNumber, individual);
				}
				else if (y - 1 != 0 && !grid[x + 1][y - 2])
				{
					moveUp(x + 2, y - 2, moveNumber, individual);
				}

				return;
			}
		}
	}

	void moveRight(unsigned short xStart, unsigned short y, unsigned short moveNumber, const Individual& individual)
	{
		for (unsigned short x = xStart; x < X; x++)
		{
			if (!grid[x][y - 1])
			{
				grid[x][y - 1] = moveNumber;
			}
			else
			{
				if (individual.genes[X + Y + ((moveNumber + 2) % stoneCount)] == 0 && y - 1 != 0 && !grid[x - 1][y - 2])
				{
					moveUp(x, y - 2, moveNumber, individual);
				}
				else if (y != Y && !grid[x - 1][y])
				{
					moveDown(x, y, moveNumber, individual);
				}

				return;
			}
		}
	}


	//funkcia pouzije geny - pohyby mnicha na garden
	void useGenes(const Individual& individual)
	{
		/*ak je cislo mensie ako X tak je to pohyb z hornej strany,
		  ak je cislo vacsie ako X a mensie rovne X + Y, tak je to pohyb z pravej strany,
		  ak je cislo vacsie ako X + Y a mensie rovne ako X + Y + X, tak je to pohyb zo spodnej strany
		  ak je cislo vacsie ako X + Y + X a mensie rovne ako X + Y + X + Y tak je to pohyb z lavej strany*/
	
		for (unsigned short i = 0; i < X + Y; i++)
		{
			if (individual.genes[i] == 0)
			{
				continue;
			}

			if (individual.genes[i] <= X)
			{
				if (!grid[individual.genes[i] - 1][0])
					moveDown(individual.genes[i], 0, i + 1, individual);
			}
			else if (individual.genes[i] <= X + Y)
			{
				if(!grid[X - 1][(individual.genes[i] - X) - 1])
					moveLeft(X - 1, individual.genes[i] - X, i + 1, individual);
			}
			else if (individual.genes[i] <= X + Y + X)
			{
				if(!grid[(individual.genes[i] - X - Y) - 1][Y - 1])
					moveUp(individual.genes[i] - X - Y, Y - 1, i + 1, individual);
			}
			else if (individual.genes[i] <= X + Y + X + Y)
			{
				if (!grid[0][(individual.genes[i] - X - Y - X) - 1])
					moveRight(0, individual.genes[i] - X - Y - X, i + 1, individual);
			}
		}
	}

	


	void placeStone(short x, short y)
	{
		grid[x][y] = -1;

		stoneCount += 1;
	}
};



