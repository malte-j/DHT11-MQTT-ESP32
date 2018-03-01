#include <WiFi.h>
#include <MQTTClient.h>
#include <DHT.h>

DHT dht;

const char ssid[] = "EnlightedBASE";
const char pass[] = "8921539555159684";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "esp32", "esp32pwd")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  client.unsubscribe("/hello");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  client.begin("192.168.178.66", net);
  client.onMessage(messageReceived);
	dht.setup(14);
  connect();
}

void loop() {
  client.loop();
	delay(dht.getMinimumSamplingPeriod());
	
	float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
	String statusString = dht.getStatusString();

  if (!client.connected()) {
    connect();
  }

	if(dht.getStatusString() != "OK") {
		Serial.println("ERROR: " + statusString);
		return;
	}

  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(humidity, 1);
  Serial.print("\t\t");
  Serial.println(temperature, 1);

  // publish a message roughly every five seconds.
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    client.publish("sensor/humidity", String(humidity));
    client.publish("sensor/temperature", String(temperature));
  }
}

