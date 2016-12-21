void read_dht()
{
	DHT.read(DHT_PIN);
};

// Get temperature from DHT11 sensor.
float get_temperature()
{
	return (float)DHT.temperature;
}

// Get humidity from DTH11 sensor.
float get_humidity()
{	
	return (float)DHT.humidity;
}

// Get light value from LDR (0-100%)
int get_light()
{
	// Smooth out the analog reading by taking average of 10 readings.
	int readings = 10;
	int total;
	int average;

	for (int i = 0; i < 10; i++)
	{
		total += 1023 - analogRead(LDR_PIN);
		delay(20);
	}

	if (total > 0)
	{
		average = total / readings;
	}
	else
	{
		return 0;
	}

	return map(average, 0, 1023, 0, 100);
}

// Get the battery voltage from the analog pin.
float get_battery_voltage()
{
	// Smooth out the analog reading by taking average of 10 readings.
	int readings = 10;
	int total;
	int average;

	for (int i = 0; i < 10; i++)
	{
		total += analogRead(BATTERY_VOLTAGE_PIN);
		delay(20);
	}

	if (total > 0)
	{
		average = total / readings;
	}
	else
	{
		return 0;
	}
	
	return ((float)average) * (REFERENCE_VOLTAGE / 1023) * 2;
}

// Get the solar voltage from the analog pin.
float get_solar_voltage()
{
	// Smooth out the analog reading by taking average of 10 readings.
	int readings = 10;
	int total;
	int average;

	for (int i = 0; i < 10; i++)
	{
		total += analogRead(SOLAR_VOLTAGE_PIN);
		delay(20);
	}

	if (total > 0)
	{
		average = total / readings;
	}
	else
	{
		return 0;
	}

	return ((float)average) * (REFERENCE_VOLTAGE / 1023) * 2;
}