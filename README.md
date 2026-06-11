# ESP32 MacroBar

一個基於 ESP32 的無線藍牙巨集控制器，靈感來自 Mac Touch Bar。  
透過搖桿導航頁面並觸發各種系統操作，免驅動程式，即插即用。

---

## 硬體規格

| 元件 | 型號 / 說明 |
|------|------------|
| 微控制器 | ESP32-WROOM-32 Dev Board |
| 顯示器 | LCD 1602A（16x2，4-bit 模式，直接排針） |
| 輸入 | Keyes Joystick（VRX / VRY 類比軸 + SW 按鈕） |
| 感測器 | DHT11（溫濕度） |
| 連接方式 | BLE HID（模擬藍牙鍵盤，macOS 免驅動） |
| 開發環境 | Arduino IDE + Espressif ESP32 Board 2.0.16 |

---

## 接線圖

### LCD 1602A → ESP32

| LCD 腳位 | 功能 | ESP32 |
|----------|------|-------|
| VSS | GND | GND |
| VDD | 5V | VIN |
| V0 | 對比度 | GND（固定最深對比） |
| RS | 暫存器選擇 | GPIO 19 |
| RW | 讀寫 | GND（固定寫入） |
| E | Enable | GPIO 23 |
| D4 | 資料 | GPIO 18 |
| D5 | 資料 | GPIO 17（TX2） |
| D6 | 資料 | GPIO 16（RX2） |
| D7 | 資料 | GPIO 15 |
| A | 背光+ | 3.3V |
| K | 背光- | GND |

### Joystick → ESP32

| Joystick | ESP32 |
|----------|-------|
| VCC | 3.3V |
| GND | GND |
| VRX | GPIO 34 |
| VRY | GPIO 35 |
| SW | GPIO 32 |

### DHT11 → ESP32

| DHT11 | ESP32 |
|-------|-------|
| VCC | VIN（5V） |
| GND | GND |
| DATA | GPIO 4 |

> ⚠️ GPIO 34 / 35 為 input-only，無內建 pull-up，適合類比讀取。  
> ⚠️ GPIO 32 SW 按鈕使用程式內建 `INPUT_PULLUP`。

---

## 選單結構

```
Page 1: Media    →  Vol+ / Vol- / Mute / Play
Page 2: Media 2  →  Prev / Next
Page 3: Sites    →  YouTube / GitHub / ChatGPT / Claude
Page 4: System   →  Lock / Screenshot
Page 5: Window   →  Mission Control / 切換輸入法 / 顯示桌面
Page 6: Sensor   →  溫度 / 濕度（即時顯示，每 2 秒更新）
```

### LCD 顯示格式

```
[Media         ]   ← 第一行：頁面名稱
[>Vol +        ]   ← 第二行：當前選項（> 表示選中）
```

Sensor 頁面格式：

```
[Temp: 26.5 C  ]
[Humi: 58.0 %  ]
```

---

## 操作方式

| 動作 | 效果 |
|------|------|
| 搖桿向左 / 右 | 切換頁面 |
| 搖桿向上 / 下 | 切換該頁選項 |
| 按下搖桿 | 執行當前選項 |
| 長按搖桿（> 1 秒） | 返回第一頁 |

---

## BLE HID 功能對應

### Media（Page 1）

| 選項 | HID 鍵碼 | macOS 效果 |
|------|---------|-----------|
| Vol + | `KEY_MEDIA_VOLUME_UP` | 系統音量增加 |
| Vol - | `KEY_MEDIA_VOLUME_DOWN` | 系統音量減少 |
| Mute | `KEY_MEDIA_MUTE` | 切換靜音 |
| Play | `KEY_MEDIA_PLAY_PAUSE` | 播放 / 暫停 |

### Media 2（Page 2）

| 選項 | HID 鍵碼 | macOS 效果 |
|------|---------|-----------|
| Prev | `KEY_MEDIA_PREVIOUS_TRACK` | 上一首 |
| Next | `KEY_MEDIA_NEXT_TRACK` | 下一首 |

### Sites（Page 3）

| 選項 | 行為 | 目標網址 |
|------|------|---------|
| YouTube | Spotlight → 逐字輸入 URL → Enter | https://www.youtube.com |
| GitHub | Spotlight → 逐字輸入 URL → Enter | https://www.github.com |
| ChatGPT | Spotlight → 逐字輸入 URL → Enter | https://www.chatgpt.com |
| Claude | Spotlight → 逐字輸入 URL → Enter | https://www.claude.ai |

> 開啟流程：`Cmd+Space` 喚起 Spotlight → 逐字輸入完整 URL（每字元 50ms 延遲）→ 等待 1500ms → `Enter`

### System（Page 4）

| 選項 | 快捷鍵 | macOS 效果 |
|------|--------|-----------|
| Lock | `Cmd+Ctrl+Q` | 鎖定螢幕 |
| Screenshot | `Cmd+Shift+4` | 區域截圖 |

### Window（Page 5）

| 選項 | 快捷鍵 | macOS 效果 |
|------|--------|-----------|
| Mission | `Ctrl+Up` | Mission Control |
| Input | `Ctrl+Space` | 切換輸入法 |
| Desktop | `F11` | 顯示桌面 |

### Sensor（Page 6）

即時顯示 DHT11 溫濕度，每 2 秒自動更新，不執行 BLE 動作。

---

## 專案架構

```
ESP32_MacroBar/
├── ESP32_MacroBar.ino   # 主程式（setup / loop）
├── menu.h               # 選單資料結構與頁面狀態機
├── joystick.h           # 搖桿 ADC 讀取、死區過濾、去抖動、長按偵測
├── lcd_display.h        # LiquidCrystal 封裝，dirty-flag 避免閃爍
├── ble_hid.h            # BLE HID 動作封裝（媒體鍵 / 快捷鍵 / URL）
└── dht_sensor.h         # DHT11 溫濕度感測器封裝
```

---

## 技術細節

### 搖桿輸入處理

- **ADC 中心值**：2048（12-bit ADC，範圍 0～4095）
- **死區（Dead Zone）**：±500，避免搖桿漂移誤觸
- **去抖動**：200ms 冷卻時間，防止連續觸發
- **長按偵測**：按住 ≥ 1000ms 觸發，使用 `millis()` 非阻塞計時
- **執行冷卻**：按下確認後 500ms 內忽略搖桿輸入，防止按鍵後漂移

### LCD 顯示優化

- **Dirty Flag**：`_lastPage` / `_lastItem` 比對，內容未變動時不重繪，避免閃爍
- **Padding**：每次更新補空白至 16 字元，清除殘留字元
- **對比度**：V0 接 GND，固定最深對比（省去電位器）

### BLE HID

- **裝置名稱**：`ESP32 MacroBar`
- **Library**：ESP32 BLE Keyboard by T-vK
- **Board 版本**：Espressif ESP32 2.x（3.x 有 breaking change 不相容）
- **URL 開啟**：逐字元輸入（每字元 50ms），避免 BLE 丟字

### DHT11 感測器

- **更新頻率**：每 2000ms 讀取一次（非阻塞，使用 `millis()` 計時）
- **錯誤處理**：讀取失敗時 LCD 顯示 `Sensor Error`

---

## 相依套件

| Library | 用途 | 安裝方式 |
|---------|------|---------|
| ESP32 BLE Keyboard | BLE HID 鍵盤模擬 | GitHub（T-vK）→ Add .ZIP Library |
| LiquidCrystal | LCD 驅動 | Arduino 內建 |
| DHT sensor library | DHT11 溫濕度讀取 | Library Manager（Adafruit） |

---

## 已知限制

- ESP32 BLE 不支援 `KEY_MEDIA_BRIGHTNESS_UP/DOWN`，亮度控制無法實作
- BLE 無法模擬 Power 鍵，睡眠功能已移除
- 開啟網站依賴 macOS Spotlight，需確認 `Cmd+Space` 快捷鍵未被更改
- WiFi 與 BLE 共用天線，本專案僅啟用 BLE

---

*最後更新：2026-06-12*
