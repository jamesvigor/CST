#include <gtk/gtk.h>
#include <stdlib.h>

#include "guielements.h"
#include "constants.h"

int prtError(int ERROR_TYPE)
{
    GtkWidget *errorDialog;

    switch(ERROR_TYPE)
    {
        case CV_FAILED_LOAD:
            errorDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "A non-fatal error occured...");
            gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(errorDialog), "The specified image failed to load.\n\nPlease ensure the selected image is valid, non-corrupted and exists on the file system.\n\nIf the error persists, try with another image or re-acquire the image from the capture device.");
            break;
        case CST_NO_CAMERA_PRESENT:
            errorDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Warning...");
            gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(errorDialog), "CST cold not detect a camera or suitable capture device connected to your computer.\n\nPlease ensure that the camera is connected, switched on and in PTP mode.");
            break;
        case CST_NO_REFERENCE_IMAGE:
            errorDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "A non-fatal error occured...");
            gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(errorDialog), "No reference images have been loaded for analysis.\n\nIn order to ensure successful processing, a reference image and deformed image must be loaded from your computer or captured from a device.\n\nPlease load the image(s) and try again.");
            break;
        case CST_NO_DEFORMED_IMAGE:
            errorDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "A non-fatal error occured...");
            gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(errorDialog), "No deformed images have been loaded for analysis.\n\nIn order to ensure successful processing, a reference image and deformed image must be loaded from your computer or captured from a device.\n\nPlease load the image(s) and try again.");
            break;
        case CST_NO_AREA_SET:
            errorDialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "A non-fatal error occured...");
            gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(errorDialog), "A measurement area has not been set, or has been incorrectly set.\n\nPlease set or verify the measurement area and try again.");
            break;
    }
    gtk_dialog_run(GTK_DIALOG(errorDialog));
    gtk_widget_destroy(GTK_WIDGET(errorDialog));
    return 0;
}
