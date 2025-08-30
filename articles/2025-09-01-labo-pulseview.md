---
title: "pico-jxgLABO × PulseView: プローブ接続いらずで気軽に使えるロジックアナライザを体験しよう!"
emoji: "✨"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["raspberrypi", "pico", "raspberrypipico", "ロジックアナライザ", "PulseView"]
published: false
---
RaspberryPi Pico の実験プラットフォーム [pico-jxgLABO](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro) が [PulseView](https://sigrok.org/wiki/PulseView) に対応しました!

pico-jxgLABO と PulseView を組み合わせることで、プローブ接続を一切することなく Pico ボードの内部信号をロジックアナライザで観測できるようになります。

もちろん、Pico ボードをロジックアナライザ専用にして外部信号の観測をすることもできますし、内部・外部信号の両方を同時に観測することも可能です。

この記事では、pico-jxgLABO と PulseView のインストールと設定方法について説明し、実際に Pico ボードの波形観測を行います。ホスト PC として Windows を想定していますが、Linux などでも同様の手順で利用できると思います。

## pico-jxgLABO の導入方法

pico-jxgLABO の導入方法は[こちら](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro#pico-jxglabo-%E3%81%AE%E5%B0%8E%E5%85%A5%E6%96%B9%E6%B3%95)。特別なハードウェアは必要なく、Pico や Pico 2 ボードを USB ケーブルで PC に接続するだけで始められます。

ターミナルソフトから以下のように `about-me` コマンドを実行して pico-jxgLABO のバージョンが `0.2.0` 以降であることを確認してください。

```text
L:/>about-me
Program Information
 name:              pico-jxgLABO
 version:           0.2.0
     :
     :
```

## PulseView のインストールと接続方法

### インストール

PulseView はロジックアナライザの信号を可視化するためのツールです。I2C や SPI をはじめとする様々なプロトコルのデコードもサポートしているので、これらを活用することで pico-jxgLABO でキャプチャした信号を簡単に解析できます。

PulseView は以下のサイトで入手できます。

▶️ [sigrok Downloads](https://sigrok.org/wiki/Downloads)

Nightly builds の「PulseView (32bit)」または「PulseView (64bit)」を選択してダウンロード・インストールしてください。

### pico-jxgLABO と PulseView の接続

pico-jxgLABO と PulseView の接続手順は以下の通りです。

1. pico-jxgLABO を書き込んだ Pico ボードを USB ケーブルで PC に接続します

2. ターミナルソフトで pico-jxgLABO のロジックアナライザコマンド `la` を実行して、測定する GPIO ピン番号を指定します。以下の例では GPIO2, GPIO3, GPIO4 を測定対象にしています

   ```text
   L:/>la -p 2,3,4
   disabled ---- 12.5MHz (samplers:1) pins:2-4 events:0/0 (heap-ratio:0.7)
   ```

3. PulseView を起動すると以下のいずれかのメイン画面が表示されます。

   ![pulseview-main](/images/2025-09-01-labo-pulseview/pulseview-main.png)

   ![pulseview-main-demo](/images/2025-09-01-labo-pulseview/pulseview-main-demo.png)

   `<No Device>` または `Demo device` と表示されている部分をクリックして「Connect to Device」ダイアログを表示します

4. `Step 1: Choose the driver` のドロップダウンリストから `RaspberryPI PICO (raspberrypi-pico)` を選択します

   ![pulseview-connect-1](/images/2025-09-01-labo-pulseview/pulseview-connect-1.png)

5. `Step 2: Choose the interface` で `Serial Port` を選択します。pico-jxgLABO は二つのシリアルポートを提供しますが、二つ目のポート (以下の例では `COM25`) がロジックアナライザ用になるので、これを選択します。ボーレートは空白のままで大丈夫です

   ![pulseview-connect-2](/images/2025-09-01-labo-pulseview/pulseview-connect-2.png)

6. `Step 3: Scan for devices` の `Scan for devices using driver above` ボタンをクリックします。`Step 4: Select the device` のリスト中に `RaspberryPi PICO with 3 channels` というアイテムが表示されているのを確認して `OK` ボタンをクリックします

   ![pulseview-connect-3](/images/2025-09-01-labo-pulseview/pulseview-connect-3.png)

7. メイン画面が以下のようになります。

   ![pulseview-main-connected](/images/2025-09-01-labo-pulseview/pulseview-main-connected.png)

   `la` コマンドの `-p` オプションで指定した順に、各 GPIO の信号が `D2`, `D3`, `D4` として表示されます。

   取得するサンプル数が `1k samples`、サンプリングレートが `5 kHz` になっていますが、これらを以下のように変更します。

   - 取得するサンプル数: 最大の `1 G samples` に変更します
   - サンプリングレート: ターミナルソフトで `la` コマンドを実行すると、pico-jxgLABO のサンプリングレートを確認できます

     ```text
     L:/>la
     disabled ---- 12.5MHz (samplers:1) pins:2-4 events:0/0 (heap-ratio:0.7)
     ```

     今回の例では `12.5MHz` であることが分かったので、PulseView のサンプリングレートはこれよりも少し大きい `15 MHz` に設定します。

   ![pulseview-main-modified](/images/2025-09-01-labo-pulseview/pulseview-main-modified.png)

これで PulseView から pico-jxgLABO を操作できるようになりました。左上の `Run` ボタンをクリックすると、ラベルが `Stop` に変わって信号のキャプチャが開始されます。

![pulseview-main-running](/images/2025-09-01-labo-pulseview/pulseview-main-running.png)

`Stop` ボタンをクリックすると、キャプチャが停止して、観測した波形が表示されます。今はまだ信号が生成されていないので、波形は何も表示されません。

それでは、実際にいろいろな信号を生成して波形を観測してみましょう!

## Pico ボード内部信号の波形観測

ここでは、pico-jxgLABO のコマンドを使用して Pico ボード自体で I2C, SPI, UART, PWM の信号を生成し、その波形を観測します。また、それらの信号をプロトコルデコーダで解析する方法も説明します。

### I2C の波形観測

PulseView で `Run` ボタンをクリックしてキャプチャを開始した後、ターミナルソフトで以下のようにコマンドを実行します。

```text
L:/>i2c1 -p 2,3 scan
```

このコマンド操作で、GPIO2 と GPIO3 を I2C1 の SDA と SCL に割り当て、I2C アドレス 0x00 から 0x7f に Read リクエストを送ります。

PulseView で `Stop` ボタンをクリックしてキャプチャを停止すると、以下のようにキャプチャした波形が表示されます。`D2` が GPIO2 (I2C1 SDA)、`D3` が GPIO3 (I2C1 SCL) の信号です。

![pulseview-main-i2c](/images/2025-09-01-labo-pulseview/pulseview-main-i2c.png)

基本的なマウス操作方法は以下の通りです。

- ホイールで拡大・縮小ができます
- 左クリックをしたままドラッグすると表示範囲を移動できます

信号波形の最初の部分を拡大したのが以下の画像です。

![pulseview-main-i2c-zoom](/images/2025-09-01-labo-pulseview/pulseview-main-i2c-zoom.png)

下図の矢印で示されるボタン:

![pulseview-decoder-button](/images/2025-09-01-labo-pulseview/pulseview-decoder-button.png)

をクリックすると、`Decoder Selector` ペインが表示され、ここからプロトコルデコーダを選択できます。検索ボックスに `i2c` を入力し、リストに表示された `I2C` をダブルクリックすると、波形に I2C デコーダが追加されます。

![pulseview-decoder-selector](/images/2025-09-01-labo-pulseview/pulseview-decoder-selector.png)

下図の矢印で示されるボタン:

![pulseview-decoder-button](/images/2025-09-01-labo-pulseview/pulseview-decoder-selector-close.png)

をクリックすると、`Decoder Selector` ペインを非表示にします。

信号名の中の `I2C` ラベルを左クリックすると、プロトコルデコーダのパラメータを設定するダイアログが表示されます。`SCL` と `SDA` にそれぞれ `D3` と `D2` を設定します。

![pulseview-main-i2c-prop](/images/2025-09-01-labo-pulseview/pulseview-main-i2c-prop.png)

ダイアログを閉じると、I2C をデコードした結果を確認できます。

![pulseview-main-i2c-dec](/images/2025-09-01-labo-pulseview/pulseview-main-i2c-dec.png)

I2C アドレス 0x00 から 0x7f に対して Read リクエストが送信されているのが分かります。

### SPI の波形観測

PulseView で `Run` ボタンをクリックしてキャプチャを開始した後、ターミナルソフトで以下のようにコマンドを実行します。

```text
L:/>spi0 -p 2,3 write:0-255
```

このコマンド操作で、GPIO2 と GPIO3 を SPI0 の MOSI と SCK に割り当て、0 から 255 までのデータを送信します。

PulseView で `Stop` ボタンをクリックしてキャプチャを停止すると、以下のようにキャプチャした波形が表示されます。`D2` が GPIO2 (SPI0 SCK)、`D3` が GPIO3 (SPI0 MOSI) の信号です。

![pulseview-main-spi](/images/2025-09-01-labo-pulseview/pulseview-main-spi.png)

信号波形の最初の部分を拡大したのが以下の画像です。

![pulseview-main-spi-zoom](/images/2025-09-01-labo-pulseview/pulseview-main-spi-zoom.png)

`Decoder Selector` ペインを表示して検索ボックスに `spi` を入力し、リストに表示された `SPI` をダブルクリックすると、波形に SPI デコーダが追加されます。信号名の中の `SPI` ラベルを左クリックするとプロトコルデコーダのパラメータを設定するダイアログが表示されるので、`CLK` と `MOSI` にそれぞれ `D2` と `D3` を設定します。

![pulseview-main-spi-prop](/images/2025-09-01-labo-pulseview/pulseview-main-spi-prop.png)

ダイアログを閉じると、SPI をデコードした結果を確認できます。

![pulseview-main-spi-dec](/images/2025-09-01-labo-pulseview/pulseview-main-spi-dec.png)

SPI の MOSI に 0 から 255 までのデータが送信されているのが分かります。

### UART の波形観測

PulseView で `Run` ボタンをクリックしてキャプチャを開始した後、ターミナルソフトで以下のようにコマンドを実行します。

```text
L:/>uart1 -p 4 write:0-255,0
```

このコマンド操作で、GPIO4 を UART1 の TX に割り当て、0 から 255 および最後に 0 のデータを送信します。最後に 0 を送信するのは、最終データが 255 だと PulseView の UART プロトコルデコーダがそのデータのストップビットを正しく認識できないためです。

PulseView で `Stop` ボタンをクリックしてキャプチャを停止すると、以下のようにキャプチャした波形が表示されます。`D4` が GPIO4 (UART1 TX) の信号です。

![pulseview-main-uart](/images/2025-09-01-labo-pulseview/pulseview-main-uart.png)

信号波形の最初の部分を拡大したのが以下の画像です。

![pulseview-main-uart-zoom](/images/2025-09-01-labo-pulseview/pulseview-main-uart-zoom.png)

`Decoder Selector` ペインを表示して検索ボックスに `uart` を入力し、リストに表示された `UART` をダブルクリックすると、波形に UART デコーダが追加されます。信号名の中の `UART` ラベルを左クリックするとプロトコルデコーダのパラメータを設定するダイアログが表示されるので、`TX` に `D4` を設定します。

![pulseview-main-uart-prop](/images/2025-09-01-labo-pulseview/pulseview-main-uart-prop.png)

ダイアログを閉じると、UART をデコードした結果を確認できます。

![pulseview-main-uart-dec](/images/2025-09-01-labo-pulseview/pulseview-main-uart-dec.png)

UART の TX に 0 から 255 および 0 のデータが送信されているのが分かります。

### PWM の波形観測

PulseView で `Run` ボタンをクリックしてキャプチャを開始した後、ターミナルソフトで以下のようにコマンドを実行します。

```text
L:/>pwm 2,3,4 func:pwm freq:1000 counter:0
L:/>pwm2 duty:.2; pwm3 duty:.5; pwm4 duty:.8
L:/>pwm 2,3,4 enable
```

このコマンド操作で、GPIO2, GPIO3, GPIO4 のファクションを PWM にし、周波数を 1kHz に設定します。デューティ比はそれぞれ 20%, 50%, 80% に設定し、PWM を有効にします。

PulseView で `Stop` ボタンをクリックしてキャプチャを停止すると、以下のようにキャプチャした波形が表示されます。

![pulseview-main-pwm](/images/2025-09-01-labo-pulseview/pulseview-main-pwm.png)

信号波形の最初の部分を拡大したのが以下の画像です。

![pulseview-main-pwm-zoom](/images/2025-09-01-labo-pulseview/pulseview-main-pwm-zoom.png)

`Decoder Selector` ペインを表示して検索ボックスに `pwm` を入力し、リストに表示された `PWM` をダブルクリックすると、波形に PWM デコーダが追加されます。PWM デコーダを全部で 3 つ追加します。信号名の中の `PWM` ラベルを左クリックするとプロトコルデコーダのパラメータを設定するダイアログが表示されるので、それぞれの PWM デコーダの `Data` に `D2`, `D3`, `D4` を設定します。

![pulseview-main-pwm-prop](/images/2025-09-01-labo-pulseview/pulseview-main-pwm-prop.png)

ダイアログを閉じると、PWM をデコードした結果を確認できます。

![pulseview-main-pwm-dec](/images/2025-09-01-labo-pulseview/pulseview-main-pwm-dec.png)

デューティ比がそれぞれ 20%, 50%, 80% で周波数 1.0kHzの PWM 信号が生成されているのが分かります。

PWM の操作方法は以下の記事で詳しく説明しているので、参考にしてください。

▶️ [操作方法を知って使いこなす! pwm コマンドで Pico の PWM の限界を探る](https://zenn.dev/ypsitau/articles/2025-08-06-labo-pwm)

## `la` コマンドのオプション

`la` は pico-jxgLABO のロジックアナライザコマンドです。`la` コマンドのオプションを変更することで、測定する GPIO ピン番号や測定対象 (内部信号/外部信号)、サンプリングレートなどを指定できます。

### 測定する GPIO ピン番号の指定方法

`la` コマンドの `-p` オプションで測定する GPIO ピン番号を指定します。単一の GPIOピン番号を指定することもできますし、範囲を指定することもできます。例を以下に示します。

|コマンド　           |説明                                 |
|--------------------|-------------------------------------|
|`la -p 0`           |GPIO0 を測定対象にします               |
|`la -p 2,3,8,9`     |GPIO2,3,8,9 を測定対象にします         |
|`la -p 2-15`        |GPIO2 から GPIO15 までを測定対象にします|
|`la -p 2-5,8-10,15` |GPIO2 から GPIO5, GPIO8 から GPIO10, GPIO15 を測定対象にします|

PulseView で表示されるラベルは、ここで設定した GPIO ピン番号にかかわらず常に `D2`, `D3`, `D4` ... となります。 ラベルを左クリックすることで表示名を変えることができるので、分かりやすい名前に変更しておくと良いでしょう。

### 測定対象の指定方法

測定対象の信号を Pico ボード内部にある信号 (internal) にするか、Pico ボードの GPIO ピンに入力される外部信号 (external) にするかを `--target` オプションで指定します。指定しない場合は、内部信号が測定対象になります。

|コマンド　           |説明                                 |
|--------------------|-------------------------------------|
|`la --target:external`|外部信号を測定対象にします           |
|`la --target:internal`|内部信号を測定対象にします           |

`--target` オプションは全体の GPIO ピンの測定対象を指定しますが、オプション `--internal` や `--external` を使うことで一部の GPIO ピンを内部信号または外部信号として指定することができます。例を以下に示します。

|コマンド　           |説明                                 |
|--------------------|-------------------------------------|
|`la --target:external --internal:2,3,4`|GPIO2,3,4 は内部信号、それ以外は外部信号|
|`la --target:internal --external:2,3,4`|GPIO2,3,4 は外部信号、それ以外は内部信号|

### サンプリングレート

pico-jxgLABO はサンプラーと呼ばれる実行単位を用いて信号のサンプリングを行い、最大 4 個まで動作させることができます。一つのサンプラーは最大 12.5MHz (Pico2) または 10.4MHz (Pico) のサンプリングレートで動作し、全体のサンプリングレートはこのサンプリングレートにサンプラー数を乗じた値になります。動作させるサンプラーの数はオプション `--samplers` で指定します。以下に例を示します。

|コマンド　           |サンプリングレート                  |
|--------------------|-------------------------------------|
|`la --samplers:1`   |12.5MHz (Pico2), 10.4MHz (Pico)  |
|`la --samplers:2`   |25.0MHz (Pico2), 20.8MHz (Pico)  |
|`la --samplers:3`   |37.5MHz (Pico2), 31.2MHz (Pico)  |
|`la --samplers:4`   |50.0MHz (Pico2), 41.7MHz (Pico)  |

サンプラーの数を増やすとサンプリングレートが上がりますが、サンプリングできるイベント数が減ります。

## Pico ボードをロジックアナライザ専用にして使用する方法

Pico ボードをロジックアナライザ専用にして、外部信号を観測することも可能です。例えば、以下のように `la` コマンドを実行すると、GPIO2 から GPIO22 および GPIO26 から GPIO28 の合計 24 本の GPIO ピンをロジックアナライザで観測できます。

```text
L:/>la -p 2-22,26-28 --target:external
```

以下の内容で `autoexec.sh` という名前のファイルを `L:` ドライブのルートディレクトリに作成しておくと、Pico ボードの電源を入れたときに自動的にロジックアナライザの GPIO ピン設定が行われるようになります。ターミナルソフトでコマンド操作をする必要がなくなるので便利です。

```text:autoexec.sh
la -p 2-22,26-28 --target:external
```

## まとめ

本記事では、pico-jxgLABO と PulseView のインストールと設定方法について説明し、実際に Pico ボードの波形観測を行いました。また、プロトコルデコーダを使用して、I2C, SPI, UART, PWM の信号を解析する方法も紹介しました。これらを活用することで、Pico ボードの内部信号や外部信号を簡単に観測・解析できるようになります。

pico-jxgLABO と PulseView を組み合わせて、快適な信号解析ライフを楽しんでください!
