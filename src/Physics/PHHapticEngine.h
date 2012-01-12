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
class PHSolidsForHaptic;
class PHSolidPairForHaptic;
typedef UTCombination< UTRef<PHSolidPairForHaptic> > PHSolidPairsForHaptic;
class PHHapticPointer : public PHSolid{
	SPR_OBJECTDEF(PHHapticPointer);
protected:
	int pointerID;
	int pointerSolidID;
	float localRange;
	double worldScale;
	double posScale;
	Posed defaultPose;
public:
	bool bDebugControl;
	bool bForce;
	bool bVibration;
	std::vector<int> neighborSolidIDs;
	PHSolid* hiSolid;
	HIBaseIf* humanInterface;
	PHHapticPointer(){ 
		hiSolid = DBG_NEW PHSolid();
		localRange = 1.0; 
		worldScale = 1.0;
		posScale = 1.0;
		bDebugControl = false;
		bForce = false;
		bVibration = false;
	}

	void	SetPointerID(int id){ pointerID = id; }
	int		GetPointerID(){ return pointerID; }
	void	SetSolidID(int id){ pointerSolidID = id; }
	int		GetSolidID(){ return pointerSolidID; }
	void	SetHumanInterface(HIBaseIf* hi){ humanInterface = hi; }
	void	UpdateInterface(float dt);
	void	UpdateHapticPointer();
	void	SetForce(SpatialVector f);
	void	HapticRendering(PHSolidsForHaptic* hsolids, PHSolidPairsForHaptic* sps, double loopCount);
	void	MultiPointRendering(PHSolidsForHaptic* hsolids, PHSolidPairsForHaptic* sps, double loopCount);
	void	SetLocalRange(float r){ localRange = r; } 
	float	GetLocalRange(){ return localRange; }
	void	SetPosScale(double scale){ posScale = scale; }
	double	GetPosScale(){ return posScale; }
	void	SetWorldScale(double scale){ worldScale = scale; }
	double	GetWorldScale(){ return worldScale; }
	void	SetDefaultPose(Posed p){ defaultPose = p; }
	Posed	GetDefaultPose(){ return defaultPose; }
	void	EnableForce(bool b){ bForce = b; }
	void	DisplayVibration(bool b){ bVibration = b; }
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

//----------------------------------------------------------------------------
// PHSolidForHaptic
class PHSolidForHaptic : public UTRefCount{  
	//SPR_OBJECTDEF(PHSolidForHaptic);
public:
	PHSolid* sceneSolid;
	PHSolid localSolid;
	bool bPointer;			// �͊o�|�C���^�ł��邩�ǂ���
	int NLocalFirst;		// �͂��߂ċߖT�ɂȂ�͊o�|�C���^�̐��i�Փ˔���ŗ��p�j
	int NLocal;				// �ߖT�ȗ͊o�|�C���^�̐��i�Փ˔���ŗ��p�j
	int doSim;				// �ߖT�ł��邩�ǂ��� 0:�ߖT�łȂ��C1:�͂��߂ċߖT�C2:�p�����ċߖT

	SpatialVector b;				///< �\���V�~�����[�V�����ŋ��߂����r���e�B�i�d�͓��̒萔���j
	SpatialVector curb;				///< �ʏ�V�~�����[�V�����łł��萔��
	SpatialVector lastb;			///< �O��̗\���V�~�����[�V�����ŋ��߂��萔��
	PHSolidForHaptic(){
		bPointer = false;
		NLocalFirst = 0;	
		NLocal = 0;	doSim = 0;
	}
};
class PHSolidsForHaptic : public std::vector< UTRef< PHSolidForHaptic > >{};


//----------------------------------------------------------------------------
// PHShapePairForHaptic


class PHSolidPairForHaptic;
class PHShapePairForHaptic : public CDShapePair{
public:	
	//PHSolidPairForHaptic* solidPair;
	// 0:pointer�A1:solid
	// Vec3d normal�͗͊o�|�C���^���獄�̂ւ̖@���x�N�g��
	Posed lastShapePoseW[2];	///< �O��̌`��p��
	Vec3d lastClosestPoint[2];	///< �O��̋ߖT�_(���[�J�����W)
	Vec3d lastNormal;			///< �O��̋ߖT���̂̒񎦖ʂ̖@��
	double sectionDepth;
	double lastSectionDepth;
	std::vector< Vec3d > intersectionVertices; ///< �ڐG�̐ς̒��_(���[�J�����W)
	std::vector< Vec3d > pointerSection;	///< �|�C���^�̐ڐG���_(���[�J�����W)
	std::vector< Vec3d > solidSection;		///< ���̂̐ڐG���_(���[�J�����W)

	PHShapePairForHaptic(){}
	/// �ڐG����D�ߖT�_�΂��펞�X�V
	bool Detect(unsigned ct, const Posed& pose0, const Posed& pose1);
	/// �ڐG���̔���
	int OnDetect(unsigned ct, const Vec3d& center0);
	bool AnalyzeContactRegion();
};

//----------------------------------------------------------------------------
// PHSolidPairForHaptic
struct ImpulsePoint{
	Vec3d contactPointW;
	Vec3d impulse;
};
class PHHapticEngineImp;
class PHSolidPairForHaptic : public PHSolidPair< PHShapePairForHaptic, PHHapticEngine >, public Object{
public:
	typedef PHSolidPair<PHShapePairForHaptic, PHHapticEngine> base_type;
	typedef base_type::shapepair_type shapepair_type;
	typedef base_type::engine_type engine_type;

	PHHapticPointer* pointer;
	int inLocal;	// 0:NONE, 1:in local first, 2:in local
	struct Accelerance{
		Vec3d force;					// LocalDynamics�Ŏg����
		TMatrixRow<6, 3, double> A;		// LocalDynamics�Ŏg���A�N�Z�������X
		std::vector< Vec3d > f;			// LocalDynamics6D�Ŏg����
		TMatrixRow<6, 6, double> Minv;  // LocalDynamics6D�Ŏg���A�N�Z�������X
	} accelerance;
	
	Vec3d test_force;				///< �\���V�~�����[�V�����Ŏg���e�X�g��
	Vec3d test_torque;				///< �\���V�~�����[�V�����Ŏg���e�X�g�g���N
	Vec3d impulse;					///< �����v���Z�X��1�X�e�b�v�I���܂łɗ͊o�|�C���^���������͐�

	std::vector< ImpulsePoint > impulsePoints;

	/// ���������m���ꂽ��̏���
	virtual bool Detect(engine_type* engine, unsigned int ct, double dt);
	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt);	///< ���������m���ꂽ�Ƃ��̏���
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
	int NHapticSolids();
	PHHapticPointer* GetHapticPointer(int i);
	PHSolidForHaptic* GetHapticSolid(int i);
	PHSolidPairForHaptic* GetSolidPairForHaptic(int i, int j);
	PHHapticPointers* GetHapticPointers();
	PHSolidsForHaptic* GetHapticSolids();

//	PHHapticPointers* GetHapticPointers();
//	PHSolidForHaptic** GetHapticSolids();

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

	///< �V�~�����[�V�������[�v�̍X�V�iPHScene::Integrate()����R�[���j
	virtual void Step(){ if(bHaptic) renderImp->Step(); }
	///< �͊o���[�v�̍X�V	
	virtual void StepHapticLoop(){ if(bHaptic) renderImp->StepHapticLoop(); }

	///< �͊o�񎦌v�Z��ON/OFF
	void EnableHaptic(bool b){ bHaptic = b; }
	///< �����_�����O���[�h�̑I��
	void SetRenderMode(RenderMode r);
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
	int GetPriority() const {return SGBP_HAPTICENGINE;}
	///< ���̂̒ǉ�
	bool AddChildObject(ObjectIf* o);
	///< ���̂̍폜
	bool DelChildObject(ObjectIf* o);
	void UpdateShapePairs(PHSolid* solid);

	///< �f�o�b�N�p�V�~�����[�V�������s
	///�iPHScene::Step�̕ς��ɌĂԁj
	virtual void StepSimulation(){ renderImp->StepSimulation(); }

};

}	//	namespace Spr
#endif
