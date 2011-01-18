#pragma once
/**
 * Generate and send Morse Code on an LED or a speaker. Allow sending
 * in a non-blocking manner (by calling a 'continue sending' method
 * every so often to turn an LED on/off, or to call tone/noTone appropriately).
 */

// for malloc and free, for the new/delete operators
#include <stdlib.h>

// Arduino language types
#include "WProgram.h"

// define lengths
typedef int             morseTiming_t;
#define UNIT            100
#define DIT             UNIT
#define DAH             3*UNIT

// sentinel
#define END             0

// the most timing numbers any unit will need; ex: k = on,off,on,off,on,end = 5
#define MAX_TIMINGS     10

// Morse Code (explicit declaration of letter timings)
const morseTiming_t MORSE[26][5] = {
	/* a */ {DIT, DAH, END},
	/* b */ {DAH, DIT, DIT, DIT, END},
	/* c */ {DAH, DIT, DAH, DIT, END},
	/* d */ {DAH, DIT, DIT, END},
	/* e */ {DIT, END},
	/* f */ {DIT, DIT, DAH, DIT, END},
	/* g */ {DAH, DAH, DIT, END},
	/* h */ {DIT, DIT, DIT, DIT, END},
	/* i */ {DIT, DIT, END},
	/* j */ {DIT, DAH, DAH, DAH, END},
	/* k */ {DAH, DIT, DAH, END},
	/* l */ {DIT, DAH, DIT, DIT, END},
	/* m */ {DAH, DAH, END},
	/* n */ {DAH, DIT, END},
	/* o */ {DAH, DAH, DAH, END},
	/* p */ {DIT, DAH, DAH, DIT, END},
	/* q */ {DAH, DAH, DIT, DAH, END},
	/* r */ {DIT, DAH, DIT, END},
	/* s */ {DIT, DIT, DIT, END},
	/* t */ {DAH, END},
	/* u */ {DIT, DIT, DAH, END},
	/* v */ {DIT, DIT, DIT, DAH},
	/* w */ {DIT, DAH, DAH, END},
	/* x */ {DAH, DIT, DIT, DAH, END},
	/* y */ {DAH, DIT, DAH, DAH, END},
	/* z */ {DAH, DAH, DIT, DIT, END},
};


/**
 * Define the logic of converting characters to on/off timing,
 * and encapsulate the state of one sending-in-progress Morse message.
 *
 * Subclasses define setOn and setOff for (for example) LED and speaker output.
 */
class MorseSender {
protected:
	const unsigned int pin;
	// These would be pure virtual, but that has compiler issues.
	// See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1167672075 .
	virtual void setOn();
	virtual void setOff();

private:
	String message;

	// on,off,...,wait,0 list, millis
	morseTiming_t timingBuffer[MAX_TIMINGS+1];

	// index of the character currently being sent
	unsigned int messageIndex;
	// timing unit currently being sent
	unsigned int timingIndex;

	// when this timing unit was started
	unsigned long lastChangedMillis;
    
	/**
	 * Fill a buffer with on,off,..,END timings (millis)
	 * @return the index at which to start within the new timing sequence
	 */
	unsigned int fillTimings(char c);
    
public:
	/**
	 * Create a sender which will output to the given pin.
	 */
	MorseSender(unsigned int outputPin);

	/**
	 * To be called during the Arduino setup(); set the pin as OUTPUT.
	 */
	void setup();

	/**
	 * Set the message to be sent.
	 * This halts any sending in progress.
	 */
	void setMessage(const String newMessage);

	/**
	 * Send the entirety of the current message before returning.
	 */
	void sendBlocking();

	/**
	 * Prepare to send and begin sending the current message.
	 */
	void startSending();

	/**
	 * Switch outputs on and off (and refill the internal timing buffer)
	 * as necessary to continue with the sending of the current message.
	 * This should be called every few milliseconds (at a significantly
	 * smaller interval than UNIT) to produce a legible fist.
	 *
	 * @see startSending, which must be called first
	 * @return false if sending is complete, otherwise true (keep sending)
	 */
	boolean continueSending();

	void *operator new(size_t size);
	void operator delete(void* ptr);
};


class SpeakerMorseSender: public MorseSender {
  private:
    unsigned int frequency;
  protected:
    virtual void setOn();
    virtual void setOff();
  public:
    // concert A = 440
    // middle C = 261.626; higher octaves = 523.251, 1046.502
    SpeakerMorseSender(int outputPin, unsigned int toneFrequency=1046);
};


class LEDMorseSender: public MorseSender {
  protected:
    virtual void setOn();
    virtual void setOff();
  public:
    LEDMorseSender(int outputPin);
};
