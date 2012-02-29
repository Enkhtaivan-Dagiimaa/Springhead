#ifndef PH_HAPTICPOINTER_H
#define PH_HAPTICPOINTER_H

#include <Springhead.h>
#include <Physics/PHSolid.h>

namespace Spr{;

//class PHSolidsForHaptic;
class PHSolidPairForHaptic;
//class PHSolidPairsForHaptic;
//class PHShapePairForHaptic;

struct PHIntermediateRepresentation :public UTRefCount{
	int solidID;				///< ���̂�ID(PHSolidsForHapitc��PHSolidPairForHaptic)�ƐN�����Ă��邩
	PHSolidPairForHaptic* solidPair;	///< PHSolidPairForHaptic�ւ̃|�C���^
	double depth;				///< �N����
	Vec3d normal;				///< �ʖ@��
	Vec3d r;					///< �͊o�|�C���^�̏d�S����̐ڐG�_�܂ł̃x�N�g��
	Vec3d pointerPointW;		///< �͊o�|���^�̐N���_(world)
	Vec3d contactPointW;		///< ���ԕ\���ʂ�vertex�𓊉e�����ʒu(world)
	Vec3d pointerPointVel;		///< �͊o�|�C���^���̐ڐG�_�̑��x(world)
	Vec3d contactPointVel;		///< ���̑��̐ڐG�_�̑��x(world)
	Posed interpolation_pose;	///< �`��̕�Ԏp��
	double f;					///< �N�������v�Z�ŐڐG�_�����i�ړ��ɍ�p����傫��
	float springK;					///< �o�l�W��
	float damperD;					///< �_���p�W��
	float mu;					///< �����C�W��
	float mu0;					///< �ő�Î~���C�W��(�ő�Î~���C�͖�����)
	PHIntermediateRepresentation(){
		solidID = -1;
		f = 0.0;
		solidPair = NULL;
	}
	bool operator < (const PHIntermediateRepresentation& s) const { return depth < s.depth; }
};
typedef PHIntermediateRepresentation PHIr;
typedef std::vector< PHIr* > PHIrs;

//----------------------------------------------------------------------------
// PHHapticPointer
struct PHHapticPointerSt{
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
	float reflexSpring, reflexDamper;
	float reflexSpringOri, reflexDamperOri;
	float localRange;
	double posScale;
	Posed defaultPose;
	double rotaionalWeight;
	SpatialVector hapticForce;

public:
	bool bDebugControl;
	bool bForce;
	bool bFriction;
	bool bVibration;
	std::vector<int> neighborSolidIDs;
	PHSolid hiSolid;
	HIBaseIf* humanInterface;
	PHHapticPointer();
	PHHapticPointer(const PHHapticPointer& p);

	//API
	void	SetHumanInterface(HIBaseIf* hi){ humanInterface = hi; }
	void	EnableForce(bool b){ bForce = b; }
	void	EnableFriction(bool b){ bFriction = b; }
	void	EnableVibration(bool b){ bVibration = b; }
	void	EnableDebugControl(bool b){ bDebugControl = b; }
	void	SetReflexSpring(float s){ reflexSpring = s; }
	float	GetReflexSpring(){ return reflexSpring; }
	void	SetReflexDamper(float d){ reflexDamper = d; }
	float	GetReflexDamper(){ return reflexDamper; }
	void	SetReflexSpringOri(float s){ reflexSpringOri = s; }
	float	GetReflexSpringOri(){ return reflexSpringOri; }
	void	SetReflexDamperOri(float d){ reflexDamperOri = d; }
	float	GetReflexDamperOri(){ return reflexDamperOri; }
	void	SetLocalRange(float r){ localRange = r; } 
	float	GetLocalRange(){ return localRange; }
	void	SetPosScale(double scale){ posScale = scale; }
	double	GetPosScale(){ return posScale; }
	void	SetRotationalWeight(double w){ rotaionalWeight = w; }
	double	GetRotationalWeight(){ return rotaionalWeight ; }
	void	SetDefaultPose(Posed p){ defaultPose = p; }
	Posed	GetDefaultPose(){ return defaultPose; }

	// Implementation
	void	SetPointerID(int id){ pointerID = id; }
	int		GetPointerID(){ return pointerID; }
	void	SetSolidID(int id){ pointerSolidID = id; }
	int		GetSolidID(){ return pointerSolidID; }
	void	UpdateInterface(float dt);
	void	UpdateDirect();
	void	AddHapticForce(SpatialVector f);
	void	DisplayHapticForce();
};
class PHHapticPointers : public std::vector< UTRef< PHHapticPointer > >{};

}
#endif