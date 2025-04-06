---
title: "pico-jxblib と TFT LCD の話 (続き)"
emoji: "😸"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["RaspberryPiPico", "TFTLCD", "library", "Cpp"]
published: false
---
[**pico-jxglib**](https://qiita.com/ypsitau/items/ca5fb14f0bda56e84486) は、ワンボードマイコン Raspberry Pi Pico の Pico SDK プログラミングをサポートするライブラリです。

前の記事では、このライブラリを使って TFT LCD の描画をしました。

https://qiita.com/ypsitau/items/300472e2dee582361303

今回は TFT LCD の初期化と描画方法についての詳細、そして複数の LCD を接続する方法について説明します。

## プログラムの解説

[前回のサンプル](https://qiita.com/ypsitau/items/300472e2dee582361303#st7789-%E3%81%AE%E6%8E%A5%E7%B6%9A)ではデモのためにテスト用関数を使いましたが、今回は素の API を使ってそれぞれの動作を見ていきたいと思います。ソースファイル `lcdtest.cpp` を以下のように書き換えます。

```cpp:lcdtest.cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/Font/shinonome16-japanese-level1.h"

using namespace jxglib;

int main()
{
    // Initialize devices
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	display.Initialize(Display::Dir::Rotate0);
	// Display items
	display.DrawImage(20, 20, image_cat_240x320, {20, 20, 200, 200});
	display.SetFont(Font::shinonome16);
	const char* str = "吾輩は猫である";
	Size sizeStr = display.CalcStringSize(str);
	int x = (display.GetWidth() - sizeStr.width) / 2, y = 230;
	display.DrawString(x, y, str);
	display.DrawRect(x - 8, y - 4, sizeStr.width + 8 * 2, sizeStr.height + 4 * 2, Color::white);
	display.DrawRectFill(0, 260, 55, 60, Color::red);
	display.DrawRectFill(60, 260, 55, 60, Color::green);
	display.DrawRectFill(120, 260, 55, 60, Color::blue);
	display.DrawRectFill(180, 260, 55, 60, Color::aqua);
}
```
![lcdtest-apis.jpg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/48975/9743f051-8e12-5d5b-afd2-92558adbea51.jpeg)

ソースファイルの前半でデバイスの初期化を行っています。

```cpp
::spi_init(spi1, 125 * 1000 * 1000);`
```
Pico SDK の関数です。SPI1 を動作クロック 125MHz で初期化します
```cpp
GPIO14.set_function_SPI1_SCK();
GPIO15.set_function_SPI1_TX();
```
GPIO14 と GPIO15 をそれぞれ SPI1 の SCK、TX (MOSI) に設定します
```cpp
ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
```
ST7789 の操作をするインスタンスを生成します。接続する SPI、ディプレイサイズ、接続する GPIO (RST: Reset、DC: Data/Command、CS: Chip Select、BL: Backlight) を指定します
```cpp
display.Initialize(Display::Dir::Rotate0);
```
LCD を初期化して描画可能な状態にします。引数には以下に示す LCD の描画方向を指定します
- `Display::Dir::Rotate0` または `Display::Dir::Normal` ... 通常方向で描画をします
- `Display::Dir::Rotate90` ... 90 度回転します
- `Display::Dir::Rotate180` ... 180 度回転します
- `Display::Dir::Rotate270` ... 270 度回転します
- `Display::Dir::MirrorHorz` ... 水平方向に鏡像反転します
- `Display::Dir::MirrorVert` ... 垂直方向に鏡像反転します

これ以降は `display` インスタンスに対して描画処理をすることができます。

```cpp
display.DrawImage(20, 20, image_cat_240x320, {20, 20, 200, 200});
```
指定した座標にイメージを描画します。四番目の引数はイメージ内のクリッピング領域を指定します
```cpp
display.SetFont(Font::shinonome16);
```
インクルードファイル `jxglib/Font/shinonome16-japanese-level1.h` で定義されているフォントデータ `Font::shinonome16` を指定します
```cpp
Size sizeStr = display.CalcStringSize(str);
```
指定したフォントで文字列を描画したときのサイズを計算します
```cpp
display.DrawString(x, y, str);
```
指定した座標に文字列を描画します
```cpp
display.DrawRect(x - 8, y - 4, sizeStr.width + 8 * 2, sizeStr.height + 4 * 2, Color::white);
```
座標・大きさ・色を指定して矩形を描画します
```cpp
display.DrawRectFill(0, 260, 55, 60, Color::red);
display.DrawRectFill(60, 260, 55, 60, Color::green);
display.DrawRectFill(120, 260, 55, 60, Color::blue);
display.DrawRectFill(180, 260, 55, 60, Color::aqua);
```
座標・大きさ・色を指定して塗りつぶし矩形を描画します

- - -

**pico-jxglib** が提供する描画機能はここに挙げたものでほぼ全てです。円や直線描画は？と思われるかもしれません。でも、ユーザがグラフィック描画に求める仕様は非常に高く、それらを自前で苦労して作っても結局は中途半端なものになってしまい、実用にはならないのです。ですから、そういう高度な描画処理は専用のライブラリにまかせるというポリシーをとります。例えばボタンやリストボックスといった Windows のような GUI が必要ならば、[LVGL](https://lvgl.io/) という優れたライブラリがあります。**pico-jxglib** はそのようなライブラリへの橋渡しをするアダプタを提供します。

## 複数の LCD 接続

LCD デバイスに CS (Chip Select) 端子がついていれば、同じ SPI インターフェースに並列で複数接続することができます。

ここでは、ST7789 を 2 個接続してみます。ブレッドボードの配線イメージは以下の通りです。

![circuit-st7789-multi.png](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/48975/41c901cf-13d9-7372-6d9d-f877c62e0740.png)

ソースファイル `lcdtest.cpp` を以下のように書き換えます。

```cpp:lcdtest.cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/ST7789.h"
#include "jxglib/Font/shinonome16-japanese-level1.h"
#include "jxglib/sample/cat-240x320.h"
#include "jxglib/sample/Text_Botchan.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display1(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	ST7789 display2(spi1, 240, 320, {RST: GPIO18, DC: GPIO19, CS: GPIO20, BL: GPIO21});
	display1.Initialize(Display::Dir::Rotate0);
	display2.Initialize(Display::Dir::Rotate90);
	display1.DrawImage(0, 0, image_cat_240x320);
	display2.SetFont(Font::shinonome16);
	display2.DrawStringWrap(0, 0, Text_Botchan);
}
```

![lcdtest-multi.jpg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/48975/0d021185-b453-025c-7673-73fd6a610afb.jpeg)

GPIO さえ確保すれば、同じ SPI インターフェースにいくつでも LCD をぶらさげられる ... と言いたいところですが、あまりつなげると信号波形が劣化するようです。4 個の LCD を同一の SPI に接続できる[^multi-connect]ことを確認しましたが、そのうちのひとつを ILI9341 にすると表示がされませんでした。

[^multi-connect]: このときは、さすがに GPIO が足りなくなくなったので BL (バックライト) 端子を VCC (3.3V) に接続しました。

## 次回の記事

次回は **pico-jxglib** を使って、LVGL による高度なグラフィカルユーザインターフェースを実現します。

https://qiita.com/ypsitau/items/be620ca50c23b115e00a
