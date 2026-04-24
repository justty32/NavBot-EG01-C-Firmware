# 🐾 NavBot EG01 Quadruped: 高性能 C++ 韌體部署指南

本專案將 NavBot EG01 的控制系統從 MicroPython 遷移至 **C++ (Arduino/ESP-IDF) 架構**，旨在大幅提升 8-DOF 四足機器人的即時控制性能與系統穩定性。

---

## 🌟 技術優化重點
1. **硬即時控制 (Hard Real-time)**：利用 ESP32 的 FreeRTOS 雙核任務調度，將控制頻率鎖定在精確的 **100Hz (10ms/cycle)**，消除 Python 的垃圾回收 (GC) 延遲。
2. **異步網路架構**：採用非阻塞式 WebServer，確保手機控制指令與步態計算不產生競態衝突。
3. **部署簡化**：採用嵌入式靜態資源技術（Raw String Literals），將 Web UI 資源編譯進二進位檔，免除 LittleFS/SPIFFS 檔案系統燒錄步驟。

---

## 🛠️ 第一步：開發環境建置 (Environment Setup)

1. **編譯器安裝**：下載並安裝 [Arduino IDE 2.x](https://www.arduino.cc/en/software)。
2. **ESP32 Core 配置 (關鍵版本限制)**：
   - 開啟 **Boards Manager (開發板管理員)** (位於左側側邊欄**第二個圖示**，外觀像一塊**小電路板**)。
   - 搜尋並安裝 `esp32 by Espressif Systems`。
   - **⚠️ 注意：** 為確保 LwIP 網路層穩定，請務必選取版本 **`2.0.17`** (避免使用 3.x 版本以防止 `tcp_alloc` 系統崩潰)。
3. **函式庫依賴 (Dependencies)**：
   - 開啟 **Library Manager (函式庫管理員)** (位於左側側邊欄**第三個圖示**，外觀像**一疊書本**)。
   - 搜尋並安裝以下元件 (**均請選取最新版本安裝**)：
     - `Adafruit PWM Servo Driver Library`
     - `AsyncTCP`
     - **`ESPAsyncWebServer`** (⚠️ 請認明此精確名稱，勿安裝為 `ESP_Async_Web_Server`)。

### 🔧 連結器報錯修正 (MD5 Linker Fix)
若在燒錄過程中，於 Arduino IDE 下方的 **「輸出視窗 (Output Panel / Console)」** 看到紅字顯示 `mbedtls_md5` 未定義的錯誤，係因新舊加密庫命名衝突，請執行以下外科手術式修正：
1. 定位檔案路徑：`C:\Users\你的用戶名\Documents\Arduino\libraries\ESPAsyncWebServer\src\WebAuthentication.cpp`。
2. 搜尋 `getMD5` 函數（約第 73 行）。
3. 將函數實作清空，直接回傳 `false`：
   ```cpp
   static bool getMD5(uint8_t * data, uint16_t len, char * output){
       return false;
   }
   ```
4. 儲存後重新編譯。

---

## 🚀 第二步：韌體燒錄 (Flashing)

1. **硬體連接**：使用 USB 數據線連接 ESP32 主控板。
   - **如何識別通訊埠 (COM Port)？**：
     - 在 Windows 系統中，右鍵點擊「開始」圖示或「本機」，開啟 **「裝置管理員」**。
     - 找到 **「連接埠 (COM 和 LPT)」** 類別並將其展開。
     - 尋找標註為 `USB-SERIAL CH340` 或 `Silicon Labs CP210x` 的 **通用序列匯流排** 轉序列通訊設備。該項目後方括號內的編號（例如 `COM3`）即為您需在 Arduino IDE 中選取的 **通訊埠**。
2. **專案開啟**：進入 `NavBot_C` 資料夾，按兩下左鍵開啟 **`NavBot_C.ino`** 檔案。這會自動啟動 Arduino IDE 並載入所有相關程式碼（其餘 `.cpp` 與 `.h` 檔案會同步載入，無需手動開啟）。
3. **燒錄參數設定**：
   - **Board**: `ESP32 Dev Module`
   - **Upload Speed**: `921600` (或預設值)
4. **執行 Upload**：點擊箭頭圖示開始編譯並燒錄。看到 `Done uploading` 代表韌體已寫入快閃記憶體。

---

## 📱 第三步：連線控制 (Operation)

1. **AP 模式連線**：手機連線至 SSID 為 **`EG01-XXXXXX`** 的 WiFi。
2. **路由保護**：**請務必關閉手機的「行動數據 (Mobile Data)」**。
   - *原因：因 ESP32 不具備網際網路出口，部分行動作業系統會強制跳回電信網路，導致無法存取區域網頁。*
3. **存取 Web UI**：於瀏覽器輸入 **`http://192.168.4.1`** 即可進入控制介面。

---

## 🛠️ 診斷與調試 (Serial Debugging)

當系統運行異常時，請透過 **Serial Monitor (序列埠監視器)** 監控底層狀態：

1. **開啟監控視窗**：確保開發板已連接電腦且 IDE 已開啟，於選單列點選 **`Tools` (工具)** -> **`Serial Monitor` (序列埠監視器)**，或點擊軟體右上角的 **「🔍 (放大鏡)」** 圖示。
2. **初始化配置**：將視窗內的波特率 (Baud rate) 設為 **`115200`**。
3. **重置診斷**：按下板載 **RST (Reset)** 按鈕觀察啟動 Log。

### 常見狀態碼對照表：
| Log 訊息 | 系統狀態說明 | 處理建議 |
| :--- | :--- | :--- |
| **`Free Heap: 22XXXX bytes`** | 系統運行穩定，內存堆棧充足。 | 正常現象。 |
| **`Warning: IMU not found`** | I2C 總線無法掃描到 MPU6050。 | 檢查 SDA/SCL 線路連接；裸板測試可忽略。 |
| **`assert failed: tcp_alloc`** | 核心網路協議棧記憶體分配失敗。 | 韌體環境錯誤，請降級 ESP32 Core 至 **2.0.17**。 |
| **`Rebooting...`** | 系統觸發 Watchdog 或是非法指令崩潰。 | 檢查電源穩定性或是否有腳位短路。 |

---

## ⚙️ 參數校準 (Configuration)
機械工程師可直接於以下檔案調整物理參數：
- **`Config.h`**：定義腿部幾何長度 (L1, L2)、PID 姿態增益、預設身高與速度係數。
- **`ServoDriver.h`**：調整舵機 PWM 中位點 (Offset) 與極限限位。

---

## 📚 專案來源與致謝 (Credits & Origins)
本專案為 **NavBot EG01** 四足機器人的 C++ 移植版本。
- **原始專案**：基於 [fuwei007/NavBot-EG01](https://github.com/fuwei007/NavBot-EG01) 提供的 MicroPython 版本韌體進行重新開發。
- **致謝**：感謝原作者設計了優雅的機械結構與基礎步態演算法，本專案在此基礎上進行了 C++ 的性能優化與系統重構。

恭喜！您已成功完成 NavBot EG01 的專業韌體升級。

