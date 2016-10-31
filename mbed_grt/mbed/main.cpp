//
// File: main.cpp
//
// Code generated for Simulink model 'test_1'.
//
// Model version                  : 1.15
// Simulink Coder version         : 8.6 (R2014a) 27-Dec-2013
// C/C++ source code generated on : Sat Feb 14 16:52:32 2015
//
// Target selection: mbed.tlc
// Embedded hardware selection: Atmel->AVR
// Code generation objectives: Unspecified
// Validation result: Not run
//
#include "test_1.h"
#include "mbed.h"

static boolean_T OverrunFlag = 0;
void rt_OneStep()
{
  // Disable interrupts here

  // Check for overun
  if (OverrunFlag++) {
    rtmSetErrorStatus(test_1_M, "Overrun");
    return;
  }

  // Save FPU context here (if necessary)
  // Re-enable timer or interrupt here
  // Remove conditional, and set model inputs here
#if 0

  // InportID: 0, TaskID: 0
  test_1_U.In1 = your_value;

#endif

  test_1_step();

  // Get model outputs here

  // Remove conditional, and get model outputs here
#if 0

  // OutportID: 0, TaskID: 0
  your_variable = test_1_Y.Out1;

#endif

  OverrunFlag--;

  // Disable interrupts here
  // Restore FPU context here (if necessary)
  // Enable interrupts here
}

int_T main(int_T argc, const char *argv[])
{
  // Unused arguments
  (void)(argc);
  (void)(argv);

  // Initialize model
  test_1_initialize();

  // Associate rt_OneStep() with a timer that executes at the base rate of the model 
  test_1_terminate();
  return 0;
}

//
// File trailer for generated code.
//
// [EOF]
//
