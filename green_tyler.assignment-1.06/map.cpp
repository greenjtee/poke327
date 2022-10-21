#include "map.hpp"

#include <inttypes.h>
#include <random>
#include <limits.h>
#include <cstring>

#include "utility.hpp"
#include "trainer.hpp"
#include "player.hpp"
#include "world.hpp"
#include "heap.h"
#include "ncurses.h"

int32_t path_cmp(const void *key, const void *with)
{
	return ((path_t *)key)->cost - ((path_t *)with)->cost;
}

int32_t trainer_cmp(const void *a, const void *b)
{
	return ((trainer *)a)->next_move - ((trainer *)b)->next_move;
}

int32_t edge_penalty(uint8_t x, uint8_t y)
{
	return (x == 1 || y == 1 || x == map::map_x - 2 || y == map::map_y - 2) ? 2 : 1;
}

map::map(int n, int s, int e, int w, int d, int p, const uint8_t num_trainers, const pair_t player_pos)
{
	this->n = n;
	this->s = s;
	this->e = e;
	this->w = w;

	this->trainers = new trainer[num_trainers];

	this->time = 0;

	heap_init(&this->player_queue, trainer_cmp, 0);

	this->smooth_height();
	this->map_terrain();

	this->place_boulders();
	this->place_trees();
	this->build_paths();

	if ((rand() % 100) < p || !d)
	{
		place_pokemart();
	}

	if ((rand() % 100) < p || !d)
	{
		place_pokecenter();
	}

	place_trainers(num_trainers, player_pos);
}

void map::print(const pair_t player_pos, const uint8_t num_trainers) const
{
	int x, y;
	uint8_t i;

	int default_reached = 0;

	// char map_to_print[MAP_Y][MAP_X+1] = {0};
	// memset(map_to_print, ' ', MAP_X*MAP_Y);

	for (y = 0; y < map::map_y; y++)
	{
		for (x = 0; x < map::map_x; x++)
		{
			attron(COLOR_PAIR(this->terrain[y][x]));
			switch (this->terrain[y][x])
			{
			case ter_boulder:
			case ter_mountain:
				// map_to_print[y][x] = '%';
				mvaddch(y + WINDOW_MAIN_TOP, x, '%');
				break;
			case ter_tree:
			case ter_forest:
				// map_to_print[y][x] = '^';
				mvaddch(y + WINDOW_MAIN_TOP, x, '^');
				break;
			case ter_path:
				// map_to_print[y][x] = '#';
				mvaddch(y + WINDOW_MAIN_TOP, x, '#');
				break;
			case ter_mart:
				// map_to_print[y][x] = 'M';
				mvaddch(y + WINDOW_MAIN_TOP, x, 'M');
				break;
			case ter_center:
				// map_to_print[y][x] = 'C';
				mvaddch(y + WINDOW_MAIN_TOP, x, 'C');
				break;
			case ter_grass:
				// map_to_print[y][x] = ':';
				mvaddch(y + WINDOW_MAIN_TOP, x, ':');
				break;
			case ter_clearing:
				// map_to_print[y][x] = '.';
				mvaddch(y + WINDOW_MAIN_TOP, x, '.');
				break;
			default:
				default_reached = 1;
				break;
			}

			attroff(COLOR_PAIR(this->terrain[y][x]));
		}
	}

	for (i = 0; i < num_trainers; i++)
	{
		char c;

		switch (this->trainers[i].type)
		{
		case trainer_hiker:
			c = 'h';
			break;
		case trainer_rival:
			c = 'r';
			break;
		case trainer_pacer:
			c = 'p';
			break;
		case trainer_wanderer:
			c = 'w';
			break;
		case trainer_sentry:
			c = 's';
			break;
		case trainer_explorer:
			c = 'e';
			break;
		default:
			default_reached = 1;
			break;
		}

		// c = '=';

		mvaddch(this->trainers[i].pos[dim_y] + WINDOW_MAIN_TOP, this->trainers[i].pos[dim_x], c);
		// printf("---------------------------\n");
	}

	mvaddch(player_pos[dim_y] + WINDOW_MAIN_TOP, player_pos[dim_x], '@');

	// printw("%s\n", (char*)map_to_print);

	if (default_reached)
	{
		printw("Default reached in %s\n", __FUNCTION__);
	}
}

void map::find_building_location(pair_t p)
{
	do
	{
		p[dim_x] = rand() % (map_x - 3) + 1;
		p[dim_y] = rand() % (map_y - 3) + 1;

		if ((((this->get_terrain(p[dim_x] - 1, p[dim_y]) == ter_path) &&
			  (this->get_terrain(p[dim_x] - 1, p[dim_y] + 1) == ter_path)) ||
			 ((this->get_terrain(p[dim_x] + 2, p[dim_y]) == ter_path) &&
			  (this->get_terrain(p[dim_x] + 2, p[dim_y] + 1) == ter_path)) ||
			 ((this->get_terrain(p[dim_x], p[dim_y] - 1) == ter_path) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y] - 1) == ter_path)) ||
			 ((this->get_terrain(p[dim_x], p[dim_y] + 2) == ter_path) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y] + 2) == ter_path))) &&
			(((this->get_terrain(p[dim_x], p[dim_y]) != ter_mart) &&
			  (this->get_terrain(p[dim_x], p[dim_y]) != ter_center) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y]) != ter_mart) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y]) != ter_center) &&
			  (this->get_terrain(p[dim_x], p[dim_y] + 1) != ter_mart) &&
			  (this->get_terrain(p[dim_x], p[dim_y] + 1) != ter_center) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y] + 1) != ter_mart) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
			(((this->get_terrain(p[dim_x], p[dim_y]) != ter_path) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y]) != ter_path) &&
			  (this->get_terrain(p[dim_x], p[dim_y] + 1) != ter_path) &&
			  (this->get_terrain(p[dim_x] + 1, p[dim_y] + 1) != ter_path))))
		{
			break;
		}
	} while (1);
}

int gaussian[5][5] = {
	{1, 4, 7, 4, 1},
	{4, 16, 26, 16, 4},
	{7, 26, 41, 26, 7},
	{4, 16, 26, 16, 4},
	{1, 4, 7, 4, 1}};

void map::smooth_height()
{
	int32_t i, x, y;
	int32_t s, t, p, q;
	queue_node_t *head, *tail, *tmp;
	uint8_t height[map_y][map_x];

	memset(&height, 0, sizeof(height));

	/* Seed with some values */
	for (i = 1; i < 255; i += 20)
	{
		do
		{
			x = rand() % map_x;
			y = rand() % map_y;
		} while (height[y][x]);
		height[y][x] = i;
		if (i == 1)
		{
			head = tail = (queue_node_t *)malloc(sizeof(*tail));
		}
		else
		{
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
		}
		tail->next = NULL;
		tail->x = x;
		tail->y = y;
	}

	/* Diffuse the vaules to fill the space */
	while (head)
	{
		x = head->x;
		y = head->y;
		i = height[y][x];

		if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1])
		{
			height[y - 1][x - 1] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x - 1;
			tail->y = y - 1;
		}
		if (x - 1 >= 0 && !height[y][x - 1])
		{
			height[y][x - 1] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x - 1;
			tail->y = y;
		}
		if (x - 1 >= 0 && y + 1 < map_y && !height[y + 1][x - 1])
		{
			height[y + 1][x - 1] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x - 1;
			tail->y = y + 1;
		}
		if (y - 1 >= 0 && !height[y - 1][x])
		{
			height[y - 1][x] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x;
			tail->y = y - 1;
		}
		if (y + 1 < map_y && !height[y + 1][x])
		{
			height[y + 1][x] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x;
			tail->y = y + 1;
		}
		if (x + 1 < map_x && y - 1 >= 0 && !height[y - 1][x + 1])
		{
			height[y - 1][x + 1] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x + 1;
			tail->y = y - 1;
		}
		if (x + 1 < map_x && !height[y][x + 1])
		{
			height[y][x + 1] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x + 1;
			tail->y = y;
		}
		if (x + 1 < map_x && y + 1 < map_y && !height[y + 1][x + 1])
		{
			height[y + 1][x + 1] = i;
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
			tail->next = NULL;
			tail->x = x + 1;
			tail->y = y + 1;
		}

		tmp = head;
		head = head->next;
		free(tmp);
	}

	/* And smooth it a bit with a gaussian convolution */
	for (y = 0; y < map_y; y++)
	{
		for (x = 0; x < map_x; x++)
		{
			for (s = t = p = 0; p < 5; p++)
			{
				for (q = 0; q < 5; q++)
				{
					if (y + (p - 2) >= 0 && y + (p - 2) < map_y &&
						x + (q - 2) >= 0 && x + (q - 2) < map_x)
					{
						s += gaussian[p][q];
						t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
					}
				}
			}
			this->set_height(y, x, t / s);
		}
	}
	/* Let's do it again, until it's smooth like Kenny G. */
	for (y = 0; y < map_y; y++)
	{
		for (x = 0; x < map_x; x++)
		{
			for (s = t = p = 0; p < 5; p++)
			{
				for (q = 0; q < 5; q++)
				{
					if (y + (p - 2) >= 0 && y + (p - 2) < map_y &&
						x + (q - 2) >= 0 && x + (q - 2) < map_x)
					{
						s += gaussian[p][q];
						t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
					}
				}
			}
			this->set_height(y, x, t / s);
		}
	}
}

void map::map_terrain()
{
	int32_t i, x, y;
	queue_node_t *head, *tail, *tmp;
	int num_grass, num_clearing, num_mountain, num_forest, num_total;
	terrain_type_t type;
	int added_current = 0;

	num_grass = rand() % 4 + 2;
	num_clearing = rand() % 4 + 2;
	num_mountain = rand() % 2 + 1;
	num_forest = rand() % 2 + 1;
	num_total = num_grass + num_clearing + num_mountain + num_forest;

	memset(&this->terrain, 0, sizeof(this->terrain));

	/* Seed with some values */
	for (i = 0; i < num_total; i++)
	{
		do
		{
			x = rand() % map_x;
			y = rand() % map_y;
		} while (this->get_terrain(y, x));
		if (i == 0)
		{
			type = ter_grass;
		}
		else if (i == num_grass)
		{
			type = ter_clearing;
		}
		else if (i == num_grass + num_clearing)
		{
			type = ter_mountain;
		}
		else if (i == num_grass + num_clearing + num_mountain)
		{
			type = ter_forest;
		}
		this->set_terrain(y, x, type);
		if (i == 0)
		{
			head = tail = (queue_node_t *)malloc(sizeof(*tail));
		}
		else
		{
			tail->next = (queue_node_t *)malloc(sizeof(*tail));
			tail = tail->next;
		}
		tail->next = NULL;
		tail->x = x;
		tail->y = y;
	}

	/* Diffuse the vaules to fill the space */
	while (head)
	{
		x = head->x;
		y = head->y;
		i = this->get_terrain(y, x);

		if (x - 1 >= 0 && !this->get_terrain(y, x - 1))
		{
			if ((rand() % 100) < 80)
			{
				this->set_terrain(y, x - 1, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x - 1;
				tail->y = y;
			}
			else if (!added_current)
			{
				added_current = 1;
				this->set_terrain(y, x, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x;
				tail->y = y;
			}
		}

		if (y - 1 >= 0 && !this->get_terrain(y - 1, x))
		{
			if ((rand() % 100) < 20)
			{
				this->set_terrain(y - 1, x, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x;
				tail->y = y - 1;
			}
			else if (!added_current)
			{
				added_current = 1;
				this->set_terrain(y, x, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x;
				tail->y = y;
			}
		}

		if (y + 1 < map_y && !this->get_terrain(y + 1, x))
		{
			if ((rand() % 100) < 20)
			{
				this->set_terrain(y + 1, x, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x;
				tail->y = y + 1;
			}
			else if (!added_current)
			{
				added_current = 1;
				this->set_terrain(y, x, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x;
				tail->y = y;
			}
		}

		if (x + 1 < map_x && !this->get_terrain(y, x + 1))
		{
			if ((rand() % 100) < 80)
			{
				this->set_terrain(y, x + 1, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x + 1;
				tail->y = y;
			}
			else if (!added_current)
			{
				added_current = 1;
				this->set_terrain(y, x, (terrain_type_t)i);
				tail->next = (queue_node_t *)malloc(sizeof(*tail));
				tail = tail->next;
				tail->next = NULL;
				tail->x = x;
				tail->y = y;
			}
		}

		added_current = 0;
		tmp = head;
		head = head->next;
		free(tmp);
	}

	for (y = 0; y < map_y; y++)
	{
		for (x = 0; x < map_x; x++)
		{
			if (y == 0 || y == map_y - 1 ||
				x == 0 || x == map_x - 1)
			{
				this->set_terrain(y, x, ter_boulder);
			}
		}
	}

	this->n = n;
	this->s = s;
	this->e = e;
	this->w = w;

	this->set_terrain(0, n, ter_path);
	this->set_terrain(1, n, ter_path);
	this->set_terrain(map_y - 1, s, ter_path);
	this->set_terrain(map_y - 2, s, ter_path);
	this->set_terrain(w, 0, ter_path);
	this->set_terrain(w, 1, ter_path);
	this->set_terrain(e, map_x - 1, ter_path);
	this->set_terrain(e, map_x - 2, ter_path);
}

void map::place_boulders()
{
	int i;
	int x, y;

	for (i = 0; i < min_boulders || rand() % 100 < boulder_prob; i++)
	{
		y = rand() % (map_y - 2) + 1;
		x = rand() % (map_x - 2) + 1;
		if (this->get_terrain(y, x) != ter_forest && this->get_terrain(y, x) != ter_path)
		{
			this->set_terrain(y, x, ter_boulder);
		}
	}
}

void map::place_trees()
{
	int i;
	int x, y;

	for (i = 0; i < min_trees || rand() % 100 < tree_prob; i++)
	{
		y = rand() % (map_y - 2) + 1;
		x = rand() % (map_x - 2) + 1;
		if (this->get_terrain(y, x) != ter_mountain && this->get_terrain(y, x) != ter_path)
		{
			this->set_terrain(y, x, ter_tree);
		}
	}
}

void map::place_pokemart()
{
	pair_t p;

	this->find_building_location(p);

	this->set_terrain(p[dim_x], p[dim_y], ter_mart);
	this->set_terrain(p[dim_x] + 1, p[dim_y], ter_mart);
	this->set_terrain(p[dim_x], p[dim_y] + 1, ter_mart);
	this->set_terrain(p[dim_x] + 1, p[dim_y] + 1, ter_mart);
}

void map::place_pokecenter()
{
	pair_t p;

	this->find_building_location(p);

	this->set_terrain(p[dim_x], p[dim_y], ter_center);
	this->set_terrain(p[dim_x] + 1, p[dim_y], ter_center);
	this->set_terrain(p[dim_x], p[dim_y] + 1, ter_center);
	this->set_terrain(p[dim_x] + 1, p[dim_y] + 1, ter_center);
}

void map::place_trainers(const uint8_t num_trainers, const pair_t player_pos)
{
	uint8_t i, j;
	uint8_t done;

	for (i = 0; i < num_trainers; i++)
	{
		this->trainers[i].pos[dim_x] = 0;
		this->trainers[i].pos[dim_y] = 0;
	}

	if (num_trainers < 1)
	{
		printf("num_trainers should not be less than 1\n");
		return;
	}
	else if (num_trainers == 1)
	{
		if (rand() % 2)
		{ // randomly pick a hiker or rival to place
			this->trainers[0].type = trainer_hiker;
		}
		else
		{
			this->trainers[0].type = trainer_rival;
		}

		done = 0;

		while (!done)
		{
			this->trainers[0].pos[dim_x] = (rand() % (map_x - 2)) + 1;
			this->trainers[0].pos[dim_y] = (rand() % (map_y - 2)) + 1;

			if (player_pos[dim_x] != this->trainers[0].pos[dim_x] ||
				player_pos[dim_y] != this->trainers[0].pos[dim_y])
			{
				done = 1;
			}
		}

		this->trainers[0].velocity[dim_y] = (rand() % 2) * 2 - 1;
		this->trainers[0].velocity[dim_y] = (rand() % 2) * 2 - 1;
	}
	else
	{
		// place hiker
		done = 0;

		while (!done)
		{
			this->trainers[0].pos[dim_x] = (rand() % (map_x - 2)) + 1;
			this->trainers[0].pos[dim_y] = (rand() % (map_y - 2)) + 1;

			if (player_pos[dim_x] != this->trainers[0].pos[dim_x] ||
				player_pos[dim_y] != this->trainers[0].pos[dim_y])
			{
				done = 1;
			}
		}
		this->trainers[0].type = trainer_hiker;
		this->trainers[0].velocity[dim_y] = (rand() % 2) * 2 - 1;
		this->trainers[0].velocity[dim_y] = (rand() % 2) * 2 - 1;

		// place rival
		done = 0;

		while (!done)
		{
			this->trainers[1].pos[dim_x] = (rand() % (map_x - 2)) + 1;
			this->trainers[1].pos[dim_y] = (rand() % (map_y - 2)) + 1;

			if (player_pos[dim_x] != this->trainers[1].pos[dim_x] ||
				player_pos[dim_y] != this->trainers[1].pos[dim_y])
			{
				done = 1;
			}

			if (this->trainers[0].pos[dim_x] != this->trainers[1].pos[dim_x] ||
				this->trainers[0].pos[dim_y] != this->trainers[1].pos[dim_y])
			{
				done = 1;
			}
			else
			{
				done = 0;
			}
		}

		this->trainers[1].type = trainer_rival;
		this->trainers[1].velocity[dim_y] = (rand() % 2) * 2 - 1;
		this->trainers[1].velocity[dim_x] = (rand() % 2) * 2 - 1;

		// place remaining trainers
		for (i = 2; i < num_trainers; i++)
		{			  // place trainer number i
			done = 0; // not done

			while (!done)
			{
				done = 1;

				this->trainers[i].pos[dim_x] = (rand() % (map_x - 2)) + 1;
				this->trainers[i].pos[dim_y] = (rand() % (map_y - 2)) + 1;

				if (player_pos[dim_x] == this->trainers[i].pos[dim_x] &&
					player_pos[dim_y] == this->trainers[i].pos[dim_y])
				{
					done = 0;
					continue;
				}

				for (j = 0; j < i; j++)
				{
					if (this->trainers[j].pos[dim_x] == this->trainers[i].pos[dim_x] &&
						this->trainers[j].pos[dim_y] == this->trainers[i].pos[dim_y])
					{
						done = 0;
						continue;
					}
				}

				this->trainers[i].type = (trainer_type_t)((rand() % (trainer_num - 1)) + 1);

				this->trainers[i].velocity[dim_y] = (rand() % 2) * 2 - 1;
				this->trainers[i].velocity[dim_x] = (rand() % 2) * 2 - 1;
			}
		}
	}

	for (i = 0; i < num_trainers; i++)
	{
		this->trainers[i].next_move = 0;
		// this->trainers[i].n = heap_insert(&this->trainer_queue, &this->trainers[i]);
		heap_insert(&this->player_queue, &this->trainers[i]);
	}
}

int map::build_paths()
{
	pair_t from, to;

	if (this->e != -1 && this->w != -1)
	{
		from[dim_x] = 1;
		to[dim_x] = map_x - 2;
		from[dim_y] = this->w;
		to[dim_y] = this->e;

		dijkstra_path(from, to);
	}

	if (this->n != -1 && this->s != -1)
	{
		from[dim_y] = 1;
		to[dim_y] = map_y - 2;
		from[dim_x] = this->n;
		to[dim_x] = this->s;

		dijkstra_path(from, to);
	}

	if (this->e == -1)
	{
		if (this->s == -1)
		{
			from[dim_x] = 1;
			from[dim_y] = this->w;
			to[dim_x] = this->n;
			to[dim_y] = 1;
		}
		else
		{
			from[dim_x] = 1;
			from[dim_y] = this->w;
			to[dim_x] = this->s;
			to[dim_y] = map_y - 2;
		}

		dijkstra_path(from, to);
	}

	if (this->w == -1)
	{
		if (this->s == -1)
		{
			from[dim_x] = map_x - 2;
			from[dim_y] = this->e;
			to[dim_x] = this->n;
			to[dim_y] = 1;
		}
		else
		{
			from[dim_x] = map_x - 2;
			from[dim_y] = this->e;
			to[dim_x] = this->s;
			to[dim_y] = map_y - 2;
		}

		dijkstra_path(from, to);
	}

	if (this->n == -1)
	{
		if (this->e == -1)
		{
			from[dim_x] = 1;
			from[dim_y] = this->w;
			to[dim_x] = this->s;
			to[dim_y] = map_y - 2;
		}
		else
		{
			from[dim_x] = map_x - 2;
			from[dim_y] = this->e;
			to[dim_x] = this->s;
			to[dim_y] = map_y - 2;
		}

		dijkstra_path(from, to);
	}

	if (this->s == -1)
	{
		if (this->e == -1)
		{
			from[dim_x] = 1;
			from[dim_y] = this->w;
			to[dim_x] = this->n;
			to[dim_y] = 1;
		}
		else
		{
			from[dim_x] = map_x - 2;
			from[dim_y] = this->e;
			to[dim_x] = this->n;
			to[dim_y] = 1;
		}

		dijkstra_path(from, to);
	}

	return 0;
}

void map::dijkstra_path(pair_t from, pair_t to)
{
	static path_t path[map_y][map_x], *p = nullptr;
	static uint32_t initialized = 0;
	heap_t h;
	uint32_t x, y;

	if (!initialized)
	{
		for (y = 0; y < map_y; y++)
		{
			for (x = 0; x < map_x; x++)
			{
				path[y][x].pos[dim_y] = y;
				path[y][x].pos[dim_x] = x;
			}
		}
		initialized = 1;
	}

	for (y = 0; y < map_y; y++)
	{
		for (x = 0; x < map_x; x++)
		{
			path[y][x].cost = INT_MAX;
		}
	}

	path[from[dim_y]][from[dim_x]].cost = 0;

	heap_init(&h, path_cmp, NULL);

	for (y = 1; y < map_y - 1; y++)
	{
		for (x = 1; x < map_x - 1; x++)
		{
			path[y][x].hn = heap_insert(&h, &path[y][x]);
		}
	}

	while ((p = (path_t *)heap_remove_min(&h)))
	{
		p->hn = NULL;

		if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x])
		{
			for (x = to[dim_x], y = to[dim_y];
				 (x != from[dim_x]) || (y != from[dim_y]);
				 p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y])
			{
				this->set_terrain(y, x, ter_path);
				this->set_height(y, x, 0);
			}
			heap_delete(&h);
			return;
		}

		if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
			(path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
			 ((p->cost + this->get_height(p->pos)) *
			  edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1))))
		{
			path[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
				((p->cost + this->get_height(p->pos)) *
				 edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
			path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
			path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
			heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
												 [p->pos[dim_x]]
													 .hn);
		}
		if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
			(path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
			 ((p->cost + this->get_height(p->pos)) *
			  edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]))))
		{
			path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
				((p->cost + this->get_height(p->pos)) *
				 edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
			path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
			path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
			heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
												 [p->pos[dim_x] - 1]
													 .hn);
		}
		if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
			(path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
			 ((p->cost + this->get_height(p->pos)) *
			  edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]))))
		{
			path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
				((p->cost + this->get_height(p->pos)) *
				 edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
			path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
			path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
			heap_decrease_key_no_replace(&h, path[p->pos[dim_y]]
												 [p->pos[dim_x] + 1]
													 .hn);
		}
		if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
			(path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
			 ((p->cost + this->get_height(p->pos)) *
			  edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1))))
		{
			path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
				((p->cost + this->get_height(p->pos)) *
				 edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
			path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
			path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
			heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
												 [p->pos[dim_x]]
													 .hn);
		}
	}
}

void map::dijkstra_map(path_t cost_map[map_y][map_x], pair_t from, trainer_type_t trainer)
{
	int32_t y, x, yl, xl;
	int64_t ter_cost;
	heap_t q;
	int64_t cost_matrix[4][11] = {
		{1, INT_MAX, INT_MAX, 10, 10, 10, 20, 12, INT_MAX, INT_MAX, 10},	  // pc
		{1, INT_MAX, INT_MAX, 10, 50, 50, 15, 12, 15, 15, INT_MAX},			  // hiker
		{1, INT_MAX, INT_MAX, 10, 50, 50, 20, 12, INT_MAX, INT_MAX, INT_MAX}, // rival
		{1, INT_MAX, INT_MAX, 10, 50, 50, 20, 12, INT_MAX, INT_MAX, INT_MAX}, // others
	};

	path_t *u;

	for (y = 0; y < map_y; y++)
	{
		for (x = 0; x < map_x; x++)
		{
			cost_map[y][x].cost = INT_MAX;
			cost_map[y][x].pos[dim_y] = y;
			cost_map[y][x].pos[dim_x] = x;
		}
	}

	cost_map[from[dim_y]][from[dim_x]].cost = 0; // pc location

	heap_init(&q, path_cmp, NULL);
	for (y = 1; y < map_y - 1; y++)
	{
		for (x = 1; x < map_x - 1; x++)
		{
			if (cost_map[y][x].cost != INT_MAX)
			{
				cost_map[y][x].hn = heap_insert(&q, &(cost_map[y][x]));
			}
			else
			{
				cost_map[y][x].hn = NULL;
			}
		}
	}

	while ((u = (path_t *)heap_remove_min(&q)))
	{
		u->hn = NULL;
		xl = u->pos[dim_x];
		yl = u->pos[dim_y];

		for (y = -1; y < 2; y++)
		{
			for (x = -1; x < 2; x++)
			{
				if ((y == 0 && x == 0) || ((yl + y) == (map_y - 1)) || ((yl + y) == 0) || ((xl + x) == (map_x - 1)) || ((xl + x) == 0))
				{
					continue; // only visit neighbors in heap
				}

				ter_cost = cost_matrix[trainer][this->get_terrain(y + yl, x + xl)];

				if (u->cost + ter_cost < cost_map[yl + y][xl + x].cost)
				{
					cost_map[yl + y][xl + x].cost = u->cost + ter_cost;
					cost_map[yl + y][xl + x].from[dim_y] = u->pos[dim_y];
					cost_map[yl + y][xl + x].from[dim_x] = u->pos[dim_x];
					if (cost_map[yl + y][xl + x].hn)
					{
						heap_decrease_key_no_replace(&q, cost_map[yl + y][xl + x].hn);
					}
					else
					{
						cost_map[yl + y][xl + x].hn = heap_insert(&q, &(cost_map[yl + y][xl + x]));
					}
				}
			}
		}
	}

	heap_delete(&q);
}

map::~map()
{
	delete[] trainers;
	heap_delete(&this->player_queue);
}
