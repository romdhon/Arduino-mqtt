#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "nortee";
const char* password = "0962757954";
const char* mqtt_server = "broker.mqttdashboard.com";

const int mqttPort = 1883;
const char* mqttUser = "mqtt-server";
const char* mqttPassword = "pnueng60";

long val = 0;
long val2 = 0;

boolean auto_mode = false; //true

int sensor1 = A0;
int sensor2 = D2;
int relay = D1;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char msg2[50];
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((char)payload[0] == '3'){
    auto_mode = true;
    return;
    }
  else if((char)payload[0] == '2'){
    auto_mode = false;
    return;
    }
  if (auto_mode == true){
    return;
    }

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    digitalWrite(relay, LOW);
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite(relay, HIGH);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("pnu_mqtt", mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("sensors_out/moisture", "my name is aina");
      client.publish("sensors_out/dht", "my name is pang");
      // ... and resubscribe
      client.subscribe("sensors_in/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(relay, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

    //read the input from sensor here
    int temp = analogRead(sensor1);
    int ph = analogRead(sensor2);
    if(auto_mode == true){
      //put something here when automode is on
        digitalWrite(BUILTIN_LED, LOW);
        delay(100);
        digitalWrite(BUILTIN_LED, HIGH);
        
//      return;
      }
    // put something here when in manual mode
//    snprintf (msg, 75, "aina really love pang #%ld", value);
    snprintf(msg, 75, "%ld", temp);
    snprintf(msg2, 75, "%ld", ph);
    Serial.print("Publish message: ");
    Serial.println(msg);
//    client.publish("sensors_out/ph", msg);
    client.publish("sensors_out/tem", msg);
    client.publish("sensors_out/ph", msg2);
  }
}
