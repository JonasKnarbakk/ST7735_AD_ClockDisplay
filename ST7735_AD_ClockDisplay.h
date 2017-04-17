#ifndef ST7735_AD_CLOCKDISPLAY_HPP
#define ST7735_AD_CLOCKDISPLAY_HPP

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Time.h>

// To keep track of analog watch hands
// so we can erase them on update
struct drawnLine {
    int x;
    int y;
    int xDestination;
    int yDestination;
    float angle;
};

class ST7335_AD_ClockDisplay{
    public:
        ST7335_AD_ClockDisplay(uint8_t CS_PIN, uint8_t DC_PIN, uint8_t RST_PIN);
        ~ST7335_AD_ClockDisplay();

        void init();
        void setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t month, uint8_t year);
        void drawClock();
        void redraw();
        void showAlarm();
        void hideAlarm();
        void showDate();
        void hideDate();
        void setAnalog() { m_analog = true; }
        void setDigital() { m_analog = false; }
        bool isAnalog() { return m_analog; }
        void incrementFgColor();
        void incrementBgColor();

        Adafruit_ST7735 getScreen() { return m_screen; }
        time_t getTimer() { return m_timer; }
        void setTimer(time_t time) { m_timer = time; }

        int getCurrentSecond() { return m_currentSecond; }
        int getCurrentMinute() { return m_currentMinute; }
        int getCurrentHour() { return m_currentHour; }
        int getCurrentDay() { return m_currentDay; }
        int getCurrentWeekday() { return m_currentWeekday; }

        int getLastSecond() { return m_lastSecond; }
        int getLastMinute() { return m_lastMinute; }
        int getLastHour() { return m_lastHour; }
        int getLastDay() { return m_lastDay; }
        int getLastWeekday() { return m_lastWeekday; }

    private:
        Adafruit_ST7735 m_screen;

        bool m_analog = false,
            m_alarm = false,
            m_date = false;

        time_t m_timer;

        // Pins
        uint8_t m_SCLK = 0,
            m_MOSI = 0,
            m_CS = 0,
            M_RST = 0,
            m_DC = 0;

        // Timer variables to check if we need
        // to redraw a value
        uint8_t m_lastSecond = 0,
            m_currentSecond = 0,
            m_lastMinute = 0,
            m_currentMinute = 56,
            m_currentHour = 20,
            m_lastHour = 0,
            m_currentDay = 0,
            m_lastDay = 0,
            m_currentWeekday = 1,
            m_lastWeekday = 0;

        // Alarm values
        uint8_t m_alarmMinute = 0,
            m_alarmHour = 0;

        // Colors
        uint16_t m_colors[8] = {
            ST7735_BLACK,
            ST7735_BLUE,
            ST7735_RED,
            ST7735_GREEN,
            ST7735_CYAN,
            ST7735_MAGENTA,
            ST7735_YELLOW,
            ST7735_WHITE
        };

        // Index for what color to use
        uint8_t fgIndex = 7;
        uint8_t bgIndex = 0;

        // Current color for background and foreground
        uint16_t m_fgColor = m_colors[fgIndex];
        uint16_t m_bgColor = m_colors[bgIndex];

        // Analog clock hands, so they can be erased when moved
        drawnLine m_drawnLines[3];

        void drawDigitalClock(uint16_t color);
        void drawAnalogClock(uint16_t color);
        void drawWatchFace(uint8_t width, uint8_t height, uint8_t radius, uint16_t color);
        void drawWatchIndicators(uint8_t width, uint8_t height, uint8_t radius, uint16_t color);
        void drawHand(uint8_t x, uint8_t y, float angle, uint8_t lenght, uint8_t index, uint16_t color);
        void cleanUpLines();
        void ForceCleanUpLines();
        bool isProblemArea(float angle);
        void drawDate(uint8_t x, uint8_t y, uint16_t color);
        void drawAlarm(uint8_t x, uint8_t y, uint16_t color);
        void drawTextCentered(uint8_t x, uint8_t y, const char *string, uint8_t size, uint16_t color);
};

#endif
