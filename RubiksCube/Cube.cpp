#include "Cube.h"
#include <SDL_opengl.h>
#include <SDL_thread.h>
#include <stack>

/*
 *           +-------+
 *           | 5 4 3 |
 *           | 6   2 |
 *           | 7 0 1 |
 *   +-------+-------+-------+-------+
 *   | 5 4 3 | 5 4 3 | 5 4 3 | 5 4 3 |
 *   | 6   2 | 6   2 | 6   2 | 6   2 |
 *   | 7 0 1 | 7 0 1 | 7 0 1 | 7 0 1 |
 *   +-------+-------+-------+-------+
 *           | 5 4 3 |
 *           | 6   2 |
 *           | 7 0 1 |
 *           +-------+
 */

/*
 *   4
 * 3 0 1 2
 *   5
 */

/*
 *   y
 * b r g o
 *   w
 */

Cube::Cube()
{
	this->returning = false;
	this->cube = new int*[6];
	for (int i = 0; i < 6; i++)
	{
		this->animAngle[i] = 0;
		this->cube[i] = new int[8];
		for (int j = 0; j < 8; j++)
		{
			this->cube[i][j] = i;
		}
	}
}

Cube::~Cube()
{
	for (int i = 0; i < 6; i++)
	{
		delete[] this->cube[i];
	}
	delete[] this->cube;
}

#define SWAP(a, b) (a)^=(b)^=(a)^=(b)

void rotateFace(int* cube)
{
	for (int i = 0; i < 6; i++)
	{
		SWAP(cube[i], cube[i + 2]);
	}
}
void Cube::rotate(int side)
{
	rotateFace(this->cube[side]);
	switch (side)
	{
	case 0:
		SWAP(this->cube[1][5], this->cube[4][7]);
		SWAP(this->cube[4][7], this->cube[3][1]);
		SWAP(this->cube[3][1], this->cube[5][3]);

		SWAP(this->cube[1][6], this->cube[4][0]);
		SWAP(this->cube[4][0], this->cube[3][2]);
		SWAP(this->cube[3][2], this->cube[5][4]);

		SWAP(this->cube[1][7], this->cube[4][1]);
		SWAP(this->cube[4][1], this->cube[3][3]);
		SWAP(this->cube[3][3], this->cube[5][5]);
		break;
	case 1:
		SWAP(this->cube[0][1], this->cube[5][1]);
		SWAP(this->cube[5][1], this->cube[2][5]);
		SWAP(this->cube[2][5], this->cube[4][1]);

		SWAP(this->cube[0][2], this->cube[5][2]);
		SWAP(this->cube[5][2], this->cube[2][6]);
		SWAP(this->cube[2][6], this->cube[4][2]);

		SWAP(this->cube[0][3], this->cube[5][3]);
		SWAP(this->cube[5][3], this->cube[2][7]);
		SWAP(this->cube[2][7], this->cube[4][3]);
		break;
	case 2:
		SWAP(this->cube[1][1], this->cube[5][7]);
		SWAP(this->cube[5][7], this->cube[3][5]);
		SWAP(this->cube[3][5], this->cube[4][3]);

		SWAP(this->cube[1][2], this->cube[5][0]);
		SWAP(this->cube[5][0], this->cube[3][6]);
		SWAP(this->cube[3][6], this->cube[4][4]);

		SWAP(this->cube[1][3], this->cube[5][1]);
		SWAP(this->cube[5][1], this->cube[3][7]);
		SWAP(this->cube[3][7], this->cube[4][5]);
		break;
	case 3:
		SWAP(this->cube[0][5], this->cube[4][5]);
		SWAP(this->cube[4][5], this->cube[2][1]);
		SWAP(this->cube[2][1], this->cube[5][5]);

		SWAP(this->cube[0][6], this->cube[4][6]);
		SWAP(this->cube[4][6], this->cube[2][2]);
		SWAP(this->cube[2][2], this->cube[5][6]);

		SWAP(this->cube[0][7], this->cube[4][7]);
		SWAP(this->cube[4][7], this->cube[2][3]);
		SWAP(this->cube[2][3], this->cube[5][7]);
		break;
	case 4:
		SWAP(this->cube[0][3], this->cube[1][3]);
		SWAP(this->cube[1][3], this->cube[2][3]);
		SWAP(this->cube[2][3], this->cube[3][3]);

		SWAP(this->cube[0][4], this->cube[1][4]);
		SWAP(this->cube[1][4], this->cube[2][4]);
		SWAP(this->cube[2][4], this->cube[3][4]);

		SWAP(this->cube[0][5], this->cube[1][5]);
		SWAP(this->cube[1][5], this->cube[2][5]);
		SWAP(this->cube[2][5], this->cube[3][5]);
		break;
	case 5:
		SWAP(this->cube[0][1], this->cube[3][1]);
		SWAP(this->cube[3][1], this->cube[2][1]);
		SWAP(this->cube[2][1], this->cube[1][1]);

		SWAP(this->cube[0][0], this->cube[3][0]);
		SWAP(this->cube[3][0], this->cube[2][0]);
		SWAP(this->cube[2][0], this->cube[1][0]);

		SWAP(this->cube[0][7], this->cube[3][7]);
		SWAP(this->cube[3][7], this->cube[2][7]);
		SWAP(this->cube[2][7], this->cube[1][7]);
		break;
	}
}

void Cube::update(float deltaTime)
{
	bool next = true;
	for (int i = 0; i < 6; i++)
	{
		if (this->animAngle[i] > 0)
		{
			this->animAngle[i] -= deltaTime * (this->returning ? 2880.f : 720.f);
			next = false;
			if (this->animAngle[i] < 0)
				this->animAngle[i] = 0;
		}
		else if (this->animAngle[i] < 0)
		{
			this->animAngle[i] += deltaTime * (this->returning ? 2880.f : 720.f);
			next = false;
			if (this->animAngle[i] > 0)
				this->animAngle[i] = 0;
		}
	}
	if (next && this->actions.size() > 0)
	{
		this->returning = this->actions.front().returning;
		if (!this->actions.front().returning)
		{
			if (this->history.empty() || this->history.top().face != this->actions.front().face)
				this->history.push(this->actions.front());
			else
			{
				int rr = this->history.top().dir;
				if (rr >= 0)
					rr++;
				this->history.pop();
				int rrr = this->actions.front().dir;
				if (rrr >= 0)
					rrr++;
				int r = rr + rrr;
				while (r >= 4)
					r -= 4;
				while (r <= -4)
					r += 4;
				switch (r)
				{
				case -3:
				case 1:
					this->history.push({ this->actions.front().face, 0 });
					break;
				case -2:
				case 2:
					this->history.push({ this->actions.front().face, 1 });
					break;
				case -1:
				case 3:
					this->history.push({ this->actions.front().face, -1 });
					break;
				}
			}
		}
		int dir = this->actions.front().dir;
		int cnt, rot;
		switch (dir)
		{
		case -1:
			cnt = 3;
			rot = -1;
			break;
		case 0:
			cnt = 1;
			rot = 1;
			break;
		case 1:
			cnt = 2;
			rot = 2;
			break;
		}
		for (int i = 0; i < cnt; i++)
			this->rotate(this->actions.front().face);
		this->animAngle[this->actions.front().face] = rot * 90.f;
		this->actions.pop();
	}
}

#define COLOR(x) \
switch(x) \
{ \
case 0: \
	glColor3f(1.f, 0.f, 0.f); \
	break; \
case 1: \
	glColor3f(0.f, .0f, 1.f); \
	break; \
case 2: \
	glColor3f(1.f, .5f, 0.f); \
	break; \
case 3: \
	glColor3f(0.f, 1.f, 0.f); \
	break; \
case 4: \
	glColor3f(1.f, 1.f, 1.f); \
	break; \
case 5: \
	glColor3f(1.f, 1.f, 0.f); \
	break; \
}

float coord[6][8][3] = {
	{
		{0.f, -1.f, 1.f},
		{1.f, -1.f, 1.f},
		{1.f, 0.f, 1.f},
		{1.f, 1.f, 1.f},
		{0.f, 1.f, 1.f},
		{-1.f, 1.f, 1.f},
		{-1.f, 0.f, 1.f},
		{-1.f, -1.f, 1.f},
	},
	{
		{1.f, -1.f, 0.f},
		{1.f, -1.f, -1.f},
		{1.f, 0.f, -1.f},
		{1.f, 1.f, -1.f},
		{1.f, 1.f, 0.f},
		{1.f, 1.f, 1.f},
		{1.f, 0.f, 1.f},
		{1.f, -1.f, 1.f},
	},
	{
		{0.f, -1.f, -1.f},
		{-1.f, -1.f, -1.f},
		{-1.f, 0.f, -1.f},
		{-1.f, 1.f, -1.f},
		{0.f, 1.f, -1.f},
		{1.f, 1.f, -1.f},
		{1.f, 0.f, -1.f},
		{1.f, -1.f, -1.f},
	},
	{
		{-1.f, -1.f, 0.f},
		{-1.f, -1.f, 1.f},
		{-1.f, 0.f, 1.f},
		{-1.f, 1.f, 1.f},
		{-1.f, 1.f, 0.f},
		{-1.f, 1.f, -1.f},
		{-1.f, 0.f, -1.f},
		{-1.f, -1.f, -1.f},
	},
	{
		{0.f, 1.f, 1.f},
		{1.f, 1.f, 1.f},
		{1.f, 1.f, 0.f},
		{1.f, 1.f, -1.f},
		{0.f, 1.f, -1.f},
		{-1.f, 1.f, -1.f},
		{-1.f, 1.f, 0.f},
		{-1.f, 1.f, 1.f},
	},
	{
		{0.f, -1.f, -1.f},
		{1.f, -1.f, -1.f},
		{1.f, -1.f, 0.f},
		{1.f, -1.f, 1.f},
		{0.f, -1.f, 1.f},
		{-1.f, -1.f, 1.f},
		{-1.f, -1.f, 0.f},
		{-1.f, -1.f, -1.f},
	},
};
float direction[6][3] = { {0.f, 0.f, 1.f}, {1.f, 0.f, 0.f}, {0.f, 0.f ,-1.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, -1.f, 0.f} };
int faces[6][8][2] = {
	{
		{5, -1},
		{1, 5},
		{1, -1},
		{4, 1},
		{4, -1},
		{3, 4},
		{3, -1},
		{5, 3},
	},
	{
		{5, -1},
		{2, 5},
		{2, -1},
		{4, 2},
		{4, -1},
		{0, 4},
		{0, -1},
		{5, 0},
	},
	{
		{5, -1},
		{3, 5},
		{3, -1},
		{4, 3},
		{4, -1},
		{1, 4},
		{1, -1},
		{5, 1},
	},
	{
		{5, -1},
		{0, 5},
		{0, -1},
		{4, 0},
		{4, -1},
		{2, 4},
		{2, -1},
		{5, 2},
	},
	{
		{0, -1},
		{1, 0},
		{1, -1},
		{2, 1},
		{2, -1},
		{3, 2},
		{3, -1},
		{0, 3},
	},
	{
		{2, -1},
		{1, 2},
		{1, -1},
		{0, 1},
		{0, -1},
		{3, 0},
		{3, -1},
		{2, 3},
	},
};

void renderCube(float x, float y, float z, int xc, int yc, int zc)
{
	glBegin(GL_QUADS);
	if (zc >= 0)
	{
		COLOR(zc);
		glVertex3f(x - .5f, y - .5f, z - .5f);
		glVertex3f(x + .5f, y - .5f, z - .5f);
		glVertex3f(x + .5f, y + .5f, z - .5f);
		glVertex3f(x - .5f, y + .5f, z - .5f);
		glVertex3f(x - .5f, y - .5f, z + .5f);
		glVertex3f(x + .5f, y - .5f, z + .5f);
		glVertex3f(x + .5f, y + .5f, z + .5f);
		glVertex3f(x - .5f, y + .5f, z + .5f);
	}
	if (yc >= 0)
	{
		COLOR(yc);
		glVertex3f(x - .5f, y - .5f, z - .5f);
		glVertex3f(x + .5f, y - .5f, z - .5f);
		glVertex3f(x + .5f, y - .5f, z + .5f);
		glVertex3f(x - .5f, y - .5f, z + .5f);
		glVertex3f(x - .5f, y + .5f, z - .5f);
		glVertex3f(x + .5f, y + .5f, z - .5f);
		glVertex3f(x + .5f, y + .5f, z + .5f);
		glVertex3f(x - .5f, y + .5f, z + .5f);
	}
	if (xc >= 0)
	{
		COLOR(xc);
		glVertex3f(x - .5f, y - .5f, z - .5f);
		glVertex3f(x - .5f, y + .5f, z - .5f);
		glVertex3f(x - .5f, y + .5f, z + .5f);
		glVertex3f(x - .5f, y - .5f, z + .5f);
		glVertex3f(x + .5f, y - .5f, z - .5f);
		glVertex3f(x + .5f, y + .5f, z - .5f);
		glVertex3f(x + .5f, y + .5f, z + .5f);
		glVertex3f(x + .5f, y - .5f, z + .5f);
	}
	glEnd();
}

#define ABS(x) ((x)>0?(x):-(x))
void Cube::render()
{
	for (int i = 0; i < 6; i++)
	{
		glPushMatrix();
		glRotatef(this->animAngle[i], direction[i][0], direction[i][1], direction[i][2]);
		renderCube(direction[i][0], direction[i][1], direction[i][2], i, i, i);
		for (int j = 0; j < 8; j++)
		{
			glPushMatrix();
			glRotatef(this->animAngle[faces[i][j][0]], direction[faces[i][j][0]][0], direction[faces[i][j][0]][1], direction[faces[i][j][0]][2]);
			if(faces[i][j][1] != -1)
				glRotatef(this->animAngle[faces[i][j][1]], direction[faces[i][j][1]][0], direction[faces[i][j][1]][1], direction[faces[i][j][1]][2]);
			renderCube(coord[i][j][0], coord[i][j][1], coord[i][j][2],
				(this->cube[i][j] + 1) * (int)ABS(direction[i][0]) - 1,
				(this->cube[i][j] + 1) * (int)ABS(direction[i][1]) - 1,
				(this->cube[i][j] + 1) * (int)ABS(direction[i][2]) - 1);
			glPopMatrix();
		}
		glPopMatrix();
	}
}

void Cube::revert()
{
	while (!this->actions.empty())
		this->actions.pop();
	while (!this->history.empty())
	{
		int face = this->history.top().face;
		switch (this->history.top().dir)
		{
		case -1:
			this->actions.push({ face, 0, true });
			break;
		case 0:
			this->actions.push({ face, -1, true });
			break;
		case 1:
			this->actions.push({ face, 1, true });
			break;
		}
		this->history.pop();
	}
}
