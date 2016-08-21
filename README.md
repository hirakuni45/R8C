R8C
=========

R8C sources 

## R8C 各ディレクトリー、及び概要など

> これは R8C と、そのコンパイラである m32c-elf-gcc,g++ によるプログラムです。
>
> 現在は、主に「R5F2M110ANDD」及び「R5F2M120ANDD」に対応した、ヘッダー、関連ライブラリーなどです。
>
> ※R8C/M110AN、R8C/M120AN
>
> ※M110AN、M120AN は通常、プログラムエリア２キロバイト、ワークエリア２５６バイトのデバイスですが、
> 実際は、プログラムエリア６４キロバイト、ワークエリア１３６６バイトを保有していて、フラッシュライター
> の制御ファイルを修正する事で、全領域を利用する事が可能です。（ロットにより異なる場合があると考えられる）
> 現在では、Windows、OS-X（Linuxでも多分動作する） で動作確認が済んだ、専用書き込みプログラムも実装して
> あり、色々な環境で、開発が出来るようになっています。
>
> プロジェクトは、Makefile、及び、関連ヘッダー、ソースコードからなり、専用のスタートアップルーチン、
> リンカースクリプトで構成されています。
> その為、専用のブートプログラムやローダーは必要なく、作成したバイナリーをそのまま実行できます。
>
> デバイスＩ／Ｏ操作では、C++ で構成されたクラスライブラリーを活用出来るように専用のヘッダーを用意
> してあり、各種デバイス用の小さなクラスライブラリーの充実も行っています。

## R8C プロジェクト・リスト

 - /r8cprog         --->   R8C フラッシュへのプログラム書き込みツール（Windows、OS-X、※Linux 対応）
 - /M120AN          --->   M120AN,M110AN デバイス、Ｉ／Ｏポート定義
 - /common          --->   R8C 共有クラス、小規模なクラスライブラリーなど
 - /pfatfs          --->   ぷち FatFS 関係ソース、ヘッダー
 - /FIRST_sample    --->   ＬＥＤ点滅テスト
 - /UART_sample     --->   シリアルインターフェースの送信、受信テスト
 - /TIMER_sample    --->   タイマーＲＢのテスト（インターバルタイマー）
 - /ADC_sample      --->   Ａ／Ｄ変換のテスト
 - /DATA_FLASH_sample --->   データフラッシュの初期化、リード、ライト
 - /PWM_sample        --->   タイマーＲＣのテスト（ＰＷＭ出力）
 - /RC_SERVO_sample --->   ラジコン用サーボの動作テスト（ＰＷＭ、２出力）
 - /COMP_sample     --->   コンパレーターのテスト
 - /DS1371_sample   --->   I2C RTC デバイスのテスト（DS1371）
 - /DS3231_sample   --->   I2C RTC デバイスのテスト（DS3231）
 - /EEPROM_sample   --->   I2C EEPROM デバイスのテスト

 - /LCD_test        --->   128x32 mono color graphics のテスト
 - /SD_test         --->   pFatFS を使った、SD カードのテスト
 - /SD_monitor      --->   pFatFS を使った、SD カードのモニター
 - /SD_WAV_play     --->   SD カード上の WAV 形式ファイルの PWM 再生
 - /PLUSE_OUT_test  --->   タイマーＲＪを使ったパルス出力テスト
 - /SWITCH_test     --->   スイッチ入力テスト（チャタリング除去とトリガー判定）
 - /ENCODER_test    --->   エンコーダー入力テスト
 - /PLUSE_OUT_LCD   --->   タイマーＲＪ、ＬＣＤ、エンコーダー
 - /PLUSE_INP_test  --->   タイマーＲＪを使った周波数計測テスト
---
   
## R8C 開発環境

 - R8C 用コンパイラ（m32c-elf）は gcc-4.9.3 を使っています。 
 - Windows では、事前に MSYS2 環境をインストールしておきます。
 - MSYS2 には、msys2、mingw32、mingw64 と３つの異なった環境がありますが、msys2 で行います。 
   
 - msys2 のアップグレード

```sh
   update-core
```

 - コンソールを開きなおす。（コンソールを開きなおすように、メッセージが表示されるはずです）

```sh
   pacman -Su
```
 - アップデートは、複数回行われ、その際、コンソールの指示に従う事。
 - ※複数回、コンソールを開きなおす必要がある。

---

### m32c-elf-gcc コンパイラのビルド方法：（msys2）

 - gcc、texinfo、gmp、mpfr、mpc、diffutils、automake、zlib tar、make、unzip コマンドなどをインストール
```sh
   pacman -S gcc
   pacman -S texinfo
   pacman -S mpc-devel
   pacman -S diffutils
   pacman -S automake
   pacman -S zlib
   pacman -S tar
   pacman -S make
   pacman -S unzip
```
  
 - git コマンドをインストール
```sh
   pacman -S git
```

#### binutils-2.25.1 をビルド
```sh
   cd
   tar xfvz binutils-2.25.1.tar.gz
   cd binutils-2.25.1
   mkdir m32c_build
   cd m32c_build
   ../configure --target=m32c-elf --prefix=/usr/local/m32c-elf --disable-nls
   make
   make install
```

 -  /usr/local/m32c-elf/bin へパスを通す（.bash_profile を編集して、パスを追加）

```
   PATH=$PATH:/usr/local/m32c-elf/bin
```

 -  コンソールを開きなおす。

```
   m32c-elf-as --version
```

 -  アセンブラコマンドを実行してみて、パスが有効か確かめる。
  
#### C コンパイラをビルド
 -  gcc-4.9.3では、MSYS2 環境を認識しない為、automake の設定をコピーする
``` sh
    cd
    tar xfvz gcc-4.9.3.tar.gz
    cd gcc-4.9.3
    cp /usr/share/automake-1.9/config.guess .
    mkdir m32c_build
	cd m32c_build
    ../configure --prefix=/usr/local/m32c-elf --target=m32c-elf --enable-languages=c --disable-libssp --with-newlib --disable-nls --disable-threads --disable-libgomp --disable-libmudflap --disable-libstdcxx-pch --disable-multilib --disable-bootstrap
    make
    make install
```
  
#### newlib をビルド
``` sh
    cd
    tar xfvz newlib-2.2.0.tar.gz
	cd newlib-2.2.0
    mkdir m32c_build
    cd m32c_build
    ../configure --target=m32c-elf --prefix=/usr/local/m32c-elf
	make
    make install
```
  
#### C++ コンパイラをビルド
``` sh
    cd
    cd gcc-4.9.3
    cd m32c_build
    ../configure --prefix=/usr/local/m32c-elf --target=m32c-elf --enable-languages=c,c++ --disable-libssp --with-newlib --disable-nls --disable-threads --disable-libgomp --disable-libmudflap --disable-libstdcxx-pch --disable-multilib --disable-bootstrap
    make
    make install
```
   
---
   
## R8C プロジェクトのソースコードを取得

```
   git clone git@github.com:hirakuni45/R8C.git
```
   
--- 

## R8C/M120AN, R8C/M110AN デバイスへのプログラム書き込み方法

幾つかの方法がありますが、最も簡単で、コストがかからない方法は、シリアルインターフェースを使って
書き込む方法です。   
   
シリアルインターフェースからの２つの信号をマイコンと接続する必要があります。   
※USB シリアル変換モジュールなどを使うと、電源も取れて簡単です。
※（秋月電子、シリアル変換モジュール）http://akizukidenshi.com/catalog/g/gK-06894/
(1) RXD シリアル受信   
(2) TXD シリアル送信   
(3) VCC 電源（５Ｖ又は３．３Ｖ）   
(4) GND 電源 ０Ｖ   
※３．３Ｖは限られた電流しか取り出せない為、レギュレーターを入れる事を推奨します。   
※中国製の格安なモジュールは、品質が安定していない為、お勧めしません、それらの事
項を理解していて対処出来る人だけ利用すると良いと思います。
   
 - 誤解しやすい事項として、R8C で標準的に使う UART の接続とは一部異なっている点です。
 - 回路図は、r8cprog/KiCAD/ を参照して下さい、標準的接続回路があります。   
 - もちろん、ルネサスエレクトロニクス販売のＥ１、又は、Ｅ２、エミュレーターでも書き込む事が出来ます。
 - シリアル通信は、開発過程では良く利用するので、スイッチで切り替えられるようにしておくと便利です。   
※切り替えの参考回路が、「r8cprog/KiCAD」にあります。   

---
   
## R8C フラッシュプログラマーの構築

 - 以下は、まだ未完、実装中です・・・・・
 - ※ Renesas Flash Programmer の最新版を使って下さい。
 - r8c_prog のビルドには「boost_1_60_0」が必要です。
 - boost はヘッダーのみ利用なので、ビルドの必要はありません、boost_1_60_0.zip を展開するだけです。
 - 又は、mingw64 環境などに pacman を使い boost をインストールして、そのパスを設定しても良いでしょう。

``` 
    cd /usr/local
    unzip boost_1_60_0.zip
```

 - r8c_prog のビルド（MSYS2）
 - ビルドした実行ファイルは、~/bin 又は、/usr/local/bin に配置します。

``` sh
    cd r8cprog
    make
    mkdir ~/bin
    cp r8c_prog.exe ~/bin/.
    cp r8c_prog.conf ~/bin/.
　　※~/bin にパスを通しておく。
```
 - r8c_prog.conf を編集して、接続する COM ポート、ボーレートの設定をする。
 - /dev/ttyS10 -> COM11 に相当します。（数字に＋１する）
 - r8cprog/KiCAD/ に、R8C プログラマー（書き込み機）の参考回路などが含まれます。
  
---
   
## 各プロジェクトの動作

 - 各プロジェクトのリストを参考に、プロジェクトのディレクトリーに移動します。

 - ビルドします。（自動で、従属規則が生成されます）
``` sh
    make
```

 - プログラムの書き込み（r8c_prog が必要）
``` sh
    make run
```

License
----

MIT
