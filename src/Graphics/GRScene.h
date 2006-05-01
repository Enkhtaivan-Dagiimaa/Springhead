#ifndef GRSCENE_H
#define GRSCENE_H
#include <Graphics/SprGRScene.h>
#include <Foundation/Scene.h>
#include <Graphics/GRFrame.h>

namespace Spr {;


/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
*/
class SPR_DLL GRScene:public InheritScene<GRSceneIf, Scene>, public GRSceneDesc{
	OBJECTDEF(GRScene);
protected:
	///	����SDK
	GRSdkIf* sdk;
	///	���[�g�m�[�h
	GRFrame* world;
	///	���ׂẴt���[��
	typedef std::vector< UTRef<GRFrame> > GRFrames;
	GRFrames frames;
public:
	///	�R���X�g���N�^
	GRScene();
	GRScene(GRSdkIf* s, const GRSceneDesc& desc);
	void Init();
	///	�f�X�g���N�^
	~GRScene(){}

	GRSdkIf* GetSdk();

	///	�V�[������ɂ���D
	void Clear();
	GRFrameIf* CreateFrame(const GRFrameDesc& desc);

	ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
protected:
	void* GetDescAddress(){ return (GRSceneDesc*)this; }
};

}
#endif
