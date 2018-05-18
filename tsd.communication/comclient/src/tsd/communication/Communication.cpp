///////////////////////////////////////////////////////
//!\file Communication.cpp
//!\brief definition of communication manager connection endpoint names
//!\deprecated
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#include <tsd/common/types/typedef.hpp>
#include <tsd/communication/Communication.hpp>

namespace tsd { namespace communication { 

const char_t* s_CMSenderName      = "TX_CM";     //!< Name of send connection for the communication manager
const char_t* s_CMReceiverName    = "RX_CM";     //!< Name of receive connection for the communication manager
const char_t* s_CM_CPUSenderName  = "TX_CM_CPU"; //!< Name of send connection for the communication manager
const char_t* s_CM_CPUReceiverName= "RX_CM_CPU"; //!< Name of receive connection for the communication manager

const char_t* s_CM_MAIN_CPUSenderName   = "TX_CM_MAIN_CPU"; //< name of send connection of communication manager infrastructure MAIN -> CPU
const char_t* s_CM_MAIN_CPUReceiverName = "RX_CM_MAIN_CPU"; //< name of receive connection of communication manager infrastructure MAIN -> CPU
const char_t* s_CM_CPU_MAINSenderName   = "TX_CM_CPU_MAIN"; //< name of send connection of communication manager infrastructure CPU -> MAIN
const char_t* s_CM_CPU_MAINReceiverName = "RX_CM_CPU_MAIN"; //< name of receive connection of communication manager infrastructure CPU -> MAIN

const char_t* s_HMISenderName     = "TX_HMI";    //!< Name of send connection for the HMI client
const char_t* s_HMIReceiverName   = "RX_HMI";    //!< Name of receive connection for the HMI client
const char_t* s_NAVISenderName    = "TX_NAVI";   //!< Name of send connection for the Navi client
const char_t* s_NAVIReceiverName  = "RX_NAVI";   //!< Name of receive connection for the Navi client
const char_t* s_RADIOSenderName   = "TX_RADIO";  //!< Name of send connection for the Radio client
const char_t* s_RADIOReceiverName = "RX_RADIO";  //!< Name of receive connection for the Radio client
const char_t* s_KEYPANELSenderName   = "TX_KEYPANEL";  //!< Name of send connection for the Keypanel client
const char_t* s_KEYPANELReceiverName = "RX_KEYPANEL";  //!< Name of receive connection for the Keypanel client

const char_t* s_TRAFFICSenderName   = "TX_TRAFFIC";  //!< Name of send connection for the Traffic client
const char_t* s_TRAFFICReceiverName = "RX_TRAFFIC";  //!< Name of receive connection for the Traffic client

const char_t* s_AUDIOSenderName   = "TX_AUDIO";  //!< Name of send connection for the Aduio client
const char_t* s_AUDIOReceiverName = "RX_AUDIO";  //!< Name of receive connection for the Audio client

const char_t* s_MEDIASenderName   = "TX_MEDIA";  //!< Name of send connection for the Media client
const char_t* s_MEDIAReceiverName = "RX_MEDIA";  //!< Name of receive connection for the Media client

const char_t* s_PHONESenderName =       "TX_PHONE";       //!< Name of send connection for the  Phone Client on IMX6 
const char_t* s_PHONEReceiverName =     "RX_PHONE";       //!< Name of receive connection for the Phone Client on IMX6

const char_t* s_BLUETOOTHSenderName =   "TX_BLUETOOTH";   //!< Name of send connection for the Bluetooth Client on IMX6
const char_t* s_BLUETOOTHReceiverName = "RX_BLUETOOTH";   //!< Name of receive connection for the Bluetooth Client on IMX6

const char_t* s_ORGANIZRERReceiverName = "RX_ORGANIZER";  //!< Name of receive connection for the ORGANIZRER Client on IMX6
const char_t* s_ORGANIZRERSenderName =   "TX_ORGANIZER";  //!< Name of send connection for the ORGANIZRER Client on IMX6

const char_t* s_MESSAGINGReceiverName = "RX_MESSAGING";   //!< Name of receive connection for the MESSAGING Client on IMX6
const char_t* s_MESSAGINGSenderName =   "TX_MESSAGING";   //!< Name of send connection for the MESSAGING Client on IMX6

const char_t* s_SHELLSenderName      = "TX_SHELL";  //!< name of the socket send connection for the shell client
const char_t* s_SHELLReceiverName    = "RX_SHELL";  //!< name of the socket receive connection for the shell client
const char_t* s_MOCKUPSenderName     = "TX_MOCKUP"; //!< name of the socket send connection name for the mockup app
const char_t* s_MOCKUPReceiverName   = "RX_MOCKUP"; //!< name of the socket receive connection name for the mockup app

const char_t* s_SWDLSenderName     = "TX_SWDL"; //!< name of the socket send connection name for the download app
const char_t* s_SWDLReceiverName   = "RX_SWDL"; //!< name of the socket receive connection name for the download app

const char_t* s_SWDLCLIENTSenderName     = "TX_SWDLCLIENT"; //!< name of the socket send connection name for the download client app
const char_t* s_SWDLCLIENTReceiverName   = "RX_SWDLCLIENT"; //!< name of the socket receive connection name for the download client app

const char_t* s_CANBUSSenderName     = "TX_CANBUS"; //!< name of the socket send connection name for the can messages on j5
const char_t* s_CANBUSReceiverName   = "RX_CANBUS"; //!< name of the socket receive connection name for the can messages on j5

const char_t* s_POSITIONSenderName     = "TX_POSITION"; //!< name of the socket send connection name for positon messages on imx6
const char_t* s_POSITIONReceiverName   = "RX_POSITION"; //!< name of the socket receive connection name for the can messages on imx6

const char_t* s_SDSSenderName     = "TX_SDS";          //!< speech dialog system on imx6
const char_t* s_SDSReceiverName   = "RX_SDS";          //!< speach dialog system on imx6

const char_t* s_TTSSenderName     = "TX_TTS";          //!< TEXT TO SPEECH system on imx6
const char_t* s_TTSReceiverName   = "RX_TTS";          //!< TEXT TO SPEECH system on imx6

// Online Services
const char_t* s_OSDESTIMPSenderName     = "TX_OSDESTIMP";          //!< Online Services: DestinationImport imx6
const char_t* s_OSDESTIMPReceiverName   = "RX_OSDESTIMP";          //!< Online Services: DestinationImport imx6

const char_t* s_OSPERSPOISenderName     = "TX_OSPERSPOI";          //!< Online Services: DynamicPersonalPoi  imx6
const char_t* s_OSPERSPOIReceiverName   = "RX_OSPERSPOI";          //!< Online Services: DynamicPersonalPoi  imx6

const char_t* s_OSJOBPROCSenderName     = "TX_OSJOBPROC";          //!< Online Services: JobProcessor   imx6
const char_t* s_OSJOBPROCReceiverName   = "RX_OSJOBPROC";          //!< Online Services: JobProcessor   imx6

const char_t* s_OSNEWSSenderName     = "TX_OSNEWS";          //!< Online Services: News    imx6
const char_t* s_OSNEWSReceiverName   = "RX_OSNEWS";          //!< Online Services: News    imx6

const char_t* s_OSPOIIMPSenderName     = "TX_OSPOIIMP";          //!< Online Services: PoiImport    imx6 
const char_t* s_OSPOIIMPReceiverName   = "RX_OSPOIIMP";          //!< Online Services: PoiImport    imx6

const char_t* s_OSPOISEARCHSenderName     = "TX_OSPOISEARCH";          //!< Online Services: PoiSearch    imx6  
const char_t* s_OSPOISEARCHReceiverName   = "RX_OSPOISEARCH";          //!< Online Services: PoiSearch    imx6

const char_t* s_OSTOURIMPSenderName     = "TX_OSTOURIMP";          //!< Online Services: TourImport      imx6 
const char_t* s_OSTOURIMPReceiverName   = "RX_OSTOURIMP";          //!< Online Services: TourImport      imx6

const char_t* s_OSTFCDATASenderName     = "TX_OSTFCDATA";          //!< Online Services: TrafficData     imx6   
const char_t* s_OSTFCDATAReceiverName   = "RX_OSTFCDATA";          //!< Online Services: TrafficData     imx6

const char_t* s_OSTRVGUIDESenderName     = "TX_OSTRVGUIDE";          //!< Online Services: TravelGuide     imx6    
const char_t* s_OSTRVGUIDEReceiverName   = "RX_OSTRVGUIDE";          //!< Online Services: TravelGuide     imx6

const char_t* s_OSVHRSenderName     = "TX_OSVHR";          //!< Online Services: VehicleHealthReport    imx6      
const char_t* s_OSVHRReceiverName   = "RX_OSVHR";          //!< Online Services: VehicleHealthReport    imx6

const char_t* s_OSWEATHERSenderName     = "TX_OSWEATHER";          //!< Online Services: Weather    imx6       
const char_t* s_OSWEATHERReceiverName   = "RX_OSWEATHER";          //!< Online Services: Weather    imx6

const char_t* s_OSTESTSenderName     = "TX_OSTEST";          //!< Online Services: OnlineServiceTester  imx6         
const char_t* s_OSTESTReceiverName   = "RX_OSTEST";          //!< Online Services: OnlineServiceTester  imx6

const char_t* s_OSBEQSenderName     = "TX_OSBEQ";          //!< Online Services: BackendQueue    imx6        
const char_t* s_OSBEQReceiverName   = "RX_OSBEQ";          //!< Online Services: BackendQueue    imx6

const char_t* s_NWMGRSenderName     = "TX_NWMGR";          //!< Online Services: NetworkManager   imx6: Std+/StdNav, J5: Std         
const char_t* s_NWMGRReceiverName   = "RX_NWMGR";          //!< Online Services: NetworkManager   imx6: Std+/StdNav, J5: Std

const char_t* s_BAPSenderName     = "TX_BAP";          //!< BAP Process on J5
const char_t* s_BAPReceiverName   = "RX_BAP";          //!< BAP Process on J5

const char_t* s_TMP1SenderName     = "TX_TMP1";          //!< For development connection one
const char_t* s_TMP1ReceiverName   = "RX_TMP1";          //!< For development connection one

const char_t* s_TMP2SenderName     = "TX_TMP2";          //!< For development connection two
const char_t* s_TMP2ReceiverName   = "RX_TMP2";          //!< For development connection two

// Added 26.7.2012
const char_t* s_MIRRORLINKASenderName     = "TX_MIRRORLINKA";          //!< Mirrorlink on imx6
const char_t* s_MIRRORLINKAReceiverName   = "RX_MIRRORLINKA";          //!< Mirrorlink on imx6

const char_t* s_MIRRORLINKBSenderName     = "TX_MIRRORLINKB";          //!< Mirrorlink2 on imx6
const char_t* s_MIRRORLINKBReceiverName   = "RX_MIRRORLINKB";          //!< Mirrorlink2 on imx6


const char_t* s_PERSISTIMX6ASenderName     = "TX_PERSISTIMX6A";          //!< Persistenz A on imx6
const char_t* s_PERSISTIMX6AReceiverName   = "RX_PERSISTIMX6A";          //!< Persistenz A on imx6

const char_t* s_PERSISTIMX6BSenderName     = "TX_PERSISTIMX6B";          //!< Persistenz B on imx6
const char_t* s_PERSISTIMX6BReceiverName   = "RX_PERSISTIMX6B";          //!< Persistenz B on imx6

const char_t* s_PERSISTJ5ASenderName     = "TX_PERSISTJ5A";          //!< Persistenz A on j5
const char_t* s_PERSISTJ5AReceiverName   = "RX_PERSISTJ5A";          //!< Persistenz A on j5



const char_t* s_DEVELIMX6ASenderName     = "TX_DEVELIMX6A";          //!< For development A connection on imx6
const char_t* s_DEVELIMX6AReceiverName   = "RX_DEVELIMX6A";          //!< For development A connection on imx6

const char_t* s_DEVELIMX6BSenderName     = "TX_DEVELIMX6B";          //!< For development B connection on imx6
const char_t* s_DEVELIMX6BReceiverName   = "RX_DEVELIMX6B";          //!< For development B connection on imx6


const char_t* s_DEVELJ5ASenderName     = "TX_DEVELJ5A";          //!< For development B connection on j5
const char_t* s_DEVELJ5AReceiverName   = "RX_DEVELJ5A";          //!< For development B connection on j5

const char_t* s_DEVELJ5BSenderName     = "TX_DEVELJ5B";          //!< For development B connection on j5
const char_t* s_DEVELJ5BReceiverName   = "RX_DEVELJ5B";          //!< For development B connection on j5


const char_t* s_DISPLAYMGRSenderName     = "TX_DISPLAYMGR";          //!< Display-Manager. Radio: on j5, other on imx6
const char_t* s_DISPLAYMGRReceiverName   = "RX_DISPLAYMGR";          //!< Display-Manager. Radio: on j5, other on imx6

const char_t* s_TESTINTERFACESenderName     = "TX_TESTINTERFACE";    //!< Test-Interface. Radio: on j5, other on imx6
const char_t* s_TESTINTERFACEReceiverName   = "RX_TESTINTERFACE";    //!< Test-Interface. Radio: on j5, other on imx6

const char_t* s_A9RSenderName     = "TX_A9R";    //!< Automounter and Audiorouter on imx6
const char_t* s_A9RReceiverName   = "RX_A9R";    //!< Automounter and Audiorouter on imx6

const char_t* s_BROWSERSenderName     = "TX_BROWSER";    //!< Opera on imx6
const char_t* s_BROWSERReceiverName   = "RX_BROWSER";    //!< Opera on imx6


const char_t* s_WAVESenderName     = "TX_WAVE";    //!< Waveplayer on imx6
const char_t* s_WAVEReceiverName   = "RX_WAVE";    //!< Waveplayer on imx6

const char_t* s_ITRSenderName     = "TX_ITR";      //!< Infotainmentrecorder on imx6
const char_t* s_ITRReceiverName   = "RX_ITR";      //!< Infotainmentrecorder on imx6

const char_t* s_LOGSenderName     = "TX_LOG";      //!< Logger on imx6
const char_t* s_LOGReceiverName   = "RX_LOG";      //!< Logger on imx6

const char_t* s_MOSTSenderName     = "TX_MOST";      //!< Most on j5
const char_t* s_MOSTReceiverName   = "RX_MOST";      //!< Most on j5

const char_t* s_SWAPSenderName     = "TX_SWAP";      //!< Swap on imx6 or j5 on radio
const char_t* s_SWAPReceiverName   = "RX_SWAP";      //!< Swap on imx6 or j5 on radio

const char_t* s_WIRESenderName     = "TX_WIRE";      //!< Wirelessadapter on imx6 or j5 on radio
const char_t* s_WIREReceiverName   = "RX_WIRE";      //!< Wirelessadapter on imx6 or j5 on radio

const char_t* s_POSITIONINGSenderName     = "TX_POSITIONING";      //!< Positioning receiver on imx6
const char_t* s_POSITIONINGReceiverName   = "RX_POSITIONING";      //!< Positioning receiver on imx6

const char_t* s_SENSORSSenderName     = "TX_SENSORS";      //!< Sensors on imx6
const char_t* s_SENSORSReceiverName   = "RX_SENSORS";      //!< Sensors on imx6

const char_t* s_POSRESSenderName     = "TX_POSRES";      //!< POSRES on imx6
const char_t* s_POSRESReceiverName   = "RX_POSRES";      //!< POSRES on imx6

const char_t* s_USBSenderName     = "TX_USB";      //!< USB
const char_t* s_USBReceiverName   = "RX_USB";      //!< USB

const char_t* s_PERSCLIENTSenderName     = "TX_PERSCLIENT";      //!< Persistence-client
const char_t* s_PERSCLIENTReceiverName   = "RX_PERSCLIENT";      //!< Persistence-client

const char_t* s_POLICESenderName     = "TX_POLICE";      //!< Theftprotection
const char_t* s_POLICEReceiverName   = "RX_POLICE";      //!< Theftprotection

const char_t* s_DEBUGSenderName     = "TX_DEBUG";      //!< Debugservice
const char_t* s_DEBUGReceiverName   = "RX_DEBUG";      //!< Debugservice

const char_t* s_CPUROOTSenderName     = "TX_CPUROOT";      //!< For services in cpu.root
const char_t* s_CPUROOTReceiverName   = "RX_CPUROOT";      //!< For services in cpu.root

const char_t* s_MAINROOTSenderName     = "TX_MAINROOT";      //!< For services in main/radio.root
const char_t* s_MAINROOTReceiverName   = "RX_MAINROOT";      //!< For services in main/radio.root


const char_t* s_ASRSenderName          = "TX_ASR";       //!< speech on imx6
const char_t* s_ASRReceiverName        = "RX_ASR";       //!< speach on imx6

const char_t* s_SPEECHSenderName       = "TX_SPEECH";    //!< asr imx6
const char_t* s_SPEECHReceiverName     = "RX_SPEECH";    //!< asr imx6

const char_t* s_DEBUGMAINSenderName    = "TX_DEBUGMAIN"; //!< Debugservice on main processor
const char_t* s_DEBUGMAINReceiverName  = "RX_DEBUGMAIN"; //!< Debugservice on main processor

const char_t* s_STATIONDBSenderName    = "TX_STATIONDB"; //!< Stationdb on app processor
const char_t* s_STATIONDBReceiverName  = "RX_STATIONDB"; //!< Debugservice on app processor


const char_t* s_BSCSenderName    = "TX_BSC"; //!< Online BSC on imx6
const char_t* s_BSCReceiverName  = "RX_BSC"; //!< Online BSC on imx6

const char_t* s_PERSTOOLSIMX6SenderName    = "TX_PERSTOOLSIMX6"; //!< Perstools for IMX6
const char_t* s_PERSTOOLSIMX6ReceiverName  = "RX_PERSTOOLSIMX6"; //!< Perstools for IMX6

const char_t* s_PERSTOOLSJ5SenderName    = "TX_PERSTOOLSJ5"; //!< Perstools for J5
const char_t* s_PERSTOOLSJ5ReceiverName  = "RX_PERSTOOLSJ5"; //!< Perstools for J5

const char_t* s_EXLAPSenderName    = "TX_EXLAP"; //!< Exlap Server on application processor
const char_t* s_EXLAPReceiverName  = "RX_EXLAP"; //!< Exlap Server on application processor


const char_t* s_NWMGRJ5SenderName    = "TX_NWMGRJ5"; //!< Network manager on J5
const char_t* s_NWMGRJ5ReceiverName  = "RX_NWMGRJ5"; //!< Network manager on J5

const char_t* s_ASIASenderName    = "TX_ASIA"; //!< Asia input on App-CPU
const char_t* s_ASIAReceiverName  = "RX_ASIA"; //!< Asia input on App-CPU


/* used only to avoid dependency of tsd.mibstd.definition in commgr, which also provides the ipc resources
   for windows.
*/
const char_t* s_SPI_PICSERVER_SenderName     = "tsd.hmi.picserver.spi";   //!< HMI-SPI to Picserver connection: for image processing on imx6
const char_t* s_SPI_PICSERVER_ReceiverName   = "tsd.hmi.picserver.ps";    //!< HMI-SPI to Picserver connection: for image processing on imx6

const char_t* s_PICSERVER_PPOI_SenderName     = "tsd.hmi.picserver.ppoi.ps";   //!< Picserver to nav personal POIcon Reader connection: for image processing on imx6
const char_t* s_PICSERVER_PPOI_ReceiverName   = "tsd.hmi.picserver.ppoi.app";  //!< Picserver to nav personal POIcon Reader connection: for image processing on imx6

} /* namespace communication */ } /* namespace tsd */

