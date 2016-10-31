#ifndef MAX6675_h
#define MAX6675_h

#include "mbed.h"

class max6675
{
    SPI& spi;
    DigitalOut ncs;
    
  public:
	//max6675(PinName _SCLK, PinName _CS, PinName _MISO);
	max6675(SPI& _SPI, PinName _CS);
    void select();
    void deselect();
    float read_temp();
  private:
    //PinName cs;
    //PinName miso;
    //PinName sclk;
};

#endif
