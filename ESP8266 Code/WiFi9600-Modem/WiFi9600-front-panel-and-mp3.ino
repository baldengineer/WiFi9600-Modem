// @baldengineer
// WiFi9600 Modem Front Panel test code
// from livestreams twitch.tv/baldengineer
//

// bits set to a 1 are active low. 2nd least-sig nibble
// isn't used by the hardware.
// EN TX RX ECgrn   HSred HSgrn ECred DC   X X X X   OH TR AA CD
const uint16_t invert_mask = 0b1110000000001111; 
void update_led(uint16_t led, bool state, bool update) {
	if (state == TURN_ON) 
		led_states = led_states | led;
	else
		led_states = led_states & ~led;

	if (update)
		send_led_states(led_states);
}

// properly masks the pattern to enable panel and XOR
// state variable to handle inverted LEDs
void send_led_states(uint16_t leds) {
	// ensure the panel-enable signal is low
	// which turns on the PFET for VCC
	leds = ((leds ^ invert_mask) & PANEL_EN);

	digitalWrite(latch_595, LOW);
	  shiftOut(data_595, clock_595, LSBFIRST, lowByte(leds));
	  shiftOut(data_595, clock_595, LSBFIRST, highByte(leds));
	digitalWrite(latch_595, HIGH);
}

// alternative to just sending send_led_update() with 0xFFFF or 0x0;
void all_led_state(bool state) {
	for (int x=0; x<11; x++) 
		update_led(front_leds[x], state, false);
}

// test routine for front panel to turn on LEDs, in order
void led_test(int wait) {
	led_states = 0x0;
	for (int x=0; x < 11; x++) {
		update_led(front_leds[x], TURN_ON, true);
		delay(wait);
		update_led(front_leds[x], TURN_OFF, true);
		delay(wait);
	}
}


// setups LEDs to simulate modem connection. 
/*
void simulate_modem_connection() {
	delay(2500);
	toggle_relay();
	mp3_play_dialout();
	update_led(OH, TURN_ON, true);
	delay(100);
	update_led(TR, TURN_ON, true);
	delay(400);
	update_led(CD, TURN_ON, true);
	delay(250);
	update_led(HSgrn, TURN_ON, true);
	delay(100);
	update_led(ECgrn, TURN_ON, true);
	delay(100);
	update_led(DC, TURN_ON, true);	
	delay(250);
	yield();
	delay(250);
	yield();
	delay(250);
	yield();
	delay(250);
	mp3_play_carrier_detect();
}
*/

// setup 595 shift registers (there are two), and turn LEDs
// off as soon as possible.
void init_shift_register() {
	pinMode(latch_595, OUTPUT);	digitalWrite(latch_595, HIGH);
	pinMode(clock_595, OUTPUT);	digitalWrite(clock_595, LOW);
	pinMode(data_595,  OUTPUT);	digitalWrite(data_595, LOW);
	send_led_states(0x0000);
	delay(500);	
}


// to simulate more realistic traffic patterns
// testing for when this code moves to the real modem firmware
/*
void simulate_traffic() {
	unsigned long current_micros = micros();
	if (current_micros - previous_baud >= baud_9600_us) {
		previous_baud = current_micros;

		// create receive traffic
		if (receive_data_pattern[receive_index]) {
			previous_RX_act = millis();
			update_led(RX, TURN_ON, true);
		}

		receive_index++;
		if (receive_index >= receive_pattern_count) {
			if (random(0,2)==1) {
				previous_TX_act = millis();			
				update_led(TX, TURN_ON, true);
			}
			receive_index = 0;
			create_random_data_pattern();
		}
	}
}*/

void activity_decay() {
/*unsigned long previous_TX_act = 0;
unsigned long previous_RX_act = 0;
unsigned long activity_interval = 50; // may need separate intervals for RX and TX*/
	unsigned long current_millis = millis();
	if (current_millis - previous_RX_act >= activity_interval) {
	//	previous_RX_act = current_micros;
		update_led(RX, TURN_OFF, true);
	}

	current_millis = millis();
	if (current_millis - previous_TX_act >= activity_interval) {
	//	previous_RX_act = current_micros;
		update_led(TX, TURN_OFF, true);
	}	
}

void mp3_play_dialout() { 
	mp3.playTrack(1);
	for (int x=0; x<5; x++) {
 	   delay(500);
	    yield();
	  }
}

void mp3_play_carrier_detect() {
	mp3.playTrack(3);
	for (int x=0; x<6; x++) {
 		delay(500);
	    yield();
	}
}

void init_mp3() {
  //Check to see if Qwiic MP3 is present on the bus
  if (mp3.begin() == false) {
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while(1) yield();
  }
  mp3.setVolume(speaker_volume);
}

void toggle_relay() {
  if (relay_state)
    relay_state = false;
  else
    relay_state = true;

	Serial.print("Relay: "); Serial.println(relay_state);

	digitalWrite(pots_relay, relay_state);
}

/* 
	// TODO: Integrate into main code base to force disconnect of telnet client
	//unsigned long previous_pb =0;
	unsigned long pb_interval = 1000;
	uint8_t previous_pb = 0x0;

	uint8_t current_pb = digitalRead(ans_pushbutton);
	if (current_pb != previous_pb) {
		toggle_relay();
		previous_pb = current_pb;
	} 

*/