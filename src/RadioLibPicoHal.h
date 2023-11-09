#include <RadioLib.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/spi.h"

class PiPicoHal : public RadioLibHal {
public:
    PiPicoHal(spi_inst_t *spi, uint32_t spi_speed = 2000000)
        : RadioLibHal(0, 1, 0, 1, 0, 1),  
          _spi(spi),
          _spi_speed(spi_speed) {
    }

    void init() override {
        stdio_init_all();

        spiBegin();

        // any other enable steps we need to do?
        //gpio_init(18);
        //gpio_set_dir(18, GPIO_OUT);
        //gpio_put(18, 1);
    }

    void term() override {
        spiEnd();

        //gpio_init(18);
        //gpio_set_dir(18, GPIO_OUT);
        //gpio_put(18, 0);

    }

    void pinMode(uint32_t pin, uint32_t mode) override {
        if (pin == RADIOLIB_NC) {
            return;
        }
        gpio_init(pin);
        gpio_set_dir(pin, mode);
    }

    void digitalWrite(uint32_t pin, uint32_t value) override {
        if (pin == RADIOLIB_NC) {
            return;
        }
        gpio_put(pin, value);
    }

    uint32_t digitalRead(uint32_t pin) override {
        if (pin == RADIOLIB_NC) {
            return 0;
        }
        return gpio_get(pin);
    }
    
    void attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode) override {
        if (interruptNum == RADIOLIB_NC) {
            return;
        }

        // Pulled from the arduino-pico core

        uint32_t events;
        switch (mode) {
        case LOW:     events = 1; break;
        case HIGH:    events = 2; break;
        case FALLING: events = 4; break;
        case RISING:  events = 8; break;
        case CHANGE:  events = 4 | 8; break;
        default:      return;  // ERROR
        }

        // Setting the interrupt with the Raspberry Pi Pico SDK
        gpio_set_irq_enabled_with_callback(interruptNum, events, true, (gpio_irq_callback_t)interruptCb);
    }

    void detachInterrupt(uint32_t interruptNum) override {
        if (interruptNum == RADIOLIB_NC) {
            return;
        }

        // Disabling the interrupt with the Raspberry Pi Pico SDK
        gpio_set_irq_enabled(pin, 0x0f, false); // pulled from arduino-pico core wiring_private.cpp
    }

    void delay(unsigned long ms) override {
        sleep_ms(ms);
    }

    void delayMicroseconds(unsigned long us) override {
        sleep_us(us);
    }

    unsigned long millis() override {
        return to_ms_since_boot(get_absolute_time());
    }

    unsigned long micros() override {
        return to_us_since_boot(get_absolute_time()) / 1000;
    }

    long pulseIn(uint32_t pin, uint32_t state, unsigned long timeout) override {
        // taken from wiring_pulse.cpp from arduino-pico
        uint64_t start = time_us_64();
        uint64_t abort = start + timeout;

        if (pin > 29) {
            DEBUGCORE("ERROR: Illegal pin in pulseIn (%d)\n", pin);
            return 0;
        }

        // Wait for deassert, if needed
        while ((!!gpio_get(pin) != !state) && (time_us_64() < abort));
        if (time_us_64() >= abort) {
            return 0;
        }

        // Wait for assert
        while ((!!gpio_get(pin) != !!state) && (time_us_64() < abort));
        uint64_t begin = time_us_64();
        if (begin >= abort) {
            return 0;
        }

        // Wait for deassert
        while ((!!gpio_get(pin) != !state) && (time_us_64() < abort));
        uint64_t end = time_us_64();
        if (end >= abort) {
            return 0;
        }

        return end - begin;
    }

    void spiBegin() {
        spi_init(_spi, _spi_speed);
        spi_set_format(_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    }

    void spiBeginTransaction() {}
    void spiEndTransaction() {}

    void spiTransfer(uint8_t *out, size_t len, uint8_t *in) {
        spi_write_blocking(_spi, out, len);
        spi_read_blocking(_spi, 0, in, len);
    }

    void spiEnd() {
        spi_deinit(_spi);
    }

private:
    const uint32_t _spi_speed;
    spi_inst_t *_spi;
    int _spiHandle = -1;
};
