# M5Stack XR Workshop

M5Stackのタッチ入力をUnityへOSCで送信するワークショップ用プロジェクトです。

## サンプルの動作

- 通常：M5Stack単体でタッチ操作を確認
- OSC有効時：タッチ操作を画面に表示し、Unityへ送信

`src/main.cpp`の次の行でOSC送信を切り替えます。

```cpp
// #define ENABLE_OSC
```

OSCを使う場合はコメントを外します。

```cpp
#define ENABLE_OSC
```

## Wi-Fi設定

`include/wifi_config.example.h`を`include/wifi_config.h`へコピーし、Wi-FiとUnity PCの情報を設定します。

```cpp
static constexpr const char* WIFI_SSID = "YOUR_WIFI_SSID";
static constexpr const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
static constexpr const char* UNITY_OSC_HOST = "172.16.80.100";
static constexpr uint16_t UNITY_OSC_PORT = 9000;
```

Unity PCのIPアドレスを確認する方法は[ネットワーク設定手順](docs/network-setup.md)を参照してください。

## ビルド対象

- M5Stack Core2：`m5stack-core2`
- M5Stack CoreS3：`m5stack-cores3`
