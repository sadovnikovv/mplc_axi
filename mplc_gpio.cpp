
#include "gpio_protocol.h"
#include "drivers/drv_user.h"
namespace axi {
	REGISTER_FB_DECLARE(GpioProtocol);  // Обявление функции инициализации

	int Init() {
		REGISTER_FB_CALL(GpioProtocol); 
		return 0;
	}
}

EXTERN_C MPLC_DRIVER_API int InitAddin(ProcessRequestCallback func, int nInFlags, int* pnOutFlags) {
	return axi::Init();
}
