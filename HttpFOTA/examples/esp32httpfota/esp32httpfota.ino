#include <WiFi.h>
#include <HttpFOTA.h>
#include <PubSubClient.h>

typedef enum {
  Runnning_e = 0x01,
  Fota_e  
}SysState;

const char* ssid = "dd-wrt";
const char* password = "0000000000";
const char* mqtt_server = "192.168.1.107";

char url[100];
char md5[50];
WiFiClient espClient;
PubSubClient client(espClient);
SysState state = Runnning_e;

/* topics */
#define OTA_TOPIC    "smarthome/room1/ota"

void progress(DlState state, int percent){
  Serial.printf("state = %d - percent = %d\n", state, percent);
}

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  
  if(strncmp(OTA_TOPIC, topic, strlen(OTA_TOPIC)) == 0){
    memset(url, 0, 100);
    memset(md5, 0, 50);
    char *tmp = strstr((char *)payload, "url:");
    char *tmp1 = strstr((char *)payload, ",");
    memcpy(url, tmp+strlen("url:"), tmp1-(tmp+strlen("url:")));
    
    char *tmp2 = strstr((char *)payload, "md5:");
    memcpy(md5, tmp2+strlen("md5:"), length-(tmp2+strlen("md5:")-(char *)&payload[0]));

    Serial.printf("started fota url: %s\n", url);
    Serial.printf("started fota md5: %s\n", md5);
    state = Fota_e;
  }
}
void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic */
      client.subscribe(OTA_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void error(char *message){
  printf("%s\n", message);
}

void startDl(void){

}
void endDl(void){

}

void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.print("Connecting to "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  } 
  Serial.println("");
  
  Serial.print("WiFi connected, IP address: "); 
  Serial.println(WiFi.localIP());

  /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
}
 
void loop() {
  switch(state)
  {
    case Runnning_e:
      /* if client was disconnected then try to reconnect again */
      if (!client.connected()) {
        mqttconnect();
      }
      /* this function will listen for incomming 
      subscribed topic-process-invoke receivedCallback */
      client.loop();
    break;
    case Fota_e:
      DlInfo info;
      info.url = url;
      info.md5 = md5;
      info.startDownloadCallback =  startDl;
      info.endDownloadCallback =    endDl;
      info.progressCallback  = progress;
      info.errorCallback     = error;
      httpFOTA.start(info);
    
      client.publish(OTA_TOPIC, "ok");
    break;
    default:
    break;
  }

}
