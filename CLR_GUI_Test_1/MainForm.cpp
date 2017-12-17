#include "MainForm.h"
#include "OBuffer.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "ScopedPaHandler.h"
#include "DLLayer.h"
#include "FLayer.h"

using namespace System;
using namespace System::Windows::Forms;


[STAThread]
void Main(array<String^>^ args)
{
	/*int address = 0x00;
	int source = 0x00;
	int dest = 0x01;

	int sf = 1024;

	try {
		DLLayer dllayer(address);
		FLayer flayer(sf);
		dllayer.setFramePusher(flayer.sendFrame, &flayer);
		flayer.setInputCallback(dllayer.frameReceivedWrap, &dllayer);

	reset:
		if (source == 0x01) {
			dllayer.sendAskForAdress();
		}
		FrameHandler f;

	}
	catch (int i) {
		return i;
	}*/

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Projekt3_Gruppe2_1::MainForm form;
	
	Application::Run(%form);
}
