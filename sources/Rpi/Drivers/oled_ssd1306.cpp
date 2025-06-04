#include "oled_ssd1306.h"
#include "oled_font.h"

#include "i2c.h"

CDisplay_SSD1306 sDisplay_SSD1306{};

CDisplay_SSD1306::CDisplay_SSD1306()
    : mBuffer(nullptr), mWidth(0), mHeight(0)
{

}

void CDisplay_SSD1306::Init(uint16_t width, uint16_t height)
{
    // zaokrouhlime nahoru na nasobek osmi (na cele stranky)
    if (height % 8 != 0)
        height += 8 - (height % 8);

    mWidth = width;
    mHeight = height;

    // alokujeme si buffer, ten je velky tak jako displej
    // dalo by se to optimalizovat, napr. primym kreslenim, bufferovanim vyrezu, apod.
    // ale to my ted nepotrebujeme, obzvlast kdyz spotrebujeme max jednotky kB a mame k dispozici >512MB RAM
    mBuffer = new uint8_t[mWidth * mHeight / 8];


    // vypneme displej, nastavime clock ratio (z datasheetu, doporucena hodnota), nastavime multiplex ratio (vysku displeje)
    static uint8_t init1[] = {SSD1306_Cmd::Command_Start,
                              SSD1306_Cmd::Display_Off,
                              SSD1306_Cmd::Set_Display_Clock_Div_Ratio,
                              0x80,
                              SSD1306_Cmd::Set_Multiplex_Ratio};
    Send_Command_Sequence(init1, sizeof(init1));

    // pokracovani predchoziho - multiplex ratio
    static uint8_t init1_1[] = {SSD1306_Cmd::Command_Start,
                                static_cast<uint8_t>(height - 1)};
    Send_Command_Sequence(init1_1, sizeof(init1_1));


    // nastavime display offset (pametovy offset a jeho matching na realnou matici), pocatecni radek a vlastnosti vnitrniho menice (nabojova pumpa)
    static uint8_t init2[] = {SSD1306_Cmd::Command_Start,
                              SSD1306_Cmd::Set_Display_Offset,
                              0x00,
                              SSD1306_Cmd::Set_Start_Line | 0x00,
                              SSD1306_Cmd::Charge_Pump};
    Send_Command_Sequence(init2, sizeof(init2));

    // pokracovani predchoziho - nabojova pumpa, 0x14 je hodnota z datasheetu pro konkretni displej
    static uint8_t init2_1[] = {SSD1306_Cmd::Command_Start,
                                0x14};
    Send_Command_Sequence(init2_1, sizeof(init2_1));


    // nastavime mod adresace (0x00 - po sloupcich a po strankach) a segment remapping (to, jak je displej "obraceny")
    // "smer" skenovani vystupu ridiciho obvodu
    static uint8_t init3[] = {SSD1306_Cmd::Command_Start,
                              SSD1306_Cmd::Memory_Addr_Mode,
                              0x00,
                              SSD1306_Cmd::Set_Segment_Remap | 0x01,
                              SSD1306_Cmd::Com_Scan_Dir_Dec};
    Send_Command_Sequence(init3, sizeof(init3));


    // mapovani na piny ridiciho obvodu
    static uint8_t init4[] = {SSD1306_Cmd::Command_Start,
                              SSD1306_Cmd::Set_Com_Pins};
    Send_Command_Sequence(init4, sizeof(init4));

    // pokracovani predchoziho - neinvertovany a sekvencni pristup
    static uint8_t init4_1[] = {SSD1306_Cmd::Command_Start,
                                0x12};
    Send_Command_Sequence(init4_1, sizeof(init4_1));

    // nastaveni kontrastu displeje
    static uint8_t init4_2[] = {SSD1306_Cmd::Command_Start,
                                SSD1306_Cmd::Set_Contrast};
    Send_Command_Sequence(init4_2, sizeof(init4_2));

    // pokracovani predchoziho - kontrast nastaven na 0x8F
    static uint8_t init4_3[] = {SSD1306_Cmd::Command_Start,
                                0xCF};
    Send_Command_Sequence(init4_3, sizeof(init4_3));

    // perioda prednabiti
    static uint8_t init4_4[] = {SSD1306_Cmd::Command_Start,
                                SSD1306_Cmd::Set_Precharge_Period};
    Send_Command_Sequence(init4_4, sizeof(init4_4));

    // pokracovani predchoziho - pro externi napajeni muze byt tato hodnota kratsi
    static uint8_t init4_5[] = {SSD1306_Cmd::Command_Start,
                                0xF1};
    Send_Command_Sequence(init4_5, sizeof(init4_5));

    // finalni aktivace displeje - uroven detekce vstupu, nahozeni panelu, neinvertovane barvy, neskrolujeme, zapneme podsviceni
    static uint8_t init5[] = {SSD1306_Cmd::Command_Start,
                              SSD1306_Cmd::Set_VCOM_Detect,
                              0x40,
                              SSD1306_Cmd::Display_All_On_Resume,
                              SSD1306_Cmd::Normal_Display,
                              SSD1306_Cmd::Deactivate_Scroll,
                              SSD1306_Cmd::Display_On};
    Send_Command_Sequence(init5, sizeof(init5));

    Clear();
}

void CDisplay_SSD1306::Send_Command_Sequence(uint8_t *cmd_seq, uint8_t n)
{
    i2c_write(SSD1306_Slave_Address, cmd_seq, n);
}

void CDisplay_SSD1306::Clear(bool clearWhite)
{
    const uint8_t clearColor = clearWhite ? 0xFF : 0x00;

    const int maxIdx = mWidth * (mHeight / 8);

    for (int i = 0; i < maxIdx; i++)
        mBuffer[i] = clearColor;

    Flip();
}

void CDisplay_SSD1306::Set_Pixel(uint16_t x, uint16_t y, bool set)
{
    if (set)
        mBuffer[x + (y / 8) * mWidth] |= (1 << (y & 7));
    else
        mBuffer[x + (y / 8) * mWidth] &= ~(1 << (y & 7));
}

void CDisplay_SSD1306::Put_VLine(uint16_t x)
{
    for (int y = 0; y < mHeight; y++)
        Set_Pixel(x, y, true);
}

void CDisplay_SSD1306::Put_HLine(uint16_t y)
{
    for (int x = 0; x < mWidth; x++)
        Set_Pixel(x, y, true);
}

void CDisplay_SSD1306::Fill_Rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool fillWhite)
{
    for (int pos_x = x; pos_x < (x + w); pos_x++)
    {
        for (int pos_y = y; pos_y < (y + h); pos_y++)
        {
            Set_Pixel(pos_x, pos_y, fillWhite);
        }
    }
}

void CDisplay_SSD1306::Put_Char(uint16_t x, uint16_t y, char c)
{
    // umime jen nektere znaky
    if (c < OLED_Font::Char_Begin || c >= OLED_Font::Char_End)
        return;

    // obsah puvodniho COLED_Display::Put_Char()
    uint16_t w = OLED_Font::Char_Width;
    uint16_t h = OLED_Font::Char_Height;
    uint8_t vflip = OLED_Font::Flip_Chars ? 1 : 0;
    const uint8_t* data = &OLED_Font::OLED_Font_Default[OLED_Font::Char_Width * (((uint16_t)c) - OLED_Font::Char_Begin)];

    // obsah puvodniho CDisplay_SSD1306::Process_External_Command() s prikazem NDisplay_Command::Draw_Pixel_Array_To_Rect
    if (vflip == 0)
    {
        for (uint16_t pos_x = x; pos_x < x + w; pos_x++)
        {
            for (uint16_t pos_y = y; pos_y < y + h; pos_y++)
            {
                const uint16_t pos = ((pos_y - y) * w + (pos_x - x));

                Set_Pixel(pos_x, pos_y, ((data[pos / 8] >> (7 - (pos % 8))) & 0x1) != 0);
            }
        }
    }
    else
    {
        for (uint16_t pos_x = 0; pos_x < w; pos_x++)
        {
            for (uint16_t pos_y = 0; pos_y < h; pos_y++)
            {
                const uint16_t pos = (pos_x * h + pos_y);

                Set_Pixel(pos_x + x, (h - pos_y) + y, ((data[pos / 8] >> (7 - (pos % 8))) & 0x1) != 0);
            }
        }
    }
}

void CDisplay_SSD1306::Put_String(uint16_t x, uint16_t y, const char* str)
{
    uint16_t xi = x;
    const char* ptr = str;
    // dokud nedojdeme na konec retezce nebo dokud nejsme 64 znaku daleko (limit, kdyby nahodou se neco pokazilo)
    while (*ptr != '\0' && ptr - str < 64)
    {
        Put_Char(xi, y, *ptr);
        xi += OLED_Font::Char_Width;
        ptr++;
    }
}

void CDisplay_SSD1306::Flip()
{
    // nastavime kurzor na levy horni roh
    static uint8_t flip1[] = {SSD1306_Cmd::Command_Start,
                              SSD1306_Cmd::Set_Page_Addr,
                              0x00,
                              0xFF,
                              SSD1306_Cmd::Set_Column_Addr,
                              0x00};
    Send_Command_Sequence(flip1, sizeof(flip1));

    static uint8_t flip2[] = {SSD1306_Cmd::Command_Start,
                              static_cast<uint8_t>(mWidth - 1)};
    Send_Command_Sequence(flip2, sizeof(flip2));

    // budeme posilat pixely po balicich 4 sloupcu (po 8 pixelech)
    constexpr int PktSize = 4;

    uint8_t packet[PktSize + 1];

    const int maxIdx = mWidth * (mHeight / 8);

    for (int i = 0; i < maxIdx; i += PktSize)
    {
        packet[0] = SSD1306_Cmd::Data_Continue;

        for (int j = 0; j < PktSize; ++j) 
        {
            packet[j + 1] = mBuffer[i + j];
        }
        
        Send_Command_Sequence(packet, sizeof(packet));
    }
}