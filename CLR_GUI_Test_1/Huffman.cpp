#include "Huffman.h"

Huffman::Huffman()
{
	for (int i = 0; i < 256; i++)
	{
		analysedData[i] = 0;
	}
}

Huffman::~Huffman()
{
}

Huffman::MinHeapNode* Huffman::getHuffmanTree()
{
	return huffmanTree;
}

int Huffman::getFileLength()
{
	return fileLength;
}

int * Huffman::getAnalysedData()
{
	return analysedData;
}

std::string * Huffman::getCodeLookup()
{
	return codeLookup;
}

double Huffman::estimateCompression()
{
	int totalSize = 0;
	for (int i = 0; i < 256; i++)
	{
		totalSize += codeLookup[i].length() * analysedData[i];
	}

	double percentage = ((double)totalSize / 8.0) / (double)fileLength * 100.0;

	return percentage;
}

void Huffman::AnalyseData(std::string filePath)
{
	std::ifstream stream(filePath, std::ios::binary);

	while (stream.good())
	{
		analysedData[unsigned char(stream.get())]++;
		fileLength++;
	}
	stream.close();
}

void Huffman::BuildTree()
{
	struct Huffman::MinHeapNode *left, *right, *top;

	std::priority_queue<Huffman::MinHeapNode*, std::vector<Huffman::MinHeapNode*>, compare> minHeap; //Creates a priority queue

	for (int i = 0; i < 256; i++)
	{
		if (analysedData[i] != 0)
		{
			minHeap.push(new Huffman::MinHeapNode(i, analysedData[i], true));
		}
	}

	while (minHeap.size() != 1) //While there are more nodes than 1
	{
		left = minHeap.top();
		minHeap.pop();

		right = minHeap.top();
		minHeap.pop();

		top = new Huffman::MinHeapNode(0, left->freq + right->freq, false);
		top->left = left;
		top->right = right;
		minHeap.push(top);
	}

	PopulateTree(minHeap.top(), ""); //Populates the Huffman tree recursively

	huffmanTree = minHeap.top();
}

void Huffman::SaveTreeToFile(std::string outputPath)
{
	std::ofstream outStream(outputPath, std::ios::binary); //Opens an output filestream to the chosen path

	EncodeTree(huffmanTree, &outStream); //Encodes the current tree to disk

	outStream.close(); //Closes output stream when done
}

void Huffman::CompressFile(std::string filePath, std::string outputPath) //Compress other file
{
	//Opens in- and output streams
	std::ifstream inStream(filePath, std::ios::binary);
	std::ofstream outStream(outputPath, std::ios::binary);

	//Declares local variables
	std::bitset<8U> currentByte = 0; //Temporary byte that is filled bitwise and written when full
	int totalCount = 0; //Operation counter to track progress through current byte

	while (inStream.good()) //While there are bytes to read
	{
		unsigned char inputByte = inStream.get(); //Get next character (byte)

		for (int i = 0; i < size(codeLookup[inputByte]); i++) //For each character in that character's code
		{
			currentByte[7 - totalCount % 8] = (int)(codeLookup[inputByte][i] - '0'); //Convert that code character to an int, and assign it to the current position in currentByte

			totalCount++; //Increments total operation counter
			if (totalCount % 8 == 0) //If at the end of byte
			{
				outStream.put(currentByte.to_ulong()); //Add temp byte to byte list
				currentByte = 0; //Reset temp byte
			}
		}
	}
	if (totalCount % 8 != 0) //If total code length not divisible by 8
	{
		outStream.put(currentByte.to_ulong()); //Add the temp byte (LSBs padded with 0) - Ensures that all data will be written, rather than partial bytes being discarded
	}

	//Closes streams
	inStream.close();
	outStream.close();
}


//Decompresses a file, and places it in another location
void Huffman::DecompressFile(std::string filePath, std::string outputPath)
{
	//Open input and output streams
	std::ifstream inStream(filePath, std::ios::binary);
	std::ofstream outStream(outputPath, std::ios::binary);

	//Initialise local variables
	Huffman::MinHeapNode* currentNode = huffmanTree; //Sets default node
	int currentLength = 0; //Comparator for filelength to avoid printing padding

	for (bool atEnd = false; inStream.good() && !atEnd;) //Runs while there are bytes in the input stream and the given file length has not been reached.
	{
		std::bitset<8U> currentByte = inStream.get(); //Loads a byte from stream into a bitset
		for (int i = 7; 0 <= i; i--) //Loops through the byte from MSB to LSB
		{
			if (currentNode->endPoint) //If the reached node is an endpoint
			{
				outStream.put(currentNode->symbol); //Output that character
				currentNode = huffmanTree; //Reset that tree

				if (++currentLength == fileLength - 1) //If the given end of the file has been reached
				{
					//Ensure that loops are broken
					atEnd = true;
					break;
				}
			}
			if (currentByte[i] == 0) //If the bit is low, go left down the tree
			{
				currentNode = currentNode->left;
			}
			else if (currentByte[i] == 1) //If the bit is high, go right up the tree
			{
				currentNode = currentNode->right;
			}
		}
	}

	//Make sure the streams are closed
	inStream.close();
	outStream.close();
}

void Huffman::PopulateTree(struct MinHeapNode* root, std::string str) //Maps tree to codes
{
	if (!root) 
		return;

	if (root->endPoint) //If node is endpoint
	{
		codeLookup[root->symbol] = str; //Add current code to lookup table for that character
	}
	PopulateTree(root->left, str + "0"); //Goes left
	PopulateTree(root->right, str + "1"); //Goes right
}

Huffman::MinHeapNode* Huffman::FindNodeFromPath(Huffman::MinHeapNode* node, std::string path)
{
	Huffman::MinHeapNode* rnode = node;
	for (char c : path)
	{
		if (c == '1')
		{
			rnode = rnode->right;
		}
		if (c == '0')
		{
			rnode = rnode->left;
		}
	}
	return rnode;
};

void Huffman::LoadTreeFromFile(std::string filePath)
{
	Huffman::MinHeapNode* node = new Huffman::MinHeapNode;
	std::ifstream inStream(filePath, std::ios::binary);

	std::string path = "";

	while (inStream.good())
	{
		Huffman::MinHeapNode* tnode = FindNodeFromPath(node, path);
		unsigned char currentByte = inStream.get();
		switch (currentByte)
		{
		case '{':
			tnode->right = new Huffman::MinHeapNode;
			path += "1";
			break;
		case '[':
			tnode->left = new Huffman::MinHeapNode;
			path += "0";
			break;
		case '/':
			tnode->symbol = inStream.get();
			tnode->endPoint = true;
			break;
		case ']':
		case '}':
			path.pop_back();
			break;
		}
	}

	inStream.close();

	huffmanTree = node;
};

void Huffman::EncodeTree(Huffman::MinHeapNode* node, std::ofstream* outStream, int dir)
{
	if (dir == 1)
		outStream->put('{');
	if (dir == 0)
		outStream->put('[');
	if (node->endPoint)
	{
		outStream->put('/');
		outStream->put(node->symbol);
	}
	else
	{
		EncodeTree(node->left, outStream, 0);
		EncodeTree(node->right, outStream, 1);
	}
	if (dir == 1)
		outStream->put('}');
	if (dir == 0)
		outStream->put(']');
};