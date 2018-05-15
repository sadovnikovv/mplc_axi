#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <regex>

#if defined(_WIN32)
#else
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
// MPLC
#include <share/mplcshare.h>
#include <drivers/drv_user.h>
#include <addincmn.h>
#include <share/driver_fb.h>

#include "GPIOClass.h"

#define MYDEVICE_BUF_SIZE 100
#define MYDEVICE_TIMEOUT 1000

#define  AXI_DRIVER_TYPE 52 

typedef struct MyDeviceCB {
	char SendBuf[MYDEVICE_BUF_SIZE];
	char RecvBuf[MYDEVICE_BUF_SIZE];
	RFLOAT PTR pFloat;
} MyDeviceCB;

using namespace std;

int Axi_MyDevice(DRIVER_MODE mode, SERIAL_TASK_CB PTR pTaskCB)
{

	if (mode == dmInit) {
		MyDeviceCB PTR pCB;
		int index, countA = 0, countD = 0;

		pTaskCB->pDriverCB = (BYTE *) GetMem(sizeof(MyDeviceCB));
		if (!pTaskCB->pDriverCB) {
			PRINT0("Driver_MyDevice. mem allocation error");
			CR;
			return E_FAIL;
		}
		pCB = (MyDeviceCB PTR) pTaskCB->pDriverCB;

		WORD modules = pTaskCB->QuanModules;

		for (index = 0; index < modules; ++index) {

			DRV_MODULE PTR pModule = GetSerialModule(pTaskCB, index);
			int c = pModule->QuanParams;
			int* pChannelIDs = (int*) GetPropertyValues(pModule->Property,
				propCatSysProperties, propIDChannelIDs);
			for (int pn = 0; pn < c; pn++) {
				int direction = GetModuleParamDirection(pModule, pn);
				int channelId = pChannelIDs != NULL ? pChannelIDs[pn] : 0;
				RPARAM PTR param = GetModuleParam(pModule, pn);

				GPIOClass* gpio = new GPIOClass(std::to_string(channelId));
				gpio->export_gpio();

				switch (direction) {
				case 1:
					gpio->setdir_gpio("in");
					break;
				case 0:
					gpio->setdir_gpio("out");
					break;
				}
			}
			countA += c;
		}

		pCB->pFloat = (RFLOAT *) GetMem(sizeof(RFLOAT) * countA);
		if (!pCB->pFloat) {
			PRINT0("Driver_MyDevice. mem allocation error");
			CR;
			FreeMem(pTaskCB->pDriverCB);
			return E_FAIL;
		}

		for (index = 0; index < countA; ++index) {
			pCB->pFloat[index] = index;
		}

		return S_OK;
	}

	if (mode == dmDone) {
		MyDeviceCB PTR pCB = (MyDeviceCB PTR) (pTaskCB->pDriverCB);
		FreeMem(pCB->pFloat);
		FreeMem(pTaskCB->pDriverCB);
		return S_OK;
	}

	if (mode == dmRead) {
		int module, param;
		MyDeviceCB PTR pCB = (MyDeviceCB PTR) pTaskCB->pDriverCB;

		for (module = 0; module < pTaskCB->QuanModules; module++) {
			DRV_MODULE PTR pModule = GetSerialModule(pTaskCB, module);
			RTIME CurTime;

			RGetDateTime(&CurTime);

			int c = pModule->QuanParams;
			int* pChannelIDs = (int*) GetPropertyValues(pModule->Property,
				propCatSysProperties, propIDChannelIDs);
			double i;
			string value;
			for (param = 0; param < c; ++param) {
				int channelId = pChannelIDs != NULL ? pChannelIDs[param] : 0;
				GPIOClass* gpio = new GPIOClass(std::to_string(channelId));
				gpio->getval_gpio(value);
				i = atof(value.c_str());
				CHECK_RESULT(iWriteFloatGood(GetModuleParam(pModule, param),
					(RFLOAT) i));
			}

		}

		return S_OK;
	}

	if (mode == dmWrite || mode == dmWriteByChange) {
		int module, param;
		MyDeviceCB PTR pCB = (MyDeviceCB PTR) pTaskCB->pDriverCB;
		int out_param = -1;

		for (module = 0; module < pTaskCB->QuanModules; module++) {
			DRV_MODULE PTR pModule = GetSerialModule(pTaskCB, module);
			int c = pModule->QuanParams;
			int* pChannelIDs = (int*) GetPropertyValues
				(pModule->Property, propCatSysProperties,
				propIDChannelIDs);
			for (param = 0; param < c; param++) {
				RFLOAT value;
				int direction = GetModuleParamDirection(pModule, param);
				out_param++;
				if (iReadFloat(GetModuleParam
					(pModule, param), &value) != S_OK)
					continue;

				if (mode == dmWriteByChange &&
					std::memcmp(GetModuleParam(pModule, param),
					&pTaskCB->pOutParams[out_param], sizeof(RPARAM)) == 0)
					continue; //Value was not changed

				pCB->pFloat[param] = value;

				if (direction == 0) {
					int channelId = pChannelIDs != NULL ? pChannelIDs[param] : 0;
					GPIOClass* gpio = new GPIOClass(std::to_string(channelId));
					gpio->setval_gpio(std::to_string(value));
				}
				SetSerialTaskAlive(pTaskCB);
			}
			out_param--;
		}
		return S_OK;
	}

	return E_FAIL;
}

EXTERN_C MPLC_DRIVER_API int InitAddin(ProcessRequestCallback func,
	int nInFlags, int* pnOutFlags)
{
	*pnOutFlags = 1;
	PRINT("------------------Init driver--------------------\n\r");
	if (RegisterDriverFunction(AXI_DRIVER_TYPE, Axi_MyDevice) == 0) {
		PRINT("------------------Init driver failed--------------------\n\r");
		return 1;
	}
	return 0;
}

