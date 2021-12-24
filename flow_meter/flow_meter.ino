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
 */

#define FLOW_PIN D4
#define MAX_MSEC_WHEN_FLOWING 50

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


void setup() {

    Serial.begin(115200); //Start Serial at 115200bps
    delay(100); // delay .1s

    pinMode(FLOW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_PIN), pin_isr, RISING);
    Serial.println("up");

}


void loop() {
    char output_s[256] = {0};
    sprintf(output_s, "%0ld, flow %0d", counter, flowing);
    Serial.println(output_s);
    if (new_flow_event) {
        new_flow_event = false;
        sprintf(output_s, "new flow event total %0ld length %0ld msec ", last_flow_counts, last_flow_length_ms);
        Serial.println(output_s);
    }
    delay(1000);
};
