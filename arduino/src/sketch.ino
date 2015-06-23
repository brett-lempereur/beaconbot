/**
 * @file sketch.ino
 * @author Brett Lempereur
 *
 * Embedded lighting controller for the DoES Liverpool radio tower.
 */

#include <WiFi.h>

#include <FastLED.h>
#include <PubSubClient.h>

// Base light configuration.
const uint8_t BASE_COUNT = 8;
const uint8_t BASE_PIN = 9;

// Wireless network configuration.
char* WIFI_SSID = "";
char* WIFI_KEY = "";

// Messaging configuration.
char* MQTT_SERVER = "m20.cloudmqtt.com";
int MQTT_PORT = 15508;
char* MQTT_USER = "";
char* MQTT_PASSWORD = "";
char* MQTT_TOPIC = "beacon/colour";

// Lighting status.
CRGB base_leds[BASE_COUNT];

// Messaging status.
WiFiClient mqtt_wifi;
PubSubClient mqtt(MQTT_SERVER, MQTT_PORT, on_colour, mqtt_wifi);

/**
 * Initialise the board.
 */
void setup()
{

    // Initialise the serial port for debugging.
    Serial.begin(9600);

    // Connect to the WiFi network.
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("No wireless shield installed.");
        while (true);
    } else {
        while (WiFi.status() != WL_CONNECTED) {
            Serial.print("Connecting to SSID: ");
            Serial.println(WIFI_SSID);
            WiFi.begin(WIFI_SSID, WIFI_KEY);
        }
    }
    Serial.print("Connected to network: ");
    Serial.print(WiFi.localIP());
    Serial.print(", gateway: ");
    Serial.println(WiFi.gatewayIP());

    // Check connectivity.
    Serial.println("Checking connection: www.google.com:80");
    while (!mqtt_wifi.connect("www.google.com", 80)) {
        Serial.println("Could not connect to: www.google.com:80");
        delay(1000);
    }
    Serial.println("Connected to: www.google.com:80");
    mqtt_wifi.stop();

    // Initialise the lighting strip.
    FastLED.addLeds<WS2812, BASE_PIN, GRB>(base_leds, BASE_COUNT);
    fill_solid(base_leds, BASE_COUNT, CRGB(255, 255, 255));
    FastLED.show();

    // Connect to the messaging server and subscribe.
    Serial.print("Connecting to messaging server: ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
    while (!mqtt.connect("arduino-mqttower", MQTT_USER, MQTT_PASSWORD)) {
        Serial.println("Retrying connection to messaging server.");
        delay(1000);
    }
    Serial.print("Subscribing to topic: ");
    Serial.println(MQTT_TOPIC);
    mqtt.subscribe(MQTT_TOPIC);
    
    // Debug messaging.
    Serial.println("Initialisation complete, waiting for messages.");

}

/**
 * Update the static lights and check for messages.
 */
void loop()
{
    mqtt.loop();
}

/**
 * Called whenever an updated colour is received from the controlling 
 * software.
 */
void on_colour(char* topic, uint8_t* payload, unsigned int length)
{

    // Payload length must be three bytes.
    if (length != 3) {
        Serial.print("Invalid message recieved for topic '");
        Serial.print(topic);
        Serial.print("', length ");
        Serial.println(length);
        return;
    }

    // Convert the colour.
    CRGB *colour = (CRGB*)payload;

    // Debug messaging.
    Serial.print("Received message on topic '");
    Serial.print(topic);
    Serial.print("', setting colour to ");
    Serial.print(colour->r);
    Serial.print(",");
    Serial.print(colour->g);
    Serial.print(",");
    Serial.println(colour->b);

    // Update the colour of the display.
    fill_solid(base_leds, BASE_COUNT, *colour);
    FastLED.show();

}

