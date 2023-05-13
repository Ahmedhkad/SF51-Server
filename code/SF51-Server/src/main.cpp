#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266Ping.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "settings.h" //GPIO NodeMCU good-pins
#include "secret.h"   //Wifi and MQTT server info
#include "webpage.h"

OneWire oneWire(TempDallas); // Dallas Temperature.
DallasTemperature sensors(&oneWire);

const char *ssid = ssidWifi; // defined on secret.h
const char *password = passWifi;
const char *mqtt_server = mqttURL;
const char *deviceName = mqttClient;

const char *http_username = Username;
const char *http_password = Password;

WiFiClient espClient;
PubSubClient client(espClient);

StaticJsonDocument<100> doc;
StaticJsonDocument<300> updater;
StaticJsonDocument<150> tempdoc;
int device;
int valuejson;
String passjson;

String StrongServerPingC = "";
String CoolServerPingC = "";

int lastCount = 0;
int count = 0;
boolean PowerLock = true;

unsigned long currentMillis;
unsigned long WifiDelayMillis = 0;
const long WifiDelayInterval = 5000; // interval to check wifi and mqtt
unsigned long TempDelayMillis = 0;
unsigned long TempDelayInterval = 30000;

unsigned long StrongPingDelayMillis = 0;
unsigned long CoolPingDelayMillis = 0;

AsyncWebServer server(webPORT);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found 404");
}

String pingStrongServer()
{
  if (Ping.ping(StrongServer_ip))
  {
    // Serial.println("StrongServer_ip Success!!");
    return "ONLINE";
  }
  else
  {
    // Serial.println("StrongServer_ip Error ");
    return "OFFLINE";
  }
  return "";
}

String pingCoolServer()
{
  if (Ping.ping(CoolServer_ip))
  {
    // Serial.println("CoolServer_ip Success!!");
    return "ONLINE";
  }
  else
  {
    // Serial.println("CoolServer_ip Error ");
    return "OFFLINE";
  }
  return "";
}

// Replaces placeholder with button section in your web page
//  String processor(const String& var) {
//   Serial.println(var);
//   // if (var == "TEMPERATUREC") {
//   //   return temperatureC;
//   // }
//   return String();
// }

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(deviceName); // DHCP Hostname (useful for finding device for static lease)
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Test if parsing succeeds.
  DeserializationError error = deserializeJson(doc, payload);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  // Print the values to data types
  device = doc["device"].as<unsigned int>();
  valuejson = doc["value"].as<unsigned int>();
  passjson = doc["password"].as<String>();
  Serial.print("device: ");
  Serial.print(device);
  Serial.print(" valuejson: ");
  Serial.println(valuejson);

  switch (device)
  {
  case 1:
    if (valuejson == 0)
    {
      PowerLock = true;
      client.publish("ServerLock", "ON");
    }
    else if (valuejson == 1)
    {
      PowerLock = false;
      client.publish("ServerLock", "OFF");
    }
    break;

  case 5:
    if (valuejson == 5 && passjson == passon && !PowerLock)
    {
      digitalWrite(StrongServer, HIGH);
      delay(300);
      digitalWrite(StrongServer, LOW);
      client.publish("StrongServer", "ON");
    }
    else if (valuejson == 9 && passjson == passoff && !PowerLock)
    {
      client.publish("StrongServer", "OFF");
    }
    break;

  case 9:
    if (valuejson == 5 && passjson == passon && !PowerLock)
    {
      client.publish("CoolServer", "ON");
    }
    else if (valuejson == 9 && passjson == passoff && !PowerLock)
    {
      digitalWrite(CoolServer, HIGH);
      delay(300);
      digitalWrite(CoolServer, LOW);
      client.publish("CoolServer", "OFF");
    }
    break;

  case 2: // set temperture send timer (default = 30,000 mill Second)
    TempDelayInterval = (valuejson * 1000);
    break;

  default:
    Serial.print("Err device in case-switch invalid.");
    break;
  }
}

void reconnect()
{
  // Loop until we're reconnected
  if (!client.connected())
  {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    uint32_t timestamp = millis() / 1000;
    char clientid[23];
    snprintf(clientid, 23, mqttClient "%02X", timestamp);
    Serial.print("Client ID: ");
    Serial.println(clientid);

    if (client.connect(clientid, mqttName, mqttPASS, mqttWillTopic, 0, true, "offline"))
    {
      Serial.println("Connected");
      // Once connected, publish an announcement...
      client.publish(mqttWillTopic, "online", true);
      // ... and resubscribe
      client.subscribe(subscribeTopic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      count = count + 1;
    }
  }
}

void setup()
{
  pinMode(TempDallas, INPUT);
  pinMode(StrongServer, OUTPUT);
  pinMode(CoolServer, OUTPUT);
  pinMode(StrongServerLDR, INPUT);
  pinMode(CoolServerLDR, INPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqttPORT);
  client.setCallback(callback);

  ArduinoOTA.setHostname(mqttClient);
  ArduinoOTA.setPort(otaPort);
  ArduinoOTA.setPassword(otaPass);
  ArduinoOTA.begin();

  //------------------------------------------web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(!request->authenticate(http_username, http_password))
      return request->requestAuthentication();
    request->send_P(200, "text/html", index_html); });

  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(401); });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", logout_html); });

  server.on("/onStrongServer", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(!request->authenticate(http_username, http_password)) 
       return request->requestAuthentication();
    digitalWrite(StrongServer, HIGH);
    client.publish("StrongServer", "ON");
    Serial.println("StrongServer ON");
    request->send(200, "text/plain", "ok"); });
  server.on("/offStrongServer", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(StrongServer, LOW);
    client.publish("StrongServer", "OFF");
    Serial.println("StrongServer OFF");
    request->send(200, "text/plain", "ok"); });

  server.on("/onCoolServer", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(!request->authenticate(http_username, http_password)) 
       return request->requestAuthentication();
    digitalWrite(CoolServer, HIGH);
    client.publish("CoolServer", "ON");
    Serial.println("CoolServer ON");
    request->send(200, "text/plain", "ok"); });
  server.on("/offCoolServer", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(CoolServer, LOW);
    client.publish("CoolServer", "OFF");
    Serial.println("CoolServer OFF");
    request->send(200, "text/plain", "ok"); });

  server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if(!request->authenticate(http_username, http_password)) 
       return request->requestAuthentication();
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      DynamicJsonDocument json(512);
      // sensors.requestTemperatures(); 
      float tempS = sensors.getTempCByIndex(0);
       
      json["Temperature"] = tempS;
      json["StrongLED"] = digitalRead(StrongServerLDR);
      json["CoolLED"] = digitalRead(CoolServerLDR);
      serializeJson(json, *response);
      Serial.println("api sensors response");
      request->send(response); });

  server.on("/StrongPing", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              StrongPingDelayMillis = currentMillis;
              request->send_P(200, "text/plain", StrongServerPingC.c_str()); });
  server.on("/CoolPing", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
              CoolPingDelayMillis = currentMillis;
              request->send_P(200, "text/plain", CoolServerPingC.c_str()); });

  server.onNotFound(notFound);

  server.begin();
}

void loop()
{
  ArduinoOTA.handle();
  currentMillis = millis();

  if (currentMillis - WifiDelayMillis >= WifiDelayInterval)
  {
    WifiDelayMillis = currentMillis;
    if (!client.connected())
    {
      Serial.println("reconnecting ...");
      reconnect();
    }
    else if (lastCount != count)
    {
      char buffer[200];
      updater["Disconnected"] = count;
      serializeJson(updater, buffer);
      client.publish(mqttDisconnectTopic, buffer, true);
      lastCount = count;
    }
  }

  if (currentMillis - TempDelayMillis >= TempDelayInterval)
  {
    TempDelayMillis = currentMillis;
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    char bufferx[150];
    tempdoc["ServerTemp"] = tempC;
    serializeJson(tempdoc, bufferx);
    client.publish("ServerTemp", bufferx, true);
  }
  
  if (currentMillis - StrongPingDelayMillis <= 120)
  {
    StrongServerPingC = pingStrongServer();
    Serial.println("pinging Strong...");
    delay(80);
  }
  if (currentMillis - CoolPingDelayMillis <= 120)
  {
    CoolServerPingC = pingCoolServer();
    Serial.println("pinging Cool ...");
    delay(80);
  }

  client.loop();
}