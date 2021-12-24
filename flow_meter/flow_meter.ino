/*
 * flow meter 
 * running on a mini ESP8266 ; original link is dead, but description was 5-sets-D1-Mini-Mini-nodemcu-4-m-bytes-moon-esp8266-WiFi-Internet-of-things-based
 * as of dec 22nd 2021, https://www.aliexpress.com/item/32851142960.html shows a similar offering
 * select NodeMCU 1.0 (ESP-12E module) to program, for whatever reason the board i have has the upload working at 57600.
 * initially developed on arduino 1.8.13
 * 
 * eventually:
 *      connect to a server
 *          after flow event, send a packet that has total flow and a time stamp, with maybe some stats - peak, median...
 *          update a local display
 *          send sme other stats (battery status?)
 *      upon command, light display etc.
 *      sleep most of the time
 *
 * NOTE:
 *  must define const char *ssid, *passwd, *host, name and const int port in secrets.h, giving the appropriate network info.
 */

#include "secrets.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define FLOW_PIN D1  // D4 wasn't a good idea, see e.g. https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
#define MAX_MSEC_WHEN_FLOWING 50

const float pulses_per_litre = 450.0;

WiFiUDP udp;

uint32_t counter = 0, last_dt = 0, last_count = 0, flow_period_start = 0, flow_period_start_counts = 0, last_flow_length_ms = 0, last_flow_counts = 0;
bool flowing = false, new_flow_event = false;

ICACHE_RAM_ATTR void pin_isr(void) {
    counter++;
    uint32_t now = millis();
    last_dt = now - last_count;
    last_count = now;
    if (last_dt < MAX_MSEC_WHEN_FLOWING) {
        if (!flowing) {
            flow_period_start = now;
            flow_period_start_counts = counter;
        }
        flowing = true;
    }
    else {
        if (flowing) {
            last_flow_length_ms = now - flow_period_start;
            last_flow_counts = counter - flow_period_start_counts;
            new_flow_event = true;
        }
        flowing = false;
    }
}

/*
 * send a packet of data to the server
 * Args:
 *      char *packetBuffer : buffer
 *      int j : length
 */
void send_packet(char *packetBuffer, int j) {
    udp.beginPacket(host, port);
    udp.write(packetBuffer, j);
    udp.endPacket();
    return;
}


void setup() {
    int i = 0;

    Serial.begin(115200); //Start Serial at 115200bps
    delay(100); // probably unnecesssary
    pinMode(FLOW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pin_isr, RISING);

    Serial.println("About to connect to network");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println(i);
        i++;
    }
    Serial.println("");

    Serial.print("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
}


void loop() {
    char output_s[256] = {0};
    int len_of_output = -1;
    float last_flow_litres = 0.0;

    sprintf(output_s, "%0ld, flow %0d", counter, flowing);
    Serial.println(output_s);
    if (new_flow_event) {
        new_flow_event = false;
        last_flow_litres = last_flow_counts / pulses_per_litre;
        len_of_output = sprintf(output_s, "%s : new flow event total %0.3f L, over %0ld msec ", name, last_flow_litres, last_flow_length_ms);
        Serial.println(output_s);
        send_packet(output_s, len_of_output);
    }
    delay(1000);
};
