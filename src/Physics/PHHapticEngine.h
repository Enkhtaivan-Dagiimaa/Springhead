/*
 *  Copyright (c) 2003-2011, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_HAPTICENGINE_H
#define PH_HAPTICENGINE_H

#include <Springhead.h>
#include <Physics/PHContactDetector.h>
#include <Physics/PHHapticPointer.h>
#include <Physics/PHHapticRender.h>

using namespace PTM;
namespace Spr{;

//----------------------------------------------------------------------------
// PHSolidForHaptic
//Haptic������Physics���֓n�����
struct PHSolidForHapticSt{
public:
	Vec3d force;			// �͊o�����_�����O�ɂ���ĉ����S�Ă̗�
	Vec3d torque;;			// �͊o�����_�����O�ɂ���ĉ����S�Ẵg���N
};
// Physics������Haptic���֓n�����
struct PHSolidForHapticSt2{
public:
	PHSolid* sceneSolid;	// PHScene��������
	
	bool bPointer;			// �͊o�|�C���^�ł��邩�ǂ���
	int doSim;				// �ߖT�ł��邩�ǂ��� 0:�ߖT�łȂ��C1:�͂��߂ċߖT�C2:�p�����ċߖT

	SpatialVector b;		///< �\���V�~�����[�V�����ŋ��߂����r���e�B�i�d�͓��̒萔���j
	SpatialVector curb;		///< �ʏ�V�~�����[�V�����łł��萔��
	SpatialVector lastb;	///< �O��̗\���V�~�����[�V�����ŋ��߂��萔��
};

class PHSolidForHaptic : public PHSolidForHapticSt, public PHSolidForHapticSt2, public UTRefCount{  
public:
	PHSolid localSolid;		// sceneSolid�̃N���[��

	// �Փ˔���p�̈ꎞ�ϐ�
	int NLocalFirst;		// �͂��߂ċߖT�ɂȂ�͊o�|�C���^�̐��i�Փ˔���ŗ��p�j
	int NLocal;				// �ߖT�ȗ͊o�|�C���^�̐��i�Փ˔���ŗ��p�j
	PHSolidForHaptic();
	PHSolid* GetLocalSolid(){ return &localSolid; }
	void AddForce(Vec3d f);
	void AddForce(Vec3d f, Vec3d r);
};
class PHSolidsForHaptic : public std::vector< UTRef< PHSolidForHaptic > >{};

//----------------------------------------------------------------------------
// PHShapePairForHaptic
class PHSolidPairForHaptic;
class PHShapePairForHaptic : public CDShapePair{
public:	
	// 0:solid�A1:pointer
	// Vec3d normal�͍��̂���͊o�|�C���^�ւ̖@���x�N�g��
	Posed lastShapePoseW[2];	///< �O��̌`��p��
	Vec3d lastClosestPoint[2];	///< �O��̋ߖT�_(���[�J�����W)
	Vec3d lastNormal;			///< �O��̋ߖT���̂̒񎦖ʂ̖@��
	std::vector< Vec3d > intersectionVertices; ///< �ڐG�̐ς̒��_(���[�J�����W)
	std::vector< UTRef< PHIr > > irs;

	PHShapePairForHaptic();
	PHShapePairForHaptic(const PHShapePairForHaptic& s);
	/// �ڐG����D�ߖT�_�΂��펞�X�V
	bool Detect(unsigned ct, const Posed& pose0, const Posed& pose1);
	/// �ڐG���̔���
	int OnDetect(unsigned ct, const Vec3d& center0);
	bool AnalyzeContactRegion();
	bool CompIntermediateRepresentation(Posed curShapePoseW[2], double t, bool bInterpolatePose, bool bMultiPoints);
};

//----------------------------------------------------------------------------

struct PHSolidPairForHapticSt{
	Vec3d force;			///< �͊o�|�C���^�����̍��̂ɉ������
	Vec3d torque;			///< �͊o�|�C���^�����̍��̂ɉ�����g���N

	Posed interpolationPose;	///< ���̂̕�Ԏp��
	Posed lastInterpolationPose;
	Posed initialRelativePose;	///< �ڐG�J�n���̑��Έʒu�p��
	Posed relativePose;			///< �ڐG���̑��Έʒu�p��

	int contactCount;
	Vec3d vibrationVel;
	enum FrictionState{
		FREE,
		FIRST,
		STATIC,
		DYNAMIC,
	} frictionState;
};

class PHSolidPairForHaptic : public PHSolidPairForHapticSt, public PHSolidPair< PHShapePairForHaptic, PHHapticEngine >, public Object{
public:
	int solidID[2];
	
	int inLocal;	// 0:NONE, 1:in local first, 2:in local
	TMatrixRow<6, 3, double> A;		// LocalDynamics�Ŏg���A�N�Z�������X
	TMatrixRow<6, 6, double> Minv;  // LocalDynamics6D�Ŏg���A�N�Z�������X
	
	PHSolidPairForHaptic();
	PHSolidPairForHaptic(const PHSolidPairForHaptic& s);
	/// ���������m���ꂽ��̏���
	virtual bool Detect(engine_type* engine, unsigned int ct, double dt);
	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt);	///< ���������m���ꂽ�Ƃ��̏���
	virtual PHIrs CompIntermediateRepresentation(PHSolid* curSolid[2], double t, bool bInterpolatePose, bool bPoints);
	virtual bool CompFrictionIntermediateRepresentation(PHShapePairForHaptic* sp);
	virtual bool CompFrictionIntermediateRepresentation2(PHShapePairForHaptic* sp);
};
class PHSolidPairsForHaptic : public UTCombination< UTRef<PHSolidPairForHaptic> >{};

//----------------------------------------------------------------------------
// PHHapticEngineImp
class PHHapticRender;
class PHHapticEngineImp : public SceneObject{
	SPR_OBJECTDEF_NOIF(PHHapticEngineImp);
public:
	PHHapticEngine* engine;
	PHHapticEngineImp(){}
	virtual void Step1(){};
	virtual void Step2(){};
	virtual void StepHapticLoop(){};

	double GetPhysicsTimeStep();
	double GetHapticTimeStep();

	int NHapticPointers();
	int NHapticSolids();
	PHHapticPointer* GetHapticPointer(int i);
	PHSolidForHaptic* GetHapticSolid(int i);
	PHSolidPairForHaptic* GetSolidPairForHaptic(int i, int j);
	PHHapticPointers* GetHapticPointers();
	PHSolidsForHaptic* GetHapticSolids();
	PHSolidPairsForHaptic* GetSolidPairsForHaptic();
	PHHapticRender* GetHapticRender();

	///< �f�o�b�N�p�V�~�����[�V�������s
	virtual void StepSimulation();
};

//----------------------------------------------------------------------------
// PHHapticEngine
class PHHapticEngine : public PHHapticEngineDesc, public PHContactDetector< PHShapePairForHaptic, PHSolidPairForHaptic, PHHapticEngine >{
public:
	SPR_OBJECTDEF1(PHHapticEngine, PHEngine);
	ACCESS_DESC(PHHapticEngine);
	UTRef< PHHapticEngineImp > engineImp;
	std::vector< UTRef<PHHapticEngineImp> > engineImps;
	UTRef< PHHapticRender > hapticRender;
	PHHapticPointers hapticPointers;
	PHSolidsForHaptic hapticSolids;
	typedef std::vector< HIBaseIf* > HIBaseIfs;
	HIBaseIfs humanInterfaces;

	struct Edge{ Vec3f min, max; };
	std::vector< Edge > edges;

	bool bHapticEngine;
	bool bPhysicStep;
	HapticEngineMode engineMode;
	PHHapticEngine();


	//-------------------------------------------------------------------
	// API�̎���
	///< �͊o�񎦌v�Z��ON/OFF
	void EnableHapticEngine(bool b){ bHapticEngine = b; }
	///< �G���W�����[�h�̑I��
	void SetHapticEngineMode(HapticEngineMode mode);
	///< �����_�����O���[�h�̑I��
	PHHapticRenderIf* GetHapticRender();
	///< �͊o�|�C���^�̐���Ԃ�
	int NHapticPointers(){ return (int)hapticPointers.size(); }
	///< �͊o�|�C���^�ւ̃|�C���^��Ԃ�
	PHHapticPointer* GetHapticPointer(int i){ return hapticPointers[i]; }

	//-------------------------------------------------------------------
	// PHHapticEngine�̎���
	///< �V�~�����[�V�������[�v�̍X�V�iPHScene::Integrate()����R�[���j
	virtual void Step(){ if(bHapticEngine && bPhysicStep) engineImp->Step1(); }
	virtual void Step2(){ if(bHapticEngine && bPhysicStep) engineImp->Step2(); }
	///< �͊o���[�v�̍X�V	
	virtual void StepHapticLoop(){ if(bHapticEngine) engineImp->StepHapticLoop(); }

	///< �͊o�����_�����O�p�̏Փ˔���J�n
	virtual void StartDetection();
	///< BBox�̌������X�V
	void UpdateEdgeList();
	///< ���鍄�̂̋ߖT�̍��̂�AABB�ł݂���irange��BBox�������range�������L����
	void Detect(PHHapticPointer* pointer);
	int GetPriority() const { return SGBP_HAPTICENGINE1; }
	///< ���̂̒ǉ�
	bool AddChildObject(ObjectIf* o);
	///< ���̂̍폜
	bool DelChildObject(ObjectIf* o);
	///< ShapePair�̍X�V
	void UpdateShapePairs(PHSolid* solid);
	///< �ڐG���[�h�̕ύX
	virtual void SetContactMode();

	///< �f�o�b�N�p�V�~�����[�V�������s
	///�iPHScene::Step�̕ς��ɌĂԁj
	virtual void StepSimulation(){ engineImp->StepSimulation(); }

};

// PHScene����Step()��2��ĂԂ��߂̋[���N���X
class PHHapticPseudEngine : public PHEngine{
public:
	SPR_OBJECTDEF_NOIF(PHHapticPseudEngine);
	UTRef< PHHapticEngine > engine;
	int GetPriority() const { return SGBP_HAPTICENGINE2; }
	virtual void Step(){ engine->Step2(); }
};


}	//	namespace Spr
#endif
