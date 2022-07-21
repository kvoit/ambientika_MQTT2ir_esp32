#include <RemoteDebug.h>
#if defined (ESP8266)
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include "ESPmDNS.h"
#else
#error The board must be ESP8266 or ESP32
#endif // ESP

void beginRemoteDebug(const char *device_name);