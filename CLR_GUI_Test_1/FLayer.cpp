#include "FLayer.h"
#include <iomanip>

FLayer::FLayer(int ss)
{
	inBuffer = new IBuffer(ss);
	outBuffer = new OBuffer(ss);

	if (paInit.result() == paNoError) {
		if (inBuffer->open(Pa_GetDefaultInputDevice())) {
			if (outBuffer->open(Pa_GetDefaultOutputDevice())) {
				if (!inBuffer->start()) throw - 4;
				if (!outBuffer->start()) throw - 5;
				//throw 0;
			}
			else { throw - 3; }
		}
		else { throw - 2; }
	}
	else { throw - 1; }
}

FLayer::~FLayer()
{
	std::cout << "ended its life\n";
	inBuffer->stop();
	outBuffer->stop();
	inBuffer->close();
	outBuffer->close();
	delete inBuffer;
	delete outBuffer;
}

/*void FLayer::inputDataCallback(std::string ss)
{
std::cout << ss << std::endl;
}

void FLayer::inputDataCallbackWrap(Frame* frame, void* cake)
{
//((FLayer*)cake)->inputDataCallback(ss);
//fHandler.loadFrame(ss);

//RawFrame rframe = frame.getRawFrame();
for (int i = 0; i < frame->length; i++)
{
std::cout << std::hex << std::setw(2) << int(frame->data[i]) << " : ";
}
std::cout << std::endl;
delete frame;
}*/

void FLayer::put(Frame* frame)
{
	//outBuffer->put(-1);
	outBuffer->put(0);
	outBuffer->put(0);
	outBuffer->put(-1);
	outBuffer->put(15);
	outBuffer->put(-1);
	std::cout << "send: ";
	for (int i = 0; i < frame->length; i++)
	{
		std::cout << std::hex << std::setw(2) << int(frame->data[i]) << " : ";
		outBuffer->put(frame->data[i] / 16 % 16);
		outBuffer->put(frame->data[i] % 16);
	}
	std::cout << std::endl;
}
void FLayer::sendFrame(Frame* frame, void* flayer) {
	((FLayer*)flayer)->put(frame);
}

void FLayer::setInputCallback(fcb cb, void* dll)
{
	inBuffer->setCallback(cb, dll);
}
