#include "FileHandler.h"
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <vector>

FileHandler::FileHandler()
{
}

//INPUT: Add error handling pls
void FileHandler::loadFileFromPath(std::string _fileInPath)
{
	inputStream.open(_fileInPath, std::ios::binary); //Opens input stream
}

char FileHandler::readNextByte()
{
	return inputStream.get(); //Gets next byte from the input stream
}

char * FileHandler::readNextBytes(int length)
{
	char * buffer = new char[length];
	for (int i = 0; i < length; i++) //Reads n bytes and outputs to char buffer
	{
		buffer[i] = readNextByte();
	}
	return buffer;
}

char * FileHandler::readAllBytes() //Reads all bytes
{
	inputStream.seekg(0, inputStream.end);
	int length = inputStream.tellg();
	inputStream.seekg(0, inputStream.beg); //Determines the length in bytes of the file

	std::vector<char> vBuffer;
	while (!inputStream.eof()) //While not at last byte
	{
		vBuffer.push_back(readNextByte()); //Write bytes to buffer
		byteCount++;
		if (byteCount % 1000000 == 0)
		{
			double percentage = ((double)byteCount / (double)length) * 100.0;
			std::cout << byteCount / 1000000 << "/" << length / 1000000 << " " << percentage << "%" << std::endl;
		}
	}

	char * buffer = new char[vBuffer.size()];
	memcpy(buffer, vBuffer.data(), vBuffer.size()); //Converts vector to char buffer

	return buffer;
}

void FileHandler::endReadFile()
{
	inputStream.close(); //Closes input stream 
}

//OUTPUT: Add error handling pls
void FileHandler::startWriteFile(std::string _outputFilePath)
{
	outputStream.open(_outputFilePath, std::ios::binary); //Opens output stream to filepath
}

void FileHandler::writeByteToFile(char _byte)
{
	outputStream.put(_byte); //Puts a byte into the ouput stream
}
void FileHandler::writeAllToFile(char * _buffer)
{
	for (int i = 0; i < byteCount - 1; i++) //Puts all the bytes from the buffer into the output stream
	{
		writeByteToFile(_buffer[i]);
	}
}
void FileHandler::writeAllToFile(char * _buffer, int length)
{
	for (int i = 0; i < length; i++) //Puts n bytes from the buffer into the output stream
	{
		writeByteToFile(_buffer[i]);
	}
}

void FileHandler::endWriteFile()
{
	outputStream.close(); //Closes the output stream
}

FileHandler::~FileHandler()
{
}
