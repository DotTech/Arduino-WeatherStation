// Setup Arduino so it can be put into deep sleep mode.
void setup_sleep()
{
	// Save power by assigning pin mode to all pins
	for (int i = 0; i < 14; i++) 
	{
		if (i != ESP_RXPIN && i != ESP_TXPIN && i != DHT_PIN && i != ESP_INTERRUPT_PIN)
		{
			pinMode(i, OUTPUT);
		}
	}

	// Enable sleep mode option.
	SMCR |= (1 << 2);
	SMCR |= 1;
}

// Put Arduino in power down sleep and ESP in deep sleep mode.
// Arduino sleeps indefinitely until woken by an interrupt.
// ESP sleeps for specified number of seconds and pulls interrupt low when its woken.
// @miliseconds: sleep time in miliseconds.
void sleep(long miliseconds)
{
	if (just_woke)
	{
		Serial.println("sleep(): just_woke=true, return..");
		return;
	}

	Serial.println("Call ESP_enter_sleep");
	ESP_enter_sleep_mode(miliseconds);

	delay(2000);

	Serial.println("Call arduino_sleep()");
	delay(1000);

	if (just_woke)
	{
		Serial.println("sleep(): [2] just_woke=true, return..");
		return;
	}

	arduino_sleep();

	Serial.println("Continue after sleep");
}

// Disable analog to digital converter.
void disable_adc()
{
	ADCSRA &= ~(1 << 7);
}

// Enable analog to digital converter.
void enable_adc()
{
	ADCSRA |= (1 << 7);
}

// Enter sleep mode and wakeup after 8 seconds using watchdog timer.
void arduino_sleep()
{
	MCUCR |= (3 << 5);
	MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6);
	__asm__  __volatile__("sleep");
}

// Wake up interrupt.
void wakeup_interrupt()
{
	Serial.println("Wake up interup!");
	just_woke = true;
}

// Watchdog timer interupt.
//ISR (WDT_vect)
//{
	// Do not remove this function!
//}