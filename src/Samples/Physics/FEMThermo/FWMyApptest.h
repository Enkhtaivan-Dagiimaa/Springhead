#ifndef FWAPPSAMPLE_H								// �w�b�_�t�@�C������鎞�̂��܂��Ȃ�
#define FWAPPSAMPLE_H								// ����

#include <Springhead.h>								// Springhead��If�^���g�����߂̃w�b�_�t�@�C��
#include "../../SampleApp.h"

class FWMyApptest : public SampleApp{
public:
	/// �y�[�WID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// �A�N�V����ID
	enum {
		ID_BOX,
	};

public:	
	//�R���X�g���N�^
	FWMyApptest(){
		appName = "FEMThermo";

		AddAction(MENU_MAIN, ID_BOX, "drop object");
		AddHotKey(MENU_MAIN, ID_BOX, 'd');
	}
	//�f�X�g���N�^
	~FWMyApptest(){}

	virtual void BuildScene(){
		UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();
		GetSdk()->LoadScene("scene.spr", import);			// �t�@�C���̃��[�h
	}

	//1�X�e�b�v�̃V�~�����[�V����	// �^�C�}�R�[���o�b�N�֐��D�^�C�}�����ŌĂ΂��
	virtual void OnStep() {
		SampleApp::OnStep();
	}

	// �`��֐��D�`��v���������Ƃ��ɌĂ΂��
	virtual void OnDraw(GRRenderIf* render) {
		SampleApp::OnDraw(render);

		std::ostringstream sstr;
		sstr << "NObj = " << phScene->NSolids();
		render->DrawFont(Vec2f(-21, 23), sstr.str());
	}

	//�A�N�V��������
	virtual void OnAction(int menu, int id){
		if(menu == MENU_MAIN){
			Vec3d v, w(0.0, 0.0, 0.2), p(0.5, 20.0, 0.0);
			Quaterniond q = Quaterniond::Rot(Rad(30.0), 'y');

			if(id == ID_BOX){
				Drop(SHAPE_BOX, GRRenderIf::RED, v, w, p, q);
				message = "box dropped.";
			}
		}
		SampleApp::OnAction(menu, id);
	}

};

#endif