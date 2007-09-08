//	Do not edit. MakeTypeDesc.bat will update this file.
	
	CRBodyDesc* pCRBodyDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRBodyDesc");
	desc->size = sizeof(CRBodyDesc);
	desc->ifInfo = CRBodyIf::GetIfInfoStatic();
	((IfInfo*)CRBodyIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRBodyDesc>;
	field = desc->AddField("CRHumanJointOrder", "enum", "jointOrder",  "");
	field->AddEnumConst("SOCKET_PARENT");
	field->AddEnumConst("PLUG_PARENT");
	field->offset = int((char*)(&pCRBodyDesc->jointOrder) - (char*)pCRBodyDesc);
	db->RegisterDesc(desc);
	
	CRHingeHumanBodyDesc* pCRHingeHumanBodyDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRHingeHumanBodyDesc");
	desc->size = sizeof(CRHingeHumanBodyDesc);
	desc->ifInfo = CRHingeHumanBodyIf::GetIfInfoStatic();
	((IfInfo*)CRHingeHumanBodyIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRHingeHumanBodyDesc>;
	field = desc->AddBase("CRBodyDesc");
	field->offset = int((char*)(CRBodyDesc*)pCRHingeHumanBodyDesc - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "waistHeight", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->waistHeight) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "waistBreadth", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->waistBreadth) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "waistThickness", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->waistThickness) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "abdomenHeight", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->abdomenHeight) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "abdomenBreadth", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->abdomenBreadth) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "abdomenThickness", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->abdomenThickness) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "chestHeight", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->chestHeight) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "chestBreadth", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->chestBreadth) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "chestThickness", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->chestThickness) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "neckLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->neckLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "neckDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->neckDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "headDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->headDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "upperArmLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->upperArmLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "upperArmDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->upperArmDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "lowerArmLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->lowerArmLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "lowerArmDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->lowerArmDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "handLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->handLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "handBreadth", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->handBreadth) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "handThickness", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->handThickness) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "upperLegLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->upperLegLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "upperLegDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->upperLegDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "interLegDistance", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->interLegDistance) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "lowerLegLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->lowerLegLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "lowerLegDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->lowerLegDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "footLength", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->footLength) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "footBreadth", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->footBreadth) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "footThickness", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->footThickness) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "ankleToeDistance", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->ankleToeDistance) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "vertexToEyeHeight", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->vertexToEyeHeight) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "occiputToEyeDistance", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->occiputToEyeDistance) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "eyeDiameter", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->eyeDiameter) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "interpupillaryBreadth", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->interpupillaryBreadth) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWaistAbdomen", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWaistAbdomen) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWaistAbdomen", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWaistAbdomen) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springAbdomenChest", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springAbdomenChest) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperAbdomenChest", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperAbdomenChest) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springChestNeckX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springChestNeckX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperChestNeckX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperChestNeckX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springChestNeckY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springChestNeckY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperChestNeckY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperChestNeckY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springChestNeckZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springChestNeckZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperChestNeckZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperChestNeckZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springNeckHeadX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springNeckHeadX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperNeckHeadX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperNeckHeadX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springNeckHeadZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springNeckHeadZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperNeckHeadZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperNeckHeadZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springShoulderZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springShoulderZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperShoulderZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperShoulderZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springShoulderX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springShoulderX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperShoulderX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperShoulderX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springShoulderY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springShoulderY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperShoulderY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperShoulderY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springElbow", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springElbow) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperElbow", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperElbow) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWristY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWristY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWristY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWristY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWristX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWristX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWristX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWristX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWristZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWristZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWristZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWristZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWaistLegZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWaistLegZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWaistLegZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWaistLegZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWaistLegX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWaistLegX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWaistLegX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWaistLegX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springWaistLegY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springWaistLegY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperWaistLegY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperWaistLegY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springKnee", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springKnee) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperKnee", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperKnee) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springAnkleY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springAnkleY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperAnkleY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperAnkleY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springAnkleX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springAnkleX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperAnkleX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperAnkleX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springAnkleZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springAnkleZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperAnkleZ", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperAnkleZ) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springEyeY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springEyeY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperEyeY", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperEyeY) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "springEyeX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->springEyeX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "double", "damperEyeX", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->damperEyeX) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "Vec3d", "posRightUpperArm", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->posRightUpperArm) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "Quaterniond", "oriRightUpperArm", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->oriRightUpperArm) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "Quaterniond", "oriRightLowerArm", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->oriRightLowerArm) - (char*)pCRHingeHumanBodyDesc);
	field = desc->AddField("", "Quaterniond", "oriRightHand", "");
	field->offset = int((char*)&(pCRHingeHumanBodyDesc->oriRightHand) - (char*)pCRHingeHumanBodyDesc);
	db->RegisterDesc(desc);
	
	CRControllerDesc* pCRControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRControllerDesc");
	desc->size = sizeof(CRControllerDesc);
	desc->ifInfo = CRControllerIf::GetIfInfoStatic();
	((IfInfo*)CRControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRControllerDesc>;
	db->RegisterDesc(desc);
	
	CREyeControllerState* pCREyeControllerState = NULL;
	desc = DBG_NEW UTTypeDesc("CREyeControllerState");
	desc->size = sizeof(CREyeControllerState);
	desc->access = DBG_NEW UTAccess<CREyeControllerState>;
	field = desc->AddField("ControlState", "enum", "controlState",  "");
	field->AddEnumConst("CS_SACCADE");
	field->AddEnumConst("CS_PURSUIT");
	field->offset = int((char*)(&pCREyeControllerState->controlState) - (char*)pCREyeControllerState);
	db->RegisterDesc(desc);
	
	CREyeControllerDesc* pCREyeControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CREyeControllerDesc");
	desc->size = sizeof(CREyeControllerDesc);
	desc->ifInfo = CREyeControllerIf::GetIfInfoStatic();
	((IfInfo*)CREyeControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CREyeControllerDesc>;
	field = desc->AddBase("CREyeControllerState");
	field->offset = int((char*)(CREyeControllerState*)pCREyeControllerDesc - (char*)pCREyeControllerDesc);
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCREyeControllerDesc - (char*)pCREyeControllerDesc);
	db->RegisterDesc(desc);
	
	CRNeckControllerState* pCRNeckControllerState = NULL;
	desc = DBG_NEW UTTypeDesc("CRNeckControllerState");
	desc->size = sizeof(CRNeckControllerState);
	desc->access = DBG_NEW UTAccess<CRNeckControllerState>;
	db->RegisterDesc(desc);
	
	CRNeckControllerDesc* pCRNeckControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRNeckControllerDesc");
	desc->size = sizeof(CRNeckControllerDesc);
	desc->ifInfo = CRNeckControllerIf::GetIfInfoStatic();
	((IfInfo*)CRNeckControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRNeckControllerDesc>;
	field = desc->AddBase("CRNeckControllerState");
	field->offset = int((char*)(CRNeckControllerState*)pCRNeckControllerDesc - (char*)pCRNeckControllerDesc);
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCRNeckControllerDesc - (char*)pCRNeckControllerDesc);
	db->RegisterDesc(desc);
	
	CRGazeControllerState* pCRGazeControllerState = NULL;
	desc = DBG_NEW UTTypeDesc("CRGazeControllerState");
	desc->size = sizeof(CRGazeControllerState);
	desc->access = DBG_NEW UTAccess<CRGazeControllerState>;
	db->RegisterDesc(desc);
	
	CRGazeControllerDesc* pCRGazeControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRGazeControllerDesc");
	desc->size = sizeof(CRGazeControllerDesc);
	desc->ifInfo = CRGazeControllerIf::GetIfInfoStatic();
	((IfInfo*)CRGazeControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRGazeControllerDesc>;
	field = desc->AddBase("CRGazeControllerState");
	field->offset = int((char*)(CRGazeControllerState*)pCRGazeControllerDesc - (char*)pCRGazeControllerDesc);
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCRGazeControllerDesc - (char*)pCRGazeControllerDesc);
	db->RegisterDesc(desc);
	
	CRAttentionControllerDesc* pCRAttentionControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRAttentionControllerDesc");
	desc->size = sizeof(CRAttentionControllerDesc);
	desc->ifInfo = CRAttentionControllerIf::GetIfInfoStatic();
	((IfInfo*)CRAttentionControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRAttentionControllerDesc>;
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCRAttentionControllerDesc - (char*)pCRAttentionControllerDesc);
	db->RegisterDesc(desc);
	
	CRReachingControllerDesc* pCRReachingControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRReachingControllerDesc");
	desc->size = sizeof(CRReachingControllerDesc);
	desc->ifInfo = CRReachingControllerIf::GetIfInfoStatic();
	((IfInfo*)CRReachingControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRReachingControllerDesc>;
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCRReachingControllerDesc - (char*)pCRReachingControllerDesc);
	field = desc->AddField("pointer", "PHSolidIf", "solid", "");
	field->offset = int((char*)&(pCRReachingControllerDesc->solid) - (char*)pCRReachingControllerDesc);
	
	CRWalkingControllerDesc* pCRWalkingControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRWalkingControllerDesc");
	desc->size = sizeof(CRWalkingControllerDesc);
	desc->ifInfo = CRWalkingControllerIf::GetIfInfoStatic();
	((IfInfo*)CRWalkingControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRWalkingControllerDesc>;
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCRWalkingControllerDesc - (char*)pCRWalkingControllerDesc);
	db->RegisterDesc(desc);
	
	CREseWalkingControllerDesc* pCREseWalkingControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CREseWalkingControllerDesc");
	desc->size = sizeof(CREseWalkingControllerDesc);
	desc->ifInfo = CREseWalkingControllerIf::GetIfInfoStatic();
	((IfInfo*)CREseWalkingControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CREseWalkingControllerDesc>;
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCREseWalkingControllerDesc - (char*)pCREseWalkingControllerDesc);
	db->RegisterDesc(desc);
	
	CRTravelControllerDesc* pCRTravelControllerDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRTravelControllerDesc");
	desc->size = sizeof(CRTravelControllerDesc);
	desc->ifInfo = CRTravelControllerIf::GetIfInfoStatic();
	((IfInfo*)CRTravelControllerIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRTravelControllerDesc>;
	field = desc->AddBase("CRControllerDesc");
	field->offset = int((char*)(CRControllerDesc*)pCRTravelControllerDesc - (char*)pCRTravelControllerDesc);
	db->RegisterDesc(desc);
	
	CRCreatureDesc* pCRCreatureDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRCreatureDesc");
	desc->size = sizeof(CRCreatureDesc);
	desc->ifInfo = CRCreatureIf::GetIfInfoStatic();
	((IfInfo*)CRCreatureIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRCreatureDesc>;
	db->RegisterDesc(desc);
	
	CRInternalSceneObjectDesc* pCRInternalSceneObjectDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRInternalSceneObjectDesc");
	desc->size = sizeof(CRInternalSceneObjectDesc);
	desc->ifInfo = CRInternalSceneObjectIf::GetIfInfoStatic();
	((IfInfo*)CRInternalSceneObjectIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRInternalSceneObjectDesc>;
	field = desc->AddField("pointer", "PHSolidIf", "solid", "");
	field->offset = int((char*)&(pCRInternalSceneObjectDesc->solid) - (char*)pCRInternalSceneObjectDesc);
	field = desc->AddField("", "Vec3f", "position", "");
	field->offset = int((char*)&(pCRInternalSceneObjectDesc->position) - (char*)pCRInternalSceneObjectDesc);
	db->RegisterDesc(desc);
	
	CRISAttractiveObjectDesc* pCRISAttractiveObjectDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRISAttractiveObjectDesc");
	desc->size = sizeof(CRISAttractiveObjectDesc);
	desc->ifInfo = CRISAttractiveObjectIf::GetIfInfoStatic();
	((IfInfo*)CRISAttractiveObjectIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRISAttractiveObjectDesc>;
	field = desc->AddBase("CRInternalSceneObjectDesc");
	field->offset = int((char*)(CRInternalSceneObjectDesc*)pCRISAttractiveObjectDesc - (char*)pCRISAttractiveObjectDesc);
	field = desc->AddField("", "float", "attractiveness", "");
	field->offset = int((char*)&(pCRISAttractiveObjectDesc->attractiveness) - (char*)pCRISAttractiveObjectDesc);
	db->RegisterDesc(desc);
	
	CRISTravelPotentialObjectDesc* pCRISTravelPotentialObjectDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRISTravelPotentialObjectDesc");
	desc->size = sizeof(CRISTravelPotentialObjectDesc);
	desc->ifInfo = CRISTravelPotentialObjectIf::GetIfInfoStatic();
	((IfInfo*)CRISTravelPotentialObjectIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRISTravelPotentialObjectDesc>;
	field = desc->AddBase("CRInternalSceneObjectDesc");
	field->offset = int((char*)(CRInternalSceneObjectDesc*)pCRISTravelPotentialObjectDesc - (char*)pCRISTravelPotentialObjectDesc);
	field = desc->AddField("", "Vec2f", "strength", "");
	field->offset = int((char*)&(pCRISTravelPotentialObjectDesc->strength) - (char*)pCRISTravelPotentialObjectDesc);
	field = desc->AddField("", "Vec2f", "decay", "");
	field->offset = int((char*)&(pCRISTravelPotentialObjectDesc->decay) - (char*)pCRISTravelPotentialObjectDesc);
	db->RegisterDesc(desc);
	
	CRInternalSceneDesc* pCRInternalSceneDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRInternalSceneDesc");
	desc->size = sizeof(CRInternalSceneDesc);
	desc->ifInfo = CRInternalSceneIf::GetIfInfoStatic();
	((IfInfo*)CRInternalSceneIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRInternalSceneDesc>;
	db->RegisterDesc(desc);
	
	CRSensorDesc* pCRSensorDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CRSensorDesc");
	desc->size = sizeof(CRSensorDesc);
	desc->ifInfo = CRSensorIf::GetIfInfoStatic();
	((IfInfo*)CRSensorIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CRSensorDesc>;
	db->RegisterDesc(desc);
	
	CROpticalSensorDesc* pCROpticalSensorDesc = NULL;
	desc = DBG_NEW UTTypeDesc("CROpticalSensorDesc");
	desc->size = sizeof(CROpticalSensorDesc);
	desc->ifInfo = CROpticalSensorIf::GetIfInfoStatic();
	((IfInfo*)CROpticalSensorIf::GetIfInfoStatic())->desc = desc;
	desc->access = DBG_NEW UTAccess<CROpticalSensorDesc>;
	field = desc->AddBase("CRSensorDesc");
	field->offset = int((char*)(CRSensorDesc*)pCROpticalSensorDesc - (char*)pCROpticalSensorDesc);
	db->RegisterDesc(desc);
