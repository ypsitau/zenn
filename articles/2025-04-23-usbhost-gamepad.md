---
title: "pico-jxglib で Pico ボードに USB ゲームパッドを接続してゲームを楽しむ話"
emoji: "📚"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["組み込み", "raspberrypi", "pico", "raspberrypipico", "usb"]
published: false
---
**pico-jxglib** は、ワンボードマイコン Raspberry Pi Pico の Pico SDK プログラミングをサポートするライブラリです。

https://zenn.dev/ypsitau/articles/2025-01-24-jxglib-intro

今回は Pico の USB ホスト機能を使って、Pico ボードに USB ゲームパッドを接続します。

## USB ゲームパッドについて

ゲームパッドはその名の通り、本来ゲームを楽しむためのデバイスです。一般的なゲームパッドで、アナログジョイスティックが左右で 2 個、十字キー入力、10 個以上のボタンという豊富な入力手段を備えており、それらが左右の手で効率よく操作できるような筐体に収められています。改めて簡易なマニピュレータとして見てみると、これが数千円で入手できるのは非常に魅力的です。

ところで、USB ゲームパッドのインターフェースの規格には DirectInput と XInput の二つが存在します。

DirectInput は USB の HID クラスとして認識されます。PC などに接続すると、ドライバ

https://github.com/mdqinc/SDL_GameControllerDB

XInput は Microsoft がゲームコンソール XBox を開発する際に提唱したゲームパッド専用のインターフェースです。
ベンダー ID 045E、プロダクト ID 028E
サードパーティが XInput に対応する際には、自社のベンダー ID およびプロダクト ID をホスト PC に登録する必要があります。

ゲームパッドによっては、これらのベンダー ID、プロダクト ID を自社の製品につけてしまっているものも存在します。

Report Descriptor パーサ


## 準備するもの

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

## 

## ゲームを楽しむ

ゲームを作りたくなるというのはプログラマの本能ですね。
一から作るのは大変な労力がかかりますので

[KenKen さん](http://www.ze.em-net.ne.jp/~kenken/index.html)
[Raspberry Pi Pico による液晶ゲーム製作](http://www.ze.em-net.ne.jp/~kenken/picogames/index.html)

ゲームパッドをこれらのゲームに組み込んでみます。

**pico-jxglib** は C++ 
今回のプロジェクトのように C がメインになっているものに組み込むためのガイドラインにもなっています。

ILI9341
同じく 240x320 の画面サイズを持つ ST7789 にも対応させました。

パックマン

https://github.com/ypsitau/picopacman

picopacman-pico-ili9341.uf2
picopacman-pico-st7789.uf2
picopacman-pico2-ili9341.uf2
picopacman-pico2-st7789.uf2

テトリス

https://github.com/ypsitau/picotetris

インベーダーゲーム

https://github.com/ypsitau/picoinvader


箱入り娘パズル

https://github.com/ypsitau/picohakomusu

ペグソリティア

https://github.com/ypsitau/picopegsol
