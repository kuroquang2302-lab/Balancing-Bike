#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

static constexpr uint8_t PIN_BUZZER = 3;
static constexpr uint8_t PIN_STATUS_LED = 8;
static constexpr uint8_t PIN_STM32_RX = 20;
static constexpr uint8_t PIN_STM32_TX = 21;
static constexpr uint32_t STM32_BAUD = 115200;
static constexpr uint16_t BEEP_FREQ = 2700;

static constexpr uint32_t ECO_STANDBY_MS = 60000;
static constexpr uint32_t HIBERNATE_MS = 180000;
static constexpr uint32_t HIB_OFF_MS = 4000;
static constexpr uint32_t HIB_ON_MS = 1000;

static const char *NUS_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char *NUS_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char *NUS_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

struct BikeControlState
{
    uint8_t enable_switch;
    int8_t drive_speed;
    int8_t steer_angle;
    int8_t speed_gear;
    int8_t steer_step;
    float kp;
    float kd;
    float k_trim;
    float nominal_ofs;
};

static portMUX_TYPE g_stateMux = portMUX_INITIALIZER_UNLOCKED;
static BikeControlState g_bikeState = {
    0,
    0,
    0,
    30,
    25,
    45.0f,
    3.80f,
    0.50f,
    1.30f};

static volatile bool g_bleConnected = false;
static volatile bool g_pendingReset = false;
static volatile bool g_forceSendFrame = false;
static volatile uint8_t g_pendingBeepMode = 0;
static volatile uint32_t g_lastDisconnectedMs = 0;
static volatile bool g_offsetBeeped = false;
static bool g_ecoMode = false;
static bool g_hibernateMode = false;
static bool g_advWindowOpen = true;
static NimBLECharacteristic *g_pTxCharacteristic = nullptr;

static void triggerBuzzer(uint8_t mode)
{
    portENTER_CRITICAL(&g_stateMux);
    g_pendingBeepMode = mode;
    portEXIT_CRITICAL(&g_stateMux);
}

static bool updateBuzzer()
{
    static uint32_t buzzerTimer = 0;
    static uint8_t beepStep = 0;

    uint8_t reqMode = 0;
    portENTER_CRITICAL(&g_stateMux);
    if (g_pendingBeepMode != 0)
    {
        reqMode = g_pendingBeepMode;
        g_pendingBeepMode = 0;
    }
    portEXIT_CRITICAL(&g_stateMux);

    uint32_t now = millis();
    if (reqMode != 0)
    {
        beepStep = reqMode * 10;
        buzzerTimer = now;
    }

    switch (beepStep)
    {
    case 10:
        tone(PIN_BUZZER, BEEP_FREQ);
        beepStep = 11;
        buzzerTimer = now;
        break;
    case 11:
        if (now - buzzerTimer >= 80)
        {
            noTone(PIN_BUZZER);
            beepStep = 0;
        }
        break;

    case 20:
        tone(PIN_BUZZER, BEEP_FREQ);
        beepStep = 21;
        buzzerTimer = now;
        break;
    case 21:
        if (now - buzzerTimer >= 70)
        {
            noTone(PIN_BUZZER);
            beepStep = 22;
            buzzerTimer = now;
        }
        break;
    case 22:
        if (now - buzzerTimer >= 70)
        {
            tone(PIN_BUZZER, BEEP_FREQ);
            beepStep = 23;
            buzzerTimer = now;
        }
        break;
    case 23:
        if (now - buzzerTimer >= 70)
        {
            noTone(PIN_BUZZER);
            beepStep = 0;
        }
        break;

    case 30:
        tone(PIN_BUZZER, BEEP_FREQ);
        beepStep = 31;
        buzzerTimer = now;
        break;
    case 31:
        if (now - buzzerTimer >= 350)
        {
            noTone(PIN_BUZZER);
            beepStep = 0;
        }
        break;

    default:
        break;
    }

    return (beepStep != 0);
}

static float clampFloat(float val, float minVal, float maxVal, float fallback)
{
    if (isnan(val) || isinf(val))
        return fallback;
    if (val < minVal)
        return minVal;
    if (val > maxVal)
        return maxVal;
    return val;
}

static float parseAndClampFloat(const char *str, float minVal, float maxVal, float fallback)
{
    char *endPtr = nullptr;
    float val = strtof(str, &endPtr);
    if (endPtr == str || isnan(val) || isinf(val))
        return fallback;
    if (val < minVal)
        return minVal;
    if (val > maxVal)
        return maxVal;
    return val;
}

static void sendFeedbackToIOS(const char *msg)
{
    Serial.print(msg);
    if (g_bleConnected && g_pTxCharacteristic != nullptr)
    {
        g_pTxCharacteristic->setValue((const uint8_t *)msg, strlen(msg));
        g_pTxCharacteristic->notify();
    }
}

static void reportCurrentStatus(const char *prefix)
{
    BikeControlState snap;
    portENTER_CRITICAL(&g_stateMux);
    snap = g_bikeState;
    portEXIT_CRITICAL(&g_stateMux);

    char reply[110];
    snprintf(reply, sizeof(reply),
             "%s | EN:%u SPD:%d GEAR:%d STR:%d KP:%.1f KD:%.2f TRIM:%.2f OFS:%.2f\n",
             prefix,
             snap.enable_switch,
             snap.drive_speed,
             snap.speed_gear,
             snap.steer_angle,
             snap.kp,
             snap.kd,
             snap.k_trim,
             snap.nominal_ofs);
    sendFeedbackToIOS(reply);
}

static bool parseBluefruitGamepad(const char *data, size_t len)
{
    if (len < 4 || data[0] != '!' || data[1] != 'B')
    {
        return false;
    }

    char btn = data[2];
    bool press = (data[3] == '1');
    uint8_t beepMode = 0;

    portENTER_CRITICAL(&g_stateMux);
    switch (btn)
    {
    case '1':
        if (press)
        {
            g_bikeState.enable_switch = 1;
            beepMode = 1;
        }
        break;
    case '2':
        if (press)
        {
            g_bikeState.enable_switch = 0;
            g_bikeState.drive_speed = 0;
            g_bikeState.steer_angle = 0;
            g_offsetBeeped = false;
            beepMode = 3;
        }
        break;
    case '3':
        if (press && g_bikeState.speed_gear > 10)
            g_bikeState.speed_gear -= 10;
        break;
    case '4':
        if (press && g_bikeState.speed_gear < 100)
            g_bikeState.speed_gear += 10;
        break;
    case '5':
        g_bikeState.drive_speed = press ? g_bikeState.speed_gear : 0;
        break;
    case '6':
        g_bikeState.drive_speed = press ? -g_bikeState.speed_gear : 0;
        break;
    case '7':
        g_bikeState.steer_angle = press ? -g_bikeState.steer_step : 0;
        break;
    case '8':
        g_bikeState.steer_angle = press ? g_bikeState.steer_step : 0;
        break;
    default:
        portEXIT_CRITICAL(&g_stateMux);
        return false;
    }
    if (beepMode != 0)
    {
        g_pendingBeepMode = beepMode;
    }
    portEXIT_CRITICAL(&g_stateMux);

    reportCurrentStatus(press ? "PAD_PRESS" : "PAD_REL");
    return true;
}

static void parseTextCommand(const char *rawInput, size_t len)
{
    if (parseBluefruitGamepad(rawInput, len))
    {
        return;
    }

    char cmdBuf[65];
    size_t copyLen = (len < sizeof(cmdBuf) - 1) ? len : (sizeof(cmdBuf) - 1);
    memcpy(cmdBuf, rawInput, copyLen);
    cmdBuf[copyLen] = '\0';

    char *start = cmdBuf;
    while (*start && isspace((unsigned char)*start))
        start++;
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1)))
    {
        *(--end) = '\0';
    }
    for (char *p = start; *p; ++p)
    {
        *p = (char)toupper((unsigned char)*p);
    }

    if (*start == '\0')
        return;

    bool recognized = true;
    uint8_t beepMode = 0;

    portENTER_CRITICAL(&g_stateMux);
    if (strcmp(start, "STOP") == 0 || strcmp(start, "ESTOP") == 0)
    {
        g_bikeState.enable_switch = 0;
        g_bikeState.drive_speed = 0;
        g_bikeState.steer_angle = 0;
        g_offsetBeeped = false;
        beepMode = 3;
    }
    else if (strcmp(start, "RESET") == 0 || strcmp(start, "REBOOT") == 0)
    {
        g_bikeState.enable_switch = 0;
        g_bikeState.drive_speed = 0;
        g_bikeState.steer_angle = 0;
        g_pendingReset = true;
    }
    else if (strcmp(start, "STATUS") == 0 || strcmp(start, "PING") == 0)
    {
        beepMode = 1;
    }
    else if (strncmp(start, "EN:", 3) == 0)
    {
        uint8_t en = (atoi(start + 3) > 0) ? 1 : 0;
        g_bikeState.enable_switch = en;
        if (en == 0)
        {
            g_offsetBeeped = false;
            beepMode = 3;
        }
        else
        {
            beepMode = 1;
        }
    }
    else if (strncmp(start, "SPD:", 4) == 0)
    {
        float v = parseAndClampFloat(start + 4, -100.0f, 100.0f, 0.0f);
        g_bikeState.drive_speed = (int8_t)lroundf(v);
    }
    else if (strncmp(start, "STR:", 4) == 0)
    {
        float v = parseAndClampFloat(start + 4, -45.0f, 45.0f, 0.0f);
        g_bikeState.steer_angle = (int8_t)lroundf(v);
    }
    else if (strncmp(start, "KP:", 3) == 0)
    {
        g_bikeState.kp = parseAndClampFloat(start + 3, 0.0f, 200.0f, g_bikeState.kp);
        beepMode = 1;
    }
    else if (strncmp(start, "KD:", 3) == 0)
    {
        g_bikeState.kd = parseAndClampFloat(start + 3, 0.0f, 50.0f, g_bikeState.kd);
        beepMode = 1;
    }
    else if (strncmp(start, "TRIM:", 5) == 0)
    {
        g_bikeState.k_trim = parseAndClampFloat(start + 5, 0.0f, 10.0f, g_bikeState.k_trim);
        beepMode = 1;
    }
    else if (strncmp(start, "OFS:", 4) == 0)
    {
        g_bikeState.nominal_ofs = parseAndClampFloat(start + 4, -10.0f, 10.0f, g_bikeState.nominal_ofs);
        beepMode = 2;
    }
    else
    {
        recognized = false;
    }
    if (beepMode != 0)
    {
        g_pendingBeepMode = beepMode;
    }
    portEXIT_CRITICAL(&g_stateMux);

    if (recognized)
    {
        reportCurrentStatus(g_pendingReset ? "REBOOTING" : "OK");
    }
    else
    {
        char errMsg[96];
        snprintf(errMsg, sizeof(errMsg),
                 "ERR: %s | Valid: EN:1 SPD:30 STR:0 KP:45 KD:3.8 TRIM:0.5 OFS:1.3 STOP RESET\n", start);
        sendFeedbackToIOS(errMsg);
    }
}

static void sendFrameToSTM32()
{
    BikeControlState snap;
    portENTER_CRITICAL(&g_stateMux);
    snap = g_bikeState;
    portEXIT_CRITICAL(&g_stateMux);

    uint16_t kp_u16 = (uint16_t)lroundf(clampFloat(snap.kp * 10.0f, 0.0f, 65535.0f, 450.0f));
    uint16_t kd_u16 = (uint16_t)lroundf(clampFloat(snap.kd * 100.0f, 0.0f, 65535.0f, 380.0f));
    uint16_t ktrim_u16 = (uint16_t)lroundf(clampFloat(snap.k_trim * 100.0f, 0.0f, 65535.0f, 50.0f));
    int16_t ofs_i16 = (int16_t)lroundf(clampFloat(snap.nominal_ofs * 100.0f, -1000.0f, 1000.0f, 130.0f));

    uint8_t frame[14];
    frame[0] = 0xAA;
    frame[1] = 0x55;
    frame[2] = snap.enable_switch;
    frame[3] = (uint8_t)snap.drive_speed;
    frame[4] = (uint8_t)snap.steer_angle;
    frame[5] = (uint8_t)(kp_u16 & 0xFF);
    frame[6] = (uint8_t)((kp_u16 >> 8) & 0xFF);
    frame[7] = (uint8_t)(kd_u16 & 0xFF);
    frame[8] = (uint8_t)((kd_u16 >> 8) & 0xFF);
    frame[9] = (uint8_t)(ktrim_u16 & 0xFF);
    frame[10] = (uint8_t)((ktrim_u16 >> 8) & 0xFF);
    frame[11] = (uint8_t)((uint16_t)ofs_i16 & 0xFF);
    frame[12] = (uint8_t)(((uint16_t)ofs_i16 >> 8) & 0xFF);

    uint8_t xorSum = 0;
    for (int i = 2; i <= 12; ++i)
    {
        xorSum ^= frame[i];
    }
    frame[13] = xorSum;

    Serial1.write(frame, sizeof(frame));
}

static void setAdvertisingInterval(uint16_t minUnits, uint16_t maxUnits, esp_power_level_t pwrLevel)
{
    NimBLEAdvertising *pAdv = NimBLEDevice::getAdvertising();
    if (pAdv->isAdvertising())
    {
        pAdv->stop();
    }
    NimBLEDevice::setPower(pwrLevel);
    pAdv->setMinInterval(minUnits);
    pAdv->setMaxInterval(maxUnits);
    pAdv->start();
}

static void stopAdvertisingRadio()
{
    NimBLEAdvertising *pAdv = NimBLEDevice::getAdvertising();
    if (pAdv->isAdvertising())
    {
        pAdv->stop();
    }
}

class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer) override
    {
        (void)pServer;
        g_bleConnected = true;
        NimBLEDevice::setPower(ESP_PWR_LVL_N0);
        digitalWrite(PIN_STATUS_LED, LOW);
        triggerBuzzer(1);
    }

    void onDisconnect(NimBLEServer *pServer) override
    {
        (void)pServer;
        g_bleConnected = false;
        portENTER_CRITICAL(&g_stateMux);
        g_bikeState.drive_speed = 0;
        g_bikeState.steer_angle = 0;
        g_lastDisconnectedMs = millis();
        g_pendingBeepMode = 3;
        portEXIT_CRITICAL(&g_stateMux);

        setAdvertisingInterval(160, 320, ESP_PWR_LVL_N0);
    }
};

class RxCallbacks : public NimBLECharacteristicCallbacks
{
    void onWrite(NimBLECharacteristic *pCharacteristic) override
    {
        std::string rxValue = pCharacteristic->getValue();
        if (!rxValue.empty())
        {
            parseTextCommand(rxValue.data(), rxValue.length());
            g_forceSendFrame = true;
        }
    }
};

void setup()
{
    setCpuFrequencyMhz(80);

    pinMode(PIN_BUZZER, OUTPUT);
    noTone(PIN_BUZZER);

    pinMode(PIN_STATUS_LED, OUTPUT);
    digitalWrite(PIN_STATUS_LED, HIGH);

    Serial.begin(115200);
    Serial1.begin(STM32_BAUD, SERIAL_8N1, PIN_STM32_RX, PIN_STM32_TX);

    g_lastDisconnectedMs = millis();

    NimBLEDevice::init("BalancingBike-C3");
    NimBLEDevice::setPower(ESP_PWR_LVL_N0);

    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    NimBLEService *pService = pServer->createService(NUS_SERVICE_UUID);
    g_pTxCharacteristic = pService->createCharacteristic(
        NUS_TX_CHAR_UUID,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ);

    NimBLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        NUS_RX_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
    pRxCharacteristic->setCallbacks(new RxCallbacks());

    pService->start();

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(NUS_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinInterval(160);
    pAdvertising->setMaxInterval(320);
    pAdvertising->start();
}

void loop()
{
    static uint32_t lastFrameMs = 0;
    static uint32_t lastBlinkMs = 0;
    static uint32_t hibCycleMs = 0;
    static bool ledState = false;
    static char stmBuf[65];
    static size_t stmIdx = 0;

    if (g_pendingReset)
    {
        sendFrameToSTM32();
        Serial1.flush();
        tone(PIN_BUZZER, BEEP_FREQ);
        delay(150);
        noTone(PIN_BUZZER);
        delay(50);
        ESP.restart();
    }

    bool buzzerBusy = updateBuzzer();
    uint32_t now = millis();

    if (g_bleConnected || (!g_ecoMode && !g_hibernateMode))
    {
        if (g_forceSendFrame || (now - lastFrameMs >= 20))
        {
            g_forceSendFrame = false;
            lastFrameMs = now;
            sendFrameToSTM32();
        }
    }

    while (Serial1.available() > 0)
    {
        char c = (char)Serial1.read();
        if (c == '\r')
        {
            continue;
        }
        if (c == '\n' || stmIdx >= sizeof(stmBuf) - 1)
        {
            stmBuf[stmIdx] = '\0';
            if (stmIdx > 0)
            {
                if (!g_offsetBeeped)
                {
                    g_offsetBeeped = true;
                    triggerBuzzer(2);
                }
                char msg[80];
                snprintf(msg, sizeof(msg), "[STM32] %s\n", stmBuf);
                sendFeedbackToIOS(msg);
                stmIdx = 0;
            }
        }
        else
        {
            stmBuf[stmIdx++] = c;
        }
    }

    if (!g_bleConnected)
    {
        uint32_t idleElapsed;
        portENTER_CRITICAL(&g_stateMux);
        idleElapsed = now - g_lastDisconnectedMs;
        portEXIT_CRITICAL(&g_stateMux);

        if (idleElapsed >= HIBERNATE_MS)
        {
            if (!g_hibernateMode)
            {
                g_hibernateMode = true;
                g_advWindowOpen = false;
                hibCycleMs = now;
                sendFrameToSTM32();
                stopAdvertisingRadio();
                digitalWrite(PIN_STATUS_LED, HIGH);
                triggerBuzzer(3);
            }
            else
            {
                uint32_t phaseElapsed = now - hibCycleMs;
                if (!g_advWindowOpen && phaseElapsed >= HIB_OFF_MS)
                {
                    g_advWindowOpen = true;
                    hibCycleMs = now;
                    setAdvertisingInterval(160, 240, ESP_PWR_LVL_N3);
                    digitalWrite(PIN_STATUS_LED, LOW);
                }
                else if (g_advWindowOpen && phaseElapsed >= 40)
                {
                    digitalWrite(PIN_STATUS_LED, HIGH);
                    if (phaseElapsed >= HIB_ON_MS)
                    {
                        g_advWindowOpen = false;
                        hibCycleMs = now;
                        stopAdvertisingRadio();
                    }
                }
            }
        }
        else if (idleElapsed >= ECO_STANDBY_MS && !g_ecoMode)
        {
            g_ecoMode = true;
            sendFrameToSTM32();
            setAdvertisingInterval(1280, 1600, ESP_PWR_LVL_N6);
        }

        if (!g_hibernateMode)
        {
            uint32_t blinkPeriod = g_ecoMode ? (ledState ? 60 : 1940) : 500;
            if (now - lastBlinkMs >= blinkPeriod)
            {
                lastBlinkMs = now;
                ledState = !ledState;
                digitalWrite(PIN_STATUS_LED, ledState ? LOW : HIGH);
            }
        }
    }
    else
    {
        if (g_ecoMode || g_hibernateMode)
        {
            g_ecoMode = false;
            g_hibernateMode = false;
            g_advWindowOpen = true;
        }
        digitalWrite(PIN_STATUS_LED, LOW);
    }

    delay((g_hibernateMode && !g_advWindowOpen && !buzzerBusy) ? 20 : 2);
}
