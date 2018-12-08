#include "GLUT.h"
#include <math.h>
#include<time.h>
#include <stdio.h>
#include <vector>
#include "Point2D.h"

using namespace std;


const int W = 600; // window width
const int H = 600; // window height

const int SPACE = 1;
const int WALL = 2;
const int VISITED = 3;
const int START = 4;
const int TARGET = 5;
const int GRAY = 6;
const int PATH = 7;

const int MSIZE = 100;
const double SQSIZE = 2.0 / MSIZE;
int maze[MSIZE][MSIZE];
Point2D* parent[MSIZE][MSIZE];

bool bfs_started = false;
bool dfs_started = false;


//gray queue for BFS algorithm
vector<Point2D*> gray;
double ground[MSIZE][MSIZE];
double eyex = 0, eyey = 90, eyez = 200;

void setupMaze()
{
	int i, j;
	for (i = 0; i < MSIZE; i++)
	{
		maze[i][0] = maze[i][MSIZE - 1] = WALL;	// left and right bound walls of the maze
		maze[0][i] = maze[MSIZE - 1][i] = WALL;	// top and bottom bound walls of the maze
	}

	//set walls
	for (i = 1; i < MSIZE - 1; i++)
	{
		for (j = 1; j < MSIZE - 1; j++)
		{
			if (i % 2 == 0)	//mostly walls
			{
				if (rand() % 100 < 50)
					maze[i][j] = WALL;
			}
			else	//mostly spaces			
			{
				if (rand() % 100 < 25)
					maze[i][j] = WALL;
			}
		}
	}

}

void init()
{
	int i, j;
	srand(time(0));
	Point2D* pt;

	//clean up the maze
	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
			maze[i][j] = SPACE;

	setupMaze();
	maze[MSIZE / 2][MSIZE / 2] = START;
	maze[rand() % MSIZE][rand() % MSIZE] = TARGET;

	pt = new Point2D(MSIZE / 2, MSIZE / 2);
	gray.push_back(pt);	//save the start point in gray

	glClearColor(0.7, 0.7, 0.7, 0);

	glOrtho(-1, 1, -1, 1, -1, 1);
}

void showPath(Point2D* pt)
{
	while (maze[pt->getY()][pt->getX()] != START)
	{
		maze[pt->getY()][pt->getX()] = PATH;
		pt = parent[pt->getY()][pt->getX()];
	}
}

void bfsIteration()
{
	Point2D* pt, *ptAdd;

	if (gray.empty())	//grey is the edges that didn't visited yet
	{
		bfs_started = false;	//there is no path to the target
	}
	else
	{
		pt = gray[0];	//this will be the parent
		gray.erase(gray.begin());	//deque

		//paint pt VISITED
		if (maze[pt->getY()][pt->getX()] == TARGET)	//found target
		{
			bfs_started = false;
		}
		else
		{
			if (maze[pt->getY()][pt->getX()] != START)
				maze[pt->getY()][pt->getX()] = VISITED;	//y is i, x is j

			if (maze[pt->getY() + 1][pt->getX()] == TARGET)	//found target
			{
				bfs_started = false;
			}
			//go up, check non visited - white, neighbors 
			if (maze[pt->getY() + 1][pt->getX()] == SPACE)
			{//add it to gray
				maze[pt->getY() + 1][pt->getX()] = GRAY;
				parent[pt->getY() + 1][pt->getX()] = pt;
				ptAdd = new Point2D(pt->getX(), pt->getY() + 1);	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (maze[pt->getY() - 1][pt->getX()] == TARGET)	//found target
			{
				bfs_started = false;
			}
			//go down
			if (maze[pt->getY() - 1][pt->getX()] == SPACE)
			{//add it to gray
				maze[pt->getY() - 1][pt->getX()] = GRAY;
				parent[pt->getY() - 1][pt->getX()] = pt;
				ptAdd = new Point2D(pt->getX(), pt->getY() - 1);	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (maze[pt->getY()][pt->getX() + 1] == TARGET)	//found target
			{
				bfs_started = false;
			}
			//go right
			if (maze[pt->getY()][pt->getX() + 1] == SPACE)
			{//add it to gray
				maze[pt->getY()][pt->getX() + 1] = GRAY;
				parent[pt->getY()][pt->getX() + 1] = pt;
				ptAdd = new Point2D(pt->getX() + 1, pt->getY());	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (maze[pt->getY()][pt->getX() - 1] == TARGET)	//found target
			{
				bfs_started = false;
			}
			//go left
			if (maze[pt->getY()][pt->getX() - 1] == SPACE)
			{//add it to gray
				maze[pt->getY()][pt->getX() - 1] = GRAY;
				parent[pt->getY()][pt->getX() - 1] = pt;
				ptAdd = new Point2D(pt->getX() - 1, pt->getY());	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (!bfs_started)	//target was found
				showPath(pt);
		}
	}
}

void dfsIteration()
{
	Point2D* pt, *ptAdd;

	if (gray.empty())	//grey is the edges that didn't visited yet
	{
		dfs_started = false;	//there is no path to the target
	}
	else
	{
		pt = gray[gray.size() -1];	//this will be the parent
		gray.pop_back();	//deque

									//paint pt VISITED
		if (maze[pt->getY()][pt->getX()] == TARGET)	//found target
		{
			dfs_started = false;
		}
		else
		{
			if (maze[pt->getY()][pt->getX()] != START)
				maze[pt->getY()][pt->getX()] = VISITED;	//y is i, x is j

			if (maze[pt->getY() + 1][pt->getX()] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go up, check non visited - white, neighbors 
			if (maze[pt->getY() + 1][pt->getX()] == SPACE)
			{//add it to gray
				maze[pt->getY() + 1][pt->getX()] = GRAY;
				parent[pt->getY() + 1][pt->getX()] = pt;
				ptAdd = new Point2D(pt->getX(), pt->getY() + 1);	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (maze[pt->getY() - 1][pt->getX()] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go down
			if (maze[pt->getY() - 1][pt->getX()] == SPACE)
			{//add it to gray
				maze[pt->getY() - 1][pt->getX()] = GRAY;
				parent[pt->getY() - 1][pt->getX()] = pt;
				ptAdd = new Point2D(pt->getX(), pt->getY() - 1);	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (maze[pt->getY()][pt->getX() + 1] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go right
			if (maze[pt->getY()][pt->getX() + 1] == SPACE)
			{//add it to gray
				maze[pt->getY()][pt->getX() + 1] = GRAY;
				parent[pt->getY()][pt->getX() + 1] = pt;
				ptAdd = new Point2D(pt->getX() + 1, pt->getY());	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (maze[pt->getY()][pt->getX() - 1] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go left
			if (maze[pt->getY()][pt->getX() - 1] == SPACE)
			{//add it to gray
				maze[pt->getY()][pt->getX() - 1] = GRAY;
				parent[pt->getY()][pt->getX() - 1] = pt;
				ptAdd = new Point2D(pt->getX() - 1, pt->getY());	// the neighbour to be added to the visiting queue of bfs
				gray.push_back(ptAdd);
			}
			if (!dfs_started)	//target was found
				showPath(pt);
		}
	}
}


void drawPath()
{
	int i, j;
	double m2scr = (2.0) / MSIZE;
	

	for (i = MSIZE; i > 0; i--)
	{
		for (j = MSIZE; j > 0; j--)
		{
			if (maze[i][j] == PATH)
			{
				glColor3d(0.8, 0.5, 1);	//purple
				glBegin(GL_POLYGON);
				glVertex2d(j * SQSIZE - 1 - SQSIZE / 2, i * SQSIZE - 1 + SQSIZE / 2);
				glVertex2d(j * SQSIZE - 1 + SQSIZE / 2, i * SQSIZE - 1 + SQSIZE / 2);
				glVertex2d(j * SQSIZE - 1 + SQSIZE / 2, i * SQSIZE - 1 - SQSIZE / 2);
				glVertex2d(j * SQSIZE - 1 - SQSIZE / 2, i * SQSIZE - 1 - SQSIZE / 2);
				glEnd();
			}
		}
	}
}


//drawing the maze 100x100px - MSIZExMSIZE, setting the color for each pixel in the maze, and displaying it
void drawMaze()
{
	int i, j;
	double m2scr = (2.0) / MSIZE;

	for (i = 0; i < MSIZE; i++)
	{
		for (j = 0; j < MSIZE; j++)
		{
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0.6, 0.4, 0.2);	//brown
				break;
			case SPACE:
				glColor3d(1, 1, 1);	//white
				break;
			case VISITED:
				glColor3d(0, 0.9, 0);	//green
				break;
			case START:
				glColor3d(0, 0, 1);	//blue
				break;
			case TARGET:
				glColor3d(0, 0, 0);	//black
				break;
			case GRAY:
				glColor3d(1, 0.8, 0);	//orange
				break;
			case PATH:
				glColor3d(0.8, 0.5, 1);	//purple
				break;
			}

			glBegin(GL_POLYGON);
				glVertex2d(j * SQSIZE - 1 - SQSIZE / 2, i * SQSIZE - 1 + SQSIZE / 2);
				glVertex2d(j * SQSIZE - 1 + SQSIZE / 2, i * SQSIZE - 1 + SQSIZE / 2);
				glVertex2d(j * SQSIZE - 1 + SQSIZE / 2, i * SQSIZE - 1 - SQSIZE / 2);
				glVertex2d(j * SQSIZE - 1 - SQSIZE / 2, i * SQSIZE - 1 - SQSIZE / 2);
			glEnd();
		}
	}

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT );
	
	drawMaze();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void idle()
{
	if (bfs_started)
		bfsIteration();

	if (dfs_started)
		dfsIteration();

	glutPostRedisplay();// calls indirectly to display
}

//menu for function choosing
void menu(int choice)
{

	switch (choice)
	{
	case 1:
		bfs_started = true;
		break;
	case 2:
		dfs_started = true;
		break;

	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Digits Example ");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	init();
	//create menu
	glutCreateMenu(menu);
	glutAddMenuEntry("BFS", 1);
	glutAddMenuEntry("DFS", 2);
	glutAddMenuEntry("A*", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
}