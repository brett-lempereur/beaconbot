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
const uint8_t BRAND_FUNCTIONS = 3;

// Wireless network configuration.
char* WIFI_SSID = "";
char* WIFI_KEY = "";

// Messaging configuration.
char* MQTT_SERVER = "m20.cloudmqtt.com";
int MQTT_PORT = 11475;
char* MQTT_USER = "arduino";
char* MQTT_PASSWORD = "arduinopassword";
char* MQTT_TOPIC = "beacon/colour";

// Base lighting status.
CRGB base_leds[BASE_COUNT];

// Brand lighting status.
uint8_t brand_percent, brand_function;
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
#ifdef DEBUG
    Serial.begin(9600);
#endif

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

    // Initialise the lighting strip.
    FastLED.addLeds<WS2812, BASE_PIN, GRB>(base_leds, BASE_COUNT);
    FastLED.addLeds<WS2812, BRAND_PIN_LEFT, GRB>(brand_leds, BRAND_COUNT);
    FastLED.addLeds<WS2812, BRAND_PIN_RIGHT, GRB>(brand_leds, BRAND_COUNT);
    fill_solid(base_leds, BASE_COUNT, CRGB(255, 255, 255));
    fill_solid(brand_leds, BRAND_COUNT, CRGB(255, 255, 255));
    fill_solid(brand_leds_p, BRAND_COUNT, CRGB(255, 255, 255));
    FastLED.show();

    // Initialise the brand lighting animation.
    brand_percent = 0;
    brand_function = random(BRAND_FUNCTIONS - 1);

    // Connect to the messaging server and subscribe.
#ifdef DEBUG
    Serial.print("Connecting to messaging server: ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.println(MQTT_PORT);
#endif
    while (!mqtt.connect("arduino-beaconbot", MQTT_USER, MQTT_PASSWORD)) {
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
    
    // Debug messaging.
#ifdef DEBUG
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
#if DEBUG
        Serial.println("Disconnected from messaging server, reconnecting.");
#endif
        while (!mqtt.connect("arduino-beaconbot", MQTT_USER, MQTT_PASSWORD)) {
#ifdef DEBUG
            Serial.println("Retrying connection to messaging server.");
#endif
        }
#ifdef DEBUG
        Serial.println("Reconnected to messaging server.");
        Serial.print("Resubscribing to topic: ");
        Serial.println(MQTT_TOPIC);
#endif
        mqtt.subscribe(MQTT_TOPIC);
    }

    // Step the lights through the animation once every ten milliseconds.
    if (millis() % 10 == 0) {
        switch (brand_function) {
            case 0:
                anim_blink();
                break;
            case 1:
                anim_chase();
                break;
            case 2:
                anim_fade();
                break;
        }
        if (brand_percent == 255) {
            brand_function = random(BRAND_FUNCTIONS);
            brand_percent = 0;
            memcpy(brand_leds_p, brand_leds, sizeof(CRGB) * BRAND_COUNT);
        } else {
            brand_percent++;
        }
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
        Serial.print("Invalid message recieved for topic '");
        Serial.print(topic);
        Serial.print("', length ");
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

/**
 * Chase brand lighting animation routine.
 */
void anim_chase()
{

    // Special case to ensure the final light is enabled before this animation
    // routine terminates.
    if (brand_percent == 255) {
        memcpy(brand_leds, brand_leds_p, sizeof(CRGB) * BRAND_COUNT);
    } else {
        // Iterate over all lights, setting an LED to black based on the current
        // progress and copying the rest from the previous value. 
        for (int i = 0; i < BRAND_COUNT; i++) {
            if (scale8(brand_percent, BRAND_COUNT) == i) {
                brand_leds[i] = CRGB::Black;
            } else {
                brand_leds[i] = brand_leds_p[i];
            }
        }
    }

    // Update the display.
    FastLED.show();

}

/**
 * Blink brand lighting animation routine.
 */
void anim_blink()
{
    if ((brand_percent / 16) % 2 == 0) {
        fill_solid(brand_leds, BRAND_COUNT, CRGB::Black);
    } else {
        memcpy(brand_leds, brand_leds_p, sizeof(CRGB) * BRAND_COUNT);
    }
    FastLED.show();
}

/**
 * Pick and fade to a random colour.
 */
void anim_fade()
{
    static CRGB target;

    // If this is the beginning of a fade, pick a random colour.
    if (brand_percent == 0) {
        CHSV rnd;
        rnd.h = random8();
        rnd.s = 255;
        rnd.v = 255;
        target = rnd;
    }

    // Interpolate the current colour.
    CRGB current;
    current.r = lerp8by8(brand_leds_p[0].r, target.r, brand_percent);
    current.g = lerp8by8(brand_leds_p[0].g, target.g, brand_percent);
    current.b = lerp8by8(brand_leds_p[0].b, target.b, brand_percent);

    // Fill the array and show the result.
    fill_solid(brand_leds, BRAND_COUNT, current);
    FastLED.show();
}

