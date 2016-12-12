#include <vector>
#include "Image.h"
#include "ImageHelper.h"
#include "ConstantValues.h"

class Descriptor;
class ImagePoint;

class DescriptorTaskBase
{
public:
	virtual std::vector<Descriptor> getDescriptors(const Image &image, const std::vector<ImagePoint> &interestingPoints) = 0;
};

class DescriptorTaskBasic : public DescriptorTaskBase {
	virtual std::vector<Descriptor> getDescriptors(const Image &image, const std::vector<ImagePoint> &interestingPoints)
	{
		return image.getDescriptors(interestingPoints, GAUSS_KERNEL_RADIUS);
	}
};

class DescriptorTaskRotateInvariant : public DescriptorTaskBase {
	virtual std::vector<Descriptor> getDescriptors(const Image &image, const std::vector<ImagePoint> &interestingPoints)
	{
		return image.getDescriptorsRotateInvariant(interestingPoints, GAUSS_KERNEL_RADIUS);
	}
};