#include <SDL.h>
#include <SDL_opengl.h>

#include <cstdlib>
#include <ctime>

#include "Cube.h"

#define WIDTH 640
#define HEIGHT 360

Cube cube;
int angle[3];

void resize(int width, int height);
void render();

int main(int argc, char **args)
{
	srand((unsigned int)time(NULL));

	uint32_t winFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
	SDL_Window *window = SDL_CreateWindow("Virtual Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, winFlags);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	resize(WIDTH, HEIGHT);

	bool running = true;
	uint64_t oldTime = SDL_GetPerformanceCounter(), newTime;
	float deltaTime;
	while (running)
	{
		newTime = SDL_GetPerformanceCounter();
		deltaTime = (newTime - oldTime) * 1.f / SDL_GetPerformanceFrequency();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					resize(event.window.data1, event.window.data2);
				}
			}
			else if (event.type == SDL_KEYDOWN)
			{
				bool shift = event.key.keysym.mod & KMOD_SHIFT;
				switch (event.key.keysym.sym)
				{
				case SDLK_w:
					angle[0] += 45;
					if (angle[0] > 90)
						angle[0] = 90;
					break;
				case SDLK_s:
					angle[0] -= 45;
					if (angle[0] < -90)
						angle[0] = -90;
					break;
				case SDLK_a:
					angle[1] += 45;
					if (angle[1] > 180)
						angle[1] -= 360;
					break;
				case SDLK_d:
					angle[1] -= 45;
					if (angle[1] <= -180)
						angle[1] += 360;
					break;

				case SDLK_k:
					cube.actions.push({ 0, shift ? -1 : 0 });
					break;
				case SDLK_l:
					cube.actions.push({ 1, shift ? -1 : 0 });
					break;
				case SDLK_SEMICOLON:
					cube.actions.push({ 2, shift ? -1 : 0 });
					break;
				case SDLK_j:
					cube.actions.push({ 3, shift ? -1 : 0 });
					break;
				case SDLK_i:
					cube.actions.push({ 4, shift ? -1 : 0 });
					break;
				case SDLK_m:
					cube.actions.push({ 5, shift ? -1 : 0 });
					break;

				case SDLK_BACKSLASH:
					cube.revert();
					break;
				case SDLK_r:
				{
					int prev = -1, face;
					for (int _ = 0; _ < rand() % 3 + 18; _++)
					{
						do face = rand() % 6; while (face == prev);
						cube.actions.push({ face, rand() % 3 - 1 });
						prev = face;
					}
					break;
				}
				}
			}
		}

		cube.update(deltaTime);
		render();

		SDL_GL_SwapWindow(window);

		SDL_Delay(1);
		oldTime = newTime;
	}

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	return 0;
}

void resize(int width, int height)
{
	const float ar = width * 1.f / height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1., 1., 2., 100.);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void render()
{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();

		glTranslatef(0.f, 0.f, -10.f);
		glRotatef(angle[2] * 1.f, 0.f, 0.f, 1.f);
		glRotatef(angle[0] * 1.f, 1.f, 0.f, 0.f);
		glRotatef(angle[1] * 1.f, 0.f, 1.f, 0.f);

		cube.render();

		glPopMatrix();
}
