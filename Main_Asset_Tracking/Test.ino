
#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <HardwareSerial.h>
#include <bits/stdc++.h>
////-----------------------------------------------------
#define LED1 33
#define LED2 25
////----------------------------------------------------
String Pack_Mac_Address , Sum_Mac_Address , Data_Mac_Frame , Devices_Count ;
char Data_publish_Mac [2000] ;
String  STR_Data = "$START,";
String  END_Data = "#END";
int CHK_SUM = 0x00;
//----------------------------------------------------
//----------------------------------------------------
int scanTime = 4; //In seconds
// Update these with values suitable for your network.
//const char* ssid = "FTS-Office";
//const char* password = "B4ngk0k1";
const char* ssid = "Khirin";
const char* password = "0123456789";
const char* mqtt_server = "27.100.44.137";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
bool Flag_Check = 0 ;
//String Srt_test = "12554b4d5011,08fd7a872222,12554b4d5033,08fd7a872244,12554b4d5055,08fd7a872266,12554b4d5007,08fd7a872208,12554b4d5009 ,08fd7a872210,12554b4d5011,08fd7a872212";// 
//---------------------------------------------------
const TickType_t xDelay2000ms = pdMS_TO_TICKS(2000);
const TickType_t xDelay200ms = pdMS_TO_TICKS(200);
const TickType_t xDelay20ms = pdMS_TO_TICKS(20);
const TickType_t xDelay50ms = pdMS_TO_TICKS(50);
const TickType_t xDelay10ms = pdMS_TO_TICKS(10);
//---------------------------------------------------
BLEScan* pBLEScan;
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String rec , sub_rec , sub_fac , dataRec;
      String setting[10] , Mac_Address[10], manufacDATA[10], Data_Mac_Address [10];
      for (int i = 0; i <= 10; i++) {
        setting[i] = "0";
        Mac_Address[i] = "0";
      }
      dataRec = advertisedDevice.toString().c_str();
      sub_rec = dataRec;

      setting[0] = sub_rec.substring(0, sub_rec.indexOf(',')); // Name
      sub_rec = sub_rec.substring(sub_rec.indexOf(',') + 1);

      setting[1] = sub_rec.substring(0, sub_rec.indexOf(',')); // Mac Address
      sub_rec = sub_rec.substring(sub_rec.indexOf(',') + 1);

      setting[2] = sub_rec.substring(0, sub_rec.indexOf(',')); // Manufactuer data
      sub_rec = sub_rec.substring(sub_rec.indexOf(',') + 1);

      setting[3] = sub_rec.substring(0, sub_rec.indexOf(',')); // Service UUID
      sub_rec = sub_rec.substring(sub_rec.indexOf(',') + 1);

      setting[4] = sub_rec.substring(0, sub_rec.indexOf(',')); // Null.....
      sub_rec = sub_rec.substring(sub_rec.indexOf(',') + 1);
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

      Serial.print("Mac Add:");  Serial.println(setting[1]);
      //      Serial.println(setting[1]);   Serial.println(setting[2]);
      sub_fac = setting[1];
      Mac_Address[0] = sub_fac.substring(0, sub_fac.indexOf(':'));  // Name
      sub_fac = sub_fac.substring(sub_fac.indexOf(':') + 1);
      Mac_Address[1] = sub_fac.substring(1, 3);
      Mac_Address[2] = sub_fac.substring(4, 6);
      Mac_Address[3] = sub_fac.substring(7, 9);
      Mac_Address[4] = sub_fac.substring(10, 12);
      Mac_Address[5] = sub_fac.substring(13, 15);
      Mac_Address[6] = sub_fac.substring(16, 18);

      //      Serial.print(" manufacDATA[1] :");  Serial.println( Mac_Address[1]);
      //      Serial.print(" manufacDATA[2] :");  Serial.println( Mac_Address[2]);
      //      Serial.print(" manufacDATA[3] :");  Serial.println( Mac_Address[3]);
      //      Serial.print(" manufacDATA[4] :");  Serial.println( Mac_Address[4]);
      //      Serial.print(" manufacDATA[5] :");  Serial.println( Mac_Address[5]);
      //      Serial.print(" manufacDATA[6] :");  Serial.println( Mac_Address[6]);
      // Serial.print("Data_Mac_Address: ");
      for (int i = 1 ; i <= 6; i++) {
        Data_Mac_Address[i] = Mac_Address[i];
        Serial.print(Data_Mac_Address[i]);
      }
      Serial.println(" ");
      Pack_Mac_Address = String(Data_Mac_Address[1] + Data_Mac_Address[2] + Data_Mac_Address[3] + Data_Mac_Address[4] + Data_Mac_Address[5] + Data_Mac_Address[6] + ",");
      //  Serial.println(" ");
      Sum_Mac_Address = Sum_Mac_Address + Pack_Mac_Address ;
      Serial.println("______________________________________________");
      Serial.println(" ");

      //      for(int i = 0 ;i<=sizeof(Data_publish_Mac)); i++){
      //       Data_publish_Mac[i] = '0';
      //    Serial.print("*****Sum_Mac_Address:");  Serial.println(Sum_Mac_Address);
      //      //  Serial.print("Data_Mac_Address:");  Serial.println(Data_Mac_Address);
      //
    }
};
//----------------------------------------------------------------
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());
}
//-------------------------------------------------------------
void setup_Bluetooth() {
  Serial.println("Scanning...");
  BLEDevice::init("BT - AccssTracking");
  digitalWrite(LED1, HIGH);
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}
//-------------------------------------------------------------

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Find out how many bottles we should generate lyrics for
  String topicStr(topic);
  // Now we can start to publish the message
  // client.beginPublish("Mac", msgLen, false);
  // Now we're done!
  // client.endPublish();
}
//-----------------------------------------------------------
void set_MultiTask() {
//  xTaskCreate(&Main_Task, "Main_Task", 4096, NULL, 1, NULL);
  xTaskCreate(&Bluetooth_Task, "Bluetooth_Task", 4096, NULL, 3, NULL);
  xTaskCreate(&MQTT_Task, "Bluetooth_Task", 4096, NULL, 2, NULL);
}
//------------------------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // Attempt to connect
    //    if (client.connect("arduinoClient")) {
    //      Serial.println("connected");
    //      // Once connected, publish an announcement...
    //      client.publish("Mac", Data_publish_Mac);
    //      // ... and resubscribe
    //      client.subscribe("Mac");
    //    } else {
    //      Serial.print(client.state());
    //      Serial.println(" try again in 5 seconds");
    //      // Wait 5 seconds before retrying
    //      delay(1000);
    //    }
  }
}

void setup() {
  //  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  setup_Bluetooth();
  set_MultiTask();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, HIGH);
}
//--------------------------------------------------------------
void loop() {
}
//---------------------------------------------------------------
void Main_Task(void *pvvalue) {
  Serial.println(" Main_Task Run ! ");
  while (1) {
    vTaskDelay(xDelay20ms);   // Delay Task นี้ 2 วินาที
  }

}

//---------------------------------------------------------------
void Bluetooth_Task(void *pvvalue) {
  Serial.println(" Bluetooth_Task Run ! ");
  while (1) {
    //-----------------------------------------------------------------
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Devices_Count = foundDevices.getCount();
    Serial.println("Scan done!");

    Serial.print("Sum_Mac_Address*:"); Serial.println(Sum_Mac_Address);
    //  Data_Mac_Frame = STR_Data + Devices_Count + ", " + Sum_Mac_Address + END_Data ;
    // Serial.print("Data_Mac_Frame: "); Serial.println(Data_Mac_Frame);

    Data_Mac_Frame = (STR_Data + Devices_Count + "," + Sum_Mac_Address  + END_Data );
    Serial.print("Data Frame: "); Serial.println(Data_Mac_Frame);
    //Srt_test
    // int count_length = Data_Mac_Frame.length();
    //    sprintf(Data_publish_Mac, "%s", Data_Mac_Frame );
    //    Serial.print("Data %s Data_publish_Mac: "); Serial.println(Data_Mac_Frame);
    //Srt_test.toCharArray(Data_publish_Mac, sizeof(Data_publish_Mac));
    Data_Mac_Frame.toCharArray(Data_publish_Mac, sizeof(Data_publish_Mac));
    Flag_Check = 1;

    //***************************

    if (client.connect("Mac")) {
      Serial.println("connected");
      if (Flag_Check == 1 ) {
        digitalWrite(LED2, HIGH);
        Serial.print("Send MQTT : ");
        client.publish("Mac", Data_publish_Mac);
        client.subscribe("Mac");
        //  client.publish("Mac", Data_publish_Mac);
        //      Data_publish_Mac = "";
        //     Serial.print(" --> Sum_Mac_Address: "); Serial.println(Sum_Mac_Address);
        Serial.print(" --> Data_publish_Mac: "); Serial.println(Data_publish_Mac);
        Flag_Check = 0;
        digitalWrite(LED2, LOW);
        //  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
        for (int i = 0; i <= sizeof(Data_publish_Mac); i++) {
          Data_publish_Mac[i] = NULL;
        }
        Sum_Mac_Address = " " ;
        Serial.print("Remove Data: "); Serial.println(Data_publish_Mac);
        // memset(Data_publish_Mac, 0 , sizeof(Data_publish_Mac));
      }
    } else {

      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
    //-------------------------------------------------------------
    vTaskDelay(xDelay200ms);   // Delay Task นี้ 2 วินาที
  }
}

//---------------------------------------------------------------
//---------------------------------------------------------------
void MQTT_Task(void *pvvalue) {
  Serial.println(" MQTT_Task Run ! ");
  while (1) {
  
    vTaskDelay(xDelay20ms);   // Delay Task นี้ 2 วินาที
  }
}

//---------------------------------------------------------------
