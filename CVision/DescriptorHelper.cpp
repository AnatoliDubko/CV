#include "DescriptorHelper.h"
#include "Descriptor.h"
#include <QPainter>

void DescriptorHelper::drawDescriptors(QPainter &painter, const std::vector<Descriptor> &descriptors, const std::vector<Descriptor> &descriptorsOfModified, const int &imageWidth, const double &minDistanceTreshold) {
	for (auto i = 0; i < descriptors.size(); ++i) {
		auto minDist = std::numeric_limits<double>::max();
		auto answer = 0;
		for (auto j = 0; j < descriptorsOfModified.size(); ++j) {
			if (minDist > descriptors[i].distanceToDescriptor(descriptorsOfModified[j])) {
				minDist = descriptors[i].distanceToDescriptor(descriptorsOfModified[j]);
				answer = j;
			}
		}
		if (minDist > minDistanceTreshold) {
			continue;
		}
		painter.setPen(QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256));
		painter.drawLine(descriptors[i].getY(),
			descriptors[i].getX(),
			descriptorsOfModified[answer].getY() + imageWidth,
			descriptorsOfModified[answer].getX());
	}
}