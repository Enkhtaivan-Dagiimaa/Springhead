/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this
 *  software. Please deal with this software under one of the following licenses:
 *  This license itself, Boost Software License, The MIT License, The BSD License.
 */
#include <Physics/PHTrackingEngine.h>
#ifdef USE_CLOSED_SRC
#include "../../closed/include/PliantMotion/PliantMotion.h"
#endif

using namespace std;
namespace Spr {

	void PHTrackingEngine::Step() {
		DSTR << "PHTrackingEngine Called" << endl;
		//TrackWithForce();
	}

	void PHTrackingEngine::Clear() {
	}

	bool PHTrackingEngine::AddChildObject(ObjectIf* o) {
		return false;
	}

	bool PHTrackingEngine::DelChildObject(ObjectIf* o) {
		return false;
	}

	void PHTrackingEngine::SetTrackingInputs(PHRootNodeIf* root, Vec3d rootPosition ,std::vector<Quaterniond> inputs) {
		ABATrackingInput* abaTrackingInput = SearchABATrackingInput(root);
		if (abaTrackingInput == NULL) {
			ABATrackingInput newABATrackingInput;
			newABATrackingInput.root = root;
			abaTrackingInputs.push_back(newABATrackingInput);
		}
		abaTrackingInput = SearchABATrackingInput(root);
		abaTrackingInput->trackingRootPosition = rootPosition;
		abaTrackingInput->inputs.clear();
		abaTrackingInput->inputs = inputs;
	}

	void PHTrackingEngine::SetTrackingRootPosition(PHRootNodeIf* root, Vec3d input) {
		ABATrackingInput newABATrackingInput;
		newABATrackingInput.root = root;
		newABATrackingInput.trackingRootPosition = input;
		abaTrackingInputs.push_back(newABATrackingInput);
		//trackingRootPosition = input;
	}

	void PHTrackingEngine::AddTrackingNode(PHRootNodeIf* root,PHBallJointIf* reactJoint, PHBallJointIf* calcJoint, PHSolidIf* reactRootSolid, PHSolidIf* calcRootSolid, bool isRoot) {
		TrackingNode newTrackingNode;
		newTrackingNode.reactJoint = reactJoint;
		newTrackingNode.calcJoint = calcJoint;
		newTrackingNode.reactRootSolid = reactRootSolid;
		newTrackingNode.calcRootSolid = calcRootSolid;
		newTrackingNode.isRoot = isRoot;
		newTrackingNode.parent = NULL;

		ABATrackingNode* abaTrackingNode = SearhABATrackingNode(root);
		if (abaTrackingNode == NULL) {
			ABATrackingNode newABATrackingNode;
			newABATrackingNode.root = root;
			abaTrackingNodes.push_back(newABATrackingNode);
			abaTrackingNode = SearhABATrackingNode(root);
		}
		std::vector<TrackingNode>& trackingNodes = abaTrackingNode->trackingNodes;
		trackingNodes.push_back(newTrackingNode);

		if (!isRoot) {
			for (int i = 0; i < trackingNodes.size(); i++) {
				if (trackingNodes[i].isRoot && trackingNodes[i].reactRootSolid == reactJoint->GetSocketSolid()) {
					newTrackingNode.parent = &trackingNodes[i];
					break;
				} else if (!trackingNodes[i].isRoot && trackingNodes[i].reactJoint->GetPlugSolid() == reactJoint->GetSocketSolid()) {
					newTrackingNode.parent = &trackingNodes[i];
					break;
				}
			}
		}

		// vector型は追加するたびにアドレスが変更されるためparentについては毎回更新が必要
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) continue;
			for (TrackingNode& parent : trackingNodes) {
				if (parent.isRoot && parent.reactRootSolid == trackingNode.reactJoint->GetSocketSolid()) {
					trackingNode.parent = &parent;
				} else if (!parent.isRoot && parent.reactJoint->GetPlugSolid() == trackingNode.reactJoint->GetSocketSolid()) {
					trackingNode.parent = &parent;
				}
			}
		}
		//PliantMotion *p = new PliantMotion();
		//p->SetTrackingInput();
	}

	SpatialVector PHTrackingEngine::GetTipAcceleration(PHRootNodeIf* root, int i) {
		ABATrackingNode* abaTrackingNode = SearhABATrackingNode(root);
		return abaTrackingNode->trackingNodes[i].targetAcceleration;
	}

	void PHTrackingEngine::TrackWithForce(PHRootNodeIf* root) {
		double timeStep = DCAST(PHScene, GetScene())->timeStep;
		ABATrackingNode* abaTrackingNode = SearhABATrackingNode(root);
		std::vector<TrackingNode>& trackingNodes = abaTrackingNode->trackingNodes;
		ABATrackingInput* abaTrackingInput = SearchABATrackingInput(root);
		std::vector<Quaterniond>& trackingInputs = abaTrackingInput->inputs;
		// 入力をtrackingNodeに書き込み
		for (int i = 0; i < trackingNodes.size(); i++) {
			trackingNodes[i].plugInput = trackingInputs[i];
		}
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) {
				trackingNode.rootPositionInput = abaTrackingInput->trackingRootPosition;
			}
		}
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.parent != NULL) {
				//if (trackingNode.parent->isRoot) {
				//	if (trackingNode.parent->reactRootSolid == NULL) {
				//		DSTR << trackingNode.reactJoint->GetName() << " null" << endl;
				//	}
				//	else {
				//		DSTR << trackingNode.reactJoint->GetName() << "'s parent " << trackingNode.parent->reactRootSolid->GetName() << endl;
				//	}
				//}
				//else {
				//	if (trackingNode.parent->reactJoint == NULL) {
				//		DSTR << trackingNode.reactJoint->GetName() << " null" << endl;
				//	}
				//	else {
				//		DSTR << trackingNode.reactJoint->GetName() << "'s parent " << trackingNode.parent->reactJoint->GetName() << endl;
				//	}
				//}
				trackingNode.socketInput = trackingNode.parent->plugInput;
			}
		}
		// local座標系の角速度を計算
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) {
				Vec3d diff = (trackingNode.plugInput * trackingNode.calcRootSolid->GetOrientation().Inv()).RotationHalf();
				trackingNode.localAngularVelocity = diff / timeStep;
				trackingNode.localAngularVelocityDot = (trackingNode.localAngularVelocity - trackingNode.calcRootSolid->GetAngularVelocity()) / timeStep;
				//DSTR << "localAngularVelocity:" << trackingNode.calcRootSolid->GetName() << " localAngularVelocity " << trackingNode.localAngularVelocity << " localAngularVelocityDot " << trackingNode.localAngularVelocityDot << endl;
			} else {
				PHSolidIf* calcSocketSolid = trackingNode.calcJoint->GetSocketSolid();
				PHSolidIf* calcPlugSolid = trackingNode.calcJoint->GetPlugSolid();
				Posed socketPose;
				trackingNode.calcJoint->GetSocketPose(socketPose);
				Posed plugPose;
				trackingNode.calcJoint->GetPlugPose(plugPose);
				Vec3d preLocalAngularVelocity = (calcSocketSolid->GetOrientation() * socketPose.Ori()).Inv() * (calcPlugSolid->GetAngularVelocity() - calcSocketSolid->GetAngularVelocity());
				//if (trackingNode.parent->isRoot) {
				//	preLocalAngularVelocity = (calcSocketSolid->GetOrientation() * socketPose.Ori()).Inv() * (calcSocketSolid->GetAngularVelocity() - trackingNode.parent->calcRootSolid->GetAngularVelocity());
				//}
				//else {
				//	preLocalAngularVelocity = (calcSocketSolid->GetOrientation() * socketPose.Ori()).Inv() * (calcSocketSolid->GetAngularVelocity() - trackingNode.parent->calcJoint->GetPlugSolid()->GetAngularVelocity());
				//}

				Quaterniond jointRotationSocketInput = trackingNode.socketInput * socketPose.Ori();
				Quaterniond jointRotationPlugInput = trackingNode.plugInput * plugPose.Ori();
				//DSTR << "trackingNode.socketInput" << trackingNode.socketInput << " " << socketPose.Ori() << "jointRotationSocketInput " << jointRotationSocketInput << " " << jointRotationPlugInput << endl;
				trackingNode.jointTargetRotation = jointRotationSocketInput.Inv() * jointRotationPlugInput;

				Quaterniond jointRotationPlugSolid = calcPlugSolid->GetOrientation() * plugPose.Ori();
				Quaterniond jointRotationSocketSolid = calcSocketSolid->GetOrientation() * socketPose.Ori();
				Vec3d diff = (trackingNode.jointTargetRotation * (jointRotationSocketSolid.Inv() * jointRotationPlugSolid).Inv()).RotationHalf();
				trackingNode.localAngularVelocity = diff / timeStep;
				trackingNode.localAngularVelocityDot = (trackingNode.localAngularVelocity - preLocalAngularVelocity) / timeStep;
				//DSTR << "preLocalAngularVelocity " << preLocalAngularVelocity << endl;
				//DSTR << "localAngularVelocityDot reactJoint:" << trackingNode.reactJoint->GetName() << " localAngularVelocity " << trackingNode.localAngularVelocity << " preLocalAngularVelocity " << preLocalAngularVelocity << " localAngularVelocityDot " << trackingNode.localAngularVelocityDot << endl;
			}
		}

		// Global座標系の角速度を計算
		for (TrackingNode& trackingNode : trackingNodes) {
			TrackingNode* node = &trackingNode;
			Vec3d globalAngularVelocity(0, 0, 0);
			while (true) {
				if (node->isRoot) {
					globalAngularVelocity += node->localAngularVelocity;
					break;
				}
				Posed socketPose;
				node->calcJoint->GetSocketPose(socketPose);
				Quaterniond convertLocal2Global = node->calcJoint->GetSocketSolid()->GetOrientation() * socketPose.Ori();
				globalAngularVelocity += convertLocal2Global * node->localAngularVelocity;
				node = node->parent;
			}
			trackingNode.globalAngularVelocity = globalAngularVelocity;
			//if (trackingNode.isRoot) {
			//	DSTR << "root :" << trackingNode.calcRootSolid->GetName() << " globalAngularVelocity " << trackingNode.globalAngularVelocity << endl;
			//}
			//else {
			//	DSTR << "calcJoint:" << trackingNode.calcJoint->GetName() << " globalAngularVelocity " << trackingNode.globalAngularVelocity << endl;
			//}
		}

		// Global座標系の角加速度を計算
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) {
				trackingNode.globalAngularVelocityDot = (trackingNode.globalAngularVelocity - trackingNode.calcRootSolid->GetAngularVelocity()) / timeStep;
			} else {
				trackingNode.globalAngularVelocityDot = (trackingNode.globalAngularVelocity - trackingNode.calcJoint->GetPlugSolid()->GetAngularVelocity()) / timeStep;
			}
			//if (trackingNode.isRoot) {
			//	DSTR << "globalAngularVelocityDot " << "root :" << trackingNode.calcRootSolid->GetName() << trackingNode.globalAngularVelocityDot << endl;
			//}
			//else {
			//	DSTR << "globalAngularVelocityDot " << "calcJoint:" << trackingNode.calcJoint->GetName() << trackingNode.globalAngularVelocityDot << endl;
			//}
		}

		// RootのDynamicalOffの剛体の加速度を計算
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) {
				trackingNode.rootAccelerration = Vec3d(0, 0, 0);
				Vec3d diffPos = trackingNode.rootPositionInput + trackingNode.plugInput * trackingNode.calcRootSolid->GetCenterOfMass() - trackingNode.calcRootSolid->GetCenterPosition();
				trackingNode.globalVelocity = diffPos / timeStep;
				trackingNode.rootAccelerration = (trackingNode.globalVelocity - trackingNode.calcRootSolid->GetVelocity()) / timeStep;
				//DSTR << "root :" << trackingNode.calcRootSolid->GetName() << " rootAcceleration  " << trackingNode.rootAccelerration << endl;
				break;
			}
		}

		// 加速度を計算
		for (TrackingNode& trackingNode : trackingNodes) {
			//trackingNode.globalA
			TrackingNode* node = &trackingNode;
			Vec3d tipPos;
			if (trackingNode.isRoot) {
				tipPos = trackingNode.calcRootSolid->GetCenterPosition();
			} else {
				tipPos = trackingNode.calcJoint->GetPlugSolid()->GetCenterPosition();
			}
			Vec3d globalAcceleration(0, 0, 0);
			while (true) {
				if (node->isRoot) {
					globalAcceleration += node->rootAccelerration + node->localAngularVelocityDot % (tipPos - node->calcRootSolid->GetCenterPosition());
					break;
				}
				Posed plugPose;
				node->calcJoint->GetPlugPose(plugPose);
				Vec3d jointPosition = (node->calcJoint->GetPlugSolid()->GetPose() * plugPose).Pos();
				Posed socketPose;
				node->calcJoint->GetSocketPose(socketPose);
				Quaterniond convertLocal2Global = node->calcJoint->GetSocketSolid()->GetOrientation() * socketPose.Ori();
				globalAcceleration += (convertLocal2Global * node->localAngularVelocityDot) % (tipPos - jointPosition);
				node = node->parent;
			}
			trackingNode.globalAcceleration = globalAcceleration;
			//if (trackingNode.isRoot) {
			//	DSTR << "globalAcceleration " << "root :" << trackingNode.calcRootSolid->GetName() << trackingNode.globalAcceleration << endl;
			//}
			//else {
			//	DSTR << "globalAcceleration " << "calcJoint:" << trackingNode.calcJoint->GetName() << trackingNode.globalAcceleration << endl;
			//}
		}

		// 目標角度を設定、DynamicalOffだけ加速度を反映
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) {
				SpatialVector dv = SpatialVector(trackingNode.calcRootSolid->GetOrientation().Inv() * trackingNode.rootAccelerration * timeStep,
					trackingNode.calcRootSolid->GetOrientation().Inv() * trackingNode.globalAngularVelocityDot * timeStep);
				trackingNode.calcRootSolid->SetDv(dv);
				trackingNode.reactRootSolid->SetDv(dv);
				PHRootNodeIf* calcRootNode = trackingNode.calcRootSolid->GetTreeNode()->Cast();
				PHRootNodeIf* reactRootNode = trackingNode.reactRootSolid->GetTreeNode()->Cast();
				Posed posed = Posed(abaTrackingInput->trackingRootPosition, trackingNode.plugInput);
				reactRootNode->SetUseNextPose(true);
				calcRootNode->SetUseNextPose(true);
				reactRootNode->SetNextPose(posed);
				calcRootNode->SetNextPose(posed);
				//DSTR << "dv " << dv << endl;
			} else {
				trackingNode.calcJoint->SetTargetPosition(trackingNode.preLocalTargetRotation);
				trackingNode.reactJoint->SetTargetPosition(trackingNode.preLocalTargetRotation);
				trackingNode.calcJoint->SetTargetVelocity(trackingNode.localAngularVelocity);
				trackingNode.reactJoint->SetTargetVelocity(trackingNode.localAngularVelocity);
				//DSTR << "preLocalTargetRotation " << "calcJoint:" << trackingNode.calcJoint->GetName() << trackingNode.preLocalTargetRotation << endl;
				//DSTR << "localAngularVelocity " << "calcJoint:" << trackingNode.calcJoint->GetName() << trackingNode.localAngularVelocity << endl;
			}
		}

		// 力とトルクを求めて追加
		for (TrackingNode& trackingNode : trackingNodes) {
			if (trackingNode.isRoot) {
				trackingNode.targetAcceleration = SpatialVector(trackingNode.calcRootSolid->GetOrientation().Inv() * trackingNode.rootAccelerration,
					trackingNode.calcRootSolid->GetOrientation().Inv() * trackingNode.globalAngularVelocityDot);
				//DSTR << "root tragetAcceleration" << trackingNode.targetAcceleration << trackingNode.calcRootSolid->GetOrientation() << trackingNode.rootAccelerration << trackingNode.globalAngularVelocityDot<<  endl;
			} else {
				PHBallJointNodeIf* calcTreeNode = trackingNode.calcJoint->GetPlugSolid()->GetTreeNode()->Cast();
				PHBallJointNodeIf* reactTreeNode = trackingNode.reactJoint->GetPlugSolid()->GetTreeNode()->Cast();
				AddTrackingForce(calcTreeNode, reactTreeNode, timeStep, trackingNode.localAngularVelocityDot, trackingNode.parent->targetAcceleration, trackingNode.targetAcceleration, trackingNode.trackingForce, trackingNode.trackingTorque);
				//DSTR << "trackingNode.targetAcceleration " << "calcJoint:" << trackingNode.calcJoint->GetName() << trackingNode.targetAcceleration << endl;
			}
		}

		// pre系を更新
		for (TrackingNode& trackingNode : trackingNodes) {
			trackingNode.preLocalTargetRotation = trackingNode.jointTargetRotation;
		}
	}
	void PHTrackingEngine::AddTrackingForce(PHBallJointNodeIf* calcNode,PHBallJointNodeIf* reactNode, double timeStep, Vec3d targetAngularAcceleration, SpatialVector parentTargetAcceleration, SpatialVector& targetAcceleration, Vec3d& force, Vec3d& torque) {
#ifdef USE_CLOSED_SRC
		PliantMotion* p = new PliantMotion();
		p->AddTrackingForce(calcNode->Cast(), reactNode, timeStep, targetAngularAcceleration, parentTargetAcceleration, targetAcceleration, force, torque);
#endif
	}
}
