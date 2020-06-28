#ifndef _GUIELEMENTS_H_
#define _GUIELEMENTS_H_

#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>
#include <gtkimageview/gtkimagetooldragger.h>
#include <gtkimageview/gtkimagetoolselector.h>

// This file conatins global definitions for user interface elements, see individual items for descriptions.

    GtkWidget *mainWindow; // Main window widget for storage of main application interface
    GtkListStore *referenceImages, *deformedImages; // Reference/Deformed Images List Container
    GtkListStore *referencePoints;
    GtkTreeIter referencePointsIter;
    GtkTreeIter referenceImageIter, deformedImageIter; // Reference/Deformed Image Iterator
    GtkWidget *cmbSelectImage; // Image select combo box for the toolbar
    GtkWidget *stbMainStatusBar;
    GdkPixbuf *currentImage;
    GtkWidget *imageView;

    // From the image capture dialog
    GtkWidget *captureWindow; // Image capture window
    GtkWidget *cmbISO, *cmbShutterSpeed, *cmbAperture, *cmbWhiteBalance, *cmbExposureCompensation, *cmbMeteringMode, *cmbImageFormat;

    // From the strain selection window
    GtkWidget *strainselectWindow;
    GtkIImageTool *strainSelector;
    GtkIImageTool *exclusionSelector;
    GtkWidget *txtMinX;
    GtkWidget *txtMinY;
    GtkWidget *txtMaxX;
    GtkWidget *txtMaxY;
    GtkWidget *txtPhysWidth;
    GtkWidget *txtPhysHeight;

    // For the output configuration window
    GtkWidget *outputWindow;
    GtkWidget *cmbOutputFormat;
    GtkWidget *txtGraphTitle;
    GtkWidget *toggleAutoY;
    GtkWidget *yFrom;
    GtkWidget *yTo;
    GtkWidget *txtYAxisMin;
    GtkWidget *txtYAxisMax;
    GtkWidget *toggleAutoX;
    GtkWidget *xFrom;
    GtkWidget *xTo;
    GtkWidget *txtXAxisMin;
    GtkWidget *txtXAxisMax;
    GtkWidget *selOutputImage;
    GtkWidget *txtOutputImage;
    GtkWidget *selInputFile;
    GtkWidget *txtInputFile;
    GtkWidget *txtXAxisTitle;
    GtkWidget *txtYAxisTitle;
    GtkWidget *imgGraphView;
    GtkWidget *toggleReverseY;
    GtkWidget *toggleReverseX;
    GtkWidget *toggleGrid;
    GtkWidget *toggleKey;
    GtkWidget *toggleLine;
    GtkObject *widthAdjustment;
    GtkObject *heightAdjustment;
    GtkWidget *lblDimAdjustment;
    GtkWidget *lblx;
    GtkWidget *lblpx1;
    GtkWidget *lblpx2;
    GtkWidget *widthSpin;
    GtkWidget *heightSpin;

#endif

