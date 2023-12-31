#include<algorithm>
#include<iostream>
#include<fstream>
#include<iomanip>
#include<random>
#include<string>
#include<utility>
#include<vector>

#include"Header.h"

unsigned short populationSize = 500;
unsigned short chanceToMutate = 10;
unsigned short maxGenerations = 200;
unsigned short individualSize = 0;
unsigned short desiredFitnesValue = 0;

Garden blankGarden; //globalny objekt, v ktorom budu len kamene, je to podklad pre vytvorenie ostatnych objektov
std::vector<unsigned short> blankGenes; //globalny objekt genov, ktory sa bude pouzivat na random generovanie prvej generacie

//vypis zahrady
std::ostream& operator<<(std::ostream& os, const Garden& garden)
{
	for (size_t y = 0; y < Y; y++)
	{
		std::cout << '\t';

		for (size_t x = 0; x < X; x++)
		{
			os << std::setw(3) << garden.grid[x][y] << ' ';
		}

		std::cout << std::endl;
	}

	return os;
}


bool individualIsMoreThan(const Individual& individual1, const Individual& individual2) {
	return individual1.fitnesValue > individual2.fitnesValue;
}


bool individualIsLessThan(const Individual& individual1, const Individual& individual2) {
	return individual1.fitnesValue < individual2.fitnesValue;
}



//lepsi random generator indexov, najdene na cppreference.com
std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

unsigned int randIndex(unsigned int min, unsigned int max)
{
	std::uniform_int_distribution<> distribution(min, max);

	return distribution(gen);
}


//fitnes funkcia ohodnoti dany gen - spocita pocet pohrabanych policiek
unsigned short fitnes(const Individual& individual)
{
	if (individual.fitnesValue != 0)
		return individual.fitnesValue;

	//z jedinca sa spravi zahrada, ktora sa ohodnoti
	Garden garden;
	garden.grid = blankGarden.grid;
	garden.useGenes(individual);


	unsigned short fitnes = 0;

	for (size_t y = 0; y < Y; y++)
	{
		for (size_t x = 0; x < X; x++)
		{
			if (garden.grid[x][y] != 0 && garden.grid[x][y] != -1)
			{
				fitnes += 1;
			}
		}
	}

	return fitnes;
}


Individual(*selection)(const Generation&);

//nahodne vyberie 2 jedincov a toho, ktory ma vacsiu fitnes vrati
Individual tournamentSelection(const Generation& population)
{

	Individual individual1 = population.individuals[randIndex(0, populationSize - 1)];

	Individual individual2 = population.individuals[randIndex(0, populationSize - 1)];

	//kontrola, aby sa 2x nevybral ten isty jedinec
	while (individual1 == individual2)
		individual2 = population.individuals[randIndex(0, populationSize - 1)];


	return (individual1.fitnesValue >= individual2.fitnesValue) ? individual1 : individual2;
}


Individual rouletteSelection(const Generation& population)
{
	unsigned int partialSum = 0;
	unsigned int randIndividual = randIndex(0, population.fitnesValuesSum);
	Individual individual;

	for (auto& i : population.individuals)
	{
		partialSum += i.fitnesValue;

		if (partialSum >= randIndividual)
		{
			individual = i;

			break;
		}
	}


	return individual;
}

//funkcia zabezpecujuca krizenie, vrati 2 potomkov
std::pair<Individual, Individual> crossover(Individual parent1, Individual parent2)
{
	//tu sa bude robit krizenie
	unsigned short splitIndex = randIndex(1, X + Y - 1);


	std::vector<unsigned short> child1Genes(parent1.genes.begin(), parent1.genes.begin() + splitIndex);
	std::vector<unsigned short> child2Genes(parent2.genes.begin(), parent2.genes.begin() + splitIndex);

	//odstrani duplicitne geny - tie ktore by sa po krizeni mohli v child vyskytnut 2x
	for (auto& gene : child1Genes)
	{
		auto it = std::find(parent2.genes.begin(), parent2.genes.end(), gene);
		if (it != parent2.genes.end())
			parent2.genes.erase(it);
	}

	for (auto& gene : child2Genes)
	{
		auto it = std::find(parent1.genes.begin(), parent1.genes.end(), gene);
		if (it != parent1.genes.end())
			parent1.genes.erase(it);
	}


	child1Genes.insert(child1Genes.end(), parent2.genes.end() - (X + Y + stoneCount - child1Genes.size()), parent2.genes.end() - stoneCount);
	child2Genes.insert(child2Genes.end(), parent1.genes.end() - (X + Y + stoneCount - child2Genes.size()), parent1.genes.end() - stoneCount);


	if (randIndex(0, 1))
	{
		child1Genes.insert(child1Genes.end(), parent1.genes.end() - stoneCount, parent1.genes.end());
		child2Genes.insert(child2Genes.end(), parent2.genes.end() - stoneCount, parent2.genes.end());
	}
	else
	{
		child1Genes.insert(child1Genes.end(), parent2.genes.end() - stoneCount, parent2.genes.end());
		child2Genes.insert(child2Genes.end(), parent1.genes.end() - stoneCount, parent1.genes.end());
	}


	Individual child1;
	Individual child2;

	child1.genes = child1Genes;
	child2.genes = child2Genes;

	std::pair<Individual, Individual> children;
	children = std::make_pair(child1, child2);

	return children;
}



//vrati 2 indexy pre funkciu mutate - prvy je vzdy mensi
std::pair<unsigned short, unsigned short> indexToMutate(unsigned short start, unsigned short end)
{
	unsigned short index1 = randIndex(start, end);
	unsigned short index2 = randIndex(start, end);

	while (index1 == index2)
	{
		index2 = randIndex(0, end);
	}

	std::pair<unsigned short, unsigned short> index;

	if (index1 < index2)
	{
		index = std::make_pair(index1, index2);
	}
	else
	{
		index = std::make_pair(index2, index1);
	}

	return index;
}

void mutate(Individual& individual)
{
	std::pair<unsigned short, unsigned short> index = indexToMutate(0, X + Y - 1);

	if (unsigned short i = randIndex(0, 1); i == 0)
	{
		// vymeni poradie 2 genov
		std::swap(individual.genes[index.first], individual.genes[index.second]);
	}
	else
	{
		// otoci sekvenciu genov
		std::reverse(individual.genes.begin() + index.first, individual.genes.begin() + index.second);
	}
}


void loadBlankGenes()
{
	blankGenes = std::vector<unsigned short>((X + Y) * 2);
	for (unsigned short i = 0; i < (X + Y) * 2; i++)
	{
		blankGenes[i] = i + 1;
	}
}


//vytvorenie nahdonych genov pre jedinca z prvej generacie
//celkovy pocet genov je X + Y + pocet kamenov
std::vector<unsigned short> generateRandomGenes()
{
	std::vector<unsigned short> genes = blankGenes;

	std::shuffle(genes.begin(), genes.end(), gen);

	genes.resize(X + Y);

	for (size_t i = 0; i < stoneCount; i++)
	{
		//nahodne vygenerovane pohyby 0 - vpravo, 1 - vlavo, ked narazi na prekazku
		genes.push_back(randIndex(0, 1));
	}

	return genes;
}

//nahodne vytvori prvu generaciu jedincov
Generation firstGeneration()
{
	Generation generation = Generation();

	for (size_t i = 0; i < populationSize; i++)
	{
		Individual newIndividual = Individual();
		newIndividual.genes = generateRandomGenes();

		newIndividual.fitnesValue = fitnes(newIndividual);


		generation.individuals.push_back(newIndividual);
	}


	return generation;
}



Individual geneticAlgorithm()
{
	Generation generation = firstGeneration();

	for (size_t i = 0; i < maxGenerations; i++)
	{
		generation.sumFitnesValues();

		Generation nextGeneration;

		for (size_t j = 0; j < (populationSize / 2) - 1; j++)
		{
			Individual parent1 = selection(generation);
			Individual parent2 = selection(generation);

			while (parent1 == parent2)
				parent2 = selection(generation);


			std::pair<Individual, Individual> children = crossover(parent1, parent2);



			if (randIndex(0, 100) < chanceToMutate)
				mutate(children.first);

			if (randIndex(0, 100) < chanceToMutate)
				mutate(children.second);



			children.first.fitnesValue = fitnes(children.first);
			if (children.first.fitnesValue == desiredFitnesValue)
			{
				std::cout << desiredFitnesValue << std::endl;
				std::cout << "Riesenie sa naslo v " << i + 1 << ". generacii" << std::endl;

				return children.first;
			}

			children.second.fitnesValue = fitnes(children.second);
			if (children.second.fitnesValue == desiredFitnesValue)
			{
				std::cout << desiredFitnesValue << std::endl;
				std::cout << "Riesenie sa naslo v " << i + 1 << ". generacii" << std::endl;
				return children.second;
			}


			nextGeneration.individuals.push_back(children.first);
			nextGeneration.individuals.push_back(children.second);
		}


		std::sort(generation.individuals.begin(), generation.individuals.end(), individualIsMoreThan);

		//10% najlepsich sa vzdy presunie do dalsej generacie
		for (size_t i = 0; i < 2; i++)
		{
			nextGeneration.individuals.push_back(generation.individuals[i]);
		}

		generation = nextGeneration;

		auto bestIndividual = std::max_element(generation.individuals.begin(), generation.individuals.end(), individualIsLessThan);

		std::cout << (*bestIndividual).fitnesValue << std::endl;
	}



	auto bestIndividual = std::max_element(generation.individuals.begin(), generation.individuals.end(), individualIsLessThan);

	if ((*bestIndividual).fitnesValue == desiredFitnesValue)
	{
		std::cout << desiredFitnesValue << std::endl;
		std::cout << "Riesenie sa naslo v " << maxGenerations << ". generacii" << std::endl;
	}
	else
	{
		std::cout << "Nenaslo sa kompletne riesenie. Ostalo nepohrabanych " << desiredFitnesValue - (*bestIndividual).fitnesValue << " policok" << std::endl;
	}


	return *bestIndividual;
}



/*Ak v�ak pr�de k prek�ke � kame�u alebo u� pohraban�mu piesku � mus� sa oto�i�, ak m� kam.
Ak m� vo�n� smery v�avo aj vpravo, je jeho vec, kam sa oto��. Ak m� vo�n� len jeden smer, oto�� sa tam.
Ak sa nem� kam oto�i�, je koniec hry.*/
int main(int argc, char* argv[])
{
	//spracovanie vstupnych argumentov
	if (argc >= 2)
	{
		populationSize = std::stoi(std::string(argv[1]));
	}

	if (argc >= 3)
	{
		chanceToMutate = std::stoi(std::string(argv[2]));
	}



	//inicializacia
	std::cout << "Zadajte rozmery zahrady" << std::endl;
	std::cin >> X >> Y;


	blankGarden = Garden(X, Y);


	std::cout << "Zadajte pocet kamenov" << std::endl;
	std::cin >> stoneCount;


	std::cout << "Zadajte pozicie jednotlivych kamenov" << std::endl;

	for (size_t i = 0; i < stoneCount; i++)
	{
		unsigned short x = 0;
		unsigned short y = 0;

		std::cin >> x >> y;

		blankGarden.placeStone(x, y);

	}


	std::cout << "Zadajte sposob vyberu jednincov" << std::endl << "R - ruletovy vyber T - turnajovy vyber" << std::endl;
	char s;
	std::cin >> s;

	if (s == 'R')
	{
		selection = rouletteSelection;
	}
	else if (s == 'T')
	{
		selection = tournamentSelection;
	}
	else
	{
		std::cout << "Nespravy znak pre vyber sposobu vyberu jedincov" << std::endl;
		return 1;
	}


	//vzorovy priklad
	//stoneCount = 6;

	//blankGarden.placeStone(1, 2);
	//blankGarden.placeStone(2, 4);
	//blankGarden.placeStone(4, 3);
	//blankGarden.placeStone(5, 1);
	//blankGarden.placeStone(8, 6);
	//blankGarden.placeStone(9, 6);


	individualSize = X + Y + stoneCount;
	desiredFitnesValue = X * Y - stoneCount;

	loadBlankGenes();

	//koniec inicializacie


	Individual best = geneticAlgorithm();



	blankGarden.useGenes(best);


	std::cout << std::endl << std::endl << blankGarden << std::endl << std::endl;



	return 0;
}