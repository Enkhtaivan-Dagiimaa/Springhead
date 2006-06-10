#ifndef GRSCENE_H
#define GRSCENE_H
#include <Graphics/SprGRScene.h>
#include <Foundation/Scene.h>
#include <Graphics/GRFrame.h>

namespace Spr {;

class GRCamera;
/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
*/
class SPR_DLL GRScene:public InheritScene<GRSceneIf, Scene>, public GRSceneDesc{
	OBJECT_DEF(GRScene);
protected:
	///	���[�g�m�[�h
	GRFrame* world;
	typedef std::vector< UTRef<GRFrame> > GRFrames;
	///	���ׂẴt���[��
	GRFrames frames;
	///	�J����
	UTRef<GRCamera> camera;
public:
	///	�R���X�g���N�^
	GRScene(const GRSceneDesc& desc=GRSceneDesc());
	void Init();
	///	�f�X�g���N�^
	~GRScene(){}

	GRSdkIf* GetSdk();

	///	�V�[������ɂ���D
	void Clear();

	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual GRFrameIf* CreateFrame(const GRFrameDesc& desc);
	virtual GRFrameIf* GetWorld(){return world;}
	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual void Render(GRRenderIf* r);
protected:
	void* GetDescAddress(){ return (GRSceneDesc*)this; }
	friend class GRFrame;
};

}
#endif
