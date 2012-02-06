#ifndef PH_HAPTICPOINTER_H
#define PH_HAPTICPOINTER_H

#include <Springhead.h>
#include <Physics/PHSolid.h>

namespace Spr{;

class PHSolidsForHaptic;
class PHSolidPairForHaptic;
class PHSolidPairsForHaptic;

struct PHIntermediateRepresentation{
	int solidID;		// �ǂ̍��̂ƐN�����Ă��邩
	double depth;		///< �N����
	Vec3d normal;		// �ʖ@��
	Vec3d r;			// �͊o�|�C���^�̏d�S����̐ڐG�_�܂ł̃x�N�g��
	Vec3d pointerPointW;	// �͊o�|���^�̐N���_(world)
	Vec3d pointerPointL;	// �͊o�|���^�̐N���_(world)
	Vec3d contactPointW;	// ���ԕ\���ʂ�vertex�𓊉e�����ʒu(world)
	Vec3d contactPointL;	
	Vec3d pointerPointVel;
	Vec3d contactPointVel;
	Posed interpolation_pose; // �`��̕�Ԏp��
	double f;			// �N�������v�Z�ŐڐG�_�����i�ړ��ɍ�p����傫��
	Vec3d force;		// ���̂ɗ^�����
	float mu;
	float mu0;
	PHSolidPairForHaptic* solidPair;
	PHIntermediateRepresentation(){
		solidID = -1;
		f = 0.0;
		solidPair = NULL;
	}
};
typedef PHIntermediateRepresentation PHIr;
typedef std::vector< PHIr > PHIrs;

//struct PHIntermediateRepresentationForShape{
//	PHIrs irs;
//};
//typedef PHIntermediateRepresentationForShape PHIrShape;
//typedef std::vector< PHIrShape > PHIrShapes;
//
//struct PHIntermediateRepresentationForSolid{
//	PHIrShapes irShapes;
//};
//typedef PHIntermediateRepresentationForSolid PHIrSolid;
//typedef std::vector< PHIrSolid > PHIrSolids;


//----------------------------------------------------------------------------
// PHHapticPointer
struct PHHapticPointerSt{
	PHIrs allIrs;
	Vec3d last_dr;
	Vec3d last_dtheta;
	Posed proxyPose;	// ���C�v�Z�p�̃v���L�V
	Posed lastProxyPose;
	Posed targetProxy;	// ���͌v�Z�p�̃v���L�V
};
class PHHapticPointer : public PHHapticPointerSt, public PHSolid{
	SPR_OBJECTDEF(PHHapticPointer);
protected:
	int pointerID;
	int pointerSolidID;
	float localRange;
	double worldScale;
	double posScale;
	Posed defaultPose;
	double rotaionalWeight;

public:
	bool bDebugControl;
	bool bForce;
	bool bVibration;
	std::vector<int> neighborSolidIDs;
	PHSolid hiSolid;
	HIBaseIf* humanInterface;
	PHHapticPointer();
	PHHapticPointer(const PHHapticPointer& p);

	void	SetPointerID(int id){ pointerID = id; }
	int		GetPointerID(){ return pointerID; }
	void	SetSolidID(int id){ pointerSolidID = id; }
	int		GetSolidID(){ return pointerSolidID; }
	void	SetHumanInterface(HIBaseIf* hi){ humanInterface = hi; }
	void	UpdateInterface(float dt);
	void	UpdateDirect();
	void	SetForce(SpatialVector f);
	void	ReflectRenderdForce2Solid(PHSolidsForHaptic* hsolids);
	void	SetLocalRange(float r){ localRange = r; } 
	float	GetLocalRange(){ return localRange; }
	void	SetPosScale(double scale){ posScale = scale; }
	double	GetPosScale(){ return posScale; }
	void	SetWorldScale(double scale){ worldScale = scale; }
	double	GetWorldScale(){ return worldScale; }
	void	SetRotationalWeight(double w){ rotaionalWeight = w; }
	double	GetRotationalWeight(){ return rotaionalWeight ; }
	void	SetDefaultPose(Posed p){ defaultPose = p; }
	Posed	GetDefaultPose(){ return defaultPose; }
	void	EnableForce(bool b){ bForce = b; }
	void	DisplayVibration(bool b){ bVibration = b; }
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

}
#endif