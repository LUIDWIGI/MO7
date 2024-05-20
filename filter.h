#ifndef FILTER_H_
#define FILTER_H_

#include "xil_types.h"
#include <stdio.h>
#include <string.h>

#define WINDOW_SIZE 8
#define SAMPLE_DELAY_LENGTH 5
#define FIR_SAMPLE_DELAY_LENGTH 9
#define IIR_SAMPLE_DELAY_LENGTH 11

#define SAMPLE_U_TO_S(x) (x<<8)
#define SAMPLE_S_TO_U(x) (x>>8)

/**
 * @brief Defines the structure for a FIR filter.
 *
 * This structure represents a Finite Impulse Response (FIR) filter.
 * It is used to store the necessary information for filtering operations.
 */
typedef struct fir_s Fir_s;

struct fir_s {
	float sampBufFir[FIR_SAMPLE_DELAY_LENGTH];
	float firConsts[FIR_SAMPLE_DELAY_LENGTH];
	u8 bufIndex;
	float sampBufOut;
	float gain;
};
/**
 * @brief Defines the structure for an IIR filter.
 * 
 * This structure represents a Infinite Impulse Response (IIR) filter.
 * It is used to store the necessary information for filtering operations.
 */
typedef struct iir_s Iir_s;

struct iir_s{
	float sampBufIirInput[IIR_SAMPLE_DELAY_LENGTH];
	float sampBufIirOutput[IIR_SAMPLE_DELAY_LENGTH];
	float iirConstsNum[IIR_SAMPLE_DELAY_LENGTH];
	float iirConstsDenum[IIR_SAMPLE_DELAY_LENGTH];
	u8 bufIndex;
	float sampBufOut;
	float gain;
};

/**
 * Sets the gain for a Finite Impulse Response (FIR) filter.
 *
 * @param firStruct Pointer to the FIR filter structure.
 * @param gain The desired gain value.
 */
void setGainFir(Fir_s* firStruct, float gain);

/**
 * Sets the gain for an Infinite Impulse Response (IIR) filter.
 *
 * @param iirStruct Pointer to the IIR filter structure.
 * @param gain The desired gain value.
 */
void setGainIir(Iir_s* iirStruct, float gain);

/**
 * @brief Calculates the moving average of the input for the left channel.
 *
 * This function takes an input value and calculates the moving average for the left channel.
 *
 * @param input The input value for which the moving average needs to be calculated.
 * @return The calculated moving average for the left channel.
 */
s32 movingAverageL(s32 input);

/**
 * @brief Calculates the moving average of the input for the right channel.
 *
 * This function takes an input value and calculates the moving average for the right channel.
 *
 * @param input The input value for which the moving average needs to be calculated.
 * @return The calculated moving average for the right channel.
 */
s32 movingAverageR(s32 input);

/**
 * Configures the FIR filter with the given coefficients.
 *
 * @param Consts Pointer to the array of coefficients.
 * @param firStruct Pointer to the FIR filter structure.
 */
void configFir(float *Consts, Fir_s* firStruct);

/**
 * Configures the IIR filter with the given coefficients.
 *
 * @param ConstsA Pointer to the array of output coefficients.
 * @param ConstsB Pointer to the array of input coefficients.
 * @param iirStruct Pointer to the IIR filter structure.
 */
void configIir(s16 *ConstsA, s16 *ConstsB, Iir_s* iirStruct);

/**
 * @brief Applies a Finite Impulse Response (FIR) filter to the input signal.
 *
 * This function takes an input signal and applies an FIR filter to it using the provided FIR filter structure.
 *
 * @param input The input signal to be filtered.
 * @param firStruct Pointer to the FIR filter structure.
 * @return The filtered output signal.
 */
s32 firFilter(s32 input, Fir_s* firStruct);

/**
 * @brief Applies an Infinite Impulse Response (IIR) filter to the input signal.
 *
 * This function takes an input signal and applies an IIR filter to it using the provided IIR filter structure.
 *
 * @param input The input signal to be filtered.
 * @param iirStruct Pointer to the IIR filter structure.
 * @return The filtered output signal.
 */
s32 iirFilter(s32 input, Iir_s* iirStruct);

/**
 * Initializes a Fir_s structure on the heap.
 *
 * @param gain The desired gain value for the FIR filter.
 * @param firConsts Pointer to the array of FIR filter coefficients.
 * @return Pointer to the initialized Fir_s structure on the heap.
 */
Fir_s* initFir(float gain, float *firConsts);

/**
 * Initializes an Iir_s structure on the heap.
 *
 * @param gain The desired gain value for the IIR filter.
 * @param iirConstsNum Pointer to the array of IIR filter output coefficients.
 * @param iirConstsDenum Pointer to the array of IIR filter input coefficients.
 * @return Pointer to the initialized Iir_s structure on the heap.
 */
Iir_s* initIir(float gain, float *iirConstsNum, float *iirConstsDenum);

#endif
