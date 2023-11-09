// include the library
#include <RadioLib.h>

// include the hardware abstraction layer
#include "RadioLibPicoHal.h"
#include "hardware/spi.h" // included for getting spi_default

// create a new instance of the HAL class
// use SPI channel 1, because on Waveshare LoRaWAN Hat,
// the SX1261 CS is connected to CE1
PiPicoHal* hal = new PiPicoHal(spi_default);

// now we can create the radio module
// pinout corresponds to the Waveshare LoRaWAN Hat
// NSS pin:   7
// DIO1 pin:  17
// NRST pin:  22
// BUSY pin:  not connected
RFM98 radio = new Module(hal, 7, 17, 22, RADIOLIB_NC);

// the entry point for the program
int main(int argc, char** argv) {
  // initialize just like with Arduino
  printf("[SX1261] Initializing ... ");
  int state = radio.begin();
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  // loop forever
  for(;;) {
    // send a packet
    printf("[SX1261] Transmitting packet ... ");
    state = radio.transmit("Hello World!");
    if(state == RADIOLIB_ERR_NONE) {
      // the packet was successfully transmitted
      printf("success!\n");

      // wait for a second before transmitting again
      hal->delay(1000);

    } else {
      printf("failed, code %d\n", state);

    }

  }

  return(0);
}