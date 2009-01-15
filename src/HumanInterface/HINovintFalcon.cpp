#include "HumanInterface.h"
#ifdef _WIN32
 #pragma hdrstop
#endif
#include "HINovintFalcon.h"
#include <WinBasis/WinBasis.h>
#include <WinBasis/WBDllLoader.h>

namespace Spr {;

#define HDLAPIENTRY  __stdcall

typedef int HDLDeviceHandle;
#define HDL_INVALID_HANDLE -1

typedef int HDLError;
#define HDL_NO_ERROR 0x0                  /**< No errors on error stack */
#define HDL_ERROR_STACK_OVERFLOW 0x01     /**< Overflow of error stack */
#define HDL_ERROR_INIT_FAILED    0x10     /**< Device initialization error */

#define HDAL_NOT_CALIBRATED    0x04

extern "C"{
//	DLL�X�^�u�̎���
static WBDllLoader dllLoader("hdl.dll");	//	�O���[�o���ϐ��Ń��[�_�[�����D

//	__declspec(dllexport) HDLDeviceHandle HDLAPIENTRY hdlInitNamedDevice(const char* deviceName, const char* configPath = 0);
#define DLLFUNC_RTYPE	HDLDeviceHandle						//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlInitNamedDevice					//	�֐���
#define DLLFUNC_STR		"_hdlInitNamedDevice@8"				//	�֐���
#define DLLFUNC_ARGDEF	(const char* deviceName, const char* configPath)	//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(deviceName, configPath)			//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) int HDLAPIENTRY hdlGetState();
#define DLLFUNC_RTYPE	int									//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlGetState							//	�֐���
#define DLLFUNC_STR		"_hdlGetState@0"					//	�֐���
#define DLLFUNC_ARGDEF	()									//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	()									//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void hdlUninitDevice(HDLDeviceHandle hHandle);
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlUninitDevice						//	�֐���
#define DLLFUNC_STR		"_hdlUninitDevice@4"				//	�֐���
#define DLLFUNC_ARGDEF	(HDLDeviceHandle hHandle)			//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(hHandle)							//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void hdlSetToolForce(double force[3]);
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlSetToolForce						//	�֐���
#define DLLFUNC_STR		"_hdlSetToolForce@4"				//	�֐���
#define DLLFUNC_ARGDEF	(double force[3])					//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(force)								//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void hdlToolPosition(double force[3]);
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlToolPosition						//	�֐���
#define DLLFUNC_STR		"_hdlToolPosition@4"				//	�֐���
#define DLLFUNC_ARGDEF	(double pos[3])						//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(pos)								//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void hdlToolButtons(int* pButtons);
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlToolButtons						//	�֐���
#define DLLFUNC_STR		"_hdlToolButtons@4"					//	�֐���
#define DLLFUNC_ARGDEF	(int * pButtons)					//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(pButtons)							//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>
}

//	__declspec(dllexport) HDLError HDLAPIENTRY hdlGetError();
#define DLLFUNC_RTYPE	HDLError							//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlGetError							//	�֐���
#define DLLFUNC_STR		"_hdlGetError@0"					//	�֐���
#define DLLFUNC_ARGDEF	()									//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	()									//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void HDLAPIENTRY hdlStart();
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlStart							//	�֐���
#define DLLFUNC_STR		"_hdlStart@0"						//	�֐���
#define DLLFUNC_ARGDEF	()									//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	()									//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void HDLAPIENTRY hdlStop();
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlStop								//	�֐���
#define DLLFUNC_STR		"hdlStop"							//	�֐���
#define DLLFUNC_ARGDEF	()									//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	()									//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>

//	__declspec(dllexport) void hdlMakeCurrent(HDLDeviceHandle hHandle);
#define DLLFUNC_RTYPE	void								//	�Ԃ�^
#define DLLFUNC_CTYPE	HDLAPIENTRY							//	�Ăяo���^
#define DLLFUNC_NAME	hdlMakeCurrent						//	�֐���
#define DLLFUNC_STR		"_hdlMakeCurrent@4"					//	�֐���
#define DLLFUNC_ARGDEF	(HDLDeviceHandle hHandle)			//	�֐��錾���̈���
#define DLLFUNC_ARGCALL	(hHandle)							//	�֐��Ăяo���̈���
#include <WinBasis/WBDllLoaderImpl.h>



HINovintFalcon::HINovintFalcon():deviceHandle(HDL_INVALID_HANDLE),button(0), good(false){
}
HINovintFalcon::~HINovintFalcon(){
	hdlUninitDevice(deviceHandle);
	deviceHandle = HDL_INVALID_HANDLE;
}
bool testHDLError(char* str){
    HDLError err = hdlGetError();
	// Check for error.
    if (err != HDL_NO_ERROR)
    {
		// Display message.
		DSTR << "Error: Novient Falcon - " << str << std::endl;
		return false;
    }
	return true;
}
bool isFalconCalibrated()
{
	// Call function to check the state of hdl.
	// State is return as bitmask.
    unsigned int state = hdlGetState();
	// Return true of the NOT_CALIBRATED bit is not set.
	return (state & HDAL_NOT_CALIBRATED) == 0;
}
void calibrateFalcon(){
	DSTR << "Please calibrate the Falcon by extending then pushing the arms all the way in.\nThe Falcon is calibrated when the light changes from RED to BLUE." << std::endl;
	int i;
	int n = 30;
	for (i=0; i< n && !isFalconCalibrated(); ++i){
		Sleep(100);
		DSTR << ".";
	}
	if (i<n)	DSTR << "Calibration finished." << std::endl;
	else DSTR << "Calibration failed!! make sure your Falcon is in order." << std::endl;
}
bool HINovintFalcon::Init(const void* desc){
	dllLoader.Load();
	deviceHandle = hdlInitNamedDevice(NULL, NULL);
	// Check return value of the handle.
    if (deviceHandle == HDL_INVALID_HANDLE){
		// There was an error trying to initialize the device.
		DSTR << "Error: Novient Falcon - Could not open device." << std::endl;
		return false;
    }

	// Start the servo thread
	hdlStart();
	testHDLError("Could not start servo thread");

	// Sleep for 1sec to allow everything to start running.
	Sleep(1000);

	// Make the device associated with the returned handle
	// the current device.  All subsequent calls will
    // be directed towards the current device.
    hdlMakeCurrent(deviceHandle);
    testHDLError("Could not make Falcon device current");

	calibrateFalcon();
	good = true;
	return true;
}
bool HINovintFalcon::BeforeCalibration(){
	return true;
}
bool HINovintFalcon::Calibration(){
	calibrateFalcon();
	return true;
}
int HINovintFalcon::GetButton(int ch){
	return (button & (1<<ch)) != 0 ;
}
int HINovintFalcon::GetButtonBit(){
	return button;
}
void HINovintFalcon::Update(float dt){
	if (good){
		Vec3d p;
		hdlToolPosition(p);
		pos = p;
		hdlToolButtons(&button);
		Vec3d f = force;
		hdlSetToolForce(f);
	}
	HIForceInterface6D::Update(dt);
}

}//	namespace Spr
