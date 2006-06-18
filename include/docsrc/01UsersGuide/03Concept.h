/** \page pageConcept	Springheadの構成

Springheadは大きく分けて以下の3つのモジュールから構成されています．
- 　 \c Springhead \c Physics　 ⇒　 \ref pagePhysics
- 　 \c Springhead \c Graphics　 ⇒　 \ref pageGraphics  
- 　 \c Springhead \c File \c I/O　 ⇒　 \ref pageFileIO

<br>

これらのモジュールは，個別に用いることもできるように設計されています．
例えば，ユーザの中にはレンダリングは手持ちのライブラリを使い，物理シミュレーションのみを
Springheadで行いたいと考えている人もいるでしょう．このような場合にも，必要最低限の追加的な
コードだけでユーザのライブラリとSpringheadとを連動させることができます．

以下では個別のモジュールについて順に解説します．
<br><br><br>


\section sec_physics 物理シミュレーションSDK
\par
\ref pagePhysics は，物理シミュレータ本体です．接触判定や物体運動のシミュレーションを行います．
物理シミュレーションのシーン(世界)には，物体の質量や慣性モーメント，関節の定義，摩擦係数や跳ね返り係数，バネダンパなどがあります．
物体の色や光源など，物体の見た目についてはGraphcisで扱います．Physicsにはありません．
<br><br><br><br>


\section sec_graphics グラフィックスSDK
\par
\ref pageGraphics は，グラフィックスレンダラです．
Physicsにはまったくシーンを描画する機能が無いため，シーンの様子を目で見ることができません．
Graphcisは，Physicsのシーンを描画して，様子を見るための，Debugのレンダリング機能を持ちます．

\par
また，Graphcisは，映像専用のシーンを用意して美しい映像提示を行うこともできます．
毎ステップ，Physicsで計算した物体運動を映像専用のシーンに反映させてから描画することになります．

\par
Graphicsを使わずに，Physicsの計算結果を自分の描画プログラムから参照して，
独自の描画プログラムを作ることもできます．
また，他のグラフィックスやシーングラフのライブラリを使って描画し，Physicsの計算結果を
ライブラリ伝えるプログラムだけを自分で用意する手もあります．
<br><br><br><br>


\section sec_fileio ファイル入出力SDK
\par
\ref pageFileIO は，ファイルローダとセーバです．シーングラフのロードとセーブを行います．
FileIOを使うと，シーングラフ(＝オブジェクトのツリー)をデータファイルからロードしたり，
データファイルにセーブしたりできます．
FileIOは他のモジュールのAPIを利用してロードしたデータからオブジェクトを生成したり，
他のモジュールのAPIを利用してシーングラフからデータを取り出して，ファイルに保存したりしています．

\par
FileIOを使わずに，自分で1からデータファイルをロードする仕組みを作ることもできます．
<br><br><br><br>
*/
