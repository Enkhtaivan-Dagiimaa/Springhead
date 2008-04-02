/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRSENSOR_H
#define CRSENSOR_H

#include <Springhead.h>

#include <Foundation/Object.h>

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief ���o����
*/
class CRSensor : public SceneObject, public CRSensorDesc {
protected:
	/// ���̃Z���T�̎����傽��Creature
	CRCreatureIf* creature;

	/// ��������V�[��
	PHSceneIf* phScene;

public:
	SPR_OBJECTDEF(CRSensor);
	ACCESS_DESC(CRSensor);

	CRSensor(){}
	CRSensor(const CRSensorDesc& desc, CRCreatureIf* c=NULL) : CRSensorDesc(desc) {
		if(c!=NULL && c->GetScene()){SetScene(c->GetScene());}
		creature = c;
	}

	/** @brief ���������s��
	*/
	virtual void Init();

	/** @brief ���o���͂̏��������s����
	*/
	virtual void Step();
};
}
//@}

#endif//CRSENSOR_H
