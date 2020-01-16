
// Name:       MIDI_Encoder.ino
// Created:	   1/8/2019
// Author:     John Kinkennon

#include <arduino.h>
#include <SPI.h>
#include "User.h"
#include "Midi.h"

const int channel = FIRST_STOP_CH;  // 0 to 5 for first MIDI stops channel
const int spiInPin = 10;            // chip select for SPI input
const int spiOutPin = 9;            // chip select for SPI output
const int spi1InPin = 31;           // chip select for SPI1 input
const int spi1OutPin = 2;           // chip select for SPI1 output
const int analogPin1 = 14;          // potentiometer input pins
const int analogPin2 = 15;
const int analogPin3 = 16;
const int analogPin4 = 17;
const int analogPin5 = 18;
const int analogPin6 = 19;
const int analogPin7 = 20;
const int analogPin8 = 21;

#ifndef USE_ALLEN_CAPTURE_SUPPLY    // GPIO pins
const int relay1 = 24;        // power relay (available)
const int relay2 = 25;        // audio power relay
const int powerLamp = 26;     // 12v power lamp (available)
const int audioLamp = 27;     // audio power lamp
const int cresc1 = 28;        // crescendo stage 1 lamp (for HW cresc)
const int cresc2 = 29;        // crescendo stage 2
const int cresc3 = 30;        // crescendo stage 3
const int cresc4 = 35;        // crescendo stage 4
#else
                              // GPIO pins -- alternate use to operate Allen stops
const int pwr1 = 24;          // Allen capture supply 1, N1 input for SP1 output
const int pwr2 = 25;          // N2 -- SP2
const int pwr3 = 26;          // N3 -- SP3
const int pwr4 = 27;          // N4 -- SP4
const int pwr5 = 28;          // Allen capture supply 2, N1 input for SP1 output
const int pwr6 = 29;          // N2 -- SP2
const int pwr7 = 30;          // N3 -- SP3
const int pwr8 = 35;          // N4 -- SP4
#endif
extern consoleStop stopTable[NUM_STOP_CHANNELS][STOPS_PER_CHANNEL];

unsigned char sysex[64];      // Buffer to assemble SysEx message
int sx = 0;                   // Index into sysex[64]
keyTable8_t keyTable;
keyTableB_t keyBit;		        // true for keys which are depressed
keyTableB_t keyOn;	  	      // true for keys which are on after debounce
keyTableB_t stopOn;           // true for stops that are on
spiTable_t spiInput;	        // buffer variable to store read data
spiTable_t spiOutput;	        // buffer variable to store write data
uint16_t newPotValue[NUM_POTS];
uint16_t oldPotValue[NUM_POTS];
int keyScanCount;             // cycles 0 through 7 to allow slowing some tasks
elapsedMillis sincePending;
elapsedMillis sincePulse;
elapsedMicros sinceScan;
bool pwr1_N1_SAMsPending; // a stop has changed in the first division
bool pwr1_N2_SAMsPending; // a stop has changed in the second division
bool pwr1_N3_SAMsPending; // a stop has changed in the third division
bool pwr1_N4_SAMsPending; // a stop has changed in the fourth division
bool pwr2_N1_SAMsPending;
bool pwr2_N2_SAMsPending;
bool pwr2_N3_SAMsPending;
bool pwr2_N4_SAMsPending;
bool setSAMsTime;
bool SAMsPowerOn;         // flag set when any SAMs power is active
bool pendingOn;
bool pulseOn;
bool scanTime;

void myNoteOff(byte, byte, byte);
void myNoteOn(byte, byte, byte);

// set up the speed, mode and endianness device
SPISettings settingsIn(5000000, MSBFIRST, SPI_MODE0);

// the setup function runs once when you press reset or power the board
void setup() {
	int i, j;
	pinMode(spiInPin, OUTPUT);
	pinMode(spiOutPin, OUTPUT);
  pinMode(spi1InPin, OUTPUT);
  pinMode(spi1OutPin, OUTPUT);
	pinMode(analogPin1, INPUT);
	pinMode(analogPin2, INPUT);
	pinMode(analogPin3, INPUT);
	pinMode(analogPin4, INPUT);
	pinMode(analogPin5, INPUT);
	pinMode(analogPin6, INPUT);
	pinMode(analogPin7, INPUT);
	pinMode(analogPin8, INPUT);
#ifndef USE_ALLEN_CAPTURE_SUPPLY 
  pinMode(relay1, OUTPUT);    // SSR_1, Switched Power
  pinMode(relay2, OUTPUT);    // SSR_2, Organ Ready Power
  pinMode(powerLamp, OUTPUT); // Ground to turn on power
  pinMode(audioLamp, OUTPUT); // Ground to turn on audio pwr
  pinMode(cresc1, OUTPUT);    // Ground to turn on cresc1 lamp
  pinMode(cresc2, OUTPUT);    // Ground to turn on cresc2 lamp
  pinMode(cresc3, OUTPUT);    // Ground to turn on cresc3 lamp
  pinMode(cresc4, OUTPUT);    // Ground to turn on cresc4 lamp
#else
  pinMode(pwr1, OUTPUT);      // Alternate use of open collector outputs
  pinMode(pwr2, OUTPUT);      //  for Allen capture power supply
  pinMode(pwr3, OUTPUT);
  pinMode(pwr4, OUTPUT);
  pinMode(pwr5, OUTPUT);
  pinMode(pwr6, OUTPUT);
  pinMode(pwr7, OUTPUT);
  pinMode(pwr8, OUTPUT);
  digitalWrite(pwr1, LOW);    // LOW into ULN2803 is high (off) to Allen capture supply input
  digitalWrite(pwr2, LOW);
  digitalWrite(pwr3, LOW);
  digitalWrite(pwr4, LOW);
  digitalWrite(pwr5, LOW);
  digitalWrite(pwr6, LOW);
  digitalWrite(pwr7, LOW);
  digitalWrite(pwr8, LOW);
#endif
	
	for (i = 0; i < NUM_SWITCHES; i++) {
		keyTable.O[i] = 0;
		keyBit.O[i] = false;
		keyOn.O[i] = false;
    stopOn.O[i] = false;
	}
	for (i = 0; i < NUM_CHANNELS; i++) {
		for (j = 0; j < NUM_REGISTERS; j++) {
			spiInput.C[i][j] = 0;
			spiOutput.C[i][j] = 0xff;
		}
	}
  for (i = 0; i < NUM_POTS; i++) {
    newPotValue[i] = 0;
    oldPotValue[i] = 0;
  }

  keyScanCount = 0;
  setSAMsTime = false;
  pwr1_N1_SAMsPending = false;
  pwr1_N2_SAMsPending = false;
  pwr1_N3_SAMsPending = false;
  pwr1_N4_SAMsPending = false;
  pwr2_N1_SAMsPending = false;
  pwr2_N2_SAMsPending = false;
  pwr2_N3_SAMsPending = false;
  pwr2_N4_SAMsPending = false;
  SAMsPowerOn = false;
  pendingOn = false;
  pulseOn = false;
  scanTime = true;

  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleNoteOn(myNoteOn);
#ifndef USE_ALLEN_CAPTURE_SUPPLY
  usbMIDI.setHandleSystemExclusive(mySystemExclusiveChunk);
#endif  
  // similar callbacks available for other MIDI message types

  // initialize SPI:
  SPI.begin();      // connectors X1 and X2
  SPI1.begin();     // connectors X3 and X4
}

// the loop function runs over and over again until power down or reset
void loop() {
	int i, j, x, y;

  while (scanTime == false) {       // for now just delay here
    if (sinceScan >= 330) {
      scanTime = true;
      sinceScan = 0;
      keyScanCount++;
      if (keyScanCount > 7) keyScanCount = 0;
    }
  }
  scanTime = false;
  
	// read and write all SPI devices
	SPI.beginTransaction(settingsIn);
  SPI1.beginTransaction(settingsIn);

	// read and write 8 registers
	digitalWrite(spiInPin, HIGH);  // HIGH to shift, LOW to load
  digitalWrite(spiOutPin, HIGH);
  digitalWrite(spi1InPin, HIGH);
  digitalWrite(spi1OutPin, HIGH);

	for (i = 0; i < NUM_CH_PER_PORT; i++) {
    x = (NUM_CHANNELS - 1) - i + channel;   // Align outbound stops data with stops input
    if (x > (NUM_CHANNELS - 1)) x = 0;      //   since input only boards do not delay output.
    if (channelTable[i] == STOPS_CH) {
      for (j = 7; j>= 4; j--) {
        spiInput.C[i][j - 4] = SPI.transfer(spiOutput.C[x][j]);
      }
      for (j = 3; j >= 0; j--) {
        spiInput.C[i][j + 4] = SPI.transfer(spiOutput.C[x][j]);
      }
    } else {
      for (j = NUM_REGISTERS - 1; j >= 0; j--) {
        spiInput.C[i][j] = SPI.transfer(spiOutput.C[x][j]);
	    }
    }
	}
 for (i = NUM_CH_PER_PORT; i < NUM_CHANNELS; i++) {
    x = (NUM_CHANNELS - 1) - i + channel;   // Align outbound stops data with stops input
    if (x > (NUM_CHANNELS - 1)) x = 0;      //   since input only boards do not delay output.
    if (channelTable[i] == STOPS_CH) {
      for (j = 7; j>= 4; j--) {
        spiInput.C[i][j - 4] = SPI1.transfer(spiOutput.C[x][j]);
      }
      for (j = 3; j >= 0; j--) {
        spiInput.C[i][j + 4] = SPI1.transfer(spiOutput.C[x][j]);
      }
    } else {
      for (j = NUM_REGISTERS - 1; j >= 0; j--) {
        spiInput.C[i][j] = SPI1.transfer(spiOutput.C[x][j]);
     }
    }
  }
	digitalWrite(spiInPin, LOW);		  // disable shifting
  digitalWrite(spiOutPin, LOW);     // high edge latches data
  digitalWrite(spi1InPin, LOW);
  digitalWrite(spi1OutPin, LOW);
 
	SPI.endTransaction();
  SPI1.endTransaction();

	for (i = 0; i < NUM_CHANNELS; i++) {
		for (j = 0; j < NUM_REGISTERS; j++) {
    
			uint8_t tempData = spiInput.C[i][j];
#ifdef INVERT_SPI_DATA
			tempData ^= 0xff;
#endif
      y = j * 8;
			for (x = 0; x < 8; x++) {
				keyBit.C[i][y++] = tempData & 0b00000001;
				tempData >>= 1;
			}
		}

		for (int key = 0; key < 64; key++) {
			uint8_t prevData = keyTable.C[i][key];
			bool newKey = keyBit.C[i][key];   // is keyDown now?
			bool keyDown = (prevData & 0x80); // was keyDown true on previous scan?
			prevData <<= 1;						        // shift the data left one bit
			if (newKey) prevData++;		        // set newest bit
			prevData &= 0x0f;					        // strip any high bits

			if (!keyDown) { // if note is off in keyTable see if it should be on
				if (prevData == 0b00000011) {	  // if 3 consecutive keyDown
				//if (prevData > 0) {           // this is a fast noteOn for testing
					keyDown = true;				        // turn key on in keyTable
					usbMIDI.sendNoteOn(key + 0x24, M_VELOCITY_ON, i + 1);
					keyOn.C[i][key] = true;
				}
			}
			else if (keyDown) {	// if note is on in keyTable see if it should be off
				if (prevData == 0b00000000) {	  // if 3 consecutive keyUp
					keyDown = false;			        // turn key off in keyTable
					usbMIDI.sendNoteOff(key + 0x24, M_VELOCITY_OFF, i + 1);
					keyOn.C[i][key] = false;
				}
			}
			if (keyDown) prevData |= 0x80;		// store the data
			keyTable.C[i][key] = prevData;
		}
	}

  i = keyScanCount;
	newPotValue[i] = analogRead(i + analogPin1);  // sequential pin #'s, nice
  newPotValue[i] >>= 3;
	newPotValue[i] = (newPotValue[i] + oldPotValue[i]) >> 1; // average old and new
	int diff = newPotValue[i] - oldPotValue[i];              // different value?
  if (diff != 0) {
		usbMIDI.sendControlChange(M_CC_VOLUME, newPotValue[i], i + 1, 0);
	}
	oldPotValue[i] = newPotValue[i];

  if (pendingOn == true) {        // 50ms delay to accumulate additional stops
    if (sincePending >= 50) {
      pendingOn = false;
      setSAMsTime = true;
      //sincePulse = 0;
      //pulseOn = true;
    }
  }

#ifdef USE_ALLEN_CAPTURE_SUPPLY
  if ((setSAMsTime) && (!SAMsPowerOn)) {
    if (pwr1_N1_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr1, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr1_N1_SAMsPending = false;
    } else if (pwr1_N2_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr2, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr1_N2_SAMsPending = false;
    } else if (pwr1_N3_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr3, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr1_N3_SAMsPending = false;
    } else if (pwr1_N4_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr4, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr1_N4_SAMsPending = false;
    } else if (pwr2_N1_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr5, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr2_N1_SAMsPending = false;
    } else if (pwr2_N2_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr6, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr2_N2_SAMsPending = false;
    } else if (pwr2_N3_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr7, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr2_N3_SAMsPending = false;
    } else if (pwr2_N4_SAMsPending) {
      SAMsPowerOn = true;   // Turn on power flag
      digitalWrite(pwr8, HIGH);
      sincePulse = 0;
      pulseOn = true;
      pwr2_N4_SAMsPending = false;
    } else {
      setSAMsTime = false;
    }
  }

  if (pulseOn == true) {          // 50ms output pulse
    if (sincePulse >= 50) {
      pulseOn = false;
      digitalWrite(pwr1, LOW);
      digitalWrite(pwr2, LOW);
      digitalWrite(pwr3, LOW);
      digitalWrite(pwr4, LOW);
      digitalWrite(pwr5, LOW);
      digitalWrite(pwr6, LOW);
      digitalWrite(pwr7, LOW);
      digitalWrite(pwr8, LOW);
      SAMsPowerOn = false;
    }
  }
#endif
  
	usbMIDI.read();							// prime USB to receive any MIDI packet
}

void myNoteOff(byte ch, byte note, byte velocity) {
  ch--;                       // convert to software numbering
  if ((ch >= channel) && (note < 100)) {
    int stopNum = note - 0x24;
    if ((ch > NUM_CHANNELS) || (ch < 0)) ch = 0;    // sanity check
    bool stopEngaged = stopTable[ch][stopNum].engaged;
    if (stopEngaged) {
      stopTable[ch][stopNum].engaged = false;
      switch (stopTable[ch][stopNum].PowerType) {
        case UNUSED_PWR:
          break;
        case PWR1_N1:
          pwr1_N1_SAMsPending = true;
          break;
        case PWR1_N2:
          pwr1_N2_SAMsPending = true;
          break;
        case PWR1_N3:
          pwr1_N3_SAMsPending = true;
          break;
        case PWR1_N4:
          pwr1_N4_SAMsPending = true;
          break;
        case PWR2_N1:
          pwr2_N1_SAMsPending = true;
          break;
        case PWR2_N2:
          pwr2_N2_SAMsPending = true;
          break;
        case PWR2_N3:
          pwr2_N3_SAMsPending = true;
          break;
        case PWR2_N4:
          pwr2_N4_SAMsPending = true;
          break;
        default:
          break;
      }
      ch++;                   // convert back to human numbering
      setSpiOutput(ch, stopNum, false);
      sincePending = 0;
      pendingOn = true;
    }
  }
}

void myNoteOn(byte ch, byte note, byte velocity) {
  ch--;                       // convert to software numbering
  if ((ch >= channel) && (note < 100)) {
    int stopNum = note - 0x24;
    if ((ch > NUM_CHANNELS) || (ch < 0)) ch = 0;    // sanity check
    bool stopEngaged = stopTable[ch][stopNum].engaged;
    if (!stopEngaged) {
      stopTable[ch][stopNum].engaged = true;
      switch (stopTable[ch][stopNum].PowerType) {
        case UNUSED_PWR:
          break;
        case PWR1_N1:
          pwr1_N1_SAMsPending = true;
          break;
        case PWR1_N2:
          pwr1_N2_SAMsPending = true;
          break;
        case PWR1_N3:
          pwr1_N3_SAMsPending = true;
          break;
        case PWR1_N4:
          pwr1_N4_SAMsPending = true;
          break;
        case PWR2_N1:
          pwr2_N1_SAMsPending = true;
          break;
        case PWR2_N2:
          pwr2_N2_SAMsPending = true;
          break;
        case PWR2_N3:
          pwr2_N3_SAMsPending = true;
          break;
        case PWR2_N4:
          pwr2_N4_SAMsPending = true;
          break;
        default:
          break;
      }
      ch++;                   // convert back to human numbering
      setSpiOutput(ch, stopNum, true);
      sincePending = 0;
      pendingOn = true;
    }
  }
}

/* see Capture_System code for pulsing SAMs */
void setSpiOutput(int ch, int stopNum, bool turnOn) {
  ch--;                       // convert to software numbering
  stopOn.C[ch][stopNum] = turnOn;            // if turnOn true then turn on
//  stop +=32;                            // FIX for a single 32-pin output board
  int spiRegister = stopNum / 8;
  int spiBit = stopNum % 8;
  if (turnOn) {
    spiOutput.C[ch][spiRegister] &= ~(1 << spiBit);  // clear bit for this note (stop)
  } else {
    spiOutput.C[ch][spiRegister] |= (1 << spiBit);  // set bit for this note (stop)
  }
}

#ifndef USE_ALLEN_CAPTURE_SUPPLY
// Sysex messages require HW output, see General Settings->General->Advanced settings
void mySystemExclusiveChunk(const byte *data, uint16_t length, bool last) {
  int i;
  for (i = sx; i < length; i++) {
    sysex[sx++] = data[i];
  }
  if (sx == 6) {
    switch (sysex[3]) {
    case 33:            // IsSetterModeOn
              // TODO: Turn on a yellow LED
      break;
    case 38:            // IsOrganReady
              // Match LED states to relays
      if (sysex[4] == 1) {
        digitalWrite(relay2, HIGH);
        digitalWrite(audioLamp, HIGH);
      }
      else if (sysex[4] == 0) {
        digitalWrite(relay2, LOW);
        digitalWrite(audioLamp, LOW);
      }
      break;
    case 39:            // IsInErrorState
              // TODO: Turn on a red LED
      break;
    case 47:      // Master Crescendo Pedal
      digitalWrite(cresc1, LOW);
      digitalWrite(cresc2, LOW);
      digitalWrite(cresc3, LOW);
      digitalWrite(cresc4, LOW);
      if (sysex[4] > 0) {
        digitalWrite(cresc1, HIGH);
      }
      if (sysex[4] > 8) {
        digitalWrite(cresc2, HIGH);
      }
      if (sysex[4] > 16) {
        digitalWrite(cresc3, HIGH);
      }
      if (sysex[4] > 24) {
        digitalWrite(cresc4, HIGH);
      }
      break;
    case 84:            // IsOrganLoaded
              // Possible substitute for IsOrganReady
      break;
    case 85:            // IsOrganLoading
              // TODO: Turn on a yellow LED
      break;
    }
  }
  
  if (last) {
    sx = 0;
    for (i = 0; i < 64; i++)
      sysex[i] = 0;
  }
}
#endif
