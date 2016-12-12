#ifndef COMPUTERVISION_CONSTANTVALUES_H
#define COMPUTERVISION_CONSTANTVALUES_H

#include <QString>

//#source files and paths
const QString SOURCE = "source_images/image.jpg";
const QString SOURCE_MODIFIED_ROTATION = "source_images/image_modified_rotation.jpg";
const QString SOURCE_MODIFIED_BASIC = "source_images/image_modified_basic.jpg";
const QString RESULT_SOBEL = "result_sobel/sobel.jpg";
const QString RESULT_PYRAMID_FOLDER = "result_pyramid";
const QString RESULT_INTERESTING_FOLDER = "result_interesting";
const QString RESULT_DESCRIPTORS_BASIC = "result_descriptors_basic/descriptors.jpg";
const QString RESULT_DESCRIPTORS_ROTATE_INVARIANT = "result_descriptors_rotate_invariant/descriptors.jpg";


//#2
const auto SCALES_PER_OCTAVE = 8;
const auto BASE_SIGMA = .5;
const auto SIGMA = 1.6;

//#3
const auto POINTS_LIMIT = 500;
const auto MORAVEC_SHIFT = 2;
const auto HARRIS_SIGMA = 1;
const auto LOCAL_MAXIMUMS_SHIFT = 2;
const auto LOCAL_MAXIMUMS_TRESHOLD = .01;
const auto NONMAX_FILTER_VALUE = .9;

//#4 #5
const auto BIN_EPSILON = 1e-9;
const auto GAUSS_KERNEL_RADIUS = 8;
const auto BIN_ROTATION_IVARIANT_ORIENTATIONS_COUNT = 36;
const auto SECOND_MAIN_ORIENTATION_TRESHOLD = .8;
const auto MINDISTANCE_TRESHOLD = .3;
const auto DEFAULT_DESCRIPTOR_SIZE = 4;
const auto DEFAULT_DESCRIPTOR_ORIENTATIONS_COUNT = 8;

#endif