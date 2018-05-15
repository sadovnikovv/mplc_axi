#pragma once
#include  <share/driver_fb.h>
#include "types.h"

namespace axi  {
	class GpioProtocol final : public DriverFB<GpioProtocol> { // Основа ФБ
															   //Поправить макрасы
		BEGIN_PARAM_MAP()
            IN_PARAM("Path", STRING)
		END_PARAM_MAP()
		ChannelList channels;
		std::string path;
	public:
		GpioProtocol(lua_State* L, const std::string& name);
		virtual ~GpioProtocol();
		void Inited(lua_State* L);
		void Execute() override;
		void AddChannel(int64_t readVarID, const char *readVarPath, int64_t writeVarID,
			const char *writeVarPath, const MapStringToVariant& fields);
	};
}
