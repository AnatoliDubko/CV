#include "Image.h"
#include "ScalePyramid.h"
#include <QPainter>
#include "DescriptorTask.h"
#include "ConstantValues.h"
#include "DescriptorHelper.h"

void sobel(const QString &source, const QString &resultPath) {
	Image::fromQImage(QImage(source))
		.sobel()
		.getNormalized()
		.saveAsImage(resultPath);
}

void scalePyramid(const QString &source, const QString &resultFolder) {
	Image::fromQImage(QImage(source))
		.buildScalePyramid(SCALES_PER_OCTAVE, BASE_SIGMA, SIGMA)
		.saveAsImageSet(resultFolder);
}

void interestingPoints(const QString &source, const QString &resultFolder)
{
	const auto image = Image::fromQImage(QImage(source));
	const auto harrisPoints = image.harris(HARRIS_SIGMA).getLocalMaximums(LOCAL_MAXIMUMS_SHIFT, LOCAL_MAXIMUMS_TRESHOLD);
	image.toQImageWithPoints(image.nonMaxSuppression(harrisPoints, POINTS_LIMIT, NONMAX_FILTER_VALUE)).save(resultFolder + "/harris.jpg");
}

void descriptors(const QString &source, 
	const QString &sourceModified, 
	const QString &resultPath, 
	DescriptorTaskBase &descriptorTask, 
	const double &minDistanceTreshold = std::numeric_limits<double>::max())
{
	const auto image = Image::fromQImage(QImage(source));
	const auto imageModified = Image::fromQImage(QImage(sourceModified));
	const auto harrisPoints = image.harris(HARRIS_SIGMA).getLocalMaximums(LOCAL_MAXIMUMS_SHIFT, LOCAL_MAXIMUMS_TRESHOLD);
	const auto interestingPoints = image.nonMaxSuppression(harrisPoints, POINTS_LIMIT, NONMAX_FILTER_VALUE);
	const auto harrisPointsOfModified = imageModified.harris(HARRIS_SIGMA).getLocalMaximums(LOCAL_MAXIMUMS_SHIFT, LOCAL_MAXIMUMS_TRESHOLD);
	const auto interestingPointsOfModified = imageModified.nonMaxSuppression(harrisPointsOfModified, POINTS_LIMIT, NONMAX_FILTER_VALUE);
	const auto descriptors = descriptorTask.getDescriptors(image, interestingPoints);
	const auto descriptorsOfModified = descriptorTask.getDescriptors(imageModified, interestingPointsOfModified);
	auto imageResult = image.toQImageWithPoints(interestingPoints);
	auto imageModifiedResult = imageModified.toQImageWithPoints(interestingPointsOfModified);
	QImage finalResult(image.getWidth() + imageModified.getWidth(), std::max(image.getHeight(), imageModified.getHeight()), QImage::Format_RGB32);
	QPainter painter(&finalResult);
	painter.fillRect(finalResult.rect(), QBrush(Qt::white));
	painter.drawImage(0, 0, imageResult);
	painter.drawImage(image.getWidth(), 0, imageModifiedResult);
	DescriptorHelper::drawDescriptors(painter, descriptors, descriptorsOfModified, image.getWidth(), minDistanceTreshold);
	finalResult.save(resultPath);
}

int main(int argc, char *argv[])
{
	////#1
	//sobel(SOURCE, RESULT_SOBEL);
	////#2
	//scalePyramid(SOURCE, RESULT_PYRAMID_FOLDER);
	//#3
	interestingPoints(SOURCE, RESULT_INTERESTING_FOLDER);
	////#4
	//descriptors(SOURCE, SOURCE_MODIFIED_BASIC, RESULT_DESCRIPTORS_BASIC, DescriptorTaskBasic());
	////#5
	//descriptors(SOURCE, SOURCE_MODIFIED_ROTATION, RESULT_DESCRIPTORS_ROTATE_INVARIANT, DescriptorTaskRotateInvariant(), MINDISTANCE_TRESHOLD);
	return 0;
}