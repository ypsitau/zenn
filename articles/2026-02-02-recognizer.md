---
title: "[Pico で機械学習] タッチパネルの手書き文字を認識する"
emoji: "🤖"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["組み込み", "pico", "raspberrypipico", "機械学習", "文字識別"]
published: false
---
Raspberry Pi Pico と ILI9341 ディスプレイを使い、EMNIST データセットに基づく文字認識を行うデモです。[pico-jxglib](https://zenn.dev/ypsitau/articles/2025-01-24-jxglib-intro) ライブラリを利用し、TensorFlow Lite for Microcontrollers を用いています。

このデモは、選択したモデルに応じて、数字（0-9）だけでなく、大文字（A-Z）や一部の小文字（a, b, d, e, f, g, h, n, q, r, t）も認識できます。

GitHub リポジトリ: https://github.com/ypsitau/pico-Recognizer-ILI9341.git

## デモ動画

YouTube でデモ動画を視聴できます:

https://youtu.be/p4s3oFeSuCg
<!-- <iframe width="560" height="315" src="https://www.youtube.com/embed/p4s3oFeSuCg" frameborder="0" allowfullscreen></iframe> -->

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

GPIO0 および GPIO1 は認識する TFLite モデルの選択に使います。各モデルは EMNIST データセットの異なるサブセットで学習されています。GPIO0, GPIO1 を GND へ接続することで、下記のようにモデルを選択できます:

|GPIO1|GPIO0|学習データセット |認識可能な文字|
|-----|-----|-----------------|-----------------------|
| NC  | NC  | EMNIST Balanced |0-9, A-Z, a, b, d, e, f, g, h, n, q, r, t |
| NC  | GND | EMNIST MNIST    |0-9                    |
| GND | NC  | EMNIST Letters  |A-Z                    |
| GND |GND  | EMNIST ByMerge  |0-9, A-Z, a, b, d, e, f, g, h, n, q, r, t |

## インストール

ビルド済みバイナリを Pico ボードに書き込むには、以下の手順で行います:

1. Pico ボードの BOOTSEL ボタンを押しながら USB で PC に接続し、接続後にボタンを離します。
2. Pico ボードがリムーバブルドライブ（`D:` など）として認識されるので、下記のいずれかの UF2 ファイルをコピーします:

   |Pico Board|UF2 ファイル|
   |-----------|------------|
   |Pico / Pico W |[pico-Recognizer-ILI9341.uf2](https://github.com/ypsitau/pico-Recognizer-ILI9341/releases/latest/download/pico-Recognizer-ILI9341.uf2)            |
   |Pico 2 / Pico 2 W |[pico2-Recognizer-ILI9341.uf2](https://github.com/ypsitau/pico-Recognizer-ILI9341/releases/latest/download/pico2-Recognizer-ILI9341.uf2)  |

## ビルド方法

ソースコードからビルドする場合は、以下の手順で行います:

1. ターミナルで次のコマンドを実行します:

   ```text
   git clone https://github.com/ypsitau/pico-Recognizer-ILI9341.git
   cd pico-Recognizer-ILI9341
   git submodule update --init --recursive
   ```

2. プロジェクトフォルダで `code .` を実行して Visual Studio Code を開き、`F7` キーでビルドします。`build` フォルダ内に `pico-Recognizer-ILI9341.uf2` が生成されます。

## 追記

この記事は Zenn と Qiita にクロスポストしています。
