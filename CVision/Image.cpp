#include "KernelsFactory.h"
#include "ImageHelper.h"
#include "Image.h"
#include <QPainter>
#include <QPen>
#include <qmath.h>
#include "ConstantValues.h"

Image::Image() {
}

Image::Image(const int height, const int width) : _height(height),
_width(width),
_dataSize(height * width),
_data(std::make_unique<double[]>(_dataSize))
{
}

Image::Image(const int height, const int width, const double *data) : _height(height),
_width(width),
_dataSize(height * width),
_data(std::make_unique<double[]>(size_t(_dataSize))) {
	for (auto i = 0; i < _dataSize; ++i) {
		_data[i] = data[i];
	}
}

Image::Image(const Image &Image) : _height(Image._height),
_width(Image._width),
_dataSize(_height * _width),
_data(std::make_unique<double[]>(size_t(_dataSize))) {
	for (auto i = 0; i < _dataSize; ++i)
		_data[i] = Image._data[i];
}

Image Image::getCopy() const {
	return Image(*this);
}

void Image::normalize() {
	const auto minValue = *std::min_element(begin(), end());
	const auto maxValue = *std::max_element(begin(), end());
	auto range = maxValue - minValue;
	range = range == 0 ? 1 : range;
	forEach([=](auto &value) { value = (value - minValue) / range; });
}

Image Image::getNormalized() const {
	auto result = getCopy();
	result.normalize();
	return result;
}

Image Image::fromQImage(const QImage &image, const GrayScaleMod &grayScaleMod) {
	auto result = Image(image.height(), image.width());
	for (auto i = 0; i < result.getHeight(); ++i) {
		for (auto j = 0; j < result.getWidth(); ++j) {
			const auto  color = image.pixel(j, i);
			const auto  red = qRed(color),
				green = qGreen(color),
				blue = qBlue(color);
			switch (grayScaleMod)
			{
			case GrayScaleMod::PAL_NTSC:
				result.set(i, j, (.299 * red + .587 * green + .114 * blue) / 255);
				break;
			case GrayScaleMod::SRGB_HDTV:
				result.set(i, j, (.213 * red + .715 * green + .072 * blue) / 255);
				break;
			default:
				break;
			}
		}
	}
	return result;
}

QImage Image::toQImage() const {
	auto image = QImage(getWidth(), getHeight(), QImage::Format_RGB32);
	for (auto i = 0; i < getHeight(); ++i) {
		for (auto j = 0; j < getWidth(); ++j) {
			const auto color = int(get(i, j) * 255);
			image.setPixel(j, i, qRgb(color, color, color));
		}
	}
	return image;
}

double Image::getValue(int i, int j, BorderEffectType borderEffect) const {
	if (contains(i, j)) {
		return get(i, j);
	}
	switch (borderEffect) {
	case BorderEffectType::COPY:
		return get(std::max(0, std::min(getHeight() - 1, i)),
			std::max(0, std::min(getWidth() - 1, j)));
	case BorderEffectType::REFLECT:
		return get(i >= getHeight() ? 2 * (getHeight() - 1) - i : abs(i),
			j >= getWidth() ? 2 * (getWidth() - 1) - j : abs(j));
	case BorderEffectType::CYCLICAL:
		return get((i + getHeight()) % getHeight(),
			(j + getWidth()) % getWidth());
	default:
		return 0;
	}
}

Image Image::conv(const Image &kernel, const BorderEffectType borderEffect) const {
	auto result = Image(getHeight(), getWidth());
	for (auto i = 0; i < getHeight(); ++i) {
		for (auto j = 0; j < getWidth(); ++j) {
			auto value = .0;
			for (auto u = 0; u < kernel.getHeight(); ++u) {
				for (auto v = 0; v < kernel.getWidth(); ++v) {
					value += getValue(i + u - kernel.getHeight() / 2,
						j + v - kernel.getWidth() / 2,
						borderEffect) *
						kernel.get(u, v);
				}
			}
			result.set(i, j, value);
		}
	}
	return result;
}

Image Image::sobelX(const BorderEffectType borderEffect) const {
	return conv(KernelsFactory::sobelGradientXKernel(), borderEffect);
}

Image Image::sobelY(const BorderEffectType borderEffect) const {
	return conv(KernelsFactory::sobelGradientYKernel(), borderEffect);
}

Image Image::sobel(const BorderEffectType borderEffect) const {
	return ImageHelper::hypo(sobelX(borderEffect), sobelY(borderEffect));
}

Image Image::gauss(const double sigma, const BorderEffectType borderEffect) const {
	auto r = int((sigma + 0.5) * 3);
	const auto maxR = std::min(getHeight(), getWidth()) / 2;
	r = std::max(std::min(r, maxR), 1);
	return conv(KernelsFactory::gaussKernel(r, GaussKernelType::ROW), borderEffect)
		.conv(KernelsFactory::gaussKernel(r, GaussKernelType::COLUMN), borderEffect);
}

void Image::resize(const int height, const int width)
{
	if (height * width > getDataSize()) {
		_dataSize = height * width;
		_data = std::make_unique<double[]>(getDataSize());
	}
	this->_height = height;
	this->_width = width;
}

Image Image::getResized(const int height, const int width) const
{
	auto result = getCopy();
	result.resize(height, width);
	return result;
}

Image Image::downSample() const
{
	auto result = getResized(getHeight() / 2, getWidth() / 2);
	for (auto i = 0; i < result.getHeight(); ++i) {
		for (auto j = 0; j < result.getWidth(); ++j) {
			result.set(i, j, 
				(get(i * 2, j * 2) + get(i * 2 + 1, j * 2) + get(i * 2, j * 2 + 1) + get(i * 2 + 1, j * 2 + 1)) / 4);
		}
	}
	return result;
}

Image& Image::operator=(const Image &Image) {
	_height = Image._height;
	_width = Image._width;
	_dataSize = getHeight() * getWidth();
	_data = std::make_unique<double[]>(size_t(getDataSize()));
	for (auto i = 0; i < getDataSize(); ++i) {
		_data[i] = Image._data[i];
	}
	return *this;
}

Image Image::moravec(const int shift, const BorderEffectType borderEffect) const {
	auto result = Image(getHeight(), getWidth());
	for (auto i = 0; i < getHeight(); ++i) {
		for (auto j = 0; j < getWidth(); ++j) {
			auto minValue = std::numeric_limits<double>::max();
			for (auto u = -1; u <= 1; ++u) {
				for (auto v = -1; v <= 1; ++v) {
					if (u == 0 && v == 0) {
						continue;
					}
					auto value = .0;
					for (auto a = -shift; a <= shift; ++a) {
						for (auto b = -shift; b <= shift; ++b) {
							value +=
								pow(getValue(i + u + a, j + v + b, borderEffect) - getValue(i + a, j + b, borderEffect), 2);
						}
					}
					minValue = std::min(minValue, value);
				}
			}
			result.set(i, j, minValue);
		}
	}
	return result;
}

Image Image::harris(const double &sigma, const BorderEffectType borderEffect) const {
	auto result = Image(getHeight(), getWidth());
	const auto gradX = conv(KernelsFactory::sobelGradientXKernel(), borderEffect);
	const auto gradY = conv(KernelsFactory::sobelGradientYKernel(), borderEffect);
	const auto A = ImageHelper::scalarMultiply(gradX, gradX).gauss(sigma, borderEffect);
	const auto B = ImageHelper::scalarMultiply(gradX, gradY).gauss(sigma, borderEffect);
	const auto C = ImageHelper::scalarMultiply(gradY, gradY).gauss(sigma, borderEffect);
	for (auto i = 0; i < getHeight(); ++i) {
		for (auto j = 0; j < getWidth(); ++j) {
			const auto a = A.get(i, j);
			const auto b = B.get(i, j);
			const auto c = C.get(i, j);
			const auto d = sqrt((a - c) * (a - c) + 4 * b * b);
			result.set(i, j, std::min(fabs((a + c - d) / 2), fabs((a + c + d) / 2)));
		}
	}
	return result;
}

void Image::saveAsImage(QString filename) const {
	toQImage().save(filename);
}

Image Image::operator-(const Image &Image) {
	return ImageHelper::zip(*this, Image, [](auto x, auto y) { return x - y; });
}

std::vector<ImagePoint> Image::getLocalMaximums(const int shift, const double treshold, const BorderEffectType borderType) const {
	auto result = std::vector<ImagePoint>();
	for (auto i = 0; i < getHeight(); ++i) {
		for (auto j = 0; j < getWidth(); ++j) {
			if (get(i, j) < treshold) {
				continue;
			}
			auto isMaximum = true;
			for (auto di = -shift; di <= shift; ++di) {
				for (auto dj = -shift; dj <= shift; ++dj) {
					if(di ==0 && dj == 0) {
						continue;
					}
					if (getValue(i + di, j + dj, borderType) >= get(i, j)) {
						isMaximum = false;
					}
				}
			}
			if(!isMaximum) {
				continue;
			}
			result.emplace_back(i, j, get(i, j));
		}
	}
	return result;
}

QImage Image::toQImageWithPoints(const std::vector<ImagePoint>& points) const
{
	auto image = toQImage();
	QPainter painter(&image);
	auto pen = QPen(Qt::red);
	pen.setWidth(3);
	painter.setPen(pen);
	for (auto i = 0; i < points.size(); ++i) {
		painter.drawPoint(points[i].getY(), points[i].getX());
	}
	return image;
}

std::vector<ImagePoint> Image::nonMaxSuppression(const std::vector<ImagePoint>& points, const int limitCount, const double filterValue) const
{
	auto result = std::vector<ImagePoint>(points);
	auto maxRadius = int(sqrt(pow(getHeight(), 2) + pow(getWidth(), 2)));
	for (auto r = 0; result.size() > limitCount && r < maxRadius; ++r) {
		for (auto i = 0; i < result.size() && result.size() > limitCount; ++i) {
			for (auto j = 0; j < result.size() && result.size() > limitCount; ++j) {
				if (sqrt(pow(result[i].getX() - result[j].getX(), 2) + pow(result[i].getY() - result[j].getY(), 2)) <= r
					&& filterValue * result[i].getValue() > result[j].getValue()) {
					result.erase(result.begin() + j);
					i--;
					break;
				}
			}
		}
	}
	return result;
}

std::vector<Descriptor> Image::getDescriptors(const std::vector<ImagePoint>& points, const int gaussKernelRadius, const BorderEffectType borderEffect) const {
	const auto gradX = conv(KernelsFactory::sobelGradientXKernel(), borderEffect);
	const auto gradY = conv(KernelsFactory::sobelGradientYKernel(), borderEffect);
	const auto kernel = KernelsFactory::gaussKernel(gaussKernelRadius, GaussKernelType::FULL);
	auto descriptors = std::vector<Descriptor>();
	for (auto point : points) {
		Descriptor descriptor(point.getX(), point.getY());
		const auto netStep = int(ceil(kernel.getWidth() / double(descriptor.getSize())));
		for (auto i = descriptor.getX() - gaussKernelRadius, kernel_i = 0; i < descriptor.getX() + gaussKernelRadius; ++i, ++kernel_i) {
			for (auto j = descriptor.getY() - gaussKernelRadius, kernel_j = 0; j < descriptor.getY() + gaussKernelRadius; ++j, ++kernel_j) {
				const auto  dx = gradX.getValue(i, j, borderEffect),
							dy = gradY.getValue(i, j, borderEffect);
				const auto gradAngle = ImageHelper::getNormalizedAngle(atan2(dy, dx));
				const auto gradLength = sqrt(dx * dx + dy * dy) * kernel.get(kernel_i, kernel_j);
				descriptor.addValueOnAngleWithIndex(kernel_i / netStep, kernel_j / netStep, gradAngle, gradLength);
			}
		}
		descriptor.normalize();
		descriptors.emplace_back(std::move(descriptor));
	}
	return descriptors;
}

std::vector<Descriptor> Image::getDescriptorsRotateInvariant(const std::vector<ImagePoint>& points, const int gaussKernelRadius, const BorderEffectType borderEffect) const
{
	const auto gradX = conv(KernelsFactory::sobelGradientXKernel(), borderEffect);
	const auto gradY = conv(KernelsFactory::sobelGradientYKernel(), borderEffect);
	const auto extraGaussKernelRadius = gaussKernelRadius * 2;
	const auto extraKernel = KernelsFactory::gaussKernel(extraGaussKernelRadius, GaussKernelType::FULL);
	auto descriptors = std::vector<Descriptor>();
	for (auto point : points) {
		auto angles = getPointMaxGradientAngles(point, extraGaussKernelRadius, gradX, gradY, extraKernel, borderEffect);
		for (auto angle : angles) {
			Descriptor descriptor(point.getX(), point.getY(), angle);
			const auto netStep = int(ceil(gaussKernelRadius * 2 / double(descriptor.getSize())));
			auto cosAngle = cos(angle);
			auto sinAngle = sin(angle);
			for (auto i = descriptor.getX() - extraGaussKernelRadius, kernel_i = 0; i < descriptor.getX() + extraGaussKernelRadius; ++i, ++kernel_i)
				for (auto j = descriptor.getY() - extraGaussKernelRadius, kernel_j = 0; j < descriptor.getY() + extraGaussKernelRadius; ++j, ++kernel_j) {
					const auto dx = gradX.getValue(i, j, borderEffect);
					const auto dy = gradY.getValue(i, j, borderEffect);
					const auto gradAngleToAdd = ImageHelper::getNormalizedAngle(atan2(dy, dx) - angle);
					const auto gradLength = sqrt(dx * dx + dy * dy) * extraKernel.get(kernel_i, kernel_j);
					auto windowRotatedX = int(round((i - descriptor.getX()) * cosAngle - (j - descriptor.getY()) * sinAngle));
					auto windowRotatedY = int(round((i - descriptor.getX()) * sinAngle + (j - descriptor.getY()) * cosAngle));
					if (windowRotatedX < -extraGaussKernelRadius || windowRotatedX > -1 || windowRotatedY < -extraGaussKernelRadius || windowRotatedY > -1) {
						continue;
					}
					descriptor.addValueOnAngleWithIndex((windowRotatedX + extraGaussKernelRadius) / netStep, (windowRotatedY + extraGaussKernelRadius) / netStep, gradAngleToAdd, gradLength);
				}
			descriptor.normalize();
			descriptors.emplace_back(std::move(descriptor));
		}
	}
	return descriptors;
}

std::vector<double> Image::getPointMaxGradientAngles(const ImagePoint& point, const int gaussKernelRadius, const Image& gradX, const Image& gradY, const Image& gaussKernel, const BorderEffectType borderEffect) const
{
	Descriptor largeDescriptor(BIN_ROTATION_IVARIANT_ORIENTATIONS_COUNT);
	auto angles = std::vector<double>();
	for (auto i = point.getX() - gaussKernelRadius, kernel_i = 0; i < point.getX() + gaussKernelRadius; ++i, ++kernel_i) {
		for (auto j = point.getY() - gaussKernelRadius, kernel_j = 0; j < point.getY() + gaussKernelRadius; ++j, ++kernel_j) {
			const auto dx = gradX.getValue(i, j, borderEffect);
			const auto dy = gradY.getValue(i, j, borderEffect);
			const auto angle = ImageHelper::getNormalizedAngle(atan2(dy, dx));
			const auto gradLength = sqrt(dx * dx + dy * dy) * gaussKernel.get(kernel_i, kernel_j);
			largeDescriptor.addValueOnAngle(angle, gradLength);
		}
	}
	return largeDescriptor.maxOrientationInterpolatedAngles();
}