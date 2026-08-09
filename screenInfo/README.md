# mainSecene.json の仕様

このファイルは、LVGL のサンプル画面を JSON で定義するためのシーン定義です。
現在は、タイトルラベル・ステータスラベル・2つのボタンを持つ簡単な画面構成を表しています。

## 目的

- 画面上の UI 要素を JSON で管理する
- ラベルやボタンの見た目・配置・フォーカス設定をまとめて記述する
- 後から UI の差し替えや追加をしやすくする

## 対象ファイル

- [screenInfo/mainSecene.json](screenInfo/mainSecene.json)

## 主要な構成

```json
{
  "labels": {
    "titleLabel": { ... },
    "statusLabel": { ... }
  },
  "buttons": {
    "fullPressButton": { ... },
    "nextButton": { ... }
  }
}
```

## 各セクションの意味

### labels

ラベル要素をまとめたセクションです。

- `titleLabel`
  - 画面上部に表示するタイトル
- `statusLabel`
  - 中央付近に表示する状態説明文

各ラベルは `appearance` 配下に以下を持ちます。

- `text.label`: 表示文字
- `text.font`: フォント名
- `text.color`: 文字色
- `text.alignment`: 文字揃え
- `size.width`, `size.height`: サイズ
- `position.align`, `position.offsetX`, `position.offsetY`: 配置

### buttons

ボタン要素をまとめたセクションです。

- `fullPressButton`
  - 上部中央に配置されるボタン
- `nextButton`
  - 下部中央に配置されるボタン

各ボタンは次の内容を持ちます。

- `appearance`
  - テキスト、サイズ、位置、色、角丸、枠線などの見た目
- `interaction.registeredEvent`
  - クリック時のイベント登録情報
- `focus`
  - フォーカス可能か、初期フォーカスか、タブ順序など

## 現在の実装イメージ

この JSON から次のような画面を作る想定です。

- タイトルラベル: `LVGL Button Demo`
- ステータスラベル: `Press a button`
- ボタン 1: `FullPress`
- ボタン 2: `Next`

## 補足

- `LV_ALIGN_*` や `LV_STATE_*` のような値は、実装側で LVGL の定数に変換して扱う前提です。
- `runtimeAction` のような追加情報は、今後の拡張用として残しておくことができます。
- 画面要素の追加・変更は、この JSON の対応するキーを増減させるだけで対応しやすい構造になっています。
