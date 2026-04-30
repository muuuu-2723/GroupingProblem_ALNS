---
description: STEP.2-2 基本設計作成
---

# 目的

STEP.2-1 要件定義に基づき、リファクタリング後の基本設計を作成してください。

この作業は STEP.2-2 の「基本設計」に該当します。

# 作成・更新するドキュメント

${input:target_documents}

# 前提

本プロジェクトでは、既存ALNSの機能維持を前提とします。

焼きなまし法は、現行ALNSとは別の独立した探索アルゴリズムとして追加するのではなく、現行ALNSの探索ループや解の受理判定へ考え方を組み込む方針です。

Debug関連・グラフ描画関連は、原則として主対象外です。

# 参照するドキュメント

以下を参照してください。

- `docs/step2/requirements.md`
- `docs/step1/current_feature_list.md`
- `docs/step1/current_input_json_spec.md`
- `docs/step1/current_process_flow.md`
- `docs/step1/search_methods_overview.md`
- `docs/step1/algorithm_implementation_mapping.md`
- `docs/step1/neighborhood_graph_design_note.md`
- `docs/step1/class_responsibility_list.md`
- `docs/step0/0-5_development_environment.md`
- `docs/step0/0-7_working_rules.md`
- `docs/step0/0-8_refactoring_policy.md`

# 主な作成・更新対象

主に以下を作成・更新してください。

- `docs/step2/basic_design.md`
- 必要に応じて `docs/step2/simulated_annealing_design.md`
- 必要に応じて `docs/step2/json_input_review.md`

# 設計で整理する内容

以下を整理してください。

- リファクタリング後の全体方針
- 既存ALNSとの関係
- 焼きなまし法の組み込み位置
- 受理判定の方針
- 温度管理の方針
- 冷却スケジュールの方針
- パラメータ指定方法
- JSON入力構造を変更するかどうか
- 既存JSONとの互換性
- 変更対象ファイル
- 変更しないファイル
- 既存機能維持のための注意点
- Debug関連・グラフ描画関連への影響有無
- テストで確認すべき観点

# Debug・グラフ描画関連の扱い

以下のファイル・機能は、原則として基本設計の主対象外です。

- `src/Debug/Debug.hpp`
- `src/Debug/Debug.cpp`
- `src/include/matplotlib.hpp`
- `src/DrawGraph/draw_graph.py`
- デバッグモードのグラフ描画機能

構造変更によりビルドできない、または利用できなくなる場合のみ、必要最小限の対応方針を設計に含めてください。

# 制約

- 実装は行わないでください。
- 既存処理を変更しないでください。
- 不明点を推測で確定しないでください。
- 複数案がある場合は、選択肢とメリット・デメリットを整理してください。
- ドキュメントは日本語で記載してください。
- ファイル名、クラス名、関数名、変数名は英語表記のまま記載してください。

# 作業後の報告

作業後、以下を報告してください。

- 作成・更新したドキュメント
- 設計上の主要な判断
- 未決定事項
- 実装前に確認すべき事項
- Debug関連・グラフ描画関連への影響有無