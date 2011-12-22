/*
 *  Copyright (c) 2003-2011, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHHAPTICENGINE_H
#define PHHAPTICENGINE_H

#include <Springhead.h>
#include <Physics/PHContactDetector.h>

using namespace PTM;
namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticPointer
class PHHapticPointer : public PHSolid{
	SPR_OBJECTDEF(PHHapticPointer);
protected:
	int solidID;
	float localRange;
public:
	std::vector<int> neighborSolidIDs;
	HIBaseIf* humanInterface;
	PHHapticPointer(){ localRange = 1.0; }
	void	SetID(int id){ solidID = id; }
	int		GetID(){ return solidID; }
	void	SetLocalRange(float r){ localRange = r; } 
	float	GetLocalRange(){ return localRange; }
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

//----------------------------------------------------------------------------
// PHSolidForHaptic
class PHSolidForHaptic : public UTRefCount{  
	//SPR_OBJECTDEF(PHSolidForHaptic);
public:
	PHSolid* solid;
	bool bPointer;
	int NLocalFirst;
	int NLocal;
	int doSim;
	SpatialVector b;				///< �\���V�~�����[�V�����ŋ��߂����r���e�B�i�d�͓��̒萔���j
	SpatialVector curb;				///< �ʏ�V�~�����[�V�����łł��萔��
	SpatialVector lastb;			///< �O��̗\���V�~�����[�V�����ŋ��߂��萔��
	PHSolidForHaptic(){
		solid = NULL;		bPointer = false;
		NLocalFirst = 0;	NLocal = 0;	doSim = 0;
	}
};
class PHSolidsForHaptic : public std::vector< UTRef< PHSolidForHaptic > >{};


//----------------------------------------------------------------------------
// PHShapePairForHaptic
class PHSolidPairForHaptic;
class PHShapePairForHaptic : public CDShapePair{
public:	
	//PHSolidPairForHaptic* solidPair;

	Posed lastShapePoseW[2];
	Vec3d lastClosestPoint[2];	///< �O��̋ߖT���̂̐ڐG�_(���[�J�����W)
	Vec3d lastNormal;			///< �O��̋ߖT���̂̒񎦖ʂ̖@��
	double sectionDepth;
	double lastSectionDepth;
	std::vector< Vec3d > intersectionVertices; ///< �ڐG�̐ς̒��_(���[�J�����W)
	std::vector< Vec3d > pointerSection;	///< �|�C���^�̐ڐG���_(���[�J�����W)
	std::vector< Vec3d > solidSection;		///< ���̂̐ڐG���_(���[�J�����W)

	Vec3d test_force;				///< �\���V�~�����[�V�����Ŏg���e�X�g��
	Vec3d test_torque;				///< �\���V�~�����[�V�����Ŏg���e�X�g�g���N
	Vec3d impulse;					///< �����v���Z�X��1�X�e�b�v�I���܂łɗ͊o�|�C���^���������͐�


	PHShapePairForHaptic(){}
	bool Detect(unsigned ct, const Posed& pose0, const Posed& pose1);
	/// �ڐG����D�ߖT�_�΂��펞�X�V
	bool DetectClosestPoints(unsigned ct, const Posed& pose0, const Posed& pose1);
	int OnDetectClosestPoints(unsigned ct, const Vec3d& center0);

};

//----------------------------------------------------------------------------
// PHSolidPairForHaptic
class PHHapticEngineImp;
class PHSolidPairForHaptic : public PHSolidPair< PHShapePairForHaptic, PHHapticEngine >, public Object{
public:
	typedef PHSolidPair<PHShapePairForHaptic, PHHapticEngine> base_type;
	typedef base_type::shapepair_type shapepair_type;
	typedef base_type::engine_type engine_type;

	int inLocal;	// 0:NONE, 1:in local first, 2:in local
	struct Accelerance{
		Vec3d force;					// LocalDynamics�Ŏg����
		TMatrixRow<6, 3, double> A;		// LocalDynamics�Ŏg���A�N�Z�������X
		std::vector< Vec3d > f;			// LocalDynamics6D�Ŏg����
		TMatrixRow<6, 6, double> Minv;  // LocalDynamics6D�Ŏg���A�N�Z�������X
	} accelerance;
	
	/// ���������m���ꂽ��̏���
	virtual bool Detect(engine_type* engine, unsigned int ct, double dt);
	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt);	///< ���������m���ꂽ�Ƃ��̏���
	/// 
	
	//void Setup(unsigned int ct, double dt);
	//void GenerateForce();
protected:
};


//----------------------------------------------------------------------------
// PHHapticRenderImp
class PHHapticRenderImp : public SceneObject{
	SPR_OBJECTDEF_NOIF(PHHapticRenderImp);
public:
	PHHapticEngine* engine;
	PHHapticRenderImp(){}
	virtual void Step(){};
	virtual void StepHapticLoop(){};

	double GetPhysicsTimeStep();
	double GetHapticTimeStep();
	int NHapticPointers();
	PHHapticPointer** GetHapticPointers();
	int NHapticSolids();
	PHSolidsForHaptic** GetHapticSolids();
	PHSolidPairForHaptic* GetSolidPairForHaptic(int i, int j);

	///< �f�o�b�N�p�V�~�����[�V�������s
	virtual void StepSimulation(){};
};

//----------------------------------------------------------------------------
// PHHapticEngine


struct PHHapticEngineDesc{
	bool bHaptic;
	double hdt;
	PHHapticEngineDesc();
};
class PHHapticEngine : public PHHapticEngineDesc, public PHContactDetector< PHShapePairForHaptic, PHSolidPairForHaptic, PHHapticEngine >{
public:
	SPR_OBJECTDEF1(PHHapticEngine, PHEngine);
	ACCESS_DESC(PHHapticEngine);
	PHHapticRenderImp* renderImp;
	std::vector< UTRef<PHHapticRenderImp> > renderImps;
	PHHapticPointers hapticPointers;
	PHSolidsForHaptic hapticSolids;
	typedef std::vector< HIBaseIf* > HIBaseIfs;
	HIBaseIfs humanInterfaces;

	struct Edge{ Vec3f min, max; };
	std::vector< Edge > edges;

	enum RenderMode{
		NONE,
		IMPULSE,
	} renderMode;

	PHHapticEngine();

	///< �͊o�񎦌v�Z��ON/OFF
	void EnableHaptic(bool b){ bHaptic = b; }
	///< �����_�����O���[�h�̑I��
	void SetRenderMode(RenderMode r);
	///< �V�~�����[�V�������[�v�̍X�V�iPHScene::Integrate()����R�[���j
	virtual void Step(){ if(bHaptic) renderImp->Step(); }
	///< �͊o���[�v�̍X�V	
	virtual void StepHapticLoop(){ if(bHaptic) renderImp->StepHapticLoop(); }
	///< �͊o���[�v�̍���
	void SetHapticTimeStep(double dt = 0.001){ hdt = dt; }

	///< �͊o�|�C���^�̏�Ԃ̍X�V
	virtual void UpdateHapticPointer();
	///< �͊o�����_�����O�p�̏Փ˔���J�n
	virtual void StartDetection();
	///< BBox�̌������X�V
	void UpdateEdgeList();
	///< ���鍄�̂̋ߖT�̍��̂�AABB�ł݂���irange��BBox�������range�������L����
	void Detect(PHHapticPointer* q);
	int GetPriority() const {return SGBP_HAPTICENGINE;}
	///< ���̂̒ǉ�
	bool AddChildObject(ObjectIf* o);
	///< ���̂̍폜
	bool DelChildObject(ObjectIf* o);

	///< �f�o�b�N�p�V�~�����[�V�������s
	///�iPHScene::Step�̕ς��ɌĂԁj
	virtual void StepSimulation(){ renderImp->StepSimulation(); }



};

}	//	namespace Spr
#endif
