#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>

#include "imageload.h"
#include "guielements.h"
#include "constants.h"
#include "imagecapture.h"
#include "callbacks.h"
#include "strainarea.h"
#include "measurestrain.h"
//#include "mapstrain.h"
#include "strainoutputconfig.h"
#include "autoreferencepoints.h"

#include "src/ui/if_mapdeformation.h"


int initializeUI(void)
{
    mainWindow = NULL;
    GtkAccelGroup *accel_group = NULL;
    accel_group = gtk_accel_group_new();
    mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(GTK_WIDGET(mainWindow), 950, 600);
    gtk_window_set_title(GTK_WINDOW(mainWindow), "<Unsaved Test> - CST");
    gtk_window_add_accel_group(GTK_WINDOW(mainWindow), GTK_ACCEL_GROUP(accel_group));

    // Vertical Container for UI Widgets
    GtkWidget *bxMainContainer;
    bxMainContainer = gtk_vbox_new(FALSE, 0);

    // Menu bar widget for
    GtkWidget *mbrMainMenu = gtk_menu_bar_new();
    GtkWidget *mnuFile = gtk_menu_item_new_with_label("File");
    GtkWidget *mnuView = gtk_menu_item_new_with_label("View");
    GtkWidget *mnuImage = gtk_menu_item_new_with_label("Image");
    GtkWidget *mnuProcess = gtk_menu_item_new_with_label("Process");
    GtkWidget *mnuHelp = gtk_menu_item_new_with_label("Help");

    GtkWidget *mnuctFile = gtk_menu_new();
    GtkWidget *mnuctView = gtk_menu_new();
    GtkWidget *mnuctImage = gtk_menu_new();
    GtkWidget *mnuctStrainArea = gtk_menu_new();
    GtkWidget *mnuctReferencePoints = gtk_menu_new();
    GtkWidget *mnuctDeflectionPoints = gtk_menu_new();
    GtkWidget *mnuctProcess = gtk_menu_new();
    GtkWidget *mnuctHelp = gtk_menu_new();

    GtkWidget *mnubtnNew = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, accel_group);
    GtkWidget *mnubtnOpen = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, accel_group);
    GtkWidget *mnubtnSep = gtk_menu_item_new();
    GtkWidget *mnubtnSaveProject = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel_group);
    GtkWidget *mnubtnSep2 = gtk_menu_item_new();
    GtkWidget *mnubtnPrintSetup = gtk_image_menu_item_new_from_stock(GTK_STOCK_PAGE_SETUP, accel_group);
    GtkWidget *mnubtnPrint = gtk_image_menu_item_new_from_stock(GTK_STOCK_PRINT, accel_group);
    GtkWidget *mnubtnSep3 = gtk_menu_item_new();
    GtkWidget *mnubtnProperties = gtk_image_menu_item_new_from_stock(GTK_STOCK_PROPERTIES, accel_group);
    GtkWidget *mnubtnSep5 = gtk_menu_item_new();
    GtkWidget *mnubtnExit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnNew));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnOpen));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnSep));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnSaveProject));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnSep2));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnPrintSetup));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnPrint));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnSep3));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnProperties));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnSep5));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctFile), GTK_WIDGET(mnubtnExit));

    GtkWidget *mnucbtnShowToolbar = gtk_check_menu_item_new_with_label("Show Toolbar");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowToolbar), TRUE);
    GtkWidget *mnuSep11 = gtk_menu_item_new();
    GtkWidget *mnucbtnShowStrainArea = gtk_check_menu_item_new_with_label("Show the strain area");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowStrainArea), TRUE);
    GtkWidget *mnucbtnShowReference = gtk_check_menu_item_new_with_label("Show assigned reference points");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowReference), TRUE);
    GtkWidget *mnucbtnShowDeflection = gtk_check_menu_item_new_with_label("Show assigned deflection points");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowDeflection), TRUE);
    GtkWidget *mnubtnSep4 = gtk_menu_item_new();
    GtkWidget *mnubtnStrainArea = gtk_menu_item_new_with_label("Strain Area");
    GtkWidget *mnubtnReferencePoints  = gtk_menu_item_new_with_label("Reference Points");
    GtkWidget *mnubtnDeflectionPoints = gtk_menu_item_new_with_label("Deflection Points");;
    GtkWidget *mnubtnSep6 = gtk_menu_item_new();
    GtkWidget *mnubtnPreferences = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, accel_group);
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnucbtnShowToolbar));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnuSep11));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnucbtnShowStrainArea));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnucbtnShowReference));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnucbtnShowDeflection));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnubtnSep4));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnubtnStrainArea));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnubtnReferencePoints));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnubtnDeflectionPoints));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnubtnSep6));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctView), GTK_WIDGET(mnubtnPreferences));

    GtkWidget *mnucbtnShowBoundingBox = gtk_check_menu_item_new_with_label("Show bounding box");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowBoundingBox), TRUE);
    GtkWidget *mnucbtnShowCrossHairs = gtk_check_menu_item_new_with_label("Show crosshairs");
    GtkWidget *mnucbtnShowCoords = gtk_check_menu_item_new_with_label("Show co-ordinates");
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctStrainArea), GTK_WIDGET(mnucbtnShowBoundingBox));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctStrainArea), GTK_WIDGET(mnucbtnShowCrossHairs));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctStrainArea), GTK_WIDGET(mnucbtnShowCoords));

    GtkWidget *mnucbtnShowLabelRef = gtk_check_menu_item_new_with_label("Show label identifier");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowLabelRef), TRUE);
    GtkWidget *mnucbtnShowCrossHairsRef = gtk_check_menu_item_new_with_label("Show crosshairs");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowCrossHairsRef), TRUE);
    GtkWidget *mnucbtnShowCoordsRef = gtk_check_menu_item_new_with_label("Show co-ordinates");
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctReferencePoints), GTK_WIDGET(mnucbtnShowLabelRef));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctReferencePoints), GTK_WIDGET(mnucbtnShowCrossHairsRef));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctReferencePoints), GTK_WIDGET(mnucbtnShowCoordsRef));

    GtkWidget *mnucbtnShowLabelDef = gtk_check_menu_item_new_with_label("Show label identifier");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowLabelDef), TRUE);
    GtkWidget *mnucbtnShowCrossHairsDef = gtk_check_menu_item_new_with_label("Show crosshairs");
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mnucbtnShowCrossHairsDef), TRUE);
    GtkWidget *mnucbtnShowCoordsDef = gtk_check_menu_item_new_with_label("Show co-ordinates");
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctDeflectionPoints), GTK_WIDGET(mnucbtnShowLabelDef));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctDeflectionPoints), GTK_WIDGET(mnucbtnShowCrossHairsDef));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctDeflectionPoints), GTK_WIDGET(mnucbtnShowCoordsDef));

    GtkWidget *mnuLoadReferenceImage = gtk_menu_item_new_with_label("Load a Reference Image");
    GtkWidget *mnuLoadDeformedImage = gtk_menu_item_new_with_label("Load a Deformed Image");
    GtkWidget *mnuSep7 = gtk_menu_item_new();
    GtkWidget *mnuImageCapture = gtk_menu_item_new_with_label("Open Image Capture Window");
    GtkWidget *mnuSep8 = gtk_menu_item_new();
    GtkWidget *mnuInspectImage = gtk_menu_item_new_with_label("Inspect an Image");
    GtkWidget *mnuRemoveImage = gtk_menu_item_new_with_label("Remove an Image");

    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuLoadReferenceImage));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuLoadDeformedImage));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuSep7));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuImageCapture));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuSep8));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuInspectImage));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctImage), GTK_WIDGET(mnuRemoveImage));

    GtkWidget *mnuDetectReferencePoints = gtk_menu_item_new_with_label("Automatically Detect Reference Points");
    //g_signal_connect(G_OBJECT(mnuDetectReferencePoints), "activate", G_CALLBACK(showMapConfigurationWindow), NULL);

    GtkWidget *mnuSep10 = gtk_menu_item_new();
    GtkWidget *mnuProcessReferencePoints = gtk_menu_item_new_with_label("Process Reference Points");
    GtkWidget *mnuProcessDeflectionPoints = gtk_menu_item_new_with_label("Process Deflection Points");
    GtkWidget *mnuSep9 = gtk_menu_item_new();
    GtkWidget *mnuGenerateStrainMap = gtk_menu_item_new_with_label("Generate Strain Map");
    GtkWidget *mnuGenerateStrainDistribution = gtk_menu_item_new_with_label("Generate Strain Distribution");
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuDetectReferencePoints));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuSep10));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuProcessReferencePoints));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuProcessDeflectionPoints));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuSep9));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuGenerateStrainMap));
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctProcess), GTK_WIDGET(mnuGenerateStrainDistribution));

    GtkWidget *mnuAbout = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, accel_group);
    gtk_menu_shell_append(GTK_MENU_SHELL(mnuctHelp), GTK_WIDGET(mnuAbout));

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnuFile),  GTK_WIDGET(mnuctFile));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnuView),  GTK_WIDGET(mnuctView));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnuImage),  GTK_WIDGET(mnuctImage));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnubtnStrainArea), GTK_WIDGET(mnuctStrainArea));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnubtnReferencePoints), GTK_WIDGET(mnuctReferencePoints));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnubtnDeflectionPoints), GTK_WIDGET(mnuctDeflectionPoints));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnuProcess), GTK_WIDGET(mnuctProcess));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mnuHelp), GTK_WIDGET(mnuctHelp));

    gtk_menu_shell_append(GTK_MENU_SHELL(mbrMainMenu), GTK_WIDGET(mnuFile));
    gtk_menu_shell_append(GTK_MENU_SHELL(mbrMainMenu), GTK_WIDGET(mnuView));
    gtk_menu_shell_append(GTK_MENU_SHELL(mbrMainMenu), GTK_WIDGET(mnuImage));
    gtk_menu_shell_append(GTK_MENU_SHELL(mbrMainMenu), GTK_WIDGET(mnuProcess));
    gtk_menu_shell_append(GTK_MENU_SHELL(mbrMainMenu), GTK_WIDGET(mnuHelp));

    // GtkImageView Section
    imageView = gtk_image_view_new();
    GdkPixbuf *defaultImage = gdk_pixbuf_new_from_file("objects/graphics/no_image_warning.png", NULL);
    gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imageView), GDK_PIXBUF(defaultImage), TRUE);

    stbMainStatusBar = gtk_statusbar_new();

    // Create a New Tool Bar for the top of the window.
    GtkWidget *tbrToolBar = gtk_toolbar_new();

    // Separator Items
    GtkWidget *tbtnSeparator0 = GTK_WIDGET(gtk_separator_tool_item_new());
    GtkWidget *tbtnSeparator1 = GTK_WIDGET(gtk_separator_tool_item_new());
    GtkWidget *tbtnSeparator2 = GTK_WIDGET(gtk_separator_tool_item_new());
    GtkWidget *tbtnSeparator3 = GTK_WIDGET(gtk_separator_tool_item_new());
    GtkWidget *tbtnSeparator4 = GTK_WIDGET(gtk_separator_tool_item_new());
    // Item for loading reference image, image widget and button widget.
    GtkWidget *imgLoadReferenceImage = gtk_image_new_from_file("objects/icons/stock_open_reference.svg");
    GtkWidget *tbtnLoadReferenceImage = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgLoadReferenceImage), "Load Reference Image"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnLoadReferenceImage));

    // Item for loading deformed image, image widget and button widget.
    GtkWidget *imgLoadDeformedImage = gtk_image_new_from_file("objects/icons/stock_open_deformed.svg");
    GtkWidget *tbtnLoadDeformedImage = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgLoadDeformedImage), "Load Deformed Image"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnLoadDeformedImage));

    // Item for deleting an image, image widget and button widget.
    GtkWidget *imgDeleteImage = gtk_image_new_from_file("objects/icons/deleteimage.svg");
    GtkWidget *tbtnDeleteImage = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgDeleteImage), "Delete an Image"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnDeleteImage));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnSeparator0));

    // Item for capturing a new image, image widget and button widget.
    GtkWidget *imgCamera = gtk_image_new_from_file("objects/icons/camera-photo.png");
    GtkWidget *tbtnCaptureNewImage = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgCamera), "Open Image Capture"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnCaptureNewImage));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnSeparator1));

    // Item for selecting the drag tool
    GtkWidget *imgDragTool = gtk_image_new_from_file("objects/icons/drag.png");
    GtkWidget *tbtnDragTool = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgDragTool), "Drag Tool"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnDragTool));
    g_signal_connect(G_OBJECT(tbtnDragTool), "clicked", G_CALLBACK(selDragTool), NULL);

    // Item for adding a reference point manually, image widget and button widget.
    GtkWidget *imgAddReferencePoint = gtk_image_new_from_file("objects/icons/add_reference_point.svg");
    GtkWidget *tbtnAddReferencePoint = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgAddReferencePoint), "Add Reference Point"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnAddReferencePoint));

    // Item for adding a reference point automatically, image widget and button widget.
    GtkWidget *imgAddAutoReferencePoint = gtk_image_new_from_file("objects/icons/add_auto_reference_point.svg");
    GtkWidget *tbtnAddAutoReferencePoint = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgAddAutoReferencePoint), "Auto Addd Reference Points"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnAddAutoReferencePoint));

    // Item for adding a deflection point, image widget and button widget.
    GtkWidget *imgAddDeflectionPoint = gtk_image_new_from_file("objects/icons/add_deflection_point.svg");
    GtkWidget *tbtnAddDeflectionPont = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgAddDeflectionPoint), "Add Deflection Point"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnAddDeflectionPont));

    // Item for adding a strain area, image widget and button widget.
    GtkWidget *imgAddStrainArea = gtk_image_new_from_file("objects/icons/add_strain_area.svg");
    GtkWidget *tbtnAddStrainArea = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgAddStrainArea), "Add Strain Area"));
    g_signal_connect(G_OBJECT(tbtnAddStrainArea), "clicked", G_CALLBACK(addStrainArea), NULL);
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnAddStrainArea));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnSeparator2));

    // Item for processing of reference points, image widget and button widget.
    GtkWidget *imgProcessReferencePoints = gtk_image_new_from_file("objects/icons/process_reference_points.svg");
    GtkWidget *tbtnProcessReferencePoints = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgProcessReferencePoints), "Process Reference Points"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnProcessReferencePoints));

    // Item for processing a deflection point, image widget and button widget.
    GtkWidget *imgProcessDeflectionPoints = gtk_image_new_from_file("objects/icons/process_deflection_points.svg");
    GtkWidget *tbtnProcessDeflectionPoints = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgProcessDeflectionPoints), "Process Deflection Points"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnProcessDeflectionPoints));

    // Item for generating a strain distribution, image widget and button widget.
    GtkWidget *imgGenerateStrainDistribution = gtk_image_new_from_file("objects/icons/generate_strain_distribution.svg");
    GtkWidget *tbtnGenerateStrainDistribution = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgGenerateStrainDistribution), "Generate Strain Distribution"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnGenerateStrainDistribution));
    g_signal_connect(G_OBJECT(tbtnGenerateStrainDistribution), "clicked", G_CALLBACK(ShowDistributionConfiguration), NULL);

    // Item for generating a strain map, image widget and button widget.
    GtkWidget *imgGenerateStrainMap = gtk_image_new_from_file("objects/icons/generate_strain_map.svg");
    GtkWidget *tbtnGenerateStrainMap = GTK_WIDGET(gtk_tool_button_new(GTK_WIDGET(imgGenerateStrainMap), "Generate Strain Map"));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnGenerateStrainMap));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnSeparator3));
    g_signal_connect(G_OBJECT(tbtnGenerateStrainMap), "clicked", G_CALLBACK(showMapConfigWindow), NULL);

    // Item for zooming the current image in
    GtkWidget *tbtnZoomIn = GTK_WIDGET(gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_IN));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnZoomIn));
    g_signal_connect(G_OBJECT(tbtnZoomIn), "clicked", G_CALLBACK(zoomIn), NULL); // Signal Handler

    // Item for zooming the curent image out
    GtkWidget *tbtnZoomOut = GTK_WIDGET(gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_OUT));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnZoomOut));
    g_signal_connect(G_OBJECT(tbtnZoomOut), "clicked", G_CALLBACK(zoomOut), NULL); // Signal Handler

    // Item for zooming to 100%
    GtkWidget *tbtnZoom100 = GTK_WIDGET(gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_100));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnZoom100));
    g_signal_connect(G_OBJECT(tbtnZoom100), "clicked", G_CALLBACK(zoom100), NULL);
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(tbtnSeparator4));

    // Label item
    GtkWidget *titmLabel = GTK_WIDGET(gtk_tool_item_new());
    GtkWidget *lblImage = gtk_label_new("Image:");
    gtk_container_add(GTK_CONTAINER(titmLabel), GTK_WIDGET(lblImage));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(titmLabel));

    // Combo Box Item
    GtkWidget *titmImageSelect = GTK_WIDGET(gtk_tool_item_new());
    cmbSelectImage = gtk_combo_box_text_new();
    GtkWidget *vbxContainer = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbxContainer), GTK_WIDGET(cmbSelectImage), TRUE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(titmImageSelect), GTK_WIDGET(vbxContainer));
    gtk_container_add(GTK_CONTAINER(tbrToolBar), GTK_WIDGET(titmImageSelect));
    gtk_widget_set_size_request(GTK_WIDGET(cmbSelectImage), -1, 35);
    gtk_combo_box_append_text(GTK_COMBO_BOX(cmbSelectImage), "No Image Loaded");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbSelectImage), 0);
    g_signal_connect(G_OBJECT(cmbSelectImage), "changed", G_CALLBACK(imageSelected), NULL);

    // Pack the menu bar and the tool bar into the vertical box.
    gtk_box_pack_start(GTK_BOX(bxMainContainer), GTK_WIDGET(mbrMainMenu), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bxMainContainer), GTK_WIDGET(tbrToolBar), FALSE, TRUE, 0);

    GtkWidget *bxImageContainer = gtk_hbox_new(FALSE, 12);

    gtk_box_pack_start(GTK_BOX(bxImageContainer), GTK_WIDGET(imageView), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(bxMainContainer), GTK_WIDGET(bxImageContainer), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(bxMainContainer), GTK_WIDGET(stbMainStatusBar), FALSE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(mainWindow), GTK_WIDGET(bxMainContainer));

    // Create the image capture window
    createCaptureWindow();
    // Create the strain window
    createStrainWindow();
    strainSelector = gtk_image_tool_selector_new(GTK_IMAGE_VIEW(imageView));
    exclusionSelector = gtk_image_tool_selector_new(GTK_IMAGE_VIEW(imageView));
    // Create the strain test configuration window
    initialize_StrainConfigurationWindow();
    // Create the output configuration window
    createOutputWindow();

    // Connect the signals from the window to their relevant callback functions
    g_signal_connect(G_OBJECT(mainWindow), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(mnubtnExit), "activate", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(mnuLoadReferenceImage), "activate", G_CALLBACK(loadReferenceImage), NULL);
    g_signal_connect(G_OBJECT(tbtnLoadReferenceImage), "clicked", G_CALLBACK(loadReferenceImage), NULL);
    g_signal_connect(G_OBJECT(mnuLoadDeformedImage), "activate", G_CALLBACK(loadDeformedImage), NULL);
    g_signal_connect(G_OBJECT(tbtnLoadDeformedImage), "clicked", G_CALLBACK(loadDeformedImage), NULL);

    g_signal_connect(G_OBJECT(mnuImageCapture), "activate", G_CALLBACK(showCaptureWindow), NULL);
    g_signal_connect(G_OBJECT(tbtnCaptureNewImage), "clicked", G_CALLBACK(showCaptureWindow), NULL);

    g_signal_connect(G_OBJECT(tbtnAddAutoReferencePoint), "clicked", G_CALLBACK(show_ReferencePointAutoWindow), NULL);

    g_signal_connect(G_OBJECT(exclusionSelector), "selection-changed", G_CALLBACK(exclusionSelectionChanged), NULL);

    g_signal_connect(G_OBJECT(tbtnProcessReferencePoints), "clicked", G_CALLBACK(process_ReferencePoints), NULL);

    // Set up the list store for the reference image list
    referenceImages = gtk_list_store_new(1, G_TYPE_STRING);
    deformedImages = gtk_list_store_new(1, G_TYPE_STRING);
    referencePoints = gtk_list_store_new(4, G_TYPE_INT, G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
    countReferenceImages = 0;
    countDeformedImages = 0;

    initialize_ReferencePointAutoWindow();

    gtk_widget_show_all(GTK_WIDGET(mainWindow));

    return 0;
}

int main (int argc, char *argv[])
{
    numthreads = sysconf(_SC_NPROCESSORS_ONLN);

    /* Initialize GTK+ */
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
    gtk_init (&argc, &argv);
    g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

    initializeUI();
    createMapConfigWindow();

    /* Enter the main loop */
    gtk_main ();
    return 0;
}
