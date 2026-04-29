# GitHub Copilot利用環境の整備

## 1. 目的

GitHub CopilotをVisual Studio Code上で利用できる状態にし、STEP.1以降の現行プログラム整理、設計書作成、リファクタリング実装、テスト作業を支援できる環境を整備する。

また、Copilotへの指示内容をプロンプトファイルとしてリポジトリ内に管理し、後続プロジェクトでも再利用できる形にする。

---

## 2. 前提

| 項目 | 内容 |
|---|---|
| 使用エディタ | Visual Studio Code |
| Copilotの現状 | 未設定 |
| 主な利用方法 | GitHub Copilot Chat / Agent Mode |
| 対象リポジトリ | GroupingProblem_ALNS |
| 使用言語 | C++ |
| ビルド方法 | Makefile |
| 実行方式 | CLI |
| 入力形式 | JSON |
| 基本言語 | 日本語 |

---

## 3. 整備対象

本項目では、以下を整備対象とする。

- GitHub Copilotの利用環境
- VS Codeの拡張機能
- Copilot Agent Modeの利用環境
- Copilot向け共通指示ファイル
- 作業別プロンプトファイル
- レビュー指摘票による修正運用
- Copilot利用時の作業ルール

---

## 4. VS Codeに導入する拡張機能

### 必須

| 拡張機能 | 用途 |
|---|---|
| GitHub Copilot | コード補完 |
| GitHub Copilot Chat | チャット、Agent Mode |
| C/C++ | C++開発支援 |
| C/C++ Extension Pack | C++開発支援一式 |

### 必要に応じて導入

| 拡張機能 | 用途 |
|---|---|
| WSL | WSL2上での開発 |
| Remote Development | リモート・WSL開発支援 |
| Makefile Tools | Makefileベースのビルド支援 |

---

## 5. Copilot利用環境の確認項目

以下を確認する。

- GitHubアカウントにログインできること
- GitHub Copilotの利用権限があること
- VS CodeでGitHubにサインインできること
- GitHub Copilotによるコード補完が利用できること
- GitHub Copilot Chatが利用できること
- Agent Modeが利用できること
- 対象リポジトリを開いた状態で、Copilotがワークスペース内のファイルを参照できること

---

## 6. Copilot関連ファイル構成

Copilot向けの指示・プロンプト・レビュー指摘票は、以下の構成で管理する。

```text
.github/
├── copilot-instructions.md
└── prompts/
    ├── step1-comment-cleanup.prompt.md
    ├── step1-current-design-doc.prompt.md
    ├── step2-basic-design.prompt.md
    ├── step2-refactoring.prompt.md
    ├── step2-test.prompt.md
    └── review-fix.prompt.md

docs/
└── reviews/
    ├── README.md
    ├── review-ticket-template.md
    ├── step1/
    │   └── .gitkeep
    └── step2/
        └── .gitkeep