#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtkimageview/gtkimageview.h>
#include <gtkimageview/gtkimagetooldragger.h>
#include <gtkimageview/gtkimagetoolselector.h>

#include "guielements.h"
#include "constants.h"
#include "strainarea.h"

void zoomIn()
{
    gtk_image_view_zoom_in(GTK_IMAGE_VIEW(imageView));
}

void zoomOut()
{
    gtk_image_view_zoom_out(GTK_IMAGE_VIEW(imageView));
}

void zoom100()
{
    gtk_image_view_set_zoom(GTK_IMAGE_VIEW(imageView), 1.0);
}

void zoomFit()
{
    gtk_image_view_set_zoom(GTK_IMAGE_VIEW(imageView), -1);
}

void imageSelected()
{
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(cmbSelectImage));

    if(countReferenceImages == 1 || countReferenceImages == 0)
    {
        // Only one image loaded, do nothing
        return;
    }

    // get the requested image path from the list of loaded deformed images
    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
    int i;
    for(i = 0; i < index; i++)
    {
        gtk_tree_model_iter_next(GTK_TREE_MODEL(referenceImages), &referenceImageIter);
    }

    char *filepath;
    gtk_tree_model_get(GTK_TREE_MODEL(referenceImages), &referenceImageIter, 0, &filepath, -1);
    printf("Image For Load: %s\n", filepath);

    currentImage = gdk_pixbuf_new_from_file(filepath, NULL);
    gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imageView), currentImage, TRUE);

    char message[500];
    sprintf(message, "Currently displaying image: %s", filepath);
    gtk_statusbar_push(GTK_STATUSBAR(stbMainStatusBar), 0, message);
}

void selDragTool()
{
    GtkIImageTool *dragTool = gtk_image_tool_dragger_new(GTK_IMAGE_VIEW(imageView));
    gtk_image_view_set_tool(GTK_IMAGE_VIEW(imageView), GTK_IIMAGE_TOOL(dragTool));
}

void strainSelectionChanged()
{
    gtk_image_tool_selector_get_selection(GTK_IMAGE_TOOL_SELECTOR(strainSelector), &strainArea);

    char num[10];
    sprintf(num, "%d", strainArea.x);
    gtk_entry_set_text(GTK_ENTRY(txtMinX), num);
    sprintf(num, "%d", strainArea.y);
    gtk_entry_set_text(GTK_ENTRY(txtMinY), num);
    sprintf(num, "%d", strainArea.x + strainArea.width);
    gtk_entry_set_text(GTK_ENTRY(txtMaxX), num);
    sprintf(num, "%d", strainArea.y + strainArea.height);
    gtk_entry_set_text(GTK_ENTRY(txtMaxY), num);
}

void addStrainArea()
{
    showStrainWindow();
    g_signal_connect(G_OBJECT(strainSelector), "selection-changed", G_CALLBACK(strainSelectionChanged), NULL);
}


