#pragma once
#include <share/driver_fb.h>
#include <string>
using namespace std;
namespace axi {
	struct ChannelInfo : DriverVarDsc {
		typedef boost::shared_ptr<ChannelInfo> ptr;
		static ptr make() { return boost::make_shared<ChannelInfo>(); }

		std::string AddressSPI;
	};

	typedef std::list<ChannelInfo::ptr> ChannelList;
}
