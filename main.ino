/*******************************************************************
    Connect to Twtich Chat with a Bot

   Created with code from TheOtherLoneStar (https://www.twitch.tv/theotherlonestar)
   Hackaday IO: https://hackaday.io/otherlonestar

   By Brian Lough (https://www.twitch.tv/brianlough)
   YouTube: https://www.youtube.com/channel/UCezJOfu7OtqGzd5xrP3q6WA
 *******************************************************************/

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <IRCClient.h>
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"

//define your default values here, if there are different values in config.json, they are overwritten.

#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     6667


//------- Replace the following! ------
char ssid[] = "XXXX";       // your network SSID (name)
char password[] = "XXXX";  // your network key

//The name of the channel that you want the bot to join
const String twitchChannelName = "XXXX";

//The name that you want the bot to have
#define TWITCH_BOT_NAME "justinfan112312312345"

SSD1306Wire display(0x3c, D3, D4); 


//------------------------------


int led = LED_BUILTIN;
String ircChannel = "";

WiFiClient wiFiClient;
IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

// put your setup code here, to run once:
void setup() {

  pinMode(led, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  
  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  displayStatus("Connecting to Wifi");
  
  WiFi.begin(ssid, password);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  displayStatus("Connecting to Twitch");
  ircChannel = "#" + twitchChannelName;

  client.setCallback(callback);
}

void displayStatus(String message) {
  display.clear();
  display.drawString(0, 0, message);
  display.display();
}

void loop() {

  // Try to connect to chat. If it loses connection try again
  if (!client.connected()) {
    Serial.println("Attempting to connect to " + ircChannel );
    // Attempt to connect
    // Second param is not needed by Twtich
    if (client.connect(TWITCH_BOT_NAME, "", "")) {
      client.sendRaw("JOIN " + ircChannel);
      Serial.println("connected and ready to rock");
      displayStatus("Connected to Twitch!");
    } else {
      Serial.println("failed... try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    return;
  }
  client.loop();
}

void callback(IRCMessage ircMessage) {
  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
    ircMessage.nick.toUpperCase();

    display.clear();
    display.drawString(0, 0, ircMessage.nick);
    display.drawStringMaxWidth(0, 10, 128, ircMessage.text);
    display.display();

    //prints chat to serial
    Serial.println("<" + ircMessage.nick + "> " + ircMessage.text);
  }
}
