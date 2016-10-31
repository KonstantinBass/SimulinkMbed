/*
 *  sfcnSpiConf.c C-MEX S-function for function call.
 *  Created by K.Bass
 *  2015-03-22
 *  Version 1.0
 */

/*
 * Must specify the S_FUNCTION_NAME as the name of the S-function.
 */
#define S_FUNCTION_NAME                sfcnSpiConf
#define S_FUNCTION_LEVEL               2

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
#include "string.h"
#define EDIT_OK(S, P_IDX) \
 (!((ssGetSimMode(S)==SS_SIMMODE_SIZES_CALL_ONLY) && mxIsEmpty(ssGetSFcnParam(S, P_IDX))))
//#define SAMPLE_TIME                    (ssGetSFcnParam(S, 2))
#define isDWorkNeeded                  ((!((ssRTWGenIsCodeGen(S) || ssGetSimMode(S)==SS_SIMMODE_EXTERNAL) && isSimulationTarget==0)) || ssIsRapidAcceleratorActive(S))
#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
 
 /*
 * Utility function prototypes.
 */
static bool IsRealMatrix(const mxArray * const m);


/* Function: mdlCheckParameters ===========================================
 * Abstract:
 *    mdlCheckParameters verifies new parameter settings whenever parameter
 *    change or are re-evaluated during a simulation. When a simulation is
 *    running, changes to S-function parameters can occur at any time during
 *    the simulation loop.
 */
static void mdlCheckParameters(SimStruct *S)
{
  /*
   * Check the parameter 1 - MOSI
   */
  if EDIT_OK(S, 0) {
    int_T *dimsArray = (int_T *) mxGetDimensions(ssGetSFcnParam(S, 0));

    /* Parameter 1 must be a vector */
    if ((dimsArray[0] > 1) && (dimsArray[1] > 1)) {
      ssSetErrorStatus(S,"Parameter 1 must be a vector");
      return;
    }

    /* Check the parameter attributes */
    ssCheckSFcnParamValueAttribs(S, 0, "P1", DYNAMICALLY_TYPED, 2, dimsArray, 0);
  }

  /*
   * Check the parameter 2 - MISO
   */
  if EDIT_OK(S, 1) {
    int_T *dimsArray = (int_T *) mxGetDimensions(ssGetSFcnParam(S, 1));

    /* Parameter 2 must be a vector */
    if ((dimsArray[0] > 1) && (dimsArray[1] > 1)) {
      ssSetErrorStatus(S,"Parameter 2 must be a vector");
      return;
    }

    /* Check the parameter attributes */
    ssCheckSFcnParamValueAttribs(S, 1, "P2", DYNAMICALLY_TYPED, 2, dimsArray, 0);
  }
  
  /*
   * Check the parameter 3 - SCLK
   */
  if EDIT_OK(S, 2) {
    int_T *dimsArray = (int_T *) mxGetDimensions(ssGetSFcnParam(S, 2));

    /* Parameter 2 must be a vector */
    if ((dimsArray[0] > 1) && (dimsArray[1] > 1)) {
      ssSetErrorStatus(S,"Parameter 2 must be a vector");
      return;
    }

    /* Check the parameter attributes */
    ssCheckSFcnParamValueAttribs(S, 2, "P3", DYNAMICALLY_TYPED, 2, dimsArray, 0);
  }
  
  /*
   * Check the parameter 4 - SPI Format -> Anz. Bits  (8 Bit or 16 Bit / 8 def)
   */
  if EDIT_OK(S, 3) {
    int_T *dimsArray = (int_T *) mxGetDimensions(ssGetSFcnParam(S, 3));

    /* Parameter 2 must be a vector */
    if ((dimsArray[0] > 1) && (dimsArray[1] > 1)) {
      ssSetErrorStatus(S,"Parameter 2 must be a vector");
      return;
    }

    /* Check the parameter attributes */
    ssCheckSFcnParamValueAttribs(S, 3, "P4", DYNAMICALLY_TYPED, 2, dimsArray, 0);
  }
  
  /*
   * Check the parameter 5 - SPI Format -> Mode (0,1,2,3 / 0 def)
   */
  if EDIT_OK(S, 4) {
    int_T *dimsArray = (int_T *) mxGetDimensions(ssGetSFcnParam(S, 4));

    /* Parameter 2 must be a vector */
    if ((dimsArray[0] > 1) && (dimsArray[1] > 1)) {
      ssSetErrorStatus(S,"Parameter 2 must be a vector");
      return;
    }

    /* Check the parameter attributes */
    ssCheckSFcnParamValueAttribs(S, 4, "P5", DYNAMICALLY_TYPED, 2, dimsArray, 0);
  }
  
  /*
   * Check the parameter 6 - SPI Frequency (def: 1000000 bzw. 1 Mhz)
   */
  if EDIT_OK(S, 5) {
    int_T *dimsArray = (int_T *) mxGetDimensions(ssGetSFcnParam(S, 5));

    /* Parameter 2 must be a vector */
    if ((dimsArray[0] > 1) && (dimsArray[1] > 1)) {
      ssSetErrorStatus(S,"Parameter 2 must be a vector");
      return;
    }

    /* Check the parameter attributes */
    ssCheckSFcnParamValueAttribs(S, 5, "P6", DYNAMICALLY_TYPED, 2, dimsArray, 0);
  }
  
}

#endif

/* Function: mdlInitializeSizes ===========================================
* Abstract:
*    The sizes information is used by Simulink to determine the S-function
*    block's characteristics (number of inputs, outputs, states, etc.).
*/
static void mdlInitializeSizes(SimStruct *S)
{

  /* Number of expected parameters */
  ssSetNumSFcnParams(S, 6);

#if defined(MATLAB_MEX_FILE)

  if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
    /*
     * If the number of expected input parameters is not equal
     * to the number of parameters entered in the dialog box return.
     * Simulink will generate an error indicating that there is a
     * parameter mismatch.
     */
    mdlCheckParameters(S);
    if (ssGetErrorStatus(S) != NULL) {
      return;
    }
  } else {
    /* Return if number of expected != number of actual parameters */
    return;
  }

#endif

  /* Set the parameter's tunability */
  ssSetSFcnParamTunable(S, 0, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable(S, 1, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable(S, 2, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable(S, 3, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable(S, 4, SS_PRM_NOT_TUNABLE);
  ssSetSFcnParamTunable(S, 5, SS_PRM_NOT_TUNABLE);

  /*
   * Set the number of pworks.
   */
  ssSetNumPWork(S, 0);

  /*
   * Set the number of dworks.
   */
  if (!ssSetNumDWork(S, 0))
    return;

  /*
   * Set the number of input ports.
   */
  if (!ssSetNumInputPorts(S, 0))
    return;  

  /*
   * Set the number of output ports.
   */
  if (!ssSetNumOutputPorts(S, 0))
    return;

  /*
   * Register reserved identifiers to avoid name conflict
   */
  if (ssRTWGenIsCodeGen(S) || ssGetSimMode(S)==SS_SIMMODE_EXTERNAL) {
    /*
     * Register reserved identifier for StartFcnSpec
     */
  }

  /*
   * This S-function can be used in referenced model simulating in normal mode.
   */
  ssSetModelReferenceNormalModeSupport(S, MDL_START_AND_MDL_PROCESS_PARAMS_OK);

  /*
   * Set the number of sample time.
   */
  ssSetNumSampleTimes(S, 0);

  /*
   * All options have the form SS_OPTION_<name> and are documented in
   * matlabroot/simulink/include/simstruc.h. The options should be
   * bitwise or'd together as in
   *   ssSetOptions(S, (SS_OPTION_name1 | SS_OPTION_name2))
   */
  /*ssSetOptions(S,
               SS_OPTION_USE_TLC_WITH_ACCELERATOR |
               SS_OPTION_CAN_BE_CALLED_CONDITIONALLY |
               SS_OPTION_EXCEPTION_FREE_CODE |
               SS_OPTION_WORKS_WITH_CODE_REUSE |
               SS_OPTION_SFUNCTION_INLINED_FOR_RTW |
               SS_OPTION_DISALLOW_CONSTANT_SAMPLE_TIME);
   */
}


/* Function: mdlInitializeSampleTimes =====================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
  /*double * const sampleTime = mxGetPr(SAMPLE_TIME);
  const  size_t stArraySize = mxGetM(SAMPLE_TIME) * mxGetN(SAMPLE_TIME);
  ssSetSampleTime(S, 0, sampleTime[0]);
  if (stArraySize == 1) {
    ssSetOffsetTime(S, 0, (sampleTime[0] == CONTINUOUS_SAMPLE_TIME?
      FIXED_IN_MINOR_STEP_OFFSET: 0.0));
  } else {
    ssSetOffsetTime(S, 0, sampleTime[1]);
  }

#if defined(ssSetModelReferenceSampleTimeDefaultInheritance)

  ssSetModelReferenceSampleTimeDefaultInheritance(S);

#endif
 */

}

#undef MDL_SET_WORK_WIDTHS
#if defined(MDL_SET_WORK_WIDTHS) && defined(MATLAB_MEX_FILE)

/* Function: mdlSetWorkWidths =============================================
 * Abstract:
 *      The optional method, mdlSetWorkWidths is called after input port
 *      width, output port width, and sample times of the S-function have
 *      been determined to set any state and work vector sizes which are
 *      a function of the input, output, and/or sample times.
 *
 *      Run-time parameters are registered in this method using methods
 *      ssSetNumRunTimeParams, ssSetRunTimeParamInfo, and related methods.
 */
static void mdlSetWorkWidths(SimStruct *S)
{
  /* Set number of run-time parameters */
  if (!ssSetNumRunTimeParams(S, 6))
    return;
}
#endif

#define MDL_RTW
void mdlRTW(SimStruct *S)
{
   
    //int32_T numBits = (int32_T) mxGetPr(ssGetSFcnParam(S,3))[0];
    //int32_T numMode = (int32_T) mxGetPr(ssGetSFcnParam(S,4))[0];
    //int32_T numFreq = (int32_T) mxGetPr(ssGetSFcnParam(S,5))[0];
    

     if (!ssWriteRTWParamSettings(S, 6,
                    SSWRITE_VALUE_STR,  "pinMosi", mxArrayToString(ssGetSFcnParam(S,0)),
                    SSWRITE_VALUE_STR,  "pinMiso", mxArrayToString(ssGetSFcnParam(S,1)),
                    SSWRITE_VALUE_STR,  "pinSclk", mxArrayToString(ssGetSFcnParam(S,2)),
                    SSWRITE_VALUE_STR,  "FormatBits", mxArrayToString(ssGetSFcnParam(S,3)),
                    SSWRITE_VALUE_STR,  "FormatMode", mxArrayToString(ssGetSFcnParam(S,4)),
                    SSWRITE_VALUE_STR,  "Frequency", mxArrayToString(ssGetSFcnParam(S,5))))
                    //SSWRITE_VALUE_DTYPE_NUM,  "FormatBits",   &numBits, DTINFO(SS_INT32, COMPLEX_NO),
                    //SSWRITE_VALUE_DTYPE_NUM,  "FormatMode",   &numMode, DTINFO(SS_INT32, COMPLEX_NO),
                    //SSWRITE_VALUE_DTYPE_NUM,  "Frequency",    &numFreq, DTINFO(SS_INT32, COMPLEX_NO)))
    {
            ssSetErrorStatus(S,"ssWriteRTWParamSettings error in mdlRTW");
            return;
    }
     
}


#define MDL_START
#if defined(MDL_START)

/* Function: mdlStart =====================================================
 * Abstract:
 *    This function is called once at start of model execution. If you
 *    have states that should be initialized once, this is the place
 *    to do it.
 */
static void mdlStart(SimStruct *S)
{
  /*
   * Get access to Parameter/Input/Output/DWork/size information
   */
}

#endif

/* Function: mdlOutputs ===================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block. Generally outputs are placed in the output vector(s),
 *    ssGetOutputPortSignal.
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
  /*
   * Get access to Parameter/Input/Output/DWork/size information
   */
    //ssPrintf("Hier %s \n",mxArrayToString(ssGetSFcnParam(S,5)));//mxArrayToString(ssGetSFcnParam(S,0)
}

/* Function: mdlTerminate =================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.
 */
static void mdlTerminate(SimStruct *S)
{
}

/* Function: IsRealMatrix =================================================
 * Abstract:
 *      Verify that the mxArray is a real (double) finite matrix
 */
static bool IsRealMatrix(const mxArray * const m)
{
  if (mxIsNumeric(m) &&
      mxIsDouble(m) &&
      !mxIsLogical(m) &&
      !mxIsComplex(m) &&
      !mxIsSparse(m) &&
      !mxIsEmpty(m) &&
      mxGetNumberOfDimensions(m) == 2) {
    const double * const data = mxGetPr(m);
    const size_t numEl = mxGetNumberOfElements(m);
    size_t i;
    for (i = 0; i < numEl; i++) {
      if (!mxIsFinite(data[i])) {
        return(false);
      }
    }

    return(true);
  } else {
    return(false);
  }
}

/*
 * Required S-function trailer
 */
#ifdef MATLAB_MEX_FILE
# include "simulink.c"
#else
# include "cg_sfun.h"
#endif
