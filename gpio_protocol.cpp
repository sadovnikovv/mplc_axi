#include "gpio_protocol.h"
#include <iostream>
#include <fstream>

namespace axi {

	////������������� ������ ��
	GpioProtocol::GpioProtocol(lua_State* L, const std::string& name) : DriverFB<GpioProtocol>(L, name) {

	}

	GpioProtocol::~GpioProtocol() {
	}

	void GpioProtocol::Execute() {
		std::vector<WriteDataRec> write_list;
	
		for (ChannelList::iterator it = channels.begin(); it != channels.end(); ++it) { // ������ � ������
			ChannelInfo::ptr& ch = *it;
			OpcUa_VariantHlp value;
			if(ch->Direction == DriverVarDsc::Input) {
				std::string val;
				
				std::string val_path = '/' + path + '/' + ch->AddressSPI;
				char buffer[] = { 0x10,0x00,0x11,0x00 };

				std::ifstream file(val_path);
				if (!file.is_open()) {
					OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
						"AxiProtocol::Execute: SPI_ID=%s\n%s\n",
						ch->AddressSPI.c_str(),
						"OPERATION FAILED: Unable to get the value of GPIO ");
					continue;
				}
				
				
				buffer >> file;
				file >> buffer;
				val = new string(buffer);
				
				
				file.close();
				WriteDataRec rec;
				rec._itemId = ch->Id;
				rec._path = ch->Path;
				rec._value.SetString(val != "0");
				write_list.push_back(rec);
				continue;
			}
		}


		if (!write_list.empty()) {
			_DataProvider->WriteValues(write_list); //���������� ������ � ������� Input
		}
	}
	void GpioProtocol::Inited(lua_State* L) {
		path = GetString("Path");
		std::cout << "SpiProtocol INITED! Path = " << path << std::endl;
	}
	void GpioProtocol::AddChannel(int64_t read_id, char const* read_path,
								  int64_t write_id, char const* write_path, 
		const MapStringToVariant& fields) {
		MapStringToVariant::const_iterator name_var = fields.find("Name"); //��� ������
		std::string tmp;
		name_var->second.GetString(tmp);
		MapStringToVariant::const_iterator nodeIdVar = fields.find("AddressSPI");//��������� � �����
		if (nodeIdVar == fields.end() || nodeIdVar->second.GetType() != OpcUaType_String)
			return;
		ChannelInfo::ptr ch = ChannelInfo::make();
		nodeIdVar->second.GetString(ch->AddressSPI); // ��������� ��������
		ch->Id = read_id;
		ch->Path = read_path;
		ch->WriteId = write_id;
		ch->WritePath = write_path;
		ch->Direction = write_id ? read_id ? 2 : 1 : 0;

		channels.push_back(ch);
	}

	REGISTER_FB_IMPLWITH_NAME(GpioProtocol, "GPIO") //����������� ��
}
