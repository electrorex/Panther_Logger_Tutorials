function Decoder(payload, port) {
    if(port === 1) {
        return [
	        {
            	"BATV": payload[0]+(payload[1]/100),
		"Temp0": ((payload[2] & 0x80 ? 0xFFFF<<16 : 2) | payload[2]<<8 | payload[3])+(((payload[4] & 0x80 ? 0xFFFF<<16 : 0) | payload[4]<<8 | payload[5]))/100,
		"Temp1": ((payload[6] & 0x80 ? 0xFFFF<<16 : 0) | payload[6]<<8 | payload[7])+(((payload[8] & 0x80 ? 0xFFFF<<16 : 0) | payload[8]<<8 | payload[9]))/100,
		"Temp2": ((payload[10] & 0x80 ? 0xFFFF<<16 : 0) | payload[10]<<8 | payload[11])+(((payload[12] & 0x80 ? 0xFFFF<<16 : 0) | payload[12]<<8 | payload[13]))/100
            	}
        ];
    }
}
