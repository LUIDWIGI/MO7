/*
 * ip_functions.c
 *
 * Contains all functions which pertain to setup and use of IP periperals.
 */

#include "adventures_with_ip.h"

Fir_s *FirL, *FirR;
Iir_s *IirL, *IirR;

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

float iirConstsA[] = {
	1.0f,                                        
  -9.185561530018109266393366851843893527985f,
  38.327450434038539128778211306780576705933f,
 -95.690175779108983533660648390650749206543f,
 158.332891616621225239214254543185234069824f,
-181.4462809101795812694035703316330909729f, 
 145.855328521816034026414854452013969421387f,
 -81.210115140884056472714291885495185852051f,
  29.973113112214083031403788481839001178741f,
  -6.621459032524155574606083973776549100876f,
   0.664808720750718951109092813567258417606f
};

float iirConstsB[] = {
 0.002167301756152853389053580812628752028f,  
-0.013464646910265129151063057122428290313f,  
 0.036851474943063712574886636730298050679f,  
-0.056103034337550879151468308236871962436f,  
 0.044673723511193114132922232784039806575f,  
 0.000000000000000013877787807814456755295f,  
-0.044673723511193114132922232784039806575f,  
 0.05610303433755086527368050042241520714f,   
-0.036851474943063712574886636730298050679f,  
 0.013464646910265129151063057122428290313f,  
-0.002167301756152853389053580812628752028f  };



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

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){
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
	if (FirL == NULL) { FirL = initFir(1.0f, firConsts);}
	if (FirR == NULL) { FirR = initFir(1.0f, firConsts);}

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){
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
	u32  in_left, in_right;

	if (IirL == NULL) { IirL = initIir(1.0f, iirConstsB, iirConstsA);}
	if (IirR == NULL) { IirR = initIir(1.0f, iirConstsB, iirConstsA);}

	while (!XUartPs_IsReceiveData(UART_BASEADDR)){
		// Read audio input from codec
		in_left = Xil_In32(I2S_DATA_RX_L_REG);
		in_right = Xil_In32(I2S_DATA_RX_R_REG);

		//timerStart();
		in_left = SAMPLE_S_TO_U(iirFilter(SAMPLE_U_TO_S(in_left), IirL));
		in_right = SAMPLE_S_TO_U(iirFilter(SAMPLE_U_TO_S(in_right), IirR));
		//timerEnd();

		// Write audio output to codec
		Xil_Out32(I2S_DATA_TX_L_REG, in_left);
		Xil_Out32(I2S_DATA_TX_R_REG, in_right);
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
