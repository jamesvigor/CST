

#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "guielements.h"
#include "constants.h"
#include "doerrors.h"

void loadImage(int IDENTIFIER)
{
    char *buffer;
    switch(IDENTIFIER)
    {
        case REFERENCE_IMAGE:
            buffer = "Load a Reference Image";
            break;
        case DEFORMED_IMAGE:
            buffer = "Load a Deformed Image";
            break;
    }

    GtkWidget *loadImageDialog = gtk_file_chooser_dialog_new(buffer, GTK_WINDOW(mainWindow), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

    // Stick some MIME type stuff in the dialog so the user may only open an image.
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "Supported Image Files");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/png");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/jpeg");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/tiff");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/gif");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/bmp");

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(loadImageDialog), GTK_FILE_FILTER(filter));

    // Set the file load dialog to modal so no other dialog may be opened
    gtk_window_set_modal(GTK_WINDOW(loadImageDialog), TRUE);

    // Initialize the file path to NULL
    gchar *filepath = NULL;

    // If the dialog_run was successful, complete the following
    if(gtk_dialog_run(GTK_DIALOG(loadImageDialog)) == GTK_RESPONSE_ACCEPT)
    {
        // Retrieve the file path
        filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(loadImageDialog));

        // Check for a valid image
        IplImage *image = cvLoadImage(filepath, CV_LOAD_IMAGE_ANYCOLOR);
        if(image == 0)
        {
            // Image failed to load, print the error CV_FAILED_LOAD
            prtError(CV_FAILED_LOAD);
            gtk_widget_destroy(GTK_WIDGET(loadImageDialog));
            cvReleaseImage(&image);
            return;
        }

        // Free the memory
        cvReleaseImage(&image);

        // If we are working with a reference Image:
        if(IDENTIFIER == REFERENCE_IMAGE)
        {
            // Append the current image to the list store of images.
            gtk_list_store_append(GTK_LIST_STORE(referenceImages), &referenceImageIter);
            gtk_list_store_set(GTK_LIST_STORE(referenceImages), &referenceImageIter, 0, filepath, -1);

            if(countReferenceImages == 0)
            {
                // Display the reference image
                currentImage = gdk_pixbuf_new_from_file(filepath, NULL);
                gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imageView), currentImage, TRUE);
                char message[500];
                sprintf(message, "Currently displaying image: %s", filepath);
                gtk_statusbar_push(GTK_STATUSBAR(stbMainStatusBar), 0, message);

                // Wipe the contents of the list store containing the data
                GtkTreeModel *store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbSelectImage));
                gtk_list_store_clear(GTK_LIST_STORE(store));
            }

            char string[50];
            sprintf(string, "Reference Image %d", countReferenceImages + 1);
            gtk_combo_box_append_text(GTK_COMBO_BOX(cmbSelectImage), string);

            if(countReferenceImages == 0)
            {
                gtk_combo_box_set_active(GTK_COMBO_BOX(cmbSelectImage), 0);
            }

            countReferenceImages++; // Increase the count of reference images
        }
        else if(IDENTIFIER == DEFORMED_IMAGE)
        {
            // Append the curent imasge to the list store of images.
            gtk_list_store_append(GTK_LIST_STORE(deformedImages), &deformedImageIter);
            gtk_list_store_set(GTK_LIST_STORE(deformedImages), &deformedImageIter, 0, filepath, -1);

            countDeformedImages++; // Increment the image quantity
        }
    }

    printf("Reference Images: %d, Deformed Images: %d\n", countReferenceImages, countDeformedImages);

    gtk_widget_destroy(GTK_WIDGET(loadImageDialog));
    return;
}

void loadReferenceImage() { loadImage(REFERENCE_IMAGE); }
void loadDeformedImage()  { loadImage(DEFORMED_IMAGE);  }
