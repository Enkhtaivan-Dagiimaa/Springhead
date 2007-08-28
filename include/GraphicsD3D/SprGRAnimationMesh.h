/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprGRAnimationMesh.h
 *	@brief キャラクタアニメーション用メッシュ
*/

#ifndef SPR_GRAnimationMesh_H
#define SPR_GRAnimationMesh_H

#include <Graphics/SprGRFrame.h>
#include <windows.h>
#include <d3dx9.h>

namespace Spr{;

///	@brief GRAnimationMeshのデスクリプタ．
struct GRAnimationMeshDesc /*: GRVisualDesc*/{
	std::string filename;		///< 読み込むファイル名(.x)
};

typedef void (SPR_CDECL *GRAnimationMeshDrawSubsetListenerFunc)(DWORD attribId);

/**	@brief	グラフィックスで使うキャラクタアニメーション表示用のMesh */
struct GRAnimationMeshIf: public GRVisualIf{
	IF_DEF(GRAnimationMesh);
	virtual void SetMotion(const std::string& name)=0;		///< モーションを選択
	virtual void SetTime(double time)=0;					///< モーションの時刻を設定(単位は秒)

	/** @brief キーフレームによるボーンの角度を取得
		@param name   ボーンの名前
	*/
	virtual Posed GetBoneKeyframePose(const std::string& name)=0;
	
	/** @brief ボーンの角度を（物理モデルの関節の角度などで）上書きする
		@param name        ボーンの名前
		@param orientation 上書きする角度
		@param weight      上書き率（0〜1, 0：キーフレームに100%従う, 1：orientationに100%従う）
	*/
	virtual void OverrideBoneOrientation(const std::string& name, const Quaterniond& orientation, double weight)=0;
	
	/** @brief ボーンの角度と親ボーンからの相対位置（ルートボーンの場合は自身の絶対位置）を上書きする
		@param name   ボーンの名前
		@param pose   上書きする位置角度
		@param weight 上書き率（0〜1, 0：キーフレームに100%従う, 1：poseに100%従う）
	*/
	virtual void OverrideBonePose(const std::string& name, const Posed& pose, double weight)=0;
	
	/** @brief DrawSubsetの実行時にコールバックされる関数を登録
		@param beforeFunc DrawSubsetが実行される直前にコールバックされる関数
		@param afterFunc  DrawSubsetが実行された直後にコールバックされる関数
	*/
	virtual void AddDrawSubsetListener(GRAnimationMeshDrawSubsetListenerFunc beforeFunc=NULL, GRAnimationMeshDrawSubsetListenerFunc afterFunc=NULL)=0;

	/** @brief 描画に使用するエフェクトを設定
		@param effect            描画に使用するエフェクト
		@param matrixPaletteSize ボーンのワールド変換を格納する配列のサイズ
		エフェクトを使う場合はRender()呼び出しをID3DXEffect::BeginPass()とEndPass()で挟む
		エフェクト内に用意するパラメータは以下のとおり（型、セマンティクス）
			float4x4						WORLD						// ワールド行列
			int								MAXVERTEXINFL				// 一つの頂点に影響するボーンの最大数
			float4x3[matrixPaletteSize]		BONEMATRIXARRAY				// ボーンの変換行列の配列（デュアルクォータニオンを使うなら不要）
			float4[matrixPaletteSize]		BONEQUATERNIONARRAYREAL		// ボーンのデュアルクォータニオンのreal-partの配列（行列を使うなら不要）
			float4[matrixPaletteSize]		BONEQUATERNIONARRAYDUAL		// ボーンのデュアルクォータニオンのdual-partの配列（行列を使うなら不要）
			float4							DIFFUSECOLOR				// マテリアルのディフューズ色
		スキニングしないメッシュではMAXVERTEXINFL,BONE***は使われない
	*/
	virtual void SetEffect(LPD3DXEFFECT effect, int matrixPaletteSize)=0;
};


}
#endif
