// @baldengineer
// WiFi9600 Modem 
// from livestreams twitch.tv/baldengineer
//

/* The digital pins map to these numbers, for whatever reason
TX 	TXD 	TXD
RX 	RXD 	RXD
A0 	Analog input, max 3.2V 	A0

D0 	IO 	GPIO16
D1 	IO, GPIO5 	SCL 
D2 	IO, GPIO4 	SDA 	
D3 	IO, GPIO0	10k Pull-up 	
D4 	IO, GPIO2 	10k Pull-up, BUILTIN_LED 	
D5 	IO, GPIO14	SCK 	
D6 	IO, GPIO12 	MISO 	
D7 	IO, GPIO13	MOSI 	
D8 	IO, 10k Pull-down, SS 	GPIO15

*/

#include "SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h" //http://librarymanager/All#SparkFun_MP3_Trigger
MP3TRIGGER mp3;
byte speaker_volume = 24;

const byte ans_pushbutton = 0; // D3, on pcb: relay1

// clicky-click noise makers
//const byte relay1 = 15; // D8 on pcb: ANS_PB
const byte pots_relay = 2; // D4
bool relay_state = false;

const byte led_pin = 16; // D0

// for the sparkfun mp3 player
const byte i2c_SCL = 4; //D1
const byte i2c_SDA = 5; //D2

// lonely incoming signal from RS232
const byte carrier_detect = A0;

// shift register on front panel
const byte latch_595 = 13; //13 - D7
const byte clock_595 = 12; //12 - D6
const byte data_595 = 14;  //14 - D5

// LED states, to make code more readable
#define TURN_ON  true
#define TURN_OFF false

#define     PRESSED 0x0
#define NOT_PRESSED 0x1

// activity indicator 
bool TX_state = TURN_OFF;
bool RX_state = TURN_OFF;

// bit pattern for led_states
#define PANEL_EN 0x7FFF
#define CD    0x0001 //!
#define AA    0x0002 //!
#define TR    0x0004 //!
#define OH    0x0008 //!
#define DC	  0x0100 
#define HSgrn 0x0400
#define HSred 0x0800
#define ECgrn 0x1000
#define ECred 0x0200
#define RX    0x2000 //!
#define TX    0x4000 //!

// The 11 LEDs, including bi-color. Visual top
// to bottom order:
uint16_t front_leds[11] = {TR, TX, RX, OH, CD, AA, HSgrn, HSred, ECgrn, ECred, DC};

// Need 16 bits because of the two shift registers.
// 4 bits not used. Signals are wired as:
// EN TX RX ECgrn   HSred HSgrn ECred DC   X X X X   OH TR AA CD
uint16_t led_states = 0x0000;

// millis() variabled for random_led() to simulate activity
unsigned long previous_activity = 0;

// millis() for TX and RX activity
unsigned long previous_TX_act = 0;
unsigned long previous_RX_act = 0;
unsigned long activity_interval = 10; // may need separate intervals for RX and TX

