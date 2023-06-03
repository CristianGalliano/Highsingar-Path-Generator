#pragma once

class Node
{
public:
	int value;
	int up, down, left, right;
	Node(int Value, int GridSize)
	{
		value = Value;
		up = Value + GridSize;
		down = Value - GridSize;
		left = Value + 1;
		right = Value - 1;
	}
};