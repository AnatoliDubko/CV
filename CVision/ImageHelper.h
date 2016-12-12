#ifndef COMPUTERVISION_IMAGEHELPER_H
#define COMPUTERVISION_IMAGEHELPER_H

#include <functional>

class Image;
class Point;
class ImagePoint;

class ImageHelper
{
	static double normalizeAngle(const double alpha);
	static bool isAngleNormalized(const double alpha);
public:
	static Image scalarMultiply(const Image& a, const Image& b);
	static Image sqrSum(const Image& a, const Image& b);
	static bool sameSize(const Image &a, const Image &b);
	static Image scalarDiv(const Image& img, const double divider);
	static Image hypo(const Image &a, const Image &b);
	static Image zip(const Image &a, const Image &b, std::function<double(double, double)> f);
	static double parabolicInterpolation(const double yLeft, const double y, const double yRight) {
		return -(yRight - yLeft) / (2 * (yLeft + yRight - 2 * y));
	}
	static double getInterpolatedAngle(double bin[], const int orientationsCount, const int orientation);
	static double getNormalizedAngle(const double alpha);
};
#endif