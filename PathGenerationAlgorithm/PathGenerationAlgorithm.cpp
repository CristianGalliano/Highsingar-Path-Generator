#include <iostream>
#include <fstream>
#include <vector> 
#include <string>
#include <random>
#include "Node.h"
#include <time.h>
#include <omp.h>
using namespace std;

#pragma region Variables
int currentNumberExamples = 0;
int maxReached = 0;
int numberExamplesToGenerate;
int gridSize;
int minMoves;
int maxMoves;

bool minNodesReached = false;

std::vector<std::string> stringsToWrite;
std::ofstream out("../Paths/8x8Examples3.txt");
std::string filename;
#pragma endregion

#pragma region Define Functions
void createList();
void workerFunction();
void generatePath(std::vector<Node>& ListOfNodes);
int sqr(int);
int calculateDirection(Node &Temp);
bool noDirection(Node &Temp);
void removeUsedNode(Node &Temp, std::vector<Node>& ListOfNodes);
void calculateBorders(Node &Temp);
std::string writeString(std::vector<int>& NodesVisited);
void writeToFile();
#pragma endregion

int main()
{
	std::cout << "Nuber of Cores : " + omp_get_thread_num();
	std::cout << "Please input the following information followed by the Enter/Return Key;" << "\n" << "Please specify the desired Grid Size: ";
	std::cin >> gridSize;
	std::cout << "Please specify the number of paths you want to generate: ";
	std::cin >> numberExamplesToGenerate;
	std::cout << "Please specify the desired Filename(not including file type): ";
	std::cin >> filename;
	out = std::ofstream("../Paths/" + filename + ".txt");
	maxMoves = sqr(gridSize);
	minMoves = maxMoves / 2;
	clock_t start = clock();
#pragma omp parallel
	workerFunction();
	writeToFile();
	cout << "Execution time: " << (double)(clock() - start)/CLOCKS_PER_SEC;
	out.close();
	exit(0);
}

void workerFunction()
{
	while (!minNodesReached)
	{
		createList();
	}
}

#pragma region Path Generation
void createList()
{
	std::vector<Node> listOfNodes;
	for (int i = 0; i < sqr(gridSize); i++)
	{
		Node temp = Node(i + 1, gridSize);
		calculateBorders(temp);
		listOfNodes.push_back(temp);
	}
	generatePath(listOfNodes);
}

void generatePath(std::vector<Node> &listOfNodes)
{
	bool hit = false;
	std::random_device rd;
	std::mt19937 rand(rd());
	std::uniform_int_distribution <int> range(0, sqr(gridSize) - 1);
	std::vector<int> nodesVisited;
	std::string path = "";
	Node temp = listOfNodes[range(rand)];
	int i = 0;
	while (temp.down != maxMoves + 1 && temp.up != maxMoves + 1 && temp.left != maxMoves + 1 && temp.right != maxMoves + 1)
	{
		temp = listOfNodes[range(rand)];		
	}
	nodesVisited.push_back(temp.value);
	path += temp.value + ",";
	while (!hit)
	{
		if (i >= maxMoves)
		{
			hit = true;
		}
		else
		{
			int next = 0;
			if (!noDirection(temp))
			{
				next = calculateDirection(temp);
				while (next == maxMoves + 1)
				{
					next = calculateDirection(temp);
				}
			}
			else
			{
				hit = true;
			}
			if (!hit)
			{
				Node prev = temp;
				removeUsedNode(prev, listOfNodes);
				if (next != maxMoves + 1)
				{			
					temp = listOfNodes[next - 1];					
				}
				path += temp.value + ",";
				nodesVisited.push_back(temp.value);
			}
		}
		i++;
	}
	if ((int)nodesVisited.size() > minMoves)
	{
		for(int value : nodesVisited)
		{
			path += value + ",";
		}
#pragma omp critical
		currentNumberExamples++;
		if (currentNumberExamples == numberExamplesToGenerate)
		{
			minNodesReached = true;
		}
#pragma omp critical
		stringsToWrite.push_back(writeString(nodesVisited));
	}
}
#pragma endregion

#pragma region MathsFunctions
int sqr(int value)
{
	return value * value;
}

int calculateDirection(Node &temp)
{
	int random = rand() % 4;
	switch (random)
	{
	case 0:
		return temp.left;
	case 1:
		return temp.right;
	case 2:
		return temp.up;
	case 3:
		return temp.down;
	}
	return 0;
}

bool noDirection(Node &temp)
{
	if (temp.up == maxMoves + 1 && temp.down == maxMoves + 1 && temp.left == maxMoves + 1 && temp.right == maxMoves + 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void removeUsedNode(Node &prev, std::vector<Node> &listOfNodes)
{
	for(Node& node : listOfNodes)
	{
		if (node.up == prev.value)
		{
			node.up = maxMoves + 1;
		}
		if (node.down == prev.value)
		{
			node.down = maxMoves + 1;
		}
		if (node.left == prev.value)
		{
			node.left = maxMoves + 1;
		}
		if (node.right == prev.value)
		{			
			node.right = maxMoves + 1;
		}
	}
	prev.value = maxMoves + 1;
}

void calculateBorders(Node &temp)
{
	if (temp.value == 1)
	{
		temp.right = maxMoves + 1;
		temp.down = maxMoves + 1;
	}
	if (temp.value == sqr(gridSize))
	{
		temp.left = maxMoves + 1;
		temp.up = maxMoves + 1;
	}
	if (temp.value == gridSize)
	{
		temp.left = maxMoves + 1;
		temp.down = maxMoves + 1;
	}
	if (temp.value == (gridSize * (gridSize - 1)) + 1)
	{
		temp.right = maxMoves + 1;
		temp.up = maxMoves + 1;
	}
	if (temp.value < gridSize)
	{
		temp.down = maxMoves + 1;
	}
	if (temp.value % (gridSize) == 0)
	{
		temp.left = maxMoves + 1;
	}
	if (temp.value > gridSize * (gridSize - 1))
	{
		temp.up = maxMoves + 1;
	}
	if ((temp.value - 1) % gridSize == 0)
	{
		temp.right = maxMoves + 1;
	}
}
#pragma endregion

#pragma region Output Functions
std::string writeString(std::vector<int>& nodesVisited)
{
	std::string stringTosave = "";
	for (int value : nodesVisited)
	{
		stringTosave += std::to_string(value) + ",";
	}
	return stringTosave;
}

void writeToFile()
{
	for (std::string path : stringsToWrite)
	{
		out << path << "\n";
	}
}
#pragma endregion