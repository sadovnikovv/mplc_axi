#include "gpio_protocol.h"
#include <iostream>
#include <fstream>
namespace axi {

	////Инициализация входов ФБ
	GpioProtocol::GpioProtocol(lua_State* L, const std::string& name) : DriverFB<GpioProtocol>(L, name) {

	}

	GpioProtocol::~GpioProtocol() {
	}

	void GpioProtocol::Execute() {
		std::vector<WriteDataRec> write_list;
		for (ChannelList::iterator it = channels.begin(); it != channels.end(); ++it) { // Запись в каналы
			ChannelInfo::ptr& ch = *it;
			OpcUa_VariantHlp value;
			if(ch->Direction == DriverVarDsc::Input) {
				std::string val;
				std::string val_path = path + '/' + ch->AddressGPIO;
				std::ifstream file(val_path);
				if (!file.is_open()) {
					OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
						"AxiProtocol::Execute: GPIO_ID=%s\n%s\n",
						ch->AddressGPIO.c_str(),
						"OPERATION FAILED: Unable to get the value of GPIO ");
					continue;
				}
				file >> val;
				file.close();
				WriteDataRec rec;
				rec._itemId = ch->Id;
				rec._path = ch->Path;
				rec._value.SetBool(val != "0");
				write_list.push_back(rec);
				continue;
			}
			if (ch->Direction == DriverVarDsc::Output) { 
				_DataProvider->ReadValue(ch->WriteId, 0, ch->WritePath, std::string(), OpcUaType_Null, &value);
				if (ch->ValueToWrite.Compare(value) != 0 || ch->NeedToWrite == true) {
					std::string val_path = path + '/' + ch->AddressGPIO;
					std::ofstream file(val_path);
					if (!file.is_open()) {
						OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,
							"AxiProtocol::Execute: GPIO_ID=%s\n%s\n",
							ch->AddressGPIO.c_str(),
							"OPERATION FAILED: Unable to set the value of GPIO ");
						continue;
					}
					
					if (value.GetType() == OpcUaType_String) {
						std::string val_path;
						value.GetString(val_path);
						file << val_path;
					}
					else {
						bool bool_val;
						value.GetBool(bool_val);//string_val
						file << bool_val ? '1' : '0';
					}
					
					file.close();
				}
			}
		}
		if (!write_list.empty()) {
			_DataProvider->WriteValues(write_list); //Обновление данных в каналах Input
		}
	}
	void GpioProtocol::Inited(lua_State* L) {
		path = GetString("Path");
		std::cout << "GpioProtocol INITED! Path = " << path << std::endl;
	}
	void GpioProtocol::AddChannel(int64_t read_id, char const* read_path,
								  int64_t write_id, char const* write_path, 
		const MapStringToVariant& fields) {
		MapStringToVariant::const_iterator name_var = fields.find("Name"); //имя канала
		std::string tmp;
		name_var->second.GetString(tmp);
		MapStringToVariant::const_iterator nodeIdVar = fields.find("AddressGPIO");//Настройка в скаде
		if (nodeIdVar == fields.end() || nodeIdVar->second.GetType() != OpcUaType_String)
			return;
		ChannelInfo::ptr ch = ChannelInfo::make();
		nodeIdVar->second.GetString(ch->AddressGPIO); // Получение значения
		ch->Id = read_id;
		ch->Path = read_path;
		ch->WriteId = write_id;
		ch->WritePath = write_path;
		ch->Direction = write_id ? read_id ? 2 : 1 : 0;

		channels.push_back(ch);
	}

	REGISTER_FB_IMPLWITH_NAME(GpioProtocol, "GPIO") //Регистрация ФБ
}
