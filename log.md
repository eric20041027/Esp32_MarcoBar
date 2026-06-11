# ESP32 MacroBar — 開發日誌

> 記錄從零開始到完成的完整開發過程

---

## 2026-06-11

### 環境建置

- 確認使用 Espressif ESP32 Board **2.0.16**（不升級，避免 3.x breaking change）
- Arduino IDE 更新提示：Libraries 和 Boards 均選擇 **LATER**，保持穩定版本
- 安裝 **ESP32 BLE Keyboard**（by T-vK，從 GitHub 手動安裝 ZIP）
- 安裝 **DHT sensor library**（by Adafruit，Library Manager）

---

### 專案命名

- 專案名稱定為 **ESP32 MacroBar**
- GitHub Repo：https://github.com/eric20041027/Esp32_MarcoBar

---

### 專案初始化

建立 Arduino 專案資料夾結構：

```
ESP32_MacroBar/
├── ESP32_MacroBar.ino
├── menu.h
├── joystick.h
├── lcd_display.h
└── ble_hid.h
```

初始 commit 推上 GitHub，並新增 README。

---

### Phase 2：硬體接線

#### LCD 1602A

確認板子為 **QAPASS 1602A 裸板**（無 I2C 模組），需要對比度控制。

| LCD 腳位 | ESP32 |
|----------|-------|
| VSS | GND |
| VDD | VIN（5V） |
| V0 | GND（直接接 GND 固定對比） |
| RS | GPIO 19 |
| RW | GND |
| E | GPIO 23 |
| D4 | GPIO 18 |
| D5 | GPIO 17（TX2） |
| D6 | GPIO 16（RX2） |
| D7 | GPIO 15 |
| A | 3.3V |
| K | GND |

**問題**：原本 V0 懸空，LCD 字元與背景亮度相近，難以辨識。  
**解法**：V0 直接接 GND，對比度固定最深，顯示正常。

#### Joystick

| Joystick | ESP32 |
|----------|-------|
| VCC | 3.3V |
| GND | GND |
| VRX | GPIO 34 |
| VRY | GPIO 35 |
| SW | GPIO 32 |

---

### Phase 3：LCD 測試

燒錄 `LCD_Test.ino` 顯示：
```
ESP32 MacroBar
LCD OK!
```
測試通過。

---

### Phase 4：Joystick 測試

燒錄 `Joystick_Test.ino`，Serial Monitor 確認：
- 靜止時 X / Y 約在 2048 附近
- 推動搖桿數值接近 0 或 4095
- 按鈕顯示 `PRESSED`

測試通過。

---

### Phase 5：BLE HID 測試

燒錄 `BLE_Test.ino`，Mac 藍牙配對 **ESP32 MacroBar** 成功。  
連線後音量每 3 秒自動增加，確認 BLE HID 正常運作。  
macOS 跳出「鍵盤設定輔助程式」視窗，直接關閉即可。

---

### Phase 6：整合主程式

燒錄 `ESP32_MacroBar.ino`，整合所有模組。

#### 遭遇問題與解法

**問題 1：編譯錯誤 `KEY_MEDIA_BRIGHTNESS_UP` 未定義**
- 原因：ESP32 BLE Keyboard library 不支援亮度鍵
- 解法：移除亮度頁面，改為其他功能

**問題 2：搖桿靜止時選項自動亂跳**
- 原因：Y 軸漂移值落在死區邊緣，持續觸發上下切換
- 解法：Dead Zone 從 200 調整為 **500**

**問題 3：編譯錯誤 `KEY_MEDIA_EJECT` 未定義**
- 原因：Sleep 功能使用了 library 不支援的鍵碼
- 解法：Sleep 功能完全移除

**問題 4：Sites 頁面所有網站都跳到 GitHub**
- 原因：Spotlight 自動完成搶先選取第一個搜尋結果
- 解法：改用完整 `https://` URL，並將每字元輸入延遲改為 50ms，Enter 前等待 1500ms

**問題 5：URL 輸入不完整（顯示 `https://www.yout/`）**
- 原因：BLE 打字速度過快，Spotlight 來不及接收
- 解法：改為逐字元 `print()` 並每字元間加 50ms delay

**問題 6：按下確認後自動觸發音量增加**
- 原因：按鍵瞬間搖桿 X 軸漂移誤觸 Vol+
- 解法：執行動作後加入 500ms 冷卻時間，期間忽略所有搖桿輸入

**問題 7：顯示桌面（`Cmd+F3`）無反應**
- 原因：macOS 顯示桌面快捷鍵為 `F11`，非 `Cmd+F3`
- 解法：改為 `_ble.write(KEY_F11)`

---

### 選單最終結構（Phase 6 當下）

```
Page 1: Media    →  Vol+ / Vol- / Mute / Play
Page 2: Media 2  →  Prev / Next
Page 3: Sites    →  YouTube / GitHub / ChatGPT
Page 4: System   →  Lock / Screenshot
Page 5: Window   →  Mission Control / 切換輸入法 / 顯示桌面
```

---

## 2026-06-12

### DHT11 溫濕度感測器新增

接線：

| DHT11 | ESP32 |
|-------|-------|
| VCC | VIN（5V） |
| GND | GND |
| DATA | GPIO 4（D4） |

新增 `dht_sensor.h`，每 2 秒非阻塞更新一次溫濕度。

LCD 顯示格式：
```
Temp: 26.5 C
Humi: 58.0 %
```

**問題：進入 Sensor 頁面後無法用搖桿跳出**
- 原因：使用 `delay(2000)` 阻塞主迴圈，搖桿無法被讀取
- 解法：改用 `millis()` 非阻塞計時，每 100ms 執行一次主迴圈

**問題：切換到 Sensor 頁面後 LCD 顯示亂碼**
- 原因：`_lcd` 從 `static` 改為非 static 後被多個 header include，造成重複定義
- 解法：改回 `static`，`dhtRender()` 直接存取 `_lcd`，不透過參數傳遞

---

### Claude 網站新增

Sites 頁面新增第四個選項：
- **Claude** → `https://www.claude.ai`

### 選單最終結構

```
Page 1: Media    →  Vol+ / Vol- / Mute / Play
Page 2: Media 2  →  Prev / Next
Page 3: Sites    →  YouTube / GitHub / ChatGPT / Claude
Page 4: System   →  Lock / Screenshot
Page 5: Window   →  Mission Control / 切換輸入法 / 顯示桌面
Page 6: Sensor   →  溫度 / 濕度（即時顯示，每 2 秒更新）
```

---

### 文件更新

- 更新 README.md，涵蓋所有功能、接線圖、技術細節
- 建立 log.md（本文件）
- 所有變更 push 至 GitHub

---

## 技術總結

| 項目 | 決策 / 結果 |
|------|------------|
| Board 版本 | ESP32 2.0.16（不升級至 3.x） |
| BLE Library | ESP32 BLE Keyboard by T-vK |
| 亮度控制 | library 不支援，已移除 |
| 睡眠功能 | BLE HID 無法模擬 Power 鍵，已移除 |
| LCD 對比度 | V0 接 GND（固定最深，省去電位器） |
| 搖桿死區 | ±500（從 200 調大，解決漂移問題） |
| URL 開啟方式 | Spotlight + 逐字元輸入（50ms/字元）+ 1500ms 等待 |
| DHT11 更新 | millis() 非阻塞，每 2000ms 刷新一次 |
| 顯示桌面快捷鍵 | F11（macOS 系統設定確認） |
