#include "ScalePyramid.h"
#include "Image.h"
#include <QString>

ScalePyramid ScalePyramid::build(const Image& image, const int scalesPerOctaveCount, const double baseSigma, const double sigma) {
	Q_ASSERT(baseSigma <= sigma);
	const auto minImageSize = 32;
	const auto minDim = std::min(image.getHeight(), image.getWidth());
	const auto octavesCount = int(log2(minDim)) - int(log2(minImageSize)) + 1;
	auto result = ScalePyramid(scalesPerOctaveCount);
	const auto k = pow(2.0, 1.0 / result.scalesPerOctaveCount());
	auto curSigma = sigma;
	auto curImage = image.gauss(sqrt(sigma * sigma - baseSigma * baseSigma));
	for (auto i = 0; i < octavesCount; ++i) {
		auto octave = std::vector<std::pair<Image, double>>(result.scalesPerOctaveCount());
		for (auto j = 0; j < octave.size(); ++j) {
			octave[j].first = curImage;
			octave[j].second = curSigma;
			const auto newSigma = curSigma * k;
			const auto deltaSigma = sqrt(newSigma * newSigma - curSigma * curSigma);
			curImage = curImage.gauss(deltaSigma);
			curSigma = newSigma;
		}
		result.pushOctave(octave);
		curSigma = curSigma / 2;
		curImage = curImage.downSample();
	}
	return result;
}

double ScalePyramid::getSigma(const int octave, const int scale) const
{
	Q_ASSERT(contains(octave, scale));
	return _octaves[octave][scale].second;
}
void ScalePyramid::saveAsImageSet(const QString &resultFolder) const
{
	for (auto i = 0; i < octavesCount(); ++i) {
		for (auto j = 0; j < scalesPerOctaveCount(); ++j) {
			auto s = getSigma(i, j);
			auto str = "octave_"
				+ QString::number(i + 1)
				+ "___scale_"
				+ QString::number(j)
				+ "___sigma_"
				+ QString::number(s) + ".jpg";
			getScale(i, j).saveAsImage(resultFolder + "/" + str);
		}
	}
}

void ScalePyramid::pushOctave(const std::vector<std::pair<Image, double>>& octave)
{
	_octaves.push_back(octave);
}

Image ScalePyramid::getScale(const int octave, const int scale) const
{
	Q_ASSERT(contains(octave, scale));
	return _octaves[octave][scale].first;
}
