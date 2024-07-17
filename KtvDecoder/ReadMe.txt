慶安２期、さんぱくんの内蔵カードリーダーを使用しB25(B1)デコードするdllファイル
今回配布するBonDriver専用で動作する
デバイス専用で動作し、他チューナーのデコードに流用することはできない

セットアップ方法

BonDriver.dllをリネームし複数用意している場合、それぞれに対応するようにKtvReader.dllを複数用意する
dllファイル名は任意
BonDriver.iniファイルの
[DECODER]
InternalReader
にdllファイル名を設定する

内蔵カードリーダーは非常にレスポンスが遅いのでTSバッファサイズを大きくしないと特にCS ND6 ND16でDropする
さんぱくんはTSバッファサイズをiniで設定できるようにしたので調整すること

詳細はさんぱくんのBonDriver.iniに記載している


KtvReader.dllを置くフォルダ
対応するBonDriver.dllと同じ場所ではなく
BonDriver.dllをロードしているexeのあるフォルダに置く
TVTest:TVTest.exeのあるフォルダ
EDCB  :EpgDataCap_Bon.exeのあるフォルダ