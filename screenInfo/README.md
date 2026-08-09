# JSON の仕様

このファイルは、LVGL の画面を JSON で定義するためのシーン定義です。

# 目的

- 画面上の UI 要素を JSON で管理する
- ラベルやボタンの見た目・配置・フォーカス設定をまとめて記述する
- 後から UI の差し替えや追加をしやすくする

# 主要な構成

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

# 各セクションの意味

## 共通
### appearance

#### text
- `label`: 表示文字
- `color`: 文字色
- `alignment`: 文字揃え
- `font`: フォント名
  - Montserratのみ

| サイズ(px) | 定数名 | 変数名 |
|---|---|---|
| 12 | `LV_FONT_MONTSERRAT_12` | `lv_font_montserrat_12` |
| 14 | `LV_FONT_MONTSERRAT_14` | `lv_font_montserrat_14` |
| 16 | `LV_FONT_MONTSERRAT_16` | `lv_font_montserrat_16` |
| 18 | `LV_FONT_MONTSERRAT_18` | `lv_font_montserrat_18` |
| 20 | `LV_FONT_MONTSERRAT_20` | `lv_font_montserrat_20` |
| 22 | `LV_FONT_MONTSERRAT_22` | `lv_font_montserrat_22` |
| 24 | `LV_FONT_MONTSERRAT_24` | `lv_font_montserrat_24` |
| 26 | `LV_FONT_MONTSERRAT_26` | `lv_font_montserrat_26` |
| 28 | `LV_FONT_MONTSERRAT_28` | `lv_font_montserrat_28` |
| 30 | `LV_FONT_MONTSERRAT_30` | `lv_font_montserrat_30` |
| 32 | `LV_FONT_MONTSERRAT_32` | `lv_font_montserrat_32` |
| 34 | `LV_FONT_MONTSERRAT_34` | `lv_font_montserrat_34` |
| 36 | `LV_FONT_MONTSERRAT_36` | `lv_font_montserrat_36` |
| 38 | `LV_FONT_MONTSERRAT_38` | `lv_font_montserrat_38` |
| 40 | `LV_FONT_MONTSERRAT_40` | `lv_font_montserrat_40` |
| 42 | `LV_FONT_MONTSERRAT_42` | `lv_font_montserrat_42` |
| 44 | `LV_FONT_MONTSERRAT_44` | `lv_font_montserrat_44` |
| 46 | `LV_FONT_MONTSERRAT_46` | `lv_font_montserrat_46` |
| 48 | `LV_FONT_MONTSERRAT_48` | `lv_font_montserrat_48` |

#### size
- `width`, `height`: サイズ

#### position

- `align`: 基準位置
  - 選択候補

| 定数 | 配置される位置 |
|---|---|
| `LV_ALIGN_TOP_LEFT` | 左上 |
| `LV_ALIGN_TOP_MID` | 上中央 |
| `LV_ALIGN_TOP_RIGHT` | 右上 |
| `LV_ALIGN_LEFT_MID` | 左中央 |
| `LV_ALIGN_CENTER` | 中央 |
| `LV_ALIGN_RIGHT_MID` | 右中央 |
| `LV_ALIGN_BOTTOM_LEFT` | 左下 |
| `LV_ALIGN_BOTTOM_MID` | 下中央 |
| `LV_ALIGN_BOTTOM_RIGHT` | 右下 |

- `offsetX`: 配置
  - 右向きが+
- `offsetY`: 配置
  - 下向きが+


## labels

ラベル要素をまとめたセクションです。

## buttons

ボタン要素をまとめたセクションです。

### appearance
ボタンの見た目に関する設定をまとめます。共通の appearance ルールに加えて、通常状態・フォーカス状態ごとの見た目を分けて定義できます。

#### baseStyle
- 通常状態のボタン見た目
- `backgroundColor`: 背景色
- `borderColor`, `borderWidth`, `radius`: 枠線・角丸
- `padding`: 内側余白

#### focusedStyle
- フォーカス時に適用する見た目
- デフォルトの設定値をあらかじめ削除を推奨します
  - lv_obj_remove_style(btn, NULL, LV_STATE_FOCUS_KEY);
- `baseStyle` と同じプロパティを上書きできる

#### focusStates
- フォーカス状態の切り替えルールを定義する
- ボタンの場合、以下の2つ状態を設定できる
  - ORで両方のビットを指定した場合は、その両方の条件が成立することを条件とする

| 定数 | 内容 |
|---|---|
| `LV_STATE_FOCUSED` | フォーカスが当たっている状態全般(タッチ/クリックでのフォーカスも含む) |
| `LV_STATE_FOCUS_KEY` | フォーカスの中でも、**キーボードやエンコーダーでのフォーカスのみ**(タッチ/クリックでは付かない) |

### interaction
ボタンの操作挙動に関する設定です。
- `callback`: イベントに対してのコールバック関数を登録します
- `event`: 以下のイベントコードから選択します

| イベント | 発生タイミング |
|---|---|
| `LV_EVENT_PRESSED` | 指/カーソルが押された瞬間（押しっぱなしでは再発火しない） |
| `LV_EVENT_CLICKED` | 押して、同じオブジェクト上で離した時（一般的な「クリック」） |
| `LV_EVENT_RELEASED` | 押されていた指/カーソルが離された時（クリック成立・不成立問わず） |
| `LV_EVENT_LONG_PRESSED` | 一定時間（デフォルト約400ms）押し続けた瞬間に1回発火 |
| `LV_EVENT_LONG_PRESSED_REPEAT` | 長押し中、一定間隔で繰り返し発火（連打的な処理向け） |
| `LV_EVENT_PRESSING` | 押されている間、継続的に発火（ドラッグ量の取得などに使用） |
| `LV_EVENT_SHORT_CLICKED` | 短時間で押して離した時（長押しと区別したい場合） |
| `LV_EVENT_VALUE_CHANGED` | チェックボタンなど、状態（トグル）が変化した時 |

### focus
フォーカス移動やキーボード操作に関する設定です。
- `group`: フォーカス対象をまとめるグループ名
  - 例: `default`
  - 同じグループ内でフォーカス移動の対象になる
- `focusable`: この要素がフォーカス可能かどうか
  - `true` のときのみフォーカス移動の対象になる
- `initialFocus`: 画面表示時に最初にフォーカスされる要素かどうか
  - `true` であれば初期フォーカス対象になる

### runtimeAction
実行時に動的に変化させる動作を定義します。
**まだ検討中**
