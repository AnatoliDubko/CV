#include "KernelsFactory.h"
#include "ImageHelper.h"
#include "Image.h"
#include <qmath.h>

const std::unique_ptr<double[]> KernelsFactory::_sobelGradXData =
std::unique_ptr<double[]>(
	new double[9]{
		-1, 0, 1,
		-2, 0, 2,
		-1, 0, 1 });

const std::unique_ptr<double[]> KernelsFactory::_sobelGradYData =
std::unique_ptr<double[]>(new double[9]{
		-1, -2, -1,
		0, 0, 0,
		1, 2, 1 });

Image KernelsFactory::sobelGradientXKernel() {
	return Image(3, 3, _sobelGradXData.get());
}

Image KernelsFactory::sobelGradientYKernel() {
	return Image(3, 3, _sobelGradYData.get());
}

Image KernelsFactory::gaussKernel(const int r, GaussKernelType gaussKernelType)
{
	auto size = r * 2;
	auto sigma = size / 3.;
	switch (gaussKernelType)
	{
	case GaussKernelType::ROW:
		return gaussRow(size, sigma);
	case GaussKernelType::COLUMN:
		return gaussColumn(size, sigma);
	default: 
		return gaussKernel(size, sigma);;
	}
}

Image KernelsFactory::gaussKernel(const int size, const double sigma) {
	auto kernel = Image(size, size);
	auto sum = 0.;
	for (auto i = 0; i < kernel.getHeight(); ++i) {
		for (auto j = 0; j < kernel.getWidth(); ++j) {
			auto x = size / 2. - i;
			auto y = size / 2. - j;
			auto a = exp(-(x * x + y * y) / (2 * sigma * sigma));
			kernel.set(i, j, a / (2 * M_PI * sigma * sigma));
			sum += a / (2 * M_PI * sigma * sigma);
		}
	}
	return ImageHelper::scalarDiv(kernel, sum);
}

Image KernelsFactory::gaussRow(const int size, const double sigma) {
	auto kernel = Image(1, size);
	auto sum = .0;
	for (auto i = 0; i < kernel.getWidth(); ++i) {
		auto y = size / 2. - i;
		auto a = exp(-(y * y) / (2 * sigma * sigma));
		kernel.set(0, i, a / (2 * M_PI * sigma * sigma));
		sum += a / (2 * M_PI * sigma * sigma);
	}
	return ImageHelper::scalarDiv(kernel, sum);
}

Image KernelsFactory::gaussColumn(const int size, const double sigma) {
	auto kernel = Image(size, 1);
	auto sum = .0;
	for (auto i = 0; i < kernel.getHeight(); ++i) {
		auto x = size / 2. - i;
		auto a = exp(-(x * x) / (2 * sigma * sigma));
		kernel.set(i, 0, a / (2 * M_PI * sigma * sigma));
		sum += a / (2 * M_PI * sigma * sigma);
	}
	return ImageHelper::scalarDiv(kernel, sum);
}