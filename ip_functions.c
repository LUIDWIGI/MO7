/*
 * ip_functions.c
 *
 * Contains all functions which pertain to setup and use of IP periperals.
 */

#include "adventures_with_ip.h"

Fir_s *FirL, *FirR;
Iir_s *IirL, *IirR, *IirL1, *IirR1;

float firConsts[] = {
		0.081071172466248717092263120775896823034f,
		0.101402019276819382875842734392790589482f,
		0.1189757136347182608915318269282579422f,
		0.130949631555567325147393376028048805892f,
		0.135202926133292711252664730636752210557f,
		0.130949631555567325147393376028048805892f,
		0.1189757136347182608915318269282579422f,
		0.101402019276819382875842734392790589482f,
		0.081071172466248717092263120775896823034f};

float num1[IIR_SAMPLE_DELAY_LENGTH] = {
		0.8689530492,   -4.343358994,    8.685313225,   -8.685313225,    4.343358994,
		    -0.8689530492
};

float den1[IIR_SAMPLE_DELAY_LENGTH] = {
		1,   -4.728032112,    8.931567192,   -8.424290657,    3.966059923,
		    -0.7453004718
};

float num2[IIR_SAMPLE_DELAY_LENGTH] = {
		0.002258419292,-0.002866517985, 0.002138149925, 0.002138149925,-0.002866517985,
		   0.002258419292
};

float den2[IIR_SAMPLE_DELAY_LENGTH] = {
		1,   -4.350269318,    7.899902821,   -7.454248428,    3.649420738,
		    -0.7417456508
};



/* ---------------------------------------------------------------------------- *
 * 								lms_filter()									*
 * ---------------------------------------------------------------------------- *
 * This function adds a tonal noise component to the sampled audio from the
 * audio codec by passing a step size to the input of an NCO component in the
 * PL. A sinusoidal signal is received back from the NCO which is then scaled
 * and added to the sampled audio. The audio + noise sample can then be
 * adaptively filtered using an LMS filter in the PL. The resulting audio,
 * filtered or not, is then output to the audio codec.
 *
 * The main menu can be accessed by entering 'q' on the keyboard.
 * ---------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------------- *
 * 								tonal_noise()									*
 * ---------------------------------------------------------------------------- *
 * This function adds a tonal noise component to the sampled audio from the
 * audio codec by passing a step size to the input of an NCO component in the
 * PL. A sinusoidal signal is received back from the NCO which is then scaled
 * and added to the sampled audio. The audio + noise sample is then sent to
 * the audio codec for output.
 *
 * The main menu can be accessed by entering 'q' on the keyboard.
 * ---------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------- *
 * 								audio_stream()									*
 * ---------------------------------------------------------------------------- *
 * This function performs audio loopback streaming by sampling the input audio
 * from the codec and then immediately passing the sample to the output of the
 * codec.
 *
 * The main menu can be accessed by entering 'q' on the keyboard.
 * ---------------------------------------------------------------------------- */
void audio_stream(){
	u32  in_left, in_right;
	u8 dataReady = 0;
	u32 statusReg = 0;

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){

		while (dataReady == 0) {
			statusReg = Xil_In32(I2S_STATUS_REG);
			dataReady = statusReg >> 21;
		}
		dataReady = 0;
		// Read audio input from codec
		in_left = Xil_In32(I2S_DATA_RX_L_REG);
		in_right = Xil_In32(I2S_DATA_RX_R_REG);

		// Write audio output to codec
		Xil_Out32(I2S_DATA_TX_L_REG, in_left);
		Xil_Out32(I2S_DATA_TX_R_REG, in_right);
	}

	/* If input from the terminal is 'q', then return to menu.
	 * Else, continue streaming. */
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q') menu();
	else audio_stream();
} // audio_stream()

void moving_avg(){
	u32  in_left, in_right;

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){
		// Read audio input from codec
		in_left = Xil_In32(I2S_DATA_RX_L_REG);
		in_right = Xil_In32(I2S_DATA_RX_R_REG);

		in_left = SAMPLE_S_TO_U(movingAverageL(SAMPLE_U_TO_S(in_left)));
		in_right = SAMPLE_S_TO_U(movingAverageR(SAMPLE_U_TO_S(in_right)));

		// Write audio output to codec
		Xil_Out32(I2S_DATA_TX_L_REG, in_left);
		Xil_Out32(I2S_DATA_TX_R_REG, in_right);
	}

	/* If input from the terminal is 'q', then return to menu.
	 * Else, continue streaming. */
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q') menu();
	else moving_avg();
} // moving_avg()

// void initFirStructs(){
// 	FirL = initFir(1.0f, firConsts);
// 	FirR = initFir(1.0f, firConsts);
// }

void fir(){
	u32  in_left, in_right;
	u8 dataReady = 0;
	u32 statusReg = 0;
	if (FirL == NULL) { FirL = initFir(1.0f, firConsts);}
	if (FirR == NULL) { FirR = initFir(1.0f, firConsts);}

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){
		while (dataReady == 0) {
			statusReg = Xil_In32(I2S_STATUS_REG);
			dataReady = statusReg >> 21;
		}
		dataReady = 0;

		// Read audio input from codec
		in_left = Xil_In32(I2S_DATA_RX_L_REG);
		in_right = Xil_In32(I2S_DATA_RX_R_REG);

		in_left = SAMPLE_S_TO_U(firFilter(SAMPLE_U_TO_S(in_left), FirL));
		in_right = SAMPLE_S_TO_U(firFilter(SAMPLE_U_TO_S(in_right), FirR));

		// Write audio output to codec
		Xil_Out32(I2S_DATA_TX_L_REG, in_left);
		Xil_Out32(I2S_DATA_TX_R_REG, in_right);
	}



/* If input from the terminal is 'q', then return to menu.
 * Else, continue streaming. */
if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q') menu();
else fir();
} // audio_stream()

void iir(){
	u32 in_left, in_right;
	s32 in_left0, in_right0, in_left1, in_right1;
	u8 dataReady = 0;
	u32 statusReg = 0;


	if (IirL == NULL) { IirL = initIir(0.5f, num1, den1);}
	if (IirR == NULL) { IirR = initIir(0.5f, num1, den1);}
	if (IirR1 == NULL) { IirR1 = initIir(0.5f, num2, den2);}
	if (IirL1 == NULL) { IirL1 = initIir(0.5f, num2, den2);
	// for (u8 i = 0; i < IIR_SAMPLE_DELAY_LENGTH; i++) {
	// 	printf("Den1[%u]: %f\n", i, IirL->iirConstsDenum[i]);
	// }

	// for (u8 i = 0; i < IIR_SAMPLE_DELAY_LENGTH; i++) {
	// 	printf("Num1[%u]: %f\n", i, IirL->iirConstsNum[i]);
	// }

	// for (u8 i = 0; i < IIR_SAMPLE_DELAY_LENGTH; i++) {
	// 	printf("Den2[%u]: %f\n", i, IirL1->iirConstsDenum[i]);
	// }

	// for (u8 i = 0; i < IIR_SAMPLE_DELAY_LENGTH; i++) {
	// 	printf("Num2[%u]: %f\n", i, IirL1->iirConstsNum[i]);
	// }
	}

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){
		while (dataReady == 0) {
			statusReg = Xil_In32(I2S_STATUS_REG);
			dataReady = statusReg >> 21;
		}
		dataReady = 0;

		// Read audio input from codec
		in_left = SAMPLE_U_TO_S((Xil_In32(I2S_DATA_RX_L_REG)));
		in_right = SAMPLE_U_TO_S((Xil_In32(I2S_DATA_RX_R_REG)));

		// Xil_Out32(I2S_DATA_TX_L_REG, SAMPLE_S_TO_U(in_left));
		// Xil_Out32(I2S_DATA_TX_R_REG, SAMPLE_S_TO_U(in_right));

		//xil_printf("Left: %d, Right: %d\n", in_left, in_right);
		//timerStart();
		in_left0 = iirFilter(in_left, IirL);
		in_right0 = iirFilter(in_right, IirR);
		in_left1 = iirFilter(in_left, IirL1);
		in_right1 = iirFilter(in_right, IirR1);
		//timerEnd();

		while(1){};

		u32 out_left = SAMPLE_S_TO_U((in_left0 + in_left1));
		u32 out_right = SAMPLE_S_TO_U((in_right0 + in_right1));

		//xil_printf("Left: %d, Right: %d\n", in_left, in_right);
		// Write audio output to codec
		Xil_Out32(I2S_DATA_TX_L_REG, SAMPLE_S_TO_U(in_left0));
		Xil_Out32(I2S_DATA_TX_R_REG, SAMPLE_S_TO_U(in_right0));
	}
	if(XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET) == 'q') menu();
	else iir();
}

void initInputCircularBuffer(InputBuffer *inputBuffer, size_t length){
	inputBuffer->buffer = (s32*)malloc(sizeof(s32) * length);
	if (inputBuffer->buffer != NULL) {
		memset(inputBuffer->buffer, 0, sizeof(s32) * length);
	}
	inputBuffer->length = length;
	inputBuffer->index = 0;
}

void storeSamplesInsideBuffer(InputBuffer *inputBuffer, u32 sample){
	// CIRCULAR BUFFER INPUT
	inputBuffer->buffer[inputBuffer->index] = SAMPLE_U_TO_S(sample);
	inputBuffer->index++;
	if (inputBuffer->index >= inputBuffer->length) {
		inputBuffer->index = 0;
	}
}


/* ---------------------------------------------------------------------------- *
 * 								gpio_initi()									*
 * ---------------------------------------------------------------------------- *
 * Initialises the GPIO driver for the push buttons and switches.
 * ---------------------------------------------------------------------------- */
/*
unsigned char gpio_init()
{
	int Status;

	Status = XGpio_Initialize(&Gpio, BUTTON_SWITCH_ID);
	if(Status != XST_SUCCESS) return XST_FAILURE;

	XGpio_SetDataDirection(&Gpio, SWITCH_CHANNEL, 0xFF);
	XGpio_SetDataDirection(&Gpio, BUTTON_CHANNEL, 0xFF);

	return XST_SUCCESS;
}
*/
/* ---------------------------------------------------------------------------- *
 * 								nco_initi()									*
 * ---------------------------------------------------------------------------- *
 * Initialises the NCO driver by looking up the configuration in the config
 * table and then initialising it.
 * ---------------------------------------------------------------------------- */
