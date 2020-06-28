/*  FILE:       measurestrain.c
    AUTHOR:     James E Vigor
    PROJECT:    CST
    DATE:       15-6-2012
    DESCRIP:    Strain Distribution Functions for the CST application
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
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "guielements.h"
#include "constants.h"
#include "doerrors.h"
#include "strainoutputconfig.h"

#define _OFFSET 5
#define _INTER_SIZE 10

// GTK Items should remain within the scope of this c file, therefore declared as static
static GtkListStore *listDstReferenceImages, *listDstDeformedImages;        // List stores for reference and deformed images
static GtkTreeIter   iterDstReferenceImages,  iterDstDeformedImages;        // Iterators for respective list stores
static GtkObject    *subsetSizeAdjustment,   *subsetStepAdjustment;         // Adjustment objects for step and size
static GtkObject    *adjUpperBound,          *adjLowerBound;                // Lower and upper bound correction values

static GtkWidget *strainconfigWindow;                                       // Strain configuration window widget
static GtkWidget *chkSubPixelAcc,    *chkShowOutputWindow;                   // Check buttons for sub-pixel accuracy and output window
static GtkWidget *cmbInterpMethod,  *cmbAccuracyLevel;                      // Combo box for accuracy level
static GtkWidget *selOutputFile;                                            // Button for output file selection
static GtkWidget *txtOutputFile;                                            // Output file text box
static GtkWidget *lblAccuracyLevel, *lblInterpolationMethod;                // Interpolation and accuracy labels
static GtkWidget *lblMapInterpolationMethod;                                // Interpolation method label

// Enumeration for the column identifiers for the tree view widget(s)
enum
{
    CHECK_COLUMN,
    IMAGE_COLUMN
};

// Enumeration to provide abstraction for the interpolation size
enum
{
    TEN_PIXEL,
    HUNDRED_PIXEL,
    THOUSAND_PIXEL
};

// Enumeration to provide abstraction for the interpolation method
enum
{
    NEAREST,
    LINEAR,
    AREA,
    CUBIC
};

typedef struct
{
    int depth;
    int correct;
    double unitdepth;
    double strain;
} StrainPoint;

// Function to hide the strain distribution configuration window (window concerned with this file)
void HideDistributionConfiguration()
{
    gtk_widget_hide(GTK_WIDGET(strainconfigWindow));
}

// Function to show the strain distribution configuration window (the window concerned with this file)
void ShowDistributionConfiguration()
{
    // Initialize the window by clearing the list of reference and deformed images
    gtk_list_store_clear(GTK_LIST_STORE(listDstReferenceImages));
    gtk_list_store_clear(GTK_LIST_STORE(listDstDeformedImages));

    // Integer i is declared for iteration
    int i = 0;

    // Append all reference and deformed images to the list store items for processing
    for(i = 0; i < countReferenceImages; i++)
    {
        char num[20];
        sprintf(num, "Reference Image %d", i);
        gtk_list_store_append(GTK_LIST_STORE(listDstReferenceImages), &iterDstReferenceImages);
        gtk_list_store_set(GTK_LIST_STORE(listDstReferenceImages), &iterDstReferenceImages, CHECK_COLUMN, TRUE, IMAGE_COLUMN, num, -1);
    }

    for(i = 0; i < countDeformedImages; i++)
    {
        char num[20];
        sprintf(num, "Deformed Image %d", i);
        gtk_list_store_append(GTK_LIST_STORE(listDstDeformedImages), &iterDstDeformedImages);
        gtk_list_store_set(GTK_LIST_STORE(listDstDeformedImages), &iterDstDeformedImages, CHECK_COLUMN, TRUE, IMAGE_COLUMN, num, -1);
    }

    // Show the widget
    gtk_widget_show(GTK_WIDGET(strainconfigWindow));
}

// Function to be called on the selection of an output file from the user interface
void SelectOutputFile()
{
    // Get the users home folder from the home environment variable
    char *homePath = getenv("HOME");
    // Create the file path string to store the file path from the dialog
    char *filePath;

    // File chooser dialog to select the output file to be created
    GtkWidget *selectFile = gtk_file_chooser_dialog_new("Create Output File", GTK_WINDOW(mainWindow),
                            GTK_FILE_CHOOSER_ACTION_SAVE,
                            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    // Set the dialogs overwrite confirmation property to true
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(selectFile), TRUE);

    // Create filters for the file chooser dialog, of type DAT and CSV
    GtkFileFilter *filterCSV = gtk_file_filter_new();
    GtkFileFilter *filterDAT = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filterCSV), "CSV Data");
    gtk_file_filter_set_name(GTK_FILE_FILTER(filterDAT), "DAT Data");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterDAT), "*.dat");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filterCSV), "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterCSV));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterDAT));
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filterCSV));

    // Set the current folder and the current file name to the default values
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(selectFile), homePath);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "output.csv");

    // Run the dialog and check the response
    if(gtk_dialog_run(GTK_DIALOG(selectFile)) == GTK_RESPONSE_ACCEPT)
    {
        // Retrieve the save target
        filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(selectFile));
        gtk_entry_set_text(GTK_ENTRY(txtOutputFile), filePath);
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
    else
    {
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
}

// Function for the generation of the strain distribution from the actual image files
void GenerateStrainDistribution(char *referenceImagePath, char *deformedImagePath, int refID, int defID)
{
    // Load the images in OpenCV for analysis
    IplImage *referenceImage = cvLoadImage(referenceImagePath, CV_LOAD_IMAGE_GRAYSCALE);
    IplImage *deformedImage  = cvLoadImage(deformedImagePath,  CV_LOAD_IMAGE_GRAYSCALE);

    // Get the subset size and step values from the user interface
    int subImageSize = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(subsetSizeAdjustment));
    int subImageStep = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(subsetStepAdjustment));

    // Calculate the width and height of the result image using the dimensions of the deformed image
    int resultWidth = deformedImage->width - subImageSize + 1;
    int resultHeight = deformedImage->height - subImageSize + 1;

    // The number of y cycles is calculated using the subimage step and the height of the measurement area
    int numYCycles = (int)strainArea.height / subImageStep;

    // For storage of strain values (as opposed to writing them directly to the file)
    StrainPoint strainValues[numYCycles];

    // Result image is created with a depth IPL_DEPTH_32F, size is width of result image and height of result image
    IplImage *resultImage = cvCreateImage(cvSize(resultWidth, resultHeight), IPL_DEPTH_32F, 1);

    // Create a progress winodow object to display completion of the measurement
    GtkWidget *progressWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(progressWindow), "Measuring Element Deformation...");
    gtk_window_set_position(GTK_WINDOW(progressWindow), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_set_size_request(GTK_WIDGET(progressWindow), 600, 50);
    gtk_window_set_resizable(GTK_WINDOW(progressWindow), FALSE);
    gtk_window_set_deletable(GTK_WINDOW(progressWindow), FALSE);
    gtk_window_set_modal(GTK_WINDOW(progressWindow), TRUE);
    gtk_window_set_type_hint(GTK_WINDOW(progressWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(progressWindow), TRUE);

    // Add the progress bar widget to the main window
    GtkWidget *prgbarMain = gtk_progress_bar_new();
    gtk_widget_show(GTK_WIDGET(prgbarMain));
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(vbox));
    GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(hbox));

    // Pack the progress bar widget into the box
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(prgbarMain), FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(vbox), TRUE, TRUE, 12);

    // Add the box to the progress window container
    gtk_container_add(GTK_CONTAINER(progressWindow), GTK_WIDGET(hbox));

    // Initialize the progress bar by setting the current progress to zero
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(prgbarMain), 0.00);

    // Set the text to display on the progress bar
    char text[150];
    sprintf(text, "Reference Image: %d, Deformed Image: %d... Generating Strain Distribution (0%%)", refID, defID);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prgbarMain), text);

    // Show the widget
    gtk_widget_show(GTK_WIDGET(progressWindow));

    // Get the output file path from the entry widget
    const char *outPath = gtk_entry_get_text(GTK_ENTRY(txtOutputFile));
    // Open the specified output path for write (overwrite if exists)
    FILE *outputWrite = fopen(outPath, "w");

    // Index stores the index of the current combo box under inspection, interpolationfactor stores the factor for interpolation and division
    int index, method, interpolationfactor;
    // If the check/toggle button is active, the user wants sub-pixel accuracy
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkSubPixelAcc)) == TRUE)
    {
        // Grab the index from the active item in the accuracy level box
        index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbAccuracyLevel));
        // Set the interpolation factor based on the index (see enumerations)
        switch(index)
        {
        case TEN_PIXEL:
            interpolationfactor = 10;
            break;
        case HUNDRED_PIXEL:
            interpolationfactor = 100;
            break;
        case THOUSAND_PIXEL:
            interpolationfactor = 1000;
            break;
        }
    }

    // Get the interpolation method index from the combo box
    method = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbInterpMethod));

    // Update the user interface if there are events pending for it
    while(gtk_events_pending())
    {
        gtk_main_iteration();
    }

    int a = 0; // For iteration in the vertical direction
    for(a = 0; a < numYCycles; a++)
    {
        // Calculate the percentage complete based on the current cycle and the total number of cycles
        double percComplete = (double)a / (double)numYCycles;
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(prgbarMain), percComplete); // Set the bar fraction using the calculation
        // Regenerate the text and set the progress bar to this text
        sprintf(text, "Reference Image: %d, Deformed Image: %d... Generating Strain Distribution (%d%%)", refID, defID, (int)(percComplete * 100));
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(prgbarMain), text);
        // Update the user interface to reflect these changes
        while(gtk_events_pending())
        {
            gtk_main_iteration();
        }

        // Calculate the x and y locations of the first and second point for the measurement to be made from
        int xLoc[2], yLoc[2];
        xLoc[0] = strainArea.x;
        yLoc[0] = (strainArea.y) + (subImageStep * (a + 1));
        xLoc[1] = strainArea.x + strainArea.width - subImageSize;
        yLoc[1] = (strainArea.y) + (subImageStep * (a + 1));
        int origLength = strainArea.width - subImageSize;
        double finalPosX[2], finalPosY[2];

        CvPoint maxLoc[2];

        int x = 0;
        for(x = 0; x < 2; x++)
        {
            // Set up the result image using the cvCreateImage function using the result width and height
            resultImage = cvCreateImage(cvSize(resultWidth, resultHeight), IPL_DEPTH_32F, 1);
            // Create a rectangle to cut the sub image from the reference subset at xLoc0 and yLoc0
            CvRect subImageRect = cvRect(xLoc[x], yLoc[x], subImageSize, subImageSize);
            // Create a matrix to store the intensity data from the reference image
            CvMat *subImageTemplate = cvCreateMat(subImageSize, subImageSize, CV_8UC1);
            // Cut the rectangle from the reference subset, into the sub image matrix, through the sub image rectangle
            cvGetSubRect(referenceImage, subImageTemplate, subImageRect);
            // Match the subimage template to the deformed image, dump the results in resultImaeg, use a normed cross correlation algorithm
            cvMatchTemplate(deformedImage, subImageTemplate, resultImage, CV_TM_CCORR_NORMED);

            // maxLoc0 contains the original maxima location of the subset in the result image
            cvMinMaxLoc(resultImage, 0, 0, 0, &maxLoc[x], 0);

            // Check if we want sub pixel accuracy in the operation
            if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkSubPixelAcc)) == TRUE)
            {
                // Point structure for the storage of the maximum location of interpolation
                CvPoint intermaxLoc;
                // Set the region of the image for inspection, and set the image ROI to this area
                CvRect region = cvRect(maxLoc[x].x - _OFFSET, maxLoc[x].y - _OFFSET, _INTER_SIZE, _INTER_SIZE);
                cvSetImageROI(resultImage, region);
                // Create the image for interpolation
                IplImage *dstImgForInterpolation = cvCreateImage(cvSize(_INTER_SIZE, _INTER_SIZE), IPL_DEPTH_32F, 1);
                // Copy the result image to the image for interpolation
                cvCopy(resultImage, dstImgForInterpolation, 0);
                // The subpixel image contains the interpolated image for interpolatin
                IplImage *subPixelImage = cvCreateImage(cvSize(_INTER_SIZE * interpolationfactor, _INTER_SIZE * interpolationfactor), IPL_DEPTH_32F, 1);

                // Get the method desired for interpolation and run the interpolation based on this
                switch(method)
                {
                case NEAREST:
                    cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_NN);
                    break;
                case LINEAR:
                    cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_LINEAR);
                    break;
                case AREA:
                    cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_AREA);
                    break;
                case CUBIC:
                    cvResize(dstImgForInterpolation, subPixelImage, CV_INTER_CUBIC);
                    break;
                }

                // Locate the maxima in the interpolated image
                cvMinMaxLoc(subPixelImage, 0, 0, 0, &intermaxLoc, 0);

                // Calculate the equivalent final x and y positions of the image
                finalPosX[x] = ((double)intermaxLoc.x / (double)interpolationfactor) + ((double)maxLoc[x].x - _OFFSET);
                finalPosY[x] = ((double)intermaxLoc.y / (double)interpolationfactor) + ((double)maxLoc[x].y - _OFFSET);

                // Release the images to free memory
                cvReleaseImage(&dstImgForInterpolation);
                cvReleaseImage(&subPixelImage);
            }
            else
            {
                // If subpixel accuracy is not wanted, just set the final locations to the originals
                finalPosX[x] = maxLoc[x].x;
                finalPosY[x] = maxLoc[x].y;
            }
        }

        // Calculate the deformed lengths of the area for measurement
        double changedLengthX = ((double)finalPosX[1] - (double)finalPosX[0]);
        double changedLengthY = ((double)finalPosY[1] - (double)finalPosY[0]);
        // Calculate the deformed length using pythagoras' theorem for the length of the hypotenuse of x and y
        double changedLength = sqrt(pow(changedLengthX, 2) + pow(changedLengthY, 2));
        // Calculate the strain
        strainValues[a].strain = (changedLength - origLength) / origLength;
        strainValues[a].depth = (int)yLoc[0];
    }

    double UpperLim = gtk_adjustment_get_value(GTK_ADJUSTMENT(adjUpperBound));
    double LowerLim = gtk_adjustment_get_value(GTK_ADJUSTMENT(adjLowerBound));

    // Verify the strain values within the limits specified by the user
    for(a = 0; a < numYCycles; a++)
    {
        // If the value of strain is greater than or less than the upper and lower limits, marker the point for correction
        if(strainValues[a].strain > UpperLim || strainValues[a].strain < LowerLim) { strainValues[a].correct = 0; }
        else { strainValues[a].correct = 1; }
    }

    // Form a regression equation based on the values known to be correct defined above
    double xy[numYCycles], xsq[numYCycles], ysq[numYCycles];
    for(a = 0; a < numYCycles; a++)
    {
        // Calculate the required values for the regression equation
        xy[a] = (strainValues[a].depth * strainValues[a].strain);
        xsq[a] = pow(strainValues[a].depth, 2.0);
        ysq[a] = pow(strainValues[a].strain, 2.0);
    }

    // Variables to store the values of the summations of values calculated above
    double sumx = 0;
    double sumy = 0;
    double sumxy = 0;
    double sumxsq = 0;
    double sumysq = 0;
    int count = 0;

    // Sum the values of the points wanted for autocorrection
    for(a = 0; a < numYCycles; a++)
    {
        if(strainValues[a].correct == 1)
        {
            sumx = sumx + strainValues[a].depth;
            sumy = sumy + strainValues[a].strain;
            sumxy = sumxy + xy[a];
            sumxsq = sumxsq + xsq[a];
            sumysq = sumysq + ysq[a];
            count++;
        }
    }

    // Calculate the a coefficient and the b coefficient of the dataset
    double acoeff = ((sumy * sumxsq) - (sumx * sumxy)) / ((count * (sumxsq)) - (pow(sumx, 2)));
    double bcoeff = ((count * sumxy) - (sumx * sumy)) / ((count * sumxsq) - pow(sumx, 2));

    // Get the physical depth and width from the user interface, and assign a scale factor thereto
    // These should be doubles, change to adjustment widgets (depreciated code)
    int physWidth = atoi(gtk_entry_get_text(GTK_ENTRY(txtPhysWidth)));
    int physHeight = atoi(gtk_entry_get_text(GTK_ENTRY(txtPhysHeight)));
    double scaleFactorWidth = physWidth / strainArea.width;
    double scaleFactorHeight = physHeight / strainArea.height;
    double scaleFactor = (scaleFactorWidth + scaleFactorHeight) / 2.0;

    // Correct the values and print the values to the output file
    for(a = 0; a < numYCycles; a++)
    {
        if(strainValues[a].correct == 0)
        {
            strainValues[a].strain = bcoeff * strainValues[a].depth + acoeff;
        }

        if(physWidth != 0 || physHeight != 0)
        {
            fprintf(outputWrite, "%d, %f\n", strainValues[a].depth, strainValues[a].strain);
        }
        else
        {
            fprintf(outputWrite, "%d, %f\n", strainValues[a].depth, strainValues[a].strain);
        }

    }

    // Close the output file
    fclose(outputWrite);

    // Hide the progress window
    gtk_widget_hide(GTK_WIDGET(progressWindow));

    // Release the images to free memory
    cvReleaseImage(&referenceImage);
    cvReleaseImage(&deformedImage);
    cvReleaseImage(&resultImage);

    // If the toggle button to show the output window is active, show it
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkShowOutputWindow)) == TRUE)
    {
        showOutputWindow(gtk_entry_get_text(GTK_ENTRY(txtOutputFile)));
    }
}

void InitializeDistributionMeasurement()
{
    if(countReferenceImages == 0)
    {
        prtError(CST_NO_REFERENCE_IMAGE);    // Error for if no reference image is loaded
        return;
    }
    if(countDeformedImages == 0)
    {
        prtError(CST_NO_DEFORMED_IMAGE);    // Error for if no deformed image is loaded
        return;
    }
    if(strainArea.width == 0 || strainArea.height == 0)
    {
        prtError(CST_NO_AREA_SET);    // Error for if area is incorrect or zero
        return;
    }

    // First hide the strain configuration window, we don't want this getting in the way
    HideDistributionConfiguration();

    // Cycle through the reference images
    int i = 0;

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listDstReferenceImages), &iterDstReferenceImages);
    for(i = 0; i < countReferenceImages; i++)
    {
        // Check if user wants this reference image to be processed
        gboolean process;
        gtk_tree_model_get(GTK_TREE_MODEL(listDstReferenceImages), &iterDstReferenceImages, CHECK_COLUMN, &process, -1);

        if(process == TRUE)
        {
            gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listDstDeformedImages), &iterDstDeformedImages);
            gtk_tree_model_get(GTK_TREE_MODEL(listDstDeformedImages), &iterDstDeformedImages, CHECK_COLUMN, &process, -1);

            int k = 0;
            for(k = 0; k < countDeformedImages; k++)
            {
                gtk_tree_model_get(GTK_TREE_MODEL(listDstDeformedImages), &iterDstDeformedImages, CHECK_COLUMN, &process, -1);
                if(process == TRUE)
                {
                    int l;
                    char *referenceImagePath;
                    char *deformedImagePath;

                    // Get the path for the reference image indexed by i
                    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
                    for(l = 0; l < i; l++)
                    {
                        gtk_tree_model_iter_next(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
                    }
                    gtk_tree_model_get(GTK_TREE_MODEL(referenceImages), &referenceImageIter, 0, &referenceImagePath, -1);

                    // Get the path for the image indexed by l
                    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(deformedImages), &deformedImageIter);
                    for(l = 0; l < k; l++)
                    {
                        gtk_tree_model_iter_next(GTK_TREE_MODEL(deformedImages), &deformedImageIter);
                    }
                    gtk_tree_model_get(GTK_TREE_MODEL(deformedImages), &deformedImageIter, 0, &deformedImagePath, -1);

                    printf("Reference Image: %d, Deformed Image: %d\n", i, k); // Debugging output
                    printf("Reference Image: %s\n", referenceImagePath); // Debugging output
                    printf("Deformed Image:  %s\n", deformedImagePath); // Debugging output
                    printf("\n"); // Debugging output
                    GenerateStrainDistribution(referenceImagePath, deformedImagePath, i, k);
                }
                gtk_tree_model_iter_next(GTK_TREE_MODEL(listDstDeformedImages), &iterDstDeformedImages);
            }
        }

        gtk_tree_model_iter_next(GTK_TREE_MODEL(listDstReferenceImages), &iterDstReferenceImages);
    }
}

void toggled_SubPixelAccuracy()
{
    gboolean toggled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chkSubPixelAcc));
    if(toggled == TRUE)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(cmbAccuracyLevel), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblAccuracyLevel), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(cmbInterpMethod), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblInterpolationMethod), TRUE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(cmbAccuracyLevel), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblAccuracyLevel), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(cmbInterpMethod), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblInterpolationMethod), FALSE);
    }
}

void toggled_ReferenceImage(GtkCellRendererToggle *toggle, gchar *path, GtkTreeView *treeView)
{
    GtkTreeIter iter;
    gboolean value;

    if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(listDstReferenceImages), &iter, path))
    {
        gtk_tree_model_get(GTK_TREE_MODEL(listDstReferenceImages), &iter, CHECK_COLUMN, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(listDstReferenceImages), &iter, CHECK_COLUMN, !value, -1);
    }
}

void toggled_DeformedImage(GtkCellRendererToggle *toggle, gchar *path, GtkTreeView *treeView)
{
    GtkTreeIter iter;
    gboolean value;

    if(gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(listDstDeformedImages), &iter, path))
    {
        gtk_tree_model_get(GTK_TREE_MODEL(listDstDeformedImages), &iter, CHECK_COLUMN, &value, -1);
        gtk_list_store_set(GTK_LIST_STORE(listDstDeformedImages), &iter, CHECK_COLUMN, !value, -1);
    }
}

void initialize_StrainConfigurationWindow()
{
    strainconfigWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(strainconfigWindow), "Strain Distribution Test Configuration");
    gtk_widget_set_size_request(GTK_WIDGET(strainconfigWindow), -1, -1);
    gtk_window_set_resizable(GTK_WINDOW(strainconfigWindow), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(strainconfigWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(strainconfigWindow), TRUE);

    GtkWidget *settingsTable = gtk_table_new(3, 3, FALSE);

    GtkWidget *lblReferenceImages = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lblReferenceImages), "<b>Reference Images</b>");
    GtkWidget *lblDeformedImages = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lblDeformedImages), "<b>Deformed Images</b>");
    gtk_widget_show(lblReferenceImages);
    gtk_widget_show(lblDeformedImages);

    GtkTreeViewColumn *column0, *column1, *column3, *column4;
    GtkCellRenderer *render0, *render1, *render3, *render4;

    listDstReferenceImages = gtk_list_store_new(2, G_TYPE_BOOLEAN, G_TYPE_STRING);
    GtkWidget *tvReferenceImages = gtk_tree_view_new_with_model(GTK_TREE_MODEL(listDstReferenceImages));
    gtk_widget_set_size_request(GTK_WIDGET(tvReferenceImages), 250, 175);

    // Create the cell renderer and the column for the check box
    render1 = gtk_cell_renderer_toggle_new();
    g_signal_connect(G_OBJECT(render1), "toggled", G_CALLBACK(toggled_ReferenceImage), NULL);
    column1 = gtk_tree_view_column_new_with_attributes ("Process", render1, "active", CHECK_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvReferenceImages), GTK_TREE_VIEW_COLUMN(column1));

    // Create the cell renderer and the column for the image
    render0 = gtk_cell_renderer_text_new();
    column0 = gtk_tree_view_column_new_with_attributes ("Image", render0, "text", IMAGE_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvReferenceImages), GTK_TREE_VIEW_COLUMN(column0));

    gtk_widget_show(tvReferenceImages);

    listDstDeformedImages = gtk_list_store_new(2, G_TYPE_BOOLEAN, G_TYPE_STRING);
    GtkWidget *tvDeformedImages = gtk_tree_view_new_with_model(GTK_TREE_MODEL(listDstDeformedImages));
    gtk_widget_set_size_request(GTK_WIDGET(tvDeformedImages), 250, 175);

    // Create the cell renderer and the column for the check box
    render4 = gtk_cell_renderer_toggle_new();
    g_signal_connect(G_OBJECT(render4), "toggled", G_CALLBACK(toggled_DeformedImage), NULL);
    column4 = gtk_tree_view_column_new_with_attributes ("Process", render4, "active", CHECK_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvDeformedImages), GTK_TREE_VIEW_COLUMN(column4));

    // Create the cell renderer and the column for the image
    render3 = gtk_cell_renderer_text_new();
    column3 = gtk_tree_view_column_new_with_attributes ("Image", render3, "text", IMAGE_COLUMN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tvDeformedImages), GTK_TREE_VIEW_COLUMN(column3));

    gtk_widget_show(tvDeformedImages);

    gtk_widget_show(settingsTable);

    GtkWidget *cmbMeasurementType = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMeasurementType), "Horizontal Measurement");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbMeasurementType), "Vertical Measurement");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbMeasurementType), 0);
    GtkWidget *lblMeasurementType = gtk_label_new("Measurement Type:");
    GtkWidget *algMeasurementType = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algMeasurementType));
    gtk_container_add(GTK_CONTAINER(algMeasurementType), GTK_WIDGET(lblMeasurementType));
    gtk_widget_show(GTK_WIDGET(lblMeasurementType));
    gtk_widget_show(GTK_WIDGET(cmbMeasurementType));

    subsetSizeAdjustment = gtk_adjustment_new(25, 1, 100, 1, 10, 0.0);
    GtkWidget *subsetSizeSpin = gtk_spin_button_new(GTK_ADJUSTMENT(subsetSizeAdjustment), 1.0, 0);
    GtkWidget *lblSubsetSize = gtk_label_new("Subset Size:");
    GtkWidget *algSubsetSize = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algSubsetSize));
    gtk_container_add(GTK_CONTAINER(algSubsetSize), GTK_WIDGET(lblSubsetSize));
    gtk_widget_show(GTK_WIDGET(subsetSizeSpin));
    gtk_widget_show(GTK_WIDGET(lblSubsetSize));

    GtkWidget *lblPixels = gtk_label_new("pixels");
    gtk_widget_show(lblPixels);

    subsetStepAdjustment = gtk_adjustment_new(25, 1, 999, 1, 10, 0.0);
    GtkWidget *subsetStepSpin = gtk_spin_button_new(GTK_ADJUSTMENT(subsetStepAdjustment), 1.0, 0);
    GtkWidget *lblSubsetStep = gtk_label_new("Subset Step:");
    GtkWidget *algSubsetStep = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algSubsetStep));
    gtk_container_add(GTK_CONTAINER(algSubsetStep), GTK_WIDGET(lblSubsetStep));
    gtk_widget_show(GTK_WIDGET(subsetStepSpin));
    gtk_widget_show(GTK_WIDGET(lblSubsetStep));

    GtkWidget *lblPixels2 = gtk_label_new("pixels");
    gtk_widget_show(lblPixels2);

    chkSubPixelAcc = gtk_check_button_new_with_label("Enable Sub-Pixel Accuracy");
    gtk_widget_show(chkSubPixelAcc);
    g_signal_connect(G_OBJECT(chkSubPixelAcc), "toggled", G_CALLBACK(toggled_SubPixelAccuracy), NULL);

    cmbAccuracyLevel = gtk_combo_box_text_new();
    gtk_widget_show(GTK_WIDGET(cmbAccuracyLevel));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbAccuracyLevel), "Pixel/10");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbAccuracyLevel), "Pixel/100");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbAccuracyLevel), "Pixel/1000");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbAccuracyLevel), 0);
    lblAccuracyLevel = gtk_label_new("Accuracy Level:");
    GtkWidget *algAccuracyLevel = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algAccuracyLevel));
    gtk_container_add(GTK_CONTAINER(algAccuracyLevel), GTK_WIDGET(lblAccuracyLevel));
    gtk_widget_show(GTK_WIDGET(lblAccuracyLevel));
    gtk_widget_set_sensitive(GTK_WIDGET(cmbAccuracyLevel), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblAccuracyLevel), FALSE);

    cmbInterpMethod = gtk_combo_box_text_new();
    gtk_widget_show(GTK_WIDGET(cmbInterpMethod));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbInterpMethod), "Nearest Neighbour");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbInterpMethod), "Linear");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbInterpMethod), "Area");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbInterpMethod), "Bicubic");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbInterpMethod), 3);
    lblMapInterpolationMethod = gtk_label_new("Interpolation Method:");
    GtkWidget *algInterpolationMethod = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algInterpolationMethod));
    gtk_container_add(GTK_CONTAINER(algInterpolationMethod), GTK_WIDGET(lblMapInterpolationMethod));
    gtk_widget_show(GTK_WIDGET(lblMapInterpolationMethod));
    gtk_widget_set_sensitive(GTK_WIDGET(cmbInterpMethod), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblMapInterpolationMethod), FALSE);

    char *homePath = getenv("HOME");
    char copyHomePath[strlen(homePath) + 1];
    strcpy(copyHomePath, homePath);
    strncat(copyHomePath, "/output.dat", 11);

    selOutputFile = gtk_button_new_with_label("Select");
    gtk_widget_show(GTK_WIDGET(selOutputFile));
    g_signal_connect(G_OBJECT(selOutputFile), "clicked", G_CALLBACK(SelectOutputFile), NULL);
    txtOutputFile = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtOutputFile), copyHomePath);
    gtk_widget_show(GTK_WIDGET(txtOutputFile));
    GtkWidget *lblOutputFile = gtk_label_new("Output File:");
    gtk_widget_show(GTK_WIDGET(lblOutputFile));
    GtkWidget *algOutputFile = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algOutputFile));
    gtk_container_add(GTK_CONTAINER(algOutputFile), GTK_WIDGET(lblOutputFile));
    GtkWidget *hboxOutput = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hboxOutput));
    gtk_box_pack_start(GTK_BOX(hboxOutput), GTK_WIDGET(txtOutputFile), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hboxOutput), GTK_WIDGET(selOutputFile), FALSE, FALSE, 0);

    chkShowOutputWindow = gtk_check_button_new_with_label("Show Output Window");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chkShowOutputWindow), TRUE);
    gtk_widget_show(GTK_WIDGET(chkShowOutputWindow));

    GtkWidget *lblAutoCorrect = gtk_label_new("Auto Correction Limits");
    GtkWidget *algAutoCorrect = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algAutoCorrect), GTK_WIDGET(lblAutoCorrect));
    gtk_widget_show(GTK_WIDGET(lblAutoCorrect));
    gtk_widget_show(GTK_WIDGET(algAutoCorrect));

    adjUpperBound = gtk_adjustment_new(0.1, 0.001, 1.0, 0.01, 0.1, 0.0);
    GtkWidget *spnUpperBound = gtk_spin_button_new(GTK_ADJUSTMENT(adjUpperBound), 0.01, 3);
    gtk_widget_show(GTK_WIDGET(spnUpperBound));
    GtkWidget *lblUpperBound = gtk_label_new("Upper:");
    gtk_widget_show(GTK_WIDGET(lblUpperBound));

    adjLowerBound = gtk_adjustment_new(-0.1, -1.0, -0.001, 0.01, 0.1, 0.0);
    GtkWidget *spnLowerBound = gtk_spin_button_new(GTK_ADJUSTMENT(adjLowerBound), 0.01, 3);
    gtk_widget_show(GTK_WIDGET(spnLowerBound));
    GtkWidget *lblLowerBound = gtk_label_new("Lower:");
    gtk_widget_show(GTK_WIDGET(lblLowerBound));

    GtkWidget *hboxAutoCorrect = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hboxAutoCorrect), GTK_WIDGET(lblLowerBound), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxAutoCorrect), GTK_WIDGET(spnLowerBound), TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxAutoCorrect), GTK_WIDGET(lblUpperBound), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxAutoCorrect), GTK_WIDGET(spnUpperBound), TRUE, FALSE, 0);
    gtk_widget_show(GTK_WIDGET(hboxAutoCorrect));

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblReferenceImages),  0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblDeformedImages),   1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(tvReferenceImages),   0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(tvDeformedImages),    1, 2, 1, 2, GTK_FILL, GTK_FILL, 12, 0);

    GtkWidget *settingsTable2 = gtk_table_new(3, 3, FALSE);
    gtk_widget_show(GTK_WIDGET(settingsTable2));

    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algMeasurementType),     0, 1, 0, 1, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(cmbMeasurementType),     1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algSubsetSize),          0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(subsetSizeSpin),         1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(lblPixels),              2, 3, 1, 2, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algSubsetStep),          0, 1, 3, 4, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(subsetStepSpin),         1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(lblPixels2),             2, 3, 3, 4, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(chkSubPixelAcc),         1, 2, 4, 5, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algAccuracyLevel),       0, 1, 5, 6, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(cmbAccuracyLevel),       1, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algInterpolationMethod), 0, 1, 6, 7, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(cmbInterpMethod),        1, 2, 6, 7, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algAutoCorrect),         0, 1, 7, 8, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(hboxAutoCorrect),        1, 2, 7, 8, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(algOutputFile),          0, 1, 8, 9, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(hboxOutput),             1, 2, 8, 9, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable2), GTK_WIDGET(chkShowOutputWindow),    1, 2, 9, 10, GTK_FILL, GTK_FILL, 0, 0);

    gtk_table_set_row_spacings(GTK_TABLE(settingsTable), 12);
    gtk_table_set_row_spacings(GTK_TABLE(settingsTable2), 6);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(vbox));

    GtkWidget *hbox2 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox2));
    GtkWidget *btnProcess, *btnClose;
    btnProcess = gtk_button_new_with_label("Process Images");
    gtk_widget_show(GTK_WIDGET(btnProcess));
    btnClose = gtk_button_new_with_label("Close");
    gtk_widget_show(GTK_WIDGET(btnClose));
    GtkWidget *algButtons = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algButtons));
    gtk_container_add(GTK_CONTAINER(algButtons), GTK_WIDGET(hbox2));

    gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(btnClose), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(btnProcess), FALSE, FALSE, 12);

    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(settingsTable),    FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(settingsTable2),   FALSE, FALSE, 6);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(algButtons),       FALSE, FALSE, 12);

    gtk_container_add(GTK_CONTAINER(strainconfigWindow), GTK_WIDGET(vbox));

    g_signal_connect(G_OBJECT(strainconfigWindow), "delete_event", G_CALLBACK(HideDistributionConfiguration), NULL);
    g_signal_connect(G_OBJECT(btnClose), "clicked", G_CALLBACK(HideDistributionConfiguration), NULL);
    g_signal_connect(G_OBJECT(btnProcess), "clicked", G_CALLBACK(InitializeDistributionMeasurement), NULL);
}
