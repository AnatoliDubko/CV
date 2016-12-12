#include "ImagePoint.h"

const ImagePoint& ImagePoint::operator=(const ImagePoint &point)
{
	_x = point._x;
	_y = point._y;
	_value = point._value;
	return *this;
}
