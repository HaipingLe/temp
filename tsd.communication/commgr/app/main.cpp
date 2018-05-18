#include <cstdlib>
#include <cstring>
#include <iostream>

#include <tsd/communication/CommunicationManager.hpp>
#include <tsd/communication/Communication.hpp>
#include <tsd/common/ipc/commlib.h>

#ifdef TARGET_OS_WIN32
#include <windows.h>
#endif

void createIPCResources()
{
#if TSD_COMMUNICATION_COMCLIENT_API_VERSION >= 106
  // Picture Server
  tsd::common::ipc::registerNamedObject( tsd::communication::s_SPI_PICSERVER_SenderName,
                                         tsd::communication::s_SPI_PICSERVER_ReceiverName,
                                         tsd::common::ipc::createConnection()
                                         );
  tsd::common::ipc::registerNamedObject( tsd::communication::s_PICSERVER_PPOI_SenderName,
                                         tsd::communication::s_PICSERVER_PPOI_ReceiverName,
                                         tsd::common::ipc::createConnection()
                                         );
#endif

tsd::common::ipc::registerNamedObject( "tsd.mibstd2.wirelessadapter.organizer",
                                         "tsd.mibstd2.organizer.wirelessadapter",
                                         tsd::common::ipc::createConnectionPair()
                                         );
tsd::common::ipc::registerNamedObject( "tsd.persmaster.tx", 
                                       "tsd.persmaster.rx",
                                       tsd::common::ipc::createConnection()
                                       );

}

void createIPCStuff()
{
   try
   {
      // create other ipc resource (independent from Communication Manager)
      createIPCResources();

   #ifdef TARGET_OS_WIN32
      SetProcessAffinityMask(GetCurrentProcess(), 0x1); // lock to one core - improves kernel object performance
   #endif

      tsd::common::ipc::createListenServer("0.0.0.0", 5555);

      tsd::common::ipc::createListenServer("shm:NAVI", 4096);
      tsd::common::ipc::createListenServer("shm:HMI", 4096);
      tsd::common::ipc::createListenServer("shm:RADIO", 4096);
      tsd::common::ipc::createListenServer("shm:MOCKUP", 4096);
      tsd::common::ipc::createListenServer("shm:KEYPANEL", 4096);
      tsd::common::ipc::createListenServer("shm:TRAFFIC", 4096);
      tsd::common::ipc::createListenServer("shm:AUDIO", 4096);
      tsd::common::ipc::createListenServer("shm:PHONE", 4096);
      tsd::common::ipc::createListenServer("shm:BLUETOOTH", 4096);
      tsd::common::ipc::createListenServer("shm:ORGANIZER", 4096);
      tsd::common::ipc::createListenServer("shm:MESSAGING", 4096);
      tsd::common::ipc::createListenServer("shm:SWDL", 4096);
      tsd::common::ipc::createListenServer("shm:SWDLCLIENT", 4096);
      tsd::common::ipc::createListenServer("shm:SHELL", 4096);
      tsd::common::ipc::createListenServer("shm:CANBUS", 4096);
      tsd::common::ipc::createListenServer("shm:POSITION", 4096);
      tsd::common::ipc::createListenServer("shm:SDS", 4096);
      tsd::common::ipc::createListenServer("shm:TTS", 4096);

      // Online Services
      tsd::common::ipc::createListenServer("shm:OSDESTIMP", 4096);
      tsd::common::ipc::createListenServer("shm:OSPERSPOI", 4096);
      tsd::common::ipc::createListenServer("shm:JOBPROC", 4096);
      tsd::common::ipc::createListenServer("shm:OSNEWS", 4096);
      tsd::common::ipc::createListenServer("shm:OSPOIIMP", 4096);
      tsd::common::ipc::createListenServer("shm:OSPOISEARCH", 4096);
      tsd::common::ipc::createListenServer("shm:OSTOURIMP", 4096);
      tsd::common::ipc::createListenServer("shm:OSTFCDATA", 4096);
      tsd::common::ipc::createListenServer("shm:OSTRVGUIDE", 4096);
      tsd::common::ipc::createListenServer("shm:OSVHR", 4096);
      tsd::common::ipc::createListenServer("shm:OSWEATHER", 4096);
      tsd::common::ipc::createListenServer("shm:OSTEST", 4096);
      tsd::common::ipc::createListenServer("shm:OSBEQ", 4096);
      tsd::common::ipc::createListenServer("shm:NWMGR", 4096);

      tsd::common::ipc::createListenServer("shm:BAP", 4096);
      tsd::common::ipc::createListenServer("shm:TMP1", 4096);
      tsd::common::ipc::createListenServer("shm:TMP2", 4096);


      tsd::common::ipc::createListenServer("shm:MIRRORLINKA", 4096);
      tsd::common::ipc::createListenServer("shm:MIRRORLINKB", 4096);

      tsd::common::ipc::createListenServer("shm:PERSISTIMX6A", 4096);
      tsd::common::ipc::createListenServer("shm:PERSISTIMX6B", 4096);
      tsd::common::ipc::createListenServer("shm:PERSISTJ5A", 4096);

      tsd::common::ipc::createListenServer("shm:DEVELIMX6A", 4096);
      tsd::common::ipc::createListenServer("shm:DEVELIMX6B", 4096);
      tsd::common::ipc::createListenServer("shm:DEVELJ5A", 4096);
      tsd::common::ipc::createListenServer("shm:DEVELJ5B", 4096);

      tsd::common::ipc::createListenServer("shm:DISPLAYMGR", 4096);

      tsd::common::ipc::createListenServer("shm:TESTINTERFACE", 4096);
      tsd::common::ipc::createListenServer("shm:A9R", 4096);
      tsd::common::ipc::createListenServer("shm:BROWSER", 4096);

      tsd::common::ipc::createListenServer("shm:WAVE", 4096);
      tsd::common::ipc::createListenServer("shm:ITR", 4096);
      tsd::common::ipc::createListenServer("shm:LOG", 4096);

      tsd::common::ipc::createListenServer("shm:MOST", 4096);
      tsd::common::ipc::createListenServer("shm:SWAP", 4096);
      tsd::common::ipc::createListenServer("shm:WIRE", 4096);
      tsd::common::ipc::createListenServer("shm:POSITIONING", 4096);
      tsd::common::ipc::createListenServer("shm:SENSORS", 4096);
      tsd::common::ipc::createListenServer("shm:POSRES", 4096);
      tsd::common::ipc::createListenServer("shm:USB", 4096);

      tsd::common::ipc::createListenServer("shm:PERSCLIENT", 4096);
      tsd::common::ipc::createListenServer("shm:POLICE", 4096);
      tsd::common::ipc::createListenServer("shm:DEBUG", 4096);

      // picture server
      tsd::common::ipc::createListenServer("shm:PICSERVER", 4096);
      tsd::common::ipc::createListenServer("shm:PPOI", 4096);

      tsd::common::ipc::createListenServer("shm:ASR", 4096);
      tsd::common::ipc::createListenServer("shm:SPEECH", 4096);
      tsd::common::ipc::createListenServer("shm:DEBUGMAIN", 4096);
      tsd::common::ipc::createListenServer("shm:STATIONDB", 4096);
      tsd::common::ipc::createListenServer("shm:MEDIA", 4096);


   // 1.18 bsc, exlap, perstools imx6, j5
      tsd::common::ipc::createListenServer("shm:BSC", 4096);
      tsd::common::ipc::createListenServer("shm:EXLAP", 4096);
      tsd::common::ipc::createListenServer("shm:PERSTOOLSIMX6", 4096);
      tsd::common::ipc::createListenServer("shm:PERSTOOLSJ5", 4096);

   // 1.19 asia and nwmgrj5
      tsd::common::ipc::createListenServer("shm:NWMGRJ5", 4096);
      tsd::common::ipc::createListenServer("shm:ASIA", 4096);

#if defined(TARGET_OS_POSIX_QNX) and TSD_COMMON_API_VERSION >= 210
      tsd::common::ipc::createListenServer("qnx:root", 0);
#endif

   }
   catch (...)
   {
      std::cerr << "CommunicationManager crashed!" << &std::endl;
   }
}

class Watchdog : public tsd::communication::IComWatchdog
{
public:
   void comChannelDied(tsd::communication::CommunicationManager *mgr, const std::string &name, uint32_t event)
   {
      std::cout << "ComMgr watchdog bite: " << name << std::endl;
   }
};

static void help(char *name)
{
   std::cout << "Usage: " << name << " [-b name] [-c name] [-h]" << std::endl;
   std::cout << "    -b name: Add backend" << std::endl;
   std::cout << "    -c name: Connect downstream manager" << std::endl;
   std::cout << "    -h:      Show this help" << std::endl;
   std::exit(1);
}

int32_t main(int argc, char **argv)
{
   Watchdog wd;

   // create communication manager
   tsd::communication::CommunicationManager cm;
   cm.registerTimeOutHandler(&wd);
   bool backendAdded = false;

   int i = 1;
   while (i < argc) {
      if (std::strcmp(argv[i], "-b") == 0 && i+1 < argc) {
         cm.addBackend(argv[++i]);
         backendAdded = true;
      } else if (std::strcmp(argv[i], "-c") == 0 && i+1 < argc) {
         cm.connectDownstreamCM(argv[++i]);
      } else if (std::strcmp(argv[i], "-h") == 0) {
         help(argv[0]);
      } else {
         help(argv[0]);
      }

      i++;
   }

   if(!backendAdded)
   {
      std::cout << "No backend to connect specified! -> using default backend tcp:// !" << &std::endl << &std::endl;
      cm.addBackend("tcp://");
      backendAdded = true;
   }

   createIPCStuff();

   std::cout << "Server started. Press any key to exit this program."
             << &std::endl << &std::endl;
   std::string name;
   std::getline (std::cin,name);

   return 0;
}
