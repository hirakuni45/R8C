R8C
=========

R8C sources 

## R8C ディレクトリーの概要

> これは R8C と、そのコンパイラである gcc-m32c-elf によるプログラムです。
>
> 現在は、主に「R5F2M110ANDD」及び「R5F2M120ANDD」に対応した、ヘッダー、関連ライブラリーなどです。
>
> ※R8C/M110AN、R8C/M120AN
>
> ※M110AN、M120AN は通常、プログラムエリア２キロバイト、ワークエリア２５６バイトのデバイスですが、
> 実際は、プログラムエリア６４キロバイト、ワークエリア１３６６バイトが有効で、フラッシュライターの
> 制御ファイルを修正する事で、全領域を利用する事が可能です。（ロットにより異なる場合があると考えられる）
>
> プロジェクトは、Makefile、及び、関連ヘッダー、ソースコードからなり、専用のスタートアップルーチン、
> リンカースクリプトで構成されています。
>
> デバイスＩ／Ｏ操作では、C++ で構成されたクラスライブラリーを活用出来るように専用のヘッダーを用意
> してあります。

 - /r8cprog         --->   R8C フラッシュへのプログラム書き込みツール（Windows、OS-X、Linux 対応）

 - /M120AN          --->   M120AN,M110AN デバイス、Ｉ／Ｏポート定義

 - /common          --->   R8C 共有クラス、小規模なツールキットなど

 - /pfatfs          --->   ぷち FatFS 関係ソース、ヘッダー

 - /LED_test        --->   ＬＥＤ点滅テスト

 - /UART_test       --->   シリアルインターフェースの送信、受信テスト

 - /TIMER_test      --->   タイマーＲＢのテスト（インターバルタイマー）

 - /ADC_test        --->   Ａ／Ｄ変換のテスト

 - /FLASH_test      --->   フラッシュデータの初期化、リード、ライト

 - /PWM_test        --->   タイマーＲＣのテスト（ＰＷＭ出力）

 - /COMP_test       --->   コンパレーターのテスト

 - /I2C_RTC_test    --->  I2C RTC デバイスのテスト

 - /I2C_EEPROM_test --->  I2C EEPROM デバイスのテスト

 - /LCD_test        --->  128x32 mono color graphics のテスト

 - /SD_test         --->  pFatFS を使った、SD カードのテスト

 - /SD_monitor      --->  pFatFS を使った、SD カードのモニター

 - /PLUSE_OUT_test  ---> 　タイマーＲＪを使ったパルス出力テスト

 - /PLUSE_INP_test  ---> 　タイマーＲＪを使った周波数計測、パルス幅計測テスト

## R8C 開発環境

・コンパイラは gcc-4.74 を使っています。

コンパイラのビルド方法：

 - gmp、mpfr、mpc コマンドをインストール

 - binutils-2.25 をビルド
```sh
   cd
   tar xfvz binutils-2.25.tar.gz
   cd binutils-2.25
   mkdir m32c_build
   cd m32c_build
   ../configure --target=m32c-elf --prefix=/usr/local/m32c-elf --disable-nls
   make
   make install
```

 -  /usr/local/m32c-elf/bin へパスを通す

 -  コンソールを開きなおす。

 -  C コンパイラをビルド
``` sh
    cd
    tar xfvz gcc-4.7.4.tar.gz
    cd gcc-4.7.4
    mkdir m32c_build
	cd m32c_build
    ../configure --prefix=/usr/local/m32c-elf --target=m32c-elf --enable-languages=c --disable-libssp --with-newlib --disable-nls
    make
    make install
```

 -  newlib をビルド
``` sh
    cd
    tar xfvz newlib-2.1.0.tar.gz
	cd newlib-2.1.0
    mkdir m32c_build
    cd m32c_build
    ../configure --target=m32c-elf --prefix=/usr/local/m32c-elf
	make
    make install
```

 -  C++ コンパイラをビルド
``` sh
    cd
    cd gcc-4.7.4
    cd m32c_build
    ../configure --prefix=/usr/local/m32c-elf --target=m32c-elf --enable-languages=c,c++ --disable-libssp --with-newlib --disable-nls --disable-libstdcxx-pch
    make
    make install
```

License
----

MIT
