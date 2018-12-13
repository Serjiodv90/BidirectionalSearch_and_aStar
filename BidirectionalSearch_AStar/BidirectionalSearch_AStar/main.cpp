#include "GLUT.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>

#include "Point2D.h"
#include "ComparePointsByDist.h"
using namespace std;


const int W = 600; // window width
const int H = 600; // window height

const int SPACE = 1;
const int WALL = 2;
const int VISITED_FROM_START = 3;
const int VISITED_FROM_TARGET = 4;
const int START = 5;
const int TARGET = 6;
const int GRAY = 7;
const int PATH = 8;


const int MSIZE = 100;
const double SQSIZE = 2.0 / MSIZE;
int maze[MSIZE][MSIZE];
Point2D* parent_forStartPath[MSIZE][MSIZE];
Point2D* parent_forTargetPath[MSIZE][MSIZE];

bool bfs_start_started = false;
bool bfs_target_started = false;
bool dfs_started = false;
bool isPathBeenDrawed = true;
bool aStar_started = false;


//gray queue for BFS algorithm
vector<Point2D*> gray_start;
vector<Point2D*> gray_target;
Point2D* startPoint, *targetPoint;

priority_queue<Point2D, vector<Point2D*>, ComparePointsByDist> grayPQ;
typedef priority_queue<Point2D, vector<Point2D*>, ComparePointsByDist> grayPriorityQueue;
const int aStarRootLevel = 0;

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

void initStartAndTargetPoints()
{
	
	int targetCordinate_row, targetCordinate_col;

	//set the start point in the maze
	maze[MSIZE / 2][MSIZE / 2] = START;
	startPoint = new Point2D(MSIZE / 2, MSIZE / 2);
	startPoint->set_f(startPoint, aStarRootLevel);
	gray_start.push_back(startPoint);	//save the start point in gray
	grayPQ.push(startPoint);

	//set the target point in the maze
	targetCordinate_row = rand() % MSIZE;
	targetCordinate_col = rand() % MSIZE;
	maze[targetCordinate_row][targetCordinate_col] = TARGET;
	targetPoint = new Point2D(targetCordinate_col, targetCordinate_row);
	targetPoint->set_f(targetPoint, aStarRootLevel);
	gray_target.push_back(targetPoint);
}

void cleanGrays()
{
	if (!gray_start.empty())
		gray_start.clear();
	if (!gray_target.empty())
		gray_target.clear();
	if (!grayPQ.empty())
		grayPQ = grayPriorityQueue();
}

void initAlgorithmsFlags()
{
	bfs_start_started = false;
	bfs_target_started = false;
	dfs_started = false;
	isPathBeenDrawed = true;
}

//clean the maze from anything but start, target and walls
void cleanUpMaze()
{
	int i, j;
	
	cleanGrays();

	gray_start.push_back(startPoint);
	gray_target.push_back(targetPoint);
	grayPQ.push(startPoint);

	initAlgorithmsFlags();

	//clean up the maze
	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
		{
			if (maze[i][j] == VISITED_FROM_START || maze[i][j] == VISITED_FROM_TARGET || maze[i][j] == GRAY || maze[i][j] == PATH)
				maze[i][j] = SPACE;
		}
}

//create a new maze pattern (start point, and target point will remain their positions)
void scrumbleMaze()
{
	int i, j;

	//clean up the maze
	for (i = 0; i < MSIZE; i++)
		for (j = 0; j < MSIZE; j++)
			maze[i][j] = SPACE;

	cleanGrays();

	setupMaze();
}

void init()
{
	initAlgorithmsFlags();

	srand(time(0));
	
	scrumbleMaze();
	initStartAndTargetPoints();
	

	glClearColor(0.7, 0.7, 0.7, 0);

	glOrtho(-1, 1, -1, 1, -1, 1);
}

void showPathFromStart(Point2D* pt)
{
	Point2D* pt1 = pt;

	while (pt1 != NULL && maze[pt1->getY()][pt1->getX()] != START)
	{
		if(maze[pt1->getY()][pt1->getX()] != TARGET)
			maze[pt1->getY()][pt1->getX()] = PATH;
		pt1 = parent_forStartPath[pt1->getY()][pt1->getX()];
	}
}

void showPathFromTarget(Point2D* pt)
{
	Point2D* pt1 = pt;

	while (pt1 != NULL && maze[pt1->getY()][pt1->getX()] != TARGET )
	{
		maze[pt1->getY()][pt1->getX()] = PATH;
		pt1 = parent_forTargetPath[pt1->getY()][pt1->getX()];
	}
}

void showBiderectionalPath()
{
	Point2D* pt1, *pt2;
	Point2D* lastVisitedPointFromStart = gray_start[gray_start.size() - 1];
	Point2D* lastVisitedPointFromTarget = gray_target[gray_target.size() - 1];

	if (lastVisitedPointFromStart == lastVisitedPointFromTarget)
	{
		pt1 = new Point2D(lastVisitedPointFromStart->getX(), lastVisitedPointFromStart->getY());
		pt2 = new Point2D(lastVisitedPointFromStart->getX(), lastVisitedPointFromStart->getY());
	}
	else
	{
		pt1 = new Point2D(lastVisitedPointFromStart->getX(), lastVisitedPointFromStart->getY());
		pt2 = new Point2D(lastVisitedPointFromTarget->getX(), lastVisitedPointFromTarget->getY());
	}
	
	showPathFromStart(pt1);
	showPathFromTarget(pt2);
}

void saveIntersectionPoint(int row, int col)
{
	Point2D* ptAddToGray = new Point2D(col, row);

	gray_start.push_back(ptAddToGray);
	gray_target.push_back(ptAddToGray);
}

bool isBfsFoundPath(int row, int col, int goalPoint, int visitedFrom)
{
	bool stopIteration = false;
	if (maze[row][col] == visitedFrom)
	{
		saveIntersectionPoint(row, col);
		stopIteration = true;
	}

	if (maze[row][col] == goalPoint)   //found target
		stopIteration = true;

	if (stopIteration)
	{
		bfs_start_started = false;
		bfs_target_started = false;
		isPathBeenDrawed = false;
		aStar_started = false;
	}

	return stopIteration;
}

void storeCurrentPointInParrentArray(int row, int col, Point2D* parentPoint, Point2D* parentArray[][MSIZE], vector<Point2D*> &grayArray)
{
	Point2D* ptAddToGray = new Point2D(col, row);
	parentArray[row][col] = parentPoint;
	grayArray.push_back(ptAddToGray);
}

void storeCurrentPointForAstar(int row, int col, Point2D* parentPoint)
{
	Point2D* ptAddToGray = new Point2D(col, row);
	ptAddToGray->set_f(targetPoint, parentPoint->get_g() + 1);
	parent_forStartPath[row][col] = parentPoint;
	grayPQ.push(ptAddToGray);
}

bool setPointAsGray(int& mazeRow, int& mazeCol, Point2D*& parentPoint, Point2D* parentArray[][MSIZE], vector<Point2D*> &grayVector, int& goalPoint, int& visitedFrom)
{
	bool isNewPointGray = true;
	if (isBfsFoundPath(mazeRow, mazeCol, goalPoint, visitedFrom))	//found target		
	{
		isNewPointGray = false;
		storeCurrentPointInParrentArray(mazeRow, mazeCol, parentPoint, parentArray, grayVector);
	}
	else if (maze[mazeRow][mazeCol] == SPACE)
	{
		maze[mazeRow][mazeCol] = GRAY;	//add it to gray
		storeCurrentPointInParrentArray(mazeRow, mazeCol, parentPoint, parentArray, grayVector);
	}
	

	return isNewPointGray;
}

void bfsIteration(Point2D* parentArray[][MSIZE], vector<Point2D*> &grayVector, int beginPoint, 
	int beginPoint_visitedFrom, int goalPoint, int goalPoint_visitedFrom)
{
	Point2D* pt;
	int mazeRow, mazeCol;

	if (grayVector.empty())	//grey is the edges that didn't visited yet
	{
		bfs_start_started = false;	//there is no path to the target
		bfs_target_started = false;
	}
	else
	{
		pt = grayVector[0];	//this will be the parent
		grayVector.erase(grayVector.begin());	//deque

		mazeRow = pt->getY();
		mazeCol = pt->getX();

		//paint pt VISITED
		if (isBfsFoundPath(mazeRow, mazeCol, goalPoint, goalPoint_visitedFrom))	//found target	
		{
			storeCurrentPointInParrentArray(mazeRow, mazeCol, pt, parentArray, grayVector);
			return;
		}
		
		else
		{
			if (maze[mazeRow][mazeCol] != beginPoint)
				maze[mazeRow][mazeCol] = beginPoint_visitedFrom;	//y is i, x is j

			//check down
			mazeRow = pt->getY() + 1;
			if (setPointAsGray(mazeRow, mazeCol, pt, parentArray, grayVector, goalPoint, goalPoint_visitedFrom))
			{
			//check up
				mazeRow = pt->getY() - 1;
				if (setPointAsGray(mazeRow, mazeCol, pt, parentArray, grayVector, goalPoint, goalPoint_visitedFrom))
				{
					//check right
					mazeRow = pt->getY();
					mazeCol = pt->getX() + 1;
					if (setPointAsGray(mazeRow, mazeCol, pt, parentArray, grayVector, goalPoint, goalPoint_visitedFrom))
					{
						//check left
						mazeCol = pt->getX() - 1;
						setPointAsGray(mazeRow, mazeCol, pt, parentArray, grayVector, goalPoint, goalPoint_visitedFrom);

					}
				}
			}
		}
	}
}

void setPointAsGrayForAStar(int& mazeRow, int& mazeCol, Point2D*& parentPoint)
{
	if (isBfsFoundPath(mazeRow, mazeCol, TARGET, VISITED_FROM_TARGET))	//found target			
		storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint);
	else if (maze[mazeRow][mazeCol] == SPACE)
	{
		maze[mazeRow][mazeCol] = GRAY;
		storeCurrentPointForAstar(mazeRow, mazeCol, parentPoint);
	}
}

void a_starIteration()
{
	Point2D* pt;
	int mazeRow, mazeCol;

	if (grayPQ.empty())	//grey is the edges that didn't visited yet
		aStar_started = false;	//there is no path to the target
	else
	{
		pt = grayPQ.top();	//this will be the parent
		grayPQ.pop();
		
		mazeRow = pt->getY();
		mazeCol = pt->getX();

		//paint pt VISITED
		if (isBfsFoundPath(mazeRow, mazeCol, TARGET, VISITED_FROM_TARGET))	//found target	
			storeCurrentPointForAstar(mazeRow, mazeCol, pt);

		else
		{
			if (maze[mazeRow][mazeCol] != START)
				maze[mazeRow][mazeCol] = VISITED_FROM_START;	//y is i, x is j

			//check down
			mazeRow = pt->getY() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt);

			//check up
			mazeRow = pt->getY() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt);

			//check right
			mazeRow = pt->getY();
			mazeCol = pt->getX() + 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt);

			//check left
			mazeCol = pt->getX() - 1;
			setPointAsGrayForAStar(mazeRow, mazeCol, pt);

			if (!aStar_started)	//target was found
				showPathFromStart(pt);
		}
	}
}

void dfsIteration()
{
	Point2D* pt, *ptAdd;

	if (gray_start.empty())	//grey is the edges that didn't visited yet
	{
		dfs_started = false;	//there is no path to the target
	}
	else
	{
		pt = gray_start[gray_start.size() -1];	//this will be the parent
		gray_start.pop_back();	//deque

									//paint pt VISITED
		if (maze[pt->getY()][pt->getX()] == TARGET)	//found target
		{
			dfs_started = false;
		}
		else
		{
			if (maze[pt->getY()][pt->getX()] != START)
				maze[pt->getY()][pt->getX()] = VISITED_FROM_START;	//y is i, x is j

			if (maze[pt->getY() + 1][pt->getX()] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go up, check non visited - white, neighbors 
			if (maze[pt->getY() + 1][pt->getX()] == SPACE)
			{//add it to gray
				maze[pt->getY() + 1][pt->getX()] = GRAY;
				parent_forStartPath[pt->getY() + 1][pt->getX()] = pt;
				ptAdd = new Point2D(pt->getX(), pt->getY() + 1);	// the neighbour to be added to the visiting queue of bfs
				gray_start.push_back(ptAdd);
			}
			if (maze[pt->getY() - 1][pt->getX()] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go down
			if (maze[pt->getY() - 1][pt->getX()] == SPACE)
			{//add it to gray
				maze[pt->getY() - 1][pt->getX()] = GRAY;
				parent_forStartPath[pt->getY() - 1][pt->getX()] = pt;
				ptAdd = new Point2D(pt->getX(), pt->getY() - 1);	// the neighbour to be added to the visiting queue of bfs
				gray_start.push_back(ptAdd);
			}
			if (maze[pt->getY()][pt->getX() + 1] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go right
			if (maze[pt->getY()][pt->getX() + 1] == SPACE)
			{//add it to gray
				maze[pt->getY()][pt->getX() + 1] = GRAY;
				parent_forStartPath[pt->getY()][pt->getX() + 1] = pt;
				ptAdd = new Point2D(pt->getX() + 1, pt->getY());	// the neighbour to be added to the visiting queue of bfs
				gray_start.push_back(ptAdd);
			}
			if (maze[pt->getY()][pt->getX() - 1] == TARGET)	//found target
			{
				dfs_started = false;
			}
			//go left
			if (maze[pt->getY()][pt->getX() - 1] == SPACE)
			{//add it to gray
				maze[pt->getY()][pt->getX() - 1] = GRAY;
				parent_forStartPath[pt->getY()][pt->getX() - 1] = pt;
				ptAdd = new Point2D(pt->getX() - 1, pt->getY());	// the neighbour to be added to the visiting queue of bfs
				gray_start.push_back(ptAdd);
			}
			if (!dfs_started)	//target was found
				showPathFromStart(pt);
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
			case VISITED_FROM_START:
				glColor3d(0, 0.9, 0);	//green
				break;
			case VISITED_FROM_TARGET:
				glColor3d(0.6, 0.9, 0);	//green
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
	if (bfs_start_started)
		bfsIteration(parent_forStartPath, gray_start, START, VISITED_FROM_START, TARGET, VISITED_FROM_TARGET);
	
	if(bfs_target_started)
		bfsIteration(parent_forTargetPath, gray_target, TARGET, VISITED_FROM_TARGET, START, VISITED_FROM_START);

	if (!isPathBeenDrawed)
	{
		showBiderectionalPath();
		isPathBeenDrawed = true;
	}

	if (dfs_started)
		dfsIteration();

	if (aStar_started)
		a_starIteration();

	glutPostRedisplay();// calls indirectly to display
}

//menu for function choosing
void menu(int choice)
{

	switch (choice)
	{
	case 1:
		init();
		break;
	case 2:
		cleanUpMaze();
		break;
	case 3:
		bfs_start_started = true;
		break;
	case 4:
		dfs_started = true;
		break;
	case 5:
		bfs_start_started = true;
		bfs_target_started = true;
		break;
	case 6:
		aStar_started = true;
		break;

	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Search Algorithms ");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // idle: when nothing happens
	init();
	//create menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Scrumble the maze", 1);
	glutAddMenuEntry("Clean up the maze", 2);
	glutAddMenuEntry("BFS", 3);
	glutAddMenuEntry("DFS", 4);
	glutAddMenuEntry("Bidirectional BFS", 5);
	glutAddMenuEntry("A*", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
}