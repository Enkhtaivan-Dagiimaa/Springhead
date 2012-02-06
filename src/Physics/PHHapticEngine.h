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

using namespace PTM;
namespace Spr{;

//----------------------------------------------------------------------------
// PHSolidForHaptic
struct PHSolidForHapticSt{
	PHSolid localSolid;		// sceneSolid�̃N���[��
};

class PHSolidForHaptic : public PHSolidForHapticSt, public UTRefCount{  
public:
	PHSolid* sceneSolid;	// PHScene��������
	
	bool bPointer;			// �͊o�|�C���^�ł��邩�ǂ���
	int doSim;				// �ߖT�ł��邩�ǂ��� 0:�ߖT�łȂ��C1:�͂��߂ċߖT�C2:�p�����ċߖT

	SpatialVector b;		///< �\���V�~�����[�V�����ŋ��߂����r���e�B�i�d�͓��̒萔���j
	SpatialVector curb;		///< �ʏ�V�~�����[�V�����łł��萔��
	SpatialVector lastb;	///< �O��̗\���V�~�����[�V�����ŋ��߂��萔��

	// �Փ˔���p�̈ꎞ�ϐ�
	int NLocalFirst;		// �͂��߂ċߖT�ɂȂ�͊o�|�C���^�̐��i�Փ˔���ŗ��p�j
	int NLocal;				// �ߖT�ȗ͊o�|�C���^�̐��i�Փ˔���ŗ��p�j
	PHSolidForHaptic();
	PHSolidForHaptic(const PHSolidForHaptic& s);
};
class PHSolidsForHaptic : public std::vector< UTRef< PHSolidForHaptic > >{};

//----------------------------------------------------------------------------
// PHShapePairForHaptic
class PHSolidPairForHaptic;
class PHShapePairForHaptic : public CDShapePair{
public:	
	//PHSolidPairForHaptic* solidPair;
	// 0:solid�A1:pointer
	// Vec3d normal�͍��̂���͊o�|�C���^�ւ̖@���x�N�g��
	Posed lastShapePoseW[2];	///< �O��̌`��p��
	Vec3d lastClosestPoint[2];	///< �O��̋ߖT�_(���[�J�����W)
	Vec3d lastNormal;			///< �O��̋ߖT���̂̒񎦖ʂ̖@��
	//double sectionDepth;
	//double lastSectionDepth;
	std::vector< Vec3d > intersectionVertices; ///< �ڐG�̐ς̒��_(���[�J�����W)
	//std::vector< Vec3d > pointerSection;	///< �|�C���^�̐ڐG���_(���[�J�����W)
	//std::vector< Vec3d > solidSection;		///< ���̂̐ڐG���_(���[�J�����W)

	PHShapePairForHaptic();
	PHShapePairForHaptic(const PHShapePairForHaptic& s);
	/// �ڐG����D�ߖT�_�΂��펞�X�V
	bool Detect(unsigned ct, const Posed& pose0, const Posed& pose1);
	/// �ڐG���̔���
	int OnDetect(unsigned ct, const Vec3d& center0);
	bool AnalyzeContactRegion();
	bool CompIntermediateRepresentation(PHIrs &irs, Posed curShapePoseW[2], double t, bool bInterpolatePose, bool bPoints);
};

//----------------------------------------------------------------------------
// PHSolidPairForHaptic
//struct ImpulsePoint{
//	Vec3d contactPointW;
//	Vec3d impulse;
//};

struct PHSolidPairForHapticSt{
	Vec3d test_force;				///< �\���V�~�����[�V�����Ŏg���e�X�g��
	Vec3d test_torque;				///< �\���V�~�����[�V�����Ŏg���e�X�g�g���N
	//Vec3d impulse;					///< �����v���Z�X��1�X�e�b�v�I���܂łɗ͊o�|�C���^���������͐�
	//std::vector< ImpulsePoint > impulsePoints;

	Posed interpolationPose;	///< ���̂̕�Ԏp��
	Posed lastInterpolationPose;
	Posed initialRelativePose;	///< �ڐG�J�n���̑��Έʒu�p��
	Posed relativePose;			///< �ڐG���̑��Έʒu�p��
	enum FrictionState{
		FREE,
		STATIC,
		DYNAMIC,
	} frictionState;
};

class PHSolidPairForHaptic : public PHSolidPairForHapticSt, public PHSolidPair< PHShapePairForHaptic, PHHapticEngine >, public Object{
public:
	//typedef PHSolidPair<PHShapePairForHaptic, PHHapticEngine> base_type;
	//typedef base_type::shapepair_type shapepair_type;
	//typedef base_type::engine_type engine_type;
	int solidID[2];
	
	int inLocal;	// 0:NONE, 1:in local first, 2:in local
	struct Accelerance{
		Vec3d force;					// LocalDynamics�Ŏg����
		TMatrixRow<6, 3, double> A;		// LocalDynamics�Ŏg���A�N�Z�������X
		std::vector< Vec3d > f;			// LocalDynamics6D�Ŏg����
		TMatrixRow<6, 6, double> Minv;  // LocalDynamics6D�Ŏg���A�N�Z�������X
	} accelerance;
	
	PHSolidPairForHaptic();
	PHSolidPairForHaptic(const PHSolidPairForHaptic& s);
	/// ���������m���ꂽ��̏���
	virtual bool Detect(engine_type* engine, unsigned int ct, double dt);
	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt);	///< ���������m���ꂽ�Ƃ��̏���
	virtual PHIrs CompIntermediateRepresentation(PHSolid* curSolid[2], double t, bool bInterpolatePose, bool bPoints);
};
class PHSolidPairsForHaptic : public UTCombination< UTRef<PHSolidPairForHaptic> >{};

//----------------------------------------------------------------------------
// PHHapticEngineImp
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

	///< �f�o�b�N�p�V�~�����[�V�������s
	virtual void StepSimulation(){};
};

//----------------------------------------------------------------------------
// PHHapticEngine


struct PHHapticEngineDesc{
	bool bHaptic;
	PHHapticEngineDesc();
};
class PHHapticEngine : public PHHapticEngineDesc, public PHContactDetector< PHShapePairForHaptic, PHSolidPairForHaptic, PHHapticEngine >{
public:
	SPR_OBJECTDEF1(PHHapticEngine, PHEngine);
	ACCESS_DESC(PHHapticEngine);
	UTRef< PHHapticEngineImp > engineImp;
	std::vector< UTRef<PHHapticEngineImp> > engineImps;
	PHHapticPointers hapticPointers;
	PHSolidsForHaptic hapticSolids;
	typedef std::vector< HIBaseIf* > HIBaseIfs;
	HIBaseIfs humanInterfaces;

	struct Edge{ Vec3f min, max; };
	std::vector< Edge > edges;

	enum EngineType{
		NONE,
		MULTI_THREAD,
		SINGLE_THREAD,
	} engineType;


	PHHapticEngine();

	///< �V�~�����[�V�������[�v�̍X�V�iPHScene::Integrate()����R�[���j
	virtual void Step(){ if(bHaptic) engineImp->Step1(); }
	virtual void Step2(){ if(bHaptic) engineImp->Step2(); }
	///< �͊o���[�v�̍X�V	
	virtual void StepHapticLoop(){ if(bHaptic) engineImp->StepHapticLoop(); }

	///< �͊o�񎦌v�Z��ON/OFF
	void EnableHaptic(bool b){ bHaptic = b; }
	///< �����_�����O���[�h�̑I��
	void SetHapticEngineType(EngineType e);
	///< �͊o�|�C���^�̐���Ԃ�
	int NHapticPointers(){ return (int)hapticPointers.size(); }
	///< �͊o�|�C���^�ւ̃|�C���^��Ԃ�
	PHHapticPointer* GetHapticPointer(int i){ return hapticPointers[i]; }

	///< �͊o�|�C���^�̏�Ԃ̍X�V
	virtual void UpdateHapticPointer();
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
	void UpdateShapePairs(PHSolid* solid);

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
