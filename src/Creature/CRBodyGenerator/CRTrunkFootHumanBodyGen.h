/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRTRUNKFOOTHUMANBODYGEN_H
#define CRTRUNKFOOTHUMANBODYGEN_H

#include <Creature/CRBodyGenerator/CRBodyGen.h>

//@{
namespace Spr{;

/// ���́E���l�̃��f���̃f�X�N���v�^
struct CRTrunkFootHumanBodyGenDesc : CRBodyGenDesc {

	enum CRHumanSolids{
		// ����
		SO_WAIST=0, SO_CHEST, SO_HEAD, SO_RIGHT_FOOT, SO_LEFT_FOOT,
		// ���̂̐�
		SO_NSOLIDS
	};

	enum CRHumanJoints{
		// �֐�
		JO_WAIST_CHEST=0, JO_CHEST_HEAD,
		// �֐߂̐�
		JO_NJOINTS
	};

	/// �T�C�Y�Ɋւ���p�����[�^
	double waistHeight, waistBreadth, waistThickness;
	double chestHeight, chestBreadth, chestThickness;
	double neckLength;
	double headDiameter;
	double footLength, footBreadth, footThickness;

	/// �e�֐߂̃o�l�_���p
	double springWaistChest, damperWaistChest;
	double springChestHead,    damperChestHead;

	/// ���搧��
	Vec2d rangeWaistChest;
	Vec2d rangeChestHead;

	CRTrunkFootHumanBodyGenDesc();

};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRTrunkFootHumanBodyGen
// �q���W�W���C���g��p�����l�̃��f���E�N���X�̎���
class CRTrunkFootHumanBodyGen : public CRBodyGen, public CRTrunkFootHumanBodyGenDesc {
private:
	void InitBody();
	void CreateWaist();
	void CreateChest();
	void CreateHead();

	void InitLegs();
	void CreateFoot(LREnum lr);

	void InitContact();

public:

	CRTrunkFootHumanBodyGen(){}
	CRTrunkFootHumanBodyGen(const CRTrunkFootHumanBodyGenDesc& desc, PHSceneIf* s=NULL) 
		: CRTrunkFootHumanBodyGenDesc(desc) 
		, CRBodyGen((const CRBodyGenDesc&)desc, s)
	{
		solids.resize(CRTrunkFootHumanBodyGenDesc::SO_NSOLIDS);
		joints.resize(CRTrunkFootHumanBodyGenDesc::JO_NJOINTS);

		InitBody();
		InitLegs();

		InitContact();
	}

	/** @brief �����������s����
	*/
	virtual void Init();

};

}
//@}

#endif//CRTRUNKFOOTHUMANBODYGEN_H
