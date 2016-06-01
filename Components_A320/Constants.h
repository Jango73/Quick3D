
#pragma once

// Noms de classe des composants

#define ClassName_CAirbusController			"CAirbusController"
#define ClassName_CAirbusStaticPort			"CAirbusStaticPort"
#define ClassName_CAirbusPitotPort			"CAirbusPitotPort"
#define ClassName_CAirbusAOASensor			"CAirbusAOASensor"
#define ClassName_CAirbusEngineGenerator	"CAirbusEngineGenerator"
#define ClassName_CAirbusFlightComputer		"CAirbusFlightComputer"
#define ClassName_CAirbusADIRU				"CAirbusADIRU"
#define ClassName_CAirbusDMC				"CAirbusDMC"
#define ClassName_CAirbusELAC				"CAirbusELAC"
#define ClassName_CAirbusFAC				"CAirbusFAC"
#define ClassName_CAirbusFADEC				"CAirbusFADEC"
#define ClassName_CAirbusFCU				"CAirbusFCU"
#define ClassName_CAirbusFMGC				"CAirbusFMGC"
#define ClassName_CAirbusSEC				"CAirbusSEC"

#define ParamName_ControlSurfaces			"ControlSurfaces"
#define ParamName_Engines					"Engines"
#define ParamName_UnitIndex					"UnitIndex"

#define EquipName_ADIRU						"Air Data and Intertial Reference Unit"
#define EquipName_AOA						"Angle of attack sensor"
#define EquipName_CFDS						"Centralized Fault Display System"
#define EquipName_CFDIU						"Centralized Fault Display Indicating Unit"
#define EquipName_DMC						"Data Monitoring Computer"
#define EquipName_EFIS						"Electronic Flight Instrument System"
#define EquipName_EGPWC						"Enhanced Ground Proximity Warning Computer"
#define EquipName_EIS						"Electronic Instrument System"
#define EquipName_ELAC						"Elevator Aileron Computer"
#define EquipName_FAC						"Flight Augmentation Computer"
#define EquipName_FADEC						"Full Authority Digital Engine Control"
#define EquipName_FCDC						"Flight Control Data Concentrators"
#define EquipName_FCU						"Flight Control Unit"									// Autopilot panel
#define EquipName_FMA						"Flight Mode Annunciator"
#define EquipName_FMGC						"Flight Management Guidance Computer"
#define EquipName_FMGS						"Flight Management Guidance System"
#define EquipName_GPS						"Global Positioning System"
#define EquipName_GPWS						"Ground Proximity Warning System"
#define EquipName_MCDU						"Multipurpose Control and Display Unit"
#define EquipName_ND						"Navigation Display"
#define EquipName_PFD						"Primary Flight Display"
#define EquipName_PITOT						"Pitot probe"
#define EquipName_RMP						"Radio Management Panel"
#define EquipName_SEC						"Spoilers Elevator Computer"
#define EquipName_STATIC					"Static probe"
#define EquipName_TAT						"Total air temperature probe"
#define EquipName_TCAS						"Traffic Alert and Collision Avoidance System"

//-------------------------------------------------------------------------------------------------
// Evènements Q3D

#define EventName_Jump500					"JUMP_500"

#define EventName_FCU_AP1					"FCU_AP1"
#define EventName_FCU_AP2					"FCU_AP2"
#define EventName_FCU_ATHR					"FCU_ATHR"
#define EventName_FCU_HDG_TRK				"FCU_HDG_TRK"
#define EventName_FCU_METRIC_ALT			"FCU_METRIC_ALT"
#define EventName_FCU_SPD_MACH				"FCU_SPD_MACH"
#define EventName_FCU_SEL_HEADING_INC		"FCU_SEL_HEADING_INC"
#define EventName_FCU_SEL_HEADING_DEC		"FCU_SEL_HEADING_DEC"
#define EventName_THR_THROTTLE_INC			"THR_THROTTLE_INC"
#define EventName_THR_THROTTLE_DEC			"THR_THROTTLE_DEC"

#define A320_Color_White					QColor(255, 255, 255)
#define A320_Color_Red						QColor(255,   0,   0)
#define A320_Color_Green					QColor(  0, 255,   0)
#define A320_Color_Blue						QColor(  0, 128, 255)
#define A320_Color_Yellow					QColor(255, 255,   0)
#define A320_Color_Amber					QColor(255, 128,   0)
#define A320_Color_Purple					QColor(255,   0, 128)
#define A320_Color_Gray						QColor(128, 128, 128)
#define A320_Color_DarkGray					QColor( 64,  64,  64)
#define A320_Color_Ground					QColor( 96,  64,   0)
#define A320_Color_Sky						A320_Color_Blue
