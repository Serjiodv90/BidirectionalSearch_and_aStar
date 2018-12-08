#include "Point2D.h"



Point2D::Point2D(int x, int y)
{
	this->x = x;
	this->y = y;
}


Point2D::~Point2D()
{
}

int Point2D::getX()
{
	return this->x;
}

int Point2D::getY()
{
	return this->y;
}
