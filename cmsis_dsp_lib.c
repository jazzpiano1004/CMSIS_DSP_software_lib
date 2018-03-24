/**
  * cmsis_dsp_lib.c - Using ARM CMSIS DSP software library.
  *
  * Written in 2017 by Nattapong W. <https://github.com/jazzpiano1004>.
  *
  * To the extent possible under law, the author(s) have dedicated all
  * copyright and related and neighboring rights to this software to
  * the public domain worldwide. This software is distributed without
  * any warranty.
  *
  *  + Meaning of each return status of "arm_status" after calling a function in arm_math.h :
  *     ARM_MATH_SUCCESS = 0,                < No error
  *     ARM_MATH_ARGUMENT_ERROR = -1,        < One or more arguments are incorrect
  *     ARM_MATH_LENGTH_ERROR = -2,          < Length of data buffer is incorrect
  *     ARM_MATH_SIZE_MISMATCH = -3,         < Size of matrices is not compatible with the operation.
  *     ARM_MATH_NANINF = -4,                < Not-a-number (NaN) or infinity is generated
  *     ARM_MATH_SINGULAR = -5,              < Generated by matrix inversion if the input matrix is singular and cannot be inverted.
  *     ARM_MATH_TEST_FAILURE = -6           < Test Failed
  */

#include "cmsis_dsp_lib.h"

/******************** DSP block : Fast Fourier Transform (FFT) - start ***********************/
/** DSP Block Description.
  * + Floating-point (f32) Real-FFT
  *     The FFT of a real N-point sequence has even symmetry in the frequency domain. The second half of the data equals the conjugate of the first half flipped in frequency.
  *     Looking at the data, we see that we can uniquely represent the FFT using only N/2 complex numbers. These are packed into the output array in alternating real and imaginary components:
  *
  *     X = { real[0], imag[0], real[1], imag[1], real[2], imag[2] ... real[(N/2)-1], imag[(N/2)-1 }
  *
  *     It happens that the first complex number (real[0], imag[0]) is actually all real. real[0] represents the DC offset, and imag[0] should be 0.
  *     (real[1], imag[1]) is the fundamental frequency, (real[2], imag[2]) is the first harmonic and so on.
  *     The real FFT functions pack the frequency domain data in this fashion. The forward transform outputs the data in this form and the inverse transform expects input data in this form.
  *     The function always performs the needed bitreversal so that the input and output data is always in normal order. The functions support lengths of [32, 64, 128, ..., 4096] samples.
  */



/**
  * @brief  CMSIS_DSP_real_FFT_f32_blockInit() : This function is used to "quick" initialize a real FFT (or can be IFFT) block for a real "float32_t" data point.
  *                                              Note, This function can be used to initialize both FFT and IFFT. FFT mode or IFFT mode, and it'll be selected automaticlly by using "flag"
  *                                                    whenever call "CMSIS_DSP_calculate_real_FFT_f32()" function or "CMSIS_DSP_real_IFFT_f32_calculate()"
  * @param  user_RFFT_struct : user's real FFT block
  * @param  set_tmp1 : any pointer of temporary buffer which is used in FFT and IFFT calculation. 					Size of buffer must equal to rFFT size
  * @param  set_tmp2 : any pointer of temporary buffer which is used in FFT and IFFT magnitude calculation. Size of buffer must equal to rFFT size
  * @param  setLength : size of real FFT
  * @retval arm_status
  */
int8_t  CMSIS_DSP_real_FFT_f32_blockInit(rfft_f32_TypeDef *user_RFFT_struct, float32_t *set_tmp1, float32_t *set_tmp2, uint16_t setLength)
{
    user_RFFT_struct->length = setLength;
    user_RFFT_struct->tmpBuffer_1 = set_tmp1;
    user_RFFT_struct->tmpBuffer_2 = set_tmp2;

    return arm_rfft_fast_init_f32(&(user_RFFT_struct->arm_struct), setLength);
}



/**
  * @brief  CMSIS_DSP_real_FFT_f32_apply() : This function is used to calculate a FFT for a real "float32_t" input data point. Format of output array will be "complex" number.
  *                                              Output = { real[0], imag[0], real[1], imag[1], real[2], imag[2] ... real[(N/2)-1], imag[(N/2)-1 }
  *
  * @param  user_RFFT_struct : user's real FFT block (size = N)
  * @param  rDataIn  : real input data (size = N)
  * @param  fftOut : FFT output data (size = N)
  * @retval none
  */
void    CMSIS_DSP_real_FFT_f32_apply(rfft_f32_TypeDef *user_RFFT_struct, float32_t *rDataIn, float32_t *fftOut)
{
		memcpy((void*)((uint8_t*)user_RFFT_struct->tmpBuffer_1), rDataIn, (sizeof(float32_t))*(user_RFFT_struct->length));

		arm_rfft_fast_f32(&(user_RFFT_struct->arm_struct),
											user_RFFT_struct->tmpBuffer_1,
											fftOut,
											REAL_FFT_SELECT_VAL);
}



/**
  * @brief  CMSIS_DSP_real_IFFT_f32_apply() : This function is used to calculate a IFFT for a real "float32_t" output data point. Format of input array will be "complex" number.
  *                                               Input = { real[0], imag[0], real[1], imag[1], real[2], imag[2] ... real[(N/2)-1], imag[(N/2)-1 }
  *
  * @param  user_RFFT_struct : user's real FFT block (size = N)
  * @param  fftIn   : FFT input data (size = N)
  * @param  ifftOut : IFFT output data (size = N)
  * @retval none
  */
void    CMSIS_DSP_real_IFFT_f32_apply(rfft_f32_TypeDef *user_RFFT_struct, float32_t *fftIn, float32_t *ifftOut)
{
    memcpy((void*)((uint8_t*)user_RFFT_struct->tmpBuffer_1), fftIn, sizeof(float32_t)*(user_RFFT_struct->length));

    arm_rfft_fast_f32(&(user_RFFT_struct->arm_struct),
											user_RFFT_struct->tmpBuffer_1,
											ifftOut,
											REAL_IFFT_SELECT_VAL);
}



/**
  * @brief  CMSIS_DSP_magnitude_FFT_f32_apply() : This function is used to calculate a magnitude of FFT for a real "float32_t" input data point.
  *                                                   Format of output array will be "Real" number with size = N/2 as
  *                                                   Output = { mag[0], mag[1], mag[2], ... mag[(N/2)-1], mag[(N/2)-1 }
  * @param  user_RFFT_struct : user's real FFT block (size = N)
  * @param  rDataIn  		: real input data (size = N)
  * @param  magFFTOut : magnitude FFT output data (size = N/2)
  * @retval none
  */
void    CMSIS_DSP_magnitude_FFT_f32_apply(rfft_f32_TypeDef *user_RFFT_struct, float32_t *rDataIn, float32_t *magFFTOut)
{
		memcpy((void*)((uint8_t*)user_RFFT_struct->tmpBuffer_1), rDataIn, (sizeof(float32_t))*(user_RFFT_struct->length));

		arm_rfft_fast_f32(&(user_RFFT_struct->arm_struct),
											user_RFFT_struct->tmpBuffer_1,
											user_RFFT_struct->tmpBuffer_2,
											REAL_FFT_SELECT_VAL);

		arm_cmplx_mag_f32(user_RFFT_struct->tmpBuffer_2,
											magFFTOut,
											(user_RFFT_struct->length)/2);
}



/**
  * @brief  CMSIS_DSP_magnitudeSquare_FFT_f32_apply() : This function is used to calculate a magnitude-square of FFT for a real "float32_t" input data point.
  *                                                         Format of output array will be "Real" number with size = N/2 as
  *                                                         Output = { mag^2[0], mag^2[1], mag^2[2], ... mag^2[(N/2)-1], mag^2[(N/2)-1 }
  * @param  user_RFFT_struct : user real FFT block (size = N)
  * @param  rDataIn  			: real input data (size = N)
  * @param  magSqFFTOut : magnitude square FFT output data (size = N/2)
  * @retval none
  */
void    CMSIS_DSP_magnitudeSquare_FFT_f32_apply(rfft_f32_TypeDef *user_RFFT_struct, float32_t *rDataIn, float32_t *magSqFFTOut)
{
		memcpy((void*)((uint8_t*)user_RFFT_struct->tmpBuffer_1), rDataIn, (sizeof(float32_t))*(user_RFFT_struct->length));

		arm_rfft_fast_f32(&(user_RFFT_struct->arm_struct),
											user_RFFT_struct->tmpBuffer_1,
											user_RFFT_struct->tmpBuffer_2,
											REAL_FFT_SELECT_VAL);

		arm_cmplx_mag_squared_f32(user_RFFT_struct->tmpBuffer_2,
															magSqFFTOut,
															(user_RFFT_struct->length)/2);
}
/********************* DSP block : Fast Fourier Transform (FFT) - end ************************/




/************************** DSP block : Window functions - start *****************************/
/** DSP Block Description.
  * + Window function
  *     This block is used to generate a window function and apply it to an input data point. After window initialization by using CMSIS_DSP_windowFunction_blockInit()
  *			,user can window any input data by calling "CMSIS_DSP_windowFunciton_apply()".
  *
  *			List of marcro of basic window functions for CMSIS_DSP_windowFunction_blockInit()
  *			- WINDOW_TYPE_RECTANGULAR
  *			- WINDOW_TYPE_HANNING
  *			- WINDOW_TYPE_HAMMING
  *
  */



/**
  * @brief  CMSIS_DSP_windowFunction_blockInit() : This function is used to initialize a window function block for a "float32_t" data point.
  * @param  user_w_struct : user's window function struct
  * @param  set_valueBuf  : any pointer of floating-point-type array which is used to store window's value (with maximum length = REAL_FFT_SIZE)
  * @param  setWindowType : type of window function (hanning, hamming etc.)
  * @param  setLength : length of window function
  * @retval none
  */
void    CMSIS_DSP_windowFunction_blockInit(window_TypeDef *user_w_struct, float32_t *set_valueBuf, uint8_t setWindowType, uint16_t setLength)
{
    uint16_t n;

    user_w_struct->value  = set_valueBuf;
    user_w_struct->length = setLength;
    user_w_struct->type   = setWindowType;

    if(user_w_struct->type == WINDOW_TYPE_RECTANGULAR)
    {
        for(n=0; n < user_w_struct->length; n++)
        {
            //for rectangular window : w = 1
            set_valueBuf[n] = 1;
        }
    }
    else if(user_w_struct->type == WINDOW_TYPE_HANNING)
    {
        for(n=0; n < user_w_struct->length; n++)
        {
            //for hanning window : w = 0.5*(1- cos(2pi*n/N))
            set_valueBuf[n] = 0.5*(1 - cos(2*(float32_t)(M_PI)*n/(user_w_struct->length)));
        }
    }
    else if(user_w_struct->type == WINDOW_TYPE_HAMMING)
    {
        for(n=0; n < user_w_struct->length; n++)
        {
            //for hamming window : w = 0.5*(1- cos(2pi*n/N))
            set_valueBuf[n] = 0.54 - 0.46*cos(2*(float32_t)(M_PI)*n/(user_w_struct->length));
        }
    }
}




/**
  * @brief  CMSIS_DSP_windowFunciton_apply() : This function is used to apply a window function block to a "float32_t" input data point.
  * @param  user_w_struct : user's window function struct
  * @param  pInput  : input data
  * @param  pOutput : output data
  * @retval none
  */
void    CMSIS_DSP_windowFunciton_apply(window_TypeDef *user_w_struct, float32_t *pInput, float32_t *pOutput)
{
    arm_mult_f32((float32_t *)pInput, (float32_t *)(user_w_struct->value), pOutput, user_w_struct->length);
}
/*************************** DSP block : Window functions - end ******************************/




/***************************** DSP block : FIR filter - start ********************************/
/** DSP Block Description.
  * + FIR filter
  *     This block is used to generate and apply a  Finite Impulse Response (FIR) filter to input data point.
  *
  *  		Algorithm:
  *		  The FIR filter algorithm is based upon a sequence of multiply-accumulate (MAC) operations. Each filter coefficient b[n] is multiplied by a state variable which equals a previous input sample x[n].
  *
  *  		y[n] = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]
  *
  * 		Note about format of coefficient (pCoeff) and state (pState):
  *			- pCoeff points to the array of filter coefficients stored in time reversed order -> 	{b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}
  *			- pState points to the array of state variables. pState is of length numTaps+blockSize-1 samples
  *
  */

/**
  * @brief  CMSIS_DSP_FIR_f32_blockInit() : This function is used to initialize a FIR filter block to a "float32_t" input data point.
  * @param  user_FIR_struct : user's FIR filter struct
  * @param  setNumTap    : number of filter's tap
  * @param  set_pCoeff   : pointer of floating-point array which is used to store filter coefficient
  * @param  set_pState 	 : pointer of floating-point array which is used to store filter's state
  * @param  setBlockSize : number of samples that are processed per call
  * @retval none
  */
void CMSIS_DSP_FIR_f32_blockInit(fir_f32_TypeDef *user_FIR_struct, float32_t *set_pCoeff, float32_t *set_pState, float32_t *set_tmp, uint16_t setNumTap, uint32_t setBlockSize)
{
        user_FIR_struct->numTap 	 = setNumTap;
        user_FIR_struct->blockSize = setBlockSize;
        user_FIR_struct->pCoeff 	 = set_pCoeff;
        user_FIR_struct->pState 	 = set_pState;
        user_FIR_struct->tmpBuffer = set_tmp;

        arm_fir_init_f32	(		&(user_FIR_struct->arm_struct),
                                    setNumTap,
                                    set_pCoeff,
                                    set_pState,
                                    setBlockSize
                            );
}



/**
  * @brief  CMSIS_DSP_FIR_f32_apply() : This function is used to apply a FIR filter block to a "float32_t" input data point.
  * @param  user_FIR_struct : user's FIR filter struct
  * @param  pInput  : input data
  * @param  pOutput : output data
  * @retval none
  */
void CMSIS_DSP_FIR_f32_apply(fir_f32_TypeDef *user_FIR_struct, float32_t *pInput, float32_t *pOutput)
{
        memcpy((void*)((uint8_t*)user_FIR_struct->tmpBuffer), pInput, (sizeof(float32_t))*(user_FIR_struct->blockSize));

        arm_fir_f32	(	&(user_FIR_struct->arm_struct),
                        user_FIR_struct->tmpBuffer,
                        pOutput,
                        user_FIR_struct->blockSize
                    );
}
/*****************************  DSP block : FIR filter - end  ********************************/
