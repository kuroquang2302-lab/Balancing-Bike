#include <Arduino.h>
#include <NimBLEDevice.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

// =========================================================================
// 1. CAU HINH PHAN CUNG ESP32-C3 SUPER MINI
// =========================================================================
static constexpr uint8_t PIN_STATUS_LED = 8; // LED xanh duong (Active LOW)
static constexpr uint8_t PIN_STM32_RX = 20;  // GPIO20 (RX) <- noi PA9  (USART1_TX STM32)
static constexpr uint8_t PIN_STM32_TX = 21;  // GPIO21 (TX) -> noi PA10 (USART1_RX STM32)
static constexpr uint32_t STM32_BAUD = 115200;

// Nordic UART Service (NUS) UUIDs - Tuong thich Bluefruit Connect, LightBlue, nRF Connect
static const char *NUS_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char *NUS_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static const char *NUS_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";

// =========================================================================
// 2. CAU TRUC TRANG THAI DIEU KHIEN & THONG SO TINH CHINH
// =========================================================================
struct BikeControlState
{
    uint8_t enable_switch; // 0: Tat can bang, 1: Bat can bang
    int8_t drive_speed;    // -100 den +100 (%) banh sau Nidec (PA11)
    int8_t steer_angle;    // -45 den +45 (do) Servo MG995 (PB8)
    int8_t speed_gear;     // Muc ga mac dinh khi bam mui ten Len/Xuong (10..100%)
    int8_t steer_step;     // Goc be lai khi bam mui ten Trai/Phai (mac dinh 25 do)
    float kp;              // Mac dinh: 45.0 (theo bugs.md #8)
    float kd;              // Mac dinh: 3.80 (theo bugs.md #8)
    float k_trim;          // Mac dinh: 0.50
    float nominal_ofs;     // Goc offset danh dinh (mac dinh: 1.30 do)
};

static portMUX_TYPE g_stateMux = portMUX_INITIALIZER_UNLOCKED;
static BikeControlState g_bikeState = {
    0,     // enable_switch = 0
    0,     // drive_speed   = 0
    0,     // steer_angle   = 0
    30,    // speed_gear    = 30%
    25,    // steer_step    = 25 do
    45.0f, // kp            = 45.0
    3.80f, // kd            = 3.80
    0.50f, // k_trim        = 0.50
    1.30f  // nominal_ofs   = +1.30 do
};

static volatile bool g_bleConnected = false;
static NimBLECharacteristic *g_pTxCharacteristic = nullptr;

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

static void sendFeedbackToIOS(const char *msg)
{
    Serial.println(msg);
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
             "%s | EN:%u SPD:%d(Gear:%d) STR:%d KP:%.1f KD:%.2f TRIM:%.2f OFS:%.2f\n",
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

// =========================================================================
// 3. XU LY TAY CAM GAMEPAD CUA APP "BLUEFRUIT CONNECT" (!B<btn><state><crc>)
//   Nut 1: BAT Can bang (EN = 1)
//   Nut 2: TAT Can bang / Dung khan cap (EN = 0, SPD = 0)
//   Nut 3: Giam muc ga (-10%)
//   Nut 4: Tang muc ga (+10%)
//   Mui ten Len   (5): Chay tien khi giu, nha ra = 0
//   Mui ten Xuong (6): Chay lui khi giu, nha ra = 0
//   Mui ten Trai  (7): Be lai trai khi giu, nha ra = 0
//   Mui ten Phai  (8): Be lai phai khi giu, nha ra = 0
// =========================================================================
static bool parseBluefruitGamepad(const char *data, size_t len)
{
    if (len < 4 || data[0] != '!' || data[1] != 'B')
    {
        return false;
    }

    char btn = data[2]; // '1'..'8'
    bool press = (data[3] == '1');

    portENTER_CRITICAL(&g_stateMux);
    switch (btn)
    {
    case '1': // Nut 1: Bat can bang
        if (press)
            g_bikeState.enable_switch = 1;
        break;
    case '2': // Nut 2: Tat can bang & dung banh sau
        if (press)
        {
            g_bikeState.enable_switch = 0;
            g_bikeState.drive_speed = 0;
        }
        break;
    case '3': // Nut 3: Giam ga
        if (press && g_bikeState.speed_gear > 10)
        {
            g_bikeState.speed_gear -= 10;
        }
        break;
    case '4': // Nut 4: Tang ga
        if (press && g_bikeState.speed_gear < 100)
        {
            g_bikeState.speed_gear += 10;
        }
        break;
    case '5': // Mui ten LEN: Chay tien
        g_bikeState.drive_speed = press ? g_bikeState.speed_gear : 0;
        break;
    case '6': // Mui ten XUONG: Chay lui
        g_bikeState.drive_speed = press ? -g_bikeState.speed_gear : 0;
        break;
    case '7': // Mui ten TRAI: Re trai
        g_bikeState.steer_angle = press ? -g_bikeState.steer_step : 0;
        break;
    case '8': // Mui ten PHAI: Re phai
        g_bikeState.steer_angle = press ? g_bikeState.steer_step : 0;
        break;
    default:
        portEXIT_CRITICAL(&g_stateMux);
        return false;
    }
    portEXIT_CRITICAL(&g_stateMux);

    reportCurrentStatus(press ? "PAD_PRESS" : "PAD_REL");
    return true;
}

// =========================================================================
// 4. XU LY LENH GO CHU TINH CHINH THONG SO TAI CHO
//   EN:1 / EN:0  | SPD:30   | STR:-15
//   KP:45.0      | KD:3.80  | TRIM:0.50 | OFS:1.30
//   STOP         | STATUS
// =========================================================================
static void parseTextCommand(const char *rawInput, size_t len)
{
    // Kiem tra neu la goi tin tay cam Bluefruit Connect (!B...)
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

    portENTER_CRITICAL(&g_stateMux);
    if (strcmp(start, "STOP") == 0 || strcmp(start, "ESTOP") == 0)
    {
        g_bikeState.enable_switch = 0;
        g_bikeState.drive_speed = 0;
    }
    else if (strcmp(start, "STATUS") == 0 || strcmp(start, "PING") == 0)
    {
        // Chi in trang thai
    }
    else if (strncmp(start, "EN:", 3) == 0)
    {
        g_bikeState.enable_switch = (atoi(start + 3) > 0) ? 1 : 0;
    }
    else if (strncmp(start, "SPD:", 4) == 0)
    {
        float v = clampFloat(strtof(start + 4, nullptr), -100.0f, 100.0f, 0.0f);
        g_bikeState.drive_speed = (int8_t)lroundf(v);
    }
    else if (strncmp(start, "STR:", 4) == 0)
    {
        float v = clampFloat(strtof(start + 4, nullptr), -45.0f, 45.0f, 0.0f);
        g_bikeState.steer_angle = (int8_t)lroundf(v);
    }
    else if (strncmp(start, "KP:", 3) == 0)
    {
        g_bikeState.kp = clampFloat(strtof(start + 3, nullptr), 0.0f, 200.0f, g_bikeState.kp);
    }
    else if (strncmp(start, "KD:", 3) == 0)
    {
        g_bikeState.kd = clampFloat(strtof(start + 3, nullptr), 0.0f, 50.0f, g_bikeState.kd);
    }
    else if (strncmp(start, "TRIM:", 5) == 0)
    {
        g_bikeState.k_trim = clampFloat(strtof(start + 5, nullptr), 0.0f, 10.0f, g_bikeState.k_trim);
    }
    else if (strncmp(start, "OFS:", 4) == 0)
    {
        g_bikeState.nominal_ofs = clampFloat(strtof(start + 4, nullptr), -10.0f, 10.0f, g_bikeState.nominal_ofs);
    }
    else
    {
        recognized = false;
    }
    portEXIT_CRITICAL(&g_stateMux);

    if (recognized)
    {
        reportCurrentStatus("OK");
    }
    else
    {
        char errMsg[96];
        snprintf(errMsg, sizeof(errMsg),
                 "ERR: '%s' (Dung: EN:1, KP:45, KD:3.8, TRIM:0.5, OFS:1.3, STOP)\n", start);
        sendFeedbackToIOS(errMsg);
    }
}

// =========================================================================
// 5. DONG GOI KHUNG 14-BYTE GUI SANG STM32F411RE QUA UART1 (GPIO21 -> PA10)
//   [0]     0xAA (Header 1)
//   [1]     0x55 (Header 2)
//   [2]     enable_switch (uint8: 0 hoac 1)
//   [3]     drive_speed   (int8: -100..100)
//   [4]     steer_angle   (int8: -45..45)
//   [5..6]  kp_x10        (uint16 LE: Kp * 10)
//   [7..8]  kd_x100       (uint16 LE: Kd * 100)
//   [9..10] ktrim_x100    (uint16 LE: K_trim * 100)
//   [11..12] ofs_x100     (int16 LE : nominal_ofs * 100, VD +1.30 -> +130)
//   [13]    checksum      (XOR tu byte [2] den byte [12])
// =========================================================================
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

// =========================================================================
// 6. BLE CALLBACKS
// =========================================================================
class ServerCallbacks : public NimBLEServerCallbacks
{
    void onConnect(NimBLEServer *pServer) override
    {
        (void)pServer;
        g_bleConnected = true;
        digitalWrite(PIN_STATUS_LED, LOW);
        Serial.println("[BLE] iPhone da ket noi!");
    }

    void onDisconnect(NimBLEServer *pServer) override
    {
        (void)pServer;
        g_bleConnected = false;
        portENTER_CRITICAL(&g_stateMux);
        g_bikeState.drive_speed = 0;
        portEXIT_CRITICAL(&g_stateMux);

        Serial.println("[BLE] Mat ket noi! Dang phat quang cao BLE lai...");
        NimBLEDevice::startAdvertising();
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
            sendFrameToSTM32();
        }
    }
};

void setup()
{
    pinMode(PIN_STATUS_LED, OUTPUT);
    digitalWrite(PIN_STATUS_LED, HIGH);

    Serial.begin(115200);
    Serial1.begin(STM32_BAUD, SERIAL_8N1, PIN_STM32_RX, PIN_STM32_TX);

    delay(1500);
    Serial.println("\n====================================================");
    Serial.println("  BALANCING BIKE - ESP32-C3 BLE CONTROLLER READY");
    Serial.println("  App iOS khuyen dung: Bluefruit Connect / LightBlue");
    Serial.println("====================================================");

    NimBLEDevice::init("BalancingBike-C3");
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

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
    pAdvertising->start();
}

void loop()
{
    static uint32_t lastFrameMs = 0;
    static uint32_t lastBlinkMs = 0;
    static bool ledState = false;

    uint32_t now = millis();

    // Gui goi 14-byte xuong STM32 voi tan so 50Hz (20ms)
    if (now - lastFrameMs >= 20)
    {
        lastFrameMs = now;
        sendFrameToSTM32();
    }

    // Nhan Active_Offset tu STM32 gui len (neu co) va ban len iPhone
    if (Serial1.available() > 0)
    {
        char stmBuf[65];
        size_t idx = 0;
        while (Serial1.available() > 0 && idx < sizeof(stmBuf) - 1)
        {
            char c = (char)Serial1.read();
            if (c == '\n')
                break;
            if (c != '\r')
                stmBuf[idx++] = c;
        }
        stmBuf[idx] = '\0';
        if (idx > 0)
        {
            char msg[80];
            snprintf(msg, sizeof(msg), "[STM32] %s\n", stmBuf);
            sendFeedbackToIOS(msg);
        }
    }

    // Den bao trang thai BLE tren GPIO8
    if (!g_bleConnected)
    {
        if (now - lastBlinkMs >= 500)
        {
            lastBlinkMs = now;
            ledState = !ledState;
            digitalWrite(PIN_STATUS_LED, ledState ? LOW : HIGH);
        }
    }
    else
    {
        digitalWrite(PIN_STATUS_LED, LOW);
    }
}