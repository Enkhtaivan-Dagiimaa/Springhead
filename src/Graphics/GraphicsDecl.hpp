#define SPR_DECLMEMBEROF_GRBlendMeshDesc \
protected:\
	std::vector<(Vec3f)>	positions;	\
	std::vector<(Vec3f)>	normals;	\
	std::vector<(Vec4f)>	colors;	\
	std::vector<(Vec2f)>	texCoords;	\
	std::vector<(size_t)>	faces;	\
	std::vector<(Vec4f)>	blends;	\
	std::vector<(Vec4f)>	matrixIndices;	\
	std::vector<(Vec4f)>	numMatrix;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		positions = ((GRBlendMeshDesc*)ptr)->positions;	\
		normals = ((GRBlendMeshDesc*)ptr)->normals;	\
		colors = ((GRBlendMeshDesc*)ptr)->colors;	\
		texCoords = ((GRBlendMeshDesc*)ptr)->texCoords;	\
		faces = ((GRBlendMeshDesc*)ptr)->faces;	\
		blends = ((GRBlendMeshDesc*)ptr)->blends;	\
		matrixIndices = ((GRBlendMeshDesc*)ptr)->matrixIndices;	\
		numMatrix = ((GRBlendMeshDesc*)ptr)->numMatrix;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		((GRBlendMeshDesc*)ptr)->positions = positions;	\
		((GRBlendMeshDesc*)ptr)->normals = normals;	\
		((GRBlendMeshDesc*)ptr)->colors = colors;	\
		((GRBlendMeshDesc*)ptr)->texCoords = texCoords;	\
		((GRBlendMeshDesc*)ptr)->faces = faces;	\
		((GRBlendMeshDesc*)ptr)->blends = blends;	\
		((GRBlendMeshDesc*)ptr)->matrixIndices = matrixIndices;	\
		((GRBlendMeshDesc*)ptr)->numMatrix = numMatrix;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRVisualDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
	}\
	virtual bool GetDesc(const void* ptr){ \
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRFrameDesc \
protected:\
	Affinef	transform;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		GRVisual::SetDesc((GRVisualDesc*)(GRFrameDesc*)ptr);	\
		transform = ((GRFrameDesc*)ptr)->transform;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		GRVisual::GetDesc((GRVisualDesc*)(GRFrameDesc*)ptr);	\
		((GRFrameDesc*)ptr)->transform = transform;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRDummyFrameDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
		GRVisual::SetDesc((GRVisualDesc*)(GRDummyFrameDesc*)ptr);	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		GRVisual::GetDesc((GRVisualDesc*)(GRDummyFrameDesc*)ptr);	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRAnimationDesc \
protected:\
	std::vector<(Spr::GRAnimationKey)>	keys;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		keys = ((GRAnimationDesc*)ptr)->keys;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		((GRAnimationDesc*)ptr)->keys = keys;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRAnimationSetDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
	}\
	virtual bool GetDesc(const void* ptr){ \
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRAnimationControllerDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
	}\
	virtual bool GetDesc(const void* ptr){ \
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRMeshDesc \
protected:\
	std::vector<(Vec3f)>	positions;	\
	std::vector<(Vec3f)>	normals;	\
	std::vector<(Vec4f)>	colors;	\
	std::vector<(Vec2f)>	texCoords;	\
	std::vector<(size_t)>	faces;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		GRVisual::SetDesc((GRVisualDesc*)(GRMeshDesc*)ptr);	\
		positions = ((GRMeshDesc*)ptr)->positions;	\
		normals = ((GRMeshDesc*)ptr)->normals;	\
		colors = ((GRMeshDesc*)ptr)->colors;	\
		texCoords = ((GRMeshDesc*)ptr)->texCoords;	\
		faces = ((GRMeshDesc*)ptr)->faces;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		GRVisual::GetDesc((GRVisualDesc*)(GRMeshDesc*)ptr);	\
		((GRMeshDesc*)ptr)->positions = positions;	\
		((GRMeshDesc*)ptr)->normals = normals;	\
		((GRMeshDesc*)ptr)->colors = colors;	\
		((GRMeshDesc*)ptr)->texCoords = texCoords;	\
		((GRMeshDesc*)ptr)->faces = faces;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRLightDesc \
protected:\
	Vec4f	ambient;	\
	Vec4f	diffuse;	\
	Vec4f	specular;	\
	Vec4f	position;	\
	float	range;	\
	float	attenuation0;	\
	float	attenuation1;	\
	float	attenuation2;	\
	Vec3f	spotDirection;	\
	float	spotFalloff;	\
	float	spotInner;	\
	float	spotCutoff;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		GRVisual::SetDesc((GRVisualDesc*)(GRLightDesc*)ptr);	\
		ambient = ((GRLightDesc*)ptr)->ambient;	\
		diffuse = ((GRLightDesc*)ptr)->diffuse;	\
		specular = ((GRLightDesc*)ptr)->specular;	\
		position = ((GRLightDesc*)ptr)->position;	\
		range = ((GRLightDesc*)ptr)->range;	\
		attenuation0 = ((GRLightDesc*)ptr)->attenuation0;	\
		attenuation1 = ((GRLightDesc*)ptr)->attenuation1;	\
		attenuation2 = ((GRLightDesc*)ptr)->attenuation2;	\
		spotDirection = ((GRLightDesc*)ptr)->spotDirection;	\
		spotFalloff = ((GRLightDesc*)ptr)->spotFalloff;	\
		spotInner = ((GRLightDesc*)ptr)->spotInner;	\
		spotCutoff = ((GRLightDesc*)ptr)->spotCutoff;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		GRVisual::GetDesc((GRVisualDesc*)(GRLightDesc*)ptr);	\
		((GRLightDesc*)ptr)->ambient = ambient;	\
		((GRLightDesc*)ptr)->diffuse = diffuse;	\
		((GRLightDesc*)ptr)->specular = specular;	\
		((GRLightDesc*)ptr)->position = position;	\
		((GRLightDesc*)ptr)->range = range;	\
		((GRLightDesc*)ptr)->attenuation0 = attenuation0;	\
		((GRLightDesc*)ptr)->attenuation1 = attenuation1;	\
		((GRLightDesc*)ptr)->attenuation2 = attenuation2;	\
		((GRLightDesc*)ptr)->spotDirection = spotDirection;	\
		((GRLightDesc*)ptr)->spotFalloff = spotFalloff;	\
		((GRLightDesc*)ptr)->spotInner = spotInner;	\
		((GRLightDesc*)ptr)->spotCutoff = spotCutoff;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRMaterialDesc \
protected:\
	Vec4f	ambient;	\
	Vec4f	diffuse;	\
	Vec4f	specular;	\
	Vec4f	emissive;	\
	float	power;	\
	std::string	texname;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		GRVisual::SetDesc((GRVisualDesc*)(GRMaterialDesc*)ptr);	\
		ambient = ((GRMaterialDesc*)ptr)->ambient;	\
		diffuse = ((GRMaterialDesc*)ptr)->diffuse;	\
		specular = ((GRMaterialDesc*)ptr)->specular;	\
		emissive = ((GRMaterialDesc*)ptr)->emissive;	\
		power = ((GRMaterialDesc*)ptr)->power;	\
		texname = ((GRMaterialDesc*)ptr)->texname;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		GRVisual::GetDesc((GRVisualDesc*)(GRMaterialDesc*)ptr);	\
		((GRMaterialDesc*)ptr)->ambient = ambient;	\
		((GRMaterialDesc*)ptr)->diffuse = diffuse;	\
		((GRMaterialDesc*)ptr)->specular = specular;	\
		((GRMaterialDesc*)ptr)->emissive = emissive;	\
		((GRMaterialDesc*)ptr)->power = power;	\
		((GRMaterialDesc*)ptr)->texname = texname;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRCameraDesc \
protected:\
	Vec2f	size;	\
	Vec2f	center;	\
	float	front;	\
	float	back;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		GRVisual::SetDesc((GRVisualDesc*)(GRCameraDesc*)ptr);	\
		size = ((GRCameraDesc*)ptr)->size;	\
		center = ((GRCameraDesc*)ptr)->center;	\
		front = ((GRCameraDesc*)ptr)->front;	\
		back = ((GRCameraDesc*)ptr)->back;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		GRVisual::GetDesc((GRVisualDesc*)(GRCameraDesc*)ptr);	\
		((GRCameraDesc*)ptr)->size = size;	\
		((GRCameraDesc*)ptr)->center = center;	\
		((GRCameraDesc*)ptr)->front = front;	\
		((GRCameraDesc*)ptr)->back = back;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRSceneDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
	}\
	virtual bool GetDesc(const void* ptr){ \
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRSdkDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
	}\
	virtual bool GetDesc(const void* ptr){ \
		return true;	\
	}\

#define SPR_DECLMEMBEROF_GRSphereDesc \
protected:\
	float	radius;	\
	int	slices;	\
	int	stacks;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		radius = ((GRSphereDesc*)ptr)->radius;	\
		slices = ((GRSphereDesc*)ptr)->slices;	\
		stacks = ((GRSphereDesc*)ptr)->stacks;	\
	}\
	virtual bool GetDesc(const void* ptr){ \
		((GRSphereDesc*)ptr)->radius = radius;	\
		((GRSphereDesc*)ptr)->slices = slices;	\
		((GRSphereDesc*)ptr)->stacks = stacks;	\
		return true;	\
	}\

