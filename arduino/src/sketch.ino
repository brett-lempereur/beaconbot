/**
 * @file sketch.ino
 * @author Brett Lempereur
 *
 * Embedded lighting controller for the DoES Liverpool radio tower.
 */

#include <SPI.h>
#include <WiFi.h>

#include <FastAnimation.h>
#include <FastLED.h>
#include <PubSubClient.h>

// Base light configuration.
const uint8_t BASE_COUNT = 20;
const uint8_t BASE_PIN = 3;

// Brand light configuration.
const uint8_t BRAND_COUNT = 13;
const uint8_t BRAND_PIN_LEFT = 5;
const uint8_t BRAND_PIN_RIGHT = 6;
const uint8_t BRAND_FUNCTIONS = 3;

// Wireless network configuration.
char* WIFI_SSID = "";
char* WIFI_KEY = "";

// Messaging configuration.
char* MQTT_ID = "arduino-beaconbot";
char* MQTT_SERVER = "m20.cloudmqtt.com";
int MQTT_PORT = 11475;
char* MQTT_USER = "arduino";
char* MQTT_PASSWORD = "arduinopassword";
char* MQTT_TOPIC = "beacon/colour";

// Animation configuration.
const uint8_t ANIM_RATE = 8;
const uint8_t ANIM_COUNT = 7;
Animation* ANIM_TYPES[] = {
    new Blink(16, CRGB::Black),
    new Blink(16, CRGB::White),
    new Chase(CRGB::Black), 
    new Chase(CRGB::White),
    new RandomWipe(255, 255),
    new RandomSolidFade(255, 255),
    new RandomFade(BRAND_COUNT, 255, 255)
};

// Base lighting status.
CRGB base_leds[BASE_COUNT];

// Brand lighting status.
CRGB brand_leds_last[BRAND_COUNT];
CRGB brand_leds[BRAND_COUNT];

// Animation controller.
RandomAnimator brand_animator(ANIM_TYPES, ANIM_COUNT);

// Messaging status.
WiFiClient mqtt_wifi;
PubSubClient mqtt(MQTT_SERVER, MQTT_PORT, on_colour, mqtt_wifi);

/**
 * Initialise the board.
 */
void setup()
{

#ifdef DEBUG
    // Initialise the serial port for debugging.
    Serial.begin(9600);
#endif

    // Initialise the lighting strip.
    FastLED.addLeds<WS2812, BASE_PIN, GRB>(base_leds, BASE_COUNT);
    FastLED.addLeds<WS2812, BRAND_PIN_LEFT, GRB>(brand_leds, BRAND_COUNT);
    FastLED.addLeds<WS2812, BRAND_PIN_RIGHT, GRB>(brand_leds, BRAND_COUNT);
    
    // Indicate that we're trying to connect to WiFi.
    fill_solid(base_leds, BASE_COUNT, CRGB::Red);
    fill_solid(brand_leds, BRAND_COUNT, CRGB::Red);
    FastLED.show();

    // Connect to the WiFi network.
    if (WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG
        Serial.println("No wireless shield installed.");
#endif
        while (true);
    } else {
        while (WiFi.status() != WL_CONNECTED) {
#ifdef DEBUG
            Serial.print("Connecting to SSID: ");
            Serial.println(WIFI_SSID);
#endif
            WiFi.begin(WIFI_SSID, WIFI_KEY);
        }
    }
#ifdef DEBUG
    Serial.print("Connected to network: ");
    Serial.print(WiFi.localIP());
    Serial.print(", gateway: ");
    Serial.println(WiFi.gatewayIP());
#endif

    // Indicate that we have connected to Wi-Fi.
    fill_solid(base_leds, BASE_COUNT, CRGB::Green);
    fill_solid(brand_leds, BRAND_COUNT, CRGB::Green);
    FastLED.show();
    
    // Check connectivity.
#ifdef DEBUG
    Serial.println("Checking connection: www.google.com:80");
#endif
    while (!mqtt_wifi.connect("www.google.com", 80)) {
#ifdef DEBUG
        Serial.println("Could not connect to: www.google.com:80");
#endif
        delay(1000);
    }
#ifdef DEBUG
    Serial.println("Connected to: www.google.com:80");
#endif
    mqtt_wifi.stop();

    // Indicate that we have connected to the test server.
    fill_solid(base_leds, BASE_COUNT, CRGB::Blue);
    fill_solid(brand_leds, BRAND_COUNT, CRGB::Blue);
    FastLED.show();

    // Connect to the messaging server and subscribe.
#ifdef DEBUG
    Serial.print("Connecting to messaging server: ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
#endif
    while (!mqtt.connect(MQTT_ID, MQTT_USER, MQTT_PASSWORD)) {
#ifdef DEBUG
        Serial.println("Retrying connection to messaging server.");
#endif
        delay(1000);
    }
#ifdef DEBUG
    Serial.print("Subscribing to topic: ");
    Serial.println(MQTT_TOPIC);
#endif
    mqtt.subscribe(MQTT_TOPIC);

    // Indicate that we have completed initialisation.
    fill_solid(base_leds, BASE_COUNT, CRGB::White);
    fill_solid(brand_leds, BRAND_COUNT, CRGB::White);
    fill_solid(brand_leds_last, BRAND_COUNT, CRGB::White);
    FastLED.show();
    
#ifdef DEBUG
    // Debug messaging.
    Serial.println("Initialisation complete, waiting for messages.");
#endif

}

/**
 * Update the static lights and check for messages.
 */
void loop()
{

    // Try to process messages from the messaging server, and if we're
    // disconnected attempt to reconnect.
    if (!mqtt.loop()) {
#ifdef DEBUG
        Serial.println("Disconnected from messaging server, reconnecting.");
#endif
        while (!mqtt.connect("arduino-beaconbot", MQTT_USER, MQTT_PASSWORD)) {
#ifdef DEBUG
            Serial.println("Retrying connection to messaging server.");
#endif
        }
#ifdef DEBUG
        Serial.println("Reconnected to messaging server.");
        Serial.print("Subscribing to topic: ");
        Serial.println(MQTT_TOPIC);
#endif
        mqtt.subscribe(MQTT_TOPIC);
    }

    // Step the lights through the animation without delaying so that we
    // minimise missed MQTT PING messages.
    if (millis() % ANIM_RATE == 0) {
        brand_animator.update(brand_leds, brand_leds_last, BRAND_COUNT);
        FastLED.show();
    }

}

/**
 * Called whenever an updated colour is received from the controlling 
 * software.
 */
void on_colour(char* topic, uint8_t* payload, unsigned int length)
{

    // Payload length must be three bytes.
    if (length != 3) {
#ifdef DEBUG
        Serial.print("Invalid message of length: ");
        Serial.println(length);
#endif
        return;
    }

    // Convert the colour.
    CRGB *colour = (CRGB*)payload;

    // Debug messaging.
#ifdef DEBUG
    Serial.print("Received message on topic '");
    Serial.print(topic);
    Serial.print("', setting colour to ");
    Serial.print(colour->r);
    Serial.print(",");
    Serial.print(colour->g);
    Serial.print(",");
    Serial.println(colour->b);
#endif

    // Update the colour of the display.
    fill_solid(base_leds, BASE_COUNT, *colour);
    FastLED.show();

}

