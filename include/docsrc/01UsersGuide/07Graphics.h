/** \page pageGraphics グラフィックスSDK


\contents


<!-------------------- グラフィックスSDKのオブジェクトのイメージ ----------------------------------------->
\section sec_GRSdkImage グラフィックスSDKのオブジェクトのイメージ
\c 1 つのグラフィックス<tt>SDK(GRSdk)</tt>は，いくつかのシーン<tt>(GRScene)</tt>を持つことができ，
シーンには座標系<tt>(Frame)</tt>のツリーの中に，メッシュ<tt>(GRMesh)</tt>，
光源<tt>(GRLight)</tt>，材質<tt>(GRMaterial)</tt>などがあります．
グラフィックス\c SDK 内のオブジェクトの参照関係の例を図に示します．
<br><br>
	\image html GRSdkScene.png
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>

<!-------------------- グラフィックスSDK ----------------------------------------------------------------->
\section sec_GRSdk	グラフィックスSDK
はじめに，グラフィックスに必要となる\c SDKを用意します．

\subsection subsec_CreateGRSdk	グラフィックスSDKの作成
グラフィックス\c SDKを作成します．
\verbatim
    GRSdkIf* grSdk = GRSdkIf::CreateSdk();
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>



<!-------------------- デバッグ情報レンダラー ------------------------------------------------------------>
\section sec_GRRender デバッグ情報レンダラー
グラフィックス\c SDK から，デバッグ情報レンダラーを作成します．

\subsection subsec_CreateGRRender	デバッグ情報レンダラーの作成
デバッグ情報レンダラーを作成します．
\verbatim
    GRDebugRenderIf* render = grSdk->CreateDebugRender();
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>

<!-------------------- レンダラー用デバイス -------------------------------------------------------------->
\section sec_GRDevice	レンダラー用デバイス
レンダリング用デバイスをしています．
今のところ OpenGL だけに対応しています．

\subsection subsec_CreateGRDevice	レンダラー用デバイスの作成
\c OpenGL レンダラー用デバイスを作成します．
\verbatim
    GRDeviceGLIf* grDevice = grSdk->CreateDeviceGL();
\endverbatim
<br>

\subsection subsec_InitGRDevice		レンダラー用デバイスの初期化
レンダラー用デバイスの初期化を行います．<br>
ライティング，デプステスト，法線の正規化などを有効にします．
\verbatim
    grDevice->Init();
\endverbatim
<br>


\subsection subsec_SetGRDevice		レンダラー用デバイスの設定
レンダラー用デバイスを設定します．
\verbatim
    render->SetDevice(grDevice);	// デバイスの設定
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 
<hr>

<!-------------------- 座標変換 -------------------------------------------------------------------------->
\section sec_TransMatrix	変換行列
\c Springhead2 では，視点行列，モデル行列，射影行列を記憶する行列を内部で持っています．<br>
これらの行列を定義することによって，意図する変換を行うことができます．

\subsection subsec_LookAt	視点・参照点の位置の指定
視点と参照点の位置を設定できます．以下の手順に従って下さい．
\verbatim
    Affinef view;                                                       // アフィン行列
    view.Pos() = Vec3f(0.0, 15.0, 15.0);                                // 視点
    view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));          // 参照点，上方向ベクトル
    view = view.inv();                                                  // 逆行列を求める
    render->SetViewMatrix(view);                                        // 視点行列へ登録
\endverbatim
<br>

\subsection subsec_MultModelMatrix		モデル行列をかける
現在の行列に乗算する行列を指定します．
\verbatim
    render->MultModelMatrix(afw);
\endverbatim
<br><br>
<div align="right">
<a href="#pageGraphics">top of page</a>
</div> 


*/
