// Tek4.vn 
#include <WiFi.h> 
#include <WiFiClient.h> 
#include "FirebaseESP32.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(16, 17); // RX: 16,TX: 17 --- RX: B7  TX: B6 --- RX: A10 TX: A9
// #define WIFI_SSID "PIF_CLUB_5G"           // Nhập thông tin wifi của access point //
// #define WIFI_PASSWORD "muoisomuoi"
#define WIFI_SSID "Luclac"           // Nhập thông tin wifi của access point //
#define WIFI_PASSWORD "2233556688"

// #define WIFI_SSID "Samsung"           // Nhập thông tin wifi của access point //
// #define WIFI_PASSWORD "chiasene"

#define FIREBASE_HOST "esp32-stm32-default-rtdb.firebaseio.com"
// #define FIREBASE_AUTH "AIzaSyDn4cLmJTCbRCBq15ZpWeTch8npwhClh_0"
#define FIREBASE_AUTH "McFaTDHKTIzvba1DU2urJWQzYV9ugH3UxPrIJqfD"
FirebaseData firebaseData;
int number;
float str;
unsigned int input;
unsigned long t1= 0;
unsigned int Hum;
unsigned int Temp;
unsigned int Hum_temp = 0;
unsigned int Temp_temp = 0;
long lastUART = 0;
int main_data;
int temp1 = 0;
int hum1 = 0;
unsigned int hour = 0;
unsigned int minute = 0;
String hour_str, minute_str;
String decoded_data, decoded_temp, decoded_hum;
void Read_Uart();  
//Wifi client;
String path = "/";
FirebaseJson json;
void setup() {
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(firebaseData, path))
  {
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  // mySerial.begin(115200);
  mySerial.begin(9600);
  Serial.println("UART Start");
  lastUART = millis();
}

void loop()
{
  // Read_Uart();
  // if (millis() - lastUART > 5000)
  if (millis() - lastUART > 7000)
  {
    // mySerial.print("* live\n");
    // Serial.println("Send : * live\n");
    Read_Uart();
    lastUART = millis();
  }
}

void Read_Uart()
{
  // // Serial.println(mySerial.read());
  // String st = "";
  // while (mySerial.available())
  // {
  //   char inChar = (char)mySerial.read();
  //   st +=  inChar;
  //   if (inChar == '#' && st.length() == 6)
  //   // if (inChar == '#' && st.length() >0)
  //   { 
  //     Serial.println("Raw data received : " + st);
  //     // xử lí chuỗi thô (decoding message) //
  //     int A = st.indexOf("*");
  //     int B = st.indexOf("#");
  //     decoded_data = st.substring(A + 1, B);
  //     decoded_temp = st.substring(A + 1, A + 3);
  //     decoded_hum = st.substring(A + 3, A + 5);
  //     // Convert string into int //
  //     main_data = decoded_data.toInt();
  //     temp1 = decoded_temp.toInt();
  //     hum1 = decoded_hum.toInt();
  //     // In chuỗi đã decode lên Serial Monitor ở IDE //
  //     Serial.println(main_data);
  //     Serial.println(temp1);
  //     Serial.println(hum1);
  //     // delay(2000);
  //     // Send data lên Firebase //

  //       // if (millis() - lastUART >= 1000)
  //       // {
  //       //   Firebase.setInt(firebaseData, "/DHT/Temp", temp1);
  //       //   Firebase.setInt(firebaseData, "/DHT/Hum", hum1);
  //       //   lastUART = millis();
  //       // }
  //     Firebase.setInt(firebaseData, "/DHT/Temp", temp1);
  //     Firebase.setInt(firebaseData, "/DHT/Hum", hum1);
  //     break;
  //   }
    //////////// Cập nhật thời gian báo thức /////////////////////

    // Firebase.getInt(firebaseData, "/ring/hour");
    // Firebase.getInt(firebaseData, "/ring/minute");
    if(Firebase.getInt(firebaseData, "/ring/hour") == true) // = true nghia la doc ve thanh cong
    {
    hour_str = firebaseData.stringData();
    // hour = atoi(hour_str);
    // mySerial.println('^ '+hour+"\n");
    // Serial.println("hour received: " +hour+"*");
    // hour = hour_str.toInt(); 
    mySerial.print('^');
    mySerial.print(' ');
    mySerial.print(hour_str);
    mySerial.print("\n");
    // mySerial.print('^ '+hour_str+"\n");
    Serial.print("hour received: ^ "+hour_str+"\n");
    }
    if(Firebase.getString(firebaseData, "/ring/minute") == true) // = true nghia la doc ve thanh cong
    {
    minute_str = firebaseData.stringData();
    // minute = minute_str.toInt();
    // mySerial.print('# '+minute_str+"\n");
    delay(5000);
    mySerial.print('#');
    mySerial.print(' ');
    mySerial.print(minute_str);
    mySerial.print("\n");
    Serial.print("minute received: # "+minute_str+"\n");

    // Serial.println("minute received: " +minute+ "*");
    }
    // mySerial.println('^ '+hour+"\n");
    // mySerial.println('# '+minute+"\n");
    // Serial.println("hour received: " +hour);
    // Serial.println("minute received: " +minute);
    // Serial.println('^ '+hour+"\n");
    // Serial.println('# '+minute+"\n");
    /////////////////////////////////////////////////////////////

    // snprintf(ring_time_str, 7, "%02g%02g", hour, minute);

    // mySerial.println(ring_time_str);
    // Serial.println("Send: " +ring_time);
  // }
}



// void loop() {
//   if (mySerial.available() > 0) {
//     str = mySerial.read();
//     // number = str.toInt();
//     if(str > 1){
//     Serial.println(str);
//     } 
//   }
// // Hum = str;
// // // Temp = Temp_temp;
// // Temp = number;
// delay(20);
// //  if (Hum!= Hum_temp){
// //   Hum = Hum_temp;
// //   Firebase.setInt(firebaseData, path + "/DHT/Hum", Hum);
// //  }
// //   if (Temp!= Temp_temp){
// //   Temp = Temp_temp;
// //   Firebase.setInt(firebaseData, path + "/DHT/Temp", Temp);
// //  }


//     str = mySerial.read();
//     Firebase.setFloat(firebaseData, path + "/DHT/Temp", str);
// if(str > 1){
//    Firebase.setInt(firebaseData, path + "/DHT/Temp", str);
//   //  Firebase.setFloat(firebaseData, path + "/DHT/Temp", str);
// }


// // Temp++;
// //    Firebase.setInt(firebaseData, path + "/DHT/Temp", Temp);


// //  Serial.print
// //  Firebase.setInt(firebaseData, path + "/LDR/Percent", 12); //gui gtri tu esp len fb kieu nguyen

//               //   Firebase.getInt(firebaseData, path + "/LDR/Percent")); //lấy gtri từ Firebase về gán vào biến x
//               //        x = firebaseData.intData(); //lay gtri tu fb ve esp
// }