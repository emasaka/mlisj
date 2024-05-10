# mlisj

Minimal Lisp Interpreter for SKK-JISYO.

## SYNOPSIS

SKK-JISYO内のLisp式を評価する最小限のインタプリタ。ライブラリとして使うことを想定している。

## USAGE

Cの場合：


```
#include <mlisj.h>

int mlisj_eval(const char *src, char *dest, size_t size, char **skk_num_list, const char *skk_henkan_key)
```

### ARGUMENTS

* `src`
    * 評価するS式の文字列
* `dest`
    * 評価結果が書き込まれるバッファ
* `size`
    * `dest`の最大サイズ
* `skk_num_list`
    * 数値変換で渡す数値のリスト
    * 文字列の配列（NULL終わり）
        * 例：`{"3", "1", NULL}`
    * 指定しない場合はNULLでよい
* `skk_henkan_key`
    * 変換前の文字列（見出し語）を渡す
    * `skk-gengo-to-ad`関数でのみ使われる
    * 通常はNULLでよい

### RETURN VALUE

* 0：正常終了
* MLISJ_ERROR_EVALUATION：S式の解釈エラー。"(笑)"のような文字列が渡されたときなど
* MLISJ_ERROR_RUNTIME：実行エラー。確保したメモリリソースを使い切ったときなど

### CAUTION

* 文字コードはUTF-8
* 「#0」などの数値変換は、呼び出し元で展開済みなものとする

## FEATURES

### 関数

* -
* car
* concat
* current-time-string
* make-string
* pwd
* string-to-char
* symbol-value
* substring
* string-to-number
* window-width
* skk-version
* skk-times
* skk-gadget-units-conversion
* skk-ad-to-gengo
* skk-gengo-to-ad
* skk-default-current-date
* skk-current-date
* skk-relative-date

### 変数

* fill-column
* comment-start
* user-full-name
* user-mail-address
* skk-num-list

## NOTE

以下、仕様と実装の注意点。

### 全体的な方針

* SKK辞書で使われている関数や特殊形式、機能のみ実装
* 1つの式を評価して文字列を返すだけに特化
* 環境はそのたびに作ってクリアする。GC無し
    * 初期化に時間をかけない
* パフォーマンスより実装の単純さを優先
* データはLispのレベルではイミュータブル
* 実行エラーの場合はすみやかに返る（“(笑)”のような文字列を渡された場合など）
* libc以外の外部ライブラリにはあまり異存したくない

### 実装していない基本的なEmacs Lisp仕様の例

* ベクター
* Lispでの変数代入
* Lispでの関数定義
* tの定義

### 現状の実装の制限（手抜き）

* リストはcdr-codedな配列で表現。点対表現による連想リストや、set-cdrはできない
* 文字列やシンボルには“\0”を入れられない（Cの文字列関数を使っているため）
* リーダーの未実装機能
    * 整数リテラルは10進数のみ対応。先頭の'+'や末尾の'.'にも未対応
    * シンボル中のエスケープ
    * 文字リテラルのエスケープ
    * マルチバイト文字リテラル
    * 文字列中の16進数表記、unicodeコード表記
    * 文字列中の8進数表記の桁数チェック
    * 文字列中のマルチバイト文字のエスケープ
* 文字列からシンボルへの名前解決や、シンボルから関数の名前解決は、線形探索
    * 数が少ないので問題にならないはず
* Lisp_Object構造体（タグ付きポインタのようなもの）は、だいたいポインタ2つぶんのサイズになっていて無駄が多い
    * 一応、src/lispobject.hの定義を変えるだけで小さくはできるはず
* skk_num_listに入ってくるのは整数の文字列のみを想定（DDSKKでいうとskk-num-convert-floatがnilの想定）
* `skk-default-current-date`関数関連は、SKK-JISYO.lispで使われている引数のみ対応

### Emacs Lispと違う仕様

* ()とnilは別
* 1引数の`concat`は引数の文字列をそのまま返す
    * SKK辞書では文字列エスケープの手段として多用されているため
