//	Do not edit. MakeTypeDesc.bat will update this file.
	
	PHConstraintDesc* pPHConstraintDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHConstraintDesc");
	desc->size = sizeof(PHConstraintDesc);
	desc->ifInfo = PHConstraintIf::GetIfInfoStatic();
	((IfInfo*)PHConstraintIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHConstraintDesc>;
	field = desc->AddField("ConstraintType", "enum", "type",  "");
	field->AddEnumConst("INVALID_CONSTRAINT");
	field->AddEnumConst("CONTACT");
	field->AddEnumConst("HINGEJOINT");
	field->AddEnumConst("SLIDERJOINT");
	field->AddEnumConst("BALLJOINT");
	field->AddEnumConst("PATHJOINT");
	field->AddEnumConst("SPRING");
	field->AddEnumConst("SPRING");
	field->offset = int((char*)(&pPHConstraintDesc->type) - (char*)pPHConstraintDesc);
	field = desc->AddField("", "bool", "bEnabled", "");
	field->offset = int((char*)&(pPHConstraintDesc->bEnabled) - (char*)pPHConstraintDesc);
	field = desc->AddField("", "Posed", "poseSocket", "");
	field->offset = int((char*)&(pPHConstraintDesc->poseSocket) - (char*)pPHConstraintDesc);
	field = desc->AddField("", "Posed", "posePlug", "");
	field->offset = int((char*)&(pPHConstraintDesc->posePlug) - (char*)pPHConstraintDesc);
	db->RegisterDesc(desc);
	
	PHJoint1DDesc* pPHJoint1DDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHJoint1DDesc");
	desc->size = sizeof(PHJoint1DDesc);
	desc->ifInfo = PHJoint1DIf::GetIfInfoStatic();
	((IfInfo*)PHJoint1DIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHJoint1DDesc>;
	field = desc->AddBase("PHJointDesc");
	field->offset = int((char*)(PHJointDesc*)pPHJoint1DDesc - (char*)pPHJoint1DDesc);
	field = desc->AddField("", "double", "lower", "");
	field->offset = int((char*)&(pPHJoint1DDesc->lower) - (char*)pPHJoint1DDesc);
	field = desc->AddField("", "double", "upper", "");
	field->offset = int((char*)&(pPHJoint1DDesc->upper) - (char*)pPHJoint1DDesc);
	field = desc->AddField("", "double", "spring", "");
	field->offset = int((char*)&(pPHJoint1DDesc->spring) - (char*)pPHJoint1DDesc);
	field = desc->AddField("", "double", "origin", "");
	field->offset = int((char*)&(pPHJoint1DDesc->origin) - (char*)pPHJoint1DDesc);
	field = desc->AddField("", "double", "damper", "");
	field->offset = int((char*)&(pPHJoint1DDesc->damper) - (char*)pPHJoint1DDesc);
	field = desc->AddField("", "double", "torque", "");
	field->offset = int((char*)&(pPHJoint1DDesc->torque) - (char*)pPHJoint1DDesc);
	db->RegisterDesc(desc);
	
	PHHingeJointDesc* pPHHingeJointDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHHingeJointDesc");
	desc->size = sizeof(PHHingeJointDesc);
	desc->ifInfo = PHHingeJointIf::GetIfInfoStatic();
	((IfInfo*)PHHingeJointIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHHingeJointDesc>;
	field = desc->AddBase("PHJoint1DDesc");
	field->offset = int((char*)(PHJoint1DDesc*)pPHHingeJointDesc - (char*)pPHHingeJointDesc);
	db->RegisterDesc(desc);
	
	PHSliderJointDesc* pPHSliderJointDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHSliderJointDesc");
	desc->size = sizeof(PHSliderJointDesc);
	desc->ifInfo = PHSliderJointIf::GetIfInfoStatic();
	((IfInfo*)PHSliderJointIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHSliderJointDesc>;
	field = desc->AddBase("PHJoint1DDesc");
	field->offset = int((char*)(PHJoint1DDesc*)pPHSliderJointDesc - (char*)pPHSliderJointDesc);
	db->RegisterDesc(desc);
	
	PHPathPoint* pPHPathPoint = NULL;
	desc = DBG_NEW UTTypeDesc("PHPathPoint");
	desc->size = sizeof(PHPathPoint);
	desc->access = DBG_NEW UTAccess<PHPathPoint>;
	field = desc->AddField("", "double", "s", "");
	field->offset = int((char*)&(pPHPathPoint->s) - (char*)pPHPathPoint);
	field = desc->AddField("", "Posed", "pose", "");
	field->offset = int((char*)&(pPHPathPoint->pose) - (char*)pPHPathPoint);
	db->RegisterDesc(desc);
	
	PHPathDesc* pPHPathDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHPathDesc");
	desc->size = sizeof(PHPathDesc);
	desc->ifInfo = PHPathIf::GetIfInfoStatic();
	((IfInfo*)PHPathIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHPathDesc>;
	field = desc->AddField("vector", "PHPathPoint", "points", "");
	field->offset = int((char*)&(pPHPathDesc->points) - (char*)pPHPathDesc);
	field = desc->AddField("", "bool", "bLoop", "");
	field->offset = int((char*)&(pPHPathDesc->bLoop) - (char*)pPHPathDesc);
	db->RegisterDesc(desc);
	
	PHPathJointDesc* pPHPathJointDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHPathJointDesc");
	desc->size = sizeof(PHPathJointDesc);
	desc->ifInfo = PHPathJointIf::GetIfInfoStatic();
	((IfInfo*)PHPathJointIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHPathJointDesc>;
	field = desc->AddBase("PHJoint1DDesc");
	field->offset = int((char*)(PHJoint1DDesc*)pPHPathJointDesc - (char*)pPHPathJointDesc);
	db->RegisterDesc(desc);
	
	PHBallJointDesc* pPHBallJointDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHBallJointDesc");
	desc->size = sizeof(PHBallJointDesc);
	desc->ifInfo = PHBallJointIf::GetIfInfoStatic();
	((IfInfo*)PHBallJointIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHBallJointDesc>;
	field = desc->AddBase("PHJointDesc");
	field->offset = int((char*)(PHJointDesc*)pPHBallJointDesc - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "swingUpper", "");
	field->offset = int((char*)&(pPHBallJointDesc->swingUpper) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "twistLower", "");
	field->offset = int((char*)&(pPHBallJointDesc->twistLower) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "twistUpper", "");
	field->offset = int((char*)&(pPHBallJointDesc->twistUpper) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "swingSpring", "");
	field->offset = int((char*)&(pPHBallJointDesc->swingSpring) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "swingDamper", "");
	field->offset = int((char*)&(pPHBallJointDesc->swingDamper) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "twistSpring", "");
	field->offset = int((char*)&(pPHBallJointDesc->twistSpring) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "double", "twistDamper", "");
	field->offset = int((char*)&(pPHBallJointDesc->twistDamper) - (char*)pPHBallJointDesc);
	field = desc->AddField("", "Vec3d", "torque", "");
	field->offset = int((char*)&(pPHBallJointDesc->torque) - (char*)pPHBallJointDesc);
	db->RegisterDesc(desc);
	
	PHSpringDesc* pPHSpringDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHSpringDesc");
	desc->size = sizeof(PHSpringDesc);
	desc->ifInfo = PHSpringIf::GetIfInfoStatic();
	((IfInfo*)PHSpringIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHSpringDesc>;
	field = desc->AddBase("PHConstraintDesc");
	field->offset = int((char*)(PHConstraintDesc*)pPHSpringDesc - (char*)pPHSpringDesc);
	field = desc->AddField("", "Vec3d", "spring", "");
	field->offset = int((char*)&(pPHSpringDesc->spring) - (char*)pPHSpringDesc);
	field = desc->AddField("", "Vec3d", "damper", "");
	field->offset = int((char*)&(pPHSpringDesc->damper) - (char*)pPHSpringDesc);
	db->RegisterDesc(desc);
	
	PHTreeNodeDesc* pPHTreeNodeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHTreeNodeDesc");
	desc->size = sizeof(PHTreeNodeDesc);
	desc->ifInfo = PHTreeNodeIf::GetIfInfoStatic();
	((IfInfo*)PHTreeNodeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHTreeNodeDesc>;
	field = desc->AddField("", "bool", "bEnabled", "");
	field->offset = int((char*)&(pPHTreeNodeDesc->bEnabled) - (char*)pPHTreeNodeDesc);
	db->RegisterDesc(desc);
	
	PHRootNodeDesc* pPHRootNodeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHRootNodeDesc");
	desc->size = sizeof(PHRootNodeDesc);
	desc->ifInfo = PHRootNodeIf::GetIfInfoStatic();
	((IfInfo*)PHRootNodeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHRootNodeDesc>;
	field = desc->AddBase("PHTreeNodeDesc");
	field->offset = int((char*)(PHTreeNodeDesc*)pPHRootNodeDesc - (char*)pPHRootNodeDesc);
	db->RegisterDesc(desc);
	
	PHTreeNode1DDesc* pPHTreeNode1DDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHTreeNode1DDesc");
	desc->size = sizeof(PHTreeNode1DDesc);
	desc->ifInfo = PHTreeNode1DIf::GetIfInfoStatic();
	((IfInfo*)PHTreeNode1DIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHTreeNode1DDesc>;
	field = desc->AddBase("PHTreeNodeDesc");
	field->offset = int((char*)(PHTreeNodeDesc*)pPHTreeNode1DDesc - (char*)pPHTreeNode1DDesc);
	db->RegisterDesc(desc);
	
	PHHingeJointNodeDesc* pPHHingeJointNodeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHHingeJointNodeDesc");
	desc->size = sizeof(PHHingeJointNodeDesc);
	desc->ifInfo = PHHingeJointNodeIf::GetIfInfoStatic();
	((IfInfo*)PHHingeJointNodeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHHingeJointNodeDesc>;
	field = desc->AddBase("PHTreeNode1DDesc");
	field->offset = int((char*)(PHTreeNode1DDesc*)pPHHingeJointNodeDesc - (char*)pPHHingeJointNodeDesc);
	db->RegisterDesc(desc);
	
	PHSliderJointNodeDesc* pPHSliderJointNodeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHSliderJointNodeDesc");
	desc->size = sizeof(PHSliderJointNodeDesc);
	desc->ifInfo = PHSliderJointNodeIf::GetIfInfoStatic();
	((IfInfo*)PHSliderJointNodeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHSliderJointNodeDesc>;
	field = desc->AddBase("PHTreeNode1DDesc");
	field->offset = int((char*)(PHTreeNode1DDesc*)pPHSliderJointNodeDesc - (char*)pPHSliderJointNodeDesc);
	db->RegisterDesc(desc);
	
	PHPathJointNodeDesc* pPHPathJointNodeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHPathJointNodeDesc");
	desc->size = sizeof(PHPathJointNodeDesc);
	desc->ifInfo = PHPathJointNodeIf::GetIfInfoStatic();
	((IfInfo*)PHPathJointNodeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHPathJointNodeDesc>;
	field = desc->AddBase("PHTreeNode1DDesc");
	field->offset = int((char*)(PHTreeNode1DDesc*)pPHPathJointNodeDesc - (char*)pPHPathJointNodeDesc);
	db->RegisterDesc(desc);
	
	PHBallJointNodeDesc* pPHBallJointNodeDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHBallJointNodeDesc");
	desc->size = sizeof(PHBallJointNodeDesc);
	desc->ifInfo = PHBallJointNodeIf::GetIfInfoStatic();
	((IfInfo*)PHBallJointNodeIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHBallJointNodeDesc>;
	field = desc->AddBase("PHTreeNodeDesc");
	field->offset = int((char*)(PHTreeNodeDesc*)pPHBallJointNodeDesc - (char*)pPHBallJointNodeDesc);
	db->RegisterDesc(desc);
	
	PHGearDesc* pPHGearDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHGearDesc");
	desc->size = sizeof(PHGearDesc);
	desc->ifInfo = PHGearIf::GetIfInfoStatic();
	((IfInfo*)PHGearIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHGearDesc>;
	field = desc->AddField("", "double", "ratio", "");
	field->offset = int((char*)&(pPHGearDesc->ratio) - (char*)pPHGearDesc);
	db->RegisterDesc(desc);
	
	PHInactiveSolids* pPHInactiveSolids = NULL;
	desc = DBG_NEW UTTypeDesc("PHInactiveSolids");
	desc->size = sizeof(PHInactiveSolids);
	desc->access = DBG_NEW UTAccess<PHInactiveSolids>;
	field = desc->AddField("vector", "string", "solids", "");
	field->offset = int((char*)&(pPHInactiveSolids->solids) - (char*)pPHInactiveSolids);
	db->RegisterDesc(desc);
	
	PHSceneState* pPHSceneState = NULL;
	desc = DBG_NEW UTTypeDesc("PHSceneState");
	desc->size = sizeof(PHSceneState);
	desc->access = DBG_NEW UTAccess<PHSceneState>;
	field = desc->AddField("", "double", "timeStep", "");
	field->offset = int((char*)&(pPHSceneState->timeStep) - (char*)pPHSceneState);
	field = desc->AddField("", "unsigned", "count", "");
	field->offset = int((char*)&(pPHSceneState->count) - (char*)pPHSceneState);
	db->RegisterDesc(desc);
	
	PHSceneDesc* pPHSceneDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHSceneDesc");
	desc->size = sizeof(PHSceneDesc);
	desc->ifInfo = PHSceneIf::GetIfInfoStatic();
	((IfInfo*)PHSceneIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHSceneDesc>;
	field = desc->AddBase("PHSceneState");
	field->offset = int((char*)(PHSceneState*)pPHSceneDesc - (char*)pPHSceneDesc);
	field = desc->AddField("", "Vec3f", "gravity", "");
	field->offset = int((char*)&(pPHSceneDesc->gravity) - (char*)pPHSceneDesc);
	field = desc->AddField("", "int", "numIter", "");
	field->offset = int((char*)&(pPHSceneDesc->numIter) - (char*)pPHSceneDesc);
	db->RegisterDesc(desc);
	
	PHSdkDesc* pPHSdkDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHSdkDesc");
	desc->size = sizeof(PHSdkDesc);
	desc->ifInfo = PHSdkIf::GetIfInfoStatic();
	((IfInfo*)PHSdkIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHSdkDesc>;
	db->RegisterDesc(desc);
	
	PHSolidState* pPHSolidState = NULL;
	desc = DBG_NEW UTTypeDesc("PHSolidState");
	desc->size = sizeof(PHSolidState);
	desc->access = DBG_NEW UTAccess<PHSolidState>;
	field = desc->AddField("", "Vec3d", "velocity", "");
	field->offset = int((char*)&(pPHSolidState->velocity) - (char*)pPHSolidState);
	field = desc->AddField("", "Vec3d", "angVelocity", "");
	field->offset = int((char*)&(pPHSolidState->angVelocity) - (char*)pPHSolidState);
	field = desc->AddField("", "Posed", "pose", "");
	field->offset = int((char*)&(pPHSolidState->pose) - (char*)pPHSolidState);
	field = desc->AddField("", "Vec3d", "force", "");
	field->offset = int((char*)&(pPHSolidState->force) - (char*)pPHSolidState);
	field = desc->AddField("", "Vec3d", "torque", "");
	field->offset = int((char*)&(pPHSolidState->torque) - (char*)pPHSolidState);
	field = desc->AddField("", "Vec3d", "nextForce", "");
	field->offset = int((char*)&(pPHSolidState->nextForce) - (char*)pPHSolidState);
	field = desc->AddField("", "Vec3d", "nextTorque", "");
	field->offset = int((char*)&(pPHSolidState->nextTorque) - (char*)pPHSolidState);
	db->RegisterDesc(desc);
	
	PHSolidDesc* pPHSolidDesc = NULL;
	desc = DBG_NEW UTTypeDesc("PHSolidDesc");
	desc->size = sizeof(PHSolidDesc);
	desc->ifInfo = PHSolidIf::GetIfInfoStatic();
	((IfInfo*)PHSolidIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<PHSolidDesc>;
	field = desc->AddBase("PHSolidState");
	field->offset = int((char*)(PHSolidState*)pPHSolidDesc - (char*)pPHSolidDesc);
	field = desc->AddField("", "double", "mass", "");
	field->offset = int((char*)&(pPHSolidDesc->mass) - (char*)pPHSolidDesc);
	field = desc->AddField("", "Matrix3d", "inertia", "");
	field->offset = int((char*)&(pPHSolidDesc->inertia) - (char*)pPHSolidDesc);
	field = desc->AddField("", "Vec3d", "center", "");
	field->offset = int((char*)&(pPHSolidDesc->center) - (char*)pPHSolidDesc);
	field = desc->AddField("", "bool", "dynamical", "");
	field->offset = int((char*)&(pPHSolidDesc->dynamical) - (char*)pPHSolidDesc);
	db->RegisterDesc(desc);
