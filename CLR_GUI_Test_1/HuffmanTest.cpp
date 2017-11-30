#include <iostream>
#include <string>
#include "Huffman.h"
#include "StopWatch.h"

using namespace std;

void main()
{
	string rootpath = "D:\\Desktop\\Huffmantest\\";
	string filePath = rootpath + "folderlist.txt";
	string treePath = rootpath + "tree.txt";
	string compressedFilePath = rootpath + "compressed";
	string reconstructedFilePath = rootpath + "reconstructed.txt";

	Huffman h;
	StopWatch st(true);
	StopWatch sw(true);

	h.AnalyseData(filePath);
	cout << "Loaded and analysed data in: " << st.StopGetAndReset() << " seconds.\n";

	cout << "Filesize: " << (double)h.getFileLength() / 1000.0 << " KB\n";

	int charCount = 0;
	int* charList = h.getAnalysedData();
	for (int i = 0; i < 256; i++)
	{
		if (charList[i] != 0)
		{
			charCount++;
		}
	}
	cout << charCount << " Unique characters found\n\n";

	h.BuildTree();
	cout << "Built tree from data in: " << st.StopGetAndReset() << " seconds.\n\n";

	cout << "Estimated compression: " << h.estimateCompression() << "% of original size.\n\n";

	h.SaveTreeToFile(treePath);
	cout << "Tree structure written to: " + treePath << " in: " << st.StopGetAndReset() << " seconds.\n\n";

	h.LoadTreeFromFile(treePath);
	cout << "Loaded tree from file in: " << st.StopGetAndReset() << " seconds.\n\n";

	h.CompressFile(filePath, compressedFilePath);

	cout << "Compressed file written to " + compressedFilePath << " in: " << st.StopGetAndReset() << " seconds.\n\n";

	h.DecompressFile(compressedFilePath, reconstructedFilePath);
	cout << "File decompressed to " + reconstructedFilePath << " in: " << st.StopAndGet() << " seconds.\n\n";

	cout << "Total time elapsed: " << sw.StopAndGet() << " seconds.\n";

	system("PAUSE");

}

