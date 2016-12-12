#ifndef COMPUTERVISION_SCALEPYRAMID_H
#define COMPUTERVISION_SCALEPYRAMID_H

#include <vector>

class Image;
class QString;

class ScalePyramid {
	int _scalesPerOctave;
	std::vector<std::vector<std::pair<Image, double>>> _octaves;

public:
	explicit ScalePyramid(const int scalesPerOctave)
		: _scalesPerOctave(scalesPerOctave)
	{
	}

	static ScalePyramid build(const Image& image,
		const int scalesPerOctave,
		const double baseSigma,
		const double sigma);

	int octavesCount() const {
		return _octaves.size();
	}

	int scalesPerOctaveCount() const {
		return _scalesPerOctave;
	}

	bool contains(const int octave, const int scale) const
	{
		return octave >= 0 && octave < octavesCount() && scale >= 0 && scale < scalesPerOctaveCount();
	}

	double getSigma(const int octave, const int scale) const;
	void saveAsImageSet(const QString &resultFolder) const;
	void pushOctave(const std::vector<std::pair<Image, double>>& octave);
	Image getScale(const int octave, const int scale) const;
};
#endif 
