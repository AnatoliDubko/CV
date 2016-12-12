#include <QtGui/QPainter>
#include "Descriptor.h"
#include "ImageHelper.h"
#include <qmath.h>
#include "ConstantValues.h"

Descriptor::Descriptor(const int orientationsCount) : Descriptor(0, 0, 1, orientationsCount) {}

Descriptor::Descriptor(const int x, const int y, const int size, const int orientationsCount) : Descriptor(x, y, 0, size, orientationsCount) {}

Descriptor::Descriptor(const int x, const int y, const double angle, const int size, const int orientationsCount)
	: _x(x), _y(y), _angle(angle), _size(size), _orientationsCount(orientationsCount), _dataSize(size * size * orientationsCount) {
	_data = std::make_unique<double[]>(size_t(_dataSize));
	std::for_each(begin(), end(), [&](double &val) { val = 0; });
}

double Descriptor::distanceToDescriptor(const Descriptor& descriptor) const
{
	Q_ASSERT(_size == descriptor._size && _orientationsCount == descriptor._orientationsCount);
	auto length = 0.;
	for (auto i = 0; i < _dataSize; ++i) {
		length += (_data[i] - descriptor._data[i]) * (_data[i] - descriptor._data[i]);
	}
	return sqrt(length);
}

void Descriptor::normalize() {
	auto length = 0.;
	for (auto i = 0; i < _dataSize; ++i) {
		length += _data[i] * _data[i];
	}
	length = sqrt(length);
	std::for_each(begin(), end(), [&](double &val) { val /= length; });
}

void Descriptor::addValueOnAngleWithIndex(const int i, const int j, const double angle, const double value) const {
	Q_ASSERT(i >= 0 && i < _size && j >= 0 && j < _size);
	const auto binStep = 2 * M_PI / _orientationsCount;
	const auto binFirst = int(angle / binStep);
	const auto binSecond =
		angle - binStep * binFirst >= binStep / 2
		? (binFirst + 1) % _orientationsCount
		: binFirst == 0
		? _orientationsCount - 1
		: binFirst - 1;

	const auto proportionalForFirst = (angle - binStep * binFirst) / binStep;
	const auto proportionalForSecond = 1 - proportionalForFirst;
	_data[i * _size * _orientationsCount + j * _orientationsCount + binFirst] += value * proportionalForFirst;
	_data[i * _size * _orientationsCount + j * _orientationsCount + binSecond] += value * proportionalForSecond;
}

void Descriptor::addValueOnAngle(const double angle, const double value) const {
	addValueOnAngleWithIndex(0, 0, angle, value);
}

std::vector<double> Descriptor::maxOrientationInterpolatedAngles() const
{
	std::vector<double> angles;
	const auto mainOrientation = int(std::max_element(begin(), end()) - begin());
	const auto maxElement = *std::max_element(begin(), end());
	angles.push_back(ImageHelper::getInterpolatedAngle(_data.get(), _orientationsCount, mainOrientation));
	_data[mainOrientation] = std::numeric_limits<double>::min();
	const auto secondOrientation = int(std::max_element(begin(), end()) - begin());
	_data[mainOrientation] = maxElement;
	if (_data[secondOrientation] >= _data[mainOrientation] * SECOND_MAIN_ORIENTATION_TRESHOLD) {
		angles.push_back(ImageHelper::getInterpolatedAngle(_data.get(), _orientationsCount, secondOrientation));
	}
	return angles;
}