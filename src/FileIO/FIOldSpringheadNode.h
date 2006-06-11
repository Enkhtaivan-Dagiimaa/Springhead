#ifndef FIOLDSPRINGHEADNODE_H
#define FIOLDSPRINGHEADNODE_H

namespace Spr{;
namespace SprOldSpringehead{;

struct Frame{
};
struct FrameTransformMatrix{
	Affinef matrix;
};

struct Light8{
	int type;
    Vec4f diffuse;		///<	�g�U��
    Vec4f specular;		///<	
    Vec4f ambient;
    Vec4f position;		///<	�����̈ʒu�Cw=0�Ƃ���Ɩ�����(��������)�ɂȂ�D
    float range;		///<	�����͂��͈�
    ///@name	�������DAtten = 1/( att0 + att1 * d + att2 * d^2)
	//@{
	float attenuation0;	///<	att0	0..��
    float attenuation1;	///<	att1	0..��
    float attenuation2;	///<	att2	0..��
	//@}
	Vec3f spotDirection;///<	�X�|�b�g���C�g�̏ꍇ�̌���
    float spotFalloff;	///<	�����̑���(�傫���قǋ}�s)		0..��
	float spotInner;	///<	�X�|�b�g���C�g�̒��S����		0..spotCutoff
	float spotCutoff;	///<	�X�|�b�g���C�g�̌���������͈�	0..��
};
struct Material{
	Vec4f face;
	float power;
	Vec3f specular;
	Vec3f emissive;
};



struct MeshFace{
	int nFaceVertexIndices;
	std::vector<int> faceVertexIndices;
};
struct Mesh{
	int nVertices;
	std::vector<Vec3f> vertices;
	int nFaces;
	std::vector<MeshFace> faces;
	
};
struct MeshNormals{
	int nNormals;
	std::vector<Vec3f> normals;
	int nFaceNormals;
	std::vector<MeshFace> faceNormals;
};
struct MeshMaterialList{
	int nMaterials;
	int nFaceIndexes;
	std::vector<int> faceIndexes;
};
struct MeshTextureCoords{
	int nTextureCoords;
	std::vector<Vec2f> textureCoords;
};


}}
#endif
