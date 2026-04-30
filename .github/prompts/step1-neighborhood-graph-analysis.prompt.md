---
description: STEP.1 NeighborhoodGraph詳細整理
---

# 目的

`NeighborhoodGraph` は現行プログラム内でも複雑な探索処理であるため、アルゴリズム上の役割と実装内容を結び付けて整理してください。

この作業は STEP.1-2 の現行設計整理のうち、`NeighborhoodGraph` に特化した詳細整理に該当します。

# 作成・更新するドキュメント

${input:target_document}

# 主な対象ファイル

以下を主な対象としてください。

- `src/Search/NeighborhoodGraph/NeighborhoodGraph.hpp`
- `src/Search/NeighborhoodGraph/NeighborhoodGraph.cpp`

# 必要に応じて参照する関連ファイル

必要に応じて、以下も参照してください。

- `src/Search/Search.hpp`
- `src/Solution/Solution.hpp`
- `src/Solution/Solution.cpp`
- `src/Group/Group.hpp`
- `src/Group/Group.cpp`
- `src/Item/Item.hpp`
- `src/Item/Item.cpp`
- `src/Weight/Weight.hpp`
- `src/Weight/Weight.cpp`
- `src/MyRandom/MyRandom.hpp`
- `src/MyRandom/MyRandom.cpp`

# 補足資料の扱い

初回整理では、主にソースコードを根拠として `NeighborhoodGraph` の処理を整理してください。

ただし、`NeighborhoodGraph` は卒論で扱ったアルゴリズムと密接に関係する可能性があるため、ソースコードだけでは以下が判断できない場合があります。

- アルゴリズム上の概念との対応
- 処理の意図
- グラフ構造の意味
- 変数名やデータ構造が表す概念
- 卒論中の図や説明との対応

これらがソースコードだけで判断できない場合は、推測で断定せず、`要確認事項` として記録してください。

必要に応じて、後続のレビューまたは追加作業で、卒論本文・卒論中の図・画像データなどの補足資料を参照して内容を補完します。

# 前提

STEP.1 では、既存処理を変更しません。

この作業では、`NeighborhoodGraph` の処理を読み取り、後から確認した際にアルゴリズムと実装の対応が分かるように整理してください。

# 記載内容

以下を整理してください。

- `NeighborhoodGraph` の目的
- 探索全体における位置づけ
- 関連ファイル
- 管理しているデータ
- 主要関数
- 初期化処理
- グラフ構築処理
- 候補生成処理
- 評価処理
- 解更新処理
- 他クラスとの関係
- Mermaidによる処理フロー
- アルゴリズム上の概念との対応
- リファクタリング時の注意点
- 不明点・要確認事項
- 卒論資料・図表との対応が必要そうな箇所

# Mermaid利用方針

`NeighborhoodGraph` の処理が複雑な場合は、Markdown + Mermaidで処理フローを記載してください。

Mermaid図は以下の方針で作成してください。

- 1つの図に詰め込みすぎない
- 初期化、候補生成、評価、更新など処理単位で分ける
- ノード名は短くする
- 詳細な説明は本文に記載する

# 制約

- ソースコードは変更しないでください。
- 処理内容を推測で断定しないでください。
- コードから読み取れない点は要確認事項として記載してください。
- ドキュメントは日本語で記載してください。
- 関数名、クラス名、ファイル名、変数名は英語表記のまま記載してください。
- 大規模なコード整形は行わないでください。

# 作業後の報告

作業後、以下を報告してください。

- 作成・更新したファイル
- 参照した主なソースファイル
- 整理した内容の概要
- 特に複雑だった処理
- Mermaid図を作成した場合、その概要
- 要確認事項