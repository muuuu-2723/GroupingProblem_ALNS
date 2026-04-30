---
description: STEP.2-3 リファクタリング実装
---

# 目的

STEP.2-1 要件定義および STEP.2-2 基本設計に基づき、現行プログラムのリファクタリング実装を行ってください。

この作業は STEP.2-3 の「実装」に該当します。

# 対象設計書

${input:design_document}

# 対象ファイル

${input:target_files}

# 前提

本プロジェクトでは、既存ALNSの機能維持を前提とします。

焼きなまし法は、現行ALNSとは別の独立した探索アルゴリズムとして追加するのではなく、現行ALNSの探索ループや解の受理判定へ考え方を組み込む方針です。

実装内容や変更方針は、対象設計書に記載された内容を優先してください。

# 参照するドキュメント

以下を参照してください。

- `docs/step2/requirements.md`
- `docs/step2/basic_design.md`
- `docs/step2/simulated_annealing_design.md`
- `docs/step2/json_input_review.md`
- `docs/step0/0-8_refactoring_policy.md`
- `docs/step0/0-7_working_rules.md`
- 対象設計書: `${input:design_document}`

# 作業内容

対象設計書に基づき、指定された対象ファイルに対して必要な実装を行ってください。

想定される作業は以下です。

- 現行ALNSへの焼きなまし法の考え方の組み込み
- 受理判定の修正
- 温度管理処理の追加
- 冷却スケジュールの追加
- 必要最小限のコード構造改善
- JSON入力処理の見直し
- 実行オプションの追加・修正
- READMEの必要箇所更新
- `docs/step2/implementation_summary.md` の更新

# Debug・グラフ描画関連の扱い

以下のファイル・機能は、原則としてリファクタリングの主対象外です。

- `src/Debug/Debug.hpp`
- `src/Debug/Debug.cpp`
- `src/include/matplotlib.hpp`
- `src/DrawGraph/draw_graph.py`
- デバッグモードのグラフ描画機能

これらは、構造変更によりビルドできない、または利用できなくなる場合のみ、必要最小限で修正してください。

修正が必要な場合は、理由と影響範囲を報告してください。

# 制約

- 要件定義・基本設計・対象設計書にない仕様変更は行わないでください。
- 既存ALNSの実行を壊さないでください。
- 既存JSON入力との互換性を可能な限り維持してください。
- 評価関数をソースコード側に固定しないでください。
- 文字コードはCP932のまま維持してください。
- ソースコードをUTF-8へ変換しないでください。
- 大規模な自動整形は行わないでください。
- 目的の異なる修正を混在させないでください。
- 不明点を推測で実装しないでください。

# 作業後の確認

可能であれば、以下を確認してください。

```bash
make
```

WSL2の場合：

```bash
./bin/run.out -p Data/A-n32-k5.json
```

Windows cmdの場合：

```cmd
bin\run.exe -p Data\A-n32-k5.json
```

# 実装内容まとめ

実装後、必要に応じて以下を更新してください。

```text
docs/step2/implementation_summary.md
```

記載内容の例：

- 実装日
- 作業ブランチ
- 変更したファイル
- 追加した処理
- 修正した処理
- 既存機能への影響
- ビルド確認結果
- 実行確認結果
- 未確認事項

# 作業後の報告

作業後、以下を報告してください。

- 変更したファイル
- 追加した処理
- 修正した処理
- 既存機能への影響
- Debug関連・グラフ描画関連への影響有無
- ビルド確認結果
- 実行確認結果
- 文字コードを変更していないことの確認
- 未確認事項
- `implementation_summary.md` に記載した内容