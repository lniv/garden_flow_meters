# garden_flow_meters

framework for a few garden flow meters to report data to a server, in order to keep track of e.g water leaks in our marvelous watering system.
basic idea for now:
* flow meter is composed of
   * flow sensor - e.g. HiLetgo 3-01-1257
   * esp8266
   * pwoer source - battery + solar panel + charging controller
   * local display - OLED? for easy readout and to allow the server to display message, for confirming which flow sensor is where.
* server - probably a raspberyy pi of some kind; i want the flow meters to report flow event, not necessarily continuous flow.

NOTE: to compile esp8266 firmware, must create a secrets.h file in that folder, which must define
* const char *ssid : network id
* const char *passwd : network wifi password
* const char *host : address of server collecting data
* const int port : port of server collecting data
* const char *name : our (node) name

the first two for joining the local network, the second two for an address to which we'll send information.
