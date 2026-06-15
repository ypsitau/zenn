---
title: "pico-jxglib で Pico ボードに USB キーボード・マウスを接続する話"
emoji: "👌"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["組み込み", "raspberrypi", "pico", "raspberrypipico", "usb"]
published: true
---
**pico-jxglib** は、ワンボードマイコン Raspberry Pi Pico の Pico SDK プログラミングをサポートするライブラリです。

▶️ [pico-jxglib: A "Pseudo" OS for Raspberry Pi Pico](https://ypsitau.github.io/pico-jxglib/)

今回は Pico の USB ホスト機能を使って、USB キーボードとマウスを接続します。Pico ボードに主要な入力デバイスがつながると、独立したマイコンになって楽しいですよー。この記事では、GUI ツールキット [LVGL](https://zenn.dev/ypsitau/articles/2025-02-04-lvgl) にキーボード・マウス入力機能をつける方法まで説明していきます。

## Pico の USB ホスト機能について

Pico で工作をしているとボタン入力などが必要になることがよくあります。そのたびにタクトスイッチをブレッドボードに配置したりするのですが、操作感はよくないですし、作業中に接続不良になったりしてストレスがたまります。なによりも、貴重な GPIO を大量に消費してしまって非常にもったいないです。

そんな時活用したいのが Pico の USB インターフェースです。コネクタ形状から察せられるように、多くの場合は Pico を USB デバイスにして PC などのホストに接続するのですが、Pico 自身を USB ホストとして機能させることもできます。つまり、ドライバソフトウェアを書けば、広く流通している PC 用の USB 周辺機器を Pico に接続できるということです。

数ある USB 周辺機器の中でも、USB キーボードやマウスは特に種類が豊富で価格もお手頃ですね。ワイヤレスタイプのものでも、キーボードとマウスのセットが 2,000 円程度で入手できました。

![USB-Keyboard-Mouse.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/USB-Keyboard-Mouse.jpg)

これでタクトスイッチよりもはるかに操作性が向上しますし、ワイヤレスなら遠隔操作も可能になって工作の幅が広がりそうです。

ところで、Pico の USB 端子は microB タイプなので、ホストとして使うにはこれを A タイプに変換する OTG アダプタが必要になります。400 円程度で入手できます。

![USB-MicroB-A-Adapter-Zoom.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/USB-MicroB-A-Adapter-Zoom.jpg)

USB の操作には、Pico SDK とともに導入されている tinyusb ライブラリを使います。tinyusb のディレクトリ中 `tinyusb/examples/host/cdc_msc_hid` に USB ホスト機能のサンプルがありましたので、これを参考にして USB キーボードやマウスをより簡便に操作できる API を **pico-jxglib** に実装しました。

## 実際のプロジェクト

USB キーボードとマウスを実際に動かしてみます。Stdio を使えば詳細な情報が得られますが、USB 経由での Stdio が使えないので、LED を使って Pico 本体のみで動作確認ができるようにします。

### 開発環境のセットアップ

Visual Studio Code や Git ツール、Pico SDK のセットアップが済んでいない方は[「Pico SDK ことはじめ」](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E9%96%8B%E7%99%BA%E7%92%B0%E5%A2%83) をご覧ください。

**pico-jxglib** は GitHub からレポジトリをクローンすることで入手できます。

```bash
git clone https://github.com/ypsitau/pico-jxglib.git
cd pico-jxglib
git submodule update --init
```

:::message
**pico-jxglib** はほぼ毎日更新されています。すでにクローンしている場合は、`pico-jxglib` ディレクトリで以下のコマンドを実行して最新のものにしてください。

```bash
git pull
```

:::

### プロジェクトの作成

VSCode のコマンドパレットから `>Raspberry Pi Pico: New Pico Project` を実行し、以下の内容でプロジェクトを作成します。Pico SDK プロジェクト作成の詳細や、ビルド、ボードへの書き込み方法については[「Pico SDK ことはじめ」](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86) を参照ください。

- **Name** ... プロジェクト名を入力します。今回は例として `usbhosttest` を入力します
- **Board type** ... ボード種別を選択します
- **Location** ... プロジェクトディレクトリを作る一つ上のディレクトリを選択します
- **Stdio support** .. Stdio に接続するポート (UART または USB) を選択しますが、USB はこのプログラムで使うので選択できません。UART のみ選択するか、どちらも未チェックのままにしておきます
- **Code generation options** ... **`Generate C++ code` にチェックをつけます**


プロジェクトディレクトリと `pico-jxglib` のディレクトリ配置が以下のようになっていると想定します。

```text
├── pico-jxglib/
└── usbhosttest/
    ├── CMakeLists.txt
    ├── usbhosttest.cpp
    └── ...
```

以下、このプロジェクトをもとに `CMakeLists.txt` やソースファイルを編集してプログラムを作成していきます。

### ブレッドボード配線

ブレッドボードの配線イメージを以下に示します。

![circuit-usbhost.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost.png)

Pico を USB デバイス として動かす場合は USB 端子から Pico に電源を供給できますが、今回は Pico 自体がホストになるので VBUS (40 番ピン) に 5V の電源を外部から供給します。VSYS (39 番ピン) に電源をつなぐと、逆流防止のダイオードのため USB バスに電源が供給されないので注意してください。

### USB キーボードの操作

**pico-jxglib** で USB のホスト機能を使うには `USBHost`クラスを使います。

`USBHost::Keyboard` インスタンスを生成し、以下の API でキーボード情報を取得します。

- `Keyboard::IsPressed()` 指定されたキーが押されているかチェックします
- `Keyboard::ScanKeyCode()` 押されているキーのキーコードを返します
- `Keyboard::ScanKeyData()` 押されているキーの `KeyData` (キーコードとキャラクタコードを扱うデータ) を返します
- `Keyboard::GetKeyCode()` キーが押されている間、リピート処理をし、押されているキーのキーコードを返します。キーが押されていない場合ブロックします
- `Keyboard::GetKeyCodeNB()` キーが押されている間、リピート処理をし、押されているキーのキーコードを返します。キーが押されていない場合、即座に `false` を返します
- `Keyboard::GetKeyData()` キーが押されている間、リピート処理をし、押されているキーの `KeyData` を返します。キーが押されていない場合ブロックします
- `Keyboard::GetKeyDataNB()` キーが押されている間、リピート処理をし、押されているキーの `KeyData` を返します。キーが押されていない場合、即座に `false` を返します

`CMakeLists.txt` に以下の行を追加します。

```cmake:CMakeLists.txt
target_link_libraries(usbhosttest jxglib_USBHost)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhosttest CFG_TUH_HID 3)
```

`jxglib_configure_USBHost()` 関数は tinyusb のビルドに必要なヘッダファイル `tusb_config.h` の生成をします。引数 `CFG_TUH_HID` に、接続する HID インターフェースの最大数を指定します。

以下に `Keyboard::IsPresssed()` を使った例を示します。押したキーに対応する LED が光ります。

```cpp:usbhosttest.cpp
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"

using namespace jxglib;

int main()
{
    GPIO18.init().set_dir_OUT();
    GPIO19.init().set_dir_OUT();
    GPIO20.init().set_dir_OUT();
    GPIO21.init().set_dir_OUT();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    for (;;) {
        GPIO18.put(keyboard.IsPressed(VK_V));
        GPIO19.put(keyboard.IsPressed(VK_C));
        GPIO20.put(keyboard.IsPressed(VK_X));
        GPIO21.put(keyboard.IsPressed(VK_Z));
        Tickable::Sleep(50);
    }
}
```

メインループ中の `Tickable::Sleep()` は、遅延時間を msec で指定します。この関数の中で tinyusb や **pico-jxglib** のタスク処理が行われます。

以下に `Keyboard::GetKeyCodeNB()` を使った例を示します。押したキーに対応する LED がリピート処理の結果を受けて点滅します。

```cpp:usbhosttest.cpp
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"

using namespace jxglib;

int main()
{
    GPIO18.init().set_dir_OUT();
    GPIO19.init().set_dir_OUT();
    GPIO20.init().set_dir_OUT();
    GPIO21.init().set_dir_OUT();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    for (;;) {
        uint8_t keyCode;
        bool rtn = keyboard.GetKeyCodeNB(&keyCode);
        GPIO18.put(rtn && keyCode == VK_V);
        GPIO19.put(rtn && keyCode == VK_C);
        GPIO20.put(rtn && keyCode == VK_X);
        GPIO21.put(rtn && keyCode == VK_Z);
        Tickable::Sleep(50);
    }
}

```

### USB マウスの操作

USB マウスの機能も USBHost クラスで実装されています。

`USBHost::Mouse` インスタンスを生成し、`Mouse::CaptureStatus()` 関数でマウスの状態を表す `Mouse::Status` インスタンスを取得します。得られる情報は以下の通りです。

- マウスの現在位置
- 移動量
- ホイール操作量
- Pan 操作量

`CMakeLists.txt` に以下の行を追加します。前の USB キーボードのものと同じです。

```cmake:CMakeLists.txt
target_link_libraries(usbhosttest jxglib_USBHost)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhosttest CFG_TUH_HID 3)
```

マウスのデータを読みこむ例を以下に示します。マウスを左右に動かしたり、左クリック・右クリックをすると LED が点灯します。

```cpp:usbhosttest.cpp
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"

using namespace jxglib;

int main()
{
    GPIO18.init().set_dir_OUT();
    GPIO19.init().set_dir_OUT();
    GPIO20.init().set_dir_OUT();
    GPIO21.init().set_dir_OUT();
    USBHost::Initialize();
    USBHost::Mouse mouse;
    for (;;) {
        Mouse::Status status = mouse.CaptureStatus();
        GPIO21.put(status.GetDeltaX() < 0);
        GPIO20.put(status.GetDeltaX() > 0);
        GPIO19.put(status.GetButtonLeft());
        GPIO18.put(status.GetButtonRight());
        Tickable::Sleep(50);
    }
}
```

## LVGL と USB キーボード・マウス

LVGL は組込み機器で GUI を実装するためのライブラリです。詳しくは以下の記事を参照してください。

https://zenn.dev/ypsitau/articles/2025-02-04-lvgl

LVGL のユーザインターフェースにはタッチスクリーンを使うことが多いのですが、ここでは USB キーボードとマウスをつなげてみます。TFT LCD には ST7789 を使用します。他のデバイスを接続する場合は[「pico-jxblib と TFT LCD の話」](https://zenn.dev/ypsitau/articles/2025-01-27-tft-lcd) を参照してください。

ブレッドボードの配線イメージを以下に示します。

![circuit-usbhost-st7789.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/circuit-usbhost-st7789.png)

`CMakeLists.txt` の最後に以下の行を追加してください。

```cmake:CMakeLists.txt
target_link_libraries(usbhosttest jxglib_USBHost jxglib_Display_ST7789 jxglib_LVGL lvgl_examples)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhosttest CFG_TUH_HID 3)
jxglib_configure_LVGL(usbhosttest LV_FONT_MONTSERRAT_14)
```

ソースコードを以下のように編集します。

```cpp:usbhosttest.cpp
#include <lvgl/examples/lv_examples.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/LVGL.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::spi_init(spi1, 125 * 1000 * 1000);
    GPIO14.set_function_SPI1_SCK();
    GPIO15.set_function_SPI1_TX();
    USBHost::Initialize();
    USBHost::Keyboard keyboard;
    USBHost::Mouse mouse;
    Display::ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
    display.Initialize(Display::Dir::Rotate90);
    LVGL::Initialize(5);
    LVGL::Adapter lvglAdapter;
    lvglAdapter.AttachDisplay(display)
        .AttachKeyboard(keyboard)
        .AttachMouse(mouse);
    ::lv_example_keyboard_1();
    for (;;) Tickable::Tick();
}
```

![lvgl-usbhid.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-02-usbhost-keyboard-mouse/lvgl-usbhid.jpg)

`LVGL::Adapter` インスタンスに対して `AttachKeyboard()` や `AttachMouse()` を実行することで、USB キーボード・マウスを LVGL に接続します。`Tickable::Tick()` は tinyusb や LVGL、**pico-jxglib** のタスク処理を実行します。
