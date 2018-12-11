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

bool Point2D::operator==(const Point2D & point) const
{
	return this->x == point.x && this->y == point.y;
}
