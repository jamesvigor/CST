#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <gtk/gtk.h>

#define CV_FAILED_LOAD 100
#define CST_NO_CAMERA_PRESENT 101
#define CST_NO_REFERENCE_IMAGE 102
#define CST_NO_DEFORMED_IMAGE 103
#define CST_NO_AREA_SET 104

#define REFERENCE_IMAGE 0
#define DEFORMED_IMAGE 1

int countReferenceImages, countDeformedImages; // Storage for count of reference images and deformed images
int numthreads;
GdkRectangle strainArea;


#endif

