#pragma once
#include <iostream>
#include <vector>
#include "CRC.h"

/* LENGTHS */
#define HEADER_LENGTH 3
#define TRAILER_LENGTH 1
#define DATA_MAX_LENGTH 16
#define FRAME_MAX_LENGHT (HEADER_LENGTH + DATA_MAX_LENGTH + TRAILER_LENGTH)
#define FRAME_MIN_LENGTH (HEADER_LENGTH + TRAILER_LENGTH)

/* FIELDS*/
#define ADDRESS_SOURCE_FIELD 0
#define ADRESS_DEST_FIELD 1
#define CONTROL_FIELD 2

/* ERRORS*/
#define OUT_OF_BOUNDS 0

/**/


typedef unsigned char byte;

struct Frame {
	byte* data;
	int length;

	Frame(int l) {
		data = new byte[l];
		length = l;
	}
	~Frame() {
		delete[] data;
	}
	void resize(int l) { //Likly not needed by design
		delete[] data;
		data = new byte[l];
		length = l;
	}
};
typedef void(*fcb)(Frame*, void*);
class FrameHandler
{
public:
	FrameHandler();
	~FrameHandler();

	bool loadFrame(Frame* frame);
	void clearFrame();

	Frame* generateFrame();

	/* DATA FRAME FLAGS */
	static const int FLAG_EOF = 2;
	//bit 4-7 are data length for data frames widt variable length

	static const int FLAG_NEWADRESS = 3;
	static const int FLAG_HS_ACK = 4;

	/* GENERIC FLAGS */
	static const int FLAG_SEQ = 3;

	/* FRAME TYPES */
	static const int TYPE_HANDSHAKE = 0b000;
	static const int TYPE_FILE = 0b001;
	static const int TYPE_FILE_INFO = 0b010;
	static const int TYPE_REQUEST = 0b011;
	static const int TYPE_ACK = 0b100;

	/* HANDSHAKE TYPES */
	static const int TYPE_HANDSHAKE_ASK = 0b00;
	static const int TYPE_HANDSHAKE_GIVE = 0b01;
	static const int TYPE_HANDSHAKE_THX = 0b10;
	static const int TYPE_HANDSHAKE_ACK = 0b11;

	/* GENERIC FUNCTIONS */
	bool getBit(int field, int pos);
	void setBit(int field, int pos);
	void clearBit(int field, int pos);
	void toggleBit(int field, int pos);

	void setDest(unsigned char address);
	void setSource(unsigned char address);

	byte getDest();
	byte getSource();

	void setControl(int, int, int, int, int, int, int, int);

	bool getFlag(int flag);
	void setFlag(int flag);
	void clearFlag(int flag);

	int getTimeLength();

	/* SPECIFIC FUNKTIONS */

	void setType(int type);
	int  getType();

	void setACKType(int type);
	int  getACKType();

	void setHSType(int type);
	int  getHSType();

	void setHeyAdress(byte adress);
	byte getHeyAdress();

	void setDataSize(int dsize);

	void setHeader(unsigned char*);
	void setData(unsigned char* data, int length);

	/* DATA FUNCTIONS */

	/* INFO FUNCTIONS */

protected:
	int dataLength;
	byte header[HEADER_LENGTH];
	byte trailer[TRAILER_LENGTH];
	byte data[DATA_MAX_LENGTH];
};