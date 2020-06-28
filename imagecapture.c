#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gphoto2/gphoto2.h>

#include "guielements.h"
#include "doerrors.h"
#include "constants.h"

static GdkPixbuf *previewImage;

void captureImage()
{
    Camera *camera;
    gp_camera_new(&camera);
    GPContext *context = gp_context_new();

    if(gp_camera_init(camera, context) != 0)
    {
        printf("CAMERA REMOVED!!!");
    }

    CameraWidget *rootconfiguration, *childconfiguration, *subchildconfiguration;
    CameraWidget *actualRootConfig = rootconfiguration;
    char *isoSetting;
    isoSetting = gtk_combo_box_get_active_text(GTK_COMBO_BOX(cmbISO));
    printf("%s\n", isoSetting);
    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "imgsettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "iso", &subchildconfiguration);
    gp_widget_set_value(subchildconfiguration, &isoSetting);
    gp_camera_set_config(camera, actualRootConfig, context);
    printf("%s\n", isoSetting);

    // Free the camera from the tyranny.
    gp_camera_exit(camera, context);
    gp_camera_free(camera);
}


int detectCamera()
{
    GtkTreeModel *store; // For pseudo clearing

    // Create a new camera object and initialize this.
    Camera *camera;
    gp_camera_new(&camera);
    GPContext *context = gp_context_new();
    int number, i;

    // Initialize the physical camera unit
    if(gp_camera_init(camera, context) != 0)
    {
        return 0;
    }

    // Get the camera configuration
    CameraWidget *rootconfiguration, *childconfiguration, *subchildconfiguration;

    // Get the ISO Setting (film speed)
    const char *isoSetting;
    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "imgsettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "iso", &subchildconfiguration);
    gp_widget_get_value(subchildconfiguration, &isoSetting);

    // Get all the ISO Settings and Fill a combo box with these values.
    number = gp_widget_count_choices(subchildconfiguration);
    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbISO));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);

        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbISO), value);

        // Set the combo box to the current value based on the returned strings (very hacky)
        int res = strncmp(value, isoSetting, 10);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbISO), i);
        }
    }

    // Get the shutter speed settring
    const char *shutterSetting;

    childconfiguration = NULL;
    subchildconfiguration = NULL;

    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "capturesettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "shutterspeed", &subchildconfiguration);
    gp_widget_get_value(subchildconfiguration, &shutterSetting);

    number = gp_widget_count_choices(subchildconfiguration);

    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbShutterSpeed));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbShutterSpeed), value);
        int res = strncmp(value, shutterSetting, 10);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbShutterSpeed), i);
        }
    }


    // Get the aperture settings from the camera
    const char *apertureSetting;

    childconfiguration = NULL;
    subchildconfiguration = NULL;

    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "capturesettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "aperture", &subchildconfiguration);

    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbAperture));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    gp_widget_get_value(subchildconfiguration, &apertureSetting);

    number = gp_widget_count_choices(subchildconfiguration);

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbAperture), value);
        int res = strncmp(value, apertureSetting, 10);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbAperture), i);
        }
    }

    // Get the white balance settings from the camera
    const char *whitebalanceSetting;

    childconfiguration = NULL;
    subchildconfiguration = NULL;

    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "imgsettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "whitebalance", &subchildconfiguration);
    gp_widget_get_value(subchildconfiguration, &whitebalanceSetting);

    number = gp_widget_count_choices(subchildconfiguration);

    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbWhiteBalance));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbWhiteBalance), value);
        int res = strncmp(value, whitebalanceSetting, 50);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbWhiteBalance), i);
        }
    }

    // Get the ecxposure compensation value from the camera
    const char *exposurecompensationSetting;
    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "capturesettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "exposurecompensation", &subchildconfiguration);
    gp_widget_get_value(subchildconfiguration, &exposurecompensationSetting);

    number = gp_widget_count_choices(subchildconfiguration);

    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbExposureCompensation));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbExposureCompensation), value);
        int res = strncmp(value, exposurecompensationSetting, 50);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbExposureCompensation), i);
        }
    }

    // Get the ecxposure compensation value from the camera
    const char *meteringModeSetting;
    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "capturesettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "meteringmode", &subchildconfiguration);
    gp_widget_get_value(subchildconfiguration, &meteringModeSetting);

    number = gp_widget_count_choices(subchildconfiguration);

    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbMeteringMode));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbMeteringMode), value);
        int res = strncmp(value, meteringModeSetting, 50);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbMeteringMode), i);
        }
    }

    // Get the image output value from the camera
    const char *imageoutputSetting;
    gp_camera_get_config(camera, &rootconfiguration, context);
    gp_widget_get_child_by_name(rootconfiguration, "imgsettings", &childconfiguration);
    gp_widget_get_child_by_name(childconfiguration, "imageformat", &subchildconfiguration);
    gp_widget_get_value(subchildconfiguration, &imageoutputSetting);

    number = gp_widget_count_choices(subchildconfiguration);

    // Free and clear the list if one already exists.
    store = gtk_combo_box_get_model(GTK_COMBO_BOX(cmbImageFormat));
    gtk_list_store_clear(GTK_LIST_STORE(store));

    for(i = 0; i < number; i++)
    {
        const char *value;
        gp_widget_get_choice(subchildconfiguration, i, &value);
        gtk_combo_box_append_text(GTK_COMBO_BOX(cmbImageFormat), value);
        int res = strncmp(value, imageoutputSetting, 50);
        if(res == 0)
        {
            gtk_combo_box_set_active(GTK_COMBO_BOX(cmbImageFormat), i);
        }
    }

    // Free the camera from the tyranny.
    gp_camera_exit(camera, context);
    gp_camera_free(camera);

    return 1;
}

void showCaptureWindow()
{
    if(detectCamera() == 0)
    {
        prtError(CST_NO_CAMERA_PRESENT);
        return;
    }
    gtk_widget_show(GTK_WIDGET(captureWindow));
}

void hideCaptureWindow()
{
    gtk_widget_hide(GTK_WIDGET(captureWindow));
}

void createCaptureWindow()
{
    captureWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(captureWindow), "Capture New Image");
    gtk_widget_set_size_request(GTK_WIDGET(captureWindow), -1, -1);
    gtk_window_set_resizable(GTK_WINDOW(captureWindow), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(captureWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(captureWindow), TRUE);

    GtkWidget *settingsTable = gtk_table_new(3, 4, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(settingsTable), 5);

    GtkWidget *lblISO = gtk_label_new("ISO:");
    GtkWidget *algISO = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algISO), GTK_WIDGET(lblISO));
    cmbISO = gtk_combo_box_text_new();
    gtk_widget_set_size_request(GTK_WIDGET(cmbISO), 100, -1);

    GtkWidget *lblSSpd = gtk_label_new("Tv:");
    GtkWidget *algSSpd = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algSSpd), GTK_WIDGET(lblSSpd));
    cmbShutterSpeed = gtk_combo_box_text_new();
    gtk_widget_set_size_request(GTK_WIDGET(cmbShutterSpeed), 100, -1);

    GtkWidget *lblAperture = gtk_label_new("Av:");
    GtkWidget *algAperture = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algAperture), GTK_WIDGET(lblAperture));
    GtkWidget *lblf = gtk_label_new("f/");
    cmbAperture = gtk_combo_box_text_new();
    gtk_widget_set_size_request(GTK_WIDGET(cmbAperture), 100, -1);

    GtkWidget *lblWB = gtk_label_new("W/B:");
    GtkWidget *algWB = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algWB), GTK_WIDGET(lblWB));
    cmbWhiteBalance = gtk_combo_box_text_new();
    gtk_widget_set_size_request(GTK_WIDGET(cmbWhiteBalance), 100, -1);

    GtkWidget *lblExposureComp = gtk_label_new("+/- EV:");
    GtkWidget *algExposureComp = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algExposureComp), GTK_WIDGET(lblExposureComp));
    cmbExposureCompensation = gtk_combo_box_text_new();
    gtk_widget_set_size_request(GTK_WIDGET(cmbExposureCompensation), 100, -1);

    GtkWidget *lblMeter = gtk_label_new("Mtr:");
    GtkWidget *algMeter = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algMeter), GTK_WIDGET(lblMeter));
    cmbMeteringMode = gtk_combo_box_text_new();
    gtk_widget_set_size_request(GTK_WIDGET(cmbMeteringMode), 100, -1);

    GtkWidget *lblImageFormat = gtk_label_new("Image Format:");
    GtkWidget *algImageFormat = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algImageFormat), GTK_WIDGET(lblImageFormat));
    cmbImageFormat = gtk_combo_box_text_new();

    GtkWidget *fchStorageLocation = gtk_file_chooser_button_new("Storage Location", GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    GtkWidget *lblStorageLoc = gtk_label_new("Storage Location:");
    GtkWidget *algStorageLoc = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algStorageLoc), GTK_WIDGET(lblStorageLoc));


    GtkObject *imageCountAdjustment = gtk_adjustment_new(1, 1, 99, 1, 10, 0.0);
    GtkWidget *imageCountSpin = gtk_spin_button_new(GTK_ADJUSTMENT(imageCountAdjustment), 1.0, 0);
    GtkWidget *lblImageCount = gtk_label_new("Number of Images:");
    GtkWidget *algImageCount = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algImageCount), GTK_WIDGET(lblImageCount));

    GtkWidget *cmbImageType = gtk_combo_box_text_new();
    gtk_combo_box_append_text(GTK_COMBO_BOX(cmbImageType), "Preview Image");
    gtk_combo_box_append_text(GTK_COMBO_BOX(cmbImageType), "Reference Image");
    gtk_combo_box_append_text(GTK_COMBO_BOX(cmbImageType), "Deformed Image");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbImageType), 0);
    GtkWidget *lblImageType = gtk_label_new("Image Type:");
    GtkWidget *algImageType = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algImageType), GTK_WIDGET(lblImageType));

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algSSpd), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbShutterSpeed), 1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algAperture), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach_defaults(GTK_TABLE(settingsTable), GTK_WIDGET(lblf), 3, 4, 0, 1);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbAperture), 4, 5, 0, 1, GTK_EXPAND, GTK_EXPAND, 0, 0);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algISO), 5, 6, 0, 1, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbISO), 6, 7, 0, 1, GTK_EXPAND, GTK_EXPAND, 12, 0);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algWB), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbWhiteBalance), 1, 2, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algExposureComp), 2, 3, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbExposureCompensation), 4, 5, 1, 2, GTK_EXPAND, GTK_EXPAND, 0, 0);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algMeter), 5, 6, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(cmbMeteringMode), 6, 7, 1, 2, GTK_EXPAND, GTK_EXPAND, 12, 0);
/*
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algImageFormat), 0, 1, 6, 7, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach_defaults(GTK_TABLE(settingsTable), GTK_WIDGET(cmbImageFormat), 2, 3, 6, 7);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algImageCount), 0, 1, 7, 8, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach_defaults(GTK_TABLE(settingsTable), GTK_WIDGET(imageCountSpin), 2, 3, 7, 8);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algImageType), 0, 1, 8, 9, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach_defaults(GTK_TABLE(settingsTable), GTK_WIDGET(cmbImageType), 2, 3, 8, 9);

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algStorageLoc), 0, 1, 9, 10, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach_defaults(GTK_TABLE(settingsTable), GTK_WIDGET(fchStorageLocation), 2, 3, 9, 10);
*/

    GtkWidget *imgPreviewImage = gtk_image_view_new();
    gtk_widget_set_size_request(GTK_WIDGET(imgPreviewImage), 320, 214);
    gtk_widget_show(GTK_WIDGET(imgPreviewImage));
    previewImage = gdk_pixbuf_new_from_file("objects/graphics/no_preview_data.png", NULL);
    gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imgPreviewImage), previewImage, TRUE);
    gtk_image_view_set_show_frame(GTK_IMAGE_VIEW(imgPreviewImage), FALSE);

    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    GtkWidget *hbox1 = gtk_hbox_new(FALSE, 0);
    GtkWidget *algButtons = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algButtons), GTK_WIDGET(hbox1));

    GtkWidget *btnFocus = gtk_button_new_with_label("Grab Focus");
    GtkWidget *btnCapture = gtk_button_new_with_label("Capture");
    GtkWidget *btnCloseCapture = gtk_button_new_with_label("Close");

    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(settingsTable), FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(imgPreviewImage), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(algButtons), FALSE, FALSE, 12);

    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnFocus), FALSE, FALSE, 9);
    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnCapture), FALSE, FALSE, 3);
    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnCloseCapture), FALSE, FALSE, 12);

    gtk_container_add(GTK_CONTAINER(captureWindow), GTK_WIDGET(vbox));

    gtk_widget_show(GTK_WIDGET(btnCloseCapture));
    gtk_widget_show(GTK_WIDGET(fchStorageLocation));
    gtk_widget_show(GTK_WIDGET(algStorageLoc));
    gtk_widget_show(GTK_WIDGET(lblStorageLoc));
    gtk_widget_show(GTK_WIDGET(lblImageType));
    gtk_widget_show(GTK_WIDGET(algImageType));
    gtk_widget_show(GTK_WIDGET(cmbImageType));
    gtk_widget_show(GTK_WIDGET(lblImageCount));
    gtk_widget_show(GTK_WIDGET(algImageCount));
    gtk_widget_show(GTK_WIDGET(imageCountSpin));
    gtk_widget_show(GTK_WIDGET(lblISO));
    gtk_widget_show(GTK_WIDGET(algISO));
    gtk_widget_show(GTK_WIDGET(algImageFormat));
    gtk_widget_show(GTK_WIDGET(algMeter));
    gtk_widget_show(GTK_WIDGET(algButtons));
    gtk_widget_show(GTK_WIDGET(lblSSpd));
    gtk_widget_show(GTK_WIDGET(algSSpd));
    gtk_widget_show(GTK_WIDGET(algExposureComp));
    gtk_widget_show(GTK_WIDGET(lblAperture));
    gtk_widget_show(GTK_WIDGET(algAperture));
    gtk_widget_show(GTK_WIDGET(lblf));
    gtk_widget_show(GTK_WIDGET(cmbISO));
    gtk_widget_show(GTK_WIDGET(cmbShutterSpeed));
    gtk_widget_show(GTK_WIDGET(cmbAperture));
    gtk_widget_show(GTK_WIDGET(settingsTable));
    gtk_widget_show(GTK_WIDGET(lblWB));
    gtk_widget_show(GTK_WIDGET(algWB));
    gtk_widget_show(GTK_WIDGET(cmbWhiteBalance));
    gtk_widget_show(GTK_WIDGET(lblExposureComp));
    gtk_widget_show(GTK_WIDGET(cmbExposureCompensation));
    gtk_widget_show(GTK_WIDGET(cmbMeteringMode));
    gtk_widget_show(GTK_WIDGET(lblMeter));
    gtk_widget_show(GTK_WIDGET(cmbImageFormat));
    gtk_widget_show(GTK_WIDGET(lblImageFormat));
    gtk_widget_show(GTK_WIDGET(vbox));
    gtk_widget_show(GTK_WIDGET(hbox1));
    gtk_widget_show(GTK_WIDGET(btnFocus));
    gtk_widget_show(GTK_WIDGET(btnCapture));

    g_signal_connect(G_OBJECT(captureWindow), "delete_event", G_CALLBACK(hideCaptureWindow), NULL);
    g_signal_connect(G_OBJECT(btnCloseCapture), "clicked", G_CALLBACK(hideCaptureWindow), NULL);
    g_signal_connect(G_OBJECT(btnCapture), "clicked", G_CALLBACK(captureImage), NULL);
}

