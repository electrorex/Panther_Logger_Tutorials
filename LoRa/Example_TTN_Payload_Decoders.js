function Decoder(payload, port) {
    if(port === 1) {
        return [
            {
                field: "TEMPERATURE",
		    value: ((payload[0] & 0x80 ? 0xFFFF<<16 : 0) | payload[0]<<8 | payload[1])+(((payload[2] & 0x80 ? 0xFFFF<<16 : 0) | payload[2]<<8 | payload[3]))/100
            },
 		{
                field: "PRESSURE",
                value: (((payload[4])+(payload[5]/100))+payload[6]/10000)*100,
            },
		{
                field: "HUMIDITY",
                value: payload[7]+(payload[8]/100)
            },
		{
                field: "AQI",
                value: payload[9]+(payload[10]/100)
            },
		{
                field: "WINDSPEED1",
                value: payload[11]+(payload[12]/100)
            },
		{
                field: "WINDDIR1",
                value: (((payload[13])+(payload[14]/100))+payload[15]/10000)*100
            },
		{
                field: "GUST1",
                value: payload[16]+(payload[17]/100)
            },
		{
                field: "WINDSPEED2",
                value: payload[18]+(payload[19]/100)
            },
		{
                field: "WINDDIR2",
                value: (((payload[20])+(payload[21]/100))+payload[22]/10000)*100
            },
		{
                field: "GUST2",
                value: payload[23]+(payload[24]/100)
            },
		{
                field: "WINDCHILL1",
                value: ((payload[25] & 0x80 ? 0xFFFF<<16 : 0) | payload[25]<<8 | payload[26])+(((payload[27] & 0x80 ? 0xFFFF<<16 : 0) | payload[27]<<8 | payload[28]))/100
            },
		{
                field: "WINDCHILL2",
                value: ((payload[29] & 0x80 ? 0xFFFF<<16 : 0) | payload[29]<<8 | payload[30])+(((payload[31] & 0x80 ? 0xFFFF<<16 : 0) | payload[31]<<8 | payload[32]))/100
            },
		{
                field: "DEWPOINT",
                value: ((payload[33] & 0x80 ? 0xFFFF<<16 : 0) | payload[33]<<8 | payload[34])+(((payload[35] & 0x80 ? 0xFFFF<<16 : 0) | payload[35]<<8 | payload[36]))/100
            },
		{
                field: "BATV",
                value: payload[37]+(payload[38]/100)
            },
		{
                field: "YEAR",
                value: payload[39]
            },
		{
                field: "MONTH",
                value: payload[40]
            },
		{
                field: "DAY",
                value: payload[41]
            },
		{
                field: "HOUR",
                value: payload[42]
            },
		{
                field: "MINUTE",
                value: payload[43]
            },
		{
                field: "SECOND",
                value: payload[44]
            }
        ];
    }
}
