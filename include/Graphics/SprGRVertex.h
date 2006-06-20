/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_GRVERTEX_H
#define SPR_GRVERTEX_H
#include <Base/Affine.h>

namespace Spr{;

enum GRVertexElementType {
    GRVET_FLOAT1 = 0,
    GRVET_FLOAT2 = 1,
    GRVET_FLOAT3 = 2,
    GRVET_FLOAT4 = 3,
    GRVET_D3DCOLOR = 4,
    GRVET_UBYTE4 = 5,
    GRVET_SHORT2 = 6,
    GRVET_SHORT4 = 7,
    GRVET_UBYTE4N = 8,
    GRVET_SHORT2N = 9,
    GRVET_SHORT4N = 10,
    GRVET_USHORT2N = 11,
    GRVET_USHORT4N = 12,
    GRVET_UDEC3 = 13,
    GRVET_DEC3N = 14,
    GRVET_FLOAT16_2 = 15,
    GRVET_FLOAT16_4 = 16,
    GRVET_UNUSED = 17
};
enum GRVertexElementMethod {
    GRVEM_DEFAULT = 0,
    GRVEM_PARTIALU = 1,
    GRVEM_PARTIALV = 2,
    GRVEM_CROSSUV = 3,
    GRVEM_UV = 4,
    GRVEM_LOOKUP = 5,
    GRVEM_LOOKUPPRESAMPLED = 6
};
enum GRVertexElementUssage{
    GRVEU_POSITION = 0,
    GRVEU_BLENDWEIGHT = 1,
    GRVEU_BLENDINDICES = 2,
    GRVEU_NORMAL = 3,
    GRVEU_PSIZE = 4,
    GRVEU_TEXCOORD = 5,
    GRVEU_TANGENT = 6,
    GRVEU_BINORMAL = 7,
    GRVEU_TESSFACTOR = 8,
    GRVEU_POSITIONT = 9,
    GRVEU_COLOR = 10,
    GRVEU_FOG = 11,
    GRVEU_DEPTH = 12,
    GRVEU_SAMPLE = 13
};

/**	���_�t�H�[�}�b�g�DDirectX9�� D3DVERTEXELEMENT ���Q�ƁD
	GRRender->SetVertexFormat()�ɔz���n���D
	GRRender�́C�z������āC�K���ȃV�F�[�_�[��ݒ肷��D
	�V�F�[�_�[��������Ȃ��ꍇ�́CNULL�V�F�[�_�[��ݒ肷�遁�`�悳��Ȃ��D
	DrawDirect/DrawIndexed �̓V�F�[�_�[���g���ĕ`�悷��D
*/
struct GRVertexElement {
    short stream;
    short offset;
    unsigned char type;
    unsigned char method;
    unsigned char usage;
    unsigned char usageIndex;

	GRVertexElement();
	static GRVertexElement Position3f(size_t o);
	static GRVertexElement Normal3f(size_t o);
	static GRVertexElement TexCoords2f(size_t o);
	static GRVertexElement Color4b(size_t o);
	static GRVertexElement Color4f(size_t o);

	///	@name �T�^�I�Ȓ��_�t�H�[�}�b�g�D�V�F�[�_�[���w�肹�������_�����O�ł���D
	//@{
	static const GRVertexElement vfP3f[];				///<
	static const GRVertexElement vfC4bP3f[];			///<
	static const GRVertexElement vfN3fP3f[];			///<
	static const GRVertexElement vfC4fN3fP3f[];			///<
	static const GRVertexElement vfT2fP3f[];			///<
	static const GRVertexElement vfT2fC4bP3f[];			///<
	static const GRVertexElement vfT2fN3fP3f[];			///<
	static const GRVertexElement vfT2fC4fN3fP3f[];		///<
	static const GRVertexElement* typicalFormats[];		///<
	//@}
	struct VFP3f{
		Vec3f p;
	};
	struct VFC4bP3f{
		unsigned long c;
		Vec3f p;
	};
	struct VFN3fP3f{
		Vec3f n;
		Vec3f p;
	};
	struct VFC4fN3fP3f{
		Vec4f c;
		Vec3f n;
		Vec3f p;
	};
	struct VFT2fP3f{
		Vec2f t;
		Vec3f p;
	};
	struct VFT2fC4bP3f{
		Vec2f t;
		unsigned long c;
		Vec3f p;
	};
	struct VFT2fN3fP3f{
		Vec2f t;
		Vec3f n;
		Vec3f p;
	};
	struct VFT2fC4fN3fP3f{
		Vec2f t;
		Vec4f c;
		Vec3f n;
		Vec3f p;
	};
};

/**	�C���^�[���[�u����Ă��Ȃ����_�o�b�t�@�̎w��
<pre>
	Vec3f pos[20]; Vec3f normal[20]; unsigned color[20];
</pre>
�̂悤�ȃo�b�t�@���w�肷��Ƃ��Ɏg���D
�z��ЂƂ�1��GRVertexArray���Ή�����D
*/
struct GRVertexArray {
	GRVertexElement format;	///<	�t�H�[�}�b�g
	void* buffer;			///<	�z��̃A�h���X
};
	
}
#endif
