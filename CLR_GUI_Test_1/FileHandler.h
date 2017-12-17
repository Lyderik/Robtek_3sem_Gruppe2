#include <string>
#include <fstream>
#include <vector>
#pragma once

class FileHandler
{
public:
	FileHandler();
	void loadFileFromPath(std::string _fileInPath);
	unsigned char readNextByte();
	unsigned char * readNextBytes(int length);
	char* readAllBytes();
	void endReadFile();

	void startWriteFile(std::string _outputFilePath);
	void writeByteToFile(char _byte);
	void writeAllToFile(char * _buffer);
	void writeAllToFile(char * _buffer, int length);
	void endWriteFile();
	~FileHandler();
private:
	std::ifstream inputStream;
	std::ofstream outputStream;
	int byteCount = 0;
};

