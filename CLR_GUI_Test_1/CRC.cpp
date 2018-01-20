#include "CRC.h"


crc CRC::crcSlow(unsigned char message[], int nBytes)
{
	crc  remainder = 0; //S�t registeret til 0.

	for (int byte = 0; byte < nBytes; ++byte) //Gentag for antallet af bytes i beskeden
	{

		remainder ^= (message[byte] << (WIDTH - 8)); //inds�t n�ste byte i remainder-registret

		for (unsigned char bit = 8; bit > 0; --bit) //Foretag modulo-2 divison
		{

			if (remainder & TOPBIT) //Unders�g om division skal foretages
			{
				remainder = (remainder << 1) ^ POLYNOMIAL;
			}
			else
			{
				remainder = (remainder << 1);
			}
		}
	}
	return (remainder); //Udskriv checksum.
}


