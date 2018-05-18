///////////////////////////////////////////////////////
//!\file Communication.hpp
//!\brief declaration of communication manager connection endpoint names
//!\deprecated
//!
//!\author jan.kloetzke@technisat.de
//!
//!Copyright (c) 2013 TechniSat Digital GmbH
//!CONFIDENTIAL
///////////////////////////////////////////////////////

#ifndef TSD_COMMUNICATION_COMMUNICATION_HPP
#define TSD_COMMUNICATION_COMMUNICATION_HPP

#include <tsd/common/types/typedef.hpp>

namespace tsd { namespace communication { 

const uint32_t CM_BUFSIZE = 0x8000; //!< Size of the buffer for transmission
   
// Static const name definitions of the connection for the whole namespace
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CMSenderName;         //!< Name of send connection for the communication manager
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CMReceiverName;       //!< Name of receive connection for the communication manager
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CM_CPUSenderName;     //!< Name of send connection for the communication manager
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CM_CPUReceiverName;   //!< Name of receive connection for the communication manager

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CM_MAIN_CPUSenderName;   //< name of send connection of communication manager infrastructure MAIN -> CPU
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CM_MAIN_CPUReceiverName; //< name of receive connection of communication manager infrastructure MAIN -> CPU
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CM_CPU_MAINSenderName;   //< name of send connection of communication manager infrastructure CPU -> MAIN
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_CM_CPU_MAINReceiverName; //< name of receive connection of communication manager infrastructure CPU -> MAIN

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_HMISenderName;        //!< Name of send connection for the HMI client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_HMIReceiverName;      //!< Name of receive connection for the HMI client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_NAVISenderName;       //!< Name of send connection for the Navi client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_NAVIReceiverName;     //!< Name of receive connection for the Navi client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_RADIOSenderName;      //!< Name of send connection for the Radio client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_RADIOReceiverName;    //!< Name of receive connection for the Radio client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_KEYPANELSenderName;   //!< Name of send connection for the Keypanel client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_KEYPANELReceiverName; //!< Name of receive connection for the Keypanel client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_TRAFFICSenderName;     //!< Name of send connection for the Traffic client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_TRAFFICReceiverName;   //!< Name of receive connection for the Traffic client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_AUDIOSenderName;       //!< Name of send connection for the Aduio client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_AUDIOReceiverName;     //!< Name of receive connection for the Audio client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_MEDIASenderName;       //!< Name of send connection for the Media client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_MEDIAReceiverName;     //!< Name of receive connection for the Media client

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_PHONESenderName;       //!< Name of send connection for the PHONE client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_PHONEReceiverName;     //!< Name of receive connection for the PHONE client

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_BLUETOOTHSenderName;       //!< Name of send connection for the BLUETOOTH client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_BLUETOOTHReceiverName;     //!< Name of receive connection for the BLUETOOTH client

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_ORGANIZRERReceiverName; //!< Name of send connection for the ORGANIZRER client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_ORGANIZRERSenderName;   //!< Name of receive connection for the ORGANIZRER client

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_MESSAGINGReceiverName;  //!< Name of send connection for the MESSAGING client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_MESSAGINGSenderName;    //!< Name of receive connection for the MESSAGING client

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_SHELLSenderName;      //!< name of the socket send connection for the shell client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_SHELLReceiverName;    //!< name of the socket receive connection for the shell client
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_MOCKUPSenderName;     //!< name of the socket send connection name for the mockup app
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_MOCKUPReceiverName;   //!< name of the socket receive connection name for the mockup app

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_SWDLSenderName;       //!< name of the socket send connection name for the download app
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t * s_SWDLReceiverName;     //!< name of the socket receive connection name for the download app

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SWDLCLIENTSenderName;   //!< name of the socket send connection name for the download client app
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SWDLCLIENTReceiverName; //!< name of the socket receive connection name for the download client app

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_CANBUSSenderName;     //!< name of the socket send connection name for the can messages on j5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_CANBUSReceiverName;   //!< name of the socket receive connection name for the can messages on j5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POSITIONSenderName;     //!< name of the socket send connection name for positon messages on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POSITIONReceiverName;   //!< name of the socket receive connection name for the can messages on imx6

/* SDS is obsolete now */
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SDSSenderName;          //!< speech dialog system on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SDSReceiverName;        //!< speach dialog system on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TTSSenderName;          //!< TEXT TO SPEECH system on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TTSReceiverName;        //!< TEXT TO SPEECH system on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSDESTIMPSenderName;      //!< Online Services: DestinationImport imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSDESTIMPReceiverName;    //!< Online Services: DestinationImport imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSPERSPOISenderName;      //!< Online Services: DynamicPersonalPoi  imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSPERSPOIReceiverName;    //!< Online Services: DynamicPersonalPoi  imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSJOBPROCSenderName;      //!< Online Services: JobProcessor   imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSJOBPROCReceiverName;    //!< Online Services: JobProcessor   imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSNEWSSenderName;         //!< Online Services: News    imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSNEWSReceiverName;       //!< Online Services: News    imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSPOIIMPSenderName;       //!< Online Services: PoiImport    imx6 
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSPOIIMPReceiverName;     //!< Online Services: PoiImport    imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSPOISEARCHSenderName;    //!< Online Services: PoiSearch    imx6  
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSPOISEARCHReceiverName;  //!< Online Services: PoiSearch    imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTOURIMPSenderName;      //!< Online Services: TourImport      imx6 
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTOURIMPReceiverName;    //!< Online Services: TourImport      imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTFCDATASenderName;      //!< Online Services: TrafficData     imx6   
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTFCDATAReceiverName;    //!< Online Services: TrafficData     imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTRVGUIDESenderName;     //!< Online Services: TravelGuide     imx6    
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTRVGUIDEReceiverName;   //!< Online Services: TravelGuide     imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSVHRSenderName;          //!< Online Services: VehicleHealthReport    imx6      
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSVHRReceiverName;        //!< Online Services: VehicleHealthReport    imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSWEATHERSenderName;      //!< Online Services: Weather    imx6       
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSWEATHERReceiverName;    //!< Online Services: Weather    imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTESTSenderName;         //!< Online Services: OnlineServiceTester  imx6         
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSTESTReceiverName;       //!< Online Services: OnlineServiceTester  imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSBEQSenderName;          //!< Online Services: BackendQueue    imx6        
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_OSBEQReceiverName;        //!< Online Services: BackendQueue    imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_NWMGRSenderName;          //!< Online Services: NetworkManager   imx6: Std+/StdNav, J5: Std         
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_NWMGRReceiverName;        //!< Online Services: NetworkManager   imx6: Std+/StdNav, J5: Std

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_BAPSenderName;            //!< BAP Process on J5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_BAPReceiverName;          //!< BAP Process on J5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TMP1SenderName;           //!< For development connection one
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TMP1ReceiverName;         //!< For development connection one

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TMP2SenderName;          //!< For development connection two
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TMP2ReceiverName;         //!< For development connection two


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MIRRORLINKASenderName;      //!< Mirrorlink on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MIRRORLINKAReceiverName;    //!< Mirrorlink on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MIRRORLINKBSenderName;      //!< Mirrorlink2 on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MIRRORLINKBReceiverName;    //!< Mirrorlink2 on imx6
 
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSISTIMX6ASenderName;     //!< Persistenz A on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSISTIMX6AReceiverName;   //!< Persistenz A on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSISTIMX6BSenderName;     //!< Persistenz B on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSISTIMX6BReceiverName;   //!< Persistenz B on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSISTJ5ASenderName;       //!< Persistenz A on j5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSISTJ5AReceiverName;     //!< Persistenz A on j5



extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELIMX6ASenderName;       //!< For development A connection on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELIMX6AReceiverName;     //!< For development A connection on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELIMX6BSenderName;       //!< For development B connection on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELIMX6BReceiverName;     //!< For development B connection on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELJ5ASenderName;         //!< For development B connection on j5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELJ5AReceiverName;       //!< For development B connection on j5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELJ5BSenderName;         //!< For development B connection on j5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEVELJ5BReceiverName;       //!< For development B connection on j5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DISPLAYMGRSenderName;       //!< Display-Manager. Radio: on j5, other on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DISPLAYMGRReceiverName;     //!< Display-Manager. Radio: on j5, other on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TESTINTERFACESenderName;    //!< Test-Interface. Radio: on j5, other on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_TESTINTERFACEReceiverName;  //!< Test-Interface. Radio: on j5, other on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_A9RSenderName;              //!< Automounter and Audiorouter on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_A9RReceiverName;            //!< Automounter and Audiorouter on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SPI_PICSERVER_SenderName;      //!< HMI-SPI to Picserver connection: for image processing on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SPI_PICSERVER_ReceiverName;    //!< HMI-SPI to Picserver connection: for image processing on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PICSERVER_PPOI_SenderName;     //!< Picserver to nav personal POIcon Reader connection: for image processing on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PICSERVER_PPOI_ReceiverName;   //!< Picserver to nav personal POIcon Reader connection: for image processing on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_BROWSERSenderName;             //!< Opera on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_BROWSERReceiverName;           //!< Opera on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_WAVESenderName;                //!< Waveplayer on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_WAVEReceiverName;              //!< Waveplayer on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_ITRSenderName;                 //!< Infotainmentrecorder on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_ITRReceiverName;               //!< Infotainmentrecorder on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_LOGSenderName;                 //!< Logger on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_LOGReceiverName;               //!< Logger on imx6


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MOSTSenderName;      //!< Most on j5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MOSTReceiverName;    //!< Most on j5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SWAPSenderName;      //!< Swap on imx6 or j5 on radio
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SWAPReceiverName;    //!< Swap on imx6 or j5 on radio

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_WIRESenderName;      //!< Wirelessadapter on imx6 or j5 on radio
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_WIREReceiverName;    //!< Wirelessadapter on imx6 or j5 on radio

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POSITIONINGSenderName;      //!< Positioning receiver on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POSITIONINGReceiverName;    //!< Positioning receiver on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SENSORSSenderName;   //!< Sensors on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SENSORSReceiverName; //!< Sensors on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POSRESSenderName;    //!< POSRES on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POSRESReceiverName;  //!< POSRES on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_USBSenderName;       //!< USB
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_USBReceiverName;     //!< USB

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSCLIENTSenderName;      //!< Persistence-client, accessible on j5 and imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSCLIENTReceiverName;    //!< Persistence-client, accessible on j5 and imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POLICESenderName;          //!< Theftprotection on j5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_POLICEReceiverName;        //!< Theftprotection on j5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEBUGSenderName;           //!< Debugservice on application processor
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEBUGReceiverName;         //!< Debugservice on application processor

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_CPUROOTSenderName;         //!< For services in cpu.root
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_CPUROOTReceiverName;       //!< For services in cpu.root

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MAINROOTSenderName;        //!< For services in main/radio.root
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_MAINROOTReceiverName;      //!< For services in main/radio.root

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_ASRSenderName;             //!< speech on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_ASRReceiverName;           //!< speach on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SPEECHSenderName;          //!< asr imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_SPEECHReceiverName;        //!< asr imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEBUGMAINSenderName;       //!< Debugservice on main processor
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_DEBUGMAINReceiverName;     //!< Debugservice on main processor

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_STATIONDBSenderName;       //!< Stationdb on app processor
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_STATIONDBReceiverName;     //!< Debugservice on app processor

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_BSCSenderName;             //!< Online BSC on imx6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_BSCReceiverName;           //!< Online BSC on imx6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSTOOLSIMX6SenderName;   //!< Perstools for IMX6
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSTOOLSIMX6ReceiverName; //!< Perstools for IMX6

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSTOOLSJ5SenderName;     //!< Perstools for J5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_PERSTOOLSJ5ReceiverName;   //!< Perstools for J5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_EXLAPSenderName;           //!< Exlap Server
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_EXLAPReceiverName;         //!< Exlap Server


extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_NWMGRJ5SenderName; //!< Network manager on J5
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_NWMGRJ5ReceiverName; //!< Network manager on J5

extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_ASIASenderName; //!< Asia input on App-CPU
extern TSD_COMMUNICATION_COMCLIENT_DLLEXPORT const char_t* s_ASIAReceiverName; //!< Asia input on App-CPU


} /* namespace communication */ } /* namespace tsd */

#endif // TSD_COMMUNICATION_COMMUNICATION_HPP

