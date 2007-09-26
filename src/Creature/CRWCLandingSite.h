#pragma once
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <Physics/PHScene.h>


using namespace Spr;

class CRWCLandingSite
{
	Vec3d change;
	Vec3d PositionOfWholeBody;
	Vec3d PositionOfHip;
	Vec3d VelocityOfWholeBody;
    Vec3d CurrentLandingSite;
	Vec3d NextLandingSite;
	Vec3d NextLandingSiteWithoutConstraint;
	Vec3d PositionOfSwingFoot;
	Vec3d VelocityOfSwingFoot;
    double timeleft;

	double DoubleSupportDistanceX;
    double DoubleSupportDistanceZ;

	double LocalX;
	double LocalZ;
	double LocalBodyVelocityX;
	double LocalBodyVelocityZ;
	double MaxFootAcceleration; 
	double MaxFootSpeed;

	////�p�����[�^�Q//////////
	double T0;
	double MaxStride;
	double Tc;
	double TimeStep;
	double VelocityX;
	double VelocityZ;
	double StrideX;
	double StrideZ;
	double Ldx;      //�����x�����Ԃɐi��x�����̋����̊�l
	double Ldz;      //�����x�����Ԃɐi��z�����̋����̊�l
	//////////////////////////

	bool LF;
	bool RF;
    bool LandAble;

	double TargetAngle;
	double CurrentDirection;
	double pi;

	double npx;
	double nvx;
	double npz;
	double nvz;

	double kx;
	double kz;
	double ax;     //�ʒu�̃Y���ɑ΂���d�� N = a(x_d - xf)^2 + b(v_d - vf)^2 
	double bx;     //���x�̃Y���ɑ΂���d��
	double cx;     //�o�����X�ɑ΂���d��
	double az;     //�ʒu�̃Y���ɑ΂���d�� N = a(x_d - xf)^2 + b(v_d - vf)^2 
	double bz;     //���x�̃Y���ɑ΂���d��
	double cz;     //�o�����X�ɑ΂���d��
	double FootLength;
	double MaxFootLength;
	double MaxRoGround;
	double MaxAcFront;    //�V�r�̉����x�̌��E�l�A���ʕ���
	double MaxAcSide;     //�V�r�̉����x�̌��E�l�A���ʕ���
	double MaxAcBack;     //�V�r�̉����x�̌��E�l�A�w�ʕ���
	double MaxSpFront;    //�V�r�̑��x�̌��E�l�A���ʕ���
	double MaxSpSide;     //�V�r�̑��x�̌��E�l�A���ʕ���
	double MaxSpBack;     //�V�r�̑��x�̌��E�l�A�w�ʕ���
	double MaxRoLandingSiteFront;
	double MaxRoLandingSiteSide;
	double MaxRoLandingSiteBack;

    double CalcPosition(double LandingSite, double cx, double cv);
	double CalcVelocity(double LandingSite, double cx, double cv);
	double CalcNextVelocity(double nextlandingsite, double np, double nv);
    double CalcWeightForBalanceX(double cz);
	double CalcWeightForBalanceZ(double cx);
	double CalcBasicLandingSiteX(double cpx, double cvx);
	double CalcLandingSiteX(double cpx, double cvx, double cz, double normallanding);
	double CalcBasicLandingsiteZL(double cpz, double cvz);
	double CalcBasicLandingsiteZR(double cpz, double cvz);
    double CalcLandingsiteZL(double cpz, double cvz, double cx, double normallanding);
    double CalcLandingsiteZR(double cpz, double cvz, double cx, double normallanding);
	double CalcDoubleSupportDistanceX(double nsslsx);
	double CalcDoubleSupportDistanceZ(double nsslsz);
	double CalcLocalX(double xb, double zb, double xt, double zt, double theta);
	double CalcLocalZ(double xb, double zb, double xt, double zt, double theta);
	double CalcLocalVX(double vx, double vz, double theta);
	double CalcLocalVZ(double vx, double vz, double theta);
	void CalcNextLandingSiteWithoutConstraint(void);

public:
	CRWCLandingSite(Vec3d il,bool lf);
	~CRWCLandingSite(void);
	Vec3d CalcNextLandingSite(void);
	void Init(void);
	double GetDoubleSupportDistanceX(void) {return DoubleSupportDistanceX;}
	double GetDoubleSupportDistanceZ(void) {return DoubleSupportDistanceZ;}
	double GetLandingVelocityX(void) {return nvx;}
	double GetLandingVelocityZ(void) {return nvz;}
	void UpdateState(Vec3d c, Vec3d p, Vec3d p2, Vec3d v, double tl, Vec3d cls, Vec3d psf, Vec3d vsf, double Theta, double cd);
    void ChangeSupportLegs(bool lf);
	bool GetLandAble(void) {return LandAble;}
	void SetParamLd(double ldx, double ldz) {Ldx = ldx; Ldz = ldz;}
	void SetParamT0(double t) {T0 = t;}
	void SetParamMaxStride(double s) {MaxStride = 2.0*s;}
	void SetParamTc(double t) {Tc = t;}
	void SetParamTimeStep(double ts) {TimeStep = ts;}
	void SetParamVelocityX(double v) {VelocityX = v;}
	void SetParamVelocityZ(double v) {VelocityZ = v;}
	void SetParamStrideX(double s) {StrideX = s;}
	void SetParamStrideZ(double s) {StrideZ = s;}
	void SetParamFootLength(double fl) {FootLength = fl;}
	void SetParamMaxFootAccerelations(double maf, double mas, double mab) {
		MaxAcFront = maf;
		MaxAcSide = mas;
		MaxAcBack = mab;
	}
	void SetParamMaxFootSpeeds(double msf, double mss, double msb) {
		MaxSpFront = msf;
		MaxSpSide = mss;
		MaxSpBack = msb;
	}
	void SetParamMaxFootLength(double mfl) {MaxFootLength = mfl;}
	void SetParamMaxRoGround(double RoGround) {MaxRoGround = RoGround;}
	void SetParamMaxRoLandingSite(double RoFront, double RoSide, double RoBack) {
		MaxRoLandingSiteFront = RoFront;
		MaxRoLandingSiteSide = RoSide;
		MaxRoLandingSiteBack = RoBack;
	}

	void SetBalanceParams(double AX, double BX, double CX, double AZ, double BZ, double CZ, double KX , double KZ){
		ax = AX;
		bx = BX;
		cx = CX;
		az = AZ;
		bz = BZ;
		cz = CZ;
		kx = KX;
		kz = KZ;
	}

};