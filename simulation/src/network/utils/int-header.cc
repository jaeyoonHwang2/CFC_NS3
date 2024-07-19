#include "int-header.h"

namespace ns3 {

const uint64_t IntHop::lineRateValues[8] = {25000000000lu,50000000000lu,100000000000lu,200000000000lu,400000000000lu,0,0,0};
uint32_t IntHop::multi = 1;

IntHeader::Mode IntHeader::mode = NONE;
int IntHeader::pint_bytes = 2;

IntHeader::IntHeader() : nhop(0) {
	for (uint32_t i = 0; i < maxHop; i++)
		hop[i] = {0};
	
	// CFC START: INT RELAY
	// if mode == CFC and rdmaHw maintains non-zero INT value (after 1 RTT)
	// CFC END: INT RELAY
	
	// // initial writeFile	
	// filePath = "/home/cfc/CFC_NS3_simulator/simulation/intHeader.txt";
	// writeFile_intHeader.open(filePath.c_str(), std::ios::app);
}

uint32_t IntHeader::GetStaticSize(){
	if (mode == NORMAL){
		return sizeof(hop) + sizeof(nhop);
	}else if (mode == TS){
		return sizeof(ts);
	}else if (mode == PINT){
		return sizeof(pint);
	}
	// CFC START: level 1
	else if (mode == CFC){
		return sizeof(hop) + sizeof(nhop);
	}
	// CFC END: level 1
	// // CFC START: level 2
	// }else if (mode == CFC){
	// 	return sizeof(cfc_hop) + sizeof(cfc_nhop);
	// // CFC END: level 2
	else {
		return 0;
	}
}

// void IntHeader::PushHop(uint64_t time, uint64_t bytes, uint32_t qlen, uint64_t rate){
// CFC START: level 1
void IntHeader::PushHop(uint64_t time, uint64_t bytes, uint32_t qlen, uint64_t rate, uint64_t port){
// CFC END: level 1
	// only do this in INT mode
	if (mode == NORMAL){
		uint32_t idx = nhop % maxHop;
		hop[idx].Set(time, bytes, qlen, rate);
		nhop++;
	}
	// CFC START: level 1
	else if (mode == CFC){
		uint32_t idx = nhop % maxHop;
		hop[idx].cfc_Set(port);
		nhop++;
	}
	// CFC END: level 1
}
// // CFC START: level 2
// void IntHeader::cfc_PushHop(uint32_t port_index){
// 	if (mode == CFC){
// 		uint32_t idx = cfc_nhop % maxHop;
// 		cfc_hop[idx].cfc_Set(port_index);
// 		cfc_nhop++;
// 	}	
// }
// // CFC END: level 2
void IntHeader::Serialize (Buffer::Iterator start) const{
	Buffer::Iterator i = start;
	if (mode == NORMAL){
		for (uint32_t j = 0; j < maxHop; j++){
			i.WriteU32(hop[j].buf[0]);
			i.WriteU32(hop[j].buf[1]);
		}
		i.WriteU16(nhop);
	}else if (mode == TS){
		i.WriteU64(ts);
	}else if (mode == PINT){
		if (pint_bytes == 1)
			i.WriteU8(pint.power_lo8);
		else if (pint_bytes == 2)
			i.WriteU16(pint.power);
	}
	// CFC START: level 1
	else if (mode == CFC){
		for (uint32_t j = 0; j < maxHop; j++){
			i.WriteU32(hop[j].buf[0]);
			i.WriteU32(hop[j].buf[1]);
		}
		i.WriteU16(nhop);
	}
	// CFC END: level 1
	// // CFC START: level 2
	// else if (mode == CFC){
	// 	for (uint32_t j = 0; j < maxHop; j++){
	// 		i.WriteU32(cfc_hop[j].forwarding_port);
	// 	}
	// 	i.WriteU16(cfc_nhop);
	// }
	// // CFC END: level 2
}

uint32_t IntHeader::Deserialize (Buffer::Iterator start){
	Buffer::Iterator i = start;
	if (mode == NORMAL){
		for (uint32_t j = 0; j < maxHop; j++){
			hop[j].buf[0] = i.ReadU32();
			hop[j].buf[1] = i.ReadU32();
		}
		nhop = i.ReadU16();
	}else if (mode == TS){
		ts = i.ReadU64();
	}else if (mode == PINT){
		if (pint_bytes == 1)
			pint.power_lo8 = i.ReadU8();
		else if (pint_bytes == 2)
			pint.power = i.ReadU16();
	}
	// CFC START: level 1
	else if (mode == CFC){
	for (uint32_t j = 0; j < maxHop; j++){
		hop[j].buf[0] = i.ReadU32();
		hop[j].buf[1] = i.ReadU32();
	}
	nhop = i.ReadU16();
	}
	// CFC END: level 1
	// // CFC START: level 2
	// else if (mode == CFC){
	// 	for (uint32_t j = 0; j < maxHop; j++){
	// 		hop[j].forwarding_port = i.ReadU32();
	// 	}
	// 	cfc_nhop = i.ReadU16();
	// }
	// // CFC END: level 2
	return GetStaticSize();
}

uint64_t IntHeader::GetTs(void){
	if (mode == TS)
		return ts;
	return 0;
}

uint16_t IntHeader::GetPower(void){
	if (mode == PINT)
		return pint_bytes == 1 ? pint.power_lo8 : pint.power;
	return 0;
}
void IntHeader::SetPower(uint16_t power){
	if (mode == PINT){
		if (pint_bytes == 1)
			pint.power_lo8 = power;
		else
			pint.power = power;
	}
}

}
