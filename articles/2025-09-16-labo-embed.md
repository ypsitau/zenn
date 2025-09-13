---
title: "その持てる力をすべて解き放て! Pico ボードプログラムに pico-jxgLABO を組み込む方法"
emoji: "⛳"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["zennfes2025free", "raspberrypi", "pico", "raspberrypipico", "library"]
published: true
published_at: 2025-09-16 09:00
---
[pico-jxgLABO](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro) は、USB ケーブル一本でマイコンボード RaspberryPi Pico の様々な機能を試すことができる実験プラットフォームです。

この記事では、pico-jxgLABO を既存のプログラムに組み込んだり、このプラットフォームをベースに新しい機能を追加したりする方法を紹介します。単なる実験プラットフォームにとどまらない、pico-jxgLABO の真の能力を引き出しましょう。

:::message
pico-jxgLABO は C/C++ 言語で開発されており、Raspberry Pi 財団が提供する [Pico SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf) をベースにしています。MicroPython や Arduino の開発経験はあるけれども、Pico SDK は初めてという方も多いかと思います。この記事では、Pico SDK 開発環境の準備から、pico-jxgLABO の組み込み方法までを丁寧に解説します。思ったよりもあっさりできますよ!
:::

## 自作プログラムに pico-jxgLABO を組み込むメリット

pico-jxgLABO のバイナリ (UF2 ファイル) を Pico ボードに書き込むと、コマンド操作で [GPIO 制御](https://zenn.dev/ypsit/articles/2025-08-03-labo-gpio)や[ロジックアナライザの機能](https://zenn.dev/ypsit/articles/2025-09-08-labo-la)を利用できるようになります。ちょっとした実験ならばこれでも十分ですが、あらかじめ用意されたコマンドレベルの機能しか使えないため、できることに限界があります。

このプラットフォームの真の力は、pico-jxgLABO のソースコードを自分のプログラムに組み込むことで発揮されます。pico-jxgLABO を組み込むことで、以下のサービスを享受できます。

- **対話システム**: pico-jxgLABO のシェルを使えば、PC のターミナルソフトから Pico ボードにコマンドを送って対話的に操作できます。プログラムの動作状況を確認したり、パラメータを変更したりするのに便利です
- **ロジックアナライザ**: ロジックアナライザ機能を使えば、プログラムが期待通りの信号を GPIO に出力しているか確認できます。また、I2C や SPI などの通信プロトコルの解析も可能です
- **内蔵フラッシュドライブ**: : pico-jxgLABO は Pico ボードの内蔵フラッシュメモリをファイルシステムとして使えるようにしています。これにより、このプラットフォームを組み込んだ
プログラムがデータをファイルに保存したり読み込んだりできます
- **USB ドライブ**: 上の内蔵フラッシュドライブは、PC から USB ドライブとしてアクセスすることができます。これにより、PC と Pico ボード間でデータのやり取りが簡単に行えます
- **USB シリアルポート**: USB シリアルポートを 2 個提供します。ひとつはターミナルソフトからのコマンド入力、もうひとつは他のアプリケーションとの通信に使用できます

組込みに必要なのは、**ビルド設定ファイルに 3 行、任意の C/C++ プログラムに 3 行のコードを追加**するだけです。早速、組み込み方法を見ていきましょう!

なお、開発環境は Windows を想定しています。

## Pico SDK 開発環境チュートリアル

このセクションでは、Pico SDK 環境を持っていない方のために、開発環境の設定と Pico SDK プロジェクトの作成方法を解説します。

## 開発環境の設定

まずは以下のツールをインストールしてください。

- Visual Studio Code: https://code.visualstudio.com/download
- Git ツール: https://git-scm.com/downloads
- ターミナルソフト: ここでは [Tera Term](https://teratermproject.github.io/) を使います

また、プロジェクトを開いたりプログラムを書き込んだりするのに CUI を多用しますので、PowerShell やコマンドプロンプトのショートカットを作っておくと便利です。

Pico SDK (C/C++ のビルドに必要なツールやライブラリ群) は Visual Studio Code (以下 VSCode) の拡張機能でインストールします。VSCode を起動したら `[F1]` キーを押して以下のコマンドパレット (VSCode の機能をコマンドで実行できるテキストボックス) を開いてください。

![vscode-palette.png](/images/2025-01-17-picosdk/vscode-palette.png)

ここで　`>Extensions: Install Extensions` (最初の `>` は初めからコマンドパレットに入力されているもの) を入力して、拡張機能のインストール画面を開きます。検索ボックスに `pico` などと入力すると、一覧の中に以下のような `Raspberry Pi Pico` という拡張機能が見つかるので、`[Install]` ボタンをクリックしてインストールしてください。

![pico-extension.png](/images/2025-01-17-picosdk/pico-extension.png)

これで Pico SDK 開発に必要な設定はすべて完了です。

### Pico SDK プロジェクトの作成

Pico SDK のプロジェクトを作成するには、VSCode で `[F1]` キーを押して表示されるコマンドパレットから `>Raspberry Pi Pico: New Pico Project` を実行します。`New Pico Project` というドロップダウンリストが出て使用言語を聞かれるので `C/C++` を選択すると以下の画面が表示されます。

![new-project.png](/images/2025-01-17-picosdk/new-project.png)

以下の項目を設定してください。ほかはデフォルトのままで結構です。

- **Basic Settings**
  - `Name`: プロジェクト名を入力します。ここでは `P00_Simple` とします
  - `Board type`: 使用しているボードタイプ (`Pico`, `Pico 2`, `Pico W`, `Pico 2 W`, `Other` のいずれか) を選択します
  - `Location`: `[Change]` ボタンをクリックしてプロジェクトディレクトリを作成するひとつ上のディレクトリ名を選択します。ここでは `C:\Users\YOUR-NAME\labo-project` とします
- **Code generation options**
  - `Generate C++ code`: チェックを入れます

右下の `[Create]` ボタンをクリックするとプロジェクトを作成します。初めてプロジェクトを作成した場合は、Pico SDK のダウンロードとインストールを行うのでかなり時間がかかります (20 分ほど)。初めの一回だけなので、気長に待ちましょう。

プロジェクトの作成が完了すると、プロジェクトディレクトリ `C:\Users\YOUR-NAME\labo-project\P00_Simple` が作成され、そのディレクトリをワークスペースにした VSCode が自動的に開きます。

`P00_Simple` ディレクトリの中には以下のようなファイルが作成されています。

- `CMakeLists.txt`: プロジェクトのビルド設定ファイル
- `P00_Simple.cpp`: メインの C++ プログラム

C++ プログラムの内容は以下のようになっていると思います。

```cpp: P00_Simple.cpp
#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
```

ビルドをするには `[F7]` キーを押します。プロジェクトを作成して初めてのビルドですと以下のようなドロップダウンリストが表示されるので、`Pico Using compilers: ...` を選択します。

![vscode-select-kit.png](/images/2025-01-17-picosdk/vscode-select-kit.png)

ビルドが成功すると、`C:\Users\YOUR-NAME\labo-project\P00_Simple\build` ディレクトリが作成され、`P00_Simple.uf2` というバイナリファイルが生成されます。

Pico ボードの BOOTSEL ボタンを押しながら USB ケーブルで PC と接続すると、Pico ボードが USB ドライブとして認識されます。`P00_Simple.uf2` ファイルをこのドライブにコピーすると、Pico ボードが自動的に再起動してプログラムが実行されます。

ところで、`printf()` の `Hello, world!` はいったいどこに出力されるのでしょうか? Pico SDK は `printf()` の出力先を UART や USB シリアルなど柔軟にカスタマイズできるようになっているのですが、この時点ではそれが設定されていないのでまだどこにも出力されないのです。pico-jxgLABO が提供する USB シリアルポートに出力することになっているので、この先は記事の続き「pico-jxgLABO の組み込み方法」に進んでください。

### Pico SDK プロジェクトの開き方

すでに存在する Pico SDK プロジェクトを開くには、コマンドプロンプトでプロジェクトディレクトリに移動して `code .` コマンドを実行します。例えば `C:\Users\YOUR-NAME\labo-project\P00_Simple` ディレクトリを開くには、以下のようにします。

```text
cd C:\Users\YOUR-NAME\labo-project\P00_Simple
code .
```

このとき、Pico SDK の設定情報などを記録した `.vscode` ディレクトリがないと、以下のようなダイアログが表示されます。

![open-workspace.png](/images/2025-01-17-picosdk/open-workspace.png)

`Select a Kit for ...` のドロップダウンリストは無視して、右下の `Do you want to import this project as Raspberry Pi Pico project?` のダイアログで `[Yes]` ボタンをクリックしてください。以下の画面が表示されます。

![import-project.png](/images/2025-01-17-picosdk/import-project.png)

`[Import]` ボタンをクリックすると、`.vscode` ディレクトリが作成され、Pico SDK プロジェクトとして開けるようになります。

## pico-jxgLABO の組み込み方法

pico-jxgLABO を自作プログラムに組み込んでみましょう。この記事では、上の「Pico SDK 開発環境チュートリアル」で作成した `P00_Simple` プロジェクトを例にして組込み方法を紹介します。`P00_Simple` は C++ のプロジェクトですが、C 言語のプロジェクトでも同様の手順で組込みができます。

:::message
既存のプロジェクトに組み込む場合、Stdio で `Console over USB` を有効にしていると、pico-jxgLABO の USB 機能と競合してしまいます。`CMakeLists.txt` の中の `pico_enable_stdio_usb()` 関数呼び出しで引数を `0` にして無効にしてください。

```cmake:CMakeLists.txt 
pico_enable_stdio_usb(PROJECT_NAME 0)
```

:::

pico-jxgLABO は pico-jxglib というライブラリ群の上に構築されており、このライブラリは [GitHub リポジトリ](https://github.com/ypsitau/pico-jxglib) で管理されています。git ツールを使って pico-jxglib のソースコードをダウンロード(クローン)するのですが、その際、組み込むプロジェクトディレクトリとの位置関係を以下のようにします[^directory]。

[^directory]: `pico-jxglib` ディレクトリの位置は `CMakeLists.txt` で指定できるので、記述を変えればほかの位置に置くこともできます。

```text
C:\Users\YOUR-NAME\labo-project
  ├── pico-jxglib/
  └── P00_Simple/
      ├── CMakeLists.txt
      ├── P00_Simple.cpp
      └── ...
```

以下のコマンドを実行してください。

```text
cd C:\Users\YOUR-NAME\labo-project
git clone https://github.com/ypsitau/pico-jxglib.git
cd pico-jxglib
git submodule update --init --recursive
```

プロジェクト `P00_Simple` のディレクトリに移り、VSCode を実行します。

```text
cd C:\Users\YOUR-NAME\labo-project\P00_Simple
code .
```

`CMakeLists.txt` の最後に以下の行を追加してください。他のプロジェクトの場合、`P00_Simple` の部分を該当するプロジェクト名に置き換えます。

```cmake:CMakeLists.txt
target_link_libraries(P00_Simple jxglib_LABOPlatform_FullCmd)          # 1
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib) # 2
jxglib_configure_LABOPlatform(P00_Simple)                              # 3
```

処理内容は以下の通りです。

1. `P00_Simple` ターゲットに pico-jxgLABO のライブラリ `jxglib_LABOPlatform_FullCmd` をリンクします。このライブラリは、pico-jxgLABO のコアになる機能と、現在提供しているすべてのシェルコマンドを含んでいます。`target_link_libraries()` はすでに `CmakeLists.txt` に記述されているものがあるので、このライブラリをそこに追加しても構いません
2. pico-jxgLABO のプロジェクトディレクトリを指定します。`${CMAKE_CURRENT_LIST_DIR}` はこの `CMakeLists.txt` ファイルがあるディレクトリを指します。2 番目の引数 `pico-jxglib` はビルド時に生成される中間ファイルを置くディレクトリ名です
3. pico-jxgLABO で使用している tinyusb や FatFS のコンフィグレーションファイルを生成します

ソースファイル `P00_Simple.cpp` の内容を以下のように変更します。

```cpp: P00_Simple.cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"        // 1

int main()
{
    stdio_init_all();
    jxglib_labo_init(false);            // 2

    while (true) {
        printf("Hello, world!\n");
        jxglib_sleep(1000);             // 3
    }
}
```

変更した内容は以下の通りです。

1. `jxglib/LABOPlatform.h` ヘッダファイルをインクルードします。これで pico-jxgLABO の API を使えるようになります
2. `jxglib_labo_init()` を呼び出して pico-jxgLABO の初期化を行います
3. `sleep_ms()` の代わりに `jxglib_sleep()` を使います。この関数内で pico-jxgLABO のタスクが実行されます

これで組み込みは完了です。`[F7]` キーを押してビルドしてください。ビルドが成功するとプロジェクトディレクトリ下の `build` ディレクトリに `P00_Simple.uf2` ファイルが生成されるので、BOOTSEL ボタンを押しながら USB ケーブルを接続して認識されるドライブにコピーしてください。コピーが完了すると Pico ボードが自動的に再起動してプログラムが実行されます。

シリアル通信をするため Tera Term を起動します。メニューバーから `[設定 (S)]`-`[シリアルポート (E)...]` を選択し、接続する Pico ボードのシリアルポートを選択します。

![teraterm-setting.png](/images/2025-08-01-labo-intro/teraterm-setting.png)

pico-jxgLABO は 2 つのシリアルポートを提供します。一つはターミナル用、もう一つはロジックアナライザやプロッタなどのアプリケーション用です。提供する最初のシリアルポート (上の例だと `COM21`) がターミナル用になります。これを選択して `[新規オープン (N)]` または `[現在の接続を再設定 (N)]` をクリックします。

Tera Term のウィンドウに `Hello, world!` が 1 秒ごとに表示されれば成功です。おめでとうございます!

ところで、pico-jxgLABO はどのように起動すればよいのでしょうか? 実は、もうすでに動いているんですよ! Tera Term で `[Enter]` キーを押してみてください。`Hello, world!` の表示に交じって以下のようなプロンプトが現れるはずです。

```text
L:/>
```

この状態で、[この記事](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro)などで紹介した機能がすべて使えるようになっています。pico-jxgLABO は**バックグラウンドで動作**するので、プログラムの本来の動作を止めることなく、様々なサービスを提供します。

`dir` コマンドを入力してみましょう。Pico ボードの内蔵フラッシュメモリに保存されているファイルの一覧が表示されます。`Hello, world!` がちょっとうるさいですが...。

```text
L:/>dir
-a--- 2000-01-01 00:00:00     77 README.txt
```

pico-jxgLABO は USB ドライブの機能も提供していて、ホスト PC から内蔵フラッシュメモリにアクセスできます。多くの場合 `D:` ドライブとして認識されると思いますが、適当なファイルを PC からこのドライブにコピーした後で Tera Term で `dir` コマンドを実行してみてください (ここでは `P00_Simple.cpp` をコピー)。

```text
L:/>dir
-a--- 2000-01-01 00:00:00     77 README.txt
-a--- 2025-09-10 15:09:54    245 P00_Simple.cpp
```

`cat` コマンドを使って Pico ボード上でファイルの中身を確認することもできます。

```text
L:/>cat P00_Simple.cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

int main()
{
    stdio_init_all();
    jxglib_labo_init(false);
    while (true) {
        printf("Hello, world!\n");
        jxglib_sleep(1000);
    }
}
```

遅延が必要なければ、`jxglib_sleep()` の代わりに `jxglib_tick()` をメインループの中で実行します。

```cpp: P00_Simple.cpp
#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

int main()
{
    stdio_init_all();
    jxglib_labo_init(false);

    while (true) {
        //
        // any jobs
        //
        jxglib_tick();
    }
}
```

この記事では VSCode がひな形として生成した Pico SDK プログラムに pico-jxgLABO を組み込む方法を紹介しましたが、既存のプログラムに組み込む場合も同様です。pico-jxgLABO を活動状態にするのに必要な手順は次の 2 つだけです。

- `jxglib_labo_init()` を一度だけ呼び出す
- メインループの中で `jxglib_sleep()` か `jxglib_tick()` のどちらかを定期的に実行する

## プログラムに組み込んだ pico-jxgLABO の活用方法

pico-jxgLABO が提供する API をプログラムの中で使うと多様なサービスが受けられるのですが、それはまた別の記事で紹介します。今のこの段階でも、以下のような活用方法が考えられます。

- **ロジックアナライザ**: `la` コマンドでロジックアナライザを作動させれば、プログラムが生成する信号のタイミングや波形の形状を確認して、期待通りの動作をしているかチェックできます。また、GPIO を通じて入ってくる外部からの信号を観測したり解析することができます。例えば、赤外線 LED を GPIO に接続すれば、赤外線リモコンの信号をロジックアナライザで解析できるので、その結果をプログラムに反映しつつ開発を進められます
- **GPIO 制御**: `gpio` コマンドを使えば、プログラムの動作中に GPIO の入出力状態を変更したり、GPIO に接続されたセンサの値を読み取ったりできます。プログラムの動作を止めることなく、GPIO の状態を確認したり変更したりできるので、デバッグや実験に便利です
- **PWM 制御**: `pwm` コマンドを使えば、GPIO に PWM 信号を出力できます。例えば、LED の明るさを調整したりモータの速度を制御したりするのに、手軽な信号発生器として利用できます
- **シリアル通信**: `i2c`, `spi`, `uart` コマンドを使えば、これらのインターフェースに接続されているデバイスを制御できます。コマンドでテスト的にデバイスを制御し、その結果をプログラムに反映させながら開発を進めることができます

## まとめ

この記事では、Pico SDK の開発環境の設定と、pico-jxgLABO の組み込み方法を解説しました。pico-jxgLABO を組み込むことで、対話システム、ロジックアナライザ、内蔵フラッシュドライブ、USB ドライブ、USB シリアルポートなどの多彩な機能を利用できるようになります。 これらの機能を活用して、Pico ボードでの開発を進めていきましょう。
