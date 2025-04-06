---
title: "Pico SDK と pico-jxglib の話"
emoji: "🕌"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["RaspberryPiPico", "PicoSDK", "Cpp"]
published: false
---
**pico-jxglib** は、ワンボードマイコン Raspberry Pi Pico の Pico SDK プログラミングをサポートするライブラリです。

今回の記事は、**pico-jxglib** の概要と導入方法について説明します。ライブラリの詳細については以下の記事を参照ください。

:arrow_forward: [pico-jxglib と TFT LCD の話](https://qiita.com/ypsitau/items/300472e2dee582361303)
:arrow_forward: [pico-jxblib と TFT LCD の話 (続き)](https://qiita.com/ypsitau/items/6117e654e1f303e770f6)
:arrow_forward: [pico-jxglib と LVGL の話](https://qiita.com/ypsitau/items/be620ca50c23b115e00a)
:arrow_forward: [pico-jxglib と Terminal の話](https://qiita.com/ypsitau/items/bbccd9f9b5fc5d48d2f1)
:arrow_forward: [pico-jxglib で Pico ボードに USB キーボード・マウスを接続する話](https://qiita.com/ypsitau/items/14e305f4e0fefcdc1b33)

## 作成のきっかけ

ロボットを作りたいと思っているのです。それほど大それたものではなくて、カメラやセンサを搭載して、モータで部屋の中をちょこちょこ動き回る「何か」です。

僕はソフト屋なので、電子回路のことはなんとか分かってもメカのことはまったくの素人です。なにはともあれ道具は必要なので、3D プリンタと糸のこ盤を買い込みました。

最初に、リンク機構の仕組みを知りたくてロボットアームを作りました。

![RobotArm.jpg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/48975/5e84bb60-4cc1-3022-4958-b2c8386f6dec.jpeg)


それぞれの関節やローテーションテーブルにサーボモータを仕込んであります。コントロールボックスにはボリュームつまみがついていて、この操作でサーボモータに PWM 信号を送ります。ちなみに、ロボットアームやコントロールボックス、ボリュームつまみもすべて 3D プリンタで作ったものです。エレキ関連以外の既製品はタミヤのユニバーサルプレートと M3x5 のネジくらい。3D プリンタってすごいなあ。

次に作ったのがこれ。Wi-Fi でコマンドを受信して動くラジコンカーです。

![RobotCar.jpg](https://qiita-image-store.s3.ap-northeast-1.amazonaws.com/0/48975/bd176555-bc07-8a20-90aa-a35b09493522.jpeg)

ステッピングモータで動いてます。Wi-Fi の接続状況や IP アドレスなどの表示のために OLED をつけてます。

動くものってやっぱり楽しい！今まで携わってきたソフトウェアや電子回路に、メカが加わったことでぐんと世界が広がった気がします。これからカメラやいろいろなセンサをつけて、キャプチャ画像をモニタするためのカラー LCD も接続して、画像やセンサ情報を PC にも送れるようにして ... といろいろ考えていて、ここではたと思いました。

Pico SDK の API をそのまま使っても、PWM 制御やステッピングモータ、また OLED の表示くらいまでならばプログラムコードもさして複雑ではありません。でもこれから先、いろいろな機能を追加するためにソフトウェアを拡張していくと混沌として管理がしづらくなるし、なによりもプログラミングが楽しくありません。開発を効率化するために、ある程度まとまった機能を API として提供するミドルウェア的なものが欲しくなってきました。

**pico-jxglib** は、そのような過程で作られたライブラリです。ですので、このライブラリに実装していく機能は僕が今現在進めているロボット開発に関連したものになります。でも、ロボットを構成する要素はとても広いので、結局 Pico でできることをすべてカバーしていくことになるのかもしれません。

あくまでも個人的なプロジェクトなのですが、ライブラリの機能の一部、特に TFT LCD まわりは興味を持つ人がいるんじゃないかと思います。なにかのお役にたてればいいなと思い、公開することにしました。

## CMake について

Pico SDK プロジェクトに **pico-jxglib** を組み込むには CMake の設定ファイルである `CMakeLists.txt` を編集します。ほんの数行追加するだけですから、編集内容だけ知れば詳細を知らなくても組み込みはできるのですが、実際に CMake が何をしているのか分からないとモヤモヤしたものが残ると思います。ここでは CMake のごく基本的な事柄を解説します[^cmake-note]。

[^cmake-note]: CMake の仕様はとても大きくて、僕自身もここに書いている以上のことはよく知らないです

CMake は `CMakeLists.txt` の内容をもとにして make や ninja といったビルドツールのファイルを生成します。Makefile を直接書けばいいんじゃない？思われるかもしれませんが、使用するコンパイラや OS の違いを CMake ツールがいろいろ賢く解決してくれるのが非常に便利で、多くの開発プラットフォームが採用しています。

Pico SDK がひな形として生成する `CMakeLists.txt` から、ビルドの骨組みになる部分を以下に抜き出しました。

```cmake:CMakeLists.txt
add_executable(hoge hoge.c)

target_link_libraries(hoge
    pico_stdlib)
    
target_include_directories(hoge PRIVATE
    ${CMAKE_CURRENT_LIST_DIR})
```

処理内容は以下の通りです。

- `add_executable()` は実行ファイルの生成を宣言するもので、引数としてターゲットの識別子と、それに続けてソースファイルを記述します。ソースファイルが複数ある場合は空白または改行で区切ります。ターゲットの識別子は実行ファイルのベース名として使われる (この例だと `hoge.elf` や `hoge.uf2` を生成する) ほか、`target_` で始まるコマンドの最初の引数として指定します。
なお、ターゲットを宣言するコマンドには他に `add_library()` があり、これはライブラリの生成を宣言します。
- `target_link_libraries()` は、このターゲットにリンクするライブラリを指定します
- `target_include_directories()` はソースファイルをコンパイルする時のインクルードパスを指定します。`PRIVATE` は、ここで指定したインクルードパスがこの `CMakeLists.txt` 内だけで有効であることを示します。ほかに `PUBLIC`、`INTERFACE` という宣言子があり、いずれかひとつをここに指定します

変数 `CMAKE_CURRENT_LIST_DIR` には、この `CMakeLists.txt` が存在しているディレクトリ名が代入されます。

`target_` で始まるコマンドは、以下のように一つのコマンド呼び出しにまとめて書いても:

```
target_link_libraries(hoge
    pico_stdlib
    hardware_i2c)
```

以下のように分割して書いても OK です。

```
target_link_libraries(hoge
    pico_stdlib)

target_link_libraries(hoge
    hardware_i2c)
```

- - -
ここまでの内容は「ちょっと記述方法が変わった Makefile」という認識でとらえられるかもしれません。でも、`target_link_libraries()` の働きについてはちょっと説明が必要です。

この `CMakeLists.txt` の `target_link_libraries()` の内容を見て、あなたは好奇心をおこして `pico_stdlib.lib` や `pico_stdlib.a`、または `pico_stdlib.so` などががどこにあるのか `.pico_sdk` ディレクトリや現在のプロジェクトの `build` ディレクトリの中を探すかもしれません。ですが、そのようなファイルはどこにも見つかりません。ここで指定されている `pico_stdlib` はファイル名ではなく、Pico SDK のディレクトリ内にある `CMakeLists.txt` の `add_library()` コマンドで宣言されたターゲットの識別子だからです[^lib-not-found]。

[^lib-not-found]: 指定したターゲット識別子が見つからない場合は、`*.lib` や　`*.a` という実在のライブラリファイルをライブラリサーチパスから探索します。

以下に、`pico_stdlib` を宣言している `CMakeLists.txt` を示します (分かりやすくするため少々書き変えています)。

```cmake:pico-sdk/src/host/pico_stdlib/CMakeLists.txt
add_library(pico_stdlib INTERFACE)

target_sources(pico_stdlib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/stdlib.c
)

target_link_libraries(pico_stdlib INTERFACE
    pico_stdlib_headers
    pico_platform
    pico_time
    pico_divider
    pico_binary_info
    pico_printf
    pico_runtime
    pico_stdio
    hardware_gpio
    hardware_uart
)
```

`INTERFACE` という宣言子が重要です。この宣言子がついたターゲットは、実際のコンパイルはせず、`target_link_libraries()` の実行元に対してソースファイルやインクルードパス、リンクするライブラリなどの情報を伝達します。

上記のファイル中、`pico_stdlib` はさらに `pico_stdlib_headers` や `pico_platform` などのライブラリをリンクしていますが、これらのライブラリもすべて `INTERFACE` 宣言されています。なので、これらのファイルで提供されるソースファイルなどの情報もすべて元の `target_link_libraries()` の実行元に伝達されることになります[^about-interface]。

[^about-interface]: このことは　[Raspberry Pi Pico-series C/C++ SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf) のセクション「2.2. Every Library is an INTERFACE Library」に詳しく記述されているのですが、最初は読んでもさっぱり頭に入ってきませんでした。「ライブラリ」というのが `hoge.lib` だとか `hoge.a` という「ライブラリファイル」であるという固定概念が邪魔をしていたからだと思います。

`INTERFACE` 宣言されたライブラリは、すでにビルドされたライブラリファイルがあるわけではないので、`target_link_libraries()` を実行した大本のプロジェクトでそれらを構成するソースファイルのコンパイルを行います。

この方法には大きなメリットがあります。ライブラリのソースファイルからコンパイルしますので、利用者はプリプロセッサのマクロ定義などを使って必要なコードだけをコンパイルすることができますし、また処理内容をコンパイル時に切り替えることができます。これによって実行パフォーマンスを高め、オブジェクトサイズも小さくすることができます。

また、インクルードパスの情報が `target_link_libraries()` を実行するだけで呼び出し元に伝達されるという点も重要です。使用するライブラリのインクルードファイルの場所をいちいち調べて `target_include_directories()` を実行するという煩わしさから完全に開放されます。

**pico-jxglib** は Pico SDK にならってライブラリを `INTERFACE` 宣言しています。

## pico-jxglib の組込み方

**pico-jxglib** は GitHub から以下のようにクローンすることで最新版を入手できます。

```
git clone https://github.com/ypsitau/pico-jxglib.git
cd pico-jxglib
git submodule update --init
```

このディレクトリを `add_subdirectory()` コマンドでプロジェクトに組み込むのですが、ディレクトリの位置によって指定の仕方が少々異なります。

- 以下のように `pico-jxglib` ディレクトリをプロジェクトディレクトリ配下に置いた場合は:

  ```
  +-[your-project]
    +-[pico-jxglib]
    +-CMakeLists.txt
    +-your-project.cpp
    +- ...
  ```

  `CMakeLists.txt` に以下のコマンドを追加します。

  ```cmake:CMakeLists.txt
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-jxglib)
  ```

- 以下のように `pico-jxglib` ディレクトリをプロジェクトの `CMakeLists.txt` よりも上のディレクトリに置いた場合は:

  ```
  +-[pico-jxglib]
  +-[your-project]
    +-CMakeLists.txt
    +-your-project.cpp
    +- ...
  ```

  `CMakeLists.txt` に以下のコマンドを追加します。

  ```cmake:CMakeLists.txt
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
  ```

  `add_subdirectory()` の第二引数として `pico-jxglib` という文字列を渡していることに注意してください[^second-arg]。これは追加するディレクトリに `..` が含まれる場合に必要です。

[^second-arg]: ここで指定した文字列は、生成ファイルを出力するディレクトリ名として使われます。`build` ディレクトリ中、ほかのディレクトリと重複しなければ何でもかまいません。

## 実際のプロジェクト

実際に **pico-jxglib** を使って L チカプログラムを作ってみます。

VSCode のコマンドパレットから `>Raspberry Pi Pico: New Pico Project` を実行し、以下の内容でプロジェクトを作成します。

- **Name** ... プロジェクト名を入力します。今回は例として `blink` を入力します
- **Board type** ... ボード種別を選択します
- **Location** ... プロジェクトディレクトリを作る一つ上のディレクトリを選択します
- **Code generation options** ... **`Generate C++ code` にチェックをつけます**

このプロジェクトディレクトリと `pico-jxglib` のディレクトリ配置が以下のようになっていると想定します。

```
+-[pico-jxglib]
+-[blink]
  +-CMakeLists.txt
  +-blink.cpp
  +- ...
```

`CMakeLists.txt` の最後に以下の行を追加してください。

```cmake:CMakeLists.txt
target_link_libraries(blink jxglib_Common)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

`jxglib_Common` が、今回追加するライブラリです。`target_link_libraries()` と `add_subdirectory()` の実行順序はどちらが先でもかまいません。また `target_link_libraries()` の内容は、すでにある同名のコマンドにつけたすことができます。

ソースファイルを以下のように編集します。

```c++:blink.cpp
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
    GPIO15.init().set_dir_OUT();
    while (true) {
        GPIO15.put(true);
        ::sleep_ms(500);
        GPIO15.put(false);
        ::sleep_ms(500);
    }
}
```

`using namespace jxglib;` は **pico-jxglib** の関数やクラスをこのコード内で使うための「おまじない」です[^namespace]。

[^namespace]: **pico-jxglib** の関数やクラス、グローバル変数は、名前がほかのライブラリとバッティングしないよう `jxglib` という識別子をもつ名前空間の中で定義されています。例えばこのコード内の `GPIO15` は、名前空間の識別子を先頭につけて `jxglib::GPIO15` と記述する必要があるのですが、記述が長くなるので `using namespace` を使って省略できるようにしています。

ビルド方法とボードへの書き込み方は [前回の記事](https://qiita.com/ypsitau/items/afd020cad554910a6a9e#%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E3%83%93%E3%83%AB%E3%83%89) を参照してください。

## 次回の記事

**pico-jxglib** を使って TFT LCD の描画をします。

https://qiita.com/ypsitau/items/300472e2dee582361303
