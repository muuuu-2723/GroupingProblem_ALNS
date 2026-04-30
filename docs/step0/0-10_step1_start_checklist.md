# STEP.1開始チェックリスト

## 1. 目的

本項目では、STEP.0で定義したプロジェクト準備、環境整備、方針決定が完了していることを確認し、STEP.1「現行プログラム整理」に進むための開始条件を定義する。

STEP.1では、GitHub Copilot Agent Modeを使用して、現行ソースコードのコメント整理および現行プログラムの設計整理を行う。

---

## 2. STEP.1開始条件

STEP.1は、以下を満たした時点で開始可能とする。

- プロジェクト目的・ゴールが明確になっている。
- 対象プログラムの概要が整理されている。
- 現行プログラムがGit上で保全されている。
- GitHub Copilotを使用できる環境が整っている。
- 開発環境・実行環境が整理されている。
- 成果物一覧が定義されている。
- 作業ルールが定義されている。
- リファクタリング方針が定義されている。
- Copilotへの指示テンプレートが作成されている。
- STEP.1で作成する成果物と作業範囲が明確になっている。

---

## 3. STEP.0成果物確認チェックリスト

| No. | 確認項目 | 対象ファイル | 状態 |
|---:|---|---|---|
| 1 | プロジェクト目的・ゴールが定義されている | `docs/step0/0-1_project_goal.md` | ○ |
| 2 | 対象プログラム概要が整理されている | `docs/step0/0-2_program_overview.md` | ○ |
| 3 | 現行プログラム保全方針が整理されている | `docs/step0/0-3_source_preservation.md` | ○ |
| 4 | GitHub Copilot利用環境と運用方針が整理されている | `docs/step0/0-4_copilot_environment.md` | ○ |
| 5 | 開発環境・実行環境が整理されている | `docs/step0/0-5_development_environment.md` | ○ |
| 6 | 成果物一覧が定義されている | `docs/step0/0-6_deliverables.md` | ○ |
| 7 | 作業ルールが定義されている | `docs/step0/0-7_working_rules.md` | ○ |
| 8 | リファクタリング方針が定義されている | `docs/step0/0-8_refactoring_policy.md` | ○ |
| 9 | Copilotへの指示テンプレートが定義されている | `docs/step0/0-9_copilot_prompt_templates.md` | ○ |
| 10 | STEP.1開始条件が定義されている | `docs/step0/0-10_step1_start_checklist.md` | ○ |

---

## 4. Git・ブランチ確認チェックリスト

| No. | 確認項目 | 確認内容 | 状態 |
|---:|---|---|---|
| 1 | `master` が現行プログラムの保全用ブランチとして扱われている | `master` に直接作業しない方針になっている | ○ |
| 2 | リファクタリング前タグが作成されている | `v0.0.0-before-refactoring` | ○ |
| 3 | STEP.0作業ブランチで作業している | `step0/project-setup` | ○ |
| 4 | STEP.0成果物がGit管理されている | `docs/step0/` 配下 | ○ |
| 5 | Copilot関連ファイルがGit管理されている | `.github/` 配下 | ○ |
| 6 | レビュー指摘票関連ファイルがGit管理されている | `docs/reviews/` 配下 | ○ |
| 7 | `.gitignore` が整理されている | ビルド成果物・一時ファイルを除外 | ○ |
| 8 | 不要な `result.txt` が削除または除外されている | 研究時の不要メモを管理対象から外す | ○ |

---

## 5. Copilot利用環境チェックリスト

| No. | 確認項目 | 確認内容 | 状態 |
|---:|---|---|---|
| 1 | VS Codeを使用する方針になっている | エディタはVS Code | ○ |
| 2 | GitHub Copilotが利用できる | 補完機能が使用可能 | ○ |
| 3 | GitHub Copilot Chatが利用できる | チャットで指示可能 | ○ |
| 4 | Agent Modeが利用できる | 複数ファイル作業を依頼可能 | ○ |
| 5 | `.github/copilot-instructions.md` が作成されている | 共通指示が定義されている | ○ |
| 6 | STEP.1用プロンプトが作成されている | `step1-comment-cleanup.prompt.md` | ○ |
| 7 | STEP.1現行設計整理用プロンプトが作成されている | `step1-current-design-doc.prompt.md` | ○ |
| 8 | NeighborhoodGraph詳細整理用プロンプトが作成されている | `step1-neighborhood-graph-analysis.prompt.md` | ○ |
| 9 | レビュー指摘修正用プロンプトが作成されている | `review-fix.prompt.md` | ○ |
| 10 | Copilotへの指示言語が日本語で統一されている | コメント・ドキュメントも原則日本語 | ○ |

---

## 6. 開発環境・実行環境チェックリスト

| No. | 確認項目 | 確認内容 | 状態 |
|---:|---|---|---|
| 1 | 使用言語・標準が整理されている | C++17 | ○ |
| 2 | コンパイラが整理されている | g++ / g++ (tdm64-1) 10.3.0で確認済み | ○ |
| 3 | ビルド方法が整理されている | `make` | ○ |
| 4 | WSL2での実行コマンドが整理されている | `./bin/run.out -p Data/A-n32-k5.json` | ○ |
| 5 | Windows cmdでの実行コマンドが整理されている | `bin\run.exe -p Data\A-n32-k5.json` | ○ |
| 6 | OpenMPの扱いが整理されている | 必須ではないが推奨 | ○ |
| 7 | 文字コード方針が整理されている | CP932維持 | ○ |
| 8 | 通常実行とデバッグモードの違いが整理されている | Debug関連は原則主対象外 | ○ |
| 9 | 動作確認用JSONが整理されている | `Data/A-n32-k5.json` | ○ |
| 10 | 実行成功の判断基準が整理されている | 評価値・グループ分け・実行時間が出力される | ○ |

---

## 7. STEP.1作業範囲チェックリスト

| No. | 確認項目 | 内容 | 状態 |
|---:|---|---|---|
| 1 | STEP.1では既存処理を変更しない | コメント整理・ドキュメント作成のみ | ○ |
| 2 | コメント整理対象が明確である | `src/` 配下の現行ソースコード | ○ |
| 3 | コメント形式が定義されている | 主要クラス・主要関数はDoxygen風コメントを使用可 | ○ |
| 4 | コメントの粒度が定義されている | 重要処理を優先し、冗長コメントは避ける | ○ |
| 5 | 現行設計整理の成果物が定義されている | `docs/step1/` 配下 | ○ |
| 6 | `NeighborhoodGraph` を個別に詳細整理する方針になっている | `neighborhood_graph_design_note.md` | ○ |
| 7 | 処理フローはMarkdown + Mermaidで整理する方針になっている | `current_process_flow.md` | ○ |
| 8 | 卒論資料・画像は必要に応じて補足資料として扱う | 初回はソースコード中心、必要に応じて追加参照 | ○ |
| 9 | レビュー指摘票で修正指示を管理する | 誤字・軽微修正も記録対象 | ○ |
| 10 | STEP.1の成果物がSTEP.2-1要件定義のインプットになる | 現行仕様・課題・要確認事項を整理する | ○ |

---

## 8. STEP.1成果物確認チェックリスト

STEP.1で作成・更新する予定の成果物は以下である。

| No. | 成果物 | 日本語名 | 区分 | 状態 |
|---:|---|---|---|---|
| 1 | コメント整理済みソース | 現行ソースコードのコメント整理 | 必須 | ○ |
| 2 | `docs/step1/current_feature_list.md` | 現行機能一覧 | 必須 | ○ |
| 3 | `docs/step1/current_input_json_spec.md` | 現行入力JSON仕様 | 必須 | ○ |
| 4 | `docs/step1/current_process_flow.md` | 現行処理フロー | 必須 | ○ |
| 5 | `docs/step1/search_methods_overview.md` | 探索手法一覧 | 必須 | ○ |
| 6 | `docs/step1/algorithm_implementation_mapping.md` | アルゴリズム・実装対応表 | 必須 | ○ |
| 7 | `docs/step1/neighborhood_graph_design_note.md` | NeighborhoodGraph詳細メモ | 必須 | ○ |
| 8 | `docs/step1/class_responsibility_list.md` | クラス責務一覧 | 準必須 | ○ |
| 9 | `docs/step1/key_function_list.md` | 主要関数一覧 | 準必須 | ○ |
| 10 | `docs/reviews/step1/` 配下のレビュー指摘票 | STEP.1レビュー記録 | 必要に応じて作成 | ○ |

---

## 9. STEP.1開始前の作業ブランチ準備

STEP.1を開始する前に、以下を実施する。

```bash
git checkout master
git pull origin master
git checkout -b step1/current-code-analysis
git push -u origin step1/current-code-analysis
```

ただし、STEP.0の成果物を `master` に反映する前にSTEP.1へ進む場合は、`step0/project-setup` からSTEP.1ブランチを作成するか、STEP.0成果物を先に `master` へ反映するかを判断する。

推奨は以下である。

1. `step0/project-setup` の成果物を確認する。
2. 問題がなければ `master` へ反映する。
3. `master` から `step1/current-code-analysis` を作成する。

---

## 10. STEP.1開始可否判定

以下を満たしていれば、STEP.1を開始してよい。

| 判定項目 | 状態 |
|---|---|
| STEP.0成果物がすべて作成されている | ○ |
| Git運用・ブランチ運用が整理されている | ○ |
| Copilot Agent Modeが利用できる | ○ |
| STEP.1用プロンプトが作成されている | ○ |
| 開発環境・実行環境が整理されている | ○ |
| STEP.1の成果物一覧が明確である | ○ |
| STEP.1で処理変更しない方針が明確である | ○ |
| レビュー指摘票運用が可能である | ○ |
| CP932を維持する方針が明確である | ○ |
| `NeighborhoodGraph` を個別に整理する方針が明確である | ○ |

---

## 11. 0-10 完了条件

0-10は、以下を満たした時点で完了とする。

- STEP.1開始条件が定義されている。
- STEP.0成果物の確認項目が定義されている。
- Git・ブランチ確認項目が定義されている。
- Copilot利用環境の確認項目が定義されている。
- 開発環境・実行環境の確認項目が定義されている。
- STEP.1作業範囲の確認項目が定義されている。
- STEP.1成果物の確認項目が定義されている。
- STEP.1開始前のブランチ準備手順が定義されている。
- STEP.1開始可否判定項目が定義されている。