#include "wk_esp32s3_dev.h"

void WkEsp32s3Dev::InitDisplay() {
#ifdef CONFIG_LCD_DISPLAY
    // Khởi tạo LCD Display nếu cấu hình
#else
    display_ = new OledDisplay();
#endif
}

void WkEsp32s3Dev::UpdateDisplayAnimation() {}

std::string WkEsp32s3Dev::GetStatusText() {
    return "Ready";
}

void WkEsp32s3Dev::ShowEmotionDisplay(const std::string& emotion) {
    current_emotion_ = emotion;
    if (display_) {
        display_->SetEmotion(emotion.c_str());
    }
}

int WkEsp32s3Dev::BreathEffect(uint32_t time_ms, int speed) {
    float phase = (time_ms % (2000 / speed)) / (2000.0f / speed) * 2.0f * 3.14159f;
    return (int)((sin(phase) + 1.0f) * 127.5f);
}

int WkEsp32s3Dev::HeartbeatEffect(uint32_t time_ms) {
    uint32_t t = time_ms % 1000;
    if (t < 150) return 255;
    if (t < 300) return 50;
    if (t < 450) return 200;
    return 0;
}

int WkEsp32s3Dev::WaveEffect(uint32_t time_ms, int speed, int led_index) {
    return 128;
}

int WkEsp32s3Dev::CometEffect(uint32_t time_ms, int speed, int led_index) {
    return 128;
}

int WkEsp32s3Dev::TwinkleEffect(uint32_t time_ms, int speed, int led_index) {
    return (rand() % 255);
}

int WkEsp32s3Dev::PulseEffect(uint32_t time_ms, int speed, int led_index) {
    return BreathEffect(time_ms, speed);
}

void WkEsp32s3Dev::ApplyLedEffect(int led_pin, LedAnimation anim) {}

void WkEsp32s3Dev::SetLedTimeout(int duration_seconds) {
    led_timeout_ms_ = duration_seconds * 1000;
    led_timeout_start_ = (uint32_t)(esp_timer_get_time() / 1000);
}

void WkEsp32s3Dev::ExecuteEmotion(const std::string& emotion) {
    ShowEmotionDisplay(emotion);
}

void WkEsp32s3Dev::UpdateEmotionByState() {
    auto& app = Application::GetInstance();
    auto state = app.GetDeviceState();
    if (state == kDeviceStateListening) ShowEmotionDisplay("listening");
    else if (state == kDeviceStateSpeaking) ShowEmotionDisplay("speaking");
    else if (state == kDeviceStateThinking) ShowEmotionDisplay("thinking");
}

void WkEsp32s3Dev::UpdateLedCreative() {
    led_tick_ += 50;
    if (anim_led1_.active) {
        ApplyLedEffect(LED_1, anim_led1_);
    }
}

void WkEsp32s3Dev::LedCreativeTask(void* arg) {
    WkEsp32s3Dev* board = (WkEsp32s3Dev*)arg;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(50));
        board->UpdateLedCreative();
        if (board->emotion_auto_mode_) {
            board->UpdateEmotionByState();
        }
    }
}

void WkEsp32s3Dev::InitializeMotor() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
}

void WkEsp32s3Dev::SetLeftMotor(int speed) {
    // Điều khiển tốc độ và chiều động cơ trái
}

void WkEsp32s3Dev::SetRightMotor(int speed) {
    // Điều khiển tốc độ và chiều động cơ phải
}

void WkEsp32s3Dev::InitializeLedGpio() {}

void WkEsp32s3Dev::InitializeMotorMcp() {
#ifdef CONFIG_BOARD_WK_HAVE_MOTOR
    McpServer::GetInstance().AddTool("self.motor.set_speed", "Điều khiển tốc độ động cơ trái và phải", 
        [this](const cJSON* args, std::string& result) {
            cJSON *left = cJSON_GetObjectItem(args, "left");
            cJSON *right = cJSON_GetObjectItem(args, "right");
            if (left && cJSON_IsNumber(left) && right && cJSON_IsNumber(right)) {
                SetLeftMotor(left->valueint);
                SetRightMotor(right->valueint);
                result = "{\"status\": \"success\"}";
                return true;
            }
            result = "{\"status\": \"error\"}";
            return false;
        }
    );
#endif
}

void WkEsp32s3Dev::InitializeVolumeMcp() {
    McpServer::GetInstance().AddTool("self.audio.set_volume", "Thay đổi âm lượng loa (0-100)", 
        [this](const cJSON* args, std::string& result) {
            cJSON *vol = cJSON_GetObjectItem(args, "volume");
            if (vol && cJSON_IsNumber(vol)) {
                current_volume_ = vol->valueint;
                if (audio_codec_) audio_codec_->SetOutputVolume(current_volume_);
                result = "{\"status\": \"success\"}";
                return true;
            }
            result = "{\"status\": \"error\"}";
            return false;
        }
    );
}

void WkEsp32s3Dev::InitializeLedMcp() {
    McpServer::GetInstance().AddTool("self.led.set_effect", "Thiết lập hiệu ứng LED sáng", 
        [this](const cJSON* args, std::string& result) {
            cJSON *pattern = cJSON_GetObjectItem(args, "pattern");
            if (pattern && cJSON_IsNumber(pattern)) {
                anim_led1_.pattern = (LedPattern)pattern->valueint;
                result = "{\"status\": \"success\"}";
                return true;
            }
            result = "{\"status\": \"error\"}";
            return false;
        }
    );
}

void WkEsp32s3Dev::InitializeEmotionMcp() {
    McpServer::GetInstance().AddTool("self.display.set_emotion", "Thay đổi biểu cảm khuôn mặt trên màn hình", 
        [this](const cJSON* args, std::string& result) {
            cJSON *emo = cJSON_GetObjectItem(args, "emotion");
            if (emo && cJSON_IsString(emo)) {
                ExecuteEmotion(emo->valuestring);
                result = "{\"status\": \"success\"}";
                return true;
            }
            result = "{\"status\": \"error\"}";
            return false;
        }
    );
}
