#ifndef SPR_FWSCENEIF_H
#define SPR_FWSCENEIF_H

#include <Foundation/SprScene.h>
#include <Physics/SprPHScene.h>
#include <Graphics/SprGRScene.h>
#include <Graphics/SprGRRender.h>

namespace Spr{

	struct FWObjectDesc;
	struct FWObjectIf;

	struct FWSceneDesc{
	};

	class HIForceDevice6D;

	struct FWSceneIf : SceneIf {
		IF_DEF(FWScene);
		
		/// Physics���W���[���̃V�[�����擾����
		virtual PHSceneIf*	GetPHScene() = 0;
		/** @brief Physics���W���[���̃V�[����ݒ肷��
			FWScene�̍쐬�Ɠ����Ɏ����I�ɍ쐬�����̂ŁA�ʏ�͌Ăяo���K�v�͖����D
		*/	
		virtual void		SetPHScene(PHSceneIf* s) = 0;

		/// Graphics���W���[���̃V�[�����擾����
		virtual GRSceneIf*	GetGRScene() = 0;
		/** @brief Graphics���W���[���̃V�[�����擾�E�ݒ肷��
			FWScene�̍쐬�Ɠ����Ɏ����I�ɍ쐬�����̂ŁA�ʏ�͌Ăяo���K�v�͖����D
		*/
		virtual void		SetGRScene(GRSceneIf* s) = 0;

		/** @brief Framework�I�u�W�F�N�g���쐬����
			PHSolid��GRFrame���֘A�t����I�u�W�F�N�g���쐬����D
			�����I�ɂ́A�܂�desc�ɂ��ƂÂ��ĐV����PHSolid�����GRFrame���A���ꂼ��
			PHSceneIf::CreateSolid�����GRSceneIf::CreateFrame�ɂ��쐬�����B
			���ɗ��҂��Q�Ƃ���FWObject���쐬����FWScene�ɓo�^�����B
		 */
		virtual FWObjectIf*	CreateObject(const PHSolidDesc& soliddesc, const GRFrameDesc& framedesc)=0;
		virtual int NObject()const=0;
		virtual FWObjectIf** GetObjects()=0;

		/** @brief �I�u�W�F�N�g�𓯊�����
			�eFWObject�ɑ΂��āAPHSolid�̈ʒu��GRFrame�ɔ��f������B
			�ʏ��FWSceneIf::Step�ɂ���ăV�~�����[�V���������s���ꂽ��ɌĂԁB
		 */
		virtual void Sync() = 0;

		/** @brief �V�~�����[�V���������s����
		 */
		virtual void Step() = 0;

		/** @brief �`�悷��
			@param grRender	�����_��
			@param debug	�f�o�b�O�`�悩
			debug��true�̏ꍇ�i�f�o�b�O�`��j�A�e���̂̏Փ˔���p�`��f�[�^�ɂ��
			�`�悪�s����B����Adebug��false�̏ꍇ�̓V�[���O���t�̌`��f�[�^��
			�}�e���A���A�e�N�X�`���𗘗p���ĕ`�悪�s����B
		 */
		virtual void Draw(GRRenderIf* grRender, bool debug=false) = 0;

		virtual void AddHumanInterface(HIForceDevice6D* d)=0;
	};

	FWSceneIf* SPR_CDECL CreateFWScene();
	FWSceneIf* SPR_CDECL CreateFWScene(const void* desc);
}

#endif
