// Name:       User.h for MIDI_Encoder.ino
// Created:    12/1/2018
// Author:     John Kinkennon

#include <arduino.h>

#pragma once

//#define USE_ALLEN_CAPTURE_SUPPLY    // comment out if not using
#define INVERT_SPI_DATA             // comment out for 12V keying

#define NUM_CHANNELS        8       // max number of input boards
#define NUM_CH_PER_PORT     (NUM_CHANNELS / 2)
#define NUM_KEYS			      64      // organs typically use 61 keys
#define NUM_SWITCHES		    (NUM_CHANNELS * NUM_KEYS)
#define NUM_REGISTERS		    8				// number of 8-bit SPI chips
#define NUM_REGISTERS_TOTAL	(NUM_CHANNELS * NUM_REGISTERS)
#define NUM_POTS			      8       // analog inputs X5 thru X8 (2 per connector)
#define FIRST_STOP_CH       4       // first stop channel starting with 0
#define NUM_STOP_CHANNELS   3
#define STOPS_PER_CHANNEL   64      // lighted or rocker stops may be 32 per channel

// typedefs that allow accessing tables by channel or by offset

typedef union {
	uint8_t C[NUM_CHANNELS][NUM_KEYS];		// channel access
	uint8_t O[NUM_SWITCHES];              // offset access
} keyTable8_t;

typedef union {
	bool C[NUM_CHANNELS][NUM_KEYS];			  // channel access
	bool O[NUM_SWITCHES];                 // offset access
} keyTableB_t;

typedef union {
	uint8_t C[NUM_CHANNELS][NUM_REGISTERS];	// channel access
	uint8_t O[NUM_REGISTERS_TOTAL];			  // offset access
} spiTable_t;

typedef enum {
  KEYBD_CH,
  PEDAL_CH,
  PISTONS_CH,
  STOPS_CH,
  UNUSED_CH
} midiChannelType;

typedef enum {
  PEDAL_STOP,     // for future use...
  GREAT_STOP,
  SWELL_STOP,
  CHOIR_STOP,
  SOLO_STOP,
  COUPLER_STOP,
  LIGHTED_PISTON,
  UNUSED_STOP
} stopType;

typedef enum {
  PWR1_N1,    // First capture supply
  PWR1_N2,
  PWR1_N3,
  PWR1_N4,
  PWR2_N1,    // Second capture supply
  PWR2_N2,
  PWR2_N3,
  PWR2_N4,
  UNUSED_PWR
} powerType;

typedef struct {
  stopType StopType;
  powerType PowerType;
  bool engaged;
} consoleStop;

midiChannelType channelTable[NUM_CHANNELS] {
  STOPS_CH,     // MIDI channel 1
  STOPS_CH,     // 2
  STOPS_CH,     // 3
  UNUSED_CH,    // 4
  UNUSED_CH,    // 5
  UNUSED_CH     // 6
};

consoleStop stopTable[NUM_STOP_CHANNELS][STOPS_PER_CHANNEL] {
  {
    {PEDAL_STOP, PWR1_N1, false},    // 1
    {PEDAL_STOP, PWR1_N1, false},    // 2
    {PEDAL_STOP, PWR1_N1, false},    // 3
    {PEDAL_STOP, PWR1_N1, false},    // 4
    {PEDAL_STOP, PWR1_N1, false},    // 5
    {PEDAL_STOP, PWR1_N1, false},    // 6
    {PEDAL_STOP, PWR1_N1, false},    // 7
    {PEDAL_STOP, PWR1_N1, false},    // 8
    {PEDAL_STOP, PWR1_N1, false},    // 9
    {PEDAL_STOP, PWR1_N1, false},    // 10
    {PEDAL_STOP, PWR1_N1, false},    // 11
    {PEDAL_STOP, PWR1_N1, false},    // 12
    {PEDAL_STOP, PWR1_N1, false},    // 13
    {PEDAL_STOP, PWR1_N1, false},    // 14
    {PEDAL_STOP, PWR1_N1, false},    // 15
    {PEDAL_STOP, PWR1_N1, false},     // 16
    {GREAT_STOP, PWR1_N2, false},    // 17
    {GREAT_STOP, PWR1_N2, false},     // 18
    {GREAT_STOP, PWR1_N2, false},     // 19
    {GREAT_STOP, PWR1_N2, false},     // 20
    {GREAT_STOP, PWR1_N2, false},     // 21
    {GREAT_STOP, PWR1_N2, false},     // 22
    {GREAT_STOP, PWR1_N2, false},     // 23
    {GREAT_STOP, PWR1_N2, false},     // 24
    {GREAT_STOP, PWR1_N2, false},     // 25
    {GREAT_STOP, PWR1_N2, false},     // 26
    {GREAT_STOP, PWR1_N2, false},     // 27
    {GREAT_STOP, PWR1_N2, false},     // 28
    {GREAT_STOP, PWR1_N2, false},     // 29
    {GREAT_STOP, PWR1_N2, false},     // 30
    {GREAT_STOP, PWR1_N2, false},     // 31
    {GREAT_STOP, PWR1_N2, false},     // 32
    {GREAT_STOP, PWR1_N2, false},     // 33
    {GREAT_STOP, PWR1_N2, false},     // 34
    {GREAT_STOP, PWR1_N2, false},     // 35
    {GREAT_STOP, PWR1_N2, false},     // 36
    {GREAT_STOP, PWR1_N2, false},     // 37
    {GREAT_STOP, PWR1_N2, false},     // 38
    {GREAT_STOP, PWR1_N2, false},     // 39
    {GREAT_STOP, PWR1_N2, false},     // 40
    {GREAT_STOP, PWR1_N2, false},     // 41
    {GREAT_STOP, PWR1_N2, false},     // 42
    {GREAT_STOP, PWR1_N2, false},     // 43
    {GREAT_STOP, PWR1_N2, false},     // 44
    {GREAT_STOP, PWR1_N2, false},     // 45
    {GREAT_STOP, PWR1_N2, false},     // 46
    {GREAT_STOP, PWR1_N2, false},     // 47
    {GREAT_STOP, PWR1_N2, false},     // 48
    {GREAT_STOP, PWR1_N2, false},     // 49
    {GREAT_STOP, PWR1_N2, false},     // 50
    {GREAT_STOP, PWR1_N2, false},     // 51
    {SWELL_STOP, PWR1_N3, false},     // 52
    {SWELL_STOP, PWR1_N3, false},     // 53
    {SWELL_STOP, PWR1_N3, false},     // 54
    {SWELL_STOP, PWR1_N3, false},     // 55
    {SWELL_STOP, PWR1_N3, false},     // 56
    {SWELL_STOP, PWR1_N3, false},     // 57
    {SWELL_STOP, PWR1_N3, false},     // 58
    {SWELL_STOP, PWR1_N3, false},     // 59
    {SWELL_STOP, PWR1_N3, false},     // 60
    {SWELL_STOP, PWR1_N3, false},     // 61
    {SWELL_STOP, PWR1_N3, false},     // 62
    {SWELL_STOP, PWR1_N3, false},     // 63
    {SWELL_STOP, PWR1_N3, false},     // 64
  },
  {
    {SWELL_STOP, PWR1_N3, false},     // 1
    {SWELL_STOP, PWR1_N3, false},     // 2
    {SWELL_STOP, PWR1_N3, false},     // 3
    {SWELL_STOP, PWR1_N3, false},     // 4
    {SWELL_STOP, PWR1_N3, false},     // 5
    {SWELL_STOP, PWR1_N3, false},     // 6
    {SWELL_STOP, PWR1_N3, false},     // 7
    {SWELL_STOP, PWR1_N3, false},     // 8
    {SWELL_STOP, PWR1_N3, false},     // 9
    {SWELL_STOP, PWR1_N3, false},     // 10
    {SWELL_STOP, PWR1_N3, false},     // 11
    {SWELL_STOP, PWR1_N3, false},     // 12
    {SWELL_STOP, PWR1_N3, false},     // 13
    {SWELL_STOP, PWR1_N3, false},     // 14
    {SWELL_STOP, PWR1_N3, false},     // 15
    {SWELL_STOP, PWR1_N3, false},     // 16
    {SWELL_STOP, PWR1_N3, false},     // 17
    {SWELL_STOP, PWR1_N3, false},     // 18
    {SWELL_STOP, PWR1_N3, false},     // 19
    {SWELL_STOP, PWR1_N3, false},     // 20
    {SWELL_STOP, PWR1_N3, false},     // 21
    {SWELL_STOP, PWR1_N3, false},     // 22
    {SWELL_STOP, PWR1_N3, false},     // 23
    {SWELL_STOP, PWR1_N3, false},     // 24
    {SWELL_STOP, PWR1_N3, false},     // 25
    {SWELL_STOP, PWR1_N3, false},     // 26
    {SWELL_STOP, PWR1_N3, false},     // 27
    {SWELL_STOP, PWR1_N3, false},     // 28
    {SWELL_STOP, PWR1_N3, false},     // 29
    {SWELL_STOP, PWR1_N3, false},     // 30
    {SWELL_STOP, PWR1_N3, false},     // 31
    {SWELL_STOP, PWR1_N3, false},     // 32
    {SWELL_STOP, PWR1_N3, false},     // 33
    {SWELL_STOP, PWR1_N3, false},     // 34
    {SWELL_STOP, PWR1_N3, false},     // 35
    {SWELL_STOP, PWR1_N3, false},     // 36
    {SWELL_STOP, PWR1_N3, false},     // 37
    {SWELL_STOP, PWR1_N3, false},     // 38
    {SWELL_STOP, PWR1_N3, false},     // 39
    {SWELL_STOP, PWR1_N3, false},     // 40
    {SWELL_STOP, PWR1_N3, false},     // 41
    {SWELL_STOP, PWR1_N3, false},     // 42
    {CHOIR_STOP, PWR1_N4, false},     // 43
    {CHOIR_STOP, PWR1_N4, false},     // 44
    {CHOIR_STOP, PWR1_N4, false},     // 45
    {CHOIR_STOP, PWR1_N4, false},     // 46
    {CHOIR_STOP, PWR1_N4, false},     // 47
    {CHOIR_STOP, PWR1_N4, false},     // 48
    {CHOIR_STOP, PWR1_N4, false},     // 49
    {CHOIR_STOP, PWR1_N4, false},     // 50
    {CHOIR_STOP, PWR1_N4, false},     // 51
    {CHOIR_STOP, PWR1_N4, false},     // 52
    {CHOIR_STOP, PWR1_N4, false},     // 53
    {CHOIR_STOP, PWR1_N4, false},     // 54
    {CHOIR_STOP, PWR1_N4, false},     // 55
    {CHOIR_STOP, PWR1_N4, false},     // 56
    {CHOIR_STOP, PWR1_N4, false},     // 57
    {CHOIR_STOP, PWR1_N4, false},     // 58
    {CHOIR_STOP, PWR1_N4, false},     // 59
    {CHOIR_STOP, PWR1_N4, false},     // 60
    {CHOIR_STOP, PWR1_N4, false},     // 61
    {CHOIR_STOP, PWR1_N4, false},     // 62
    {CHOIR_STOP, PWR1_N4, false},     // 63
    {CHOIR_STOP, PWR1_N4, false},     // 64
  },
  {
    {CHOIR_STOP, PWR1_N4, false},     // 1
    {CHOIR_STOP, PWR1_N4, false},     // 2
    {CHOIR_STOP, PWR1_N4, false},     // 3
    {CHOIR_STOP, PWR1_N4, false},     // 4
    {CHOIR_STOP, PWR1_N4, false},     // 5
    {SOLO_STOP, PWR2_N1, false},      // 6
    {SOLO_STOP, PWR2_N1, false},      // 7
    {SOLO_STOP, PWR2_N1, false},      // 8
    {SOLO_STOP, PWR2_N1, false},      // 9
    {SOLO_STOP, PWR2_N1, false},      // 10
    {SOLO_STOP, PWR2_N1, false},      // 11
    {SOLO_STOP, PWR2_N1, false},      // 12
    {SOLO_STOP, PWR2_N1, false},      // 13
    {SOLO_STOP, PWR2_N1, false},      // 14
    {SOLO_STOP, PWR2_N1, false},      // 15
    {SOLO_STOP, PWR2_N1, false},      // 16
    {SOLO_STOP, PWR2_N1, false},      // 17
    {SOLO_STOP, PWR2_N1, false},      // 18
    {SOLO_STOP, PWR2_N1, false},      // 19
    {COUPLER_STOP, PWR2_N2, false},   // 20
    {COUPLER_STOP, PWR2_N2, false},   // 21
    {COUPLER_STOP, PWR2_N2, false},   // 22
    {COUPLER_STOP, PWR2_N2, false},   // 23
    {COUPLER_STOP, PWR2_N2, false},   // 24
    {COUPLER_STOP, PWR2_N2, false},   // 25
    {COUPLER_STOP, PWR2_N2, false},   // 26
    {COUPLER_STOP, PWR2_N2, false},   // 27
    {COUPLER_STOP, PWR2_N2, false},   // 28
    {COUPLER_STOP, PWR2_N2, false},   // 29
    {COUPLER_STOP, PWR2_N2, false},   // 30
    {COUPLER_STOP, PWR2_N2, false},   // 31
    {COUPLER_STOP, PWR2_N2, false},   // 32
    {UNUSED_STOP, UNUSED_PWR, false}, // 33
    {UNUSED_STOP, UNUSED_PWR, false}, // 34
    {UNUSED_STOP, UNUSED_PWR, false}, // 35
    {UNUSED_STOP, UNUSED_PWR, false}, // 36
    {UNUSED_STOP, UNUSED_PWR, false}, // 37
    {UNUSED_STOP, UNUSED_PWR, false}, // 38
    {UNUSED_STOP, UNUSED_PWR, false}, // 39
    {UNUSED_STOP, UNUSED_PWR, false}, // 40
    {UNUSED_STOP, UNUSED_PWR, false}, // 41
    {UNUSED_STOP, UNUSED_PWR, false}, // 42
    {UNUSED_STOP, UNUSED_PWR, false}, // 43
    {UNUSED_STOP, UNUSED_PWR, false}, // 44
    {UNUSED_STOP, UNUSED_PWR, false}, // 45
    {UNUSED_STOP, UNUSED_PWR, false}, // 46
    {UNUSED_STOP, UNUSED_PWR, false}, // 47
    {UNUSED_STOP, UNUSED_PWR, false}, // 48
    {UNUSED_STOP, UNUSED_PWR, false}, // 49
    {UNUSED_STOP, UNUSED_PWR, false}, // 50
    {UNUSED_STOP, UNUSED_PWR, false}, // 51
    {UNUSED_STOP, UNUSED_PWR, false}, // 52
    {UNUSED_STOP, UNUSED_PWR, false}, // 53
    {UNUSED_STOP, UNUSED_PWR, false}, // 54
    {UNUSED_STOP, UNUSED_PWR, false}, // 55
    {UNUSED_STOP, UNUSED_PWR, false}, // 56
    {UNUSED_STOP, UNUSED_PWR, false}, // 57
    {UNUSED_STOP, UNUSED_PWR, false}, // 58
    {UNUSED_STOP, UNUSED_PWR, false}, // 59
    {UNUSED_STOP, UNUSED_PWR, false}, // 60
    {UNUSED_STOP, UNUSED_PWR, false}, // 61
    {UNUSED_STOP, UNUSED_PWR, false}, // 62
    {UNUSED_STOP, UNUSED_PWR, false}, // 63
    {UNUSED_STOP, UNUSED_PWR, false}, // 64
  }
};
