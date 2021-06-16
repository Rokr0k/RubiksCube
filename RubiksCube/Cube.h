#pragma once

#include <queue>
#include <stack>

class Cube
{
public:
	struct action
	{
		int face, dir;
		bool returning = false;
	};
private:
	int **cube = nullptr;
	float animAngle[6];
	std::stack<action> history;
	void rotate(int side);
	bool returning;
public:
	std::queue<action> actions;
	Cube();
	~Cube();
	void update(float deltaTime);
	void render();
	void revert();
};