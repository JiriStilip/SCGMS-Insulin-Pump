#pragma once

#include <stdint.h>

constexpr uint32_t DHT11_Timeout_us = 1000000;

class CSensor_DHT11
{
    private:
        uint8_t mPin;

        uint8_t* mTemperature;
        uint8_t* mHumidity;

        void Delay_Microseconds(uint32_t us);
        int Expect_Level(uint8_t level);
    protected:
    public:
        CSensor_DHT11(uint8_t gpio_pin);

        void Read();
        double Get_Temperature();
        double Get_Humidity();
};

extern CSensor_DHT11 sSensor_DHT11;