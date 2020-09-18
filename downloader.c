#include "downloader.h"
#include <curl/curl.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <assert.h>
#include "convenience.h"

static char const * const __destPath = "/tmp/nowPlexing_XXXXXX.jpg";

void downloadConfig_init(DownloadConfig* config, const char* url, const char* token, DownloadCompletion completion) {
	config->used = false;
	config->url = allocating_stringCopy(url);
	config->localFilePath = NULL;
	config->token = allocating_stringCopy(token);
	config->completion = completion;
}

void downloadConfig_free(DownloadConfig* config) {
	freeIfNonNull(config->url);
	freeIfNonNull(config->localFilePath);
	freeIfNonNull(config->token);
	
	// memset(config, 0, sizeof(*config));
	free(config);
}

static gpointer impl_downloadFileAtURL(gpointer inData) {
	struct DownloadConfig* config = inData;
	
	CURL* curl_handle = curl_easy_init();

	/* set URL to get here */
	curl_easy_setopt(curl_handle, CURLOPT_URL, config->url);

	/* disable progress meter, set to 0L to enable and disable debug output */
	curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

	char* tokenHeaderString;
	asprintf(&tokenHeaderString, "X-Plex-Token: %s", config->token);
	
	struct curl_slist *list = NULL;
	list = curl_slist_append(list, tokenHeaderString);
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
	
	free(tokenHeaderString);
	
	config->localFilePath = allocating_stringCopy(__destPath);
	FILE* destFile = fdopen(mkstemps(config->localFilePath, 4), "w");

	if (destFile) {
		/* write the page body to this file handle */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, destFile);

		/* get it! */
		curl_easy_perform(curl_handle);

		/* close the header file */
		fclose(destFile);
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
	
	config->completion(config);
	
	return NULL;
}

void downloadConfig_perform(DownloadConfig* config) {
	assert(!config->used);
	config->used = true;
	
	GThread* downloadThread = g_thread_new("downloader", impl_downloadFileAtURL, config);
}