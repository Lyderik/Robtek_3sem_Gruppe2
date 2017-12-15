#include "Frame.h"

FrameHandler::FrameHandler()
{
	clearFrame();
}

FrameHandler::~FrameHandler()
{
}

bool FrameHandler::getBit(int field, int pos) {
	// int	field	header field 
	// int	pos		posistion in field
	if (field < HEADER_LENGTH && pos < 8)
		return (header[field] && (1 << pos) > 0);
	else
		throw OUT_OF_BOUNDS;
	return false;
}

void FrameHandler::setBit(int field, int pos)
{
	// int	field	header field 
	// int	pos		posistion in field
	if (field < HEADER_LENGTH && pos < 8)
		header[field] |= 1 << pos;
	else
		throw OUT_OF_BOUNDS;
}

void FrameHandler::clearBit(int field, int pos)
{
	// int	field	header field 
	// int	pos		posistion in field
	if (field < HEADER_LENGTH && pos < 8)
		header[field] &= ~(1 << pos);
	else
		throw OUT_OF_BOUNDS;
}

void FrameHandler::toggleBit(int field, int pos)
{
	// int	field	header field 
	// int	pos		posistion in field
	if (field < HEADER_LENGTH && pos < 8)
		header[field] ^= 1 << pos;
	else
		throw OUT_OF_BOUNDS;
}

void FrameHandler::setDest(unsigned char d)
{
	//tager en char som argument, og sætter det i det slot i arrayet der svarer til destination adressen, lig med den char.
	header[ADRESS_DEST_FIELD] = d;
}

void FrameHandler::setSource(unsigned char s)
{
	//tager en char som argument, og sætter det i det slot i arrayet der svarer til source adressen, lig med den char.
	header[ADDRESS_SOURCE_FIELD] = s;
}

byte FrameHandler::getDest()
{
	return header[ADRESS_DEST_FIELD];
}

byte FrameHandler::getSource()
{
	return header[ADDRESS_SOURCE_FIELD];
}

void FrameHandler::setControl(int o, int t, int th, int f, int fi, int s, int se, int e)
{
	//Tager individuelle bits til at sætte hvert enkelt felt i control slottet i arrayet.
	header[CONTROL_FIELD] = 0x00;
	if (o == 1)
	{
		setBit(CONTROL_FIELD, 7);
	}
	if (t == 1)
	{
		setBit(CONTROL_FIELD, 6);
	}
	if (th == 1)
	{
		setBit(CONTROL_FIELD, 5);
	}
	if (f == 1)
	{
		setBit(CONTROL_FIELD, 4);
	}
	if (fi == 1)
	{
		setBit(CONTROL_FIELD, 3);
	}
	if (s == 1)
	{
		setBit(CONTROL_FIELD, 2);
	}
	if (se == 1)
	{
		setBit(CONTROL_FIELD, 1);
	}
	if (e == 1)
	{
		setBit(CONTROL_FIELD, 0);
	}
}


bool FrameHandler::getFlag(int flag)
{
	return getBit(CONTROL_FIELD, flag);
}

void FrameHandler::setFlag(int flag)
{
	setBit(CONTROL_FIELD, flag);
}

void FrameHandler::clearFlag(int flag)
{
	clearBit(CONTROL_FIELD, flag);
}
void FrameHandler::setType(int type)
{
	if (type >= 0b00 && type <= 0b100) {
		if (type % 2 == 1) setFlag(0); else clearFlag(0);
		if ((type >> 1) % 2 == 1) setFlag(1); else clearFlag(1);
		//if (type & 0b11 == 0b00)
		if ((type >> 2) % 2 == 1) setFlag(2); else clearFlag(2);
	}
}
int FrameHandler::getType()
{
	if ((header[CONTROL_FIELD] & 0b11) == 0b00)
		return header[CONTROL_FIELD] & 0b111;
	else
		return header[CONTROL_FIELD] & 0b11;
}

void FrameHandler::setACKType(int type)
{
	if (type >= 0b00 && type <= 0b100) {
		if (type % 2 == 1) setFlag(4); else clearFlag(4);
		if ((type >> 1) % 2 == 1) setFlag(5); else clearFlag(5);
	}
}

int FrameHandler::getACKType()
{
	return (header[CONTROL_FIELD] & 0b110000) >> 4;
}

void FrameHandler::setHSType(int type)
{
	if (type >= 0b00 && type <= 0b11) {
		if (type % 2 == 1) setFlag(3); else clearFlag(3);
		if ((type >> 1) % 2 == 1) setFlag(4); else clearFlag(4);
	}
}

int FrameHandler::getHSType()
{
	return (header[CONTROL_FIELD] & 0b11000) >> 3;
}

void FrameHandler::setHeyAdress(byte adress)
{
	setFlag(FLAG_NEWADRESS);
	data[0] = adress;
	dataLength = 1;
}

byte FrameHandler::getHeyAdress()
{
	return data[0];
}

int FrameHandler::getTimeLength()
{
	return 2 * (FRAME_MAX_LENGHT + FRAME_MIN_LENGTH + dataLength + 18) * (1024 * 1000 / 44100);
}

void FrameHandler::setDataSize(int dsize) {
	int d = dsize - 1;
	if (d % 2 == 1) setFlag(4); else clearFlag(4);
	if ((d >> 1) % 2 == 1) setFlag(5); else clearFlag(5);
	if ((d >> 2) % 2 == 1) setFlag(6); else clearFlag(6);
	if ((d >> 3) % 2 == 1) setFlag(7); else clearFlag(7);
}

void FrameHandler::setHeader(unsigned char * h)
{
	for (int i = 0; i < HEADER_LENGTH; i++)
	{
		header[i] = h[i];
	}
}

void FrameHandler::setData(unsigned char * d, int length)
{
	if (length > 0 && length <= DATA_MAX_LENGTH) {
		setDataSize(length);
		dataLength = length;
		for (int i = 0; i < length; i++)
			data[i] = d[i];
	}
}

Frame* FrameHandler::generateFrame()
{
	int size = HEADER_LENGTH + TRAILER_LENGTH + dataLength;
	Frame* frame = new Frame(size);
	int p = 0;
	for (int i = 0; i < HEADER_LENGTH; i++) {
		frame->data[p++] = header[i];
	}
	for (int i = 0; i < dataLength; i++) {
		frame->data[p++] = data[i];
	}
	frame->data[p] = CRC::crcSlow(frame->data, frame->length - 1);

	return frame;
}



bool FrameHandler::loadFrame(Frame* frame)
{
	int length = frame->length;
	dataLength = length - FRAME_MIN_LENGTH;
	if (length >= FRAME_MIN_LENGTH && length <= FRAME_MAX_LENGHT) {
		int p = 0;
		for (int i = 0; i < HEADER_LENGTH; i++)
			header[i] = frame->data[p++];

		for (int i = 0; i < dataLength; i++)
			data[i] = frame->data[p++];

		trailer[0] = frame->data[p];
		if (CRC::crcSlow(frame->data, length) == 0)
			return true;
	}
	return false;
}

void FrameHandler::clearFrame()
{
	for (int i = 0; i < HEADER_LENGTH; i++) {
		header[i] = 0x00;
	}
	for (int i = 0; i < TRAILER_LENGTH; i++) {
		trailer[i] = 0x00;
	}
	for (int i = 0; i < DATA_MAX_LENGTH; i++)
	{
		data[i] = 0x00;
	}
	dataLength = 0;
}
