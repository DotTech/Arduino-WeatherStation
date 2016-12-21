/*
	Weather station code.

	- ESP8266 must have a WiFi connection configured.
	- ESP8266 must be modified to support deep sleep (GPIO16 connected to RST).
	- Arduino must run at 3.3v!!
*/

#include "DHT11.h"
#include <SoftwareSerial.h>

#define ESP_RXPIN 3
#define ESP_TXPIN 4 // TODO: Resolder pin!
#define ESP_INTERRUPT_PIN 2 // TODO: Solder to ESP RST PIN
#define SOLAR_VOLTAGE_PIN 1
#define BATTERY_VOLTAGE_PIN 0
#define LDR_PIN 2
#define DHT_PIN 8
#define DEBUGMODE false

// Global variables
SoftwareSerial ESP(ESP_RXPIN, ESP_TXPIN);
dht11 DHT;
bool just_woke = false;

// Constants
const float REFERENCE_VOLTAGE = 3.3;
const unsigned long REQUEST_TIMEOUT = 5000;
const long ESP_BOOT_TIME = 10000;
const long SLEEP_TIMEOUT = 900000;
const long WAIT_BEFORE_SLEEP = 5000;
const char API_DOMAIN[] = "api.thingspeak.com";
const char API_REQUEST_FORMAT[] = "GET https://api.thingspeak.com/update?api_key=D5VJU895LRPE1FXG&field1=%TEMPERATURE%&field2=%HUMIDITY%&field3=%SOLAR_VOLTAGE%&field4=%BATTERY_VOLTAGE%&field5=%LIGHT% HTTP/1.0";
const char CRLF[] = "\r\n";

// Setup code
void setup() 
{	
	attachInterrupt(digitalPinToInterrupt(ESP_INTERRUPT_PIN), wakeup_interrupt, FALLING);

	analogReference(EXTERNAL);
	
	if (DEBUGMODE)
	{
		Serial.begin(115200);
		Serial.println("Setting up...");
	}

	setup_sleep();
	ESP_setup();

	DHT.begin();
}

// Main loop
void loop()
{
	if (DEBUGMODE)
	{
		Serial.println("loop()");
	}

	just_woke = false;
	delay(ESP_BOOT_TIME);

	enable_adc();
	delay(500);

	read_dht();
	submit_data(get_temperature(), get_humidity(), get_light(), get_solar_voltage(), get_battery_voltage());

	delay(WAIT_BEFORE_SLEEP);

	if (DEBUGMODE)
	{
		Serial.println("Enter sleep mode");
	}

	disable_adc();
	
	sleep(SLEEP_TIMEOUT);
	
	// Arduino will sleep indefinitely until ESP_INTERRUPT pin is pulled low by the ESP module, which happens when the ESP wakes up.
}

// Submit the collected data to the endpoint, using the ESP8266.
void submit_data(float temperature, float humidity, int light, float solar_voltage, float battery_voltage)
{
	String data = String(API_REQUEST_FORMAT);

	data.replace("%TEMPERATURE%", String(temperature, 2));
	data.replace("%HUMIDITY%", String(humidity, 2));
	data.replace("%LIGHT%", String(light));
	data.replace("%SOLAR_VOLTAGE%", String(solar_voltage, 2));
	data.replace("%BATTERY_VOLTAGE%", String(battery_voltage, 2));

	if (DEBUGMODE)
	{
		Serial.print("Request data: ");
		Serial.println(data);
	}

	char request[data.length() + 2];
	data.toCharArray(request, data.length() + 1);

	Serial.print("ToCharAray: ");
	Serial.println(request);

	ESP_get(request);
}