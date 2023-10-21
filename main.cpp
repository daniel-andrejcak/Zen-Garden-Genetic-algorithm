#include<algorithm>
#include<iostream>
#include<iomanip>
#include<random>
#include<utility>
#include<vector>

#include"Header.h"


Garden blankGarden; //globalny objekt, v ktorom budu len kamene, je to podklad pre vytvorenie ostatnych objektov
std::vector<unsigned short> blankGenes; //globalny objekt genov, ktory sa bude pouzivat na random generovanie prvej generacie


//vypis zahrady
std::ostream& operator<<(std::ostream& os, const Garden& garden)
{
	for (size_t y = 0; y < Y; y++)
	{
		os << '\t';

		for (size_t x = 0; x < X; x++)
		{
			os << std::setw(3) << garden.grid[x][y] << ' ';
		}

		os << std::endl;
	}

	return os;
}


bool customIndividualort(const Individual& individual1, const Individual& individual2) {
	return individual1.fitnesValue > individual2.fitnesValue;
}


//lepsi random generator indexov, najdene na cppreference.com
std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

unsigned short randIndex(unsigned short min, unsigned short max)
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





//nahodne vyberie 2 jedincov a toho, ktory ma vacs fitnes vrati
Individual tournamentSelection(std::vector<Individual> population)
{
	
	Individual individual1 = population[randIndex(0, populationSize - 1)];

	Individual individual2 = population[randIndex(0, populationSize - 1)];

	//kontrola, aby sa 2x nevybral ten isty jedinec
	while (individual1 == individual2)
	{
		individual2 = population[randIndex(0, populationSize - 1)];
	}
	

	return (individual1.fitnesValue >= individual2.fitnesValue) ? individual1 : individual2;
}


/*Individual& rouletteSeleciton(std::vector<Individual> population)
{
	//population.erase(std::find(population.begin(), population.end()));
}*/


//funkcia zabezpecujuca krizenie, vrati 2 potomkov
std::pair<Individual, Individual> crossover(Individual parent1, Individual parent2)
{
	//tu sa bude robit krizenie
	unsigned short splitIndex = randIndex(1, individualSize - 1);

	
	std::vector<unsigned short> child1Genes(parent1.genes.begin(), parent1.genes.begin() + splitIndex);
	std::vector<unsigned short> child2Genes(parent2.genes.begin(), parent2.genes.begin() + splitIndex);


	for (auto& gene : child1Genes)
	{
		auto it = std::find(parent2.genes.begin(), parent2.genes.end(), gene);
		if (it != parent2.genes.end())
			parent2.genes.erase(it);
	}

	
	child1Genes.insert(child1Genes.end(), parent2.genes.end() - (individualSize - child1Genes.size()), parent2.genes.end());


	for (auto& gene : child2Genes)
	{
		auto it = std::find(parent1.genes.begin(), parent1.genes.end(), gene);
		if (it != parent1.genes.end())
			parent1.genes.erase(it);
	}


	child2Genes.insert(child2Genes.end(), parent1.genes.end() - (individualSize - child2Genes.size()), parent1.genes.end());


	/*for (size_t i = 0; i < parent2.genes.size(); i++)
	{
		if (std::find(child1Genes.begin(), child1Genes.end(), parent2.genes[i]) == child1Genes.end())
			child1Genes.push_back(parent2.genes[i]);
	}

	for (size_t i = 0; i < parent1.genes.size(); i++)
	{
		if (std::find(child2Genes.begin(), child2Genes.end(), parent1.genes[i]) == child2Genes.end())
			child2Genes.push_back(parent1.genes[i]);
	}*/


	Individual child1;
	Individual child2;

	child1.genes = child1Genes;
	child2.genes = child2Genes;


	std::pair<Individual, Individual> children;
	children = std::make_pair(child1, child2);

	return children;
}


void mutate(const Individual& individual){}



void loadBlankGenes()
{
	blankGenes = std::vector<unsigned short>((X + Y) * 2);
	for (size_t i = 0; i < (X + Y) * 2; i++)
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


//nahodne vytvori prvu generaciu jedincov - asi to bude std::vector<Genes>
std::vector<Individual> firstGeneration()
{
	std::vector<Individual> firstGeneration;

	for (size_t i = 0; i < populationSize; i++)
	{
		Individual newIndividual = Individual();
		newIndividual.genes = generateRandomGenes();

		newIndividual.fitnesValue = fitnes(newIndividual);


		firstGeneration.push_back(newIndividual);
	}


	return firstGeneration;
}


Individual geneticAlgorithm()
{
	std::vector<Individual> generation = firstGeneration();
	
	for (size_t i = 0; i < 100; i++)
	{
		std::vector<Individual> nextGeneration;

		for (size_t i = 0; i < (populationSize * 0.9 / 2); i++)
		{
			Individual parent1 = tournamentSelection(generation);
			Individual parent2 = tournamentSelection(generation);

			while (parent1 == parent2)
			{
				parent2 = tournamentSelection(generation);
			}

			std::pair<Individual, Individual> children = crossover(parent1, parent2);

			children.first.fitnesValue = fitnes(children.first);
			if (children.first.fitnesValue == desiredFitnesValue)
			{
				return children.first;
			}

			children.second.fitnesValue = fitnes(children.second);
			
			if (children.second.fitnesValue == desiredFitnesValue)
			{
				return children.second;
			}


			nextGeneration.push_back(children.first);
			nextGeneration.push_back(children.second);
		}


		std::sort(generation.begin(), generation.end(), customIndividualort);

		//10% najlepsich sa vzdy presunie do dalsej generacie
		for (size_t i = 0; i < (populationSize * 0.1); i++)
		{
			nextGeneration.push_back(generation[i]);
		}

		generation = nextGeneration;
	}

	 



	Individual bestIndividual = Individual();

	for (auto& i : generation)
	{
		i.fitnesValue = fitnes(i);

		if (i.fitnesValue > bestIndividual.fitnesValue)
		{
			bestIndividual = i;
		}

	}






	return bestIndividual;
}




/*
* 1 generacia bude 50 jedincov, lebo sak preco nie
* v prvej generacii sa nahodne vygeneruju tieto jedince
* najlepsich 10 sa vzdy dostane do dalsej generacie
* 
* krizenie - nahodne(podla ohodnotenia je vacsia alebo mensia sanca) sa nejak vyberie ze ktore sa s ktorymi krizia
* 
* mutacia - nahodne sa nejak skusi mutovat, to este neviem jak funguje :(
*/

/*Ak v�ak pr�de k prek�ke � kame�u alebo u� pohraban�mu piesku � mus� sa oto�i�, ak m� kam.
Ak m� vo�n� smery v�avo aj vpravo, je jeho vec, kam sa oto��. Ak m� vo�n� len jeden smer, oto�� sa tam.
Ak sa nem� kam oto�i�, je koniec hry.*/
int main()
{
	

	//inicializacia
	std::cin >> X >> Y;

	/*--X;
	--Y;*/
	
	blankGarden = Garden(X, Y);

	blankGarden.placeStone(1, 2);
	blankGarden.placeStone(2, 4);
	blankGarden.placeStone(4, 3);
	blankGarden.placeStone(5, 1);
	blankGarden.placeStone(8, 6);
	blankGarden.placeStone(9, 6);

	individualSize = X + Y + stoneCount;
	desiredFitnesValue = X * Y - stoneCount;

	loadBlankGenes();

	//koniec inicializacie

	/*std::vector<Individual> firstGen = firstGeneration();

	Individual individual = Individual();

	individual.genes = generateRandomGenes();
	
	Individual genes = geneticAlgorithm();

	blankGarden.useGenes(individual);

	individual.genes[X + Y] = 1;
	individual.genes[X + Y + 1] = 1;
	individual.genes[X + Y + 2] = 1;
	individual.genes[X + Y + 3] = 0;
	individual.genes[X + Y + 4] = 1;
	individual.genes[X + Y + 5] = 1;


	blankGarden.moveLeft(X - 1, 5, 1, individual);
	blankGarden.moveDown(5, 0, 4, individual);
	blankGarden.moveLeft(X - 1, 3, 2, individual);*/


	/*Individual best = Individual();
	best.genes = { 6, 2, 21, 41, 29, 15, 20, 16, 33, 24, 18, 10, 17, 43, 9, 28, 39, 15, 25, 12, 35, 30, 1, 0, 1, 1, 0, 0, };*/

	Individual best = geneticAlgorithm();
	blankGarden.useGenes(best);

	std::cout << std::endl << std::endl << blankGarden << std::endl << std::endl;

	
	
	return 0;
}