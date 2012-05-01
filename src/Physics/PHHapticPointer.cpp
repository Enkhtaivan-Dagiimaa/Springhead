#include <Physics/PHHapticPointer.h>
#include <Physics/PHHapticEngine.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticPointer
PHHapticPointer::PHHapticPointer(){
	reflexSpring = 300;
	reflexDamper = 0.1f;
	localRange = 1.0; 
	posScale = 1.0;
	bDebugControl = false;
	bForce = false;
	bFriction = true;
	bMultiPoints = true;
	bVibration = false;
	rotaionalWeight = 1e5;
	dynamical = false;
	hapticRenderMode = CONSTRAINT;
}
PHHapticPointer::PHHapticPointer(const PHHapticPointer& p){
	*this = p;
}

void PHHapticPointer::UpdateHumanInterface(Posed pose, SpatialVector vel){
	if(bDebugControl) return;
	// HumanInterface�����Ԃ��擾
	double s = GetPosScale();
	vel.v() = vel.v() * s;
	pose.Pos() = pose.Pos() * s;
	hiSolid.SetVelocity(vel.v());
	hiSolid.SetAngularVelocity(vel.w());
	hiSolid.SetPose(GetDefaultPose() * pose);
}

void PHHapticPointer::UpdateDirect(){
	// �|�C���^�̏�ԍX�V
	SetVelocity(hiSolid.GetVelocity());
	SetAngularVelocity(hiSolid.GetAngularVelocity());
	SetFramePosition(hiSolid.GetFramePosition());
	SetOrientation(hiSolid.GetOrientation());
	
	// �v���L�V�̏�Ԃ̕ۑ��ƍX�V
	lastProxyPose = proxyPose;
	proxyPose = GetPose();
	targetProxy = GetPose();
}

void PHHapticPointer::AddHapticForce(SpatialVector f){
	hapticForce += f;
}


}