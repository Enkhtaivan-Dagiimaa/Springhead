#define SPR_DECLMEMBEROF_FWBoneDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_FWInteractPointerDesc \
protected:\
	PHSolidIf*	pointerSolid;	\
	HIBaseIf*	humanInterface;	\
	double	springK;	\
	double	damperD;	\
	Posed	defaultPosition;	\
	Posed	pointersCalibPosition;	\
	double	posScale;	\
	double	forceScale;	\
	double	localRange;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		pointerSolid = ((FWInteractPointerDesc*)ptr)->pointerSolid;	\
		humanInterface = ((FWInteractPointerDesc*)ptr)->humanInterface;	\
		springK = ((FWInteractPointerDesc*)ptr)->springK;	\
		damperD = ((FWInteractPointerDesc*)ptr)->damperD;	\
		defaultPosition = ((FWInteractPointerDesc*)ptr)->defaultPosition;	\
		pointersCalibPosition = ((FWInteractPointerDesc*)ptr)->pointersCalibPosition;	\
		posScale = ((FWInteractPointerDesc*)ptr)->posScale;	\
		forceScale = ((FWInteractPointerDesc*)ptr)->forceScale;	\
		localRange = ((FWInteractPointerDesc*)ptr)->localRange;	\
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		((FWInteractPointerDesc*)ptr)->pointerSolid = pointerSolid;	\
		((FWInteractPointerDesc*)ptr)->humanInterface = humanInterface;	\
		((FWInteractPointerDesc*)ptr)->springK = springK;	\
		((FWInteractPointerDesc*)ptr)->damperD = damperD;	\
		((FWInteractPointerDesc*)ptr)->defaultPosition = defaultPosition;	\
		((FWInteractPointerDesc*)ptr)->pointersCalibPosition = pointersCalibPosition;	\
		((FWInteractPointerDesc*)ptr)->posScale = posScale;	\
		((FWInteractPointerDesc*)ptr)->forceScale = forceScale;	\
		((FWInteractPointerDesc*)ptr)->localRange = localRange;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_FWInteractSceneDesc \
protected:\
	Spr::FWSceneIf*	fwScene;	\
	Spr::FWInteractMode	iaMode;	\
	Spr::FWHapticMode	hMode;	\
	double	hdt;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		fwScene = ((FWInteractSceneDesc*)ptr)->fwScene;	\
		iaMode = ((FWInteractSceneDesc*)ptr)->iaMode;	\
		hMode = ((FWInteractSceneDesc*)ptr)->hMode;	\
		hdt = ((FWInteractSceneDesc*)ptr)->hdt;	\
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		((FWInteractSceneDesc*)ptr)->fwScene = fwScene;	\
		((FWInteractSceneDesc*)ptr)->iaMode = iaMode;	\
		((FWInteractSceneDesc*)ptr)->hMode = hMode;	\
		((FWInteractSceneDesc*)ptr)->hdt = hdt;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_FWObjectDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_FWBoneObjectDesc \
protected:\
	bool	modifyShapeLength;	\
	bool	modifyShapeWidth;	\
	bool	modifyShapePose;	\
	bool	modifyPlugSocketPose;	\
public:\
	virtual void SetDesc(const void* ptr){ \
		modifyShapeLength = ((FWBoneObjectDesc*)ptr)->modifyShapeLength;	\
		modifyShapeWidth = ((FWBoneObjectDesc*)ptr)->modifyShapeWidth;	\
		modifyShapePose = ((FWBoneObjectDesc*)ptr)->modifyShapePose;	\
		modifyPlugSocketPose = ((FWBoneObjectDesc*)ptr)->modifyPlugSocketPose;	\
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		((FWBoneObjectDesc*)ptr)->modifyShapeLength = modifyShapeLength;	\
		((FWBoneObjectDesc*)ptr)->modifyShapeWidth = modifyShapeWidth;	\
		((FWBoneObjectDesc*)ptr)->modifyShapePose = modifyShapePose;	\
		((FWBoneObjectDesc*)ptr)->modifyPlugSocketPose = modifyPlugSocketPose;	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_FWSceneDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		return true;	\
	}\

#define SPR_DECLMEMBEROF_FWSdkDesc \
protected:\
public:\
	virtual void SetDesc(const void* ptr){ \
		AfterSetDesc();	\
	}\
	virtual bool GetDesc(void* ptr) const { \
		BeforeGetDesc();	\
		return true;	\
	}\

