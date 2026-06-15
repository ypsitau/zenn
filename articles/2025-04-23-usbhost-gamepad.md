---
title: "pico-jxglib で Pico ボードに USB ゲームパッドを接続してゲームを楽しむ話"
emoji: "🎮"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["組み込み", "raspberrypi", "pico", "raspberrypipico", "usb"]
published: true
---
**pico-jxglib** は、ワンボードマイコン Raspberry Pi Pico の Pico SDK プログラミングをサポートするライブラリです。

▶️ [pico-jxglib: A "Pseudo" OS for Raspberry Pi Pico](https://ypsitau.github.io/pico-jxglib/)

以前に[「pico-jxglib で Pico ボードに USB キーボード・マウスを接続する話」](https://zenn.dev/ypsitau/articles/2025-04-02-usbhost-keyboard-mouse)という記事を上げましたが、今回は Pico の USB ホスト機能を使って、Pico ボードに USB ゲームパッドを接続します。ゲームも楽しみますよ。

## USB ゲームパッドについて

ゲームパッドはその名の通り、本来ゲームを楽しむためのデバイスですが、ロボットを操作するマニピュレータとしても役立ちます。アナログジョイスティックが左右で 2 個、十字キー入力、10 個以上のボタンという豊富な入力手段を備えており、それらが左右の手で効率よく操作できるような筐体に収められているので、これが数千円で入手できるのは非常に魅力的です。

ただ、多くのメーカがいろいろな仕様で製品を出してきたため、規格としてはかなり混沌としています。どの製品を選べばよいか指標にもなると思うので、以下にゲームパッドの仕様について簡単にまとめます。

### DirectInput と XInput

USB ゲームパッドのインターフェースの規格には **DirectInput** と **XInput** の二つが存在します。**pico-jxglib** は **DirectInput** をサポートします。

**DirectInput** は古くからある規格で、USB の HID クラスとして実装されています。HID クラスは、キーボード・マウス・ボタン・アナログジョイスティック・タッチパネル・ポインティングデバイスなど、様々なヒューマンインターフェースに対応するため、これらのデータフォーマットをデバイス側で定義することができる Report Descriptor という仕様を定めています。これによってボタンなどの配置や数を自由にレイアウトしたデバイスを作れるのですが、柔軟性の豊富さは無秩序につながります。製品によってレポートデータ中のビット配置が異なるのはよいのですが、Report Descriptor 中でボタンなどの用途を定めた Usage 値の解釈も、メーカごと、ときによっては同じメーカでも製品ごとに異なります。PC 用のマルチメディアライブラリ SDL はゲームパッドの API も提供しますが、この差異を[デバイスごとのデーターベース](https://github.com/mdqinc/SDL_GameControllerDB)で個別に吸収しています。

**XInput** は Microsoft がゲームコンソール XBox を開発する際に提唱したゲームパッド専用のインターフェースです。DirectInput の仕様があまりにも無秩序になったのを嫌ったのか、ボタンやジョイスティックの配置や数を完全に規定しています。ただ、HID クラスではなく、Vendor ID が `0x045E` (Microsoft)、Product ID が `0x028E` (XBox360 Controller) などのベンダー固有クラスとして扱われます。ベンダー固有クラスですから、サードパーティが XInput に対応する際には、INF ファイルなどを使って自社の Vendor ID および Product ID を OS にインストールする必要があります[^vendorid]。

[^vendorid]: ゲームパッドによっては、Microsoft の Vendor ID や Product ID を自社の製品に勝手につけてしまっているものも存在します。インストール作業なしで接続できる簡便さを得ようとしたのでしょうが、デバイス詐称ともいえますので、問題がありそうです。

サードベンダが提供しているゲームパッドには、DirectInput と XInput をスイッチで切り替えられるようになっているものが多く存在します。ただし XBox コントローラと銘打って発売しているゲームパッドは、当然 XInput のみサポートします。

### Pico ボードに接続できる USB ゲームパッド

市販されている全ての USB ゲームパッドの中で、Pico ボードに接続できるものはかなり限定されます。異なるベンダから 3 個のゲームパッドを入手しましたが、接続できたのはそのうちの一つでした。

- EasySMX 社のワイヤレスタイプ ESM-9013 を試したところ、USB 端子にレシーバを接続しても認識がされませんでした。接続した状態でボードをリセットすると認識するのですが、USB ハブを経由するとやはり不安定です。接続に成功した後は、レポートの受信が正常に行えました。

- Logicool 社の有線タイプ F310 はかなりメジャーな製品ですが、今のところ使えていません。ハードウェアの接続性に問題はなく、Descriptor の取得までは成功するのですが、レポートの受信ができません。何かのきっかけで受信ができたこともあったので tinyusb の修正で解決すると思うのですが、原因は不明です。

- **Elecom 社のワイヤレスタイプ JC-U4113S** が手元にある中では唯一使えるゲームパッドとなりました。

## Report Descriptor パーサについて

USB HID クラスのデバイスは report と呼ばれるデータを一定周期ごとにホストに送出しますが、このデータフォーマットは Report Descriptor で定義されます。Report Descriptor は、キーボード、ボタン、ジョイスティックなどの情報が report 中に格納されている位置や、最大・最小値、使用用途などの情報を提供します。

様々なデータに対応するため、フォーマット仕様は少々複雑です。今回、USB ゲームパッドから情報を得る際、たかだか数十バイトのデータを扱うために大層なパーサを作ることにはためらいがありました。特定の USB ゲームパッドに対応させるのならば、構造体などをハードコーディングすればよいのですから。実際、tinyusb のサンプルにゲームパッドを扱うものがありますが、それはそのように実装しています。

でも、どうしてもむずむずと落ち着かなくて、Report Descriptor の情報をすべて得られるようなパーサを組み込みました。これでいろいろなゲームパッドに対応できるはずですが、それ以外の用途にも使わないともったいないので、独立した記事に詳細を記したいと思っています。

## 開発環境のセットアップ

Visual Studio Code や Git ツール、Pico SDK のセットアップが済んでいない方は[「Pico SDK ことはじめ」](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E9%96%8B%E7%99%BA%E7%92%B0%E5%A2%83) をご覧ください。

GitHub から **pico-jxglib** をクローンします。

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

## モニタプログラムの作成

ゲームパッドのボタンやジョイスティック情報を TFT LCD に表示するモニタプログラムを作成します。

### 配線イメージ

TFT LCD に ILI9341 を使うブレッドボードの配線イメージを以下に示します。USB ゲームパッドは、microB から A タイプに変換する OTG ケーブルで接続します。圧電ブザーがついているのは、後に紹介するゲームのためです。

![circuit-ili9341.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/circuit-ili9341.png)

TFT LCD に ST7789 を使うブレッドボードの配線イメージを以下に示します。

![circuit-st7789.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/circuit-st7789.png)

### プロジェクトの作成

VSCode のコマンドパレットから `>Raspberry Pi Pico: New Pico Project` を実行し、以下の内容でプロジェクトを作成します。Pico SDK プロジェクト作成の詳細や、ビルド、ボードへの書き込み方法については[「Pico SDK ことはじめ」](https://zenn.dev/ypsitau/articles/2025-01-17-picosdk#%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AE%E4%BD%9C%E6%88%90%E3%81%A8%E7%B7%A8%E9%9B%86) を参照ください。

- **Name** ... プロジェクト名を入力します。今回は例として `usbhost-gamepad-monitor` を入力します
- **Board type** ... ボード種別を選択します
- **Location** ... プロジェクトディレクトリを作る一つ上のディレクトリを選択します
- **Stdio support** .. どちらも未チェックのままにしておきます
- **Code generation options** ... **`Generate C++ code` にチェックをつけます**

プロジェクトディレクトリと `pico-jxglib` のディレクトリ配置が以下のようになっていると想定します。

```text
├── pico-jxglib/
└── usbhost-gamepad-monitor/
    ├── CMakeLists.txt
    ├── usbhost-gamepad-monitor.cpp
    └── ...
```

`CMakeLists.txt` の最後に以下の行を追加してください。

```cmake:CMakeLists.txt
target_link_libraries(usbhost-gamepad-monitor jxglib_USBHost jxglib_Display_ILI9341 jxglib_Display_ST7789)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_USBHost(usbhost-gamepad-monitor CFG_TUH_HID 3)
```

ソースファイルを以下のように編集します。

```cpp:usbhost-gamepad-monitor.cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Display/ST7789.h"
#include "jxglib/USBHost/HID.h"
#include "jxglib/Font/naga10.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::spi_init(spi0, 125 * 1000 * 1000);
    GPIO18.set_function_SPI0_SCK();
    GPIO19.set_function_SPI0_TX();
    Display::Terminal terminal;
    Display::ILI9341 display(spi0, 240, 320, {RST: GPIO13, DC: GPIO12, CS: GPIO11, BL: GPIO::None});
    //Display::ST7789 display(spi0, 240, 320, {RST: GPIO13, DC: GPIO12, CS: GPIO11, BL: GPIO::None});
    display.Initialize(Display::Dir::Rotate270);
    terminal.Initialize().AttachDisplay(display).SetFont(Font::naga10);
    USBHost::Initialize();
    USBHost::GamePad gamePad;
    terminal.Printf("USB GamePad Monitor\n");
    for (;;) {
        if (gamePad.HasReportChanged()) {
            terminal.Printf("%s%s%s%s%s%s%s%s%s%s%s%s%s %X % 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f% 1.2f\n",
                gamePad.Get_Button0()? "0" : ".",
                gamePad.Get_Button1()? "1" : ".",
                gamePad.Get_Button2()? "2" : ".",
                gamePad.Get_Button3()? "3" : ".",
                gamePad.Get_Button4()? "4" : ".",
                gamePad.Get_Button5()? "5" : ".",
                gamePad.Get_Button6()? "6" : ".",
                gamePad.Get_Button7()? "7" : ".",
                gamePad.Get_Button8()? "8" : ".",
                gamePad.Get_Button9()? "9" : ".",
                gamePad.Get_Button10()? "A" : ".",
                gamePad.Get_Button11()? "B" : ".",
                gamePad.Get_Button12()? "C" : ".",
                gamePad.Get_HatSwitch(),
                gamePad.Get_Axis0(),
                gamePad.Get_Axis1(),
                gamePad.Get_Axis2(),
                gamePad.Get_Axis3(),
                gamePad.Get_Axis4(),
                gamePad.Get_Axis5(),
                gamePad.Get_Axis6(),
                gamePad.Get_Axis7(),
                gamePad.Get_Axis8());
        }
        Tickable::Sleep(300);
    }
}
```

`USBHost::GamePad` インスタンスを作成してゲームパッドの情報を取得します。Get_Button0() ～ Get_Button12() はボタン情報を取得する関数で、押下状態によって true または false を返します。Get_Axis0() ～ Get_Axis8() はアナログジョイスティックの操作量で、-1 から +1 の数値を返します。


## ゲームパッドでゲームを楽しむ

せっかくゲームパッドを接続したのですから、本来の目的であるゲームを楽しみたいものです。[KenKen さん](http://www.ze.em-net.ne.jp/~kenken/index.html) の [Raspberry Pi Pico による液晶ゲーム製作](http://www.ze.em-net.ne.jp/~kenken/picogames/index.html) で Pico ボードで動作するクオリティの高いゲームを見つけましたので、ゲームパッドでこれらのゲームを遊べるようにしました。

ゲームパッドの各部への操作の割り当ては以下の通りです。前述したように、各ボタンやアナログジョイスティックが実際のゲームパッドのどこに割り当てられているのかは機種によって異なりますが、この程度の割り当てならばだいたい当てはまると思います (右アナログジョイスティックは除く)。

- 左右アナログジョイスティック ... 上・下・左・右
- 十字キー ... 上・下・左・右
- A, B, X, Y ボタン ... Fire
- その他のボタン ... Start

配線は、[前述の図](#%E9%85%8D%E7%B7%9A%E3%82%A4%E3%83%A1%E3%83%BC%E3%82%B8)を参照ください。

### ビルド方法

1. レポジトリをクローンします。

   ```bash
   git clone https://github.com/ypsitau/picopacman
   git clone https://github.com/ypsitau/picotetris
   git clone https://github.com/ypsitau/picoinvader
   git clone https://github.com/ypsitau/picohakomusu
   git clone https://github.com/ypsitau/picopegsol
   git clone https://github.com/ypsitau/pico-jxglib
   cd pico-jxglib
   git submodule update --init
   ```

1. ビルドするゲームのディレクトリに移り、`code .` を実行して VSCode を起動します。`Do you want to import this project as Raspberry Pi Pico project?` というメッセージボックスが表示されるので、`[Yes]` をクリックします

1. `Import Pico Project` というペインが開くので、内容を確認して `[Import]` をクリックします

1. コマンドパレットから `>Raspberry Pi Pico: Switch Board` を実行して、使用するボードを選択します

1. コマンドパレットから `>CMake: Build` を実行、または `[F7]` キーを押します。`Select a kit for project` というコンボボックスが出るので `Pico Using compilers:...` を選択してビルドを行います

デフォルトでは ILI9341 を TFT LCD に使用する設定になっています。ST7789 を使用する場合は `ili9341_spi.c` 中に記述されている　`//#define USE_ST7789` のコメントを外してください。

### パックマン

UF2 ファイル

- [Pico ボード + ILI9341](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico-ili9341.uf2)
- [Pico ボード + ST7789](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico-st7789.uf2)
- [Pico2 ボード + ILI9341](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico2-ili9341.uf2)
- [Pico2 ボード + ST7789](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico2-st7789.uf2)

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picopacman.jpg)

### テトリス

UF2 ファイル

- [Pico ボード + ILI9341](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico-ili9341.uf2)
- [Pico ボード + ST7789](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico-st7789.uf2)
- [Pico2 ボード + ILI9341](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico2-ili9341.uf2)
- [Pico2 ボード + ST7789](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico2-st7789.uf2)

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picotetris.jpg)

### インベーダーゲーム

UF2 ファイル

- [Pico ボード + ILI9341](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico-ili9341.uf2)
- [Pico ボード + ST7789](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico-st7789.uf2)
- [Pico2 ボード + ILI9341](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico2-ili9341.uf2)
- [Pico2 ボード + ST7789](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico2-st7789.uf2)

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picoinvader.jpg)

### 箱入り娘パズル

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picohakomusu.jpg)

### ペグソリティア

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picopegsol.jpg)
