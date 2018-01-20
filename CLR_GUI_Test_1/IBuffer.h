#pragma once
#include "portaudio.h"
#include <vector>
#include <math.h>
#include <complex>
#include <Windows.h>

#include "Frame.h"

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#define IDLE 0
#define FIRST_FLAG_TONE_FOUND 1
#define BEFORE_COLLECTING 2
#define COLLECTING 3
#define COLLECTING_DATA_FRAME 4
#define COLLECTING_INFO_FRAME 5

#define FIRST_FLAG_TONE 0
#define LAST_FLAG_TONE 15

#define WINDOW_RESOLUTION 4

class IBuffer
{
public:
	IBuffer(int bz = 1024);
	~IBuffer();
	PaStream *stream;
	bool open(PaDeviceIndex index);
	bool close();
	bool start();
	bool stop();
	std::vector<float> getFFT();
	std::vector<float> getFFTs();

	void setCallback(fcb inputCallBack, void* dll);

private:
	/**/
	fcb callback;
	void* dll;
	int currentFrameSize = 0;

	/* CONSOLE COLOR ON WINDOWS*/
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	/**  STATE MACHINE **/
	int state = IDLE;
	int flagCounter = 0;
	int toneCounter = 0;
	int iPointer = 0;
	int lowPointFlag = 0;
	float lowValueFlag = 0;

	std::vector<char> inchars;

	int startLookFrame;
	int endLookFrame;
	float* LookArrs[16];
	int calcFFTn(int f);
	int n[8];
	std::vector<float> fft;
	std::vector<float> fftOutBuffer[20];
	std::complex<float>* fftBuffer;
	std::complex<float>* fftOut;
	int fftPointer;
	static const int DTMF_FREQ[8];
	int pointer;
	bool filled;
	int sampleBufferSize;
	int bufferSize;
	static int sampleRate;
	std::vector<float> buffer;
	char message[20];
	int paCallbackMethod(const void * inputBuffer, void * outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo * timeInfo, PaStreamCallbackFlags statusFlags);
	static int paCallback(const void * inputBuffer, void * outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo * timeInfo, PaStreamCallbackFlags statusFlags, void * userData);
	static void paStreamFinished(void * userData);
	void paStreamFinishedMethod();
	std::vector<float> goertzel_mag();
	void separate(std::complex<float>* a, int n);
	void fft2(std::complex<float>* X, int N);
};


