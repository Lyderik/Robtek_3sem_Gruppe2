#pragma once
#include <iostream>

#define POLYNOMIAL 0x9b; //Generator Polynomium

typedef unsigned char crc; //St�rrelsen p� CRC

#define WIDTH  (8 * sizeof(crc)) //Datast�rrelsen
#define TOPBIT (1 << (WIDTH - 1)) 

class CRC
{
public:
	static crc crcSlow(unsigned char message[], int);
};


