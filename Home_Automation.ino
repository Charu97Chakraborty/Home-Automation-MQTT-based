//Name            :   Shilpi Maity
//Date            :   01-10-2023
//Description     :   Home Automation MQTT based

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <DHT.h>

// Define pin numbers for components
#define ROOM1_LIGHT1 7
#define ROOM2_LIGHT1 8
#define ROOM1_DHTPIN 2
#define ROOM2_DHTPIN 3

// Define sensor type
#define DHTTYPE DHT11

// Create DHT sensor instances
DHT dht1(ROOM1_DHTPIN, DHTTYPE);
DHT dht2(ROOM2_DHTPIN, DHTTYPE);

// Define MAC, IP, and MQTT server addresses
byte mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,1,100);
IPAddress server(192,168,209,60);

void callback(char* topic, byte* payload, unsigned int length);

// Create Ethernet client instance
EthernetClient ethClient;

// Create MQTT client instance
PubSubClient client(server, 1883, callback, ethClient);

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message: ");
  if (strcmp(topic, "home/room1/light1") == 0) 
  {
    if (payload[0] == '1') 
    {
      digitalWrite(ROOM1_LIGHT1, HIGH);
      client.publish("home/feedback", "Room1 Light1 On");
      Serial.println("Room1 Light1 On");
    }
    else if (payload[0] == '0') 
    {
      digitalWrite(ROOM1_LIGHT1, LOW);
      client.publish("home/feedback", "Room1 Light1 Off");
      Serial.println("Room1 Light1 Off");
    }
  }
  else if (strcmp(topic, "home/room2/light1") == 0) 
  {
    if (payload[0] == '1') 
    {
      digitalWrite(ROOM2_LIGHT1, HIGH);
      client.publish("home/feedback", "Room2 Light1 On");
      Serial.println("Room2 Light1 On");
    }
    else if (payload[0] == '0') 
    {
      digitalWrite(ROOM2_LIGHT1, LOW);
      client.publish("home/feedback", "Room2 Light1 Off");
      Serial.println("Room2 Light1 Off");
    }
  }
}

// Function to handle reconnection to MQTT server
void reconnect()
{
/* Loop until we're reconnected */
while (!client.connected())
{
Serial.print("Attempting MQTT connection...");
/* Create a random client ID */
String clientId = "Home_automation_UNO";
clientId += String(random(0xffff), HEX);
/* Attempt to connect*/
if (client.connect(clientId.c_str()))
{
Serial.println("connected");
/* Once connected, publish an announcement...*/
/* ... and resubscribe*/
      client.subscribe("home/room1/light1");
      client.subscribe("home/room2/light1");
    }
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Set pin modes and initial states for lights
void setup() 
{
  pinMode(ROOM1_LIGHT1, OUTPUT);
  pinMode(ROOM2_LIGHT1, OUTPUT);
  
  digitalWrite(ROOM1_LIGHT1, LOW);
  digitalWrite(ROOM2_LIGHT1, LOW);

  // Start serial communication
  Serial.begin(9600);

  // Initialize Ethernet connection
  Ethernet.begin(mac, ip);

  // Initialize DHT sensors
  dht1.begin();
  dht2.begin();

  // Check Ethernet hardware and cable status
  if (Ethernet.hardwareStatus() == EthernetNoHardware) 
  {
    Serial.println("Ethernet shield was not found. Sorry, can't run without hardware. :(");
    while (true) 
    {
      delay(1);
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) 
  {
    Serial.println("Ethernet cable is not connected.");
  }
}

void loop() 
{
  if (!client.connected()) 

// Reconnect to MQTT if not connected  
  {
    reconnect();
  }
  client.loop(); // Process incoming MQTT messages

  // Read temperature from sensors and publish to MQTT
delay(100);
float room1_temp = dht1.readTemperature();

if (!isnan(room1_temp)) 
{
  String room1_tempString = String(room1_temp, 2);
  client.publish("home/room1/temperature", room1_tempString.c_str());
}
delay(100);
float room2_temp = dht2.readTemperature();

if (!isnan(room2_temp)) 
{
  String room2_tempString = String(room2_temp, 2);
  client.publish("home/room2/temperature", room2_tempString.c_str());
}

delay(10000); // Publish temperature every 10 seconds

// Print temperature to Serial Monitor
Serial.print("Room 1 Temperature: ");
Serial.print(room1_temp);
Serial.println(" °C");

Serial.print("Room 2 Temperature: ");
Serial.print(room2_temp);
Serial.println(" °C");
}
