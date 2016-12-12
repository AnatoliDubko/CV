#ifndef COMPUTERVISION_IMAGE_H
#define COMPUTERVISION_IMAGE_H

#include "ImagePoint.h"
#include "ScalePyramid.h"
#include <memory>
#include <algorithm>
#include <qglobal.h>
#include <vector>
#include "Descriptor.h"

class QImage;
class QString;
class ScalePyramid;

enum class GrayScaleMod { PAL_NTSC, SRGB_HDTV };
enum class BorderEffectType { ZERO, COPY, REFLECT, CYCLICAL };

class Image {
	int _height = 0,
		_width = 0,
		_dataSize = 0;

	std::unique_ptr<double[]> _data = nullptr;

	double *begin() const
	{
		return &_data[0];
	}

	double *end() const
	{
		return &_data[0] + getHeight() * getWidth();
	}

	void normalize();
	void resize(const int rowSize, const int columnSize);
	std::vector<double> getPointMaxGradientAngles(const ImagePoint& point, const int gaussKernelRadius, const Image& gradX, const Image& gradY, const Image& gaussKernel, const BorderEffectType borderEffect = BorderEffectType::COPY) const;

public:
	bool contains(const int &i, const int &j) const {
		return i >= 0 && i < getHeight() && j >= 0 && j < getWidth();
	}

	double get(const int i, const int j) const {
		Q_ASSERT(contains(i, j));
		return _data[i * getWidth() + j];
	}

	void set(const int i, const int j, const double value) const
	{
		Q_ASSERT(contains(i, j));
		_data[i * getWidth() + j] = value;
	}

	template<typename Func>
	void forEach(Func f) {
		std::for_each(begin(), end(), f);
	}

	template<typename Func>
	void enumerate(Func delegate) {
		for (auto i = 0; i < getHeight() * getWidth(); ++i)
			delegate(i, _data[i]);
	}

	ScalePyramid Image::buildScalePyramid(const int scalesPerOctave, const double baseSigma, const double sigma) const
	{
		return ScalePyramid::build(*this, scalesPerOctave, baseSigma, sigma);
	}

	Image();
	Image(const int height, const int width);
	Image(const int height, const int width, const double *data);
	Image(const Image &matrix);
	Image(Image &&matrix) = default;

	int getHeight() const { return _height; }
	int getWidth() const { return _width; }
	int getDataSize() const { return _dataSize; }
	int getDataValue(const int i) const { return _data[i]; }
	double getValue(int i, int j, BorderEffectType typeBorder = BorderEffectType::COPY) const;

	Image getCopy() const;
	Image getNormalized() const;
	Image getResized(const int height, const int width) const;

	Image conv(const Image& kernel, const BorderEffectType typeBorder = BorderEffectType::COPY) const;

	Image &operator=(const Image &matrix);
	Image operator-(const Image &matrix);

	static Image fromQImage(const QImage &image, const GrayScaleMod &grayScaleMod = GrayScaleMod::SRGB_HDTV);
	QImage toQImage() const;
	QImage toQImageWithPoints(const std::vector<ImagePoint>& points) const;
	void saveAsImage(QString filename) const;

	Image sobelX(const BorderEffectType borderEffect = BorderEffectType::COPY) const;
	Image sobelY(const BorderEffectType borderEffect = BorderEffectType::COPY) const;
	Image sobel(const BorderEffectType borderEffect = BorderEffectType::COPY) const;
	
	Image gauss(const double sigma, const BorderEffectType borderEffect = BorderEffectType::COPY) const;
	
	Image moravec(const int shift, const BorderEffectType borderEffect = BorderEffectType::COPY) const;
	Image harris(const double &sigma, const BorderEffectType borderEffect = BorderEffectType::COPY) const;

	Image downSample() const;

	std::vector<ImagePoint> getLocalMaximums(const int shift, const double treshold, const BorderEffectType border = BorderEffectType::COPY) const;
	std::vector<ImagePoint> nonMaxSuppression(const std::vector<ImagePoint>& points, const int limitCount, const double filterValue) const;

	std::vector<Descriptor> getDescriptors(const std::vector<ImagePoint>& points, const int gaussKernelRadius, const BorderEffectType borderEffect = BorderEffectType::COPY) const;
	std::vector<Descriptor> getDescriptorsRotateInvariant(const std::vector<ImagePoint> &points, const int gaussKernelRadius, const BorderEffectType borderEffect = BorderEffectType::COPY) const;
};

#endif
