#ifndef _SKYNETCLIENT_H
#define _SKYNETCLIENT_H

// #include "Arduino.h"
// #include "SPI.h"
// #include <EEPROM.h>
//#include <JsonParser.h>

#include "jsmn.h"
#include "application.h"

#include "ringbuffer.h"
#include "b64.h"

#define SKYNETCLIENT_DEBUG
#ifdef SKYNETCLIENT_DEBUG
	#ifdef PSTR
		#define DBGCSN( ... ) Serial.println( F(__VA_ARGS__) )
		#define DBGCS( ... ) Serial.print( F(__VA_ARGS__) )
	#else
		#define DBGCSN( ... ) Serial.println( __VA_ARGS__ )
		#define DBGCS( ... ) Serial.print( __VA_ARGS__ )
	#endif
	#define DBGCN( ... ) Serial.println( __VA_ARGS__ )
	#define DBGC( ... ) Serial.print( __VA_ARGS__ )
#else
	#define DBGCN( ... )
	#define DBGC( ... )
	#define DBGCSN( ... )
	#define DBGCS( ... )
#endif

#define SID_MAXLEN 24
#define UUIDSIZE 37
#define TOKENSIZE 33
#define MAXACK 5

#define HEARTBEATTIMEOUT 60000
#define SOCKETTIMEOUT 10000UL

#define EEPROMBLOCKADDRESS 0
#define EEPROMBLOCK 'S'
#define TOKENADDRESS EEPROMBLOCKADDRESS+1
#define UUIDADDRESS TOKENADDRESS+TOKENSIZE

#define MAX_PARSE_OBJECTS 16 //16 needed for Ready from Skynet
#define MAX_FLASH_STRING 50 //for PROGMEM strings

// Length of static data buffers
#define SOCKET_RX_BUFFER_SIZE 186 //186 needed for biggest skynet message, READY
#define SKYNET_TX_BUFFER_SIZE 150 //~150 is needed for firmata's capability query on an uno
#define SKYNET_RX_BUFFER_SIZE 64

#define FLOG1 F("{\"name\":\"data\",\"args\":[{")
#define FLOG2 F(", \"uuid\":\"")

#define FIDENTIFY1 F("{\"name\":\"identity\",\"args\":[{\"socketid\":\"")
#define FIDENTIFY2 F("\", \"uuid\":\"")
#define FIDENTIFY3 F("\", \"token\":\"")
#define FCLOSE F("\"}]}")

#define FBIND1 F("+[{\"result\":\"ok\"}]")

#define FMESSAGE1 F("{\"name\":\"message\",\"args\":[{\"devices\":\"")
#define FMESSAGE2 F("\",\"payload\":\"")

#define FGET1 F("GET /socket.io/1/websocket/")
#define FGET2 F(" HTTP/1.1\r\nHost: ")
#define FGET3 F("\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\n\r\n")

#define FPOST1 F("POST /socket.io/1/ HTTP/1.1\r\nHost: ")
#define FPOST2 F("\r\n\r\n")

#define IDENTIFY "identify"
#define READY "ready"
#define NOTREADY "notReady"
#define BIND "bindSocket"
#define MESSAGE "message"

#define EMIT "5:::"
#define MSG "3:::"
#define HEARTBEAT "2::"
#define BND "6:::"

class SkynetClient : public Stream {
	public:
		SkynetClient(Client &_client);
		
		typedef void (*MessageDelegate)(const char *data);

		void setMessageDelegate(MessageDelegate messageDelegate);
		void sendMessage(const char* device, char const *object);
		void logMessage(char const *object);

		int connect(IPAddress ip, uint16_t port);
	    int connect(const char *host, uint16_t port);
	    size_t write(uint8_t c);
	    size_t write(const uint8_t *buf, size_t size);
	    size_t writeRaw(const uint8_t *buf, size_t size);

	    int available();
	    int read();
	    // int read(uint8_t *buf, size_t size);
	    int peek();
	    void flush();
	    void stop();
	    uint8_t connected();
	    operator bool();
		int monitor();
		void getUuid(char *uuid);
		void getToken(char *token);
		void setUuid(char *uuid);
		void setToken(char *token);
		
	private:
		Client* client;
		char databuffer[SOCKET_RX_BUFFER_SIZE];
		uint8_t status;
		uint8_t bind;
		unsigned long lastBeat;
        MessageDelegate messageDelegate;

		void xmit(const __FlashStringHelper* data);
		void xmit(const char *data);
		void xmit(char data);
		void xmitToken(const char *js, jsmntok_t t);
		void xmit(IPAddress data);

		uint8_t waitSocketData();
		uint8_t readLine(char *buf, uint8_t max);

		void eeprom_write_bytes(int, char*, int);
		void eeprom_read_bytes(int, char*, int);

		void processSkynet(char *data, char *ack);
		void processIdentify(char *data, jsmntok_t *tok);
		void processReady(char *data, jsmntok_t *tok);
		void processNotReady(char *data, jsmntok_t *tok);
		void processMessage(char *data, jsmntok_t *tok);
		void processBind(char *data, jsmntok_t *tok, char *ack);
};

#endif // _SKYNETCLIENT_H