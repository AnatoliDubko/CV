#include "ImageHelper.h"
#include "ConstantValues.h"
#include "Image.h"
#include <qglobal.h>
#include <qmath.h>

Image ImageHelper::zip(const Image& a, const Image& b, std::function<double(double, double)> f) {
	Q_ASSERT(sameSize(a, b));
	auto result = Image(a.getHeight(), a.getWidth());
	result.enumerate([=](int i, double& x) { x = f(a.getDataValue(i), b.getDataValue(i)); });
	return result;
}

Image ImageHelper::scalarMultiply(const Image& a, const Image& b)
{
	return zip(a, b, [](double x, double y) { return x * y; });
}

Image ImageHelper::sqrSum(const Image& a, const Image& b)
{
	return zip(a, b, [](double x, double y) { return x * x + y * y; });
}

bool ImageHelper::sameSize(const Image& a, const Image& b)
{
	return a.getHeight() == b.getHeight() && a.getWidth() == b.getWidth();
}

Image ImageHelper::scalarDiv(const Image& img, const double divider)
{
	auto result = img.getCopy();
	result.forEach([=](auto &value) { value /= divider; });
	return result;
}

Image ImageHelper::hypo(const Image &a, const Image &b) {
	Q_ASSERT(sameSize(a, b));
	const auto height = a.getHeight();
	const auto width = a.getWidth();
	auto result = Image(height, width);
	for (auto i = 0; i < height; ++i) {
		for (auto j = 0; j < width; ++j) {
			auto value = sqrt(a.get(i, j) * a.get(i, j) + b.get(i, j) * b.get(i, j));
			result.set(i, j, value);
		}
	}
	return result;
}

double ImageHelper::getInterpolatedAngle(double bin[], const int orientationsCount, const int orientation)
{
	const auto yLeft = bin[(orientationsCount + orientation - 1) % orientationsCount];
	const auto y = bin[orientation];
	const auto yRight = bin[(orientation + 1) % orientationsCount];
	const auto binStep = 2 * M_PI / orientationsCount;
	const auto alpha = (parabolicInterpolation(yLeft, y, yRight) + orientation) * binStep;
	return normalizeAngle(alpha);
}

double ImageHelper::getNormalizedAngle(const double alpha)
{
	if(isAngleNormalized(alpha)) {
		return alpha;
	}
	return getNormalizedAngle(normalizeAngle(alpha));
}

double ImageHelper::normalizeAngle(const double alpha)
{
	return abs(alpha) <= BIN_EPSILON
	? 0
	: alpha < 0
		? alpha + 2 * M_PI
		: alpha >= 2 * M_PI
			? alpha - 2 * M_PI
			: alpha;
}

bool ImageHelper::isAngleNormalized(const double alpha)
{
	return alpha < 2 * M_PI && alpha >= 0 || alpha < 2 * M_PI && alpha > BIN_EPSILON;
}
