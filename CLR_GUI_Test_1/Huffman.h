#include <string>
#include <queue>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <bitset>

#pragma once

class Huffman
{
public:
	struct MinHeapNode
	{
		unsigned char symbol;
		int freq;
		bool endPoint;
		MinHeapNode *left, *right; // Left and right child

		MinHeapNode(unsigned char _symbol = '0', unsigned _freq = 0, bool _endPoint = false)
		{
			left = right = NULL;
			this->symbol = _symbol;
			this->freq = _freq;
			this->endPoint = _endPoint;
		};
	};

	Huffman();
	~Huffman();

	void AnalyseData(std::string filePath);
	void BuildTree();
	void SaveTreeToFile(std::string outputPath);
	void CompressFile(std::string filePath, std::string outputPath);
	void DecompressFile(std::string filePath, std::string outputPath);
	void LoadTreeFromFile(std::string filePath);

	MinHeapNode* getHuffmanTree();
	int getFileLength();
	int* getAnalysedData();
	std::string* getCodeLookup();
	double estimateCompression();

private:
	void PopulateTree(struct MinHeapNode* root, std::string str);
	void EncodeTree(MinHeapNode* node, std::ofstream* stream, int dir = -1);
	MinHeapNode* FindNodeFromPath(Huffman::MinHeapNode* node, std::string path);

	std::string codeLookup[256];
	int analysedData[256];
	MinHeapNode* huffmanTree;
	int fileLength = 0;

	static struct compare
	{
		bool operator()(MinHeapNode* l, MinHeapNode* r)
		{
			return (l->freq > r->freq);
		}
	};


};

