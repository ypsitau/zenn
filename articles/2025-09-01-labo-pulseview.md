---
title: "pico-jxgLABO × PulseView: "
emoji: "✨"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["raspberrypi", "pico", "raspberrypipico", "ロジックアナライザ", "PulseView"]
published: false
---
RaspberryPi Pico の実験プラットフォーム [pico-jxgLABO](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro) が [PulseView](https://sigrok.org/wiki/PulseView) に対応しました!

Pico ボードの内部信号を、配線なしでロジックアナライザで観測できるようになります。これにより、より簡単に信号の解析やデバッグが行えるようになります。

もちろん、通常のロジックアナライザのように外部信号の観測も可能です。この記事では、pico-jxgLABO と PulseView の連携方法について説明します。実行環境として、Windows を想定しています。

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

### PulseView と pico-jxgLABO の接続

PulseView と pico-jxgLABO の接続手順は以下の通りです。

1. pico-jxgLABO を書き込んだ Pico ボードを USB ケーブルで PC に接続します

2. ターミナルソフトで pico-jxgLABO のロジックアナライザコマンド `la` を実行して、測定対象にする GPIO ピン番号を指定します。以下の例では GPIO2, GPIO3, GPIO4 を測定対象にしています

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

   取得するサンプル数が `1k samples`、サンプリングレートが `5 kHz` になっていますが、これらを以下のように変更します。

   - 取得するサンプル数: 最大の `1 G samples` に変更します
   - サンプリングレート: ターミナルソフトで `la` コマンドを実行すると、pico-jxgLABO のサンプリングレートを確認できます

     ```text
     L:/>la
     disabled ---- 12.5MHz (samplers:1) pins:2-4 events:0/0 (heap-ratio:0.7)
     ```

     今回の例では `12.5MHz` であることが分かったので、これよりも少し大きい `15 MHz` に設定します。

   ![pulseview-main-modified](/images/2025-09-01-labo-pulseview/pulseview-main-modified.png)

これで PulseView から pico-jxgLABO を操作できるようになりました。左上の `Run` ボタンをクリックすると、ラベルが `Stop` に変わって信号のキャプチャが開始されます。

![pulseview-main-running](/images/2025-09-01-labo-pulseview/pulseview-main-running.png)

`Stop` ボタンをクリックすると、キャプチャが停止して、観測した波形が表示されます。

それでは、実際に波形を観測してみましょう!

## 各種信号の波形観測

ここでは、pico-jxgLABO のコマンドを使用して I2C, SPI, UART の信号を生成し、その波形を観測します。

### I2C の波形観測

PulseView で `Run` ボタンをクリックしてキャプチャを開始した後、ターミナルソフトで以下のようにコマンドを実行します。

```text
L:>i2c1 -p 2,3 scan
```

PulseView で `Stop` ボタンをクリックしてキャプチャを停止すると、以下のようにキャプチャした波形が表示されます。

![pulseview-main-i2c](/images/2025-09-01-labo-pulseview/pulseview-main-i2c.png)

基本的なマウス操作方法は以下の通りです。

- ホイールで拡大・縮小ができます
- 左クリックをしたままドラッグすると表示範囲を移動できます

信号波形の最初の部分を拡大したのが以下の画像です。

![pulseview-main-i2c-zoom](/images/2025-09-01-labo-pulseview/pulseview-main-i2c-zoom.png)

右上の ![pulseview-decoder-button](/images/2025-09-01-labo-pulseview/pulseview-decoder-button.png) をクリックすると、`Decoder Selector` ペインが表示され、ここからプロトコルデコーダを選択できます。検索ボックスに `i2c` を入力し、リストに表示された `I2C` をダブルクリックすると、波形に I2C デコーダが追加されます。

![pulseview-decoder-selector](/images/2025-09-01-labo-pulseview/pulseview-decoder-selector.png)

![pulseview-main-i2c-prop](/images/2025-09-01-labo-pulseview/pulseview-main-i2c-prop.png)

![pulseview-main-i2c-dec](/images/2025-09-01-labo-pulseview/pulseview-main-i2c-dec.png)


### SPI の波形観測

### UART の波形観測


