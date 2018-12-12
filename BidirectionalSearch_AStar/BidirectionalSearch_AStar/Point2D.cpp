#include "Point2D.h"



Point2D::Point2D(int x, int y)
{
	this->x = x;
	this->y = y;
	this->g = 0;
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

void Point2D::calcDistanceFromTarget(Point2D * targetPoint)
{
	double xPow = pow(this->x - targetPoint->x, 2);
	double yPow = pow(this->y - targetPoint->y, 2);
	this->h = sqrtf(xPow + yPow);
}

void Point2D::set_g(int levelOfPointInTree)
{
	this->g = levelOfPointInTree;
}

void Point2D::set_f(Point2D* targetPoint, int levelOfPointInTree)
{
	set_g(levelOfPointInTree);
	calcDistanceFromTarget(targetPoint);
	this->f = this->g + this->h;
}

double Point2D::get_f()
{
	return this->f;
}
