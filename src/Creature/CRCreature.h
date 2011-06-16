/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CRCREATURE_H
#define CRCREATURE_H

#include <Foundation/Scene.h>
#include <Creature/SprCRCreature.h>
#include <vector>

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �N���[�`���̎���
*/
class CRCreature : public Scene, public CRCreatureDesc {
protected:
	///	�G���W��
	typedef std::vector< UTRef<CREngineIf> > CREngines;
	CREngines engines;

	///	�V�[��
	typedef std::vector< UTRef<CRSceneIf> > CRScenes;
	CRScenes scenes;

	///	�{�f�B
	typedef std::vector< UTRef<CRBodyIf> > CRBodies;
	CRBodies bodies;

	/// �֘A����PHScene
	PHSceneIf* phScene;

public:
	SPR_OBJECTDEF(CRCreature);
	ACCESS_DESC(CRCreature);

	CRCreature(){
		CRRegisterTypeDescs();
	}
	CRCreature(const CRCreatureDesc& desc) : CRCreatureDesc(desc) {
		CRRegisterTypeDescs();
	}

	/** @brief ���o����񏈗����^�� �̂P�X�e�b�v�����s����
	*/
	virtual void Step();

	/** @brief �{�f�B������
	*/
	virtual CRBodyIf* CreateBody(const IfInfo* ii, const CRBodyDesc& desc);

	/** @brief �{�f�B���擾����
	*/
	virtual CRBodyIf* GetBody(int i) { return bodies[i]; }

	/** @brief �{�f�B�̐����擾����
	*/
	virtual int NBodies() { return (int)bodies.size(); }

	/** @brief CREngine���쐬����
	*/
	virtual CREngineIf* CreateEngine(const IfInfo* ii, const CREngineDesc& desc);

	/** @brief CREngine���擾����
	*/
	virtual CREngineIf* GetEngine(int i) { return engines[i]; }

	/** @brief CREngine�̐����擾����
	*/
	virtual int NEngines() { return (int)engines.size(); }

	/** @brief CRScene���쐬����
	*/
	virtual CRSceneIf* CreateScene(const IfInfo* ii, const CRSceneDesc& desc);
	
	/** @brief CRScene���擾����
	*/
	virtual CRSceneIf* GetScene(int i) { return scenes[i]; }

	/** @brief CRScene�̐����擾����
	*/
	virtual int NScenes() { return (int)scenes.size(); }

	/** @brief �֘A����PHScene���擾����
	*/
	PHSceneIf* GetPHScene() { return phScene; }

	/** @brief �q�v�f�̈���
	*/
	virtual size_t NChildObject() const { return bodies.size()+engines.size()+scenes.size(); }
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
};

}
//@}

#endif//CRCREATURE_H
