#include "dht11.h"

#include "gpio.h"
#include "timer.h"

CSensor_DHT11 sSensor_DHT11(PIN_DHT11_DATA);

CSensor_DHT11::CSensor_DHT11(uint8_t gpio_pin)
    : mPin(gpio_pin)
{
    mTemperature = new uint8_t[2];
    mHumidity = new uint8_t[2];
}

void CSensor_DHT11::Delay_Microseconds(uint32_t us)
{
    uint32_t start = micros();

    while ((micros() - start) < us);
}

int CSensor_DHT11::Expect_Level(uint8_t level)
{
    uint32_t start = micros();

    while (gpio_get_input(mPin) != level) {
        if ((micros() - start) > DHT11_Timeout_us) return -1;
    }

    return 0;
}

void CSensor_DHT11::Read()
{
    uint8_t data[5] = {0};

    // Send start signal
    gpio_set_function(mPin, GPIO_FUNCTION_OUTPUT);
    gpio_set_output(mPin, LOW);
    Delay_Microseconds(20000); // >18ms
    // gpio_set_output(mPin, HIGH);
    gpio_set_function(mPin, GPIO_FUNCTION_INPUT);
    Delay_Microseconds(20);

    // Wait for sensor response
    if (Expect_Level(LOW) < 0) return;
    if (Expect_Level(HIGH) < 0) return;
    if (Expect_Level(LOW) < 0) return;

    // Read 40 bits
    for (int i = 0; i < 40; ++i) {
        if (Expect_Level(HIGH) < 0) return;
        uint32_t start = micros();
        if (Expect_Level(LOW) < 0) return;
        uint32_t duration = micros() - start;

        int byte = i / 8;
        data[byte] <<= 1;
        if (duration > 50) {
            data[byte] |= 1;
        }
    }

    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) return;

    mHumidity[0] = data[0];
    mHumidity[1] = data[1];
    mTemperature[0] = data[2];
    mTemperature[1] = data[3];
}

double CSensor_DHT11::Get_Temperature()
{
    double temp = mTemperature[0];

    if (mTemperature[1] & 0x80) {
        temp = -1 - temp;
    }
    temp += (mTemperature[1] & 0x0f) * 0.1;

    return temp;
}

double CSensor_DHT11::Get_Humidity()
{
    return mHumidity[0] + mHumidity[1] * 0.1;
}