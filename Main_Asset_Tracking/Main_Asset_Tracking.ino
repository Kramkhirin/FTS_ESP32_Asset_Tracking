//#include <WiFi.h>
//#define SSID_NAME "DUCK_2G"
//#define SSID_PASS "30597999"
//#define LED1 33
//#define LED2 25
//
//
//void setup() {
//  pinMode(LED1 , OUTPUT);
//  pinMode(LED2 , OUTPUT);
//  digitalWrite(LED1, HIGH);
//  Serial.begin(115200);
//  WiFi.mode(WIFI_STA);
//  WiFi.disconnect();
//  delay(100);
//  Serial.print("Connecting to ");
//  Serial.println(SSID_NAME);
//  WiFi.begin(SSID_NAME, SSID_PASS);
//  while (WiFi.status() != WL_CONNECTED) {
//     digitalWrite(LED2, HIGH);
//    delay(500);
//    Serial.print(".");
//     digitalWrite(LED2, LOW);
//  }
//  Serial.println(" ");
//  Serial.println("WiFi Status -> : Connected");
//  Serial.print("IP address: ");
//  Serial.println(WiFi.localIP() );
//}
//
//
//void loop() {
//  // put your main code here, to run repeatedly:
//
//}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <HardwareSerial.h>
#include <WiFi.h>
//-----------------------------------------------------
#define LED1 33
#define LED2 25
//----------------------------------------------------
String Pack_Mac_Address , Sum_Mac_Address , Data_Mac_Frame , Devices_Count ;
String  STR_Data = "$BTMAC,";
String  END_Data = "#BTEND";
int CHK_SUM = 0x00;
//----------------------------------------------------
// Replace with your network credentials
const char* ssid     = "";
const char* password = "";
// Set web server port number to 80
WiFiServer server(80);
// Variable to store the HTTP request
String header;
// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";
// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;
//----------------------------------------------------
int scanTime = 5; //In seconds
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
      Serial.print("Data_Mac_Address: ");
      for (int i = 1 ; i <= 6; i++) {
        Data_Mac_Address[i] = Mac_Address[i];
        Serial.print(Data_Mac_Address[i]);

      }
      Pack_Mac_Address = String(Data_Mac_Address[1] + Data_Mac_Address[2] + Data_Mac_Address[3] + Data_Mac_Address[4] + Data_Mac_Address[5] + Data_Mac_Address[6] + ",");
      Serial.println(" ");
      Sum_Mac_Address = Sum_Mac_Address + Pack_Mac_Address ;
      Serial.print("Pack_Mac_Address: "); Serial.println(Pack_Mac_Address);
      Serial.println("_____________________________");
      Serial.println(" ");
      // Serial.print("Data_Mac_Address:");  Serial.println(Data_Mac_Address);
      //  Serial.print("Data_Mac_Address:");  Serial.println(Data_Mac_Address);

    }


};

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(115200);
  Serial2.begin(57600, SERIAL_8N1, 16, 17);
  Serial.setTimeout(20);
  Serial2.setTimeout(100);
  Serial.println("Scanning...");
  BLEDevice::init("BT - AccssTracking");
  digitalWrite(LED1, HIGH);
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  //-------------------------------------------------------------
}

void loop() {

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Devices_Count = foundDevices.getCount();
  Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  Serial.print("Sum_Mac_Address: "); Serial.println(Sum_Mac_Address);
  Data_Mac_Frame = STR_Data + Devices_Count + "," + Sum_Mac_Address + END_Data ;
  Serial.print("Data_Mac_Frame: "); Serial.println(Data_Mac_Frame);
  Sum_Mac_Address = " " ;
  digitalWrite(LED2, HIGH);
  delay (200);
  digitalWrite(LED2, LOW);
  delay(500);
  //-------------------------------------------------------------

}
