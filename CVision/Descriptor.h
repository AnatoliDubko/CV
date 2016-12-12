#ifndef COMPUTERVISION_DESCRIPTOR_H
#define COMPUTERVISION_DESCRIPTOR_H

#include <memory>
#include "ConstantValues.h"

class Descriptor {
	std::unique_ptr<double[]> _data;
	int _x, _y;
	double _angle;
	int _size;
	int _orientationsCount;
	int _dataSize;

public:
	Descriptor(const int orientationsCount);
	Descriptor(const int x, const int y, const int size = DEFAULT_DESCRIPTOR_SIZE, const int orientationsCount = DEFAULT_DESCRIPTOR_ORIENTATIONS_COUNT);
	Descriptor(const int x, const int y, const double angle, const int size = DEFAULT_DESCRIPTOR_SIZE, const int orientationsCount = DEFAULT_DESCRIPTOR_ORIENTATIONS_COUNT);
	Descriptor(Descriptor&& other) noexcept 
		: _x(other._x), _y(other._y), _angle(other._angle), _size(other._size), _orientationsCount(other._orientationsCount), _dataSize(other._dataSize) {
		_data = move(other._data);
	}

	int getSize() {
		return _size;
	}

	int getX() const {
		return _x;
	}

	void setX(int x) {
		_x = x;
	}

	int getY() const {
		return _y;
	}

	void setY(int y) {
		_y = y;
	}

	double* begin() const
	{
		return &_data[0];
	}

	double* end() const
	{
		return &_data[0] + _dataSize;
	}

	void addValueOnAngleWithIndex(const int i, const int j, const double angle, const double value) const;
	void addValueOnAngle(const double angle, const double value) const;
	double distanceToDescriptor(const Descriptor &descriptor) const;
	void normalize();
	std::vector<double> maxOrientationInterpolatedAngles() const;
};

#endif