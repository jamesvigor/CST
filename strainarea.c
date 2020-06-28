#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>
#include <gtkimageview/gtkimagetooldragger.h>
#include <gtkimageview/gtkimagetoolselector.h>

#include "guielements.h"
#include "constants.h"

#define WIDTH_REQUEST 75

void updateStrain()
{
    strainArea.x = atoi(gtk_entry_get_text(GTK_ENTRY(txtMinX)));
    strainArea.y = atoi(gtk_entry_get_text(GTK_ENTRY(txtMinY)));
    strainArea.width = atoi(gtk_entry_get_text(GTK_ENTRY(txtMaxX))) - atoi(gtk_entry_get_text(GTK_ENTRY(txtMinX)));
    strainArea.height = atoi(gtk_entry_get_text(GTK_ENTRY(txtMaxY))) - atoi(gtk_entry_get_text(GTK_ENTRY(txtMinY)));;
    gtk_image_tool_selector_set_selection(GTK_IMAGE_TOOL_SELECTOR(strainSelector), &strainArea);
}

void showStrainWindow()
{   if(countReferenceImages != 0)
    {
        gtk_image_view_get_tool(GTK_IMAGE_VIEW(imageView));
        gtk_image_view_set_tool(GTK_IMAGE_VIEW(imageView), GTK_IIMAGE_TOOL(strainSelector));
        gtk_image_tool_selector_set_selection(GTK_IMAGE_TOOL_SELECTOR(strainSelector), &strainArea);
    }

    gtk_widget_show(GTK_WIDGET(strainselectWindow));

}

void hideStrainWindow()
{
    GtkIImageTool *dragTool = gtk_image_tool_dragger_new(GTK_IMAGE_VIEW(imageView));
    gtk_image_view_set_tool(GTK_IMAGE_VIEW(imageView), GTK_IIMAGE_TOOL(dragTool));
    gtk_widget_hide(GTK_WIDGET(strainselectWindow));
}

void createStrainWindow()
{
    strainselectWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(strainselectWindow), "Set Measurement Area");
    gtk_widget_set_size_request(GTK_WIDGET(strainselectWindow), -1, -1);
    gtk_window_set_resizable(GTK_WINDOW(strainselectWindow), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(strainselectWindow), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(strainselectWindow), TRUE);

    GtkWidget *settingsTable = gtk_table_new(3, 3, FALSE);
    gtk_widget_show(GTK_WIDGET(settingsTable));

    GtkWidget *lblX = gtk_label_new("x");
    GtkWidget *lblY = gtk_label_new("y");

    GtkWidget *lblMin = gtk_label_new("Minimum:");
    GtkWidget *algMin = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algMin), GTK_WIDGET(lblMin));
    gtk_widget_show(algMin);

    GtkWidget *lblMax = gtk_label_new("Maximum:");
    GtkWidget *algMax = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algMax), GTK_WIDGET(lblMax));
    gtk_widget_show(algMax);

    gtk_widget_show(GTK_WIDGET(lblX));
    gtk_widget_show(GTK_WIDGET(lblY));
    gtk_widget_show(GTK_WIDGET(lblMin));
    gtk_widget_show(GTK_WIDGET(lblMax));

    txtMinX = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtMinX), "0000");
    gtk_widget_set_size_request(GTK_WIDGET(txtMinX), WIDTH_REQUEST, -1);
    txtMinY = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtMinY), "0000");
    gtk_widget_set_size_request(GTK_WIDGET(txtMinY), WIDTH_REQUEST, -1);
    txtMaxX = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtMaxX), "0000");
    gtk_widget_set_size_request(GTK_WIDGET(txtMaxX), WIDTH_REQUEST, -1);
    txtMaxY = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(txtMaxY), "0000");
    gtk_widget_set_size_request(GTK_WIDGET(txtMaxY), WIDTH_REQUEST, -1);

    gtk_widget_show(GTK_WIDGET(txtMinX));
    gtk_widget_show(GTK_WIDGET(txtMinY));
    gtk_widget_show(GTK_WIDGET(txtMaxX));
    gtk_widget_show(GTK_WIDGET(txtMaxY));

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblX), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblY), 2, 3, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algMin), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(txtMinX), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(txtMinY), 2, 3, 1, 2, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algMax), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(txtMaxX), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(txtMaxY), 2, 3, 2, 3, GTK_FILL, GTK_FILL, 12, 0);

    gtk_table_set_row_spacings(GTK_TABLE(settingsTable), 5);

    GtkWidget *vbox1 = gtk_vbox_new(FALSE, 5);
    gtk_widget_show(vbox1);
    GtkWidget *hbox1 = gtk_hbox_new(FALSE, 5);
    gtk_widget_show(GTK_WIDGET(hbox1));
    GtkWidget *btnOK = gtk_button_new_from_stock(GTK_STOCK_OK);
    gtk_widget_show(btnOK);
    gtk_widget_set_size_request(GTK_WIDGET(btnOK), 100, -1);

    GtkWidget *lblPhysicalDimensions = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lblPhysicalDimensions), "<b>Physical Dimensions:</b>");
    gtk_widget_show(GTK_WIDGET(lblPhysicalDimensions));

    GtkWidget *lblWidth = gtk_label_new("Width:");
    GtkWidget *algWidth = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algWidth), GTK_WIDGET(lblWidth));
    gtk_widget_show(algWidth);
    gtk_widget_show(lblWidth);

    GtkWidget *lblMM1 = gtk_label_new("mm");
    GtkWidget *algMM1 = gtk_alignment_new(0.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algMM1), GTK_WIDGET(lblMM1));
    gtk_widget_show(algMM1);
    gtk_widget_show(lblMM1);

    GtkWidget *lblHeight = gtk_label_new("Height:");
    GtkWidget *algHeight = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algHeight), GTK_WIDGET(lblHeight));
    gtk_widget_show(algHeight);
    gtk_widget_show(lblHeight);

    GtkWidget *lblMM2 = gtk_label_new("mm");
    GtkWidget *algMM2 = gtk_alignment_new(0.0, 0.5, 0.0, 0.0);
    gtk_container_add(GTK_CONTAINER(algMM2), GTK_WIDGET(lblMM2));
    gtk_widget_show(algMM2);
    gtk_widget_show(lblMM2);

    txtPhysWidth = gtk_entry_new();
    gtk_widget_show(txtPhysWidth);
    gtk_widget_set_size_request(GTK_WIDGET(txtPhysWidth), WIDTH_REQUEST, -1);
    gtk_entry_set_text(GTK_ENTRY(txtPhysWidth), "0.0");

    txtPhysHeight = gtk_entry_new();
    gtk_widget_show(txtPhysHeight);
    gtk_widget_set_size_request(GTK_WIDGET(txtPhysHeight), WIDTH_REQUEST, -1);
    gtk_entry_set_text(GTK_ENTRY(txtPhysHeight), "0.0");

    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(lblPhysicalDimensions),   0, 3, 3, 4, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algWidth),                0, 1, 4, 5, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(txtPhysWidth),            1, 2, 4, 5, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algMM1),                  2, 3, 4, 5, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algHeight),               0, 1, 5, 6, GTK_FILL, GTK_FILL, 12, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(txtPhysHeight),           1, 2, 5, 6, GTK_FILL, GTK_FILL, 0, 0);
    gtk_table_attach(GTK_TABLE(settingsTable), GTK_WIDGET(algMM2),                  2, 3, 5, 6, GTK_FILL, GTK_FILL, 12, 0);

    gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(btnOK), TRUE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(settingsTable), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(hbox1), FALSE, FALSE, 6);

    gtk_container_add(GTK_CONTAINER(strainselectWindow), GTK_WIDGET(vbox1));

    g_signal_connect(G_OBJECT(strainselectWindow), "delete_event", G_CALLBACK(hideStrainWindow), NULL);
    g_signal_connect(G_OBJECT(btnOK), "clicked", G_CALLBACK(hideStrainWindow), NULL);

    g_signal_connect(G_OBJECT(txtMinX), "activate", G_CALLBACK(updateStrain), NULL);
    g_signal_connect(G_OBJECT(txtMinY), "activate", G_CALLBACK(updateStrain), NULL);
    g_signal_connect(G_OBJECT(txtMaxX), "activate", G_CALLBACK(updateStrain), NULL);
    g_signal_connect(G_OBJECT(txtMaxY), "activate", G_CALLBACK(updateStrain), NULL);
}
