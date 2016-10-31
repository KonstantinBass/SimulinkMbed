/*
 * sfcnAnalogIn.c 
 * Created by K.Bass
 * 2015-03-01
 */

/*
 * Must specify the S_FUNCTION_NAME as the name of the S-function.
 */
#define S_FUNCTION_NAME                sfcnAnalogIn
#define S_FUNCTION_LEVEL               2

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
#include "string.h"

#define EDIT_OK(S, P_IDX) \
 (!((ssGetSimMode(S)==SS_SIMMODE_SIZES_CALL_ONLY) && mxIsEmpty(ssGetSFcnParam(S, P_IDX))))
#define SAMPLE_TIME                    (ssGetSFcnParam(S, 2))
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
   * Check the parameter 1 - ReadFunction
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
   * Check the parameter 1 - PIN
   */
  if EDIT_OK(S, 1) {
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
   * Check the parameter 2 (sample time)
   */
  if EDIT_OK(S, 2) {
    const double * sampleTime = NULL;
    const size_t stArraySize = mxGetM(SAMPLE_TIME) * mxGetN(SAMPLE_TIME);

    /* Sample time must be a real scalar value or 2 element array. */
    if (IsRealMatrix(SAMPLE_TIME) &&
        (stArraySize == 1 || stArraySize == 2) ) {
      sampleTime = mxGetPr(SAMPLE_TIME);
    } else {
      ssSetErrorStatus(S,
                       "Invalid sample time. Sample time must be a real scalar value or an array of two real values.");
      return;
    }

    if (sampleTime[0] < 0.0 && sampleTime[0] != -1.0) {
      ssSetErrorStatus(S,
                       "Invalid sample time. Period must be non-negative or -1 (for inherited).");
      return;
    }

    if (stArraySize == 2 && sampleTime[0] > 0.0 &&
        sampleTime[1] >= sampleTime[0]) {
      ssSetErrorStatus(S,
                       "Invalid sample time. Offset must be smaller than period.");
      return;
    }

    if (stArraySize == 2 && sampleTime[0] == -1.0 && sampleTime[1] != 0.0) {
      ssSetErrorStatus(S,
                       "Invalid sample time. When period is -1, offset must be 0.");
      return;
    }

    if (stArraySize == 2 && sampleTime[0] == 0.0 &&
        !(sampleTime[1] == 1.0)) {
      ssSetErrorStatus(S,
                       "Invalid sample time. When period is 0, offset must be 1.");
      return;
    }
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
  int swiReadFcn = (int) mxGetPr(ssGetSFcnParam(S,0))[0];
  //1 -> strReadFcn0[] = "read_u16()";
  //2 -> strReadFcn1[] = "read()";
  
  //ssPrintf("Hier %i", swiReadFcn);

  /* Number of expected parameters */
  ssSetNumSFcnParams(S, 3);

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
  ssSetNumInputPorts(S, 0);

  /*
   * Set the number of output ports.
   */
  ssSetNumOutputPorts(S, 1);
  
  ssSetOutputPortWidth(S, 0, 1);
  ssSetOutputPortComplexSignal(S, 0, COMPLEX_NO);
  ssSetOutputPortOptimOpts(S, 0, SS_REUSABLE_AND_LOCAL);
  //ssSetOutputPortOutputExprInRTW(S, 0, 1);
  // ssSetOutputPortDataType
  //swiReadFcn = (int) mxGetPr(ssGetSFcnParam(S,0))[0];
  if (swiReadFcn == 1)
    ssSetOutputPortDataType(S, 0, SS_UINT16);
  else if (swiReadFcn == 2)
      ssSetOutputPortDataType(S, 0, SS_DOUBLE);
  else
      ssSetErrorStatus(S, "Invalid Read Function Parameter (0 or 1)!");
    
//   ssPrintf("Hier %s",p);

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
  ssSetNumSampleTimes(S, 1);

  /*
   * All options have the form SS_OPTION_<name> and are documented in
   * matlabroot/simulink/include/simstruc.h. The options should be
   * bitwise or'd together as in
   *   ssSetOptions(S, (SS_OPTION_name1 | SS_OPTION_name2))
   */
  ssSetOptions(S,
               SS_OPTION_USE_TLC_WITH_ACCELERATOR |
               SS_OPTION_CAN_BE_CALLED_CONDITIONALLY |
               SS_OPTION_EXCEPTION_FREE_CODE |
               SS_OPTION_WORKS_WITH_CODE_REUSE |
               SS_OPTION_SFUNCTION_INLINED_FOR_RTW |
               SS_OPTION_DISALLOW_CONSTANT_SAMPLE_TIME);
}


/* Function: mdlInitializeSampleTimes =====================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
  double * const sampleTime = mxGetPr(SAMPLE_TIME);
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

}

//#define MDL_SET_WORK_WIDTHS
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
  if (!ssSetNumRunTimeParams(S, 2))
    return;
}
#endif

#define MDL_RTW
void mdlRTW(SimStruct *S)
{
    int swiReadFcn = (int) mxGetPr(ssGetSFcnParam(S,0))[0];
    //1 -> strReadFcn0[] = "read_u16()";
    //2 -> strReadFcn1[] = "read()";
    char strReadFcn0[] = "read_u16()";
    char strReadFcn1[] = "read()";
    char *p;
    char *pin;
    
    pin = mxArrayToString(ssGetSFcnParam(S,1));

    if (swiReadFcn == 1){
        //ssSetOutputPortDataType(S, 0, SS_UINT16);
         p = strReadFcn0;
    } 
    else if (swiReadFcn == 2){
        //ssSetOutputPortDataType(S, 0, SS_DOUBLE);
        p = strReadFcn1;
    }
    else
        ssSetErrorStatus(S, "Invalid Read Function Parameter (0 or 1)!");
    
    if (!ssWriteRTWParamSettings(S, 2,
                    SSWRITE_VALUE_STR,        "ReadFcn",     p,
                    SSWRITE_VALUE_STR,        "PinName",     pin))
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
