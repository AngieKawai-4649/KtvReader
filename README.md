# 慶安２期、さんぱくんの内蔵カードリーダーを使用しB25(B1)デコードするdllファイル  

ここで配布するBonDriver_FSUSB2N、BonDriver_SANPAKUN専用で動作し、他のBonDriverでは動作しない  
デバイス専用で動作し、他チューナー(PT2等)のデコードに流用することはできない  
KtvReader.dllにB25デコード機能を組み込んでビルドしているのでlibaribb25.dll等のデコーダーdllは必要ない  

## 【セットアップ方法】  

BonDriver.dllをリネームし複数用意している場合、それぞれに対応するようにKtvReader.dllを複数用意する  
dllファイル名は任意  
BonDriver.iniファイルの  
  [DECODER]  
  InternalReader  
  にdllファイル名を設定する  

例 BonDriver_FSUSB2N-1.dllの場合  
  BonDriver_FSUSB2N-1.iniファイルの  
  [DECODER]  
  InternalReader=KtvReader-1.dll  

## 【TSバッファ調整】  

内蔵カードリーダーは非常にレスポンスが遅いのでTSバッファサイズを大きくしないと特にCS ND6 ND16でDropする  
さんぱくんはTSバッファサイズをiniで設定できるようにしたので調整すること  

詳細はさんぱくんのBonDriver.iniに記載している  


## 【KtvReader.dllを置くフォルダ】  
対応するBonDriver.dllと同じ場所ではなく  
BonDriver.dllをロードしているexeのあるフォルダに置く  
TVTest:TVTest.exeのあるフォルダ  
