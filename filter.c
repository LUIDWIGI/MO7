#include "filter.h"
#include <stdlib.h>

u32 newDataPosL;
u32 newDataPosR;
s32 bufL[WINDOW_SIZE];
s32 bufR[WINDOW_SIZE];

s32 movingAverageL(s32 input) {
	s64 average = 0;

	bufL[newDataPosL] = input;
	newDataPosL++;
	if (newDataPosL >= WINDOW_SIZE) {
		newDataPosL = 0;
	}

	for (u8 i = 0; i < WINDOW_SIZE; i++) {
		average += bufL[i];
	}
	average = average / WINDOW_SIZE;
	return average;
}

s32 movingAverageR(s32 input) {
	s64 average = 0;

	bufR[newDataPosR] = input;
	newDataPosR++;
	if (newDataPosR >= WINDOW_SIZE) {
		newDataPosR = 0;
	}

	for (u8 i = 0; i < WINDOW_SIZE; i++) {
		average += bufR[i];
	}
	average = average / WINDOW_SIZE;
	return average;
}

//MARK FIR
void configFir(float *Consts, Fir_s* firStruct) {
	for (u8 i = 0; i < FIR_SAMPLE_DELAY_LENGTH; i++) {
		firStruct->firConsts[i] = Consts[i];
		firStruct->sampBufFir[i] = 0.0f;
	}
	firStruct->bufIndex = 0;
	firStruct->sampBufOut = 0.0f;
}

void setGainFir(Fir_s* firStruct, float gain) {
	firStruct->gain = gain;
}

s32 firFilter(s32 input, Fir_s* firStruct) {
	float filtOut = 0;		// Filter output

	// Delay line (unused)
	// firStruct->sampBufFir[8] = firStruct->sampBufFir[7];
	// firStruct->sampBufFir[7] = firStruct->sampBufFir[6];
	// firStruct->sampBufFir[6] = firStruct->sampBufFir[5];
	// firStruct->sampBufFir[5] = firStruct->sampBufFir[4];
	// firStruct->sampBufFir[4] = firStruct->sampBufFir[3];
	// firStruct->sampBufFir[3] = firStruct->sampBufFir[2];
	// firStruct->sampBufFir[2] = firStruct->sampBufFir[1];
	// firStruct->sampBufFir[1] = firStruct->sampBufFir[0];
	// firStruct->sampBufFir[0] = input;

	// CIRCULAR BUFFER
	firStruct->bufIndex++;
	if (firStruct->bufIndex >= FIR_SAMPLE_DELAY_LENGTH) {
		firStruct->bufIndex = 0;
	}
	u8 sumIndex = firStruct->bufIndex + 1;


	// Convolution
	// for (u8 i = 0; i < FIR_SAMPLE_DELAY_LENGTH; i++) {
	// 	filtOut += firStruct->firConsts[i] * firStruct->sampBufFir[i];
	// }
	firStruct->sampBufFir[firStruct->bufIndex] = (float)input;
	// Convultion 2: Electric boogaloo
	for (u8 i = 0; i < FIR_SAMPLE_DELAY_LENGTH; i++) {
		if (sumIndex > 0) {
			sumIndex--;
		}
		else {
			sumIndex = FIR_SAMPLE_DELAY_LENGTH - 1;
		}
		filtOut += firStruct->firConsts[i] * firStruct->sampBufFir[sumIndex];
	}

	// Gain
	filtOut *= firStruct->gain;

	return (s32)filtOut;
}

u32 delayNSamples(u32 newSample) {
  static u32 sampBuf[SAMPLE_DELAY_LENGTH] = {0};

  for(u8 i = SAMPLE_DELAY_LENGTH - 1; i > 0; i--) {
  	if (i == 0) {
  		sampBuf[i] = newSample;
  	}
  	else {
  		sampBuf[i] = sampBuf[i - 1];
  	}
  }
  return sampBuf[SAMPLE_DELAY_LENGTH - 1];
}

//MARK IIR
void configIir(s16 *ConstsA, s16 *ConstsB, Iir_s* iirStruct) {
	for (u8 i = 0; i < IIR_SAMPLE_DELAY_LENGTH; i++) {
		iirStruct->iirConstsDenum[i] = (float)ConstsA[i];
		iirStruct->iirConstsNum[i] = (float)ConstsB[i];
		iirStruct->sampBufIirInput[i] = 0.0f;
		iirStruct->sampBufIirOutput[i] = 0.0f;
	}
	iirStruct->bufIndex = 0;
	iirStruct->sampBufOut = 0.0f;
}

void setGainIir(Iir_s* iirStruct, float gain) {
	iirStruct->gain = gain;
}

s32 iirFilter(s32 input, Iir_s* iirStruct) {

	/// CIRCULAR BUFFER INPUT

	iirStruct->bufIndex++;
	if (iirStruct->bufIndex >= IIR_SAMPLE_DELAY_LENGTH) {
		iirStruct->bufIndex = 0;
	}
	iirStruct->sampBufIirInput[iirStruct->bufIndex] = (float)(input);
	//printf("sampBufIirInput: %f\n", iirStruct->sampBufIirInput[iirStruct->bufIndex]);
	// if (isnan(iirStruct->sampBufIirInput[iirStruct->bufIndex])) {
	// 	printf("input nan\n");
	// }
	
	// RECURSIVE CONVOLUTION

	iirStruct->sampBufIirOutput[iirStruct->bufIndex] = iirStruct->iirConstsNum[0] * iirStruct->sampBufIirInput[iirStruct->bufIndex];

	u8 sumIndex = iirStruct->bufIndex;
	for (u8 i = 1; i < IIR_SAMPLE_DELAY_LENGTH; i++) {
		sumIndex--;
		if (sumIndex < 0) {
			sumIndex = IIR_SAMPLE_DELAY_LENGTH - 1;
		}
		iirStruct->sampBufIirOutput[iirStruct->bufIndex] +=
			(iirStruct->iirConstsNum[i] * iirStruct->sampBufIirInput[sumIndex]) /*using input*/
			- (iirStruct->iirConstsDenum[i] * iirStruct->sampBufIirOutput[sumIndex]); /*using output*/
		printf("sampBufIirOutput: %f\n", iirStruct->sampBufIirOutput[iirStruct->bufIndex]);
		printf("numcalc: %f\n", (iirStruct->iirConstsNum[i] * iirStruct->sampBufIirInput[sumIndex]));
		printf("denumcalc: %f\n", (iirStruct->iirConstsDenum[i] * iirStruct->sampBufIirOutput[sumIndex]));
	}
	printf("final sampBufIirOutput: %f\n", (iirStruct->sampBufIirOutput[iirStruct->bufIndex]));

	// GAIN
	return (s32)(iirStruct->sampBufIirOutput[iirStruct->bufIndex] * iirStruct->gain);
}

Fir_s* initFir(float gain, float *firConsts) {
	Fir_s* firStruct = (Fir_s*)malloc(sizeof(Fir_s));
	if (firStruct != NULL) {
		memset(firStruct, 0, sizeof(Fir_s));
		firStruct->gain = gain;
		memcpy(firStruct->firConsts, firConsts, sizeof(float) * FIR_SAMPLE_DELAY_LENGTH);
	}
	return firStruct;
}

Iir_s* initIir(float gain, float *iirConstsNum, float *iirConstsDenum) {
	Iir_s* iirStruct = (Iir_s*)malloc(sizeof(Iir_s));
	if (iirStruct != NULL) {
		memset(iirStruct, 0, sizeof(Iir_s));
		iirStruct->gain = gain;
		memcpy(iirStruct->iirConstsDenum, iirConstsDenum, sizeof(float) * IIR_SAMPLE_DELAY_LENGTH);
		memcpy(iirStruct->iirConstsNum, iirConstsNum, sizeof(float) * IIR_SAMPLE_DELAY_LENGTH);
	}
	return iirStruct;
}


