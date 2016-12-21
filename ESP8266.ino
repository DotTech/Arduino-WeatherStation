// Setup the ESP8266 for communication.
void ESP_setup()
{
	pinMode(ESP_TXPIN, OUTPUT);
	pinMode(ESP_RXPIN, INPUT);

	ESP.begin(115200);
	ESP.listen();

	ESP_read();
}

// Enter deep sleep mode until timeout.
// @miliseconds: sleep timeout in miliseconds.
void ESP_enter_sleep_mode(long miliseconds)
{
	ESP.print("AT+GSLP=");
	ESP.print(miliseconds);
	ESP.print(CRLF);

	// Try entering sleep mode until we succeed.
	bool read_ok = ESP_read_ok();

	Serial.print("Read ok: ");
	Serial.println(read_ok);

	if (!read_ok)
	{
		delay(REQUEST_TIMEOUT);
		ESP_enter_sleep_mode(miliseconds);
	}
}

// Perform an HTTP GET request using the specified URL.
// @request: HTTP request data.
void ESP_get(char request[])
{
	ESP.print("AT+CIPMUX=1");
	ESP.print(CRLF);

	delay(500);

	ESP.print("AT+CIPSTART=0,\"TCP\",\"api.thingspeak.com\",80");
	ESP.print(CRLF);
	ESP_read();

	delay(500);

	ESP.print("AT+CIPSEND=0,");
	ESP.print(strlen(request) + 6);
	ESP.print(CRLF);
	ESP_read();

	delay(500);

	ESP.print(request);
	ESP.print(CRLF);
	ESP.print(CRLF);
	ESP.print(CRLF);
	ESP_read();

	delay(500);
}

// Wait for the ESP to return data or until timeout.
void ESP_read()
{
	unsigned long timeoutWaitStart = millis();

	while (!ESP.available())
	{
		if (millis() - timeoutWaitStart > REQUEST_TIMEOUT)
		{
			return;
		}
	}
	
	char data = (char)ESP.read();

	if (DEBUGMODE)
	{
		Serial.print(data);
	}

	ESP_read();
}

// Read ESP data until OK is returned.
bool ESP_read_ok()
{
	unsigned long timeoutWaitStart = millis();

	while (!ESP.available())
	{
		if (millis() - timeoutWaitStart > REQUEST_TIMEOUT)
		{
			return false;
		}
	}

	bool first = false;

	while (1)
	{
		if (!first)
		{
			if ((char)ESP.read() == 'O')
			{
				first = true;
			}
		}
		else
		{
			return (char)ESP.read() == 'K';
		}

		while (!ESP.available())
		{
			if (millis() - timeoutWaitStart > REQUEST_TIMEOUT)
			{
				return false;
			}
		}
	}
}