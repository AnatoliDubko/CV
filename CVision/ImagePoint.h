#pragma once
class ImagePoint
{
	int _x, _y;
	double _value;
public:
	int getX() const { return _x; }
	int getY() const { return _y; }
	double getValue() const { return _value; }
	ImagePoint(int x, int y) : _x(x), _y(y), _value(0) {}
	ImagePoint(int x, int y, double value) : _x(x), _y(y), _value(value) {}
	ImagePoint(const ImagePoint& point) : _x(point._x), _y(point._y), _value(point._value) {}
	ImagePoint(ImagePoint&& point) = default;
	const ImagePoint &operator=(const ImagePoint &point);
};