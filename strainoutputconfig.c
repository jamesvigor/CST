
/*  FILE:       strainoutputconfig.c
    AUTHOR:     James E Vigor
    PROJECT:    CST
    DATE:       11-6-2012
    DESCRIP:    Strain Plotting functions for the CST application, including UI elements
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

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>

#include "gnuplot_i.h"
#include "gnuplot_i.c"
#include "guielements.h"

// Function to call on display of output window.
void showOutputWindow(char *inputFile)
{
    // Set the text of the input file to the output file in the strain map window
    // This assumes this is being called after the strain map window has been shown!
    gtk_entry_set_text(GTK_ENTRY(txtInputFile), inputFile);
    // Show the window widget
    gtk_widget_show(GTK_WIDGET(outputWindow));
}

// Function to hide the window
void hideOutputWindow()
{
    // Hide the window widget
    gtk_widget_hide(GTK_WIDGET(outputWindow));
}

// Function to call upon toggling of the auto-axis toggle button (for y-axis)
void toggleAutoYToggled()
{
    // If the button is active (on), set the labels and widgets to insensitive
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleAutoY)) == TRUE)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(yFrom), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(yTo), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtYAxisMin), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtYAxisMax), FALSE);
    }
    // Else set them to sensitive
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(yFrom), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(yTo), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtYAxisMin), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtYAxisMax), TRUE);
    }
}

// Function to call upon toggling of the auto-axis toggle button (for x-axis)
void toggleAutoXToggled()
{
    // If the button is active (on), set the labels and widgets to insensitive
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleAutoX)) == TRUE)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(xFrom), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(xTo), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtXAxisMin), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtXAxisMax), FALSE);
    }
    // Else set them to sensitive
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(xFrom), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(xTo), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtXAxisMin), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(txtXAxisMax), TRUE);
    }
}

// Function to be called on the selection of an output image or document (ie pdf)
void selectOutputImage()
{
    // Get the home environment variable for processing
    char *home = getenv("HOME");
    char *filePath;
    // Create the selectFile Dialog for saving the file.
    GtkWidget *selectFile = gtk_file_chooser_dialog_new("Create Output Image", GTK_WINDOW(mainWindow),
                            GTK_FILE_CHOOSER_ACTION_SAVE,
                            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(selectFile), TRUE);

    // Depending on the users selection from the cmbOutputFormat, restrict the available file save types from the file save dialog
    GtkFileFilter *filter = gtk_file_filter_new();
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbOutputFormat));
    switch(index)
    {
    case 0:
        gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "GIF Image");
        gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/gif");
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "newgraph.gif");
        break;
    case 1:
        gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "JPEG Image");
        gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/jpeg");
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "newgraph.jpeg");
        break;
    case 2:
        gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "PNG Image");
        gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/png");
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "newgraph.png");
        break;
    case 3:
        gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "PDF Document");
        gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "application/pdf");
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(selectFile), "newgraph.pdf");
    }
    // Set the filter as set by the switch function to the save dialog
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filter));
    // Set the default save directory to the users home folder
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(selectFile), home);

    // Run the dialog, if gtk response is to accept
    if(gtk_dialog_run(GTK_DIALOG(selectFile)) == GTK_RESPONSE_ACCEPT)
    {
        // Retrieve the save target
        filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(selectFile));
        gtk_entry_set_text(GTK_ENTRY(txtOutputImage), filePath);
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
    // In other case, we can destroy the widget
    else
    {
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
}

// Function to be called on the selection of an input dialog
void selectInputFile()
{
    char *home = getenv("HOME");
    char *filePath; // Storage for the file path
    // Create the selectFile dialog to set the open file window
    GtkWidget *selectFile = gtk_file_chooser_dialog_new("Create Output Image", GTK_WINDOW(mainWindow),
                            GTK_FILE_CHOOSER_ACTION_OPEN,
                            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

    // Create a new filter to get rid of the stuff we arent interested in
    GtkFileFilter *filter1 = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filter1), "CSV/DAT Data Files");
    gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter1), "text/csv");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter1), "*.dat");

    // Create another filter for all file types, just incase the user wants this.
    GtkFileFilter *filter2 = gtk_file_filter_new();
    gtk_file_filter_set_name(GTK_FILE_FILTER(filter2), "Other Files");
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter2), "*.*");

    // Add the file filters to the file chooser, set the DAT/CSV filter as default
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filter1));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filter2));
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(selectFile), GTK_FILE_FILTER(filter1));

    // Set the current folder to the users home folder
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(selectFile), home);

    // Run the dialog, if successful
    if(gtk_dialog_run(GTK_DIALOG(selectFile)) == GTK_RESPONSE_ACCEPT)
    {
        // Retrieve the save target
        filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(selectFile));
        gtk_entry_set_text(GTK_ENTRY(txtInputFile), filePath);
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
    // If cancel is clicked (or the x button) destroy the dialog
    else
    {
        gtk_widget_destroy(GTK_WIDGET(selectFile));
    }
}

// Function called when the user changes the output format from the combo box
void outputFormatChanged()
{
    char *homePath = getenv("HOME");
    char copy_homePath[strlen(homePath) + 16];
    strcpy(copy_homePath, homePath);
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbOutputFormat));
    switch(index)

    {
        case 0:
            strncat(copy_homePath, "/newgraph.gif", 15);
            break;
        case 1:
            strncat(copy_homePath, "/newgraph.jpeg", 15);
            break;
        case 2:
            strncat(copy_homePath, "/newgraph.png", 15);
            break;
        case 3:
            strncat(copy_homePath, "/newgraph.pdf", 15);
            break;
    }

    if(gtk_combo_box_get_active(GTK_COMBO_BOX(cmbOutputFormat)) == 3)
    {
        gtk_widget_set_sensitive(GTK_WIDGET(widthSpin), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(heightSpin), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblx), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblDimAdjustment), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblpx2), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblpx1), FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(widthSpin), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(heightSpin), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblx), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblDimAdjustment), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblpx2), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(lblpx1), TRUE);
    }


    gtk_entry_set_text(GTK_ENTRY(txtOutputImage), copy_homePath);
}

// Function called when creating the graph from the user interface
void createGraph()
{
    // We are wrapping around gnuplot for ease (and because it produces nice graphs), create the object
    gnuplot_ctrl *plot;
    // Initialize the object
    plot = gnuplot_init();

    // Set the x/y values from above using the set xlabel and set ylabel functions
    gnuplot_set_xlabel(plot, (char*)gtk_entry_get_text(GTK_ENTRY(txtXAxisTitle)));
    gnuplot_set_ylabel(plot, (char*)gtk_entry_get_text(GTK_ENTRY(txtYAxisTitle)));

    // Set the graph title from the user interface (this might cause a compile warning too)
    gnuplot_cmd(plot, "set title \"%s\"", gtk_entry_get_text(GTK_ENTRY(txtGraphTitle)));

    // Set automatic scaling of the x axis depending on the user selection
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleAutoX)) == TRUE)
    {
        gnuplot_cmd(plot, "set auto x");
    }
    // If scaling is manually specified, set the xMin and xMax vales for the plot
    else
    {
        double xMin = atof(gtk_entry_get_text(GTK_ENTRY(txtXAxisMin)));
        double xMax = atof(gtk_entry_get_text(GTK_ENTRY(txtXAxisMax)));
        gnuplot_cmd(plot, "set xrange [%f:%f]", xMin, xMax );
    }

    // Set automatic scaling of the y axis depending on the user selection
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleAutoY)) == TRUE)
    {
        gnuplot_cmd(plot, "set auto y");
    }
    // If scaling is manually specified, set the yMin and yMax vales for the plot
    else
    {
        double yMin = atof(gtk_entry_get_text(GTK_ENTRY(txtYAxisMin)));
        double yMax = atof(gtk_entry_get_text(GTK_ENTRY(txtYAxisMax)));
        gnuplot_cmd(plot, "set yrange [%f:%f]", yMin, yMax );
    }

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleGrid)) == TRUE)
    {
        gnuplot_cmd(plot, "set grid");
    }
    else
    {
        gnuplot_cmd(plot, "unset grid");
    }

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleReverseX)) == TRUE)
    {
        gnuplot_cmd(plot, "set xrange [] reverse");
    }

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleReverseY)) == TRUE)
    {
        gnuplot_cmd(plot, "set yrange [] reverse");
    }

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleKey)) == TRUE)
    {
        gnuplot_cmd(plot, "set key");
    }
    else
    {
        gnuplot_cmd(plot, "unset key");
    }

    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbOutputFormat));
    int width = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(widthAdjustment));
    int height = (int)gtk_adjustment_get_value(GTK_ADJUSTMENT(heightAdjustment));
    gnuplot_cmd(plot, "set output \"%s\"", gtk_entry_get_text(GTK_ENTRY(txtOutputImage)));

    switch(index)
    {
        case 0:
            gnuplot_cmd(plot, "set terminal gif size %d,%d", width, height);
            break;
        case 1:
            gnuplot_cmd(plot, "set terminal jpeg size %d,%d", width, height);
            break;
        case 2:
            gnuplot_cmd(plot, "set terminal png size %d,%d", width, height);
            break;
        case 3:
            gnuplot_cmd(plot, "set terminal pdfcairo");
    }

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleLine)) == TRUE)
    {
        gnuplot_cmd(plot, "f(x) = a*x + b \n fit f(x) \'%s\' using 2:1 via a, b \n plot \'%s\' using 2:1, f(x)", (char*)gtk_entry_get_text(GTK_ENTRY(txtInputFile)), (char*)gtk_entry_get_text(GTK_ENTRY(txtInputFile)));

    }
    else
    {
        gnuplot_cmd(plot, "plot \"%s\" using 2:1");
    }

    gnuplot_close(plot);
}

void createOutputWindow()
{
    outputWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(outputWindow), "Output Configuration");
    gtk_widget_set_size_request(GTK_WIDGET(outputWindow), -1, -1);
    gtk_window_set_resizable(GTK_WINDOW(outputWindow), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(outputWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(outputWindow), TRUE);

    GtkWidget *frmGenerateGraph = gtk_frame_new(NULL);
    GtkWidget *lblGenerateGraph = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lblGenerateGraph), "<b>Graphing Settings</b>");
    gtk_widget_show(lblGenerateGraph);
    gtk_widget_show(frmGenerateGraph);
    gtk_frame_set_label_widget(GTK_FRAME(frmGenerateGraph), GTK_WIDGET(lblGenerateGraph));
    gtk_frame_set_shadow_type(GTK_FRAME(frmGenerateGraph), GTK_SHADOW_NONE);


    GtkWidget *tabSettingsTable = gtk_table_new(3, 3, FALSE);
    gtk_widget_show(GTK_WIDGET(tabSettingsTable));

    txtGraphTitle = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtGraphTitle));
    gtk_entry_set_text(GTK_ENTRY(txtGraphTitle), "UNTITLED TEST");
    GtkWidget *lblGraphTitle = gtk_label_new("Graph Title:");
    gtk_widget_show(GTK_WIDGET(lblGraphTitle));
    GtkWidget *algGraphTitle = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algGraphTitle));
    gtk_container_add(GTK_CONTAINER(algGraphTitle), GTK_WIDGET(lblGraphTitle));

    selInputFile = gtk_button_new_with_label("Select");
    gtk_widget_show(GTK_WIDGET(selInputFile));
    g_signal_connect(G_OBJECT(selInputFile), "clicked", G_CALLBACK(selectInputFile), NULL);
    txtInputFile = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtInputFile));
    GtkWidget *lblInputFile = gtk_label_new("Input File:");
    gtk_widget_show(GTK_WIDGET(lblInputFile));
    GtkWidget *algInputFile = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algInputFile));
    gtk_container_add(GTK_CONTAINER(algInputFile), GTK_WIDGET(lblInputFile));
    GtkWidget *hboxInput = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hboxInput));
    gtk_box_pack_start(GTK_BOX(hboxInput), GTK_WIDGET(txtInputFile), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hboxInput), GTK_WIDGET(selInputFile), FALSE, FALSE, 0);

    txtXAxisTitle = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtXAxisTitle));
    gtk_entry_set_text(GTK_ENTRY(txtXAxisTitle), "Strain (unitless)");
    GtkWidget *lblXAxisTitle = gtk_label_new("x-Axis Title:");
    gtk_widget_show(GTK_WIDGET(lblXAxisTitle));
    GtkWidget *algXAxisTitle = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algXAxisTitle));
    gtk_container_add(GTK_CONTAINER(algXAxisTitle), GTK_WIDGET(lblXAxisTitle));

    txtYAxisTitle = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtYAxisTitle));
    gtk_entry_set_text(GTK_ENTRY(txtYAxisTitle), "Position (mm or px)");
    GtkWidget *lblYAxisTitle = gtk_label_new("y-Axis Title:");
    gtk_widget_show(GTK_WIDGET(lblYAxisTitle));
    GtkWidget *algYAxisTitle = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algYAxisTitle));
    gtk_container_add(GTK_CONTAINER(algYAxisTitle), GTK_WIDGET(lblYAxisTitle));

    txtXAxisMin = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtXAxisMin));
    txtXAxisMax = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtXAxisMax));
    gtk_entry_set_text(GTK_ENTRY(txtXAxisMin), "-0.005");
    gtk_entry_set_text(GTK_ENTRY(txtXAxisMax), "0.005");
    GtkWidget *lblXAxisRange = gtk_label_new("x-Axis Range");
    gtk_widget_show(GTK_WIDGET(lblXAxisRange));
    xFrom = gtk_label_new("from:");
    gtk_widget_show(GTK_WIDGET(xFrom));
    xTo = gtk_label_new("to:");
    gtk_widget_show(GTK_WIDGET(xTo));
    toggleAutoX = gtk_toggle_button_new();
    gtk_widget_show(GTK_WIDGET(toggleAutoX));
    GtkWidget *imgAutoX = gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(imgAutoX));
    gtk_button_set_image(GTK_BUTTON(toggleAutoX), GTK_WIDGET(imgAutoX));
    g_signal_connect(G_OBJECT(toggleAutoX), "toggled", G_CALLBACK(toggleAutoXToggled), NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggleAutoX), TRUE);
    toggleAutoXToggled();
    GtkWidget *hboxRange1 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(GTK_WIDGET(hboxRange1));
    gtk_box_pack_start(GTK_BOX(hboxRange1), GTK_WIDGET(xFrom), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange1), GTK_WIDGET(txtXAxisMin), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange1), GTK_WIDGET(xTo), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange1), GTK_WIDGET(txtXAxisMax), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange1), GTK_WIDGET(toggleAutoX), FALSE, FALSE, 0);
    GtkWidget *algXAxisRange = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algXAxisRange));
    gtk_container_add(GTK_CONTAINER(algXAxisRange), GTK_WIDGET(lblXAxisRange));

    txtYAxisMin = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtYAxisMin));
    txtYAxisMax = gtk_entry_new();
    gtk_widget_show(GTK_WIDGET(txtYAxisMax));
    gtk_entry_set_text(GTK_ENTRY(txtYAxisMin), "-0.005");
    gtk_entry_set_text(GTK_ENTRY(txtYAxisMax), "0.005");
    GtkWidget *lblYAxisRange = gtk_label_new("y-Axis Range");
    gtk_widget_show(GTK_WIDGET(lblYAxisRange));
    yFrom = gtk_label_new("from:");
    gtk_widget_show(GTK_WIDGET(yFrom));
    yTo = gtk_label_new("to:");
    gtk_widget_show(GTK_WIDGET(yTo));
    toggleAutoY = gtk_toggle_button_new();
    gtk_widget_show(GTK_WIDGET(toggleAutoY));
    GtkWidget *imgAutoY = gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_BUTTON);
    gtk_widget_show(GTK_WIDGET(imgAutoY));
    gtk_button_set_image(GTK_BUTTON(toggleAutoY), GTK_WIDGET(imgAutoY));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggleAutoY), TRUE);
    toggleAutoYToggled();
    g_signal_connect(G_OBJECT(toggleAutoY), "toggled", G_CALLBACK(toggleAutoYToggled), NULL);
    GtkWidget *hboxRange2 = gtk_hbox_new(FALSE, 2);
    gtk_widget_show(GTK_WIDGET(hboxRange2));
    gtk_box_pack_start(GTK_BOX(hboxRange2), GTK_WIDGET(yFrom), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange2), GTK_WIDGET(txtYAxisMin), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange2), GTK_WIDGET(yTo), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange2), GTK_WIDGET(txtYAxisMax), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hboxRange2), GTK_WIDGET(toggleAutoY), FALSE, FALSE, 0);
    GtkWidget *algYAxisRange = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algYAxisRange));
    gtk_container_add(GTK_CONTAINER(algYAxisRange), GTK_WIDGET(lblYAxisRange));

    toggleReverseY = gtk_check_button_new_with_label("Reverse y-Axis Range");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggleReverseY), TRUE);
    toggleReverseX = gtk_check_button_new_with_label("Reverse x-Axis Range");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggleReverseX), FALSE);
    gtk_widget_show(GTK_WIDGET(toggleReverseY));
    gtk_widget_show(GTK_WIDGET(toggleReverseX));
    GtkWidget *hbox3 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox3));
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toggleReverseX), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toggleReverseY), FALSE, FALSE, 0);

    toggleGrid = gtk_check_button_new_with_label("Show Grid");
    gtk_widget_show(GTK_WIDGET(toggleGrid));

    toggleKey = gtk_check_button_new_with_label("Show Legend");
    gtk_widget_show(GTK_WIDGET(toggleKey));

    toggleLine = gtk_check_button_new_with_label("Plot Best Fit Line");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggleLine), TRUE);
    gtk_widget_show(GTK_WIDGET(toggleLine));

    GtkWidget *hbox4 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox4));
    gtk_box_pack_start(GTK_BOX(hbox4), GTK_WIDGET(toggleGrid), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox4), GTK_WIDGET(toggleKey), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox4), GTK_WIDGET(toggleLine), FALSE, FALSE, 0);

    cmbOutputFormat = gtk_combo_box_text_new();
    gtk_widget_show(GTK_WIDGET(cmbOutputFormat));
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbOutputFormat), "GIF Image");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbOutputFormat), "JPEG Image");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbOutputFormat), "PNG Image");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cmbOutputFormat), "PDF Document");
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmbOutputFormat), 3);
    g_signal_connect(G_OBJECT(cmbOutputFormat), "changed", G_CALLBACK(outputFormatChanged), NULL);
    GtkWidget *lblOutputFormat = gtk_label_new("Output Format:");
    gtk_widget_show(GTK_WIDGET(lblOutputFormat));
    GtkWidget *algOutputFormat = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algOutputFormat));
    gtk_container_add(GTK_CONTAINER(algOutputFormat), GTK_WIDGET(lblOutputFormat));

    char *homePath = getenv("HOME");
    char copy_homePath[strlen(homePath) + 1];
    strcpy(copy_homePath, homePath);
    strncat(copy_homePath, "/newgraph.pdf", 13);
    selOutputImage = gtk_button_new_with_label("Select");
    gtk_widget_show(GTK_WIDGET(selOutputImage));
    g_signal_connect(G_OBJECT(selOutputImage), "clicked", G_CALLBACK(selectOutputImage), NULL);
    txtOutputImage = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtOutputImage), copy_homePath);
    gtk_widget_show(GTK_WIDGET(txtOutputImage));
    GtkWidget *lblOutputFile = gtk_label_new("Output File:");
    gtk_widget_show(GTK_WIDGET(lblOutputFile));
    GtkWidget *algOutputFile = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algOutputFile));
    gtk_container_add(GTK_CONTAINER(algOutputFile), GTK_WIDGET(lblOutputFile));
    GtkWidget *hboxOutput = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hboxOutput));
    gtk_box_pack_start(GTK_BOX(hboxOutput), GTK_WIDGET(txtOutputImage), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hboxOutput), GTK_WIDGET(selOutputImage), FALSE, FALSE, 0);

    widthAdjustment = gtk_adjustment_new(640, 320, 2048, 1, 100, 0.0);
    heightAdjustment = gtk_adjustment_new(480, 240, 1536, 1, 100, 0.0);
    widthSpin = gtk_spin_button_new(GTK_ADJUSTMENT(widthAdjustment), 1.0, 0);
    heightSpin = gtk_spin_button_new(GTK_ADJUSTMENT(heightAdjustment), 1.0, 0);
    gtk_widget_show(GTK_WIDGET(widthSpin));
    gtk_widget_show(GTK_WIDGET(heightSpin));
    lblDimAdjustment = gtk_label_new("Width/Height:");
    lblx = gtk_label_new("by");
    lblpx1 = gtk_label_new("(pix)");
    lblpx2 = gtk_label_new("(pix)");
    GtkWidget *algDimAdj = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(lblDimAdjustment));
    gtk_widget_show(GTK_WIDGET(lblx));
    gtk_widget_show(GTK_WIDGET(lblpx1));
    gtk_widget_show(GTK_WIDGET(lblpx2));
    gtk_widget_show(GTK_WIDGET(algDimAdj));
    gtk_container_add(GTK_CONTAINER(algDimAdj), GTK_WIDGET(lblDimAdjustment));

    GtkWidget *hbox5 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox5));

    gtk_widget_set_sensitive(GTK_WIDGET(widthSpin), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(heightSpin), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblx), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblDimAdjustment), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblpx2), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(lblpx1), FALSE);

    gtk_box_pack_start(GTK_BOX(hbox5), GTK_WIDGET(widthSpin), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), GTK_WIDGET(lblpx1), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), GTK_WIDGET(lblx), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), GTK_WIDGET(heightSpin), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox5), GTK_WIDGET(lblpx2), FALSE, FALSE, 0);

    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algGraphTitle),    0, 1, 0, 1, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(txtGraphTitle),    1, 2, 0, 1, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algInputFile),     0, 1, 1, 2, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hboxInput),        1, 2, 1, 2, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algXAxisTitle),    0, 1, 2, 3, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(txtXAxisTitle),    1, 2, 2, 3, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algYAxisTitle),    0, 1, 3, 4, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(txtYAxisTitle),    1, 2, 3, 4, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algXAxisRange),    0, 1, 4, 5, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hboxRange1),       1, 2, 4, 5, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algYAxisRange),    0, 1, 5, 6, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hboxRange2),       1, 2, 5, 6, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hbox3),            1, 2, 6, 7, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hbox4),            1, 2, 7, 8, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algOutputFormat),  0, 1, 8, 9, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(cmbOutputFormat),  1, 2, 8, 9, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algOutputFile),    0, 1, 9, 10, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hboxOutput),       1, 2, 9, 10, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(algDimAdj),        0, 1, 10, 11, GTK_FILL, GTK_FILL, 6, 0);
    gtk_table_attach(GTK_TABLE(tabSettingsTable), GTK_WIDGET(hbox5),            1, 2, 10, 11, GTK_FILL, GTK_FILL, 6, 0);

    gtk_table_set_row_spacings(GTK_TABLE(tabSettingsTable), 5);
    GtkWidget *vbox2 = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(tabSettingsTable), FALSE, FALSE, 6);
    gtk_widget_show(GTK_WIDGET(vbox2));
    gtk_container_add(GTK_CONTAINER(frmGenerateGraph), GTK_WIDGET(vbox2));

    GtkWidget *vbox1 = gtk_vbox_new(FALSE, 3);
    gtk_widget_show(GTK_WIDGET(vbox1));

    GtkWidget *btnGenerateGraph = gtk_button_new_with_label("Generate Graph");
    gtk_widget_show(GTK_WIDGET(btnGenerateGraph));
    g_signal_connect(G_OBJECT(btnGenerateGraph), "clicked", G_CALLBACK(createGraph), NULL);
    GtkWidget *btnCloseWindow = gtk_button_new_with_label("Close");
    gtk_widget_show(GTK_WIDGET(btnCloseWindow));
    GtkWidget *hbox1 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox1));
    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnGenerateGraph), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnCloseWindow), FALSE, FALSE, 9);
    GtkWidget *algButtons = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_widget_show(GTK_WIDGET(algButtons));
    gtk_container_add(GTK_CONTAINER(algButtons), GTK_WIDGET(hbox1));

    GtkWidget *hbox2 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(GTK_WIDGET(hbox2));

    gtk_box_pack_start(GTK_BOX(hbox2), GTK_WIDGET(frmGenerateGraph), TRUE, TRUE, 12);
    gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(hbox2), FALSE, FALSE, 12);
    gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(algButtons), FALSE, FALSE, 12);

    gtk_container_add(GTK_CONTAINER(outputWindow), GTK_WIDGET(vbox1));

    g_signal_connect(G_OBJECT(outputWindow), "delete_event", G_CALLBACK(hideOutputWindow), NULL);
    g_signal_connect(G_OBJECT(btnCloseWindow), "clicked", G_CALLBACK(hideOutputWindow), NULL);
}

// End of File
