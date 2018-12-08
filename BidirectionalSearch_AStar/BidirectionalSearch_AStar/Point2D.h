#ifndef __POINT2D_H
#define __POINT2D_H


class Point2D
{
public:
	Point2D(int x, int y);
	~Point2D();

	int getX();
	int getY();

private:
	int x;
	int y;
};

#endif // !__POINT2D_H