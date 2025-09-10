---
title: "その持てる力をすべて解き放て! Pico SDK と pico-jxgLABO の組み込み方法"
emoji: "⛳"
type: "tech" # tech: 技術記事 / idea: アイデア
topics: ["zennfes2025free", "raspberrypi", "pico", "raspberrypipico", "library"]
published: true
published_at: 2025-09-16 09:00
---
[pico-jxgLABO](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro) は、USB ケーブル一本でマイコンボード RaspberryPi Pico の様々な機能を試すことができる実験プラットフォームです。

この記事では、pico-jxgLABO を既存のプログラムに組み込んだり、このプラットフォームをベースに新しい機能を追加したりする方法を紹介します。実験プラットフォームにとどまらない、pico-jxgLABO の真の能力を引き出しましょう。

:::message
pico-jxgLABO は C/C++ 言語で開発されており、Raspberry Pi 財団が提供する [Pico SDK](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf) をベースにしています。MicroPython や Arduino の開発経験はあるけれども、C/C++ は初めてという方も多いかと思います。この記事では、Pico SDK 開発環境の準備から、pico-jxgLABO の組み込み方法までを丁寧に解説します。思ったよりもあっさりできますよ!
:::

## pico-jxgLABO を組み込むメリット

pico-jxgLABO のバイナリ (UF2 ファイル) を Pico ボードに書き込むと、コマンド操作で [GPIO 制御](https://zenn.dev/ypsit/articles/2025-08-03-labo-gpio)や[ロジックアナライザの機能](https://zenn.dev/ypsit/articles/2025-09-08-labo-la)を利用できるようになります。ちょっとした実験ならばこれでできますが、あらかじめ用意されたコマンドレベルの機能しか使えないため、できることに限界があります。

このプラットフォームの真の力は、pico-jxgLABO のソースコードを自分のプログラムに組み込むことで発揮されます。pico-jxgLABO を組み込むことで、以下のサービスを享受できます。

- **対話システム**: pico-jxgLABO のシェルを使えば、PC のターミナルソフトから Pico ボードにコマンドを送って対話的に操作できます。プログラムの動作状況を確認したり、パラメータを変更したりするのに便利です
- **ロジックアナライザ**: ロジックアナライザ機能を使えば、プログラムが期待通りの信号を GPIO に出力しているか確認できます。また、I2C や SPI などの通信プロトコルの解析も可能です
- **内蔵フラッシュドライブ**: : pico-jxgLABO は Pico ボードの内蔵フラッシュメモリをファイルシステムとして使えるようにしています。これにより、このプラットフォームを組み込んだ
プログラムがデータをファイルに保存したり読み込んだりできます
- **USB ドライブ**: 上の内蔵フラッシュドライブは、USB 接続された PC からもアクセス可能です。これにより、PC と Pico ボード間でデータのやり取りが簡単に行えます
- **USB シリアルポート**: USB シリアルポートを 2 個提供します。ひとつはターミナルソフトからのコマンド入力、もうひとつは他のアプリケーションとの通信に使用できます

組込みに必要なのは、ビルド設定ファイルに 3 行、任意の C または C++ プログラムに 3 行のコードを追加するだけです。早速、組み込み方法を見ていきましょう!

なお、開発環境は Windows を想定しています。

## 初めての Pico SDK アプリ開発

このセクションでは、Piso SDK 環境を持っていない方のために、開発環境の設定と Pico SDK プロジェクトの作成方法を解説します。

## 開発環境の設定

まずは以下のツールをインストールしてください。

- Visual Studio Code: https://code.visualstudio.com/download
- Git ツール: https://git-scm.com/downloads
- ターミナルソフト: ここでは [Tera Term](https://teratermproject.github.io/) を使います

また、プロジェクトを開いたりプログラムを書き込んだりするのに CUI を多用しますので、PowerShell やコマンドプロンプトのショートカットを作っておくと便利です。

Pico SDK (C/C++ のビルドに必要なツールやライブラリ群) は Visual Studio Code (以下 VSCode) の拡張機能でインストールします。VSCode を起動したら `[F1]` キーを押して以下のコマンドパレット (VSCode の機能をコマンドで実行できるテキストボックス) を開いてください。

![vscode-palette.png](/images/2025-01-17-picosdk/vscode-palette.png)

ここで　`>Extensions: Install Extensions` (最初の `>` は初めからコマンドパレットに入力されているもの) を入力して、拡張機能のインストール画面を開きます。検索ボックスに `pico` などと入力すると、一覧の中に以下のような `Raspberry Pi Pico` という拡張機能が見つかるので、これをクリックしてインストールしてください。

![pico-extension.png](/images/2025-01-17-picosdk/pico-extension.png)

これで開発に必要な設定はすべて完了です。

### Pico SDK プロジェクトの作成

Pico SDK のプロジェクトを作成するには、VSCode で `[F1]` キーを押して表示されるコマンドパレットから `>Raspberry Pi Pico: New Pico Project` を実行します。`New Pico Project` というダイアログボックスが出て使用言語を聞かれるので `C/C++` を選択すると以下の画面が表示されます。

![new-project.png](/images/2025-01-17-picosdk/new-project.png)

以下の項目を設定してください。ほかはデフォルトのままで結構です。

- **Basic Settings**
  - `Name`: プロジェクト名を入力します。ここでは `Sample1` とします
  - `Board type`: 使用しているボードタイプを選択します
  - `Location`: `[Change]` ボタンをクリックしてプロジェクトディレクトリを作成するひとつ上のディレクトリ名を選択します。ここでは `C:\Users\YOUR-NAME\pico-project` とします
- **Code generation options**
  - `Generate C++ code`: チェックを入れます

右下の `[Create]` ボタンをクリックするとプロジェクトを作成します。初めてプロジェクトを作成した場合は、Pico SDK のダウンロードとインストールを行うのでかなり時間がかかります (20 分ほど)。初めの一回だけなので、気長に待ちましょう。

プロジェクトの作成が完了すると、プロジェクトディレクトリ `C:\Users\YOUR-NAME\pico-project\Sample1` が作成され、そのディレクトリをワークスペースにした VSCode が自動的に開きます。

`Sample1` ディレクトリの中には以下のようなファイルが作成されています。

- `CMakeLists.txt`: プロジェクトのビルド設定ファイルです
- `Sample1.cpp`: メインの C++ プログラムです

C++ プログラムの内容は以下のようになっていると思います。

```cpp: Sample1.cpp
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

ビルドをするには `[F7]` キーを押します。プロジェクトを作成して初めてのビルドだと、以下のようなダイアログが表示されるので、`Pico Using compilers: ...` を選択します。

![vscode-select-kit.png](/images/2025-01-17-picosdk/vscode-select-kit.png)

ビルドが成功すると、`C:\Users\YOUR-NAME\pico-project\Sample1\build` ディレクトリが作成され、`Sample1.uf2` というバイナリファイルが生成されます。

Pico ボードの BOOTSEL ボタンを押しながら USB ケーブルで PC と接続すると、Pico ボードが USB ドライブとして認識されます。Sample1.uf2 ファイルをこのドライブにコピーすると、Pico ボードが自動的に再起動してプログラムが実行されます。

ところで、`printf()` の `Hello, world!` はいったいどこで行ってしまうのでしょうか? Pico SDK は `printf()` の出力先を UART や USB シリアルなど柔軟にカスタマイズできるようになっていて、今回は pico-jxgLABO が提供する USB シリアルポートに出力することにします。この記事の続き、pico-jxgLABO の組み込み方法に進んでください。

### Pico SDK プロジェクトの開き方

すでに存在する Pico SDK プロジェクトを開くには、コマンドプロンプトでプロジェクトディレクトリに移動して `code .` コマンドを実行します。例えば `C:\Users\YOUR-NAME\pico-project\Sample1` ディレクトリを開くには、以下のようにします。

```text
cd C:\Users\YOUR-NAME\pico-project\Sample1
code .
```

このとき、Pico SDK の設定情報などを記録した `.vscode` ディレクトリがないと、以下のようなダイアログが表示されます。

![open-workspace.png](/images/2025-01-17-picosdk/open-workspace.png)

`Select a Kit for ...` のダイアログは無視して、右下の `Do you want to import this project as Raspberry Pi Pico project?` のダイアログで `[Yes]` ボタンをクリックしてください。以下のメイン画面が表示されます。

![import-project.png](/images/2025-01-17-picosdk/import-project.png)

`[Import]` ボタンをクリックすると、`.vscode` ディレクトリが作成され、Pico SDK プロジェクトとして開けるようになります。

## pico-jxgLABO の組み込み方法

pico-jxgLABO は pico-jxglib というライブラリ群の上に構築されています。pico-jxglib は [GitHub リポジトリ](https://github.com/ypsitau/pico-jxglib) で管理されています。

pico-jxglib と、それを組み込むプロジェクト (ここでは `Sample1`) のディレクトリ構成が以下のようになっているとします。ここでは C++ のプロジェクトを想定していますが、C 言語のプロジェクトでも同様です。

```text
C:\Users\YOUR-NAME\pico-project
  ├── pico-jxglib/
  └── Sample1/
      ├── CMakeLists.txt
      ├── Sample1.cpp
      └── ...
```

git ツールを使って pico-jxglib のソースコードをダウンロード(クローン)します。

```text
cd C:\Users\YOUR-NAME\pico-project
git clone https://github.com/ypsitau/pico-jxglib.git
cd pico-jxglib
git submodule update --init --recursive
```

プロジェクト `Sample1` のワークスペースを VSCode で開きます。`CMakeLists.txt` の最後に以下の行を追加してください。

```cmake:CMakeLists.txt
target_link_libraries(Sample1 jxglib_LABOPlatform_FullCmd)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
jxglib_configure_LABOPlatform(Sample1)
```

ソースファイル `Sample1.cpp` の内容を以下のように変更します。

```cpp: Sample1.cpp
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
3. `sleep_ms()` の代わりに `jxglib_sleep()` を使います。pico-jxgLABO のスリープ関数は内部でシステムの状態を管理しているため、こちらを使う必要があります。

これで組み込みは完了です。`[F7]` キーを押してビルドしてください。ビルドが成功すると `build` ディレクトリに `Sample1.uf2` ファイルが生成されるので、BOOTSEL ボタンを押しながら USB ケーブルを接続して認識されるドライブにコピーしてください。

シリアル通信をするため Tera Term を起動します。メニューバーから `[設定 (S)]`-`[シリアルポート (E)...]` を選択し、接続する Pico ボードのシリアルポートを選択します。

![teraterm-setting.png](/images/2025-08-01-labo-intro/teraterm-setting.png)

pico-jxgLABO は 2 つのシリアルポートを提供します。一つはターミナル用、もう一つはロジックアナライザやプロッタなどのアプリケーション用です。提供する最初のシリアルポート (上の例だと `COM21`) がターミナル用になります。これを選択して `[新規オープン (N)]` または `[現在の接続を再設定 (N)]` をクリックします。

Tera Term のウィンドウに `Hello, world!` が 1 秒ごとに表示されれば成功です。おめでとうございます!

ところで、pico-jxgLABO の機能はどうすれば使えるのでしょうか? 実は、もうすでに動いているんですよ! Tera Term で `[Enter]` キーを押してみてください。`Hello, world!` の表示に交じって以下のようなプロンプトが現れるはずです。

```text
L:/>
```

この状態で、[この記事](https://zenn.dev/ypsitau/articles/2025-08-01-labo-intro)などで紹介した機能がすべて使えるようになっています。pico-jxgLABO は**バックグラウンドで動作**するので、プログラムの本来の動作を止めることなく、様々なサービスを提供します。

`dir` コマンドを入力してみましょう。Pico ボードの内蔵フラッシュメモリに保存されているファイルの一覧が表示されます。`Hello, world!` がちょっとうるさいですが...。

```text
L:/>dir
-a--- 2000-01-01 00:00:00     77 README.txt
```

この内蔵フラッシュメモリは、ホスト PC からも USB ドライブとしてアクセスできます。多くの場合 `D:` ドライブとして認識されると思います。適当なファイルを PC からこのドライブにコピーしてから、Tera Term で `dir` コマンドを実行してみてください (ここでは `Sample1.cpp` をコピー)。

```text
L:/>dir
-a--- 2000-01-01 00:00:00     77 README.txt
-a--- 2025-09-10 15:09:54    245 Sample1.cpp
```

`cat` コマンドでファイルの中身を表示することもできます。

```text
L:/>cat Sample1.cpp
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

このプラットフォームを組み込んだことで、多くの機能が使えるようになりました。

## まとめ

この記事では、Pico SDK の開発環境の設定と、pico-jxgLABO の組み込み方法を解説しました。pico-jxgLABO を組み込むことで、対話システム、ロジックアナライザ、内蔵フラッシュドライブ、USB ドライブ、USB シリアルポートなどの多彩な機能を利用できるようになります。 これらの機能を活用して、Pico ボードでの開発を進めていきましょう。
