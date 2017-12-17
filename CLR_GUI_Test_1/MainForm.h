#pragma once
#include <Windows.h>
#include "OBuffer.h"
#include <string>
#include <iostream>
#include <dirent.h>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <vector>
#include "DirectoryHandler.h"
#include "TreeElement.h"
#include <fstream>
#include <algorithm>
#include "Huffman.h"
#include "NewDirPopup.h"
#include <direct.h>
#include "DLLayer.h"
#include "FLayer.h"

namespace Projekt3_Gruppe2_1 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{			
			InitializeComponent();
			
			//
			//TODO: Add the constructor code here
			//
			
		};
		void addFilesToListView(String ^ folderPath)
		{			
			treeView1->Nodes->Clear();
			const char * _folderPath = context->marshal_as<const char*>(folderPath); //Converts given foldername to char* for use
			int lengthOfRoot = folderPath->LastIndexOf("\\") + 1;

			String^ folderName = folderPath->Remove(0, lengthOfRoot);

			std::vector<TreeElement> folderList = DirectoryHandler::getAllFolders(_folderPath); //Gets all folders within root dir (Recursively)	
			std::ofstream folderListLog("folderlist.txt");
			folderList.emplace(folderList.begin(), TreeElement(_folderPath, _folderPath,false));

			TreeNode^ node = gcnew TreeNode;
			//node->

			for (int i = 0; i < folderList.size(); i++) //Looping over all folders found from root dir
			{
				if (i == 0)
				{
					treeView1->Nodes->Add(makeNode(folderList[0], Color::LightBlue, "D" + i));					
				}
				else
				{
					addNodeToTreeView(treeView1, folderList[i], folderList[i].path, Color::LightYellow, "D" + i);
				}
				folderListLog << folderList[i].path << std::endl;
			}		
			folderListLog.close();

			std::vector<std::string> fileList = DirectoryHandler::getAllFiles(folderList);		
			std::ofstream fileListLog("filelist.txt");

			for(int i = 0; i < fileList.size(); i++)
			{
				std::string name = fileList[i];
				name = name.substr(name.find_last_of("\\") + 1);

				addNodeToTreeView(treeView1, TreeElement(name, name, false), fileList[i], Color::LightSteelBlue, "F" + i);

				std::string s = fileList[i];
				fileListLog << s << std::endl;
			}
			fileListLog.close();

			richTextBox1->Text += "Total of " + fileList.size() + " files.\n";
			//treeView1->ExpandAll();
			//treeView1->Nodes[0]->Nodes[0]->Nodes[0]->ExpandAll();

		}
		TreeNode^ makeNode(TreeElement element, Color color, String^ tag)
		{
			TreeNode^ node = gcnew TreeNode();
			
			node->Name = msclr::interop::marshal_as<String^>(element.path);
			node->Text = msclr::interop::marshal_as<String^>(element.name);
			node->Tag = tag;

			node->BackColor = color;

			return node;			
		}
		void populateTreeViewFromFile()
		{
			std::string fileLogFilePath = "C:\\Programming Utilities\\filelist.txt";
			std::string folderLogFilePath = "C:\\Programming Utilities\\folderlist.txt";

			std::vector<std::string> filePaths = DirectoryHandler::loadFromFile(fileLogFilePath);
			std::vector<std::string> folderPaths = DirectoryHandler::loadFromFile(folderLogFilePath);
			
			for (int i = 0; i < folderPaths.size(); i++)
			{
				std::string name = folderPaths[i];
				name = name.substr(name.find_last_of("\\") + 1);

				if (i == 0)
				{
					treeView2->Nodes->Add(makeNode(TreeElement(folderPaths[i],folderPaths[i],false), Color::BurlyWood, "D" + i));
				}
				else
				{
					addNodeToTreeView(treeView2, TreeElement(folderPaths[i], name, false), folderPaths[i], Color::Aquamarine, "D" + i);
				}				
			}

			for (int i = 0; i < filePaths.size(); i++)
			{
				std::string name = filePaths[i];
				name = name.substr(name.find_last_of("\\") + 1);

				addNodeToTreeView(treeView2, TreeElement(name, name, false), filePaths[i], Color::Aqua, "F" + i);
			}

			richTextBox1->Text += "Done";
		}
		void addNodeToTreeView(TreeView^ treeView, TreeElement element, std::string path, Color color, String^ tag)
		{
			cli::array<TreeNode^, 1>^ nodes = findNode(msclr::interop::marshal_as<String^>(path), treeView);

			nodes[0]->Nodes->Add(makeNode(element, color, tag));
		}
		cli::array<TreeNode^, 1>^ findNode(String^ path, TreeView^ treeView)
		{
			String^ parentname = path;
			parentname = parentname->Remove(parentname->LastIndexOf("\\")); //Gets path to parent of current directory

			return treeView->Nodes->Find(parentname, true); //Finds matching directories based on nodes' Name attribute and places them in an array
		}

	private:
		Huffman* h;
		String^ treeView1FolderPath;
		DLLayer* dlLayer;
		FLayer* fLayer;
		int sf = 2048;
		msclr::interop::marshal_context^ context = gcnew msclr::interop::marshal_context(); //Creates marshal_context to convert String^ to char*

private: System::Windows::Forms::Button^  btn_pop;
private: System::Windows::Forms::TreeView^  treeView1;
private: System::Windows::Forms::RichTextBox^  richTextBox1;

private: System::Windows::Forms::TreeView^  treeView2;


private: System::Windows::Forms::Button^  deleteNodeButton;
private: System::Windows::Forms::Button^  newFolderButton;
private: System::Windows::Forms::Button^  selectServerButton;
private: System::Windows::Forms::Button^  requestTreeButton;
private: System::Windows::Forms::Button^  getFileButton;
private: System::Windows::Forms::Button^  sendFileButton;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Button^  resetButton;

private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Button^  exitButton;
private: System::Windows::Forms::Button^  selectClientButton;
		
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
			delete fLayer;
			delete dlLayer;
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{			
			this->btn_pop = (gcnew System::Windows::Forms::Button());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			this->treeView2 = (gcnew System::Windows::Forms::TreeView());
			this->deleteNodeButton = (gcnew System::Windows::Forms::Button());
			this->newFolderButton = (gcnew System::Windows::Forms::Button());
			this->selectServerButton = (gcnew System::Windows::Forms::Button());
			this->selectClientButton = (gcnew System::Windows::Forms::Button());
			this->requestTreeButton = (gcnew System::Windows::Forms::Button());
			this->getFileButton = (gcnew System::Windows::Forms::Button());
			this->sendFileButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->resetButton = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->exitButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// btn_pop
			// 
			this->btn_pop->Location = System::Drawing::Point(383, 28);
			this->btn_pop->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->btn_pop->Name = L"btn_pop";
			this->btn_pop->Size = System::Drawing::Size(197, 28);
			this->btn_pop->TabIndex = 1;
			this->btn_pop->Text = L"Select working folder";
			this->btn_pop->UseVisualStyleBackColor = true;
			this->btn_pop->Click += gcnew System::EventHandler(this, &MainForm::btn_pop_Click);
			// 
			// treeView1
			// 
			this->treeView1->Location = System::Drawing::Point(12, 28);
			this->treeView1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(365, 661);
			this->treeView1->TabIndex = 2;
			this->treeView1->NodeMouseDoubleClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &MainForm::treeView1_NodeMouseDoubleClick);
			// 
			// richTextBox1
			// 
			this->richTextBox1->Location = System::Drawing::Point(383, 477);
			this->richTextBox1->Margin = System::Windows::Forms::Padding(4);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->Size = System::Drawing::Size(200, 212);
			this->richTextBox1->TabIndex = 3;
			this->richTextBox1->Text = L"";
			// 
			// treeView2
			// 
			this->treeView2->Location = System::Drawing::Point(588, 28);
			this->treeView2->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->treeView2->Name = L"treeView2";
			this->treeView2->Size = System::Drawing::Size(365, 661);
			this->treeView2->TabIndex = 5;
			this->treeView2->NodeMouseDoubleClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &MainForm::treeView2_NodeMouseDoubleClick);
			// 
			// deleteNodeButton
			// 
			this->deleteNodeButton->Location = System::Drawing::Point(385, 160);
			this->deleteNodeButton->Name = L"deleteNodeButton";
			this->deleteNodeButton->Size = System::Drawing::Size(94, 28);
			this->deleteNodeButton->TabIndex = 8;
			this->deleteNodeButton->Text = L"Delete";
			this->deleteNodeButton->UseVisualStyleBackColor = true;
			this->deleteNodeButton->Click += gcnew System::EventHandler(this, &MainForm::deleteNodeButton_Click);
			// 
			// newFolderButton
			// 
			this->newFolderButton->Location = System::Drawing::Point(485, 160);
			this->newFolderButton->Name = L"newFolderButton";
			this->newFolderButton->Size = System::Drawing::Size(95, 28);
			this->newFolderButton->TabIndex = 9;
			this->newFolderButton->Text = L"New Folder";
			this->newFolderButton->UseVisualStyleBackColor = true;
			this->newFolderButton->Click += gcnew System::EventHandler(this, &MainForm::newFolderButton_Click);
			// 
			// selectServerButton
			// 
			this->selectServerButton->Location = System::Drawing::Point(383, 61);
			this->selectServerButton->Name = L"selectServerButton";
			this->selectServerButton->Size = System::Drawing::Size(96, 28);
			this->selectServerButton->TabIndex = 10;
			this->selectServerButton->Text = L"Server";
			this->selectServerButton->UseVisualStyleBackColor = true;
			this->selectServerButton->Click += gcnew System::EventHandler(this, &MainForm::selectServerButton_Click);
			// 
			// selectClientButton
			// 
			this->selectClientButton->Location = System::Drawing::Point(486, 61);
			this->selectClientButton->Name = L"selectClientButton";
			this->selectClientButton->Size = System::Drawing::Size(95, 28);
			this->selectClientButton->TabIndex = 11;
			this->selectClientButton->Text = L"Client";
			this->selectClientButton->UseVisualStyleBackColor = true;
			this->selectClientButton->Click += gcnew System::EventHandler(this, &MainForm::selectClientButton_Click);
			// 
			// requestTreeButton
			// 
			this->requestTreeButton->Location = System::Drawing::Point(385, 194);
			this->requestTreeButton->Name = L"requestTreeButton";
			this->requestTreeButton->Size = System::Drawing::Size(196, 28);
			this->requestTreeButton->TabIndex = 12;
			this->requestTreeButton->Text = L"Request file tree";
			this->requestTreeButton->UseVisualStyleBackColor = true;
			this->requestTreeButton->Click += gcnew System::EventHandler(this, &MainForm::requestTreeButton_Click);
			// 
			// getFileButton
			// 
			this->getFileButton->Location = System::Drawing::Point(385, 228);
			this->getFileButton->Name = L"getFileButton";
			this->getFileButton->Size = System::Drawing::Size(195, 28);
			this->getFileButton->TabIndex = 13;
			this->getFileButton->Text = L"<- Get file";
			this->getFileButton->UseVisualStyleBackColor = true;
			this->getFileButton->Click += gcnew System::EventHandler(this, &MainForm::getFileButton_Click);
			// 
			// sendFileButton
			// 
			this->sendFileButton->Location = System::Drawing::Point(385, 262);
			this->sendFileButton->Name = L"sendFileButton";
			this->sendFileButton->Size = System::Drawing::Size(195, 28);
			this->sendFileButton->TabIndex = 14;
			this->sendFileButton->Text = L"Send file ->";
			this->sendFileButton->UseVisualStyleBackColor = true;
			this->sendFileButton->Click += gcnew System::EventHandler(this, &MainForm::sendFileButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(585, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(83, 17);
			this->label1->TabIndex = 15;
			this->label1->Text = L"Server Files";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 9);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(75, 17);
			this->label2->TabIndex = 16;
			this->label2->Text = L"Local Files";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(382, 9);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(45, 17);
			this->label3->TabIndex = 17;
			this->label3->Text = L"Setup";
			// 
			// resetButton
			// 
			this->resetButton->Enabled = false;
			this->resetButton->Location = System::Drawing::Point(383, 95);
			this->resetButton->Name = L"resetButton";
			this->resetButton->Size = System::Drawing::Size(198, 28);
			this->resetButton->TabIndex = 18;
			this->resetButton->Text = L"Reset";
			this->resetButton->UseVisualStyleBackColor = true;
			this->resetButton->Click += gcnew System::EventHandler(this, &MainForm::resetButton_Click_1);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(384, 456);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(32, 17);
			this->label4->TabIndex = 19;
			this->label4->Text = L"Log";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(384, 140);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(87, 17);
			this->label5->TabIndex = 20;
			this->label5->Text = L"FTP Options";
			// 
			// exitButton
			// 
			this->exitButton->Location = System::Drawing::Point(385, 425);
			this->exitButton->Name = L"exitButton";
			this->exitButton->Size = System::Drawing::Size(195, 28);
			this->exitButton->TabIndex = 21;
			this->exitButton->Text = L"Exit";
			this->exitButton->UseVisualStyleBackColor = true;
			this->exitButton->Click += gcnew System::EventHandler(this, &MainForm::exitButton_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(966, 699);
			this->Controls->Add(this->exitButton);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->resetButton);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->sendFileButton);
			this->Controls->Add(this->getFileButton);
			this->Controls->Add(this->requestTreeButton);
			this->Controls->Add(this->selectClientButton);
			this->Controls->Add(this->selectServerButton);
			this->Controls->Add(this->treeView1);
			this->Controls->Add(this->newFolderButton);
			this->Controls->Add(this->btn_pop);
			this->Controls->Add(this->deleteNodeButton);
			this->Controls->Add(this->richTextBox1);
			this->Controls->Add(this->treeView2);
			this->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->MaximizeBox = false;
			this->Name = L"MyForm";
			this->Text = L"DTMF FTP";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		OpenFileDialog^ openFileDialog1 = gcnew OpenFileDialog;
		openFileDialog1->InitialDirectory = "c:\\";
		
		openFileDialog1->ShowDialog();
		MessageBox::Show(openFileDialog1->FileName);

	}
	private: System::Void btn_pop_Click(System::Object^  sender, System::EventArgs^  e) {
		FolderBrowserDialog folderBrowserDialog1;
		folderBrowserDialog1.ShowDialog();
		treeView1FolderPath = folderBrowserDialog1.SelectedPath;
		addFilesToListView(folderBrowserDialog1.SelectedPath);
	}
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {		
	}
	private: System::Void treeView1_NodeMouseDoubleClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e)
	{
		System::Diagnostics::Process::Start(e->Node->FullPath);		
		richTextBox1->Text += e->Node->Tag;
	}			 
private: System::Void loadFromFileButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	populateTreeViewFromFile();
}
private: System::Void treeView2_NodeMouseDoubleClick(System::Object^  sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^  e) 
{
	System::Diagnostics::Process::Start(e->Node->FullPath);
}
private: System::Void compressFileButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	String^ readFilePath = treeView1->SelectedNode->FullPath;
	const char * filePath = context->marshal_as<const char*>(readFilePath); //Converts given foldername to char* for use
	h = new Huffman;

	h->AnalyseData(filePath);
	
	h->BuildTree();
	
	h->SaveTreeToFile("C:\\Users\\aszel\\Desktop\\Huffmantest\\Tree.tr");
	
	h->CompressFile(filePath, "C:\\Users\\aszel\\Desktop\\Huffmantest\\data");
	richTextBox1->Text += "Compressed file to working directory\n";
	treeView1->Nodes->Clear();
	addFilesToListView(treeView1FolderPath);
	treeView1->ExpandAll();
}
private: System::Void decompressFileButton_Click(System::Object^  sender, System::EventArgs^  e)
{
	h->LoadTreeFromFile("C:\\Users\\aszel\\Desktop\\Huffmantest\\Tree.tr");
	h->DecompressFile("C:\\Users\\aszel\\Desktop\\Huffmantest\\data", "C:\\Users\\aszel\\Desktop\\Huffmantest\\reconstructed.txt");
	richTextBox1->Text += "Decompressed file to working directory\n";
	treeView1->Nodes->Clear();
	addFilesToListView(treeView1FolderPath);
	treeView1->ExpandAll();
}
private: System::Void deleteNodeButton_Click(System::Object^  sender, System::EventArgs^  e) 
{	
	String^ nodePath = treeView1->SelectedNode->FullPath;
	String^ nodeName = nodePath->Substring(nodePath->LastIndexOf("\\") + 1);
	cli::array<TreeNode^, 1>^ node = findNode(nodePath, treeView1);
	
	for (int i = 0; i < node[0]->Nodes->Count; i++)
	{
		if (node[0]->Nodes[i]->Name == nodeName)
		{
			node[0]->Nodes[i]->Remove();
		}
	}
	//treeView1->SelectedNode->Remove();
}
private: System::Void newFolderButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	String^ text;
	
	NewDirPopup^ dlg = gcnew NewDirPopup;
	dlg->ShowDialog();
	text = dlg->GetTextResult();
	delete dlg;
	
	String^ nodePath = treeView1->SelectedNode->FullPath;
	String^ nodeName = nodePath->Substring(nodePath->LastIndexOf("\\") + 1);
	cli::array<TreeNode^, 1>^ node = findNode(nodePath, treeView1);

	node[0]->Nodes->Add(text);
	int i = node[0]->Nodes->Count;
	const char* dirName = context->marshal_as<const char*>(node[0]->Nodes[i-1]->FullPath);
	_mkdir(dirName);	
}
private: System::Void selectServerButton_Click(System::Object^  sender, System::EventArgs^  e) {
	selectServerButton->Enabled = false;
	selectClientButton->Enabled = false;
	deleteNodeButton->Enabled = false;
	getFileButton->Enabled = false;
	sendFileButton->Enabled = false;
	requestTreeButton->Enabled = false;
	newFolderButton->Enabled = false;

	richTextBox1->Text += "Server mode selected\n";
	treeView2->Enabled = false;
	dlLayer = new DLLayer(0x00);
	fLayer = new FLayer(sf);

	dlLayer->setFramePusher(fLayer->sendFrame, fLayer);
	fLayer->setInputCallback(dlLayer->frameReceivedWrap, dlLayer);
}
private: System::Void selectClientButton_Click(System::Object^  sender, System::EventArgs^  e) {
	selectServerButton->Enabled = false;
	selectClientButton->Enabled = false;
	resetButton->Text = "Reset Client";
	resetButton->Enabled = true;
	richTextBox1->Text += "Client mode selected\n";
	dlLayer = new DLLayer(0xFF);
	fLayer = new FLayer(sf);

	dlLayer->setFramePusher(fLayer->sendFrame, fLayer);
	fLayer->setInputCallback(dlLayer->frameReceivedWrap, dlLayer);

	dlLayer->sendAskForAdress();
}
private: System::Void exitButton_Click(System::Object^  sender, System::EventArgs^  e) {
	delete dlLayer;
	delete fLayer;
	Application::Exit();
}
private: System::Void resetButton_Click_1(System::Object^  sender, System::EventArgs^  e) {
	dlLayer->sendAskForAdress();
}
private: System::Void requestTreeButton_Click(System::Object^  sender, System::EventArgs^  e) {
}
private: System::Void getFileButton_Click(System::Object^  sender, System::EventArgs^  e) {
	std::vector<std::string> pathList;
	std::ifstream stream;
	int counter = 0;

	int index = Convert::ToInt32(treeView1->SelectedNode->Tag->ToString()->Substring(1));
	String^ type = treeView1->SelectedNode->Tag->ToString()->Substring(0, 1);
	if (type == "F")
	{
		stream.open("filelist.txt", std::ios::binary);
	}
	else if (type == "D")
	{
		stream.open("folderlist.txt", std::ios::binary);
	}

	while (stream.good())
	{
		counter++;
		char line[1024];
		stream.getline(line, 1024);
		pathList.push_back(line);
	}
	std::cout << pathList[index] << std::endl;

}
private: System::Void sendFileButton_Click(System::Object^  sender, System::EventArgs^  e) {
}
};
}
