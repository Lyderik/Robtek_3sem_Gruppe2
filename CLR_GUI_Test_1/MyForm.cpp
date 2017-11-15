#include "MyForm.h"
#include "OBuffer.h"
#include <string>
#include <iostream>
#include <filesystem>

using namespace System;
using namespace System::Windows::Forms;


[STAThread]
int Main(array<String^>^ args)
{
	ScopedPaHandler paInit;
	OBuffer wBuffer(100);
	if (paInit.result() == paNoError) {
		if (wBuffer.open(Pa_GetDefaultOutputDevice())) {
			if (!wBuffer.start()) {
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}

	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	CLRGUITest1::MyForm form;
	form.addOBuffer(wBuffer);
	Application::Run(%form);
}

void CLRGUITest1::MyForm::addOBuffer(OBuffer & obuffer)
{
	wBuffer = &obuffer;
}
