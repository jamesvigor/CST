
/*  FILE:       mapstrain.c
    AUTHOR:     James E Vigor
    PROJECT:    CST
    DATE:       13-6-2012
    DESCRIP:    Strain Mapping Functions for the CST application (Multithread Version)
    LICENSE:    GPL v2.0 (see below)
    COPYRIGHT:  JAMES EDWARD VIGOR 2011-2012

    CST, A Civil Engineering Strain Measurement Tool
    Copyright (C) 2012  James Edward Vigor

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

// User interface contents files, constants header (global data) and error checking
#include "guielements.h"
#include "constants.h"
#include "doerrors.h"

// OpenCV includes for the processing algorithms
#include "opencv/cv.h"
#include "opencv/highgui.h"

#define _OFFSET 5       // Map offset for sub pixel accuracy
#define _INTER_SIZE 10  // Interpolaton area size for sub-pixel accuracy (too high may cause memory errors!)

// GTK Items should remain within the scope of this c file, therefore declared as static
static GtkListStore *listMapReferenceImages,    *listMapDeformedImages;         // Reference and Deformed Image List Stores
static GtkTreeIter   iterMapReferenceImages,     iterMapDeformedImages;         // Iteration for these list stores
static GtkObject    *subsetResXAdjustment,  *subsetResYAdjustment;      // Adjustment objects for x and y step
static GtkObject    *subsetMapSizeAdjustment;                                   // Adjustment object for subset size

static GtkWidget *mapconfigWindow;                                                      // Map configuration window
static GtkWidget *chkMapSubPixelAccuracy;                                               // Sub-pixel accuracy check box
static GtkWidget *cmbMapAccuracyLevel, *cmbMapInterpMethod, *cmbMapMeasurementType;     // Combo boxes for option selection (see name)
static GtkWidget *subsetResXSpin,      *subsetResYSpin;                                // Spin items for x and y subset step
static GtkWidget *selMapOutputXFile,   *selMapOutputYFile;                              // Buttons for x and y output file selection
static GtkWidget *txtMapOutputXFile,   *txtMapOutputYFile;                              // Text output for x and y strain maps
static GtkWidget *lblMapAccuracyLevel, *lblMapInterpolationMethod;                      // Labels for interpolation options
static GtkWidget *lblOutputFileY,      *lblOutputFileX;                                 // Labels for output files
static GtkWidget *lblSubsetStepX,      *lblSubsetStepY, *lblPixel2, *lblPixel3;         // Labels for subset steps and pixels
static GtkWidget *prgbarMain;

// The following data should be accessible by all threads, and is therefore declared as static prior to the code
static int threadHeightAllocation, currentCycle;
static int subImageXStep, subImageYStep, subImageSize;
static int resultWidth, resultHeight;
static int pseudoBoolSubPix = FALSE;
static int interpolationfactor, interpmethod;
static char *referenceImagePath, *deformedImagePath;
pthread_mutex_t MUTEX;

// Enumeration for the column identifiers for the tree view widget(s)
enum
{
    CHECK_COLUMN,
    IMAGE_COLUMN
} ;

enum
{
    ORIG_X,
    ORIG_Y,
    DEFO_X,
    DEFO_Y
};

// Structure type for the deformation surface map
typedef struct
{
    int origXLoc;
    int origYLoc;
    double defoXLoc;
    double defoYLoc;
    double physXLoc;
    double physYLoc;
} deformedPoint;

// Generate a static deformationmap which may be written to by all threads.
// Size must be no greater than 5000 points by 5000 points (25,000,000 total)
static deformedPoint deformationMap[5000][5000];

// Function called to hide the map configuration window (window concerned with this file)
void hideMapConfigurationWindow()
{
    gtk_widget_hide(GTK_WIDGET(mapconfigWindow));
}

// Function called to show the map configuration window (window concerned with this file)
void showMapConfigurationWindow()
{
    // Before showing the window, clear the list of reference and deformed images
    gtk_list_store_clear(GTK_LIST_STORE(listMapReferenceImages));
    gtk_list_store_clear(GTK_LIST_STORE(listMapDeformedImages));

    // integer i declared for iteration
    int i = 0;

    // Append all reference and deformed images to the list store items for processing
    for(i = 0; i < countReferenceImages; i++)
    {
        char num[20];
        sprintf(num, "Reference Image %d", i);
        gtk_list_store_append(GTK_LIST_STORE(listMapReferenceImages), &iterMapReferenceImages);
        gtk_list_store_set(GTK_LIST_STORE(listMapReferenceImages), &iterMapReferenceImages, CHECK_COLUMN, TRUE, IMAGE_COLUMN, num, -1);
    }

    for(i = 0; i < countDeformedImages; i++)
    {
        char num[20];
        sprintf(num, "Deformed Image %d", i);
        gtk_list_store_append(GTK_LIST_STORE(listMapDeformedImages), &iterMapDeformedImages);
        gtk_list_store_set(GTK_LIST_STORE(listMapDeformedImages), &iterMapDeformedImages, CHECK_COLUMN, TRUE, IMAGE_COLUMN, num, -1);
    }

    gtk_widget_show(GTK_WIDGET(mapconfigWindow));
}

void detOptSubsetSize(char *referenceImagePath, char *deformedImagePath)
{
    // The value of the subset size which minimizes the deformation will be the optimum size
    // Get
}

// Function called by each thread to process its respective part of the image
void *ThreadMapStrain(void *threadnumber)
{
    // Get the id of the thread from the argument passed to the function
    int id = *((int *) threadnumber);

    // Determine the desired starting y location of the thread (threads are split in the y direction)
    int startYPos = threadHeightAllocation * id;

    // Calculate the number of y and x cycles for the thread, always round up, so we dont lack any data
    int numXCycles = ceil((double)strainArea.width / (double)subImageXStep);
    int numYCycles = ceil((double)threadHeightAllocation / (double)subImageYStep);

    // Integers for iteration through the image
    int i = 0;
    int j = 0;

    // Load the reference and deformed images for processing, as grayscale from the paths provided to the thread
    IplImage *referenceImage = cvLoadImage(referenceImagePath, CV_LOAD_IMAGE_GRAYSCALE);
    IplImage *deformedImage = cvLoadImage(deformedImagePath, CV_LOAD_IMAGE_GRAYSCALE);

    // Calculate a result width and a result height from image dimensions
    int resultWidth = deformedImage->width - subImageSize + 1;
    int resultHeight = deformedImage->height - subImageSize + 1;

    double finalPosX, finalPosY; // Storage for the determined final locations of the subimage

    // Iterate through the image, in the y direction then the x direction
    for(i = 0; i < numXCycles; i++)
    {
        for(j = 0; j < numYCycles; j++)
        {
            // Create the result image from the dimensions calculated above
            IplImage *resultImage = cvCreateImage(cvSize(resultWidth, resultHeight), IPL_DEPTH_32F, 1);

            // Calcuate the current x location from the strain area, start position and current iteration
            int currentXLoc = strainArea.x + (subImageXStep * i);
            int currentYLoc = strainArea.y + startYPos + (subImageYStep * j);

            // Set up a rectangle which can be used to cut the sub image from the template, with the specified sub image size
            CvRect subImageRect = cvRect(currentXLoc, currentYLoc, subImageSize, subImageSize);

            // Create a matrix to store the cut intensity values in
            CvMat *subImageTemplate = cvCreateMat(subImageSize, subImageSize, CV_8UC1);

            // Cut the sub image from the reference image using the template, store in the matirx
            cvGetSubRect(referenceImage, subImageTemplate, subImageRect);

            // Match the template to the deformed image, dump the result to resultimage, use normed cross correlation
            cvMatchTemplate(deformedImage, subImageTemplate, resultImage, CV_TM_CCORR_NORMED);

            cvReleaseMat(&subImageTemplate);

            // Create the maxLoc point for the sub image, this will yield the location in the deformed geometry, fill this with cvMinMaxLoc
            CvPoint maxLoc;
            cvMinMaxLoc(resultImage, 0, 0, 0, &maxLoc, 0);

            // Check if sub-pixel determination is desired by the user
            if(pseudoBoolSubPix == TRUE)
            {
                // Create a point for the interpolated maximum position on the image
                CvPoint intermaxLoc;

                // Region identifies the area of interest in the result image !! WARNING: A large region may produce out of memory errors !!
                // These are defined at the beginning of the file
                CvRect region = cvRect(maxLoc.x - _OFFSET, maxLoc.y - _OFFSET, _INTER_SIZE, _INTER_SIZE);

                // Set the region of interest as above
                cvSetImageROI(resultImage, region);

                // Create a new image for the area of interest to be cut into
                IplImage *dstImgForInterpolation = cvCreateImage(cvSize(_INTER_SIZE, _INTER_SIZE), IPL_DEPTH_32F, 1);

                // Copy the image, but do not resize yet!
                cvCopy(resultImage, dstImgForInterpolation, 0);

                // Create an image for the resize function to take place in
                IplImage *subPixelImage = cvCreateImage(cvSize(_INTER_SIZE * interpolationfactor, _INTER_SIZE * interpolationfactor), IPL_DEPTH_32F, 1);

                // Select a resize method based on the user selection in the configuration window
                switch(interpmethod)
                {
                    case 0:
                        // Use a nearest neighbour interpolation method
                        cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_NN);
                        break;
                    case 1:
                        // Use a linear interpolation method
                        cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_LINEAR);
                        break;
                    case 2:
                        // Use an area interpolation method
                        cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_AREA);
                        break;
                    case 3:
                        // Use a cubic interpolation method (best)
                        cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_CUBIC);
                        break;
                }

                // Re-locate the maxima using cvMinMaxLoc
                cvMinMaxLoc(subPixelImage, 0, 0, 0, &intermaxLoc, 0);

                // Calculate the final x position and y position using the interpolation factors and original locations
                finalPosX = ((double)intermaxLoc.x / (double)interpolationfactor) + ((double)maxLoc.x - _OFFSET);
                finalPosY = ((double)intermaxLoc.y / (double)interpolationfactor) + ((double)maxLoc.y - _OFFSET);

                // Release the images from memory, freeing it for the next iteration
                cvReleaseImage(&subPixelImage);
                cvReleaseImage(&dstImgForInterpolation);
            }
            else
            {
                // If sub-pixel accuracy is not desired, then dont bother and just set the final locations to that found previously
                finalPosX = maxLoc.x;
                finalPosY = maxLoc.y;
            }

            // Lock the mutex to prevent other threads writing to the data array
            pthread_mutex_lock(&MUTEX);

            // Calculate the correct locations in the deformation map
            int arrXPos = i;
            int arrYPos = (numYCycles * id) + j;

            // Write the locations to the deformation map
            deformationMap[arrXPos][arrYPos].origXLoc = currentXLoc;
            deformationMap[arrXPos][arrYPos].origYLoc = currentYLoc;
            deformationMap[arrXPos][arrYPos].defoXLoc = finalPosX;
            deformationMap[arrXPos][arrYPos].defoYLoc = finalPosY;

            // Increment the current cycle so that the progress bar may update
            currentCycle++;

            // Unlock the mutex to allow other threads to write their data
            pthread_mutex_unlock(&MUTEX);

            // Release the result image to free memory
            cvReleaseImage(&resultImage);
        }
    }

    // Release the reference and deformed images to free memory once the thread processing is complete
    cvReleaseImage(&referenceImage);
    cvReleaseImage(&deformedImage);

    return NULL;
}

void generateStrainMap(int refID, int defID)
{
    // Create a progress bar window, center position with a utility hint type, skip the taskbar
    GtkWidget *progressWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(progressWindow), "Measuring Element Deformation...");
    gtk_window_set_position(GTK_WINDOW(progressWindow), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_set_size_request(GTK_WIDGET(progressWindow), 600, 50);
    gtk_window_set_resizable(GTK_WINDOW(progressWindow), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(progressWindow), FALSE);
    gtk_window_set_modal(GTK_WINDOW(progressWindow), TRUE);
    gtk_window_set_type_hint(GTK_WINDOW(progressWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(progressWindow), TRUE);

    // Add a progress bar widget to the window
    prgbarMain = gtk_progress_bar_new();
    gtk_widget_show(GTK_WIDGET(prgbarMain));
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(vbox));
    GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(hbox));

    // Pack the progress bar widget into the box
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(prgbarMain), FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(vbox), TRUE, TRUE, 12);

    // Add the box to the container
    gtk_container_add(GTK_CONTAINER(progressWindow), GTK_WIDGET(hbox));

    // Initialize the progress bar to a zero fraction
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(prgbarMain), 1.00);

    // Set the widget text to give output to the user
    char text[150];
    sprintf(text, "Reference Image: %d, Deformed Image: %d... Generating Strain Surface Map (0%%)", refID, defID);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prgbarMain), text);

    // Show the widget
    gtk_widget_show(GTK_WIDGET(progressWindow));

    // Update the GUI by calling gtk_main_iteration
    while(gtk_events_pending()) { gtk_main_iteration(); }

    /* ******************************* */
    /* IMAGE PROCESSING INITIALIZATION */
    /* ******************************* */

     // Load the reference and deformed images into IplImage Structures for analysis
    IplImage *deformedImage  = cvLoadImage(deformedImagePath,  CV_LOAD_IMAGE_GRAYSCALE);

    // Retrieve the subimage size, step x direction and step y direction adjustment values from the UI
    subImageSize =  (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(subsetMapSizeAdjustment));

    // Calculate the result image widths and heights from the deformed image dimensions
    resultWidth  = deformedImage->width  - subImageSize + 1;
    resultHeight = deformedImage->height - subImageSize + 1;

    // Grab the x resolution and the y resolution (number of x iterations and y iterations) from the UI
    int xResolution = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(subsetResXAdjustment));
    int yResolution = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(subsetResYAdjustment));

    // Calculate the height allocation for each thread
    threadHeightAllocation = strainArea.height / numthreads;

    // Calculate required sub-image step for the number of readings
    subImageYStep = strainArea.height / yResolution; // Distance between Y readings
    subImageXStep = strainArea.width  / xResolution; // Distance between X readings

    // Find out if the user wants sub pixel accuracy
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkMapSubPixelAccuracy)) == TRUE)
    {
        // Set the pseudo boolean identifier to true to enable the accuracy in processing
        pseudoBoolSubPix = TRUE;

        // Get the index of the combo box from the user interface
        int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbMapAccuracyLevel));
        // Switch the index
        switch(index)
        {
            case 0:
                // If index = 0 set interpolationfactor to 10, as 0 rep 10 in model
                interpolationfactor = 10;
                break;
            case 1:
                // If index = 1 set interpolationfactor to 100, as 1 rep 100 in model
                interpolationfactor = 100;
                break;
            case 2:
                // If index = 2 set interpolationfactor to 1000, as 2 rep 1000 in model
                interpolationfactor = 1000;
                break;
        }

        // For the method, we can get the active object in the combo box for the map method, set this for switching later
        interpmethod = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbMapInterpMethod));
    }

    /* ==================================================================================== */

    int t = 0;                           // For the count of threads
    currentCycle = 0;                    // Re-initialize the cycle count for the threads
    pthread_t threads[numthreads];       // Create the required number of thread objects based on the number of cores available
    int arg[numthreads];                // Create thread ID arguments, again, based on the number of cores available
    pthread_mutex_init(&MUTEX, NULL);   // Initialize the mutex for thread locking when accessing shared memory

    // Initialize all threads, with the argument as the thread ID
    for(t = 0; t < numthreads; t++)
    {
        arg[t] = t;
        pthread_create(&threads[t], NULL, ThreadMapStrain, (void *) &arg[t]);
    }

    while(currentCycle < (xResolution * yResolution))
    {
        // Calculate the percentage complete taking into account any other threads
        double percComplete = (double)currentCycle / (double)(xResolution * yResolution);
        // If the processing is 100% complete, break the loop.
        if(percComplete > 1.0) { break; }

        // Update the progress bar when completed
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(prgbarMain), percComplete);
        char text[50];
        sprintf(text, "Generating Strain Surface Map (%d%%)", (int)(percComplete * 100));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prgbarMain), text);
        while(gtk_events_pending()) { gtk_main_iteration(); }
    }

    // Join the threads back together when complete
    for(t = 0; t < numthreads; t++) { pthread_join(threads[t], NULL); }

    /* ==================================================================================== */

    // This should be modified to write the deformation map directly to a binary output file

    const char *xoutput = gtk_entry_get_text(GTK_ENTRY(txtMapOutputXFile));
    //const char *youtput = gtk_entry_get_text(GTK_ENTRY(txtMapOutputYFile));

    FILE *xout = fopen(xoutput, "w");
    //FILE *yout = fopen(youtput, "w");

    int i, j;
    for(i = 0; i < xResolution; i++)
    {
        for(j = 0; j < yResolution; j++)
        {
            double origLen = (deformationMap[i + 1][j].origXLoc - deformationMap[i][j].origXLoc);
            double xStrain = ((deformationMap[i + 1][j].defoXLoc - deformationMap[i][j].defoXLoc) - origLen) / origLen;
            fprintf(xout, "%f %f %f\n", deformationMap[i][j].defoXLoc, deformationMap[i][j].defoYLoc, xStrain);
        }
        fprintf(xout, "\n");
    }

    fclose(xout);
    //fclose(yout);

    cvReleaseImage(&deformedImage);

    gtk_widget_hide(GTK_WIDGET(progressWindow));
}


// Function called to prepare for measurement of strain distribution on an image, when button clicked signall is processed
void measureStrainMap()
{
    // If no reference image has been loaded, print an error and return to the user interface
    if(countReferenceImages == 0)
    {
        prtError(CST_NO_REFERENCE_IMAGE);
        return;
    }

    // If no deformed images have been loaded, print an error and return to the user interface
    if(countDeformedImages == 0)
    {
        prtError(CST_NO_DEFORMED_IMAGE);
        return;
    }

    // If the strain area height or width or both is set to zero pass the message to the user and quit the function
    if(strainArea.width == 0 || strainArea.height == 0)
    {
        prtError(CST_NO_AREA_SET);
        return;
    }

    // Hide the strain map configuration window
    hideMapConfigurationWindow();

    int i = 0; // For iterating through the images
    // Get the first iteration of the reference images list store object
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listMapReferenceImages), &iterMapReferenceImages);

    // Iterate through the images using a for loop
    for(i = 0; i < countReferenceImages; i++)
    {
        // Check if the user wants this reference image to be processed, will have a boolean toggle associated
        gboolean processReference;
        gtk_tree_model_get(GTK_TREE_MODEL(listMapReferenceImages), &iterMapReferenceImages, CHECK_COLUMN, &processReference, -1); // Grab the boolean value

        // If process resolves to true, the user wants the reference image to be analysed
        if(processReference == TRUE)
        {
            int j = 0; // For iteration
            // Get the first iteration in the tree view storing the deformed images
            gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listMapDeformedImages), &iterMapDeformedImages);

            // Now iterate through the list of deformed images to get images to process with this reference image
            for(j = 0; j < countDeformedImages; j++)
            {
                gboolean processDeformed;
                gtk_tree_model_get(GTK_TREE_MODEL(listMapDeformedImages), &iterMapDeformedImages, CHECK_COLUMN, &processDeformed, -1); // Grab the boolean value

                // If the user wants the current deformed image to be processed
                if(processDeformed == TRUE)
                {
                    // Get the paths of this set of reference and deformed images for processing
                    int k = 0; // For iteration (again!)
                    referenceImagePath = NULL; // Reference image path storage
                    deformedImagePath  = NULL; // Deformed  image path storage

                    // Get the path stored by reference image (indicated by index of i integer)
                    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
                    for(k = 0; k < i; k++)
                    {
                        // Push to the next list item until we reach the path corresponding to an image with index i
                        gtk_tree_model_iter_next(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
                    }

                    // Get the path stored once the loop has completed, store it in *referenceImagePath
                    gtk_tree_model_get(GTK_TREE_MODEL(referenceImages), &referenceImageIter, 0, &referenceImagePath, -1);

                    // Now rinse, and repeat for the deformed image

                    // Get the path stored by deformed image (indicated by index of j integer)
                    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(deformedImages), &deformedImageIter);
                    for(k = 0; k < j; k++)
                    {
                        // Push to the next list item until we reach the path corresponding to an image with index i
                        gtk_tree_model_iter_next(GTK_TREE_MODEL(deformedImages), &deformedImageIter);
                    }

                    // Get the path stored once the loop has completed, store it in *referenceImagePath
                    gtk_tree_model_get(GTK_TREE_MODEL(deformedImages), &deformedImageIter, 0, &deformedImagePath, -1);

                    // For now, we'll just print the image path, and not execute the actual function which carries out the correlation
                    printf("Ref: %s\nDef: %s\n", referenceImagePath, deformedImagePath);
                    generateStrainMap(i, j);
                }
            }
        }
    }

    // Show a dialog upon completion of the processing to signify that the work has been done
    GtkWidget *completionDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Processing was Successfully Completed");
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(completionDialog), "A strain map was successfully produced for the selected images.\n\nPress OK to continue.");
    gtk_dialog_run(GTK_DIALOG(completionDialog));
    gtk_widget_destroy(GTK_WIDGET(completionDialog));
}

// Function called on toggle signal of the sub-pixel accuracy toggle button
void subPixelToggled()
{
    // If the button is toggled, set the property widgets for the interpolation system to sensitive
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkMapSubPixelAccuracy)) == TRUE)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(cmbMapAccuracyLevel), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblMapAccuracyLevel), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(cmbMapInterpMethod), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblMapInterpolationMethod), TRUE);
    }
    // Else set all these widgets to insensitive, and leave them inactive in the window
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(cmbMapAccuracyLevel), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblMapAccuracyLevel), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(cmbMapInterpMethod), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblMapInterpolationMethod), FALSE);
    }
}

// If the toggle button on a reference image identifier in the tree view is toggled
void refMapIdentifierToggled(GtkCellRendererToggle *toggle, gchar *path, GtkTreeView *treeView)
{
    GtkTreeIter iter;
    gboolean value;

    // Set the toggled state to true or false depending on the previous value of the toggle
    if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(listMapReferenceImages), &iter, path))
    {
        gtk_tree_model_get(GTK_TREE_MODEL(listMapReferenceImages), &iter, CHECK_COLUMN, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(listMapReferenceImages), &iter, CHECK_COLUMN, !value, -1);
    }
}

// If the toggle button on a deformed image identifier in the tree view is toggled
void defMapIdentifierToggled(GtkCellRendererToggle *toggle, gchar *path, GtkTreeView *treeView)
{
    GtkTreeIter iter;
    gboolean value;

    // Set the toggled state to true or false depending on the previous value of the toggle
    if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(listMapDeformedImages), &iter, path))
    {
        gtk_tree_model_get(GTK_TREE_MODEL(listMapDeformedImages), &iter, CHECK_COLUMN, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(listMapDeformedImages), &iter, CHECK_COLUMN, !value, -1);
    }
}

// If the measurement type combo box is modified, call this signal
void measurementTypeChanged()
{
    // Grab the index from the active item in the combo box, store in index integer
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbMapMeasurementType));
    // Switch the index
    switch(index)
    {
        case 0:
            // For an x axis only measurement, set the following to sensitive/insensitive
            gtk_widget_set_sensitive(GTK_WIDGET(subsetResXSpin),   TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblSubsetStepX),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(selMapOutputXFile), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(txtMapOutputXFile), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileX),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(subsetResYSpin),   FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileX),    FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileY),    FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(txtMapOutputYFile), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(selMapOutputYFile), FALSE);
            break;
        case 1:
            // For a y axis only measurement, set the following to sensitive/insensitive
            gtk_widget_set_sensitive(GTK_WIDGET(subsetResXSpin),   FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblSubsetStepX),    FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(selMapOutputXFile), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(txtMapOutputXFile), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileX),    FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(subsetResYSpin),   TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblSubsetStepY),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileY),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(txtMapOutputYFile), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(selMapOutputYFile), TRUE);
            break;
        case 2:
            // For measurement on both axes, set the following to sensitive only
            gtk_widget_set_sensitive(GTK_WIDGET(subsetResXSpin),   TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblSubsetStepX),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(selMapOutputXFile), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(txtMapOutputXFile), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileX),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(subsetResYSpin),   TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblSubsetStepY),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(lblOutputFileY),    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(txtMapOutputYFile), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(selMapOutputYFile), TRUE);
            break;
    }
}

// Function called to select the x output file for the strain mapping function
void selectOutputXFile()
{
    // Get the home path environment variable for the default folder
    char *homePath = getenv("HOME");
    char *filePath; // For storage from the file selection dialog

    // Create the file selection dialog box for saving of the x axis data
    GtkWidget *selectFile = gtk_file_chooser_dialog_new("Create Output File for x-Axis Strain", GTK_WINDOW(mainWindow),
                                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    // Set the dialog to warn if overwriting a file which exists in the f/s
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(selectFile), TRUE);

    // Allow the file chooser to save as CSV and DAT files, by setting group of filters
    GtkFileFilter *filterCSV = gtk_file_filter_new();
    GtkFileFilter *filterDAT = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filterCSV), "CSV Data");
    gtk_file_filter_set_name(GTK_FILE_FILTER(filterDAT), "DAT Data");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterDAT), "*.dat");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterCSV), "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterCSV));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterDAT));

    // Set the default file type to a CSV (comma separated file)
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterCSV));

    // Set the current folder to the users home folder, typ. /home/user
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(selectFile), homePath);
    // Set the current name to something generic, xoutput.csv for example
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "xoutput.csv");

    // Run the dialog, if we get an accept response
    if(gtk_dialog_run(GTK_DIALOG(selectFile)) == GTK_RESPONSE_ACCEPT)
    {
        // Retrieve the save target from the file chooser widget
        filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(selectFile));
        // Set the text box to show the specified path, may be modified further by user
        gtk_entry_set_text(GTK_ENTRY(txtMapOutputXFile), filePath);
        // Destroy the widget
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
    else
    {
        // Cancel was hit, so just destroy the widget
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
}

// Function called to select the x output file for the strain mapping function
void selectOutputYFile()
{
    // Get the home path environment variable for the default folder
    char *homePath = getenv("HOME");
    char *filePath; // For storage from the file selection dialog

    // Create the file selection dialog box for saving of the y axis data
    GtkWidget *selectFile = gtk_file_chooser_dialog_new("Create Output File for y-Axis Strain", GTK_WINDOW(mainWindow),
                                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                                        GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    // Set the dialog to warn if overwriting a file which exists in the f/s
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(selectFile), TRUE);

    // Allow the file chooser to save as CSV and DAT files, by setting group of filters
    GtkFileFilter *filterCSV = gtk_file_filter_new();
    GtkFileFilter *filterDAT = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filterCSV), "CSV Data");
    gtk_file_filter_set_name(GTK_FILE_FILTER(filterDAT), "DAT Data");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterDAT), "*.dat");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterCSV), "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterCSV));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterDAT));

    // Set the default file type to a CSV (comma separated file)
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterCSV));

    // Set the current folder to the users home folder, typ. /home/user
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(selectFile), homePath);
    // Set the current name to something generic, youtput.csv for example
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "youtput.csv");

    // Run the dialog, if we get an accept response
    if(gtk_dialog_run(GTK_DIALOG(selectFile)) == GTK_RESPONSE_ACCEPT)
    {
        // Retrieve the save target from the file chooser widget
        filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(selectFile));
        // Set the text box to show the specified path, may be modified further by user
        gtk_entry_set_text(GTK_ENTRY(txtMapOutputYFile), filePath);
        // Destroy the widget
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
    else
    {
        // Cancel was hit, so just destroy the widget
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
}

// Function to create the configuration window (gtk+ code)
// This function is executed on application start-up, before the main loop is called
void createMapConfigurationWindow()
{
    // Configure the main window widget, size automatic, type hint utility, skipping the taskbar, not resizable
    mapconfigWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(mapconfigWindow), "Strain Map Test Configuration");
    gtk_widget_set_size_request(GTK_WIDGET(mapconfigWindow), -1, -1);
    gtk_window_set_resizable(GTK_WINDOW(mapconfigWindow), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(mapconfigWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(mapconfigWindow), TRUE);

    // settingsTable widget contains the labels and tree view widgets for selecting images, set here.
    GtkWidget *settingsTable = gtk_table_new(3, 3, FALSE);
    GtkWidget *lblReferenceImages = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lblReferenceImages), "<b>Reference Images</b>");
    GtkWidget *lblDeformedImages = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lblDeformedImages), "<b>Deformed Images</b>");
    gtk_widget_show(GTK_WIDGET(lblReferenceImages));
    gtk_widget_show(GTK_WIDGET(lblDeformedImages));
    gtk_widget_show(GTK_WIDGET(settingsTable));

    // List store objects for the reference images and deformed images, each identical, type boolean (col1) string (col2)
    listMapReferenceImages = gtk_list_store_new(2, G_TYPE_BOOLEAN, G_TYPE_STRING);
    listMapDeformedImages = gtk_list_store_new(2, G_TYPE_BOOLEAN, G_TYPE_STRING);
    // Tree view objects for the reference images and deformed images, each identical, set model to above
    GtkWidget *tvReferenceImages = gtk_tree_view_new_with_model(GTK_TREE_MODEL(listMapReferenceImages));
    GtkWidget *tvDeformedImages = gtk_tree_view_new_with_model(GTK_TREE_MODEL(listMapDeformedImages));
    // Set the default size request (dimensions, pix) of the widgets, 250 h by 175 w.
    gtk_widget_set_size_request(GTK_WIDGET(tvReferenceImages), 250, 175);
    gtk_widget_set_size_request(GTK_WIDGET(tvDeformedImages), 250, 175);
    gtk_widget_show(GTK_WIDGET(tvReferenceImages));
    gtk_widget_show(GTK_WIDGET(tvDeformedImages));

    // Vertical box contains the two tables found on the window and the buttons at the bottom of the window
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(vbox));

    // Tree view columns and cell renderers for the tree views, one respectively for each column
    GtkTreeViewColumn *column0, *column1, *column2, *column3;
    GtkCellRenderer *render0, *render1, *render2, *render3;

    // render0, column0 (created above) contains toggle object for first column: reference images
    render0 = gtk_cell_renderer_toggle_new();
    g_signal_connect(G_OBJECT(render0), "toggled", G_CALLBACK(refMapIdentifierToggled), NULL);
    column0 = gtk_tree_view_column_new_with_attributes("Process", render0, "active", CHECK_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvReferenceImages), GTK_TREE_VIEW_COLUMN(column0)); // Column is appended to the tree view

    // render1, column1 (created above) contains text object for second column: reference images
    render1 = gtk_cell_renderer_text_new();
    column1 = gtk_tree_view_column_new_with_attributes("Image", render1, "text", IMAGE_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvReferenceImages), GTK_TREE_VIEW_COLUMN(column1)); // Column is appended to the tree view

    // render2, column2 (created above) contains toggle object for first column: deformed images
    render2 = gtk_cell_renderer_toggle_new();
    g_signal_connect(G_OBJECT(render2), "toggled", G_CALLBACK(defMapIdentifierToggled), NULL);
    column2 = gtk_tree_view_column_new_with_attributes("Process", render2, "active", CHECK_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvDeformedImages), GTK_TREE_VIEW_COLUMN(column2));

    // render4, column4 (created above) contains text object for second column: deformed images
    render3 = gtk_cell_renderer_text_new();
    column3 = gtk_tree_view_column_new_with_attributes("Image", render3, "text", IMAGE_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvDeformedImages), GTK_TREE_VIEW_COLUMN(column3));

    // Attach labels and tree view widgets to the settingsTable widget (see above)
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblReferenceImages),  0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblDeformedImages),   1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(tvReferenceImages),   0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(tvDeformedImages),    1, 2, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    // Set the row spacings to 12 pixels
    gtk_table_set_row_spacings(GTK_TABLE(settingsTable), 12);
    // Append the table to the vbox widget containing the settings widgets for the window
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(settingsTable), TRUE, TRUE, 12);

    // settingsTable2 contains other widgets controlling the strain measurement
    GtkWidget *settingsTable2 = gtk_table_new(3, 3, FALSE);
    gtk_widget_show(GTK_WIDGET(settingsTable2));

    // Combo box for the measurement type, contains 3 options, horiz, vert, both.
    cmbMapMeasurementType = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapMeasurementType), "Horizontal Measurement");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapMeasurementType), "Vertical Measurement");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapMeasurementType), "Dual Axis Measurement");
    // Set the active item to both (index 2)
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbMapMeasurementType), 2);
    GtkWidget *lblMeasurementType = gtk_label_new("Measurement Type:");
    GtkWidget *algMeasurementType = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algMeasurementType));
    gtk_container_add(GTK_CONTAINER(algMeasurementType), GTK_WIDGET(lblMeasurementType));
    gtk_widget_show(GTK_WIDGET(lblMeasurementType));
    gtk_widget_show(GTK_WIDGET(cmbMapMeasurementType));
    // Signal identifies when the combo box active item has been changed.
    g_signal_connect(G_OBJECT(cmbMapMeasurementType), "changed", G_CALLBACK(measurementTypeChanged), NULL);

    // Subset size adjustment for the map configuration window, default 25, step 1, max 100, page 10.
    subsetMapSizeAdjustment = gtk_adjustment_new(25, 1, 100, 1, 10, 0.0);
    // Set the spin widget to the subset size adjustment
    GtkWidget *subsetSizeSpin = gtk_spin_button_new(GTK_ADJUSTMENT(subsetMapSizeAdjustment), 1.0, 0);
    gtk_widget_show(GTK_WIDGET(subsetSizeSpin));
    GtkWidget *lblSubsetSize = gtk_label_new("Subset Size:");
    gtk_widget_show(GTK_WIDGET(lblSubsetSize));
    GtkWidget *algSubsetSize = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algSubsetSize));
    gtk_container_add(GTK_CONTAINER(algSubsetSize), GTK_WIDGET(lblSubsetSize));
    GtkWidget *lblPixel1 = gtk_label_new("pixels");
    gtk_widget_show(GTK_WIDGET(lblPixel1));

    // Subset step x adjustment for the map configuration window, default 25, step 1, max 100, page 10.
    subsetResXAdjustment = gtk_adjustment_new(50, 10, 4999, 1, 10, 0.0);
    // Set the spin widget to the subset step adjustment
    subsetResXSpin = gtk_spin_button_new(GTK_ADJUSTMENT(subsetResXAdjustment), 1.0, 0);
    gtk_widget_show(GTK_WIDGET(subsetResXSpin));
    lblSubsetStepX = gtk_label_new("Subset x-Axis Resolution:");
    gtk_widget_show(GTK_WIDGET(lblSubsetStepX));
    GtkWidget *algSubsetStepX = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algSubsetStepX));
    gtk_container_add(GTK_CONTAINER(algSubsetStepX), GTK_WIDGET(lblSubsetStepX));
    lblPixel2 = gtk_label_new("samples");
    gtk_widget_show(GTK_WIDGET(lblPixel2));

    // Subset step y adjustment for the map configuration window, default 25, step 1, max 100, page 10.
    subsetResYAdjustment = gtk_adjustment_new(50, 10, 4999, 1, 10, 0.0);
    // Set the spin widget to the subset step adjustment
    subsetResYSpin = gtk_spin_button_new(GTK_ADJUSTMENT(subsetResYAdjustment), 1.0, 0);
    gtk_widget_show(GTK_WIDGET(subsetResYSpin));
    lblSubsetStepY = gtk_label_new("Subset y-Axis Resolution:");
    gtk_widget_show(GTK_WIDGET(lblSubsetStepY));
    GtkWidget *algSubsetStepY = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algSubsetStepY));
    gtk_container_add(GTK_CONTAINER(algSubsetStepY), GTK_WIDGET(lblSubsetStepY));
    lblPixel3 = gtk_label_new("samples");
    gtk_widget_show(GTK_WIDGET(lblPixel3));

    // Check button widget for enabling subset accuracy
    chkMapSubPixelAccuracy = gtk_check_button_new_with_label("Enable Sub-Pixel Accuracy");
    gtk_widget_show(GTK_WIDGET(chkMapSubPixelAccuracy));
    // Signal for when the subpixel accuracy widget is checked, call the subPixelToggled() function.
    g_signal_connect(G_OBJECT(chkMapSubPixelAccuracy), "toggled", G_CALLBACK(subPixelToggled), NULL);

    // Map accuracy level combo box, 3 options, 1/10th pixel, 1/100th pixel, 1/1000th pixel, sensitive only when chkMapSubPixelAccuracy is active
    cmbMapAccuracyLevel = gtk_combo_box_text_new();
    gtk_widget_show(GTK_WIDGET(cmbMapAccuracyLevel));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapAccuracyLevel), "Pixel/10");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapAccuracyLevel), "Pixel/100");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapAccuracyLevel), "Pixel/1000");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbMapAccuracyLevel), 0);
    lblMapAccuracyLevel = gtk_label_new("Accuracy Level:");
    GtkWidget *algAccuracyLevel = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algAccuracyLevel));
    gtk_container_add(GTK_CONTAINER(algAccuracyLevel), GTK_WIDGET(lblMapAccuracyLevel));
    gtk_widget_show(GTK_WIDGET(lblMapAccuracyLevel));
    gtk_widget_set_sensitive(GTK_WIDGET(cmbMapAccuracyLevel), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblMapAccuracyLevel), FALSE);

    // Interpolation Method combo, 4 options, NN, Linear, Area, Bicubic, bicubic default (best results), not sensitive unless cchkMapSubPixelAccuracy is active
    cmbMapInterpMethod = gtk_combo_box_text_new();
    gtk_widget_show(GTK_WIDGET(cmbMapInterpMethod));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapInterpMethod), "Nearest Neighbour");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapInterpMethod), "Linear");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapInterpMethod), "Area");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMapInterpMethod), "Bicubic");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbMapInterpMethod), 3);
    lblMapInterpolationMethod = gtk_label_new("Interpolation Method:");
    GtkWidget *algInterpolationMethod = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algInterpolationMethod));
    gtk_container_add(GTK_CONTAINER(algInterpolationMethod), GTK_WIDGET(lblMapInterpolationMethod));
    gtk_widget_show(GTK_WIDGET(lblMapInterpolationMethod));
    gtk_widget_set_sensitive(GTK_WIDGET(cmbMapInterpMethod), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblMapInterpolationMethod), FALSE);

    // Get the environment variable path for the user home directory (typic /home/user), env: $HOME
    char *homePath = getenv("HOME");
    // Copy it to a safe character array for the x axis, if modified will not result in a modification of the environment variable
    char copyHomePathX[strlen(homePath) + 1];
    strcpy(copyHomePathX, homePath);
    strncat(copyHomePathX, "/xoutput.csv", 12);

    // Copy it to a safe character array for the x axis, if modified will not result in a modification of the environment variable
    char copyHomePathY[strlen(homePath) + 1];
    strcpy(copyHomePathY, homePath);
    strncat(copyHomePathY, "/youtput.csv", 12);

    // Output file selection for the strain map in the x direction
    selMapOutputXFile = gtk_button_new_with_label("Select");
    gtk_widget_show(GTK_WIDGET(selMapOutputXFile));
    g_signal_connect(G_OBJECT(selMapOutputXFile), "clicked", G_CALLBACK(selectOutputXFile), NULL);
    txtMapOutputXFile = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtMapOutputXFile), copyHomePathX);
    gtk_widget_show(GTK_WIDGET(txtMapOutputXFile));
    lblOutputFileX = gtk_label_new("x-Axis Output File:");
    gtk_widget_show(GTK_WIDGET(lblOutputFileX));
    GtkWidget *algOutputFileX = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algOutputFileX));
    gtk_container_add(GTK_CONTAINER(algOutputFileX), GTK_WIDGET(lblOutputFileX));
    GtkWidget *hboxOutputX = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hboxOutputX));
    gtk_box_pack_start(GTK_BOX(hboxOutputX), GTK_WIDGET(txtMapOutputXFile), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hboxOutputX), GTK_WIDGET(selMapOutputXFile), FALSE, FALSE, 0);

    // Output file selection for the strain map in the y direction
    selMapOutputYFile = gtk_button_new_with_label("Select");
    gtk_widget_show(GTK_WIDGET(selMapOutputYFile));
    g_signal_connect(G_OBJECT(selMapOutputYFile), "clicked", G_CALLBACK(selectOutputYFile), NULL);
    txtMapOutputYFile = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtMapOutputYFile), copyHomePathY);
    gtk_widget_show(GTK_WIDGET(txtMapOutputYFile));
    lblOutputFileY = gtk_label_new("y-Axis Output File:");
    gtk_widget_show(GTK_WIDGET(lblOutputFileY));
    GtkWidget *algOutputFileY = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algOutputFileY));
    gtk_container_add(GTK_CONTAINER(algOutputFileY), GTK_WIDGET(lblOutputFileY));
    GtkWidget *hboxOutputY = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hboxOutputY));
    gtk_box_pack_start(GTK_BOX(hboxOutputY), GTK_WIDGET(txtMapOutputYFile), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hboxOutputY), GTK_WIDGET(selMapOutputYFile), FALSE, FALSE, 0);

    // Check box to display the graph plot window on completion of the processing
    GtkWidget *chkShowMapOutputWindow = gtk_check_button_new_with_label("Show Output Window");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkShowMapOutputWindow), TRUE);
    gtk_widget_show(GTK_WIDGET(chkShowMapOutputWindow));

    // Attach the widgets to the table, 12 pixel padding for widgets to far left, 6 for widgets to far right
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algMeasurementType),     0, 1, 0, 1, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(cmbMapMeasurementType),  1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algSubsetSize),          0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(subsetSizeSpin),         1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(lblPixel1),              2, 3, 1, 2, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algSubsetStepX),         0, 1, 2, 3, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(subsetResXSpin),         1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(lblPixel2),              2, 3, 2, 3, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algSubsetStepY),         0, 1, 3, 4, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(subsetResYSpin),         1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(lblPixel3),              2, 3, 3, 4, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(chkMapSubPixelAccuracy), 1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algAccuracyLevel),       0, 1, 5, 6, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(cmbMapAccuracyLevel),    1, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algInterpolationMethod), 0, 1, 6, 7, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(cmbMapInterpMethod),     1, 2, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algOutputFileX),         0, 1, 7, 8, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(hboxOutputX),            1, 2, 7, 8, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algOutputFileY),         0, 1, 8, 9, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(hboxOutputY),            1, 2, 8, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(chkShowMapOutputWindow), 1, 2, 9, 10, GTK_FILL, GTK_FILL, 0, 0);
    // Set the row spacings for the table to 6 pixels
    gtk_table_set_row_spacings(GTK_TABLE(settingsTable2), 6);

    // Pack the settings table into the vbox containing the settings widgets
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(settingsTable2), TRUE, TRUE, 6);

    // Create a process images and close button(s), pack these into an hbox with an alignment to keep them right
    GtkWidget *hboxButtons = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hboxButtons));
    GtkWidget *btnProcessImages = gtk_button_new_with_label("Process Images");
    gtk_widget_show(GTK_WIDGET(btnProcessImages));
    GtkWidget *btnClose = gtk_button_new_with_label("Close");
    gtk_widget_show(GTK_WIDGET(btnClose));
    gtk_box_pack_start(GTK_BOX(hboxButtons), GTK_WIDGET(btnClose), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxButtons), GTK_WIDGET(btnProcessImages), FALSE, FALSE, 12);
    GtkWidget *algButtons = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algButtons));
    gtk_container_add(GTK_CONTAINER(algButtons), GTK_WIDGET(hboxButtons));
    // Pack the buttons into the vbox containing the settings widgets
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(algButtons), FALSE, FALSE, 12);

    // Add the vbox to the container window mapconfigWindow
    gtk_container_add(GTK_CONTAINER(mapconfigWindow), GTK_WIDGET(vbox));

    // Connect the signals for hiding the window when the user clicks the x button or the close button
    g_signal_connect(G_OBJECT(btnClose), "clicked", G_CALLBACK(hideMapConfigurationWindow), NULL);
    g_signal_connect(G_OBJECT(mapconfigWindow), "delete_event", G_CALLBACK(hideMapConfigurationWindow), NULL);
    g_signal_connect(G_OBJECT(btnProcessImages), "clicked", G_CALLBACK(measureStrainMap), NULL);
}

// End of file
