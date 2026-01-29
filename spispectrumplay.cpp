/*
	based on the portaudio live spectrum analyser example
	but largely enhanced and modified

	2015may08, created by stephane.poirier@oifii.org (or spi@oifii.org)
			   from BASS spectrum analyser example
*/
/*
 * Copyright (c) 2012-2026 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 1901 rue Gilford, #53
 * Montreal, QC, H2H 1G8
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
//#include "bass.h"
#include "portaudio.h"

#ifdef WIN32
#if PA_USE_ASIO
#include "pa_asio.h"
#endif
#endif

//2023jan26, spi, begin
/*
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_CHANNELS    (1)
//#define NUM_CHANNELS    (2)

// Select sample format.
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif
*/
#include "defs.h"
#include "spiaudiodevice.h"
#include <vector>
int global_numchannels = NUM_CHANNELS;
SPIAudioDevice mySPIAudioDevice;
//2023jan26, spi, end

#include "resource.h"

#include <string>
#include <map>
using namespace std;

//fourier.h is the addon from audio programming book to the rfftw library,
//see audio programming book page 536 for details. in short, fourier.cpp
//wraps the rfftw by providing 2 functions: fft() and ifft().
//fourier.h also depends on libsndfile so it makes rfftw.lib depends on
//libsndfile for compiling (no need for linking it if you don't use it
//elsewhere).
//fft() function can only be called always with the same sample size N,
//this because within fft() implementation rfftw_create_plan() is called
//only once (the first time fft() is called).
#include <fourier.h> //in rfftw.lib (static library)

#include <ctime> //for random number initialization seed


//2024feb20, spi, begin
int prev_specmode = 0;
//#define SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE	(2*1920*2)	//max 2 channels and 2 screens 1920x1080
//#define SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE	(2*2048*2)	//max 2 channels * max framesperbuffer (should be greater than width of a 4K screen or greater than width of 2 screens of 1920x1080)
#define SPECTRUMWINDOW_MAXNUMCHANNELS		2
//#define SPECTRUMWINDOW_MAXFRAMEBUFFER		(2*2048)
//2 * FRAMES_PER_BUFFER
#define SPECTRUMWINDOW_MAXFRAMESPERBUFFER	(2*FRAMES_PER_BUFFER)
//2 * max num channels * max framesperbuffer * 2 (for 4K displays, has to be greater than width of a 4K screen or greater than width of 2 screens of 1920x1080)
#define SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE	(2*SPECTRUMWINDOW_MAXNUMCHANNELS*SPECTRUMWINDOW_MAXFRAMESPERBUFFER)	

bool audiobuffer_ready = false;
float audiobuffer[SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE];

float buf[SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE];
float buf2[SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE];
float fftbuf[SPECTRUMWINDOW_MAXAUDIOBUFFERSIZE];

//string global_filename = "testwav.wav";
string global_filename = "testbeat2.w64";
SNDFILE* global_pSNDFILE = NULL;
SF_INFO global_SF_INFO;
//2024feb20, spi, end


/*
//#define SPECWIDTH 368	// display width
//#define SPECHEIGHT 127	// height (changing requires palette adjustments too)
#define SPECWIDTH 1000	// display width
#define SPECHEIGHT 64	// height (changing requires palette adjustments too)
*/
int SPECWIDTH=500;	// display width
int SPECHEIGHT=250;	// display height 

BYTE global_alpha=200;

//char global_buffer[1024];
//string global_filename="testwav.wav";
//string global_devicename="";
//map<string,int> global_devicemap;
//int global_deviceid=-1;

//2023jan26, spi, begin
/*
PaStream* global_stream;
PaStreamParameters global_inputParameters;
PaError global_err;
*/
//2023jan26, spi, end
//string global_audiodevicename="";
//int global_inputAudioChannelSelectors[2];
//PaAsioStreamInfo global_asioInputInfo;

FILE* pFILE= NULL;


float global_fSecondsPlay; //negative for always playing, looping
DWORD global_timer=0;
int global_x=200;
int global_y=200;

HWND win=NULL;
DWORD timer=0;

//DWORD chan;
//HRECORD chan;	// recording channel

HDC specdc=0;
HBITMAP specbmp=0;
BYTE *specbuf;

int specmode=0,specpos=0; // spectrum mode (and marker pos for 2nd mode)

//new parameters
string global_classname="SPI-Spectrum-Play";
string global_title="spispectrumplay (click to toggle mode)";
string global_begin="begin.ahk";
string global_end="end.ahk";
int global_idcolorpalette=0;
int global_bands=20;


//2024feb20, spi, begin
bool global_abort = false;

static int renderCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

static int gNumNoInputs = 0;
// This routine will be called by the PortAudio engine when audio is needed.
// It may be called at interrupt level on some machines so don't do anything
// that could mess up the system like calling malloc() or free().
//
static int renderCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	SAMPLE *out = (SAMPLE*)outputBuffer;
	const SAMPLE *in = (const SAMPLE*)inputBuffer;
	unsigned int i;
	(void)timeInfo; // Prevent unused variable warnings.
	(void)statusFlags;
	(void)userData;

	if (global_abort == true) return paAbort;

	if (0)
	{
		//////////////////////////////////
		//for listening to an input signal
		//////////////////////////////////
		if (inputBuffer == NULL)
		{
			//silence the output when no input buffer yet
			/*
			for (i = 0; i < framesPerBuffer; i++)
			{
				*out++ = 0;  // left - silent
				*out++ = 0;  // right - silent
			}
			*/
			memset(outputBuffer, 0, sizeof(float)*framesPerBuffer*NUM_CHANNELS);
			gNumNoInputs += 1;
		}
		else
		{
			//to listen to the input without any processing
			/*
			for (i = 0; i<framesPerBuffer; i++)
			{
				*out++ = *in++;  // left - unprocessed
				*out++ = *in++;  // right - unprocessed
			}
			*/
			memcpy(outputBuffer, inputBuffer, sizeof(float)*framesPerBuffer*NUM_CHANNELS);

			/* //no input ring buffer nor any tonic synth
			global_RingBufferWriter.write((float*)inputBuffer, framesPerBuffer, NUM_CHANNELS);
			global_pSynth->fillBufferOfFloats((float*)outputBuffer, framesPerBuffer, NUM_CHANNELS);
			*/

			/*
			if (0) pthread_mutex_lock(&g_mutex);
			callbackFilledAudioBuffers->acquireAndSeparateNewBuffer((float*)outputBuffer);
			g_ready = true;
			if (0) pthread_mutex_unlock(&g_mutex);
			*/
			/* //these classes are not used in here
			if (global_pSpectrumWindow && global_pSpectrumWindow->IsWindowVisible())
			{
				global_pSpectrumWindow->AcquireAudioBuffer((float*)outputBuffer, framesPerBuffer, NUM_CHANNELS);
			}
			if (global_pSpectrum24bitWindow && global_pSpectrum24bitWindow->IsWindowVisible())
			{
				global_pSpectrum24bitWindow->AcquireAudioBuffer((float*)outputBuffer, framesPerBuffer, NUM_CHANNELS);
			}
			if (global_pTimeframeWindow && global_pTimeframeWindow->IsWindowVisible())
			{
				global_pTimeframeWindow->AcquireAudioBuffer((float*)outputBuffer, framesPerBuffer, NUM_CHANNELS);
			}
			*/
		}
	}
	else
	{
		///////////////////////////
		//for playing an audio file
		///////////////////////////
		//sf_count_t itemsread = sf_read_float(global_pSNDFILE, (float*)buffer, length/4) ; //4 bytes per float
		//sf_count_t itemsread = sf_readf_float(global_pSNDFILE, (float*)buffer, length / 8); //8 bytes per frame
		sf_count_t itemsread = sf_readf_float(global_pSNDFILE, (float*)outputBuffer, framesPerBuffer); 
		if (itemsread == 0) //==0 if end of file
		{
			/*
			if (global_fSecondsPlay <= 0)
			{
				//end of file, stop here
				int returnvalue = itemsread * 8;
				returnvalue |= (int)BASS_STREAMPROC_END; // set indicator flag
				sf_close(global_pSNDFILE);
				global_pSNDFILE = NULL;
				PostMessage(win, WM_DESTROY, 0, 0);
				return returnvalue;
			}
			*/
			//end of file, loop stream
			sf_seek(global_pSNDFILE, 0, SEEK_SET);
			itemsread = sf_readf_float(global_pSNDFILE, (float*)outputBuffer, framesPerBuffer);
		}
	}
	audiobuffer_ready = false;
	memcpy(audiobuffer, outputBuffer, sizeof(float)*framesPerBuffer*NUM_CHANNELS);
	audiobuffer_ready = true;
	return paContinue;
}
//2024feb20, spi, end


/*
bool SelectAudioDevice()
{
	const PaDeviceInfo* deviceInfo;
    int numDevices = Pa_GetDeviceCount();
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
		string devicenamestring = deviceInfo->name;
		global_devicemap.insert(pair<string,int>(devicenamestring,i));
		if(pFILE) fprintf(pFILE,"id=%d, name=%s\n", i, devicenamestring.c_str());
	}

	int deviceid = Pa_GetDefaultInputDevice(); // default input device 
	map<string,int>::iterator it;
	it = global_devicemap.find(global_audiodevicename);
	if(it!=global_devicemap.end())
	{
		deviceid = (*it).second;
		//printf("%s maps to %d\n", global_audiodevicename.c_str(), deviceid);
		deviceInfo = Pa_GetDeviceInfo(deviceid);
		//assert(inputAudioChannelSelectors[0]<deviceInfo->maxInputChannels);
		//assert(inputAudioChannelSelectors[1]<deviceInfo->maxInputChannels);
	}
	else
	{
		//Pa_Terminate();
		//return -1;
		//printf("error, audio device not found, will use default\n");
		MessageBox(win,"error, audio device not found, will use default\n",0,0);
		deviceid = Pa_GetDefaultInputDevice();
	}


	global_inputParameters.device = deviceid; 
	if (global_inputParameters.device == paNoDevice) 
	{
		MessageBox(win,"error, no default input device.\n",0,0);
		return false;
	}
	//global_inputParameters.channelCount = 2;
	global_inputParameters.channelCount = NUM_CHANNELS;
	global_inputParameters.sampleFormat =  PA_SAMPLE_TYPE;
	global_inputParameters.suggestedLatency = Pa_GetDeviceInfo( global_inputParameters.device )->defaultLowOutputLatency;
	//inputParameters.hostApiSpecificStreamInfo = NULL;

	//Use an ASIO specific structure. WARNING - this is not portable. 
	//PaAsioStreamInfo asioInputInfo;
	global_asioInputInfo.size = sizeof(PaAsioStreamInfo);
	global_asioInputInfo.hostApiType = paASIO;
	global_asioInputInfo.version = 1;
	global_asioInputInfo.flags = paAsioUseChannelSelectors;
	global_asioInputInfo.channelSelectors = global_inputAudioChannelSelectors;
	if(deviceid==Pa_GetDefaultInputDevice())
	{
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paASIO) 
	{
		global_inputParameters.hostApiSpecificStreamInfo = &global_asioInputInfo;
	}
	else if(Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paWDMKS) 
	{
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else
	{
		//assert(false);
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	return true;
}
*/


/*
void CALLBACK StopPlayingFile(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	PostMessage(win, WM_DESTROY, 0, 0);
}
*/
void CALLBACK StopPlaying(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	PostMessage(win, WM_DESTROY, 0, 0);
}

/*
// display error messages
void Error(const char *es)
{
	char mes[200];
	sprintf(mes,"%s\n(error code: %d)",es,BASS_ErrorGetCode());
	MessageBox(win,mes,0,0);
}
*/

/*
BOOL PlayFile(const char* filename)
{
	if (!(chan=BASS_StreamCreateFile(FALSE,filename,0,0,BASS_SAMPLE_LOOP))
		&& !(chan=BASS_MusicLoad(FALSE,filename,0,0,BASS_MUSIC_RAMP|BASS_SAMPLE_LOOP,1))) 
	{
		Error("Can't play file");
		return FALSE; // Can't load the file
	}
	if(global_fSecondsRecord<=0)
	{
		QWORD length_byte=BASS_ChannelGetLength(chan,BASS_POS_BYTE);
		global_fSecondsPlay=BASS_ChannelBytes2Seconds(chan,length_byte);
	}
	global_timer=timeSetEvent(global_fSecondsPlay*1000,25,(LPTIMECALLBACK)&StopPlayingFile,0,TIME_ONESHOT);

	BASS_ChannelPlay(chan,FALSE);
	return TRUE;
}
*/

/*
// select a file to play, and play it
BOOL PlayFile()
{
	char file[MAX_PATH]="";
	OPENFILENAME ofn={0};
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=win;
	ofn.nMaxFile=MAX_PATH;
	ofn.lpstrFile=file;
	ofn.Flags=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_EXPLORER;
	ofn.lpstrTitle="Select a file to play";
	ofn.lpstrFilter="playable files\0*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp3;*.mp2;*.mp1;*.ogg;*.wav;*.aif\0All files\0*.*\0\0";
	if (!GetOpenFileName(&ofn)) return FALSE;
	
	return PlayFile(file);
}
*/

//2024feb21, spi, begin
int getv(int &x, int &c, int framesperbuffer, int numchannels) //int getv(int &x, int &c, int &framesperbuffer, int &numchannels)
{
	/*
	int v = (1 - _buf[x*numchannels + c])*_SPECHEIGHT / 2; // invert and scale to fit display //2021nov29, spi, was //int v = (1 - _buf[x*NUM_CHANNELS + c])*_SPECHEIGHT / 2; // invert and scale to fit display
	if (v<0) v = 0;
	else if (v >= _SPECHEIGHT) v = _SPECHEIGHT - 1;
	*/
	int v = 0;
	if (framesperbuffer < SPECWIDTH)
	{
		int x_inframes = (int)((float)x * ((float)framesperbuffer) / ((float)SPECWIDTH));
		v = (1 - buf[x_inframes*numchannels + c])*SPECHEIGHT / 2;
	}
	else
	{
		v = (1 - buf[x*numchannels + c])*SPECHEIGHT / 2;
	}
	if (v < 0) v = 0;
	else if (v >= SPECHEIGHT) v = SPECHEIGHT - 1;
	return v;
}
//2024feb21, spi, end

// update the spectrum display - the interesting bit :)
void CALLBACK UpdateSpectrum(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	HDC dc;
	int x,y,y1;

	//2024feb20, spi, begin
	//overriding user-specified global_bands
	if (1)
	{
		global_bands = SPECWIDTH / 16; //proportional, 120 bands for 2K display, 240 bands for 4K display, etc.
	}
	else
	{
		global_bands = 1920 / 16; //constant, 16 pixels per band, 120 bands on any display
	}
	if (global_bands < 4) global_bands = 4;
	if (global_bands > (FRAMES_PER_BUFFER - 2)) global_bands = (FRAMES_PER_BUFFER - 2);
	//2024feb20, spi, end

	if (specmode==3 || specmode==4 || specmode==5 || specmode==6 ||
		specmode==7 || specmode==8 || specmode==9 || specmode==10 ||
		specmode==11 || specmode==12 || specmode==13 || specmode==14 || 
		specmode==15 || specmode==16 || specmode==17 || specmode==18) 
	{ // waveform and filled waveform
		int c;
		//2024feb20, spi, begin
		//float *buf;
		//2024feb20, spi, end
		//BASS_CHANNELINFO ci;
		
		//black background
		if(specmode==3 || specmode==4 || specmode==5 || specmode==6)
		{
			memset(specbuf,0,SPECWIDTH*SPECHEIGHT);
		}
		//noisy background
		else if(specmode==7 || specmode==8 || specmode==9 || specmode==10)
		{	
			for(int i=0; i<SPECWIDTH; i++)
			{
				for (int j=0; j<SPECHEIGHT; j++)
				{
					
					int random_integer;
					//int lowest=1, highest=127;
					int lowest=64, highest=96; //good
					int range=(highest-lowest)+1;
					random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
					specbuf[j*SPECWIDTH+i]=random_integer;
				}
			}
		}
		//solid slightly shifting background
		else if(specmode==11 || specmode==12 || specmode==13 || specmode==14)
		{
			int random_integer;
			//int lowest=1, highest=127;
			//int lowest=1, highest=255;
			int lowest=64, highest=96; //good
			int range=(highest-lowest)+1;
			random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
			memset(specbuf,random_integer,SPECWIDTH*SPECHEIGHT);
		}
		//solid radically shifting background
		else //specmode==15 || specmode==16 || specmode==17 || specmode==18
		{
			int random_integer;
			//int lowest=1, highest=127;
			int lowest=1, highest=255;
			//int lowest=64, highest=96; //good
			int range=(highest-lowest)+1;
			random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
			memset(specbuf,random_integer,SPECWIDTH*SPECHEIGHT);
		}

		//BASS_ChannelGetInfo(chan,&ci); // get number of channels
		//buf=(float*)alloca(ci.chans*SPECWIDTH*sizeof(float)); // allocate buffer for data
		//2024feb20, spi, begin
		//buf=(float*)alloca(NUM_CHANNELS*SPECWIDTH*sizeof(float)); // allocate buffer for data
		//2024feb20, spi, end
		//BASS_ChannelGetData(chan,buf,(ci.chans*SPECWIDTH*sizeof(float))|BASS_DATA_FLOAT); // get the sample data (floating-point to avoid 8 & 16 bit processing)

		//2024feb20, spi, begin
		/*
        //global_err = Pa_ReadStream( global_stream, buf, FRAMES_PER_BUFFER );
		mySPIAudioDevice.global_err = Pa_ReadStream(mySPIAudioDevice.global_stream, buf, NUM_CHANNELS*SPECWIDTH );
        if(mySPIAudioDevice.global_err != paNoError )
		{
			//char errorbuf[2048];
			//sprintf(errorbuf, "Error reading stream: %s\n", Pa_GetErrorText(global_err));
			//MessageBox(0,errorbuf,0,MB_ICONERROR);
			return;
		}
		*/
		while (!audiobuffer_ready) Sleep(1);
		memcpy(buf, audiobuffer, sizeof(float)*NUM_CHANNELS*FRAMES_PER_BUFFER); //we have
		//memcpy(buf, _audiobuffer, sizeof(float)*NUM_CHANNELS*SPECWIDTH); //we need
		//2024feb20, spi, end

		//under waveform filled down to bottom
		if (specmode==6 || specmode==10 || specmode==14 || specmode==18)
		{
			for (c=0;c<NUM_CHANNELS;c++) 
			{
				for (x=0;x<SPECWIDTH;x++) 
				{
					//2024feb21, spi, begin
					/*
					int v=(1-buf[x*NUM_CHANNELS+c])*SPECHEIGHT/2; // invert and scale to fit display
					if (v<0) v=0;
					else if (v>=SPECHEIGHT) v=SPECHEIGHT-1;
					*/
					// invert and scale to fit display
					int v = getv(x, c, FRAMES_PER_BUFFER, NUM_CHANNELS);
					//2024feb21, spi, end
					//under waveform filled down to bottom
					y=v;
					while(--y>=0) specbuf[y*SPECWIDTH+x]=c&1?127:1; 
				}
			}
		}
		//waveform filled towards center
		else if (specmode==5 || specmode==9 || specmode==13 || specmode==17)
		{
			for (c=0;c<NUM_CHANNELS;c++) 
			{
				for (x=0;x<SPECWIDTH;x++) 
				{
					//2024feb21, spi, begin
					/*
					int v=(1-buf[x*NUM_CHANNELS+c])*SPECHEIGHT/2; // invert and scale to fit display
					if (v<0) v=0;
					else if (v>=SPECHEIGHT) v=SPECHEIGHT-1;
					*/
					// invert and scale to fit display
					int v = getv(x, c, FRAMES_PER_BUFFER, NUM_CHANNELS);
					//2024feb21, spi, end
					//waveform filled towards center
					y=v;
					if(y>(SPECHEIGHT/2))
						while(--y>=(SPECHEIGHT/2)) specbuf[y*SPECWIDTH+x]=c&1?127:1; 
					else if(y<(SPECHEIGHT/2))
						while(++y<=(SPECHEIGHT/2)) specbuf[y*SPECWIDTH+x]=c&1?127:1; 
					else specbuf[y*SPECWIDTH+x]=c&1?127:1; 
				}
			}
		}
		//waveform filled towards opposite
		else if (specmode==4 || specmode==8 || specmode==12 || specmode==16)
		{
			for (c=0;c<NUM_CHANNELS;c++) 
			{
				for (x=0;x<SPECWIDTH;x++) 
				{
					//2024feb21, spi, begin
					/*
					int v=(1-buf[x*NUM_CHANNELS+c])*SPECHEIGHT/2; // invert and scale to fit display
					if (v<0) v=0;
					else if (v>=SPECHEIGHT) v=SPECHEIGHT-1;
					*/
					// invert and scale to fit display
					int v = getv(x, c, FRAMES_PER_BUFFER, NUM_CHANNELS);
					//2024feb21, spi, end
					//waveform filled towards opposite
					y=v;
					if(y>(SPECHEIGHT/2))
						while(--y>=(SPECHEIGHT/2-(v-(SPECHEIGHT/2)))) specbuf[y*SPECWIDTH+x]=c&1?127:1; 
					else if(y<(SPECHEIGHT/2))
						while(++y<=(SPECHEIGHT/2+((SPECHEIGHT/2)-v))) specbuf[y*SPECWIDTH+x]=c&1?127:1; 
					else specbuf[y*SPECWIDTH+x]=c&1?127:1; 
				}
			}
		}
		//waveform (original)
		else if(specmode==3 || specmode==7 || specmode==11 || specmode==15)
		{
			for (c=0;c<NUM_CHANNELS;c++) 
			{
				for (x=0;x<SPECWIDTH;x++) 
				{
					//2024feb21, spi, begin
					/*
					int v=(1-buf[x*NUM_CHANNELS+c])*SPECHEIGHT/2; // invert and scale to fit display
					if (v<0) v=0;
					else if (v>=SPECHEIGHT) v=SPECHEIGHT-1;
					*/
					// invert and scale to fit display
					int v = getv(x, c, FRAMES_PER_BUFFER, NUM_CHANNELS);
					//2024feb21, spi, end
					if (!x) y=v;
					do 
					{ // draw line from previous sample...
						if (y<v) y++;
						else if (y>v) y--;
						specbuf[y*SPECWIDTH+x]=c&1?127:1; // left=green, right=red (could add more colours to palette for more chans)
					} while (y!=v);
				}
			}
		}
	} 
	else 
	{
		//2024feb20, spi, begin
		//float fftbuf[1024];
		//2024feb20, spi, end
		//BASS_ChannelGetData(chan,fft,BASS_DATA_FFT2048); // get the FFT data
		//2024feb20, spi, begin
		//float *buf2;
		//2024feb20, spi, end
		int numberofsamples = FRAMES_PER_BUFFER; // =SPECWIDTH;
		//if(numberofsamples<1024) numberofsamples=1024;
		//2024feb20, spi, begin
		//buf2=(float*)alloca(NUM_CHANNELS*numberofsamples*sizeof(float)); // allocate buffer for data
		/*
		mySPIAudioDevice.global_err = Pa_ReadStream(mySPIAudioDevice.global_stream, buf2, NUM_CHANNELS*numberofsamples );
        if(mySPIAudioDevice.global_err != paNoError )
		{
			//char errorbuf[2048];
			//sprintf(errorbuf, "Error reading stream (2): %s\n", Pa_GetErrorText(global_err));
			//MessageBox(0,errorbuf,0,MB_ICONERROR);
			return;
		}
		*/
		while (!audiobuffer_ready) Sleep(1);
		memcpy(buf2, audiobuffer, sizeof(float)*NUM_CHANNELS*FRAMES_PER_BUFFER); //we have
		//2024feb20, spi, end
		/*
		for(int i=0; i<1024; i++)
		{
			fftbuf[i]=0.0f;
			//fftbuf[i]=abs(buf2[i]);
		}
		*/
		
		fft(buf2, fftbuf, numberofsamples); // 1024);
		for(int i=0; i< numberofsamples; i++) //1024; i++)
		{
			fftbuf[i]=abs(fftbuf[i]);
		}

		if (!specmode) 
		{ // "normal" FFT
			memset(specbuf,0,SPECWIDTH*SPECHEIGHT);
			for (x=0;x<SPECWIDTH/2;x++) 
			{
				/*
#if 1
				y=sqrt(fftbuf[x+1])*3*SPECHEIGHT-4; // scale it (sqrt to make low values more visible)
#else
				y=fftbuf[x+1]*10*SPECHEIGHT; // scale it (linearly)
#endif
				*/
				//2024feb20, spi, begin
				int x_infft = 0;
				if (numberofsamples < SPECWIDTH)
				{
					x_infft = 1 + (int)((float)x *  ((float)numberofsamples) / ((float)SPECWIDTH));
				}
				else
				{
					x_infft = 1 + x;
				}
				/*
				#if 1
					y = sqrt(_fftbuf[x + 1]) * 3 * _SPECHEIGHT - 4; // scale it (sqrt to make low values more visible)
				#else
					y = _fftbuf[x + 1] * 10 * _SPECHEIGHT; // scale it (linearly)
				#endif
				*/
				if (1)
				{
					y = sqrt(fftbuf[x_infft]) * 3 * SPECHEIGHT - 4; // scale it (sqrt to make low values more visible)
				}
				else
				{
					y = fftbuf[x_infft] * 10 * SPECHEIGHT; // scale it (linearly)
				}
				//2024feb20, spi, end

				if (y>SPECHEIGHT) y=SPECHEIGHT; // cap it
				if (x && (y1=(y+y1)/2)) // interpolate from previous to make the display smoother
					//while (--y1>=0) specbuf[y1*SPECWIDTH+x*2-1]=y1+1;
					while (--y1>=0) specbuf[y1*SPECWIDTH+x*2-1]=(127*y1/SPECHEIGHT)+1;
				y1=y;
				//while (--y>=0) specbuf[y*SPECWIDTH+x*2]=y+1; // draw level
				while (--y>=0) specbuf[y*SPECWIDTH+x*2]=(127*y/SPECHEIGHT)+1; // draw level
			}
		} 
		else if (specmode==1) 
		{ // logarithmic, acumulate & average bins
			int b0=0;
			memset(specbuf,0,SPECWIDTH*SPECHEIGHT);
//#define BANDS 28
//#define BANDS 80
//#define BANDS 12
			//2024feb20, spi, begin
			if (global_bands > 2 && global_bands < SPECWIDTH)
			{
			//2024feb20, spi, end

				for (x = 0; x < global_bands; x++)
				{
					float peak = 0;
					int b1 = pow(2, x*10.0 / (global_bands - 1));
					//2024feb20, spi, begin
					//if (b1 > 1023) b1 = 1023;
					if (b1 > (numberofsamples - 1)) b1 = (numberofsamples - 1);
					//2024feb20, spi, end
					if (b1 <= b0) b1 = b0 + 1; // make sure it uses at least 1 FFT bin
					for (; b0 < b1; b0++)
					{
					
						//find bin peak
						//2024feb20, spi, begin
						int b_infft = 0;
						if (numberofsamples < SPECWIDTH)
						{
							b_infft = 1 + (int)((float)b0 * ((float)numberofsamples) / ((float)SPECWIDTH));
						}
						else
						{
							b_infft = 1 + b0;
						}
						//if (peak < _fftbuf[1 + b0]) peak = _fftbuf[1 + b0];
						if (peak < fftbuf[b_infft]) peak = fftbuf[b_infft];
						//2024feb20, spi, end

					}
					y = sqrt(peak) * 3 * SPECHEIGHT - 4; // scale it (sqrt to make low values more visible)
					if (y > SPECHEIGHT) y = SPECHEIGHT; // cap it
					while (--y >= 0)
					{
						//memset(specbuf+y*SPECWIDTH+x*(SPECWIDTH/global_bands),y+1,SPECWIDTH/global_bands-2); // draw bar
						memset(specbuf + y * SPECWIDTH + x * (SPECWIDTH / global_bands), (127 * y / SPECHEIGHT) + 1, SPECWIDTH / global_bands - 2); // draw bar
					}
				}
			//2024feb20, spi, begin
			}
			//2024feb20, spi, end
		} 
		else 
		{ 
			// "3D"
			//2024feb20, spi, begin
			if (prev_specmode != specmode)
			{
				memset(specbuf, 0, SPECWIDTH*SPECHEIGHT);
			}
			//2024feb20, spi, end

			for (x=0;x<SPECHEIGHT;x++) 
			{
				//2024feb20, spi, begin
				//y = sqrt(_fftbuf[x + 1]) * 3 * 127; // scale it (sqrt to make low values more visible)
				int x_infft = 0;
				if (numberofsamples < SPECHEIGHT)
				{
					x_infft = 1 + (int)((float)x * ((float)numberofsamples) / ((float)SPECHEIGHT));
				}
				else
				{
					x_infft = 1 + x;
				}
				y = sqrt(fftbuf[x_infft]) * 3 * 127; // scale it (sqrt to make low values more visible)
				//2024feb20, spi, end
				if (y>127) y=127; // cap it
				specbuf[x*SPECWIDTH+specpos]=128+y; // plot it
			}
			// move marker onto next position
			specpos=(specpos+1)%SPECWIDTH;
			for (x=0;x<SPECHEIGHT;x++) specbuf[x*SPECWIDTH+specpos]=255; //draws marker
		}
	}

	// update the display
	dc=GetDC(win);
	BitBlt(dc,0,0,SPECWIDTH,SPECHEIGHT,specdc,0,0,SRCCOPY);
	ReleaseDC(win,dc);
	//2024feb20, spi, begin
	prev_specmode = specmode;
	//2024feb20, spi, end

}

/*
// Recording callback - not doing anything with the data
BOOL CALLBACK DuffRecording(HRECORD handle, const void *buffer, DWORD length, void *user)
{
	return TRUE; // continue recording
}
*/

// window procedure
long FAR PASCAL SpectrumWindowProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m) {
		case WM_PAINT:
			if (GetUpdateRect(h,0,0)) {
				PAINTSTRUCT p;
				HDC dc;
				if (!(dc=BeginPaint(h,&p))) return 0;
				BitBlt(dc,0,0,SPECWIDTH,SPECHEIGHT,specdc,0,0,SRCCOPY);
				EndPaint(h,&p);
			}
			return 0;

		case WM_LBUTTONUP:
			//specmode=(specmode+1)%4; // swap spectrum mode
			specmode=(specmode+1)%19; // swap spectrum mode
			memset(specbuf,0,SPECWIDTH*SPECHEIGHT);	// clear display
			return 0;

		case WM_RBUTTONUP:
			specmode=(specmode-1); // swap spectrum mode
			if(specmode<0) specmode = 19-1;
			memset(specbuf,0,SPECWIDTH*SPECHEIGHT);	// clear display
			return 0;

		case WM_CREATE:
			win=h;
			//spi, avril 2015, begin
			SetWindowLong(h, GWL_EXSTYLE, GetWindowLong(h, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(h, 0, global_alpha, LWA_ALPHA);
			//SetLayeredWindowAttributes(h, 0, 200, LWA_ALPHA);
			//spi, avril 2015, end
			/*
			// initialize BASS
			if (!BASS_Init(-1,44100,0,win,NULL)) {
				Error("Can't initialize device");
				return -1;
			}
			//if (!PlayFile()) { // start a file playing
			if (!PlayFile(global_filename.c_str())) { // start a file playing
				BASS_Free();
				return -1;
			}
			*/

			/*
			// initialize BASS recording
			//if (!BASS_RecordInit(-1)) { //default device
			//if (!BASS_RecordInit(0)) { //device 0
			//if (!BASS_RecordInit(1)) { //device 1
			if (!BASS_RecordInit(global_deviceid)) {
				Error("Can't initialize device");
				return -1;
			}
			// start recording (44100hz mono 16-bit)
			if (!(chan=BASS_RecordStart(44100,1,0,&DuffRecording,0))) {
				Error("Can't start recording");
				return -1;
			}
			*/
			if(global_fSecondsPlay > 0)
			{
				global_timer=timeSetEvent(global_fSecondsPlay *1000,100,(LPTIMECALLBACK)&StopPlaying,0,TIME_ONESHOT);
			}
			{ // create bitmap to draw spectrum in (8 bit for easy updating)
				BYTE data[2000]={0};
				BITMAPINFOHEADER *bh=(BITMAPINFOHEADER*)data;
				RGBQUAD *pal=(RGBQUAD*)(data+sizeof(*bh));
				int a;
				bh->biSize=sizeof(*bh);
				bh->biWidth=SPECWIDTH;
				bh->biHeight=SPECHEIGHT; // upside down (line 0=bottom)
				bh->biPlanes=1;
				bh->biBitCount=8;
				bh->biClrUsed=bh->biClrImportant=256;
				// setup palette
				
				if(global_idcolorpalette==0)
				{
					//original palette, green shifting to red
					for (a=1;a<128;a++) {
						pal[a].rgbGreen=256-2*a;
						pal[a].rgbRed=2*a;
					}
					for (a=0;a<32;a++) {
						pal[128+a].rgbBlue=8*a;
						pal[128+32+a].rgbBlue=255;
						pal[128+32+a].rgbRed=8*a;
						pal[128+64+a].rgbRed=255;
						pal[128+64+a].rgbBlue=8*(31-a);
						pal[128+64+a].rgbGreen=8*a;
						pal[128+96+a].rgbRed=255;
						pal[128+96+a].rgbGreen=255;
						pal[128+96+a].rgbBlue=8*a;
					}
				}
				else if(global_idcolorpalette==1)
				{
					//altered palette, red shifting to green
					for (a=1;a<128;a++) {
						pal[a].rgbRed=256-2*a;
						pal[a].rgbGreen=2*a;
					}
					for (a=0;a<32;a++) {
						pal[128+a].rgbBlue=8*a;
						pal[128+32+a].rgbBlue=255;
						pal[128+32+a].rgbGreen=8*a;
						pal[128+64+a].rgbGreen=255;
						pal[128+64+a].rgbBlue=8*(31-a);
						pal[128+64+a].rgbRed=8*a;
						pal[128+96+a].rgbGreen=255;
						pal[128+96+a].rgbRed=255;
						pal[128+96+a].rgbBlue=8*a;
					}
				}
				else if(global_idcolorpalette==2)
				{
					//altered palette, blue shifting to green
					for (a=1;a<128;a++) {
						pal[a].rgbBlue=256-2*a;
						pal[a].rgbGreen=2*a;
					}
					for (a=0;a<32;a++) {
						pal[128+a].rgbBlue=8*a;
						pal[128+32+a].rgbRed=255;
						pal[128+32+a].rgbGreen=8*a;
						pal[128+64+a].rgbGreen=255;
						pal[128+64+a].rgbRed=8*(31-a);
						pal[128+64+a].rgbBlue=8*a;
						pal[128+96+a].rgbGreen=255;
						pal[128+96+a].rgbBlue=255;
						pal[128+96+a].rgbRed=8*a;
					}
				}
				else if(global_idcolorpalette==3)
				{
					//altered palette, black shifting to white - grascale
					for (a=1;a<256;a++) {
						pal[a].rgbRed=a;
						pal[a].rgbBlue=a;
						pal[a].rgbGreen=a;
					}
				}
				else if(global_idcolorpalette==4)
				{
					//altered palette, pink
					for (a=1;a<256;a++) {
						pal[a].rgbRed=255;
						pal[a].rgbBlue=255;
						pal[a].rgbGreen=a;
					}
				}
				else if(global_idcolorpalette==5)
				{
					//altered palette, yellow
					for (a=1;a<256;a++) {
						pal[a].rgbRed=255;
						pal[a].rgbBlue=a;
						pal[a].rgbGreen=255;
					}
				}
				else if(global_idcolorpalette==6)
				{
					//altered palette, cyan
					for (a=1;a<256;a++) {
						pal[a].rgbRed=a;
						pal[a].rgbBlue=255;
						pal[a].rgbGreen=255;
					}
				}
				else if(global_idcolorpalette==7)
				{
					//altered palette, lite green
					for (a=1;a<256;a++) {
						pal[a].rgbRed=a;
						pal[a].rgbBlue=127;
						pal[a].rgbGreen=255;
					}
				}


				// create the bitmap
				specbmp=CreateDIBSection(0,(BITMAPINFO*)bh,DIB_RGB_COLORS,(void**)&specbuf,NULL,0);
				specdc=CreateCompatibleDC(0);
				SelectObject(specdc,specbmp);
			}
			// setup update timer (40hz)
			timer=timeSetEvent(25,25,(LPTIMECALLBACK)&UpdateSpectrum,0,TIME_PERIODIC);
			//timer=timeSetEvent(10,25,(LPTIMECALLBACK)&UpdateSpectrum,0,TIME_PERIODIC);
			//timer=timeSetEvent(100,100,(LPTIMECALLBACK)&UpdateSpectrum,0,TIME_PERIODIC);
			break;

		case WM_DESTROY:
			{
				if (timer) timeKillEvent(timer);
				if (global_timer) timeKillEvent(global_timer);
				//BASS_Free();
				//BASS_RecordFree();
				Sleep(100);
				mySPIAudioDevice.global_err = Pa_StopStream(mySPIAudioDevice.global_stream );
				if(mySPIAudioDevice.global_err != paNoError )
				{
					char errorbuf[2048];
					sprintf(errorbuf, "Error stoping stream: %s\n", Pa_GetErrorText(mySPIAudioDevice.global_err));
					MessageBox(0,errorbuf,0,MB_ICONERROR);
					return 1;
				}
				mySPIAudioDevice.global_err = Pa_CloseStream(mySPIAudioDevice.global_stream );
				if(mySPIAudioDevice.global_err != paNoError )
				{
					char errorbuf[2048];
					sprintf(errorbuf, "Error closing stream: %s\n", Pa_GetErrorText(mySPIAudioDevice.global_err));
					MessageBox(0,errorbuf,0,MB_ICONERROR);
					return 1;
				}
				Pa_Terminate();
				if (specdc) DeleteDC(specdc);
				if (specbmp) DeleteObject(specbmp);

				//2024feb20, spi, begin
				sf_close(global_pSNDFILE);
				global_pSNDFILE = NULL;
				//2024feb20, spi, end
				int nShowCmd = false;
				//ShellExecuteA(NULL, "open", "end.bat", "", NULL, nShowCmd);
				//ShellExecuteA(NULL, "open", "end.ahk", "", NULL, nShowCmd);
				if (!global_end.empty()) ShellExecuteA(NULL, "open", global_end.c_str(), "", NULL, nShowCmd);
				
				PostQuitMessage(0);
			}
			break;
	}
	return DefWindowProc(h, m, w, l);
}

PCHAR*
    CommandLineToArgvA(
        PCHAR CmdLine,
        int* _argc
        )
    {
        PCHAR* argv;
        PCHAR  _argv;
        ULONG   len;
        ULONG   argc;
        CHAR   a;
        ULONG   i, j;

        BOOLEAN  in_QM;
        BOOLEAN  in_TEXT;
        BOOLEAN  in_SPACE;

        len = strlen(CmdLine);
        i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

        argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
            i + (len+2)*sizeof(CHAR));

        _argv = (PCHAR)(((PUCHAR)argv)+i);

        argc = 0;
        argv[argc] = _argv;
        in_QM = FALSE;
        in_TEXT = FALSE;
        in_SPACE = TRUE;
        i = 0;
        j = 0;

        while( a = CmdLine[i] ) {
            if(in_QM) {
                if(a == '\"') {
                    in_QM = FALSE;
                } else {
                    _argv[j] = a;
                    j++;
                }
            } else {
                switch(a) {
                case '\"':
                    in_QM = TRUE;
                    in_TEXT = TRUE;
                    if(in_SPACE) {
                        argv[argc] = _argv+j;
                        argc++;
                    }
                    in_SPACE = FALSE;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    if(in_TEXT) {
                        _argv[j] = '\0';
                        j++;
                    }
                    in_TEXT = FALSE;
                    in_SPACE = TRUE;
                    break;
                default:
                    in_TEXT = TRUE;
                    if(in_SPACE) {
                        argv[argc] = _argv+j;
                        argc++;
                    }
                    _argv[j] = a;
                    j++;
                    in_SPACE = FALSE;
                    break;
                }
            }
            i++;
        }
        _argv[j] = '\0';
        argv[argc] = NULL;

        (*_argc) = argc;
        return argv;
    }

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	int nShowCmd = false;

	//LPWSTR *szArgList;
	LPSTR *szArgList;
	int argCount;
	//szArgList = CommandLineToArgvW(GetCommandLineW(), &argCount);
	szArgList = CommandLineToArgvA(GetCommandLine(), &argCount);
	if (szArgList == NULL)
	{
		MessageBox(NULL, "Unable to parse command line", "Error", MB_OK);
		return 10;
	}
	
	global_filename = "testwav.wav";
	if (argCount > 1)
	{
		global_filename = szArgList[1];
	}
	//global_audiodevicename="E-MU ASIO"; //"Wave (2- E-MU E-DSP Audio Proce"
	mySPIAudioDevice.global_audiooutputdevicename = "E-MU ASIO"; //"Wave (2- E-MU E-DSP Audio Proce"
	if(argCount>2)
	{
		mySPIAudioDevice.global_audiooutputdevicename = szArgList[2];
	}
	mySPIAudioDevice.global_outputAudioChannelSelectors[0] = 0; // on emu patchmix ASIO device channel 1 (left)
	mySPIAudioDevice.global_outputAudioChannelSelectors[1] = 1; // on emu patchmix ASIO device channel 2 (right)
	if(argCount>3)
	{
		mySPIAudioDevice.global_outputAudioChannelSelectors[0] = atoi((LPCSTR)(szArgList[3])); //0 for first asio channel (left) or 2, 4, 6, etc.
	}
	if(argCount>4)
	{
		mySPIAudioDevice.global_outputAudioChannelSelectors[1] = atoi((LPCSTR)(szArgList[4])); //1 for second asio channel (right) or 3, 5, 7, etc.
	}
	//2024feb20, spi, begin
	/* //finally removed
	//temporary patch to listen to the input signal
	mySPIAudioDevice.global_audioinputdevicename = mySPIAudioDevice.global_audiooutputdevicename; // "E-MU ASIO"; //"Wave (2- E-MU E-DSP Audio Proce"
	mySPIAudioDevice.global_inputAudioChannelSelectors[0] = 0; // on emu patchmix ASIO device channel 1 (left)
	mySPIAudioDevice.global_inputAudioChannelSelectors[1] = 1; // on emu patchmix ASIO device channel 2 (right)
	*/
	//2024feb20, spi, end
	global_fSecondsPlay = -1.0; //negative for always playing
	if(argCount>5)
	{
		global_fSecondsPlay = atof((LPCSTR)(szArgList[5]));
	}
	if(argCount>6)
	{
		global_x = atoi((LPCSTR)(szArgList[6]));
	}
	if(argCount>7)
	{
		global_y = atoi((LPCSTR)(szArgList[7]));
	}
	if(argCount>8)
	{
		specmode = atoi((LPCSTR)(szArgList[8]));
	}
	if(argCount>9)
	{
		global_classname = szArgList[9]; 
	}
	if(argCount>10)
	{
		global_title = szArgList[10]; 
	}
	if(argCount>11)
	{
		global_begin = szArgList[11]; 
	}
	if(argCount>12)
	{
		global_end = szArgList[12]; 
	}
	if(argCount>13)
	{
		global_idcolorpalette = atoi((LPCSTR)(szArgList[13]));
	}
	if(argCount>14)
	{
		global_bands = atoi((LPCSTR)(szArgList[14]));
	}
	if(argCount>15)
	{
		SPECWIDTH = atoi((LPCSTR)(szArgList[15]));
	}
	if(argCount>16)
	{
		SPECHEIGHT = atoi((LPCSTR)(szArgList[16]));
	}
	if(argCount>17)
	{
		global_alpha = atoi(szArgList[17]);
	}
	LocalFree(szArgList);

	/*
	//map devicename onto id
	BASS_DEVICEINFO info;
	for (int iii=0; BASS_RecordGetDeviceInfo(iii, &info); iii++)
	{
		string devicenamestring = info.name;
		global_devicemap.insert(pair<string,int>(devicenamestring,iii));
	}
	map<string,int>::iterator it;
	it = global_devicemap.find(global_audiodevicename);
	if(it!=global_devicemap.end())
	{
		global_deviceid = (*it).second;
	}
	else
	{
		global_deviceid = -1;
	}
	*/
	//2024feb20, spi, begin
	if (!(global_pSNDFILE = sf_open(global_filename.c_str(), SFM_READ, &global_SF_INFO)))
	{
		char errorbuf[2048];
		//sprintf(errorbuf, "Error stoping stream: %s\n", Pa_GetErrorText(mySPIAudioDevice.global_err));
		sprintf(errorbuf, "Error, Can't open file\n");
		MessageBox(0, errorbuf, 0, MB_ICONERROR);
		return 1; // Can't load the file
	}
	//2024feb20, spi, end

	//ShellExecuteA(NULL, "open", "begin.bat", "", NULL, nShowCmd);
	//ShellExecuteA(NULL, "open", "begin.ahk", "", NULL, nShowCmd);
	if(!global_begin.empty()) ShellExecuteA(NULL, "open", global_begin.c_str(), "", NULL, nShowCmd);

	//////////////////////////
	//initialize random number
	//////////////////////////
	srand((unsigned)time(0));

	///////////////////////
	//initialize port audio
	///////////////////////
	mySPIAudioDevice.global_err = Pa_Initialize();
    if(mySPIAudioDevice.global_err != paNoError )
	{
		MessageBox(0,"portaudio initialization failed",0,MB_ICONERROR);
		return 1;
	}

	////////////////////////
	//audio device selection
	////////////////////////
	pFILE = fopen("devices.txt","w");
	//2023jan26, spi, begin
	mySPIAudioDevice.m_pFILE = pFILE;
	
	//SelectAudioDevice();
	//mySPIAudioDevice.SelectAudioInputDevice();
	//2023jan26, spi, end	
	//2024feb20, spi, begin
	mySPIAudioDevice.SelectAudioOutputDevice();
	//2024feb20, spi, end
	fclose(mySPIAudioDevice.m_pFILE);
	mySPIAudioDevice.m_pFILE =NULL;

	//////////////
    //setup stream  
	//////////////
	mySPIAudioDevice.global_err = Pa_OpenStream(
        &mySPIAudioDevice.global_stream,
        NULL, //&mySPIAudioDevice.global_inputParameters,
		//2024feb20, spi, begin
		&mySPIAudioDevice.global_outputParameters, //NULL, // &outputParameters,
		//2024feb20, spi, end
		//global_SF_INFO.samplerate, //for using sound file's samplerate
		SAMPLE_RATE, //for using app pre-defined samplerate
		FRAMES_PER_BUFFER,
        0, //paClipOff,      // we won't output out of range samples so don't bother clipping them
		//2024feb20, spi, begin
		renderCallback, //NULL, // no callback, use blocking API
		//2024feb20, spi, end
        NULL ); // no callback, so no callback userData
    if(mySPIAudioDevice.global_err != paNoError )
	{
		char errorbuf[2048];
        sprintf(errorbuf, "Unable to open stream: %s\n", Pa_GetErrorText(mySPIAudioDevice.global_err));
		MessageBox(0,errorbuf,0,MB_ICONERROR);
        return 1;
    }

	//////////////
    //start stream  
	//////////////
	mySPIAudioDevice.global_err = Pa_StartStream(mySPIAudioDevice.global_stream );
    if(mySPIAudioDevice.global_err != paNoError )
	{
		char errorbuf[2048];
        sprintf(errorbuf, "Unable to start stream: %s\n", Pa_GetErrorText(mySPIAudioDevice.global_err));
		MessageBox(0,errorbuf,0,MB_ICONERROR);
        return 1;
    }


	WNDCLASS wc={0};
    MSG msg;

	/*
	// check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		MessageBox(0,"An incorrect version of BASS.DLL was loaded",0,MB_ICONERROR);
		return 0;
	}
	*/

	// register window class and create the window
	//2023jan26, spi, begin
	//wc.lpfnWndProc = SpectrumWindowProc;
	wc.lpfnWndProc = (WNDPROC) SpectrumWindowProc;
	//2023jan26, spi, end
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)); //spi, added
	wc.lpszClassName = global_classname.c_str();
	if (!RegisterClass(&wc) || !CreateWindow(global_classname.c_str(),
			//"BASS spectrum example (click to toggle mode)",
			//"spispectrumplay (click to toggle mode)",
			global_title.c_str(),
			//WS_POPUPWINDOW|WS_VISIBLE, global_x, global_y,
			WS_POPUP|WS_VISIBLE, global_x, global_y,
			
			//WS_POPUPWINDOW|WS_CAPTION|WS_VISIBLE, global_x, global_y,
			//WS_POPUPWINDOW|WS_VISIBLE, 200, 200,
			SPECWIDTH,
			//SPECWIDTH+2*GetSystemMetrics(SM_CXDLGFRAME),
			SPECHEIGHT,
			//SPECHEIGHT+GetSystemMetrics(SM_CYCAPTION)+2*GetSystemMetrics(SM_CYDLGFRAME),
			NULL, NULL, hInstance, NULL)) 
	{
		//Error("Can't create window");
		MessageBox(0,"Can't create window",0,MB_ICONERROR);
		return 0;
	}
	ShowWindow(win, SW_SHOWNORMAL);

	while (GetMessage(&msg,NULL,0,0)>0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
