# ALNS for Grouping Problems
グループ分け問題に対する適応的巨大近傍探索(ALNS)を作成しました

Dataディレクトリ内部に問題ファイル(jsonファイル)をALNSを用いて解くプログラムです

## 1. 準備
### makeコマンドの準備
Windows(wslを除く)は下記のURLからダウンロードし, パスを通しておきます

http://gnuwin32.sourceforge.net/packages/make.htm

<br>
Linux(wsl含む)は下記コマンドでインストールします

```
sudo apt install make
```
<br>

### OpenMPの準備
Windowsでインストールしたコンパイラの場合, それぞれのインストーラを開き, OpenMPを有効化してください

Linux(wsl含む)の場合はおそらく初期から有効化されていると思います

<br>

OpenMPを有効にしなくても動きますが, 問題のサイズによっては解が出るまで時間がかかる可能性があります

無効化する場合はMakefileのCFLAGSを以下のように変更してください
```Makefile
CFLAGS = -O3 -std=c++17 -finput-charset=CP932
```

## 2. 問題ファイルの作成
後で書く

## 3. 実行
cloneしたディレクトリでmakeコマンドを実行します
```
make
```
binディレクトリが作成され, その中にrun.exe(Linux, wslではrun.out)が作成されます

以下のように実行します
```
bin\run.exe -p 問題ファイル
```
もしくは
```
./bin/run.out -p 問題ファイル
```
問題ファイルにはDataディレクトリ内の問題ファイル名を指定するか問題ファイルへのパスを指定してください

ただし実行コマンドからDataディレクトリを探索するためファイル名のみの指定はうまくファイルを見つけることができないことがあります

その場合は問題ファイルへのパスで指定してください