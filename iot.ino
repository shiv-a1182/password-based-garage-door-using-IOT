#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Esp32WifiManager.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>


// Wifi network station credentials
#define WIFI_SSID "Ben"
#define WIFI_PASSWORD "123456789"
// Telegram BOT Token (Get from Botfather)

#define BOT_TOKEN "5572310529:AAFdxiDIX7T5PiZoEoBbia4erv4w2ZpFhcc"

#define CHAT_id  "1454443856"

 
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done
const unsigned long BOT_MTBS = 1000; // mean time between scanmessages

const int ledPin = LED_BUILTIN;
int ledStatus = 0;
int servoPin2 = 14;
int servoPin1 = 2; 
Servo servo1; 

//Servo servo1 ;
Servo servo2;
int pos = 0;
int trigPin = 25;
int echoPin = 26;
int ir = 35;
int ir1 =34;
int t=0;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701


long duration;
float distanceCm;
float distanceInch;

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/1234" )
    {
         servo1.write(90); 
         servo2.write(90);
         bot.sendMessage(CHAT_id, "Gate is open", "");
         int irvalue = digitalRead(ir);
         int ir1value = digitalRead(ir1);
    if(irvalue == HIGH || ir1value ==HIGH){
      Serial.println("PARKing mt");
      if(irvalue ==HIGH && ir1value== HIGH){
       bot.sendMessage(CHAT_id, "slot-1 and Slot-2 are empty", "");
       irvalue = digitalRead(ir);
       ir1value = digitalRead(ir1);
       while(irvalue!=LOW && ir1value!=LOW ){
          irvalue = digitalRead(ir);
          ir1value = digitalRead(ir1);
         }
         if(irvalue == LOW){
          bot.sendMessage(CHAT_id, "Parked at Slot-1", "");
          servo1.write(0); 
         servo2.write(0);
         }
         else if(ir1value == LOW){
          bot.sendMessage(CHAT_id, "Parked at Slot-2", "");
          servo1.write(0); 
         servo2.write(0);
         }
         else{
          servo1.write(0); 
         servo2.write(0);
         }
      }
      else if(irvalue ==HIGH && ir1value== LOW){
        bot.sendMessage(CHAT_id, "Slot-1 is empty", "");
        irvalue = digitalRead(ir);
        while(irvalue!=LOW){
          irvalue = digitalRead(ir);
         }
        if(irvalue == LOW){
          bot.sendMessage(CHAT_id, "Parked at Slot-1", "");
          servo1.write(0); 
         servo2.write(0);
         }
      }
      else{
        bot.sendMessage(CHAT_id, "Slot-2 is empty", "");
        ir1value = digitalRead(ir1);
        while(ir1value!=LOW){
          ir1value = digitalRead(ir1);
         }
        if(ir1value == LOW){
          bot.sendMessage(CHAT_id, "Parked at Slot-2", "");
          servo1.write(0); 
         servo2.write(0);
         }
      }
    }
  else
  {
    Serial.println("Parking full");
    bot.sendMessage(CHAT_id, "Parking is Full", "");
    while(distanceCm <=10){
      distanceCm = utra();
    }
     servo1.write(0);
     servo2.write(0);
  }    
    }
    else{
      if(text != ""){
         servo1.write(0); 
         servo2.write(0);
        bot.sendMessage(CHAT_id, "Incorrect Password", "");
        distanceCm = utra();
        if(distanceCm <=10){
          bot.sendMessage(CHAT_id, "ReEnter the Password", "");
          check();
        }
        else{
          return ;
        }
    }
  }
  }
}
void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); // initialize pin as off (active LOW)
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 // servo1.attach(servopin1);
  servo2.attach(servoPin2);
  servo1.attach(servoPin1);
  pinMode(ir,LOW);
   pinMode(ir1,LOW);
  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

}
int utra(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
  return distanceCm;
}
void check(){
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (!numNewMessages)
    {
      distanceCm = utra();
      if(distanceCm <=10){
       numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    else{
      return ;
    } 
    }
    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}
void loop()
{
  distanceCm = utra();
  servo1.write(0); 
  servo2.write(0);
  if(distanceCm <=10){
    bot.sendMessage(CHAT_id, "vehicle is Arrived", "");
    bot.sendMessage(CHAT_id, "enter password", "");
    check();
  UniversalTelegramBot bot(BOT_TOKEN, secured_client);
  }
Serial.print("Distance (cm): ");
Serial.println(distanceCm);
delay(1000);
}
