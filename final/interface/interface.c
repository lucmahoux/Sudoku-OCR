#include <string.h>
#include <gtk/gtk.h>
#include "../imgProcessing/imageProcess.h"

static GtkBuilder* builder;
static GtkWidget* window;
static GtkWidget* fixed;
static GtkWidget* fileChooser;
static GtkWidget* button;
static GtkWidget* label;
static GtkWidget* preview;
static char* filename;
static int cptEaster;

/**
 * \brief Check if the file given is an image (.png or .jpg)
 * \param len Length of the filename tested
 * \sa isValidImg
 */
int isValidImg(int len){
    if(len > 4){
        return filename[len-4] == '.' && filename[len-1] == 'g'
        && ((filename[len-3] == 'p'&& filename[len-2] == 'n')
        || (filename[len-3] == 'j'&& filename[len-2] == 'p'));
    }
    return 0;
}

/**
 * \brief Update the preview to match the actual filename
 * \param fc FileChooser used to choose the file
 * \sa updateImage
 */
void updateImage(GtkFileChooser *fc){
    filename = gtk_file_chooser_get_preview_filename(fc);
    if(filename){
        if(isValidImg(strlen(filename))){
            gtk_label_set_text(GTK_LABEL(label), "Valid Format");
        } else {
            gtk_label_set_text(GTK_LABEL(label), ".png or .jpg");
        }
        gtk_image_set_from_file(GTK_IMAGE(preview), (const gchar*) filename);
    }
}

/**
 * \brief Can call the entire project if the file given is valid
 * \sa callOCRProject
 */
void callOCRProject(){
    if(filename){
        size_t lenF = strlen(filename);
        if(lenF == 0) {  //EasterEgg
            switch(cptEaster) {
            case 0:
                gtk_label_set_text(GTK_LABEL(label), "Choose Your Grid !");
                break;
            case 1:
                gtk_label_set_text(GTK_LABEL(label), "CHOOSE YOUR GRID !");
                break;
            case 2:
                gtk_label_set_text(GTK_LABEL(label), "CHOOSE IT !!!!!!");
                break;
            case 3:
                gtk_label_set_text(GTK_LABEL(label), "You know what...");
                break;
            case 4:
                gtk_label_set_text(GTK_LABEL(label), "Just forget about it.");
                break;
            case 5:
                gtk_label_set_text(GTK_LABEL(label), "             ...");
                break;
            }
            cptEaster += cptEaster <= 5;
        }
        else if(lenF > 4) {
            if(isValidImg(lenF)) {
                //Call main_Interface
                gtk_label_set_text(GTK_LABEL(label), "Processing...");
                if(main_ImageProcess(filename)){
                    gtk_label_set_text(GTK_LABEL(label), "Tadaaaaaa!");
                    gtk_image_set_from_file(GTK_IMAGE(preview), (const gchar*)
                        "results/step9_Solved.png");
                } else {
                    gtk_label_set_text(GTK_LABEL(label), "No Solution Found !");
                }
            } else {
                gtk_label_set_text(GTK_LABEL(label), ".PNG OR .JPG");
            }
        }
    }
}

/**
 * \brief Initialize everything about the interface
 * \param argc Used to intialize gtk in case specific arguments are given
 * \param argv Used to intialize gtk in case specific arguments are given
 * \sa main_Interface
 */
int main_Interface(int argc, char** argv) {
    gtk_init(&argc, &argv);
    filename = "";
    cptEaster = 0;

    builder = gtk_builder_new_from_file("interface.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));
    fileChooser = GTK_WIDGET(gtk_builder_get_object(builder, "fileChooser"));
    button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
    label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));
    preview = GTK_WIDGET(gtk_builder_get_object(builder, "preview"));

    g_signal_connect(window, "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_FILE_CHOOSER(fileChooser), "update-preview",
        G_CALLBACK (updateImage), NULL);
    g_signal_connect(GTK_BUTTON(button), "clicked",
        G_CALLBACK(callOCRProject), NULL);

    gtk_widget_show(window);
    gtk_main();

    return EXIT_SUCCESS;
}
