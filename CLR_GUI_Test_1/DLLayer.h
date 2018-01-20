#pragma once
#include <iomanip>
#include <chrono>
#include "Frame.h"
#include <Windows.h>

/* STATES */
#define IDLE 0
#define ADRESSING 1
#define SENDING 2
#define RECIECING 3

/* */
#define MAX_ADDRESS 0xFF

#define MAX_RESENDS 10

class DLLayer
{
public:
	DLLayer(byte type);
	~DLLayer();

	static const byte SERVER = 0x00;
	static const byte CLIENT = MAX_ADDRESS;

	static void frameReceivedWrap(Frame* frame, void* dll);
	void sendFrame(bool ackNeeded = false);
	void sendFrameDirect(Frame* frame, bool ackNeeded = false);


	void setFramePusher(fcb framecb, void* flayer);

	void sendAskForAdress(int dest = SERVER);

private:
	/* ONLY SERVER*/
	//std::vector<byte> addresses;
	byte nextAvailableAdress = 0x01;
	int resendCounter = 0;

	/*ONLY CLIENT */
	byte tempaddress;

	/* ALL */
	byte address;
	fcb sendFrameFlayer;
	void* flayer;
	FrameHandler frameReader;
	FrameHandler frameBuilder;

	int state = IDLE;

	int sequence = 0;

	//void sendFrame(Frame* frame);
	void ReceiveFrame(Frame* frame);

	void handleHeyFrame();
	void handleDataSizeFrame();
	void handleDataFrame();
	void handleReqFrame();
	void handleACKFrame();

	void sendThxFrame();

/*	static void ThreadProc(System::Object^ data);*/
	static void ackTimerCallbackWrap(DLLayer* dll, int seqNr);
	void ackTimerCallback(int seqNr);
	int getNextSeq();

	//void sendAdressAck();
};