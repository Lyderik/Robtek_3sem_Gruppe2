#pragma once
#include <string>
#include "ScopedPaHandler.h"

#include "IBuffer.h"
#include "OBuffer.h"

#include <iostream>



class FLayer
{
public:
	FLayer(int ss = 2048);
	~FLayer();

	void put(Frame* rframe);
	static void sendFrame(Frame * frame, void *);

	void setInputCallback(fcb, void*);

private:
	//void (*inputFrameCallback)(std::string);
	ScopedPaHandler paInit;

	IBuffer *inBuffer;
	OBuffer *outBuffer;

	FrameHandler fHandler;
};

