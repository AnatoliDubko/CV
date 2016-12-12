#ifndef COMPUTERVISION_KERNELSFACTORY_H
#define COMPUTERVISION_KERNELSFACTORY_H

#include <memory>

class Image;

enum class GaussKernelType { FULL, ROW, COLUMN };

class KernelsFactory {
	static const std::unique_ptr<double[]> _sobelGradXData;
	static const std::unique_ptr<double[]> _sobelGradYData;
	static Image gaussKernel(const int size, const double sigma);
	static Image gaussRow(const int size, const double sigma);
	static Image gaussColumn(const int size, const double sigma);

public:
	static Image sobelGradientXKernel();
	static Image sobelGradientYKernel();
	static Image gaussKernel(const int r, GaussKernelType gaussKernelType);
};
#endif