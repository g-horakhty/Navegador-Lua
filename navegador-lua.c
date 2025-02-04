#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *web_view;
    GtkWidget *back_button;
    GtkWidget *forward_button;
    GtkWidget *reload_button;
    GtkWidget *url_entry;
    GtkWidget *history_button;
    GList *history;
    GList *current_history_item;
} BrowserData;

// Função para exibir a janela "Sobre" com algumas informações sobre projeto Alpha
static void show_about_dialog(GtkButton *button, gpointer data) {
    GtkWidget *about_dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Navegador Lua");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "© 2025 Gabriel Horakhty");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "Um navegador simples desenvolvido em C com WebKitGTK.");
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about_dialog), GTK_LICENSE_GPL_3_0);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "https://github.com/g-horakhty/Navegador-Lua/");
    gtk_dialog_run(GTK_DIALOG(about_dialog));
    gtk_widget_destroy(about_dialog);
}

// Função para exibir o histórico de navegação (Bastante limitado)
static void show_history_dialog(GtkButton *button, gpointer data) {
    BrowserData *browser_data = (BrowserData *)data;
    GtkWidget *dialog, *content_area, *scroll, *box, *label;
    GList *item;

    dialog = gtk_dialog_new_with_buttons("Histórico de Navegação",
                                         GTK_WINDOW(browser_data->window),
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         "Fechar",
                                         GTK_RESPONSE_CLOSE,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    scroll = gtk_scrolled_window_new(NULL, NULL);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scroll), box);
    gtk_container_add(GTK_CONTAINER(content_area), scroll);

    for (item = browser_data->history; item != NULL; item = item->next) {
        label = gtk_label_new((gchar *)item->data);
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    }

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void destroy_window_cb(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

static void uri_changed_cb(WebKitWebView *web_view, GParamSpec *pspec, gpointer data) {
    BrowserData *browser_data = (BrowserData *)data;
    gchar *title;
    gchar *uri = webkit_web_view_get_uri(web_view);

    title = g_strdup_printf("Lua Browser - %s", uri);
    gtk_window_set_title(GTK_WINDOW(browser_data->window), title);
    g_free(title);

    gtk_entry_set_text(GTK_ENTRY(browser_data->url_entry), uri);

    // Adicionar ao histórico
    if (browser_data->current_history_item == NULL || strcmp(uri, (char *)browser_data->current_history_item->data) != 0) {
        browser_data->history = g_list_append(browser_data->history, g_strdup(uri));
        browser_data->current_history_item = g_list_last(browser_data->history);
    }
}

static void go_back_cb(GtkButton *button, gpointer data) {
    BrowserData *browser_data = (BrowserData *)data;
    if (browser_data->current_history_item != NULL && browser_data->current_history_item->prev != NULL) {
        browser_data->current_history_item = browser_data->current_history_item->prev;
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(browser_data->web_view), (gchar *)browser_data->current_history_item->data);
    }
}

static void go_forward_cb(GtkButton *button, gpointer data) {
    BrowserData *browser_data = (BrowserData *)data;
    if (browser_data->current_history_item != NULL && browser_data->current_history_item->next != NULL) {
        browser_data->current_history_item = browser_data->current_history_item->next;
        webkit_web_view_load_uri(WEBKIT_WEB_VIEW(browser_data->web_view), (gchar *)browser_data->current_history_item->data);
    }
}

static void reload_cb(GtkButton *button, gpointer data) {
    BrowserData *browser_data = (BrowserData *)data;
    webkit_web_view_reload(WEBKIT_WEB_VIEW(browser_data->web_view));
}

static void load_url_cb(GtkEntry *entry, gpointer data) {
    BrowserData *browser_data = (BrowserData *)data;
    const gchar *url = gtk_entry_get_text(entry);
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(browser_data->web_view), url);
}

int main(int argc, char* argv[]) {
    GtkWidget *window;
    GtkWidget *web_view;
    GtkWidget *scrolled_window;
    GtkWidget *toolbar;
    GtkWidget *back_button;
    GtkWidget *forward_button;
    GtkWidget *reload_button;
    GtkWidget *history_button;
    GtkWidget *about_button;
    GtkWidget *url_entry;
    GtkWidget *box;

    BrowserData browser_data;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy_window_cb), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, FALSE, 0);

    // Botão "Voltar" com ícone simples
    back_button = gtk_button_new_from_icon_name("go-previous", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(back_button, "clicked", G_CALLBACK(go_back_cb), &browser_data);
    gtk_box_pack_start(GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);

    // Botão "Avançar" com ícone simples
    forward_button = gtk_button_new_from_icon_name("go-next", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(forward_button, "clicked", G_CALLBACK(go_forward_cb), &browser_data);
    gtk_box_pack_start(GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);

    // Botão "Recarregar" com ícone simples
    reload_button = gtk_button_new_from_icon_name("view-refresh", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(reload_button, "clicked", G_CALLBACK(reload_cb), &browser_data);
    gtk_box_pack_start(GTK_BOX(toolbar), reload_button, FALSE, FALSE, 0);

    // Barra de endereço ainda que meio transparente
    url_entry = gtk_entry_new();
    g_signal_connect(url_entry, "activate", G_CALLBACK(load_url_cb), &browser_data);
    gtk_box_pack_start(GTK_BOX(toolbar), url_entry, TRUE, TRUE, 0);

    // Botão "Histórico" com ícone 
    history_button = gtk_button_new_from_icon_name("document-open-recent", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(history_button, "clicked", G_CALLBACK(show_history_dialog), &browser_data);
    gtk_box_pack_start(GTK_BOX(toolbar), history_button, FALSE, FALSE, 0);

    // Botão "Sobre" com ícone amarelo
    about_button = gtk_button_new_from_icon_name("help-about", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about_dialog), NULL);
    gtk_box_pack_start(GTK_BOX(toolbar), about_button, FALSE, FALSE, 0);

    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

    web_view = webkit_web_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), web_view);

    browser_data.window = window;
    browser_data.web_view = web_view;
    browser_data.back_button = back_button;
    browser_data.forward_button = forward_button;
    browser_data.reload_button = reload_button;
    browser_data.url_entry = url_entry;
    browser_data.history = NULL;
    browser_data.current_history_item = NULL;

    g_signal_connect(web_view, "notify::uri", G_CALLBACK(uri_changed_cb), &browser_data);
    // Se for desejado pode trocar por qualquer outra URL que desejar
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "https://www.google.com"); 

    gtk_widget_grab_focus(web_view);
    gtk_widget_show_all(window);

    gtk_main();

    // Liberar histórico pois não existe uma função ainda de armazenar essa informação nem remotamente nem localmente
    g_list_free_full(browser_data.history, g_free);

    return 0;
}
