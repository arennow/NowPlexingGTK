#ifndef downloader_h
#define downloader_h

#include <stdbool.h>

struct DownloadConfig;

typedef void(*DownloadCompletion)(struct DownloadConfig* config);

typedef struct DownloadConfig {
	bool used;
	char* url;
	char* localFilePath;
	char* token;
	DownloadCompletion completion;
} DownloadConfig;

/// `url` and `token` are copied
void downloadConfig_init(DownloadConfig* config, const char* url, const char* token, DownloadCompletion completion);
void downloadConfig_free(DownloadConfig* config);

void downloadConfig_perform(DownloadConfig* config);

#endif