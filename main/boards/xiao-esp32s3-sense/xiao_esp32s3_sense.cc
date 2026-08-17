#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h" 
#include "mcp_server.h"
#include "lamp_controller.h"

#include "shared_i2c_bus.h"

#include "led/gpio_led.h"
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/gpio.h>
#include <driver/i2s_std.h> 

#define TAG "XIAO_S3_Sense"
extern "C" {
    void audio_hardware_init(void);
}
// ==========================================
// 自定义 AudioCodec 类：适配 ReSpeaker I2S
// ==========================================
class ReSpeakerAudioCodec : public NoAudioCodecDuplex {
public:
    ReSpeakerAudioCodec()
        : NoAudioCodecDuplex(AUDIO_INPUT_SAMPLE_RATE,
              AUDIO_OUTPUT_SAMPLE_RATE,
              AUDIO_I2S_BCK_IO,
              AUDIO_I2S_WS_IO,
              AUDIO_I2S_DO_IO,
              AUDIO_I2S_DI_IO) {
        ESP_LOGI(TAG, "ReSpeaker I2S Driver Initialized (%dHz, 32bit)", AUDIO_INPUT_SAMPLE_RATE);
    }

    void SetOutputVolume(int volume) override {
        // 硬件音量已在 audio_bsp.c 设置，此处留空
        (void)volume;
    }
};


class XIAO_ESP32S3_Sense : public WifiBoard {
 private:
    Button boot_button_;
    Button user_button_;

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });

        user_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }

    void InitializeTools() {
    }

 public:
    XIAO_ESP32S3_Sense() :
        boot_button_(BOOT_BUTTON_GPIO),
        user_button_(USER_BUTTON_GPIO) {
        audio_hardware_init();
        InitializeButtons();
        InitializeTools();
    }

    virtual Led* GetLed() override {
        static GpioLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
        static ReSpeakerAudioCodec audio_codec;
        return &audio_codec;
    }

};

DECLARE_BOARD(XIAO_ESP32S3_Sense);