#include <RemoteDebugFunctions.h>

extern RemoteDebug Debug;

void beginRemoteDebug(const char *device_name){
    MDNS.addService("telnet", "tcp", 23);

    Debug.begin(device_name); // Initiaze the telnet server
    Debug.setResetCmdEnabled(true); // Enable the reset command
    Debug.showTime(true); // To show time
    Debug.showProfiler(true); // To show profiler - time between messages of Debug
                              // Good to "begin ...." and "end ...." messages
}