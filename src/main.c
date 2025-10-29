#include <SDL2/SDL.h>
#include "cs50.h"

#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>


typedef struct
{
	double x;
	double height;
	int rgb[3];
}
Line;

typedef struct
{
	int fov;
	double x;
	double y;
	double angle; 
}
Player;

typedef struct
{
	int fps;
	double apr;
	int width;
	int height;
	int gwidth;
	int gheight;
}
Config;

void draw_player(int height, int width, int delay, Player player, int grid[height][width]);
void draw(int height, int width, int grid[height][width], int px, int py, double angle);
void clean(int height, int width, int space[height][width]);
double fac(double n);
void raycaster(SDL_Renderer* renderer, Player player, double apr, int width, int height, int space[height][width], int swidth, int sheight);
Config input();


int main(void)
{
	Config cfg = input();

	Player player;
	player.fov = 30;
	player.x = cfg.gwidth / 2;
	player.y = cfg.gheight - 2;
	player.angle = 0;
	double rot_speed = 5.0;
	double move_speed = 0.5;
	
	int space[cfg.gheight][cfg.gwidth];
	clean(cfg.gheight, cfg.gwidth, space);

	int delay = 1000 / cfg.fps;
	bool running = true;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event e;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(cfg.width*4, cfg.height*4, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, 4, 4);

	printf("\033[2J\033[1;1H");
	printf("\033[?25l");
	while (running)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				running = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
					   case SDLK_RIGHT:
							player.angle += rot_speed;
							if (player.angle >= 360) player.angle -= 360;
							break;
						case SDLK_LEFT:
							player.angle -= rot_speed;
							if (player.angle < 0) player.angle += 360;
							break;
						case SDLK_UP:
							player.x += cos(player.angle * M_PI / 180.0) * move_speed;
							player.y += sin(player.angle * M_PI / 180.0) * move_speed;
							break;
						case SDLK_DOWN:
							player.x -= cos(player.angle * M_PI / 180.0) * move_speed;
							player.y -= sin(player.angle * M_PI / 180.0) * move_speed;
							break;
				}
			}
		}

		draw_player(cfg.gheight, cfg.gwidth, delay, player, space);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		raycaster(renderer, player, cfg.apr, cfg.gwidth, cfg.gheight, space, cfg.width, cfg.height);
		
		SDL_RenderPresent(renderer);
		SDL_Delay(delay);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	printf("\033[?25h");
	return 0;
}

Config input()
{
	Config cfg;
	cfg.fps = get_int("FPS: ");
	cfg.apr = get_double("Angle per ray: ");
	cfg.height = get_int("Screen height: ");
	cfg.width = get_int("Screen width: ");
	cfg.gheight = get_int("Game height: ");
	cfg.gwidth = get_int("Game width: ");
	return cfg;
}

void raycaster
(SDL_Renderer* renderer, Player player, double apr, int width, int height, int space[height][width], int swidth, int sheight)
{
	// CALCULATIONS

	int ray_count = round(player.fov / apr) + 1;
	Line lines[ray_count];
	double scalar = (double)sheight / 50.0;
	double colour_scalar = 0.1;

	for (int ray = 0; ray < ray_count; ray++)
	{
		// ray positioning
		double ray_angle = player.angle - player.fov / 2.0 + ray * (player.fov / (ray_count - 1));
		double p_ray_x = ray * ((double)swidth / ray_count);
		double theta = (ray_angle * M_PI) / 180;

		// wall positioning
		double wall_x = NAN;
		double wall_y = NAN; 

		double dx = cos(theta);
		double dy = sin(theta);

		double ray_x = player.x;
		double ray_y = player.y;

		int steps;
		for (steps = 0; steps < width + height; steps++)
		{
			int check_x = (int)(ray_x + dx * steps);
			int check_y = (int)(ray_y + dy * steps);

			if (check_x < 0 || check_x >= width || check_y < 0 || check_y >= height) { break; }

			if (space[check_y][check_x] == 1) { break; }
		}
		
		// line
		lines[ray].rgb[0] = 255;
		lines[ray].rgb[1] = 255;
		lines[ray].rgb[2] = 255;
		for (int i = 0; i < 3; i++)
		{
			double brightness = 255.0 / (1.0 + steps * colour_scalar);
			lines[ray].rgb[i] = (int)brightness;
			if (lines[ray].rgb[i] < 0) { lines[ray].rgb[i] = 0; }
		}

		lines[ray].height = sheight - (steps * scalar);
		if (lines[ray].height < 0) { lines[ray].height = 0; }
		lines[ray].x = p_ray_x;
	}

	// RENDERING

	// clear screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	// draw lines
	int line_width = round((double)swidth / ray_count);
	for (int line = 0; line < ray_count; line++)
	{
		int y_pos = 0;
		if (sheight - lines[line].height != 0) { y_pos = (sheight - lines[line].height) / 2; }
		for (int y = 0; y < lines[line].height; y++)
		{
			SDL_SetRenderDrawColor(renderer, lines[line].rgb[0], lines[line].rgb[1], lines[line].rgb[2], 255);
			for (int x = 0; x < line_width; x++)
			{
				SDL_RenderDrawPoint(renderer, (int)round(lines[line].x) + x, y_pos + y);
			}
		}
	}
}

double fac(double n)
{
    if (n < 0) return 0;
    double result = 1.0;
    for (int i = 2; i <= (int)n; i++) {
        result *= i;
    }
    return result;
}

void clean(int height, int width, int space[height][width])
{
	srand(time(NULL));
	int min = 1;
	int max = 100;
	int chance = 10;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int random = min + rand() % (max - min + 1);
			if (random <= chance) { space[y][x] = 1; }
			else { space[y][x] = 0; }

			if (y == 0 || y == height - 1 || x == 0 || x == width - 1)
			{
				space[y][x] = 1;
			}
		}
	}
}

void draw(int height, int width, int grid[height][width], int px, int py, double angle)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (x == px && y == py)
			{
				if (angle >= 315 || angle < 45)
				{
					// north
					printf(">");
				}
				else if (angle >= 45 && angle < 135)
				{
					// east
					printf("|");
				}
				else if (angle >= 135 && angle < 225)
				{
					// south
					printf("<");
				}
				else
				{
					// west
					printf("^");
				}
			}
			if (grid[y][x] == 1)
			{
				printf("▓");
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}
}

void draw_player(int height, int width, int delay, Player player, int grid[height][width])
{
	printf("\033[H");
	int px = (int)round(player.x);
	int py = (int)round(player.y);
	draw(height, width, grid, px, py, player.angle);
	usleep(delay);
}
