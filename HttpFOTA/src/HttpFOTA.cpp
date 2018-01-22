
#include <WiFi.h>
#include <functional>
#include <HTTPClient.h>
#include "HttpFOTA.h"
#include "MD5Builder.h"
#include "Update.h"


HttpFOTA::HttpFOTA()
{ 
}

HttpFOTA::~HttpFOTA(){

}

int HttpFOTA::start(DlInfo &info) {
    int downloaded = 0;
    int written = 0;
    int total = 1;
    int len = 1;
    uint8_t buff[1024] = { 0 };
    size_t size = sizeof(buff);
    int ret = 0;
    HTTPClient http;

    if( info.progressCallback == NULL || info.errorCallback == NULL ||
        info.startDownloadCallback == NULL || info.endDownloadCallback == NULL ||
        info.url == NULL){
        return -1;
    }
    if(info.caCert != NULL){
        http.begin(info.url, info.caCert);
    } else {
        http.begin(info.url);
    }
    
    int httpCode = http.GET();

    if(httpCode > 0 && httpCode == HTTP_CODE_OK) {
        info.startDownloadCallback();
        // get lenght of document (is -1 when Server sends no Content-Length header)
        len = http.getSize();
        total = len;
        downloaded = 0;
        // get tcp stream
        WiFiClient * stream = http.getStreamPtr();
        if (Update.begin(total, U_FLASH)){
            Update.setMD5(info.md5);
            downloaded = 0;
            while (!Update.isFinished()) {
                // read all data from server
                if(http.connected() && (len > 0)) {
                  // get available data size
                  size = stream->available();

                    if(size > 0) {
                        // read up to 128 byte
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                        // write to storage
                        written = Update.write(buff, c);
                        if (written > 0) {
                            if(written != c){
                                info.errorCallback("Flashing chunk not full ... warning!");
                            }
                            downloaded += written;
                            info.progressCallback(Flashing_e, (100*downloaded)/total);
                        } else {
                            info.errorCallback("Flashing ... failed!");
                            ret = -1;
                            break;
                        }

                        if(len > 0) {
                            len -= c;
                        }
                    }
                  delay(1);
                }
            }
        } else {
            info.errorCallback("Flashing init ... failed!");
            ret = -1;
        }
    info.endDownloadCallback(); 
    }else {
        info.errorCallback("[HTTP] GET... failed!");
        ret -1;
    }

    http.end();
    if(downloaded == total && len == 0){
        if(Update.end()){
            info.errorCallback("Flashing ... done!");
            delay(100);
            ESP.restart();                
        } else {
            info.errorCallback("Flashing md5 ... failed!"); 
            ret = -1;
        }
    } else {
        info.errorCallback("Download firmware ... failed!");
        ret = -1;
    }

    return ret;
}

HttpFOTA httpFOTA;


