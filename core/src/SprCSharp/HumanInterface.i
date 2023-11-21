#	Do not edit. RunSwig.bat will update this file.
%ignore Spr::HIRealDeviceIf::Rent;
// %ignore Spr::HISdkIf::AddRealDevice;
%ignore Spr::HISdkIf::FindRealDevice;
%ignore Spr::HISdkIf::RentVirtualDevice;
//%ignore Spr::HISdkIf::CreateHumanInterface;
//?
%ignore Spr::HISkeletonIf::GetTrackID;
%module HumanInterface
%include "../../include/SprBase.h"
%include "../../include/Base/Env.h"
%include "../../include/Foundation/SprObject.h"
%include "../../include/Foundation/SprScene.h"
%include "../../include/HumanInterface/SprHIBase.h"
%include "../../include/HumanInterface/SprHIDevice.h"
%include "../../include/HumanInterface/SprHIDRUsb.h"
%include "../../include/HumanInterface/SprHIDRUart.h"
%include "../../include/HumanInterface/SprHIJoyStick.h"
%include "../../include/HumanInterface/SprHIKeyMouse.h"
%include "../../include/HumanInterface/SprHISkeletonSensor.h"
%include "../../include/HumanInterface/SprHILeap.h"
%include "../../include/HumanInterface/SprHINovint.h"
%include "../../include/HumanInterface/SprHISdk.h"
%include "../../include/HumanInterface/SprHISpaceNavigator.h"
%include "../../include/HumanInterface/SprHISpidar.h"
%include "../../include/HumanInterface/SprHIKorokoro.h"
%include "../../include/HumanInterface/SprHITrackball.h"
%include "../../include/HumanInterface/SprHIXbox360Controller.h"
/*
#define DOUBLECOLON :: 
%include "../../include/Springhead.h"
%include "../../include/base/Env.h"
%include "../../include/Foundation/SprObject.h"
%include "../../include/Foundation/SprScene.h"
%include "../../include/Foundation/SprUTTimer.h"
%include "../../include/HumanInterface/SprHIBase.h"
%include "../../include/HumanInterface/SprHIDevice.h"
%include "../../include/HumanInterface/SprHIDRUsb.h"
%include "../../include/HumanInterface/SprHIJoyStick.h"
%include "../../include/HumanInterface/SprHIKeyMouse.h"
%include "../../include/HumanInterface/SprHILeap.h"
%include "../../include/HumanInterface/SprHINovint.h"
%include "../../include/HumanInterface/SprHISdk.h"
%include "../../include/HumanInterface/SprHISkeletonSensor.h"
%include "../../include/HumanInterface/SprHISpaceNavigator.h"
%include "../../include/HumanInterface/SprHISpidar.h"
%include "../../include/HumanInterface/SprHIKorokoro.h"
%include "../../include/HumanInterface/SprHITrackball.h"
%include "../../include/HumanInterface/SprHIXbox360Controller.h"
%include "../../include/Base/BaseDebug.h"
%include "../../src/Foundation/UTTypeDesc.h"
%include "../Foundation/Foundation.h"
%include "../Foundation/Object.h"
%include "../Foundation/Scene.h"
%include "../Foundation/UTBaseType.h"
%include "../Foundation/UTClapack.h"
%include "../Foundation/UTDllLoader.h"
%include "../Foundation/UTDllLoaderImpl.h"
%include "../Foundation/UTLoadContext.h"
%include "../Foundation/UTLoadHandler.h"
%include "../Foundation/UTMMTimer.h"
%include "../Foundation/UTPath.h"
%include "../Foundation/UTPreciseTimer.h"
%include "../Foundation/UTQPTimer.h"
%include "../Foundation/UTSocket.h"
%include "../Foundation/UTTimer.h"
%include "../Foundation/UTTypeDesc.h"
%include "../HumanInterface/Device.h"
%include "../HumanInterface/DRADAPIO.h"
%include "../HumanInterface/DRContecIsaCounter.h"
%include "../HumanInterface/DRContecIsaDa.h"
%include "../HumanInterface/DRCyUsb20Sh4.h"
%include "../HumanInterface/DRJoyStickGLUT.h"
%include "../HumanInterface/DRKeyMouseGLUT.h"
%include "../HumanInterface/DRKeyMouseWin32.h"
%include "../HumanInterface/DRNittaForce.h"
%include "../HumanInterface/DRPortIO.h"
%include "../HumanInterface/DRTokin3D.h"
%include "../HumanInterface/DRUPP.h"
%include "../HumanInterface/DRUsb20Sh4.h"
%include "../HumanInterface/DRUsb20Simple.h"
%include "../HumanInterface/DRUsbH8Simple.h"
%include "../HumanInterface/DRUsbH8SimpleDef.h"
%include "../HumanInterface/DRWinDriver.h"
%include "../HumanInterface/HIBase.h"
%include "../HumanInterface/HIDevice.h"
%include "../HumanInterface/HILeap.h"
%include "../HumanInterface/HINovintFalcon.h"
%include "../HumanInterface/HISdk.h"
%include "../HumanInterface/HISkeletonSensor.h"
%include "../HumanInterface/HISpaceNavigator.h"
%include "../HumanInterface/HISpidar.h"
%include "../HumanInterface/HIKorokoro.h"
%include "../HumanInterface/HISpidarCalc.h"
%include "../HumanInterface/HISpidarMotor.h"
%include "../HumanInterface/HIKorokoroMotor.h"
%include "../HumanInterface/HITrackball.h"
%include "../HumanInterface/HIXbox360Controller.h"
%include "../HumanInterface/HumanInterface.h"
*/
