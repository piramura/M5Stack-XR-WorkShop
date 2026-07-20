# Vendored OSC library

- Source: https://github.com/CNMAT/OSC
- PlatformIO package: `cnmat/OSC@1.0.0`
- License: `LICENSE`を参照

このプロジェクトではUDPによるOSC送信のみを使用します。
ESP32-S3でサポートされないClassic Bluetoothへの依存を避けるため、次のファイルをvendoring時に除外しています。

- `SLIPEncodedBluetoothSerial.cpp`
- `SLIPEncodedBluetoothSerial.h`

また、配布元パッケージ内で不一致だった`library.properties`のバージョン表記を、PlatformIOパッケージおよび`library.json`に合わせて`1.0.0`へ統一しています。
