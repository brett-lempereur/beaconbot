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
const uint8_t BASE_COUNT = 20;
const uint8_t BASE_PIN = 3;

// Brand light configuration.
const uint8_t BRAND_COUNT = 13;
const uint8_t BRAND_PIN_LEFT = 5;
const uint8_t BRAND_PIN_RIGHT = 6;

// Wireless network configuration.
char* WIFI_SSID = "DoESLiverpool";
char* WIFI_KEY = "decafbad00";

// Messaging configuration.
char* MQTT_SERVER = "m20.cloudmqtt.com";
int MQTT_PORT = 15508;
char* MQTT_USER = "vujsgagk";
char* MQTT_PASSWORD = "AtgjF07N_ahU";
char* MQTT_TOPIC = "beacon/colour";

// Base lighting status.
CRGB base_leds[BASE_COUNT];

// Brand lighting status.
uint8_t brand_percent;
CRGB brand_leds_p[BRAND_COUNT];
CRGB brand_leds[BRAND_COUNT];

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
    FastLED.addLeds<WS2812, BRAND_PIN_LEFT, GRB>(brand_leds, BRAND_COUNT);
    FastLED.addLeds<WS2812, BRAND_PIN_RIGHT, GRB>(brand_leds, BRAND_COUNT);
    fill_solid(base_leds, BASE_COUNT, CRGB(255, 255, 255));
    fill_solid(brand_leds, BRAND_COUNT, CRGB(255, 255, 255));
    fill_solid(brand_leds_p, BRAND_COUNT, CRGB(255, 255, 255));
    FastLED.show();

    // Connect to the messaging server and subscribe.
    Serial.print("Connecting to messaging server: ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
    while (!mqtt.connect("arduino-beaconbot", MQTT_USER, MQTT_PASSWORD)) {
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

    // Try to process messages from the messaging server, and if we're
    // disconnected attempt to reconnect.
    if (!mqtt.loop()) {
        Serial.println("Disconnected from messaging server, reconnecting.");
        while (!mqtt.connect("arduino-beaconbot", MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("Retrying connection to messaging server.");
        }
        Serial.println("Reconnected to messaging server.");
    }

    // Animate the lights.
    anim_blink(brand_percent);

    // If we've reached the end of the animation, reset the counter and copy
    // the current light status to the previous.
    if (brand_percent == 255) {
        brand_percent = 0;
        memcpy(brand_leds_p, brand_leds, sizeof(CRGB) * BRAND_COUNT);
    } else {
        brand_percent++;
    }

    // Pause before continuing so the animation doesn't run too fast.
    delay(10);

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

/**
 * Blink brand lighting animation routine.
 */
void anim_blink(const uint8_t p)
{

    // Special case to ensure the final light is enabled before this animation
    // routine terminates.
    if (p == 255) {
        memcpy(brand_leds, brand_leds_p, sizeof(CRGB) * BRAND_COUNT);
    } else {
        // Iterate over all lights, setting an LED to black based on the current
        // progress and copying the rest from the previous value. 
        for (int i = 0; i < BRAND_COUNT; i++) {
            if (scale8(p, BRAND_COUNT) == i) {
                brand_leds[i] = CRGB::Black;
            } else {
                brand_leds[i] = brand_leds_p[i];
            }
        }
    }

    // Update the display.
    FastLED.show();

}

