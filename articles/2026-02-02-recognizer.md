---
title: "[Pico で機械学習] タッチパネルの手書き文字を認識する"
emoji: "🤖"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["組み込み", "pico", "raspberrypipico", "機械学習", "文字識別"]
published: false
---
Raspberry Pi Pico と ILI9341 ディスプレイを使い、EMNIST データセットに基づく文字認識を行うデモです。[pico-jxglib](https://github.com/ypsitau/pico-jxglib) ライブラリを利用し、TensorFlow Lite for Microcontrollers を用いています。

GitHub リポジトリ: https://github.com/ypsitau/pico-Recognizer-ILI9341.git

## デモ動画

YouTube でデモ動画を視聴できます:

[![デモ動画](https://img.youtube.com/vi/p4s3oFeSuCg/0.jpg)](https://youtu.be/p4s3oFeSuCg)

## 必要なもの

本プロジェクトをビルドするには以下の部品が必要です:

- Raspberry Pi Pico または Pico W, Pico 2, Pico 2 W
- ILI9341 ディスプレイモジュール
- ブレッドボードとジャンパワイヤ
- Micro USB ケーブル

## 配線

Raspberry Pi Pico と ILI9341 ディスプレイの接続は以下の通りです:

| Raspberry Pi Pico | ILI9341 Display |
|-------------------|-----------------|
| GPIO2 (SPI0 SCK)  | T_CLK           |
| GPIO3 (SPI0 TX)   | T_DIN           |
| GPIO4 (SPI0 RX)   | T_DO            |
| GPIO8             | T_CS            |
| GPIO9             | T_IRQ           |
| GPIO10            | RESET           |
| GPIO11            | DC              |
| GPIO12            | CS              |
| GPIO13            | LED             |
| GPIO14 (SPI1 SCK) | SCK             |
| GPIO15 (SPI1 TX)  | SDI (MOSI)      |

![circuit.png](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2026-02-02-recognizer/circuit.png)

GPIO0 および GPIO1 は認識する TFLite モデルの選択に使います。各モデルは EMNIST データセットの異なるサブセットで学習されています。GPIO0, GPIO1 を GND へ接続または未接続 (NC) にすることで、下記のようにモデルを選択できます:

|GPIO1|GPIO0|学習データセット |認識可能な文字|
|-----|-----|-----------------|-----------------------|
| NC  | NC  | EMNIST Balanced |0-9, A-Z, a, b, d, e, f, g, h, n, q, r, t |
| NC  | GND | EMNIST MNIST    |0-9                    |
| GND | NC  | EMNIST Letters  |A-Z                    |
| GND |GND  | EMNIST ByMerge  |0-9, A-Z, a, b, d, e, f, g, h, n, q, r, t |

## インストール

ビルド済みバイナリを Pico ボードに書き込むには、以下の手順で行います:

1. Pico ボードの BOOTSEL ボタンを押しながら USB で PC に接続し、接続後にボタンを離します。
2. Pico ボードがリムーバブルドライブ（例:D:）として認識されるので、下記のいずれかの UF2 ファイルをコピーします:

   |Pico Board|UF2 ファイル|
   |-----------|------------|
   |Pico |[pico-Recognizer-ILI9341.uf2](https://github.com/ypsitau/pico-Recognizer-ILI9341/releases/latest/download/pico-Recognizer-ILI9341.uf2)            |
   |Pico 2 |[pico2-Recognizer-ILI9341.uf2](https://github.com/ypsitau/pico-Recognizer-ILI9341/releases/latest/download/pico2-Recognizer-ILI9341.uf2)  |

## ビルド方法

ソースコードからビルドする場合は、以下の手順で行います:

1. ターミナルで次のコマンドを実行します:

   ```text
   git clone https://github.com/ypsitau/pico-Recognizer-ILI9341.git
   cd pico-Recognizer-ILI9341
   git submodule update --init --recursive
   ```

2. プロジェクトフォルダで `code .` を実行して Visual Studio Code を開き、`F7` キーでビルドします。`build` フォルダ内に `pico-Recognizer-ILI9341.uf2` が生成されます。
