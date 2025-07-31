---
title: "Pico ボードは実験室! pico-jxgLABO でロジックアナライザを動かす"
emoji: "🐙"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["組み込み", "raspberrypi", "pico", "raspberrypipico", "ロジックアナライザ"]
published: false
---
Raspberry Pi Pico は Raspberry Pi Ltd. が提供する小型で安価 (約 800 円) なマイコンボードです。初代の Pico は 125MHz の 32bit ARM コア (しかもデュアル)、264KByte の SRAM, 2MByte のフラッシュメモリ。Pico2 にいたっては CPU 150MHz (デュアル), 520KByte の SRAM, 4MByte のフラッシュメモリ、ハードウエア浮動小数点演算をサポートするなど、非常に高性能なマイコンボードです。

![pico-and-pico2.jpg](/images/2025-07-23-labo-logicanalyzer/pico-and-pico2.jpg)

でもそんな豊富な機能も、使い方を知らなければ宝の持ち腐れ。そこで、Pico ボードで実際に様々な機能を試すことができる実験プラットフォーム **pico-jxgLABO** を開発しました。主な特徴は以下の通りです。

- **容易な環境構築**: Pico ボードを USB ケーブルで接続するだけで、すぐに実験を開始できます
- **ロジックアナライザ**: 最大 40MHz でサンプリングができるロジックアナライザ機能を搭載。配線をすることなく、内部信号をキャプチャできます。もちろん、GPIO ピンに接続することで外部信号のキャプチャも可能です
- **シリアル通信**: UART, SPI, I2C の各種シリアル通信プロトコルで、データの送受信が可能です
- **GPIO 制御**: GPIO ピンの制御が可能。入力、出力、PWM 制御などができます。
- **ホスト PC と共有できるファイルシステム**: Pico ボードのフラッシュメモリをファイルシステムにマウントし、ホスト PC と共有することができます
- **PC ライクなファイル操作**: ファイルのコピー、削除、ディレクトリの作成など、PC のようなファイル操作が可能です

**pico-jxgLABO** は単なる実験プラットフォームにとどまりません。任意のプログラムにわずか 4 行のコードを追加するだけで pico-jxgLABO の豊富な機能を組み込むことができます。例えば、ロジックアナライザ機能を使って、Pico ボード上で動作するプログラムのデバッグや、シリアル通信のモニタリングなど、いろいろな用途に利用できます。僕にとってはこちらが主な使用目的です。

## pico-jxgLABO の導入方法

### 必要な機材

pico-jxgLABO を使うのに必要な機材は以下の通りです。

- ホスト PC (Windows, Linux など)
- Pico ボード (Pico, Pico2, Pico W など)
- USB ケーブル

ホスト PC にはシリアル通信のためのターミナルソフトが必要です。以下の説明では、ホストとして Windows PC を使用し、ターミナルソフトには [Tera Term](https://teratermproject.github.io/) を使います。

### Pico ボードへの書き込み

1. 以下のいずれかの UF2 ファイルをダウンロードします。
   Pico ボード: [pico-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/download/0.1.0/pico-jxgLABO.uf2)
   Pico2 ボード: [pico2-jxgLABO.uf2](https://github.com/ypsitau/pico-jxgLABO/releases/download/0.1.0/pico2-jxgLABO.uf2)
2. Pico ボードの BOOTSEL ボタンを押しながら USB ケーブルを接続すると、Pico ボードが USB マスストレージとして認識されます。多くの場合 `D:` ドライブとして認識されます
3. ダウンロードした UF2 ファイルを上記のドライブにコピーします。コピーが完了すると、Pico ボードは自動的に再起動され、pico-jxgLABO が利用できるようになります

## pico-jxgLABO の使い方

### 環境設定と動作確認

Pico ボードを接続すると、ホスト PC にはマスストレージデバイスとして認識されます。ドライブ名は UF2 をコピーしたときと同じになると思います。

シリアル通信をするため Tera Term を起動します。`[設定 (S)]-[シリアルポート (E)...]` を選択し、接続する Pico ボードのシリアルポートを選択します。

![teraterm-setting.png](/images/2025-07-23-labo-logicanalyzer/teraterm-setting.png)

pico-jxgLABO は 2 つのシリアルポートを提供します。一つはターミナル用、もう一つはプロットアプリ用です。追加された最初のシリアルポート (上の例だと `COM21`) がターミナル用になります。選択して `[新規オープン (N)]` をクリックします。

ターミナル上で `[Return]` を押すと、以下のようなプロンプトが表示されます。

```text
L:/>
```

このプロンプトを使って pico-jxgLABO の操作をします。プロンプトの入力内容は、カーソルキーやデリートキーなどで編集ができます。`TAB` キーでコマンド名やファイル名の補完ができます。また、上下矢印キーでコマンド履歴を参照できます。

まずは、`help` コマンドを入力して、使用可能なコマンドの一覧を表示してみましょう。

```text
L:/>help
.               executes the given script file
about-me        prints information about this own program
about-platform  prints information about the platform
adc             controls ADC (Analog-to-Digital Converter)
adc0            controls ADC (Analog-to-Digital Converter)
adc1            controls ADC (Analog-to-Digital Converter)
                        :
                        :
```

PC でおなじみのファイル操作コマンドが利用可能です。

```text
L:/>ls
-a--- 2000-01-01 00:00:00     77 README.txt
L:/>cat README.txt
Welcome to pico-jxgLABO!
Type 'help' in the shell to see available commands.
L:/>
```

ここで見られるファイル情報は、ホスト PC のマスストレージとして認識されているドライブと同じです。Pico ボード上で作成したファイルは、ホスト PC から参照できますし、ホスト PC から Pico ボード上にファイルをコピーすることもできます。

### ロジックアナライザの使い方

```text
L:/>la -p 2,3 enable
enabled pio:2 12.5MHz (samplers:1) target:internal pins:2,3 events:1/88674 (heap-ratio:0.7)
```

```text
L:/>i2c1 -p 2,3 scan
Bus Scan on I2C1
   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
```

```text
L:/>la
enabled pio:2 12.5MHz (samplers:1) target:internal pins:2,3 events:3461/88674 (heap-ratio:0.7)
```

```text
L:/>la print
 Time [usec] P2  P3  
             │   │  
             :   :  
        0.00 └─┐ │  
        1.20   │ └─┐
      575.04 ┌─┘   │
      579.52 │   ┌─┘
      585.84 │   └─┐
      590.24 │   ┌─┘
      596.64 │   └─┐
      601.04 │   ┌─┘
      607.36 │   └─┐
      611.76 │   ┌─┘
      618.16 │   └─┐
      622.56 │   ┌─┘
      628.88 │   └─┐
      633.28 │   ┌─┘
      639.68 │   └─┐
      644.08 │   ┌─┘
      650.40 │   └─┐
      654.88 │   ┌─┘
      655.52 └─┐ │  
      661.20   │ └─┐
      665.60   │ ┌─┘
      671.92   │ └─┐
      676.32   │ ┌─┘
      676.64 ┌─┘ │  
      682.72 │   └─┐
      687.84 └─┐   │
         :
         :
```


1 行あたりの時間間隔はデフォルトで 1000usec (1msec) ですが、上の例だと 579.52 と 585.84 の間隔が 6.32usec であることから、それよりも短い時間間隔を指定しないとタイムスケールが合わないことがわかります。そこで、オプション `--reso` を使って、1 行あたりの時間間隔を 4usec に設定して波形表示をします。

```text
L:/>la print --reso:4
 Time [usec] P2  P3  
             │   │  
             :   :  
        0.00 └─┐ │  
        1.20   │ └─┐
               :   :
      575.04 ┌─┘   │
             │     │
      579.52 │   ┌─┘
             │   │  
      585.84 │   └─┐
             │     │
      590.24 │   ┌─┘
             │   │  
      596.64 │   └─┐
             │     │
      601.04 │   ┌─┘
             │   │  
      607.36 │   └─┐
             │     │
      611.76 │   ┌─┘
             │   │  
      618.16 │   └─┐
             │     │
      622.56 │   ┌─┘
             │   │  
      628.88 │   └─┐
             │     │
      633.28 │   ┌─┘
             │   │  
      639.68 │   └─┐
             │     │
      644.08 │   ┌─┘
             │   │  
      650.40 │   └─┐
             │     │
      654.88 │   ┌─┘
      655.52 └─┐ │  
               │ │  
      661.20   │ └─┐
               │   │
      665.60   │ ┌─┘
               │ │  
      671.92   │ └─┐
               │   │
      676.32   │ ┌─┘
      676.64 ┌─┘ │  
             │   │  
      682.72 │   └─┐
             │     │
      687.84 └─┐   │
               │   │
         :
         :
```

![la-i2c.png](/images/2025-07-23-labo-logicanalyzer/la-i2c.png)
