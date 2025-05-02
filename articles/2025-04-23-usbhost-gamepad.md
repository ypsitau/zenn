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

ただ、多くのメーカがいろいろな仕様で製品を出してきたため、規格としてはかなり混沌としています。どの製品を選べばよいか指標にもなると思うので、以下にゲームパッドの仕様について簡単にまとめます。

### DirectInput と XInput

USB ゲームパッドのインターフェースの規格には **DirectInput** と **XInput** の二つが存在します。**pico-jxglib** は **DirectInput** をサポートします。

**DirectInput** は古くからある規格で、USB の HID クラスとして実装されています。PC などに接続すると、HID ドライバによってゲームパッドまたはジョイスティックデバイスとして認識されます。HID クラスは、ボタン・アナログジョイスティック・キーボード・タッチパネル・ポインティングデバイスなど、様々なヒューマンインターフェースに対応するため、これらのデータのフォーマットをデバイス側で定義することができる Report Descriptor という仕様を定めています。これによってボタンなどの配置や数を自由にレイアウトしたデバイスを作れるのですが、柔軟性の豊富さは無秩序につながります。製品によってレポートデータ中のビット配置が異なるのはまだよいとして、Report Descriptor 中でボタンなどの用途を定めた Usage 値の解釈も、メーカごと、ときによっては同じメーカでも製品ごとに異なります。PC 用のマルチメディアライブラリ SDL はゲームパッドの API も提供しますが、このデバイスごとの差異を以下のようなデーターベースで個別に吸収しています。

https://github.com/mdqinc/SDL_GameControllerDB

**XInput** は Microsoft がゲームコンソール XBox を開発する際に提唱したゲームパッド専用のインターフェースです。DirectInput の仕様があまりにも無秩序になったのを嫌ったのか、ボタンやジョイスティックの配置や数を完全に規定しています。ただ、HID クラスではなく、Vendor ID が `0x045E` (Microsoft)、Product ID が `0x028E` (XBox360 Controller) などのベンダー固有クラスとして扱われます。ベンダー固有クラスですから、サードパーティが XInput に対応する際には、INF ファイルなどを使って自社の Vendor ID および Product ID を OS にインストールする必要があります。ゲームパッドによっては、Microsoft の Vendor ID や Product ID を自社の製品に勝手につけてしまっているものも存在します。インストール作業作業ないで接続できる簡便さを得ようとしたのでしょうが、デバイス詐称ともいえますので、問題がありそうです。

サードベンダが提供しているゲームパッドには、DirectInput と XInput をスイッチで切り替えられるようになっているものが多く存在します。ただし XBox コントローラと銘打って発売しているゲームパッドは、当然 XInput のみサポートします。

### Pico ボードに接続できる USB ゲームパッド

市販されている全ての USB ゲームパッドの中で、Pico ボードに接続できるものはかなり限定されます。異なるベンダから 3 個のゲームパッドを入手しましたが、接続できたのはそのうちの一つでした。問題点は以下の通りです。

- **ハードウェアの問題** EasySMX 社のワイヤレスタイプを試したところ、USB 端子にレシーバを接続しても認識がされませんでした。接続した状態でボードをリセットすると認識するのですが、USB ハブを経由するとやはり不安定です。接続に成功した後は、レポートの受信が正常に行えました。

- **ソフトウェアの問題** Logicool 社の有線タイプ F310 はかなりメジャーな製品ですが、今のところ使えていません。ハードウェアの接続性に問題はなく、Descriptor の取得までは成功するのですが、レポートの受信ができません。何かのきっかけで受信ができたこともあったので、tinyusb 内でメモリ破壊が起こっているのではないかと思うのですが、原因は不明です。

そのような訳で、手元にある中では **Elecom JC-U4113S** が唯一使えるゲームパッドとなりました。

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

## 



## ゲームを楽しむ

せっかくゲームパッドを接続したのですから、本来の目的であるゲームを楽しみたいものです。[KenKen さん](http://www.ze.em-net.ne.jp/~kenken/index.html) の [Raspberry Pi Pico による液晶ゲーム製作](http://www.ze.em-net.ne.jp/~kenken/picogames/index.html) に Pico ボードで動作するクオリティの高いゲームを見つけましたので、ゲームパッドをこれらのゲームに組み込んでみます。

**pico-jxglib** は C++ で書かれていますが、今回のプロジェクトのように C がメインになっているものにも組み込む。

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
