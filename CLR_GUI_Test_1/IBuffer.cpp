#include "IBuffer.h"
#include <iostream>
#include <iomanip> 

int IBuffer::sampleRate = 44100;
const int IBuffer::DTMF_FREQ[8] = { 697, 770, 852, 941, 1209, 1336, 1477, 1633 };



IBuffer::IBuffer(int bz)
{
	sampleBufferSize = bz / WINDOW_RESOLUTION;
	bufferSize = WINDOW_RESOLUTION * sampleBufferSize;
	buffer = std::vector<float>(bufferSize);
	fftBuffer = new std::complex<float>[bufferSize];

	fftOut = new std::complex<float>[bufferSize];
	fft = std::vector<float>(bufferSize / 2);
	fftPointer = 0;
	for (int i = 0; i < 20; i++)
		fftOutBuffer[i] = std::vector<float>(bufferSize / 2);

	for (int i = 0; i < 8; i++)
		n[i] = calcFFTn(DTMF_FREQ[i]);
	startLookFrame = calcFFTn(600);
	endLookFrame = calcFFTn(1700);

}

IBuffer::~IBuffer()
{

}

bool IBuffer::open(PaDeviceIndex index)
{
	PaStreamParameters inputParameters;

	inputParameters.device = index;
	if (inputParameters.device == paNoDevice) {
		return false;
	}

	const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
	if (pInfo != 0)
	{

		/*printf("Output device name: '%s'\n", pInfo->name);
		std::cout << pInfo->maxInputChannels << std::endl;
		std::cout << pInfo->maxOutputChannels << std::endl;
		std::cout << pInfo->defaultSampleRate << std::endl;*/
	}

	inputParameters.channelCount = 1;       /* mono input */
	inputParameters.sampleFormat = paFloat32; /* 32 bit floating point input */
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(
		&stream,
		&inputParameters,
		NULL, /* no output */
		sampleRate,
		sampleBufferSize,
		paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		&IBuffer::paCallback,
		this            /* Using 'this' for userData so we can cast to Sine* in paCallback method */
	);


	if (err != paNoError)
	{
		/* Failed to open stream to device !!! */
		return false;
	}

	err = Pa_SetStreamFinishedCallback(stream, &IBuffer::paStreamFinished);

	if (err != paNoError)
	{
		std::cout << "\n\n   Input open fail   \n\n";
		Pa_CloseStream(stream);
		stream = 0;

		return false;
	}

	return true;
}

bool IBuffer::close()
{
	if (stream == 0)
		return false;

	PaError err = Pa_CloseStream(stream);
	stream = 0;

	return (err == paNoError);
}


bool IBuffer::start()
{
	if (stream == 0)
		return false;

	PaError err = Pa_StartStream(stream);
	return (err == paNoError);
}

bool IBuffer::stop()
{
	if (stream == 0)
		return false;

	PaError err = Pa_StopStream(stream);

	return (err == paNoError);
}
std::vector<float> IBuffer::getFFT()
{
	return fft;
}

std::vector<float> IBuffer::getFFTs()
{
	return fftOutBuffer[fftPointer];
}

void IBuffer::setCallback(fcb cb, void * dllp)
{
	callback = cb;
	dll = dllp;
}

int IBuffer::calcFFTn(int f)
{
	return (int)(((double)(bufferSize * f) / (double)sampleRate) + 0.5);
}


int IBuffer::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags)
{
	(void)outputBuffer;
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;

	const float * in = (const float*)inputBuffer;
	if (state == IDLE || state == FIRST_FLAG_TONE_FOUND) {
		int endStarFrame = bufferSize - sampleBufferSize; //sidste frame før nye data
		for (int i = 0; i < endStarFrame; i++) // shit indhold array frem 
			fftBuffer[i] = fftBuffer[i + sampleBufferSize];
		for (int i = 0; i < framesPerBuffer; i++) // overskriv enden af array med ny data
			fftBuffer[endStarFrame + i] = *(in + i);
	}
	if (state == BEFORE_COLLECTING) {
		if (++iPointer >= WINDOW_RESOLUTION) {
			iPointer = 0;
		}
		else
		{
			return paContinue;
		}
	}
	if (state == COLLECTING || state == COLLECTING_DATA_FRAME || state == COLLECTING_INFO_FRAME) {
		int offset = iPointer * framesPerBuffer;
		for (int i = 0; i < framesPerBuffer; i++)
			fftBuffer[offset + i] = *(in + i);
		if (++iPointer >= WINDOW_RESOLUTION) {
			iPointer = 0;
		}
		else {
			return paContinue;
		}
	}
	/* FFT MAGIC */
	for (int i = 0; i < bufferSize; i++)
		fftOut[i] = fftBuffer[i];

	fft2(fftOut, bufferSize);

	int fftLenght = bufferSize / 2;
	for (int i = 0; i < fftLenght; i++) {
		fft[i] = abs(fftOut[i]);
	}

	float avr = 0;
	for (int j = startLookFrame; j < endLookFrame; j++)
		avr += fft[j];
	avr /= endLookFrame - startLookFrame;
	avr *= 2;
	float mag[16];
	for (int i = 0; i < 16; i++) {
		mag[i] = 0;
		int nl = n[i / 4];
		int nh = n[i % 4 + 4];
		for (int j = startLookFrame; j < endLookFrame; j++)
		{
			if (j == nl || j == nh)
				mag[i] -= fft[j];
			else
				mag[i] += fft[j];
		}
		mag[i] /= avr;
	}

	float avr2 = 0;
	float smalls = 9999999;
	int nsmall = 0;
	for (int i = 0; i < 16; i++) {
		avr2 += mag[i];
		if (mag[i] < smalls) {
			smalls = mag[i];
			nsmall = i;
		}
	}
	avr2 /= 18.0f;
	if (mag[nsmall] > avr2)
		nsmall = -1;

	/** CONSOLE LOGGING **/

	
	/** STATE MACHINE **/
	switch (state)
	{
	case IDLE: // Listening for first flag tone
		if (nsmall == FIRST_FLAG_TONE) {
			if (++flagCounter >= WINDOW_RESOLUTION/2) {
				flagCounter = 0;
				state = FIRST_FLAG_TONE_FOUND;
				std::cout << "FIRST FLAG TONE FOUND\n";
			}
		}
		else if (flagCounter > 0)
		{
			flagCounter--;
		}
		break;

	case FIRST_FLAG_TONE_FOUND:
		if (!(flagCounter == 0 && nsmall == FIRST_FLAG_TONE)) {
			if (toneCounter++ == 0) {
				lowValueFlag = mag[15];
			}
			if (++iPointer >= WINDOW_RESOLUTION) {
				iPointer = 0;
			}

			if (nsmall == LAST_FLAG_TONE) {
				flagCounter++;
				if (mag[15] < lowValueFlag) {
					lowValueFlag = mag[15];
					iPointer = 1;
				}
			}
			if (toneCounter > 1.5 * WINDOW_RESOLUTION) {
				if (flagCounter > 1) {
					std::cout << "SECOND FLAG TONE FOUND\n";
					state = BEFORE_COLLECTING;
				}
				else {
					std::cout << "FAIIL\n";
					state = IDLE;
				}
				toneCounter = 0;
				flagCounter = 0;
			}
		}
		break;

	case BEFORE_COLLECTING:
		if (iPointer == 0)
			state = COLLECTING;
		break;

	case COLLECTING:
		if (nsmall == -1) {
			state = IDLE;
			std::cout << "error: failed to hear start off frame\n";
			inchars.clear();
		}
		else {
			inchars.push_back(nsmall);
			if (inchars.size() == 6) {
				state = COLLECTING_DATA_FRAME;
				int type = inchars[5] & 0b11;
				switch (type)
				{
				case FrameHandler::TYPE_FILE:
				case FrameHandler::TYPE_REQUEST:
					currentFrameSize = 2 * inchars[4] + 10;
					break;
				case FrameHandler::TYPE_FILE_INFO:
					currentFrameSize = 40;
					break;
				case FrameHandler::TYPE_HANDSHAKE:
					if ((16 * inchars[4] + inchars[5] & 0b11111) == 0b01000) {
						currentFrameSize = 10;
					}
					else
					{
						currentFrameSize = 8;
					}
					break;
				}
			}
		}
		break;
	case COLLECTING_DATA_FRAME:
		if (nsmall == -1) {
			state = IDLE;
			std::cout << "error: failed to hear data frame\n";
			inchars.clear();
		}
		else {
			inchars.push_back(nsmall);
			if (inchars.size() == currentFrameSize) {
				state = IDLE;
				int fsize = currentFrameSize / 2;
				Frame* frame = new Frame(fsize);
				for (int i = 0; i < fsize; i++) {
					frame->data[i] = inchars[i * 2] * 16 + inchars[i * 2 + 1];
				}
				inchars.clear();
				callback(frame, dll);
			}
		}
		break;
	}
	if (state != IDLE)
	{
		//std::cout << std::fixed << std::setw(2) << int(nsmall);
		std::cout << std::fixed;
		for (int i = 0; i < 16; i++) {
			if (i == nsmall) {
				if (state == COLLECTING)
					SetConsoleTextAttribute(hConsole, 10);
				else
					SetConsoleTextAttribute(hConsole, 12);
			}
			else
				SetConsoleTextAttribute(hConsole, 15);
			std::cout << std::setw(3) << std::setprecision(0) << mag[i];
		}
		//std::cout << "              \r";
		std::cout << std::endl;
	}

	return paContinue;

}

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
int IBuffer::paCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	/* Here we cast userData to Sine* type so we can call the instance method paCallbackMethod, we can do that since
	we called Pa_OpenStream with 'this' for userData */
	return ((IBuffer*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
		framesPerBuffer,
		timeInfo,
		statusFlags);
}


void IBuffer::paStreamFinishedMethod()
{
	//printf("Stream Completed: %s\n", message);
}

/*
* This routine is called by portaudio when playback is done.
*/
void IBuffer::paStreamFinished(void* userData)
{
	return ((IBuffer*)userData)->paStreamFinishedMethod();
}


std::vector<float> IBuffer::goertzel_mag() {
	int     k[8];
	float   floatnumSamples;
	float   omega[8], sine[8], cosine[8], coeff[8], q0[8], q1[8], q2[8];
	float  scalingFactor = bufferSize / 2.0;
	std::vector<float> magnitude(8);

	floatnumSamples = (float)bufferSize;
	for (int f = 0; f < 8; f++) {
		k[f] = (int)(0.5 + ((floatnumSamples * DTMF_FREQ[f]) / sampleRate));
		omega[f] = (2.0 * M_PI * k[f]) / floatnumSamples;
		sine[f] = sin(omega[f]);
		cosine[f] = cos(omega[f]);
		coeff[f] = 2.0 * cosine[f];
		q0[f] = 0;
		q1[f] = 0;
		q2[f] = 0;
	}
	for (int i = 0; i<(bufferSize); i++)
	{
		float d = buffer[i];
		for (int f = 0; f < 8; f++) {
			q0[f] = coeff[f] * q1[f] - q2[f] + d;
			q2[f] = q1[f];
			q1[f] = q0[f];
		}
		if (sampleBufferSize * 4 > i)
			buffer[i] = buffer[i + sampleBufferSize];
	}

	// calculate the real and imaginary results
	// scaling appropriately
	for (int f = 0; f < 8; f++) {
		float real = (q1[f] - q2[f] * cosine[f]) / scalingFactor;
		float imag = (q2[f] * sine[f]) / scalingFactor;

		magnitude[f] = sqrt(real*real + imag*imag);
	}
	return magnitude;
}

// separate even/odd elements to lower/upper halves of array respectively.
// Due to Butterfly combinations, this turns out to be the simplest way 
// to get the job done without clobbering the wrong elements.
void IBuffer::separate(std::complex<float>* a, int n) {
	std::complex<float>* b = new std::complex<float>[n / 2];  // get temp heap storage
	for (int i = 0; i<n / 2; i++)    // copy all odd elements to heap storage
		b[i] = a[i * 2 + 1];
	for (int i = 0; i<n / 2; i++)    // copy all even elements to lower-half of a[]
		a[i] = a[i * 2];
	for (int i = 0; i<n / 2; i++)    // copy all odd (from heap) to upper-half of a[]
		a[i + n / 2] = b[i];
	delete[] b;                 // delete heap storage
}

// N must be a power-of-2, or bad things will happen.
// Currently no check for this condition.
//
// N input samples in X[] are FFT'd and results left in X[].
// Because of Nyquist theorem, N samples means 
// only first N/2 FFT results in X[] are the answer.
// (upper half of X[] is a reflection with no new information).
void IBuffer::fft2(std::complex<float>* X, int N) {
	if (N < 2) {
		// bottom of recursion.
		// Do nothing here, because already X[0] = x[0]
	}
	else {
		separate(X, N);      // all evens to lower half, all odds to upper half
		fft2(X, N / 2);   // recurse even items
		fft2(X + N / 2, N / 2);   // recurse odd  items
								  // combine results of two half recursions
		for (int k = 0; k<N / 2; k++) {
			std::complex<float> e = X[k];   // even
			std::complex<float> o = X[k + N / 2];   // odd
													// w is the "twiddle-factor"
			std::complex<float> w = exp(std::complex<float>(0, -2.*M_PI*k / N));
			X[k] = e + w * o;
			X[k + N / 2] = e - w * o;
		}
	}
}