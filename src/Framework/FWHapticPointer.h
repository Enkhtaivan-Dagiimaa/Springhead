#ifndef FW_HAPTICPOINTER_H
#define FW_HAPTICPOINTER_H

#include <Springhead.h>
#include <Framework/SprFWHapticPointer.h>
#include <Physics/PHHapticPointer.h>
#include <Foundation/Object.h>

namespace Spr{;

class FWHapticPointer : public SceneObject{
SPR_OBJECTDEF(FWHapticPointer);
//ACCESS_DESC(FWHapticPointer);
protected:
	HIBaseIf* humanInterface;
	PHHapticPointerIf* hapticPointer;
	SpatialVector hapticForce;
public:
	void SetPHHapticPointer(PHHapticPointerIf* hpGlobal){ hapticPointer = hpGlobal; }
	PHHapticPointerIf* GetPHHapticPointer(){ return hapticPointer; }
	void SetHumanInterface(HIBaseIf* hi){ humanInterface = hi; }
	HIBaseIf* GetHumanInterface(){ return humanInterface; }
	/** @breif�͊o�C���^�t�F�[�X�����Ԃ��擾���APHHapticPointer�ɔ��f������
		@param hpLocal �}���`���[�g�̏ꍇ�͍����X�V����PHHapticPointer��ݒ�
		@param dt HumanInterface�̍X�V����
	 */
	void UpdateHumanInterface(PHHapticPointer* hpLocal, float dt);
	Vec3d GetHapticForce(){ return hapticForce.v(); }
	Vec3d GetHapticTorque(){ return hapticForce.w(); }
};


}
#endif