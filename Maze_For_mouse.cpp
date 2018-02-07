// Maze generator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CImg.h"
#include <vector>
#include <time.h>
#include <iostream>
#include <conio.h>

using namespace std;
using namespace cimg_library;

const unsigned char white[] = { 255, 255, 255 }, black[] = { 0, 0, 0 }, green[] = { 0, 127, 0 }, blue[] = { 0, 0, 255 }, grey[] = { 127, 127, 127 }, red[] = { 255, 0, 0 };

class cell
{
public:
	bool top, left, right, bottom, visited, cleared;
	cell()
	{
		top = true;
		left = true;
		right = true;
		bottom = true;
		visited = false;
		cleared = false;
	}
};
#define gridsize 20
#define SIDE gridsize

class node
{
public:
	__int8 edgen;
	INT16* edges;

	void init()
	{
		this->edgen = 0;
		this->edges = (INT16*)malloc(sizeof(INT16)*0);
	}

	int connected(int to)
	{
		for (int i = 0; i < this->edgen; i++)
		{
			if (this->edges[i] == to)
				return true;
		}
		return false;
	}
private:
	void connect(int to)
	{
		if (!connected(to))
		{
			this->edgen++;
			this->edges = (INT16*)realloc(this->edges, sizeof(INT16)*((this->edgen)));
			this->edges[(this->edgen) - 1] = to;
		}
	}
	void disconnect(int from)
	{
		int found = 0;
		for (int i = 0; i < this->edgen; i++)
		{
			if (found)
			{
				this->edges[i - 1] = this->edges[i];
			}
			if (this->edges[i] == from) found = 1;
		}
		if (found)
			this->edgen--;

	}
	friend void join(node* graph, int first, int second);
	friend void separate(node* grid, int first, int second);
};

void join(node* graph, int first, int second)
{
	graph[first].connect(second);
	graph[second].connect(first);
}
void separate(node* grid, int first, int second)
{
	grid[first].disconnect(second);
	grid[second].disconnect(first);
}

void draw(CImg <unsigned char>(*im), vector <cell> grid)
{
	(*im).draw_rectangle(0, 0, gridsize * 20 + 1, gridsize * 20 + 1, grey);
	for (int i = 0; i < gridsize*gridsize; i++)
	{
		if (grid[i].visited)
		{
			(*im).draw_rectangle(((i%gridsize) * 20), ((i / gridsize) * 20), (((i%gridsize) + 1) * 20), (((i / gridsize) + 1) * 20), green);
		}

		if (grid[i].cleared)
		{
			(*im).draw_rectangle(((i%gridsize) * 20), ((i / gridsize) * 20), (((i%gridsize) + 1) * 20), (((i / gridsize) + 1) * 20), white);
		}
	}
	for (int i = 0; i < gridsize*gridsize; i++)
	{
		if (grid[i].top)
		{
			(*im).draw_line((i%gridsize) * 20, (i / gridsize) * 20-1, ((i%gridsize) + 1) * 20, (i / gridsize) * 20-1, black);
			(*im).draw_line((i%gridsize) * 20, (i / gridsize) * 20, ((i%gridsize) + 1) * 20, (i / gridsize) * 20, black);
			(*im).draw_line((i%gridsize) * 20, (i / gridsize) * 20+1, ((i%gridsize) + 1) * 20, (i / gridsize) * 20+1, black);
		}
		if (grid[i].bottom)
		{
			(*im).draw_line((i%gridsize) * 20, ((i / gridsize) + 1) * 20-1, ((i%gridsize) + 1) * 20, ((i / gridsize) + 1) * 20-1, black);
			(*im).draw_line((i%gridsize) * 20, ((i / gridsize) + 1) * 20, ((i%gridsize) + 1) * 20, ((i / gridsize) + 1) * 20, black);
			(*im).draw_line((i%gridsize) * 20, ((i / gridsize) + 1) * 20-1, ((i%gridsize) + 1) * 20, ((i / gridsize) + 1) * 20-1, black);
		}
		if (grid[i].left)
		{
			(*im).draw_line((i%gridsize) * 20-1, (i / gridsize) * 20, (i%gridsize) * 20-1, (((i / gridsize) + 1) * 20), black);
			(*im).draw_line((i%gridsize) * 20, (i / gridsize) * 20, (i%gridsize) * 20, (((i / gridsize) + 1) * 20), black);
			(*im).draw_line((i%gridsize) * 20+1, (i / gridsize) * 20, (i%gridsize) * 20+1, (((i / gridsize) + 1) * 20), black);
		}
		if (grid[i].right)
		{
			(*im).draw_line(((i%gridsize) + 1) * 20-1, (i / gridsize) * 20, ((i%gridsize) + 1) * 20-1, (((i / gridsize) + 1) * 20), black);
			(*im).draw_line(((i%gridsize) + 1) * 20, (i / gridsize) * 20, ((i%gridsize) + 1) * 20, (((i / gridsize) + 1) * 20), black);
			(*im).draw_line(((i%gridsize) + 1) * 20+1, (i / gridsize) * 20, ((i%gridsize) + 1) * 20+1, (((i / gridsize) + 1) * 20), black);
		}
	}


}


int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
	int j = 0;
	vector <cell> grid;
	node graph[gridsize*gridsize];
	for (int i = 0; i < (gridsize*gridsize); i++)
	{
		cell a;
		grid.push_back(a);
	}
	vector <int> visited;

	CImg <unsigned char> im(gridsize * 20 + 1, gridsize * 20 + 1, 1, 3, 127);
	draw(&im, grid);

	CImgDisplay disp(im, "aMAZEing");

	int current = 0;
	bool reduced = false;
	visited.push_back(0);
	grid[0].visited = true;
	int chances[] = { -gridsize, gridsize, -1, 1 };

	while (!(disp.is_closed()))
	{
		while (!(disp.is_closed()) && visited.size())
		{

			int i;
			//visited.push_back(current);
			for (i = 0; i < 4; i++)
			{
				int temp = chances[i];
				int index = rand() % (4 - i);
				chances[i] = chances[index];
				chances[index] = temp;
			}
			for (i = 0; i < 4; i++)
			{
				int temp = chances[i];
				int index = rand() % (4 - i);
				chances[i] = chances[index];
				chances[index] = temp;
				cout << chances[i];
			}
			cout << endl;
			cout << visited.size() << endl;
			for (i = 0; i < 4; i++)
			{
				if ((current + chances[i]) < (gridsize*gridsize) && (current + chances[i]) >= 0 && !grid[current + chances[i]].visited)
				{
					if (!((chances[i] == -1 || chances[i] == 1) && ((current / gridsize) != ((current + chances[i]) / gridsize))))
					{
						if (chances[i] == -1)
							grid[current].left = false;
						if (chances[i] == 1)
							grid[current].right = false;
						if (chances[i] == gridsize)
							grid[current].bottom = false;
						if (chances[i] == -gridsize)
							grid[current].top = false;

						current += chances[i];
						visited.push_back(current);

						if (chances[i] == -1)grid[current].right = false;
						if (chances[i] == 1)grid[current].left = false;
						if (chances[i] == gridsize)grid[current].top = false;
						if (chances[i] == -gridsize)grid[current].bottom = false;
						i = 6;
					}
				}
			}
			if (i != 7)
			{
				current = visited.back();
				visited.pop_back();
			}
			if (!grid[current].visited)
				grid[current].visited = true;
			else grid[current].cleared = true;
			draw(&im, grid);
			if (!grid[0].cleared)
				im.draw_rectangle(((current%gridsize) * 20) + 1, ((current / gridsize) * 20) + 1, (((current%gridsize) + 1) * 20) - 1, (((current / gridsize) + 1) * 20) - 1, blue);
			disp.display(im);
			//Sleep(2);
			if (disp.is_keyESC())
			{
				visited.clear();
				grid.clear();
			}
		}
#pragma region "Hurkok képzése"
		if (!reduced)
		{
			int i = 0;
			while (i < gridsize*gridsize / 15)
			{
				int randbox = rand() % (gridsize*gridsize);
				int randwall = rand() % 4;
				if (randwall == 0) if ((randbox%gridsize) != gridsize - 1) if (grid[randbox].right)
				{
					i++; grid[randbox].right = false; grid[randbox + 1].left = false;
				}
				if (randwall == 1) if (randbox / gridsize) if (grid[randbox].top)
				{
					i++; grid[randbox].top = false; grid[randbox-gridsize].bottom = false;
				}
				if (randwall == 2) if (randbox%gridsize) if (grid[randbox].left)
				{
					i++; grid[randbox].left = false; grid[randbox - 1].right = false;
				}
				if (randwall == 3) if ((randbox / gridsize) != gridsize - 1) if (grid[randbox].bottom)
				{
					i++; grid[randbox].bottom = false; grid[randbox+gridsize].top = false;
				}

			}
			draw(&im, grid);
			disp.display(im);
			printf("reduced\n");
			reduced = true;
#pragma endregion

#pragma region "Gráf megrajzolása"
			
			for (int i = 0; i < gridsize*gridsize; i++)
			{
				graph[i].init();
			}
			for (int i = 0; i < gridsize*gridsize; i++)
			{
				if (!(i%gridsize)) printf("\n");
				if (!grid[i].left) join(graph, i, i - 1);
				if (!grid[i].right) join(graph, i, i + 1);
				if (!grid[i].top) join(graph, i, i - gridsize);
				if (!grid[i].bottom) join(graph, i, i + gridsize);
				printf("%i:%i\t", i, graph[i].edgen);

				for (j = 0; j < graph[i].edgen; j++)
				{
					int x1, y1, x2, y2;
					x1 = i % gridsize;
					y1 = i / gridsize;
					x2 = graph[i].edges[j] % gridsize;
					y2 = graph[i].edges[j] / gridsize;
					im.draw_line(x1*20+11, y1*20+11, x2*20+11, y2*20+11, red);
					disp.display(im);
				}
			}
		}

#pragma endregion

#pragma region "Egyszerûsítés"
		
		bool found = true;
		INT16 connections = 0;
		INT8 iterations = 0;
		while (found)
		{
			found = 0;
			iterations++;
			for (INT16 i = 0; i < gridsize*gridsize; i++)
			{
				
					if (graph[i].edgen == 2)
					{
						INT8 x1, y1, x2, y2;
						x1 = graph[i].edges[0] % gridsize;
						y1 = graph[i].edges[0] / gridsize;
						x2 = graph[i].edges[1] % gridsize;
						y2 = graph[i].edges[1] / gridsize;
						if (x1 == x2 || y1 == y2)
						{
							join(graph, graph[i].edges[0], graph[i].edges[1]);
							separate(graph, i, graph[i].edges[1]);
							separate(graph, i, graph[i].edges[0]);
							
							found = true;
							connections++;
						}
					}
			}
		}
		printf("\nconnections: %i\n", connections);
		printf("\niterations: %i\n", iterations);

#pragma endregion

#pragma region "Áthurkolás"
		found = true;
		connections = 0;
		iterations = 0;
		while (found)
		{
			iterations++;
			found = 0;
			for (INT16 i = 0; i < gridsize*gridsize; i++)
			{
					INT8 *x, *y;
					x = (INT8*)malloc(sizeof(INT8)*graph[i].edgen);
					y = (INT8*)malloc(sizeof(INT8)*graph[i].edgen);
					for (INT8 k = 0; k < graph[i].edgen; k++)
					{
						x[k] = graph[i].edges[k] % SIDE;
						y[k] = graph[i].edges[k] / SIDE;
					}
					for (INT8 k = 0; k < graph[i].edgen; k++)
					{
						for (INT8 l = 0; l < graph[i].edgen; l++)
						{
							if (k != l)
							{
								if (x[k] == x[l] || y[k] == y[l])
								{
									if (!graph[graph[i].edges[k]].connected(graph[i].edges[l]))
									{
										join(graph, graph[i].edges[k], graph[i].edges[l]);
										found = 1;
										connections++;
									}
								}
							}
						}



					}
			}
		}
		printf("Shortcuts: %i\n", connections);
		printf("Iterations: %i\n", iterations);
#pragma endregion
		char* edges = (char*)malloc(sizeof(char)* 3);
		for (int i = 0; i < gridsize*gridsize; i++)
		{
			sprintf_s(edges, sizeof(char)*3, "%i", graph[i].edgen);
			int x = i % gridsize;
			int y = i / gridsize;
			im.draw_text(x*20+10,y*20+4,edges, green);
		}
		disp.display(im);
		_getch();
		disp.close();
		disp.wait();
		if (disp.is_keySPACE() || disp.is_keyENTER())
		{
			grid.clear();
			reduced = false;
			for (int i = 0; i < (gridsize*gridsize); i++)
			{
				cell a;
				grid.push_back(a);
			}
			current = 0;
			visited.push_back(0);
		}
		if (disp.is_keyS())
		{
			im.save("./maze.bmp", j++, 2);
		}
	}
	return 0;
}

