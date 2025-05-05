#pragma once

#include <stdint.h>

// na teto adrese posloucha kazdy displej SSD1306
constexpr uint8_t SSD1306_Slave_Address = 0x3C;

// zakladni selekce prikazu displeje SSD1306
enum SSD1306_Cmd
{
    Command_Start               = 0x00,
    Data_Start                  = 0xC0,
    Data_Continue               = 0x40,

    Set_Contrast                = 0x81,
    Display_All_On_Resume       = 0xA4,
    Display_All_On              = 0xA5,
    Normal_Display              = 0xA6,
    Inverted_Display            = 0xA7,

    Display_Off                 = 0xAE,
    Display_On                  = 0xAF,

    Nop                         = 0xE3,

    Horizontal_Scroll_Right     = 0x26,
    Horizontal_Scroll_Left      = 0x27,
    Horizontal_Scroll_Vrt_Right = 0x29,
    Horizontal_Scroll_Vrt_Left  = 0x2A,
    Deactivate_Scroll           = 0x2E,
    Activate_Scroll             = 0x2F,
    Set_Vrt_Scroll_Area         = 0xA3,

    Set_Lower_Column            = 0x00,
    Set_Upper_Column            = 0x10,
    Memory_Addr_Mode            = 0x20,
    Set_Column_Addr             = 0x21,
    Set_Page_Addr               = 0x22,
    Set_Start_Line              = 0x40,
    Set_Segment_Remap           = 0xA0,
    Set_Multiplex_Ratio         = 0xA8,
    Com_Scan_Dir_Inc            = 0xC0,
    Com_Scan_Dir_Dec            = 0xC8,
    Set_Display_Offset          = 0xD3,
    Set_Com_Pins                = 0xDA,
    Charge_Pump                 = 0x8D,

    Set_Display_Clock_Div_Ratio = 0xD5,
    Set_Precharge_Period        = 0xD9,
    Set_VCOM_Detect             = 0xD8,
};

// SSD1306 OLED displej, datasheet: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
class CDisplay_SSD1306
{
    private:
        // buffer pro bitovou mapu pixelu
        uint8_t* mBuffer;
        // sirka a vyska displeje
        uint8_t mWidth, mHeight;

    protected:
        // odesle pres I2C prikazovou sekvenci do displeje
        void Send_Command_Sequence(uint8_t *cmd_seq, uint8_t n);

    public:
        CDisplay_SSD1306();

        // otevre displej; definujeme velikost matice displeje v pixelech
        // predpoklada provedenou inicializaci i2c
        void Init(uint8_t width = 128, uint8_t height = 64);

        // vymaze obsah displeje
        void Clear(bool clearWhite = false);
        // nastavi pixel na danych souradnicich
        void Set_Pixel(uint16_t x, uint16_t y, bool set);
        // vykresli znak na vystup displeje z interni sady znaku
        void Put_Char(uint16_t x, uint16_t y, char c);
        // vykresli retezec znaku na displej
        void Put_String(uint16_t x, uint16_t y, const char* str);
        // "preklopi" buffer z pameti do displeje
        void Flip();
};

extern CDisplay_SSD1306 sDisplay_SSD1306;