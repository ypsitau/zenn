---
title: "pico-jxgLABO × PulseView: "
emoji: "✨"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["raspberrypi", "pico", "raspberrypipico", "ロジックアナライザ", "PulseView"]
published: false
---
RaspberryPi Pico の実験プラットフォーム [pico-jxgLABO](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro) が [PulseView](https://sigrok.org/wiki/PulseView) に対応しました!

Pico ボードの内部信号を、配線なしでロジックアナライザで観測できるようになります。これにより、より簡単に信号の解析やデバッグが行えるようになります。

もちろん、通常のロジックアナライザのように外部信号の観測も可能です。この記事では、pico-jxgLABO と PulseView の連携方法について説明します。

## pico-jxgLABO の導入方法

pico-jxgLABO の導入方法は[こちら](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro#pico-jxglabo-%E3%81%AE%E5%B0%8E%E5%85%A5%E6%96%B9%E6%B3%95)。特別なハードウェアは必要なく、Pico や Pico 2 ボードを USB ケーブルで PC に接続するだけで始められます。

pico-jxgLABO は v0.2.0 以降で PulseView に対応しています。ターミナルソフトから以下のように `about-me` コマンドを実行してバージョンを確認してください。

```shell

```text
L:/>about-me
Program Information
 name:              pico-jxgLABO
 version:           0.2.0
     :
     :
```

## PulseView のインストールと設定

PulseView は、[Sigrok](https://sigrok.org/) プロジェクトの一部であり、ロジックアナライザの信号を可視化するためのツールです。I2C や SPI をはじめとする様々なプロトコルのデコードもサポートしているので、これらを活用することで pico-jxgLABO でキャプチャした信号を簡単に解析できます。

PulseView は以下のサイトで入手できます。

▶️ [sigrok Downloads](https://sigrok.org/wiki/Downloads)

Nightly builds の「PulseView (32bit)」または「PulseView (64bit)」を選択してダウンロード・インストールしてください。


