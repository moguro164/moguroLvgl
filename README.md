# LVGL Dev Environment (WSL + VSCode)

Windows上でLVGLのUI開発を行うための環境構築リポジトリです。
[lv_port_pc_vscode](https://github.com/lvgl/lv_port_pc_vscode) をベースに、WSL2 + VSCode + CMakeでビルドできるようにしています。

---

## 構成

| 項目 | 内容 |
|---|---|
| ホストOS | Windows 11 |
| 開発環境 | WSL2 (Ubuntu) |
| エディタ | VSCode + Remote-WSL |
| ビルドシステム | CMake |
| 表示 | SDL2(PCシミュレーター用) |

---

## クローン方法

このプロジェクトは `lvgl` / `FreeRTOS` をGitサブモジュールとして参照しています。
**必ず `--recurse-submodules` を付けてクローンしてください。**

```bash
git clone --recurse-submodules <このリポジトリのURL>
```

クローン後にサブモジュールが空だった場合は以下で取得できます。

```bash
git submodule update --init --recursive
```

---

## セットアップ手順

### 1. WSLのインストール

Windows側で、PowerShellを**管理者として実行**し、以下を実行します。

```powershell
wsl --install
```

WSL2とUbuntuが導入されます。完了後、再起動が必要です。

### 2. Ubuntuの初期設定

再起動後、スタートメニューから「Ubuntu」を起動し、案内に従ってLinux用のユーザー名・パスワードを設定します。

### 3. 必要パッケージのインストール

WSL(Ubuntu)のターミナルで以下を実行します。

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y build-essential cmake git libsdl2-dev pkg-config
```

| パッケージ | 役割 |
|---|---|
| build-essential | Cコンパイラ(gcc/g++)など基本ビルドツール |
| cmake | ビルド設定を生成するツール |
| git | ソース取得用 |
| libsdl2-dev | PC上でLVGLの画面を表示するためのライブラリ |
| pkg-config | ライブラリの場所をCMakeに伝える補助ツール |

### 4. VSCode拡張機能の導入(Windows側)

VSCodeの拡張機能タブから以下をインストールします。

- **WSL**(Microsoft製) — Remote-WSL接続用
- **CMake Tools**(Microsoft製)
- **C/C++**(Microsoft製)

### 5. プロジェクトを開く

WSLのターミナルで作業ディレクトリに移動し(`~`配下推奨。`/mnt/c/...`はビルドが遅くなるため非推奨)、本リポジトリをクローンします。

```bash
cd ~
git clone --recurse-submodules <このリポジトリのURL>
cd <リポジトリ名>
code .
```

VSCodeが起動し、左下に `WSL: Ubuntu` と表示されていれば接続成功です。
初回は「Restricted Mode」の警告バナーが出るので、**Manage → Trust** で信頼してください。

`simulator.code-workspace` をダブルクリックして開くと、ビルド/デバッグ設定が読み込まれます。

### 6. ビルド

```bash
cmake -B build -DCMAKE_SKIP_INSTALL_RULES=ON
cmake --build build
```

> **`-DCMAKE_SKIP_INSTALL_RULES=ON` について**
> LVGLのCMakeスクリプトに、`lv_conf.h` のパス解決に関する既知の不具合があり、
> インストールルールの検証時に `INTERFACE_INCLUDE_DIRECTORIES ... which is prefixed in the source directory` というエラーで
> configure/generateが失敗することがあります。シミュレーター用途ではインストール機能自体が不要なため、
> このオプションでインストールルールの生成自体をスキップして回避しています。

### 7. 実行

```bash
./bin/main
```

WSLg機能により、追加設定なしでWindows上にLVGLのデモウィンドウが表示されます。

---

## つまずきポイントと対処

### `lvgl` ディレクトリに `CMakeLists.txt` がないと言われる

サブモジュールが未取得です。以下を実行してください。

```bash
git submodule update --init --recursive
```

### `CMake Generate step failed`(INTERFACE_INCLUDE_DIRECTORIES 関連)

上述の「ビルド」手順の通り、`-DCMAKE_SKIP_INSTALL_RULES=ON` を付けてconfigureし直してください。

### `#error "LV_USE_VECTOR_GRAPHIC requires ..."`

`lv_conf.h` で `LV_USE_VECTOR_GRAPHIC` が `1` になっているのに、対応する描画エンジン(`LV_USE_THORVG`など)が `0` のままだと発生します。

`lv_conf.h` を編集し、いずれかで解消してください。

- ベクターグラフィックが不要なら `LV_USE_VECTOR_GRAPHIC` を `0` に変更(このリポジトリではこちらを採用)
- ベクターグラフィックを使いたいなら `LV_USE_THORVG` を `1` に変更
