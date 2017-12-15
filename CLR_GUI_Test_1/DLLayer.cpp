#include "DLLayer.h"
using namespace System;
using namespace System::Threading;

DLLayer::DLLayer(byte type)
{
	address = type;
	tempaddress = type;
	if (address == CLIENT) {
		//sendAskForAdress();
	}
}

DLLayer::~DLLayer()
{
}

void DLLayer::frameReceivedWrap(Frame * frame, void * dll)
{
	((DLLayer*)dll)->ReceiveFrame(frame);
}

void DLLayer::ReceiveFrame(Frame * frame)
{
	std::cout << "got : ";
	for (int i = 0; i < frame->length; i++)
	{
		std::cout << std::hex << std::setw(2) << int(frame->data[i]) << " : ";
	}
	std::cout << std::endl;

	if (frameReader.loadFrame(frame)) {
		if (frameReader.getDest() == address || frameReader.getDest() == tempaddress) {
			switch (frameReader.getType())
			{
			case FrameHandler::TYPE_FILE:
				std::cout << "Received File Frame\n";
				handleDataFrame();
				break;
			case FrameHandler::TYPE_FILE_INFO:
				std::cout << "Received File Info Frame\n";
				handleDataSizeFrame();
				break;
			case FrameHandler::TYPE_HANDSHAKE:
				handleHeyFrame();
				break;
			case FrameHandler::TYPE_REQUEST:
				std::cout << "Received Request Frame\n";
				handleReqFrame();
				break;
			case FrameHandler::TYPE_ACK:
				std::cout << "Received Acknowledge Frame\n";
				handleACKFrame();
				break;
			}
		}
	}
	else std::cout << "Received invalid Frame\n";


	delete frame;
}


ref class CallBack
{
public:
	CallBack(int s, DLLayer* dll)
	{
		sequence = s;
		DLL = dll;
	}
	int sequence;
	DLLayer* DLL;
};

void DLLayer::ThreadProc(Object^ data)
{
	CallBack^ cB = (CallBack^)data;
	ackTimerCallbackWrap(cB->DLL, cB->sequence);
}

void DLLayer::sendFrame(bool ackNeeded)
{
	Frame* frame = frameBuilder.generateFrame();
	sendFrameFlayer(frame, flayer);
	if (ackNeeded) {
		int time = frameBuilder.getTimeLength();
		
		CallBack^ c = gcnew CallBack(sequence, this);

		Thread^ oThread = gcnew Thread(gcnew ParameterizedThreadStart(&ThreadProc));
		oThread->Sleep(time);
		oThread->Start(c);
	}
}

void DLLayer::sendFrameDirect(Frame * frame, bool ackNeeded)
{
	frameBuilder.loadFrame(frame);
	frameBuilder.setSource(address);
	sendFrame(ackNeeded);
}

void DLLayer::setFramePusher(fcb framecb, void * fl)
{
	sendFrameFlayer = framecb;
	flayer = fl;
}

/* HANDSHAKE HANDLING */

void DLLayer::handleHeyFrame()
{
	switch (frameReader.getHSType())
	{
	case FrameHandler::TYPE_HANDSHAKE_ASK:
		std::cout << "Received Handshake Ask Frame\n";
		if (nextAvailableAdress < MAX_ADDRESS) {
			frameBuilder.clearFrame();

			frameBuilder.setSource(address);
			frameBuilder.setDest(frameReader.getSource());

			frameBuilder.setType(FrameHandler::TYPE_HANDSHAKE);
			frameBuilder.setHSType(FrameHandler::TYPE_HANDSHAKE_GIVE);
			frameBuilder.setHeyAdress(nextAvailableAdress);

			std::cout << "Send Handshake Give Frame\n";
			//Frame* f = frameBuilder.getFrame();
			sendFrame();
		}
		break;

	case FrameHandler::TYPE_HANDSHAKE_GIVE:
		std::cout << "Received Handshake Give Frame\n";
		tempaddress = frameReader.getHeyAdress();

		frameBuilder.clearFrame();

		frameBuilder.setSource(tempaddress);
		frameBuilder.setDest(frameReader.getSource());

		frameBuilder.setType(FrameHandler::TYPE_HANDSHAKE);
		frameBuilder.setHSType(FrameHandler::TYPE_HANDSHAKE_THX);

		getNextSeq();
		std::cout << "Send Handshake Thx Frame\n";
		sendFrame(true);
		break;

	case FrameHandler::TYPE_HANDSHAKE_THX:
		std::cout << "Received Handshake Thx Frame\n";

		frameBuilder.clearFrame();
		frameBuilder.setSource(address);
		frameBuilder.setDest(frameReader.getSource());

		frameBuilder.setType(FrameHandler::TYPE_HANDSHAKE);
		frameBuilder.setHSType(FrameHandler::TYPE_HANDSHAKE_ACK);

		if (frameReader.getSource() == nextAvailableAdress) {
			nextAvailableAdress++;
		}
		std::cout << "Send Handshake ACK Frame\n";

		sendFrame();
		break;

	case FrameHandler::TYPE_HANDSHAKE_ACK:
		std::cout << "Received Handshake ACK Frame\n";
		address = tempaddress;
		getNextSeq();
		break;
	}
}

void DLLayer::handleDataSizeFrame()
{
}

void DLLayer::handleDataFrame()
{
	frameBuilder.clearFrame();

	frameBuilder.setSource(address);
	frameBuilder.setDest(frameReader.getSource());

	frameBuilder.setType(FrameHandler::TYPE_ACK);

	sendFrame();
}

void DLLayer::handleReqFrame()
{
}

void DLLayer::handleACKFrame()
{
	getNextSeq();
}

void DLLayer::ackTimerCallbackWrap(DLLayer* dll, int seqNr)
{
	dll->ackTimerCallback(seqNr);
}

void DLLayer::ackTimerCallback(int seqNr)
{
	if (seqNr == sequence && resendCounter < MAX_RESENDS) {
		resendCounter++;
		sendFrame(true);
	}
	else {
		resendCounter = 0;
	};
}

int DLLayer::getNextSeq()
{
	sequence = !sequence;
	return sequence;
}


void DLLayer::sendAskForAdress(int dest)
{
	frameBuilder.clearFrame();
	frameBuilder.setSource(address);
	frameBuilder.setDest(dest);
	frameBuilder.setType(FrameHandler::TYPE_HANDSHAKE);
	frameBuilder.setHSType(FrameHandler::TYPE_HANDSHAKE_ASK);

	std::cout << "Send Handshake Ask Frame\n";

	sendFrame(true);
}

