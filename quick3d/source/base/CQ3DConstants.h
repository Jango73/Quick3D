
#pragma once

// Qt
#include <QExplicitlySharedDataPointer>

// Application
#include "quick3d_global.h"

//-------------------------------------------------------------------------------------------------
// Macros

#define QSP                     QExplicitlySharedDataPointer
#define QSP_CAST(T,O)           QSP<T>(dynamic_cast<T*>(O.data()))

#define DELETE_VECTOR_ITEMS(v)  { for (auto d : v) { delete d; } }

//-------------------------------------------------------------------------------------------------
// Conversion factors

// Meters <-> Feet
#define FAC_METERS_TO_FEET	3.280840
#define FAC_FEET_TO_METERS	(1.0 / FAC_METERS_TO_FEET)

// Meters per second <-> Kilometers per hour
#define FAC_MS_TO_KMH		3.600000
#define FAC_KMH_TO_MS		(1.0 / FAC_MS_TO_KMH)

// Meters per second <-> Knots
#define FAC_MS_TO_KNOTS		1.943840
#define FAC_KNOTS_TO_MS		(1.0 / FAC_MS_TO_KNOTS)

// Meters per second <-> Feet per minute
#define FAC_MS_TO_FPM		(FAC_METERS_TO_FEET * 60.0)
#define FAC_FPM_TO_MS		(1.0 / FAC_MS_TO_FPM)

// Kilometers per hour <-> Knots
#define FAC_KMH_TO_KNOTS	0.539957
#define FAC_KNOTS_TO_KMH	(1.0 / FAC_KMH_TO_KNOTS)

// Meters <-> Nautical miles
#define FAC_M_TO_NM			0.000539957
#define FAC_NM_TO_M			(1.0 / FAC_M_TO_NM)

// Kilometers <-> Nautical miles
#define FAC_KM_TO_NM		0.539957
#define FAC_NM_TO_KM		(1.0 / FAC_KM_TO_NM)

//-------------------------------------------------------------------------------------------------
// Component class names

#define ClassName_CComponent                "CComponent"
#define ClassName_CPhysicalComponent        "CPhysicalComponent"
#define ClassName_CBoundingBox              "CBoundingBox"
#define ClassName_CCamera                   "CCamera"

#define ClassName_CLight                    "CLight"
#define ClassName_CFog                      "CFog"

#define ClassName_CVertex                   "CVertex"
#define ClassName_CFace                     "CFace"
#define ClassName_CMesh                     "CMesh"
#define ClassName_CMeshGeometry             "CMeshGeometry"
#define ClassName_CMeshInstance             "CMeshInstance"
#define ClassName_CBoundedMeshInstances     "CBoundedMeshInstances"
#define ClassName_CGeometryGenerator        "CGeometryGenerator"
#define ClassName_CVegetationGenerator      "CVegetationGenerator"

#define ClassName_CWater                    "CWater"
#define ClassName_CParticle                 "CParticle"
#define ClassName_CParticleSystem           "CParticleSystem"
#define ClassName_CRain                     "CRain"
#define ClassName_CBox                      "CBox"
#define ClassName_CCone                     "CCone"
#define ClassName_CSkyBox                   "CSkyBox"

#define ClassName_CWorldTerrain             "CWorldTerrain"
#define ClassName_CTerrain                  "CTerrain"
#define ClassName_CWorldChunk               "CWorldChunk"
#define ClassName_CWorldTerrainMap          "CWorldTerrainMap"

#define ClassName_CArmature                 "CArmature"
#define ClassName_CBone                     "CBone"

#define ClassName_CMaterial                 "CMaterial"
#define ClassName_CTiledMaterial            "CTiledMaterial"
#define ClassName_CWaterMaterial            "CWaterMaterial"

#define ClassName_CRenderContext            "CRenderContext"

#define ClassName_CController               "CController"
#define ClassName_CStandardController       "CStandardController"
#define ClassName_CForceController          "CForceController"
#define ClassName_CCartoController          "CCartoController"
#define ClassName_CTankController           "CTankController"
#define ClassName_CAircraftController       "CAircraftController"
#define ClassName_CQuadDroneController      "CQuadDroneController"

#define ClassName_CAnimator                 "CAnimator"
#define ClassName_CBasicAnimator            "CBasicAnimator"

#define ClassName_CMan                      "CMan"

#define ClassName_CSeaVehicle               "CSeaVehicle"
#define ClassName_CTerrestrialVehicle       "CTerrestrialVehicle"
#define ClassName_CVehicle                  "CVehicle"
#define ClassName_CWheel                    "CWheel"

#define ClassName_CAircraft                 "CAircraft"
#define ClassName_CWing                     "CWing"
#define ClassName_CElevator                 "CElevator"
#define ClassName_CRudder                   "CRudder"
#define ClassName_CEngine                   "CEngine"
#define ClassName_CJetEngine                "CJetEngine"

#define ClassName_CTrajectorable            "CTrajectorable"
#define ClassName_CTrajectory               "CTrajectory"
#define ClassName_CWaypoint                 "CWaypoint"

#define ClassName_CElectricalComponent      "CElectricalComponent"
#define ClassName_CElectricalBus            "CElectricalBus"
#define ClassName_CElectricalConsumer       "CElectricalConsumer"
#define ClassName_CElectricalContactor      "CElectricalContactor"
#define ClassName_CEngineGenerator          "CEngineGenerator"
#define ClassName_CHydraulicGenerator       "CHydraulicGenerator"

#define ClassName_CHydraulicComponent       "CHydraulicComponent"

//-------------------------------------------------------------------------------------------------
// Common parameters

#define ParamName_Acceleration              "Acceleration"
#define ParamName_AccelerationFactor        "AccelerationFactor"
#define ParamName_Active                    "Active"
#define ParamName_AileronPosition           "AileronPosition"
#define ParamName_Altitude                  "Altitude"
#define ParamName_Ambient                   "Ambient"
#define ParamName_Bounds                    "Bounds"
#define ParamName_BranchLengthScale         "BranchLengthScale"
#define ParamName_BranchRadiusScale         "BranchRadiusScale"
#define ParamName_Building                  "Building"
#define ParamName_Buildings                 "Buildings"
#define ParamName_Bush                      "Bush"
#define ParamName_CenterOfMass              "CenterOfMass"
#define ParamName_Camera1                   "Camera1"
#define ParamName_Camera2                   "Camera2"
#define ParamName_Camera3                   "Camera3"
#define ParamName_Camera4                   "Camera4"
#define ParamName_Cast_Shadows              "CastShadows"
#define ParamName_Class                     "Class"
#define ParamName_Closed                    "Closed"
#define ParamName_CollisionsActive          "CollisionsActive"
#define ParamName_Color                     "Color"
#define ParamName_Component                 "Component"
#define ParamName_Components                "Components"
#define ParamName_ControlledComponent       "ControlledComponent"
#define ParamName_Controller                "Controller"
#define ParamName_Coverage                  "Coverage"
#define ParamName_Diffuse                   "Diffuse"
#define ParamName_Displace                  "Displace"
#define ParamName_Display                   "Display"
#define ParamName_Distance                  "Distance"
#define ParamName_DynTex                    "DynTex"
#define ParamName_DynamicTexture            "DynamicTexture"
#define ParamName_DNA                       "DNA"
#define ParamName_Engine1Target             "Engine1Target"
#define ParamName_Engine2Target             "Engine2Target"
#define ParamName_Engine3Target             "Engine3Target"
#define ParamName_Engine4Target             "Engine4Target"
#define ParamName_EngineInput               "EngineInput"
#define ParamName_Face                      "Face"
#define ParamName_Faces                     "Faces"
#define ParamName_Factor                    "Factor"
#define ParamName_FFD                       "FFD"
#define ParamName_File                      "File"
#define ParamName_ForceFactor               "ForceFactor"
#define ParamName_FOV                       "FOV"
#define ParamName_Function                  "Function"
#define ParamName_Functions                 "Functions"
#define ParamName_General                   "General"
#define ParamName_Generators                "Generators"
#define ParamName_Geoloc                    "Geoloc"
#define ParamName_GravityFactor             "GravityFactor"
#define ParamName_Hardness                  "Hardness"
#define ParamName_Heading                   "Heading"
#define ParamName_Height                    "Height"
#define ParamName_HydraulicInput            "HydraulicInput"
#define ParamName_Ignore                    "Ignore"
#define ParamName_InputScale                "InputScale"
#define ParamName_Intensity                 "Intensity"
#define ParamName_IR                        "IR"
#define ParamName_Iterations                "Iterations"
#define ParamName_Latitude                  "Latitude"
#define ParamName_Leaf                      "Leaf"
#define ParamName_LeafScale                 "LeafScale"
#define ParamName_Length                    "Length"
#define ParamName_Level                     "Level"
#define ParamName_Levels                    "Levels"
#define ParamName_Longitude                 "Longitude"
#define ParamName_LookTarget                "LookTarget"
#define ParamName_Map                       "Map"
#define ParamName_Material                  "Material"
#define ParamName_Maximum                   "Maximum"
#define ParamName_MaxThrustKG               "MaxThrustKG"
#define ParamName_Mesh                      "Mesh"
#define ParamName_MeshList                  "MeshList"
#define ParamName_MaxClamp                  "MaxClamp"
#define ParamName_MinClamp                  "MinClamp"
#define ParamName_Minimum                   "Minimum"
#define ParamName_Name                      "Name"
#define ParamName_Normal                    "Normal"
#define ParamName_Offset                    "Offset"
#define ParamName_Operand                   "Operand"
#define ParamName_OutputScale               "OutputScale"
#define ParamName_Parameters                "Parameters"
#define ParamName_Parent                    "Parent"
#define ParamName_Path                      "Path"
#define ParamName_Pause                     "Pause"
#define ParamName_Physics                   "Physics"
#define ParamName_PhysicsActive             "PhysicsActive"
#define ParamName_Playing                   "Playing"
#define ParamName_Point                     "Point"
#define ParamName_Position                  "Position"
#define ParamName_PositionTarget            "PositionTarget"
#define ParamName_PowerInput                "PowerInput"
#define ParamName_ProceduralMesh            "ProceduralMesh"
#define ParamName_Resolution                "Resolution"
#define ParamName_Rotation                  "Rotation"
#define ParamName_RotationFactor            "RotationFactor"
#define ParamName_RotationMinimum           "RotationMinimum"
#define ParamName_RotationMaximum           "RotationMaximum"
#define ParamName_RotationTarget            "RotationTarget"
#define ParamName_Scale                     "Scale"
#define ParamName_Servo                     "Servo"
#define ParamName_Specular                  "Specular"
#define ParamName_Spread                    "Spread"
#define ParamName_Sphere                    "Sphere"
#define ParamName_Step                      "Step"
#define ParamName_Tag                       "Tag"
#define ParamName_Target                    "Target"
#define ParamName_Texture                   "Texture"
#define ParamName_Thrust                    "Thrust"
#define ParamName_Type                      "Type"
#define ParamName_Translation               "Translation"
#define ParamName_Tree                      "Tree"
#define ParamName_TrunkLength               "TrunkLength"
#define ParamName_TrunkRadius               "TrunkRadius"
#define ParamName_Updater                   "Updater"
#define ParamName_Value                     "Value"
#define ParamName_Vegetation                "Vegetation"
#define ParamName_Velocity                  "Velocity"
#define ParamName_VelocityFactor            "VelocityFactor"
#define ParamName_Vertex                    "Vertex"
#define ParamName_Vertices                  "Vertices"
#define ParamName_Width                     "Width"
#define ParamName_x                         "x"
#define ParamName_y                         "y"
#define ParamName_z                         "z"
#define ParamName_r                         "r"
#define ParamName_g                         "g"
#define ParamName_b                         "b"
#define ParamName_u                         "u"
#define ParamName_v                         "v"

//-------------------------------------------------------------------------------------------------
// Shader names

#define SP_Standard_Billboard               "Standard_Billboard"
#define SP_Standard_Mesh                    "Standard_Mesh"
#define SP_Special_Lines                    "Special_Lines"

//-------------------------------------------------------------------------------------------------
// Q3D events

#define Q3D_Event                           "Event"
#define Q3D_LeftMouseButton                 "LMB"
#define Q3D_RightMouseButton                "RMB"
#define Q3D_MiddleMouseButton               "MMB"

#define Q3DEvent_ToggleEdit                 "TOGGLE_EDIT"

#define Q3DEvent_Up                         "UP"
#define Q3DEvent_Down                       "DOWN"
#define Q3DEvent_UpFast                     "UP_FAST"
#define Q3DEvent_DownFast                   "DOWN_FAST"
#define Q3DEvent_Forward                    "FORWARD"
#define Q3DEvent_Backward                   "BACKWARD"
#define Q3DEvent_StrafeRight                "STRAFE_RIGHT"
#define Q3DEvent_StrafeLeft                 "STRAFE_LEFT"
#define Q3DEvent_TurnRight                  "TURN_RIGHT"
#define Q3DEvent_TurnLeft                   "TURN_LEFT"
#define Q3DEvent_LookUp                     "LOOK_UP"
#define Q3DEvent_LookDown                   "LOOK_DOWN"

#define Q3DEvent_AileronRight               "AILERON_RIGHT"
#define Q3DEvent_AileronLeft                "AILERON_LEFT"
#define Q3DEvent_NoseUp                     "NOSE_UP"
#define Q3DEvent_NoseDown                   "NOSE_DOWN"
#define Q3DEvent_RudderRight                "RUDDER_RIGHT"
#define Q3DEvent_RudderLeft                 "RUDDER_LEFT"
#define Q3DEvent_Engine1ThrustUp            "ENGINE_1_THRUST_UP"
#define Q3DEvent_Engine2ThrustUp            "ENGINE_2_THRUST_UP"
#define Q3DEvent_Engine1ThrustDown          "ENGINE_1_THRUST_DOWN"
#define Q3DEvent_Engine2ThrustDown          "ENGINE_2_THRUST_DOWN"

#define Q3DEvent_LookFront                  "LOOK_FRONT"
#define Q3DEvent_LookFrontRight             "LOOK_FRONT_RIGHT"
#define Q3DEvent_LookRight                  "LOOK_RIGHT"
#define Q3DEvent_LookBackRight              "LOOK_BACK_RIGHT"
#define Q3DEvent_LookBack                   "LOOK_BACK"
#define Q3DEvent_LookBackLeft               "LOOK_BACK_LEFT"
#define Q3DEvent_LookLeft                   "LOOK_LEFT"
#define Q3DEvent_LookFrontLeft              "LOOK_FRONT_LEFT"
#define Q3DEvent_LookFrontDown              "LOOK_FRONT_DOWN"
