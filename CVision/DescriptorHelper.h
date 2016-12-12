#ifndef COMPUTERVISION_DESCRIPTORHELPER_H
#define COMPUTERVISION_DESCRIPTORHELPER_H

#include <vector>

class QPainter;
class Descriptor;

class DescriptorHelper
{
public:
	static void drawDescriptors(QPainter & painter, const std::vector<Descriptor>& descriptors, const std::vector<Descriptor>& descriptorsOfModified, const int & imageWidth, const double & minDistanceTreshold);
};

#endif