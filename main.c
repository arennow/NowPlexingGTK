#include <gtk/gtk.h>
#include "PlexMessage-Umbrella.h"
#include <stdlib.h>
#include "downloader.h"
#include <assert.h>
#include <curl/curl.h>
#include <stdbool.h>

#ifdef DEBUG
#include <stdio.h>
#endif

static GtkWidget* __window;
static GtkWidget* __imageWidget;

static void destroy(GtkWidget* widget, gpointer data) {
	gtk_main_quit();
}

static gboolean delete_event(GtkWidget* widget, GdkEvent* event, gpointer data) {
	gtk_main_quit();
	return FALSE;
}

static void getScreenSize(gint* width, gint* height) {
	assert(width);
	assert(height);
	
	GdkScreen* screen = gtk_window_get_screen(GTK_WINDOW(__window));
	*width = gdk_screen_get_width(screen);
	*height = gdk_screen_get_height(screen);
}

static gboolean setPosterImageToContentsOfFile(gpointer inData) {
	assert(__imageWidget);
	DownloadConfig* downloadConfig = inData;
	
	gint width, height;
	getScreenSize(&width, &height);
	
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file_at_size(downloadConfig->localFilePath, width, height, NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(__imageWidget), pixbuf);
	g_object_unref(pixbuf);
	
	downloadConfig_free(downloadConfig);
	
	return G_SOURCE_REMOVE;
}

static gboolean setPosterImageToBlank(gpointer inData) {
	assert(__imageWidget);
	
	gtk_image_clear(GTK_IMAGE(__imageWidget));
	
	return G_SOURCE_REMOVE;
}

static void handlePosterDownloadCompletion(DownloadConfig* downloadConfig) {
	g_main_context_invoke(NULL, setPosterImageToContentsOfFile, (void*)downloadConfig);
}

static void handlePlexMessage(PlexMessage* message) {
	char* string = plexMessage_description(message);
	puts(string);
	free(string);
	
	if (strcmp(message->accountName, "arennow")) { goto cleanup; }
	if (message->mediaType == PlexMediaTypeUnknown) { goto cleanup; }
	
	if (message->event == PlexEventStop) {
		g_main_context_invoke(NULL, setPosterImageToBlank, NULL);
	} else {
		DownloadConfig* config = malloc(sizeof(DownloadConfig));
		downloadConfig_init(config, message->imageURL, "7cvGga8sD98L-bQPbnwe", handlePosterDownloadCompletion);
		downloadConfig_perform(config);
	}
	
	cleanup:
	plexMessage_free(message);
}

static gpointer start_server(gpointer data) {
	plexMessageReceivedCallback = handlePlexMessage;
	httpServer_start();
	
	return NULL;
}

static void hideCursor(void) {
	GdkCursor* cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_BLANK_CURSOR);
	gdk_window_set_cursor(gtk_widget_get_window(__window), cursor);
}

int main(int argc, char *argv[]) {
	curl_global_init(CURL_GLOBAL_ALL);
	gtk_init(&argc, &argv);

	__window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(__window, "delete-event", G_CALLBACK (delete_event), NULL);

	g_signal_connect(__window, "destroy", G_CALLBACK (destroy), NULL);

	__imageWidget = gtk_image_new();
	gtk_container_add(GTK_CONTAINER(__window), __imageWidget);
	
	GdkRGBA black = {.alpha = 1};
	gtk_widget_override_background_color(__window, 0, &black);
	
	gtk_window_fullscreen(GTK_WINDOW(__window));
	gtk_widget_show(__imageWidget);
	gtk_widget_show(__window);
	
	g_thread_new("server", start_server, NULL);
	
	hideCursor();
	
	gtk_main();

	return 0;
}