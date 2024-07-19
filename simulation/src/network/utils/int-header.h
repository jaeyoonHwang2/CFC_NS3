#ifndef INT_HEADER_H
#define INT_HEADER_H

#include "ns3/buffer.h"
#include <stdint.h>
#include <cstdio>

#include <fstream>
#include <sstream>
#include <iostream>

namespace ns3 {

class IntHop{
public:
	// INT placeholder in bits
	static const uint32_t timeWidth = 24;
	static const uint32_t bytesWidth = 20;
	static const uint32_t qlenWidth = 17;
	static const uint64_t lineRateValues[8];
	union{
		struct {
			uint64_t lineRate: 64-timeWidth-bytesWidth-qlenWidth,
					 time: timeWidth,
					 bytes: bytesWidth,
					 qlen: qlenWidth;
		};
		uint64_t port_index;
		uint32_t buf[2];
	};
	static const uint32_t byteUnit = 128;
	static const uint32_t qlenUnit = 80;
	static uint32_t multi;

	// CFC START: level 1
	uint64_t GetForwardingPort(){
		return (uint64_t)port_index;
	}
	// CFC END: level 1

	uint64_t GetLineRate(){
		return lineRateValues[lineRate];
	}
	uint64_t GetBytes(){
		return (uint64_t)bytes * byteUnit * multi;
	}
	uint32_t GetQlen(){
		return (uint32_t)qlen * qlenUnit * multi;
	}
	uint64_t GetTime(){
		return time;
	}
	// CFC START: level 1
	void cfc_Set(uint64_t _forwarding_port){
		port_index = _forwarding_port;
	}	
	//CFC END: level 1
	void Set(uint64_t _time, uint64_t _bytes, uint32_t _qlen, uint64_t _rate){
		time = _time;
		bytes = _bytes / (byteUnit * multi);
		qlen = _qlen / (qlenUnit * multi);
		switch (_rate){
			case 25000000000lu:
				lineRate=0;break;
			case 50000000000lu:
				lineRate=1;break;
			case 100000000000lu:
				lineRate=2;break;
			case 200000000000lu:
				lineRate=3;break;
			case 400000000000lu:
				lineRate=4;break;
			default:
				printf("Error: IntHeader unknown rate: %lu\n", _rate);
				break;
		}
	}
	uint64_t GetBytesDelta(IntHop &b){
		if (bytes >= b.bytes)
			return (bytes - b.bytes) * byteUnit * multi;
		else
			return (bytes + (1<<bytesWidth) - b.bytes) * byteUnit * multi;
	}
	uint64_t GetTimeDelta(IntHop &b){
		if (time >= b.time)
			return time - b.time;
		else
			return time + (1<<timeWidth) - b.time;
	}
};

// CFC START: level 2
class CfcHop{
public:
	uint32_t forwarding_port;

	uint64_t GetForwardingPort(){
		return forwarding_port;
	}
	void cfc_Set(uint32_t _forwarding_port){
		forwarding_port = _forwarding_port;
	}	
};
// CFC END: level 2

class IntHeader{
public:
	static const uint32_t maxHop = 5;
	// CFC START
	enum Mode{
		NORMAL = 0,
		TS = 1,
		PINT = 2,
		CFC = 3,
		NONE
	};
	// CFC END
	static Mode mode;
	static int pint_bytes;

	// std::ofstream writeFile_intHeader;
	// std::string filePath;

	// Note: the structure of IntHeader must have no internal padding, because we will directly transform the part of packet buffer to IntHeader*
	union{
		struct {
			IntHop hop[maxHop];
			uint16_t nhop;
		};
		// CFC start: level 2
		struct {
			CfcHop cfc_hop[maxHop];
			uint16_t cfc_nhop;
		};		
		// CFC end: level 2
		uint64_t ts;
		union {
			uint16_t power;
			struct{
				uint8_t power_lo8, power_hi8;
			};
		}pint;
	};

	IntHeader();
	static uint32_t GetStaticSize();
	// void PushHop(uint64_t time, uint64_t bytes, uint32_t qlen, uint64_t rate);
	// CFC START: level 1
	void PushHop(uint64_t time, uint64_t bytes, uint32_t qlen, uint64_t rate, uint64_t port);
	// CFC END: level 1
	// CFC START: level 2
	void cfc_PushHop(uint32_t port);
	// CFC END: level 2
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	uint64_t GetTs(void);
	uint16_t GetPower(void);
	void SetPower(uint16_t);
};

}

#endif /* INT_HEADER_H */
