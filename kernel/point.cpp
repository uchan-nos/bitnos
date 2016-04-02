/*
 * point.cpp
 *
 *  Created on: 2009/07/30
 *      Author: uchan
 */

#include <bitnos/point.h>

Point::Point() :
    X(0), Y(0)
{}

Point::Point(int x, int y) :
    X(x), Y(y)
{}

Point::Point(const Point& point) :
    X(point.X), Y(point.Y)
{}

Point Point::operator+(const Point& rhs) const
{
    Point result(this->X + rhs.X, this->Y + rhs.Y);
    return result;
}

Point& Point::operator+=(const Point& rhs)
{
    this->X += rhs.X;
    this->Y += rhs.Y;
    return *this;
}

Point Point::operator-(const Point& rhs) const
{
    Point result(this->X - rhs.X, this->Y - rhs.Y);
    return result;
}

Point& Point::operator-=(const Point& rhs)
{
    this->X -= rhs.X;
    this->Y -= rhs.Y;
    return *this;
}

Point& Point::operator=(const Point& rhs)
{
    this->X = rhs.X;
    this->Y = rhs.Y;
    return *this;
}

bool Point::operator==(const Point& rhs) const
{
    if (X == rhs.X && Y == rhs.Y) {
        return true;
    } else {
        return false;
    }
}

