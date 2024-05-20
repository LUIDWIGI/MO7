#include "FilipTimerV2.h"

XTime tStart = 0, tEnd = 0;
XTime summedTime = 0;
u32 iteration = 0;
const u32 maxIterations = samples+2;
float finalAnswer;


void timerStart() {
	if (iteration < samples) {
		XTime_GetTime(&tStart);
	}
}

void timerEnd() {
	if (iteration < maxIterations) {
		if (iteration < samples) {//taking another sample
				XTime_GetTime(&tEnd);
				summedTime = summedTime + (u32)(tEnd - tStart); //summing function
				//printf("read: %d\n", ((int)(tEnd - tStart) / CPuS));
		}
		else if (iteration == samples) { //calculating
			finalAnswer = (float)summedTime / (float)samples / (float)CPuS;

		}
		else { //printing (not done at the same time as calculating due to performance)
			printf("process takes %f us\n",finalAnswer);
		}
		iteration++;
	}
}
