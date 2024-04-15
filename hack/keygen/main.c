#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

GtkWidget *fileChooser = NULL, *passwordInput = NULL;

static uint16_t hashFunc(const char *password) {
    int len = strlen(password);

    uint16_t bx = 0;

    while(len-->0)
    {
        uint8_t al = *(password++);
        *(((uint8_t*)&bx)) ^= al;
        bx >>= 1;
        *(((uint8_t*)&bx)+1) += al;
        bx <<= 3;
        bx = -bx;
        bx <<= 4;
    }

    return bx;
}

static int crack(const char *filename, const char *password) {
    FILE *file = fopen(filename, "rb");
    if(!file)
        return 1;

    uint8_t buffer[1024];
    size_t fileSize = fread(buffer, sizeof(*buffer), sizeof(buffer), file);
    fclose(file);

    uint16_t hash = hashFunc(password);

    *((uint16_t*) &buffer[0xa4]) = hash;

    file = fopen(filename, "wb");
    if(!file)
        return 1;
    fwrite(buffer, sizeof(*buffer), fileSize, file);
    fclose(file);

    return 0;
}

static void clicked(GtkWidget *widget, gpointer data)
{
    const char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileChooser));
    const char *password = gtk_entry_get_text(GTK_ENTRY(passwordInput));

    GtkWindow *window = GTK_WINDOW(data);

    int status = crack(filename, password);
    GtkWidget *dialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL,
            !status ? GTK_MESSAGE_INFO : GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            !status ? "Success" : "Error");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
 
static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}
 
int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);
 
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "KeyGen");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    fileChooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
    passwordInput = gtk_entry_new();
    GtkWidget *button = gtk_button_new_with_label("Crack!");

    g_signal_connect(button, "clicked", G_CALLBACK(clicked),   window);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy),   NULL);
 
    gtk_container_add(GTK_CONTAINER(box), fileChooser);
    gtk_container_add(GTK_CONTAINER(box), passwordInput);
    gtk_container_add(GTK_CONTAINER(box), button);
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_widget_show_all(window);
    gtk_main();
 
    return 0;
}


