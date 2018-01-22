#ifndef __HTTP_FOTA_H
#define __HTTP_FOTA_H

typedef enum {
    DownloadingAndSaving_e = 0x01,
    Flashing_e
}DlState;

typedef void (*startDownloadCb)(void);
typedef void (*endDownloadCb)(void);
typedef void (*progressCb)(DlState state, int percent);
typedef void (*errorCb)(char *message);

typedef struct {
    char *url;
    char* caCert;
    char *md5;
    startDownloadCb     startDownloadCallback;
    endDownloadCb       endDownloadCallback;
    progressCb          progressCallback;
    errorCb             errorCallback;
}DlInfo;


class HttpFOTA
{
  public:

    HttpFOTA();
    ~HttpFOTA();
	
    int start(DlInfo &info);

};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_HttpFOTA)
extern HttpFOTA httpFOTA;
#endif

#endif /* __HTTP_FOTA_H */
