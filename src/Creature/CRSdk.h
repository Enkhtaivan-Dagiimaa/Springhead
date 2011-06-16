/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRSDK_H
#define CRSDK_H

#include <Foundation/Scene.h>
#include <Creature/SprCRSdk.h>
#include <Creature/SprCRCreature.h>

namespace Spr {;

class CRSdkFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return CRSdkIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, const ObjectIf*){
		return CRSdkIf::CreateSdk();
	}
};

class SPR_DLL CRSdk : public Sdk {
protected:
	///	�N���[�`��
	typedef std::vector< UTRef<CRCreatureIf> > CRCreatures;
	CRCreatures creatures;

public:
	SPR_OBJECTDEF(CRSdk);
	CRSdk();
	~CRSdk();

	/** @brief �N���[�`������̍쐬����
	 */
	virtual CRCreatureIf* CreateCreature(const IfInfo* ii, const CRCreatureDesc& desc);

	/** @brief �N���[�`���̌��i�l���H�j���擾����
	 */
	virtual int NCreatures() const {
		return (int)creatures.size();
	}

	/** @brief �N���[�`����̂��擾����
	 */
	virtual CRCreatureIf* GetCreature(int index) {
		return creatures[index];
	}

	/** @brief �S�N���[�`���ɂ��ĂP�X�e�b�v���������s����
	 */
	virtual void Step();

	/** @brief ��x�S�Ă��N���A����SDK����蒼��
	 */
	virtual void Clear();

	/** @brief �q�v�f�̈���
	 */
	virtual size_t NChildObject() const { return NCreatures(); }
	virtual ObjectIf* GetChildObject(size_t i){ return GetCreature((int)i)->Cast(); }
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);

};

}
#endif // CRSDK_H
