#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "guielements.h"
#include "constants.h"
#include "doerrors.h"

typedef struct
{
    int origXLoc;
    int origYLoc;
    double defoXLoc;
    double defoYLoc;
} referencePoint;

static GdkRectangle exclusionzone;
static GtkWidget *pointconfigWindow;
static GtkWidget *txtMinExclX, *txtMinExclY;
static GtkWidget *txtMaxExclX, *txtMaxExclY;
static GtkWidget *cmbImage;
static GtkObject *adjPointCount;
static GtkObject *adjPointDistance;
static GdkPixbuf *referenceImage;

static int countReferencePoints;

void process_ReferencePoints()
{
    // For debugging output, print the number of reference points for processing
    printf("Number of Points for Processing: %d\n", countReferencePoints);

    // Get the active image from the combo box for processing
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbSelectImage));

    // i will be used for iteration throughout the function
    int i = 0;
    // Get the first iter from the tree model storing the reference images
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(referenceImages), &referenceImageIter);

    // Iterate through the reference images, increment the iter each time unti the desired reference image is sought
    for(i = 0; i < index; i++)
    {
        gtk_tree_model_iter_next(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
    }

    char *imagePath; // For storage of the path of the desired image
    // Get the value from the tree model for the current iteration.
    gtk_tree_model_get(GTK_TREE_MODEL(referenceImages), &referenceImageIter, 0, &imagePath, -1);

    // Debugging output for the current image, remove this
    printf("Processing Image: %s\n", imagePath);

    // Load the images into memory using the opencv library
    IplImage *referenceImage = cvLoadImage(imagePath, CV_LOAD_IMAGE_GRAYSCALE);
    // IplImage *deformedImage  = cvLoadImage(imagePath, CV_LOAD_IMAGE_G)

    int subsetSize = 50; // Arbitrary value for the size of a subset, take this from the user interface when completed
    // int resultWidth = image->width

    // Create a map of reference points based on the number of reference points and the number of deformed images
    referencePoint pointMap[countReferencePoints][countDeformedImages];

    // Iterate through the reference points
    for(i = 0; i < countReferencePoints; i++)
    {
        // Debugging output for the current point, remove this
        printf("Processing Reference Point: %d\n", i);
        // IplImage *resultImage =


    }

    cvReleaseImage(&referenceImage);

}

void show_ReferencePointAutoWindow()
{
    if(countReferenceImages != 0)
    {
        GtkListStore *tmpListStore = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(cmbImage)));
        gtk_list_store_clear(GTK_LIST_STORE(tmpListStore));
        countReferencePoints = 0;
    }

    int i = 0;
    for(i = 0; i < countReferenceImages; i++)
    {
        char buffer[100];
        sprintf(buffer, "Reference Image %d", i);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbImage), buffer);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbImage), 0);
    if(countReferenceImages != 0)
    {
        gtk_image_view_get_tool(GTK_IMAGE_VIEW(imageView));
        gtk_image_view_set_tool(GTK_IMAGE_VIEW(imageView), GTK_IIMAGE_TOOL(exclusionSelector));
        gtk_image_tool_selector_set_selection(GTK_IMAGE_TOOL_SELECTOR(exclusionSelector), &exclusionzone);
    }
    gtk_widget_show(GTK_WIDGET(pointconfigWindow));
}

void hide_ReferencePointAutoWindow()
{
    GtkIImageTool *dragTool = gtk_image_tool_dragger_new(GTK_IMAGE_VIEW(imageView));
    gtk_image_view_set_tool(GTK_IMAGE_VIEW(imageView), GTK_IIMAGE_TOOL(dragTool));
    gtk_widget_hide(GTK_WIDGET(pointconfigWindow));
}

void updateArea()
{
    exclusionzone.x = atoi(gtk_entry_get_text(GTK_ENTRY(txtMinExclX)));
    exclusionzone.y = atoi(gtk_entry_get_text(GTK_ENTRY(txtMinExclY)));
    exclusionzone.width = atoi(gtk_entry_get_text(GTK_ENTRY(txtMaxExclX))) - atoi(gtk_entry_get_text(GTK_ENTRY(txtMinExclX)));
    exclusionzone.height = atoi(gtk_entry_get_text(GTK_ENTRY(txtMaxExclY))) - atoi(gtk_entry_get_text(GTK_ENTRY(txtMinExclY)));;
    gtk_image_tool_selector_set_selection(GTK_IMAGE_TOOL_SELECTOR(exclusionSelector), &exclusionzone);
}

void exclusionSelectionChanged()
{
    gtk_image_tool_selector_get_selection(GTK_IMAGE_TOOL_SELECTOR(exclusionSelector), &exclusionzone);

    char num[10];
    sprintf(num, "%d", exclusionzone.x);
    gtk_entry_set_text(GTK_ENTRY(txtMinExclX), num);
    sprintf(num, "%d", exclusionzone.y);
    gtk_entry_set_text(GTK_ENTRY(txtMinExclY), num);
    sprintf(num, "%d", exclusionzone.x + exclusionzone.width);
    gtk_entry_set_text(GTK_ENTRY(txtMaxExclX), num);
    sprintf(num, "%d", exclusionzone.y + exclusionzone.height);
    gtk_entry_set_text(GTK_ENTRY(txtMaxExclY), num);
}

void processImage()
{
    // If no reference image has been loaded, print an error and return to the user interface
    if(countReferenceImages == 0)
    {
        prtError(CST_NO_REFERENCE_IMAGE);
        return;
    }

    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbImage));

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(referenceImages), &referenceImageIter);

    int i = 0;
    for(i = 0; i < index; i++)
    {
        gtk_tree_model_iter_next(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
    }

    char *imagePath;
    gtk_tree_model_get(GTK_TREE_MODEL(referenceImages), &referenceImageIter, 0, &imagePath, -1);

    IplImage *imageForProcess = cvLoadImage(imagePath, CV_LOAD_IMAGE_GRAYSCALE);
    IplImage *colourImage = cvLoadImage(imagePath, CV_LOAD_IMAGE_ANYCOLOR);
    IplImage *eigImage = cvCreateImage(cvSize(imageForProcess->width, imageForProcess->height), IPL_DEPTH_32F, 1);
    IplImage *tmpImage = cvCreateImage(cvSize(imageForProcess->width, imageForProcess->height), IPL_DEPTH_32F, 1);

    int minXExclusion = atoi(gtk_entry_get_text(GTK_ENTRY(txtMinExclX)));
    int minYExclusion = atoi(gtk_entry_get_text(GTK_ENTRY(txtMinExclY)));
    int maxXExclusion = atoi(gtk_entry_get_text(GTK_ENTRY(txtMaxExclX)));
    int maxYExclusion = atoi(gtk_entry_get_text(GTK_ENTRY(txtMaxExclY)));

    int PointDistance = gtk_adjustment_get_value(GTK_ADJUSTMENT(adjPointDistance));
    int MaxNumCorners = gtk_adjustment_get_value(GTK_ADJUSTMENT(adjPointCount));
    CvPoint2D32f corners[MaxNumCorners];

    cvGoodFeaturesToTrack(imageForProcess, eigImage, tmpImage, corners, &MaxNumCorners, 0.1, PointDistance, 0, 3, 0, 0.04);

    gtk_list_store_clear(GTK_LIST_STORE(referencePoints));

    for(i = 0; i < MaxNumCorners; i++)
    {
        if((corners[i].x > minXExclusion && corners[i].y > minYExclusion) && (corners[i].x < maxXExclusion && corners[i].y < maxYExclusion))
        {
            continue;
        }

        CvPoint center = cvPoint(corners[i].x, corners[i].y);
        CvScalar colour = cvScalar(0, 255, 255, 255);
        cvCircle(colourImage, center, 5, colour, 3, 8, 0);
        gtk_list_store_append(GTK_LIST_STORE(referencePoints), &referencePointsIter);
        gtk_list_store_set(GTK_LIST_STORE(referencePoints), &referencePointsIter, 0, corners[i].x, 1, corners[i].y, -1);
        countReferencePoints++;
    }

    cvCvtColor(colourImage, colourImage, CV_BGR2RGB);

    referenceImage = gdk_pixbuf_new_from_data((guchar*)colourImage->imageData, GDK_COLORSPACE_RGB, FALSE, colourImage->depth, colourImage->width, colourImage->height, colourImage->widthStep,NULL, NULL);

    gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imageView), GDK_PIXBUF(referenceImage), 0);

    cvReleaseImage(&eigImage);
    cvReleaseImage(&tmpImage);
    cvReleaseImage(&imageForProcess);
}

void initialize_ReferencePointAutoWindow()
{
    // Configure the main window widget, size automatic, type hint utility, skipping the taskbar, not resizable
    pointconfigWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(pointconfigWindow), "Automatic Reference Point Detection");
    gtk_widget_set_size_request(GTK_WIDGET(pointconfigWindow), -1, -1);
    gtk_window_set_resizable(GTK_WINDOW(pointconfigWindow), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(pointconfigWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(pointconfigWindow), TRUE);

    GtkWidget *settingsTable = gtk_table_new(3, 3, FALSE);
    gtk_widget_show(GTK_WIDGET(settingsTable));

    GtkWidget *lblImage = gtk_label_new("Image:");
    gtk_widget_show(GTK_WIDGET(lblImage));
    GtkWidget *algImage = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algImage));
    gtk_container_add(GTK_CONTAINER(algImage), GTK_WIDGET(lblImage));

    cmbImage = gtk_combo_box_new_text();
    gtk_combo_box_append_text(GTK_COMBO_BOX(cmbImage), "No Image Loaded");
    gtk_widget_show(GTK_WIDGET(cmbImage));

    GtkWidget *lblPointCount = gtk_label_new("Maximum Point Count:");
    gtk_widget_show(GTK_WIDGET(lblPointCount));
    GtkWidget *algPointCount = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algPointCount));
    gtk_container_add(GTK_CONTAINER(algPointCount), GTK_WIDGET(lblPointCount));

    adjPointCount = gtk_adjustment_new(50, 1, 999, 1, 10, 0.0);
    GtkWidget *spnPointCount = gtk_spin_button_new(GTK_ADJUSTMENT(adjPointCount), 1.0, 0.0);
    gtk_widget_show(GTK_WIDGET(spnPointCount));

    GtkWidget *lblPointDistance = gtk_label_new("Minimum Point Distance:");
    gtk_widget_show(GTK_WIDGET(lblPointDistance));
    GtkWidget *algPointDistance = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algPointDistance));
    adjPointDistance = gtk_adjustment_new(10, 1, 99, 1, 10, 0.0);
    GtkWidget *spnPointDistance = gtk_spin_button_new(GTK_ADJUSTMENT(adjPointDistance), 1.0, 0.0);
    gtk_widget_show(GTK_WIDGET(spnPointDistance));
    gtk_container_add(GTK_CONTAINER(algPointDistance), GTK_WIDGET(lblPointDistance));
    GtkWidget *lblPixels = gtk_label_new("pixels");
    gtk_widget_show(GTK_WIDGET(lblPixels));

    GtkWidget *lblExclusionZone = gtk_label_new("Exclusion Zone, from:");
    gtk_widget_show(GTK_WIDGET(lblExclusionZone));
    GtkWidget *algExclusionZone = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algExclusionZone));
    gtk_container_add(GTK_CONTAINER(algExclusionZone), GTK_WIDGET(lblExclusionZone));

    txtMinExclX = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtMinExclX));
    gtk_entry_set_text(GTK_ENTRY(txtMinExclX), "0000");
    txtMinExclY = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtMinExclY));
    gtk_entry_set_text(GTK_ENTRY(txtMinExclY), "0000");
    GtkWidget *lblComma = gtk_label_new(",");
    gtk_widget_show(GTK_WIDGET(lblComma));
    GtkWidget *hbox = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox));
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(txtMinExclX), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(lblComma), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(txtMinExclY), FALSE, FALSE, 0);

    GtkWidget *lblExclusionZoneTo = gtk_label_new("to:");
    gtk_widget_show(GTK_WIDGET(lblExclusionZoneTo));
    GtkWidget *algExclusionZoneTo = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algExclusionZoneTo));
    gtk_container_add(GTK_CONTAINER(algExclusionZoneTo), GTK_WIDGET(lblExclusionZoneTo));

    txtMaxExclX = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtMaxExclX));
    gtk_entry_set_text(GTK_ENTRY(txtMaxExclX), "0000");
    txtMaxExclY = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtMaxExclY));
    gtk_entry_set_text(GTK_ENTRY(txtMaxExclY), "0000");
    GtkWidget *lblComma2 = gtk_label_new(",");
    gtk_widget_show(GTK_WIDGET(lblComma2));
    GtkWidget *hbox2 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox2));
    gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(txtMaxExclX), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(lblComma2), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(txtMaxExclY), FALSE, FALSE, 0);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algImage),             0, 1, 0, 1, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbImage),             1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algPointCount),        0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(spnPointCount),        1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algPointDistance),     0, 1, 2, 3, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(spnPointDistance),     1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblPixels),            2, 3, 2, 3, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algExclusionZone),     0, 1, 3, 4, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(hbox),                 1, 2, 3, 4, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algExclusionZoneTo),   0, 1, 4, 5, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(hbox2),                1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);

    gtk_table_set_row_spacings(GTK_TABLE(settingsTable), 5);

    GtkWidget *btnProcess = gtk_button_new_with_label("Process");
    GtkWidget *btnClose = gtk_button_new_with_label("Close");
    gtk_widget_show(GTK_WIDGET(btnProcess));
    gtk_widget_show(GTK_WIDGET(btnClose));
    GtkWidget *hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(hbox1));

    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnClose), FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnProcess), FALSE, FALSE, 12);

    GtkWidget *algButtons = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algButtons));
    gtk_container_add(GTK_CONTAINER(algButtons), GTK_WIDGET(hbox1));

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(vbox));
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(settingsTable), FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(algButtons), FALSE, FALSE, 12);

    gtk_container_add(GTK_CONTAINER(pointconfigWindow), GTK_WIDGET(vbox));

    g_signal_connect(G_OBJECT(pointconfigWindow), "delete_event", G_CALLBACK(hide_ReferencePointAutoWindow), NULL);
    g_signal_connect(G_OBJECT(btnClose), "clicked", G_CALLBACK(hide_ReferencePointAutoWindow), NULL);
    g_signal_connect(G_OBJECT(btnProcess), "clicked", G_CALLBACK(processImage), NULL);
}
