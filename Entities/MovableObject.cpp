//////////////////////////////////////////////////////////////////////////////////////////
// File:            MovableObject.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the MovableObject class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "MovableObject.h"
#include "PresetMan.h"
#include "SceneMan.h"
#include "ConsoleMan.h"
#include "SettingsMan.h"
#include "LuaMan.h"
#include "Atom.h"
#include "Actor.h"

namespace RTE {

AbstractClassInfo(MovableObject, SceneObject)

unsigned long int MovableObject::m_UniqueIDCounter = 1;

std::unordered_map<std::string, std::function<void(MovableObject *, Reader &)>> MovableObject::m_PropertyMatchers = MovableObject::RegisterPropertyMatchers();

std::unordered_map<std::string, std::function<void(MovableObject *, Reader &)>> MovableObject::RegisterPropertyMatchers()
{
	std::unordered_map<std::string, std::function<void(SceneObject *, Reader &)>> parent = SceneObject::RegisterPropertyMatchers();
	std::unordered_map<std::string, std::function<void(MovableObject *, Reader &)>> m;

	for (auto iter = parent.begin(); iter != parent.end(); ++iter)
		m[iter->first] = iter->second;

	m["Mass"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_Mass;
		if (e->m_Mass == 0)
			e->m_Mass = 0.0001;
	};
	m["Velocity"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_Vel; 
	};
	m["Scale"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_Scale; 
	};
	m["GlobalAccScalar"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_GlobalAccScalar; 
	};
	m["AirResistance"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_AirResistance;
		// Backwards compatibility after we made this value scaled over time
		e->m_AirResistance /= 0.01666;
	};
	m["AirThreshold"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_AirThreshold;
	};
	m["PinStrength"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_PinStrength; 
	};
	m["RestThreshold"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_RestThreshold; 
	};
	m["LifeTime"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_Lifetime; 
	};
	m["Sharpness"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_Sharpness; 
	};
	m["HitsMOs"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_HitsMOs; 
	};
	m["GetsHitByMOs"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_GetsHitByMOs; 
	};
	m["IgnoresTeamHits"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_IgnoresTeamHits; 
	};
	m["IgnoresAtomGroupHits"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_IgnoresAtomGroupHits; 
	};
	m["IgnoresAGHitsWhenSlowerThan"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_IgnoresAGHitsWhenSlowerThan; 
	};
	m["RemoveOrphanTerrainRadius"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_RemoveOrphanTerrainRadius;
		if (e->m_RemoveOrphanTerrainRadius > MAXORPHANRADIUS)
			e->m_RemoveOrphanTerrainRadius = MAXORPHANRADIUS;
	};
	m["RemoveOrphanTerrainMaxArea"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_RemoveOrphanTerrainMaxArea;
		if (e->m_RemoveOrphanTerrainMaxArea > MAXORPHANRADIUS * MAXORPHANRADIUS)
			e->m_RemoveOrphanTerrainMaxArea = MAXORPHANRADIUS * MAXORPHANRADIUS;
	};
	m["RemoveOrphanTerrainRate"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_RemoveOrphanTerrainRate; 
	};
	m["MissionCritical"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_MissionCritical; 
	};
	m["CanBeSquished"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_CanBeSquished; 
	};
	m["HUDVisible"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_HUDVisible; 
	};
	m["ProvidesPieMenuContext"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_ProvidesPieMenuContext; 
	};
	m["AddPieSlice"] = [](MovableObject * e, Reader & reader) {
		PieMenuGUI::Slice newSlice;
		reader >> newSlice;
		PieMenuGUI::AddAvailableSlice(newSlice);
	};
	m["ScriptPath"] = [](MovableObject * e, Reader & reader) {
		std::string scriptPath = reader.ReadPropValue();
		if (e->LoadScript(scriptPath) == -2) { reader.ReportError("Duplicate script path " + scriptPath); }
	};
	m["ScreenEffect"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_ScreenEffectFile;
		e->m_pScreenEffect = e->m_ScreenEffectFile.GetAsBitmap();
		e->m_ScreenEffectHash = e->m_ScreenEffectFile.GetHash();
	};
	m["EffectStartTime"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_EffectStartTime; 
	};
	m["EffectRotAngle"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_EffectRotAngle; 
	};
	m["InheritEffectRotAngle"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_InheritEffectRotAngle; 
	};
	m["RandomizeEffectRotAngle"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_RandomizeEffectRotAngle; 
	};
	m["RandomizeEffectRotAngleEveryFrame"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_RandomizeEffectRotAngleEveryFrame; 
	};
	m["EffectStopTime"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_EffectStopTime; 
	};
	m["EffectStartStrength"] = [](MovableObject * e, Reader & reader) {
		float strength;
		reader >> strength;
		e->m_EffectStartStrength = floorf((float)255 * strength);
	};
	m["EffectStopStrength"] = [](MovableObject * e, Reader & reader) {
		float strength;
		reader >> strength;
		e->m_EffectStopStrength = floorf((float)255 * strength);
	};
	m["EffectAlwaysShows"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_EffectAlwaysShows;
	};
	m["DamageOnCollision"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_DamageOnCollision; 
	};
	m["DamageOnPenetration"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_DamageOnPenetration; 
	};
	m["WoundDamageMultiplier"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_WoundDamageMultiplier; 
	};
	m["IgnoreTerrain"] = [](MovableObject * e, Reader & reader) {
		reader >> e->m_IgnoreTerrain; 
	};

	return m;
}

int MovableObject::ReadProperty(std::string propName, Reader &reader) {
	auto it = m_PropertyMatchers.find(propName);

	if (it != m_PropertyMatchers.end())
	{
		(*it).second(this, reader);
		return 0;
	}

	return Serializable::ReadProperty(propName, reader);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this MovableObject, effectively
//                  resetting the members of this abstraction level only.

void MovableObject::Clear()
{
    m_MOType = TypeGeneric;
    m_Mass = 0;
    m_Vel.Reset();
    m_PrevPos.Reset();
    m_PrevVel.Reset();
    m_Scale = 1.0;
    m_GlobalAccScalar = 1.0;
    m_AirResistance = 0;
    m_AirThreshold = 5;
    m_PinStrength = 0;
    m_RestThreshold = 500;
    m_Forces.clear();
    m_ImpulseForces.clear();
    m_AgeTimer.Reset();
    m_RestTimer.Reset();
    m_Lifetime = 0;
    m_Sharpness = 1.0;
//    m_MaterialId = 0;
    m_CheckTerrIntersection = false;
    m_HitsMOs = false;
    m_pMOToNotHit = 0;
    m_MOIgnoreTimer.Reset();
    m_GetsHitByMOs = false;
    m_IgnoresTeamHits = false;
    m_IgnoresAtomGroupHits = false;
    m_IgnoresAGHitsWhenSlowerThan = -1;
    m_MissionCritical = false;
    m_CanBeSquished = true;
    m_IsUpdated = false;
    m_WrapDoubleDraw = true;
    m_DidWrap = false;
    m_MOID = g_NoMOID;
    m_RootMOID = g_NoMOID;
    m_HasEverBeenAddedToMovableMan = false;
    m_MOIDFootprint = 0;
    m_AlreadyHitBy.clear();
    m_VelOscillations = 0;
    m_ToSettle = false;
    m_ToDelete = false;
    m_HUDVisible = true;
    m_AllLoadedScripts.clear();
    m_FunctionsAndScripts.clear();
    m_ScriptPresetName.clear();
    m_ScriptObjectName.clear();
    m_ScreenEffectFile.Reset();
    m_pScreenEffect = 0;
	m_EffectRotAngle = 0;
	m_InheritEffectRotAngle = false;
	m_RandomizeEffectRotAngle = false;
	m_RandomizeEffectRotAngleEveryFrame = false;
	m_ScreenEffectHash = 0;
    m_EffectStartTime = 0;
    m_EffectStopTime = 0;
    m_EffectStartStrength = 128;
    m_EffectStopStrength = 128;
    m_EffectAlwaysShows = false;
	m_RemoveOrphanTerrainRadius = 0;
	m_RemoveOrphanTerrainMaxArea = 0;
	m_RemoveOrphanTerrainRate = 0.0;
	m_DamageOnCollision = 0.0;
	m_DamageOnPenetration = 0.0;
	m_WoundDamageMultiplier = 1.0;
	m_IgnoreTerrain = false;

	m_MOIDHit = g_NoMOID;
	m_TerrainMatHit = g_MaterialAir;
	m_ParticleUniqueIDHit = 0;

	m_ProvidesPieMenuContext = false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the MovableObject object ready for use.

int MovableObject::Create()
{
    if (SceneObject::Create() < 0)
        return -1;

    m_AgeTimer.Reset();
    m_RestTimer.Reset();

    // If the stop time hasn't been assigned, just make the same as the life time.
    if (m_EffectStopTime <= 0)
        m_EffectStopTime = m_Lifetime;

	m_UniqueID = MovableObject::GetNextUniqueID();

	m_MOIDHit = g_NoMOID;
	m_TerrainMatHit = g_MaterialAir;
	m_ParticleUniqueIDHit = 0;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the MovableObject object ready for use.

int MovableObject::Create(const float mass,
                          const Vector &position,
                          const Vector &velocity,
                          float rotAngle,
                          float angleVel,
                          unsigned long lifetime,
                          bool hitMOs,
                          bool getHitByMOs)
{
    m_Mass = mass;
    m_Pos = position;
    m_Vel = velocity;
    m_AgeTimer.Reset();
    m_RestTimer.Reset();
    m_Lifetime = lifetime;
//    m_MaterialId = matId;
    m_HitsMOs = hitMOs;
    m_GetsHitByMOs = getHitByMOs;

	m_UniqueID = MovableObject::GetNextUniqueID();

	m_MOIDHit = g_NoMOID;
	m_TerrainMatHit = g_MaterialAir;
	m_ParticleUniqueIDHit = 0;

	g_MovableMan.RegisterObject(this);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a MovableObject to be identical to another, by deep copy.

int MovableObject::Create(const MovableObject &reference)
{
    SceneObject::Create(reference);

    m_MOType = reference.m_MOType;
    m_Mass = reference.m_Mass;
    m_Pos = reference.m_Pos;
    m_Vel = reference.m_Vel;
    m_Scale = reference.m_Scale;
    m_GlobalAccScalar = reference.m_GlobalAccScalar;
    m_AirResistance = reference.m_AirResistance;
    m_AirThreshold = reference.m_AirThreshold;
    m_PinStrength = reference.m_PinStrength;
    m_RestThreshold = reference.m_RestThreshold;
//    m_Force = reference.m_Force;
//    m_ImpulseForce = reference.m_ImpulseForce;
    // Should reset age instead??
//    m_AgeTimer = reference.m_AgeTimer;
    m_AgeTimer.Reset();
    m_RestTimer.Reset();
    m_Lifetime = reference.m_Lifetime;
    m_Sharpness = reference.m_Sharpness;
//    m_MaterialId = reference.m_MaterialId;
    m_CheckTerrIntersection = reference.m_CheckTerrIntersection;
    m_HitsMOs = reference.m_HitsMOs;
    m_GetsHitByMOs = reference.m_GetsHitByMOs;
    m_IgnoresTeamHits = reference.m_IgnoresTeamHits;
    m_IgnoresAtomGroupHits = reference.m_IgnoresAtomGroupHits;
    m_IgnoresAGHitsWhenSlowerThan = reference.m_IgnoresAGHitsWhenSlowerThan;
    m_pMOToNotHit = reference.m_pMOToNotHit;
    m_MOIgnoreTimer = reference.m_MOIgnoreTimer;
    m_MissionCritical = reference.m_MissionCritical;
    m_CanBeSquished = reference.m_CanBeSquished;
    m_HUDVisible = reference.m_HUDVisible;
    
    for (const std::pair<std::string, bool> &referenceScriptEntry : reference.m_AllLoadedScripts) {
        m_AllLoadedScripts.push_back({referenceScriptEntry.first, referenceScriptEntry.second});
    }
    ReloadScripts(false);

    if (reference.m_pScreenEffect)
    {
        m_ScreenEffectFile = reference.m_ScreenEffectFile;
        m_pScreenEffect = reference.m_pScreenEffect;

    }
	m_EffectRotAngle = reference.m_EffectRotAngle;
	m_InheritEffectRotAngle = reference.m_InheritEffectRotAngle;
	m_RandomizeEffectRotAngle = reference.m_RandomizeEffectRotAngle;
	m_RandomizeEffectRotAngleEveryFrame = reference.m_RandomizeEffectRotAngleEveryFrame;

	if (m_RandomizeEffectRotAngle)
		m_EffectRotAngle = c_PI * 2 * NormalRand();

	m_ScreenEffectHash = reference.m_ScreenEffectHash;
    m_EffectStartTime = reference.m_EffectStartTime;
    m_EffectStopTime = reference.m_EffectStopTime;
    m_EffectStartStrength = reference.m_EffectStartStrength;
    m_EffectStopStrength = reference.m_EffectStopStrength;
    m_EffectAlwaysShows = reference.m_EffectAlwaysShows;
	m_RemoveOrphanTerrainRadius = reference.m_RemoveOrphanTerrainRadius;
	m_RemoveOrphanTerrainMaxArea = reference.m_RemoveOrphanTerrainMaxArea;
	m_RemoveOrphanTerrainRate = reference.m_RemoveOrphanTerrainRate;
	m_DamageOnCollision = reference.m_DamageOnCollision;
	m_DamageOnPenetration = reference.m_DamageOnPenetration;
	m_WoundDamageMultiplier = reference.m_WoundDamageMultiplier;
	m_IgnoreTerrain = reference.m_IgnoreTerrain;

	m_MOIDHit = reference.m_MOIDHit;
	m_TerrainMatHit = reference.m_TerrainMatHit;
	m_ParticleUniqueIDHit = reference.m_ParticleUniqueIDHit;

	m_UniqueID = MovableObject::GetNextUniqueID();
	g_MovableMan.RegisterObject(this);

	m_ProvidesPieMenuContext = reference.m_ProvidesPieMenuContext;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this MovableObject to an output stream for
//                  later recreation with Create(istream &stream);

int MovableObject::Save(Writer &writer) const
{
    SceneObject::Save(writer);
// TODO: Make proper save system that knows not to save redundant data!
/*
    writer.NewProperty("Mass");
    writer << m_Mass;
    writer.NewProperty("Velocity");
    writer << m_Vel;
    writer.NewProperty("Scale");
    writer << m_Scale;
    writer.NewProperty("GlobalAccScalar");
    writer << m_GlobalAccScalar;
    writer.NewProperty("AirResistance");
    writer << m_AirResistance;
    writer.NewProperty("AirThreshold");
    writer << m_AirThreshold;
    writer.NewProperty("PinStrength");
    writer << m_PinStrength;
    writer.NewProperty("RestThreshold");
    writer << m_RestThreshold;
    writer.NewProperty("LifeTime");
    writer << m_Lifetime;
    writer.NewProperty("Sharpness");
    writer << m_Sharpness;
    writer.NewProperty("HitsMOs");
    writer << m_HitsMOs;
    writer.NewProperty("GetsHitByMOs");
    writer << m_GetsHitByMOs;
    writer.NewProperty("IgnoresTeamHits");
    writer << m_IgnoresTeamHits;
    writer.NewProperty("IgnoresAtomGroupHits");
    writer << m_IgnoresAtomGroupHits;
    writer.NewProperty("IgnoresAGHitsWhenSlowerThan");
    writer << m_IgnoresAGHitsWhenSlowerThan;
    writer.NewProperty("MissionCritical");
    writer << m_MissionCritical;
    writer.NewProperty("CanBeSquished");
    writer << m_CanBeSquished;
    writer.NewProperty("HUDVisible");
    writer << m_HUDVisible;
    if (!m_ScriptPath.empty())
    {
        writer.NewProperty("ScriptPath");
        writer << m_ScriptPath;
    }
    writer.NewProperty("ScreenEffect");
    writer << m_ScreenEffectFile;
    writer.NewProperty("EffectStartTime");
    writer << m_EffectStartTime;
    writer.NewProperty("EffectStopTime");
    writer << m_EffectStopTime;
    writer.NewProperty("EffectStartStrength");
    writer << (float)m_EffectStartStrength / 255.0f;
    writer.NewProperty("EffectStopStrength");
    writer << (float)m_EffectStopStrength / 255.0f;
    writer.NewProperty("EffectAlwaysShows");
    writer << m_EffectAlwaysShows;
*/
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MovableObject::Destroy(bool notInherited) {
    if (ObjectScriptsInitialized()) {
        RunScriptedFunctionInAppropriateScripts("Destroy");
        g_LuaMan.RunScriptString(m_ScriptObjectName + " = nil;");
    }

    if (!notInherited) { SceneObject::Destroy(); }
    Clear();

	g_MovableMan.UnregisterObject(this);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::InitializeObjectScripts() {
    m_ScriptObjectName = GetClassName() + "s." + g_LuaMan.GetNewObjectID();

    // Give Lua access to this object, then use that access to set up the object's Lua representation
    g_LuaMan.SetTempEntity(this);

    if (g_LuaMan.RunScriptString(m_ScriptObjectName + " = To" + GetClassName() + "(LuaMan.TempEntity);") < 0) {
        m_ScriptObjectName = "ERROR";
        return -2;
    }

	if (!(*m_FunctionsAndScripts.find("Create")).second.empty() && RunScriptedFunctionInAppropriateScripts("Create", true, true) < 0) {
		m_ScriptObjectName = "ERROR";
		return -3;
	}
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::LoadScript(const std::string &scriptPath, bool loadAsEnabledScript) {
    // Return an error if the script path is empty or already there
    if (scriptPath.empty()) {
        return -1;
    } else if (HasScript(scriptPath)) {
        return -2;
    }
    m_AllLoadedScripts.push_back({scriptPath, loadAsEnabledScript});

    // Clear the temporary variable names that will hold the functions read in from the file
    for (const std::string &functionName : GetSupportedScriptFunctionNames()) {
        if (g_LuaMan.RunScriptString(functionName + " = nil;") < 0) {
            return -3;
        }
    }
    // Create a new table for all presets and object instances of this class, to organize things a bit
    if (g_LuaMan.RunScriptString(GetClassName() + "s = " + GetClassName() + "s or {};") < 0) {
        return -3;
    }

    // Run the specified lua file to load everything in it into the global namespace for assignment
    if (g_LuaMan.RunScriptFile(scriptPath) < 0) {
        return -4;
    }

    // If there's no ScriptPresetName this is the first script being loaded for this preset, or scripts have been reloaded.
    // Generate a ScriptPresetName, setup a table for the preset's functions, and clear the instance object name so it gets created in the first run of UpdateScripts
    if (m_ScriptPresetName.empty()) {
        m_ScriptPresetName = GetClassName() + "s." + g_LuaMan.GetNewPresetID();

        if (g_LuaMan.RunScriptString(m_ScriptPresetName + " = {};") < 0) {
            return -3;
        }

        m_ScriptObjectName.clear();
    }

    // Assign the different functions read in from the script to their permanent locations in the preset's table
    for (const std::string &functionName : GetSupportedScriptFunctionNames()) {
        if (m_FunctionsAndScripts.find(functionName) == m_FunctionsAndScripts.end()) {
            m_FunctionsAndScripts.insert({functionName, std::vector<std::pair<std::string, bool> *>()});
        }
        if (g_LuaMan.GlobalIsDefined(functionName)) {
            m_FunctionsAndScripts.find(functionName)->second.push_back(&m_AllLoadedScripts.back());
            int error = g_LuaMan.RunScriptString(
                m_ScriptPresetName + "." + functionName + " = " + m_ScriptPresetName + "." + functionName + " or {}; " +
                m_ScriptPresetName + "." + functionName + "[\"" + scriptPath + "\"] = " + functionName + ";"
            );

            if (error < 0) {
                return -3;
            }
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::ReloadScripts(bool alsoReloadPresetScripts) {
    if (m_AllLoadedScripts.empty()) {
        return 0;
    }

    /// <summary>
    /// Internal lambda function to clear a given object's script configurations, and then load them all again in order to reset them.
    /// </summary>
    auto clearScriptConfigurationAndLoadPreexistingScripts = [](MovableObject *object, bool shouldClearScriptPresetName) {
        std::vector<std::pair<std::string, bool>> loadedScriptsCopy = object->m_AllLoadedScripts;
        object->m_AllLoadedScripts.clear();
        object->m_FunctionsAndScripts.clear();
        if (shouldClearScriptPresetName) {
            object->m_ScriptPresetName.clear();
        } else {
            object->m_ScriptObjectName.clear();
        }

        int status = 0; 
        for (const std::pair<std::string, bool> &scriptEntry : loadedScriptsCopy) {
            status = object->LoadScript(scriptEntry.first, scriptEntry.second);
            if (status < 0) {
                return status;
            }
        }
        return status;
    };

    //TODO consider getting rid of this const_cast. It would require either code duplication or creating some none const methods (specifically of PresetMan::GetEntityPreset, which may be unsafe. Could be this gross exceptional handling is the best way to go.
    MovableObject *pPreset = const_cast<MovableObject *>(dynamic_cast<const MovableObject *>(g_PresetMan.GetEntityPreset(GetClassName(), GetPresetName(), GetModuleID())));

    int status = clearScriptConfigurationAndLoadPreexistingScripts(this, pPreset == this);
    if (alsoReloadPresetScripts && status <= 0 && pPreset && pPreset != this) {
        status = clearScriptConfigurationAndLoadPreexistingScripts(pPreset, true);
    }

    return status;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MovableObject::AddScript(const std::string &scriptPath) {
    switch (LoadScript(scriptPath)) {
        case 0:
            // If we have a ScriptObjectName that means Create has already been run for pre-existing scripts. Run it right away for this one.
            if (ObjectScriptsInitialized()) {
                RunScriptedFunction(scriptPath, "Create");
                return false;
            }
            return true;
        case -1:
            g_ConsoleMan.PrintString("ERROR: The script path was empty.");
            break;
        case -2:
            g_ConsoleMan.PrintString("ERROR: The script path " + scriptPath + " is already loaded onto this object.");
            break;
        case -3:
            g_ConsoleMan.PrintString("ERROR: Failed to do necessary setup to add scripts while attempting to add the script with path " + scriptPath + ". This has nothing to do with your script, please report it to a developer.");
            break;
        default:
            RTEAbort("Reached default case while adding script. This should never happen!");
            break;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MovableObject::EnableScript(const std::string &scriptPath) {
    if (m_AllLoadedScripts.empty() || m_ScriptPresetName.empty()) {
        return false;
    }

    std::vector<std::pair<std::string, bool>>::iterator scriptEntryIterator = FindScript(scriptPath);
    if (scriptEntryIterator != m_AllLoadedScripts.end() && scriptEntryIterator->second == false) {
        if (ObjectScriptsInitialized() && RunScriptedFunction(scriptPath, "OnScriptEnable") < 0) {
            return false;
        }
        scriptEntryIterator->second = true;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MovableObject::DisableScript(const std::string &scriptPath) {
    if (m_AllLoadedScripts.empty() || m_ScriptPresetName.empty()) {
        return false;
    }

    std::vector<std::pair<std::string, bool>>::iterator scriptEntryIterator = FindScript(scriptPath);
    if (scriptEntryIterator != m_AllLoadedScripts.end() && scriptEntryIterator->second == true) {
        if (ObjectScriptsInitialized() && RunScriptedFunction(scriptPath, "OnScriptDisable") < 0) {
            return false;
        }
        scriptEntryIterator->second = false;
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::RunScriptedFunction(const std::string &scriptPath, const std::string &functionName, std::vector<Entity *> functionEntityArguments, std::vector<std::string> functionLiteralArguments) {
    if (m_AllLoadedScripts.empty() || m_ScriptPresetName.empty() || !ObjectScriptsInitialized()) {
        return -1;
    }

    std::string presetAndFunctionName = m_ScriptPresetName + "." + functionName;
    std::string fullFunctionName = presetAndFunctionName + "[\"" + scriptPath + "\"]";
    
    int status = g_LuaMan.RunScriptedFunction(fullFunctionName, m_ScriptObjectName, {presetAndFunctionName, m_ScriptObjectName, fullFunctionName}, functionEntityArguments, functionLiteralArguments);
    functionEntityArguments.clear();
    functionLiteralArguments.clear();
    
    if (status < 0 && m_AllLoadedScripts.size() > 1) {
        g_ConsoleMan.PrintString("ERROR: An error occured while trying to run the " + functionName + " function for script at path " + scriptPath);
        return -2;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::RunScriptedFunctionInAppropriateScripts(const std::string &functionName, bool runOnDisabledScripts, bool stopOnError, std::vector<Entity *> functionEntityArguments, std::vector<std::string> functionLiteralArguments) {
    if (m_AllLoadedScripts.empty() || m_ScriptPresetName.empty() || !ObjectScriptsInitialized() || m_FunctionsAndScripts.find(functionName) == m_FunctionsAndScripts.end()) {
        return -1;
    }

    int status = 0;
    for (const std::pair<std::string, bool> *scriptEntry : m_FunctionsAndScripts.at(functionName)) {
        if (runOnDisabledScripts || scriptEntry->second == true) {
            status = RunScriptedFunction(scriptEntry->first, functionName, functionEntityArguments, functionLiteralArguments);
            if (status < 0 && stopOnError) {
                return status;
            }
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     MovableObject
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Copy constructor method used to instantiate a MovableObject object
//                  identical to an already existing one.

MovableObject::MovableObject(const MovableObject &reference):
    m_Mass(reference.GetMass()),
    m_Pos(reference.GetPos()),
    m_Vel(reference.GetVel()),
    m_AgeTimer(reference.GetAge()),
    m_Lifetime(reference.GetLifetime())
{
    
}
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetAltitude
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the altitide of this' pos (or appropriate low point) over the
//                  terrain, in pixels.

float MovableObject::GetAltitude(int max, int accuracy)
{
    return g_SceneMan.FindAltitude(m_Pos, max, accuracy);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  RestDetection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does the calculations necessary to detect whether this MO appears to
//                  have has settled in the world and is at rest or not. IsAtRest()
//                  retreves the answer.

void MovableObject::RestDetection()
{
    if (m_PinStrength)
        return;

    // Translational settling detection
    if ((m_Vel.Dot(m_PrevVel) < 0)) {
        if (m_VelOscillations >= 2 && m_RestThreshold >= 0)
            m_ToSettle = true;
        else
            ++m_VelOscillations;
    }
    else
        m_VelOscillations = 0;

//    if (fabs(m_Vel.m_X) >= 0.25 || fabs(m_Vel.m_Y) >= 0.25)
//        m_RestTimer.Reset();

    if (fabs(m_Pos.m_X - m_PrevPos.m_X) >= 1.0f || fabs(m_Pos.m_Y - m_PrevPos.m_Y) >= 1.0f)
        m_RestTimer.Reset();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  IsAtRest
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates wheter the MovableObject has been at rest (no velocity) for
//                  more than one (1) second.

bool MovableObject::IsAtRest()
{
    if (m_PinStrength)
        return false;

    if (m_RestThreshold < 0)
        return false;
    else
        return m_RestTimer.IsPastSimMS(m_RestThreshold);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  OnMOHit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Defines what should happen when this MovableObject hits another MO.
//                  This is called by the owned Atom/AtomGroup of this MovableObject during
//                  travel.

bool MovableObject::OnMOHit(HitData &hd)
{
    if (hd.RootBody[HITOR] != hd.RootBody[HITEE] && (hd.Body[HITOR] == this || hd.Body[HITEE] == this)) {
        RunScriptedFunctionInAppropriateScripts("OnCollideWithMO", false, false, {hd.Body[hd.Body[HITOR] == this ? HITEE : HITOR], hd.RootBody[hd.Body[HITOR] == this ? HITEE : HITOR]});
    }
    return hd.Terminate[hd.RootBody[HITOR] == this ? HITOR : HITEE] = OnMOHit(hd.RootBody[hd.RootBody[HITOR] == this ? HITEE : HITOR]);
}

void MovableObject::SetHitWhatTerrMaterial(unsigned char matID) {
    m_TerrainMatHit = matID;
    m_LastCollisionSimFrameNumber = g_MovableMan.GetSimUpdateFrameNumber();
    RunScriptedFunctionInAppropriateScripts("OnCollideWithTerrain", false, false, {}, {std::to_string(m_TerrainMatHit)});
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyForces
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gathers and applies the global and accumulated forces. Then it clears
//                  out the force list.Note that this does NOT apply the accumulated
//                  impulses (impulse forces)!

void MovableObject::ApplyForces()
{
    // Don't apply forces to pinned objects
    if (m_PinStrength > 0)
    {
        m_Forces.clear();
        return;
    }

    float deltaTime = g_TimerMan.GetDeltaTimeSecs();

//// TODO: remove this!$@#$%#@%#@%#@^#@^#@^@#^@#")
//    if (m_PresetName != "Test Player")
    // Apply global acceleration (gravity), scaled by the scalar we have that can even be negative.
    m_Vel += g_SceneMan.GetGlobalAcc() * m_GlobalAccScalar * deltaTime;

    // Calculate air resistance effects, only when something flies faster than a threshold
    if (m_AirResistance > 0 && m_Vel.GetLargest() >= m_AirThreshold)
        m_Vel *= 1.0 - (m_AirResistance * deltaTime);

    // Apply the translational effects of all the forces accumulated during the Update()
    for (deque<pair<Vector, Vector> >::iterator fItr = m_Forces.begin(); fItr != m_Forces.end(); ++fItr)
    {
        // Continuous force application to transformational velocity.
        // (F = m * a -> a = F / m).
        m_Vel += ((*fItr).first / GetMass()) * deltaTime;
    }

    // Clear out the forces list
    m_Forces.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyImpulses
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gathers and applies the accumulated impulse forces. Then it clears
//                  out the impulse list.Note that this does NOT apply the accumulated
//                  regular forces (non-impulse forces)!

void MovableObject::ApplyImpulses()
{
    // Don't apply forces to pinned objects
    if (m_PinStrength > 0)
    {
        m_ImpulseForces.clear();
        return;
    }

//    float totalImpulses.

    // Apply the translational effects of all the impulses accumulated during the Update()
    for (deque<pair<Vector, Vector> >::iterator iItr = m_ImpulseForces.begin(); iItr != m_ImpulseForces.end(); ++iItr) {
        // Impulse force application to the transformational velocity of this MO.
        // Don't timescale these because they're already in kg * m/s (as opposed to kg * m/s^2).
        m_Vel += (*iItr).first / GetMass();
    }

    // Clear out the impulses list
    m_ImpulseForces.clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  PreTravel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does stuff that needs to be done before Travel(). Always call before
//                  calling Travel.

void MovableObject::PreTravel()
{
    if (m_GetsHitByMOs)
    {
		if (g_SettingsMan.PreciseCollisions())
		{
			// Temporarily remove the representation of this from the scene MO layers
			Draw(g_SceneMan.GetMOIDBitmap(), Vector(), g_DrawNoMOID, true);
		}
    }

    // Save previous position and velocities before moving
    m_PrevPos = m_Pos;
    m_PrevVel = m_Vel;

	m_MOIDHit = g_NoMOID;
	m_TerrainMatHit = g_MaterialAir;
	m_ParticleUniqueIDHit = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Travel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Travels this MovableObject, using its physical representation.

void MovableObject::Travel()
{
    
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  PostTravel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does stuff that needs to be done after Travel(). Always call after
//                  calling Travel.

void MovableObject::PostTravel()
{
    // Toggle whether this gets hit by other AtomGroup MOs depending on whether it's going slower than a set threshold
    if (m_IgnoresAGHitsWhenSlowerThan > 0)
        m_IgnoresAtomGroupHits = m_Vel.GetLargest() < m_IgnoresAGHitsWhenSlowerThan;

    if (m_GetsHitByMOs)
    {
		if (g_SettingsMan.PreciseCollisions())
		{
			// Replace updated MOID representation to scene after Update
			Draw(g_SceneMan.GetMOIDBitmap(), Vector(), g_DrawMOID, true);
		}
        m_AlreadyHitBy.clear();
    }
    m_IsUpdated = true;

    // Check for age expiration
    if (m_Lifetime && m_AgeTimer.GetElapsedSimTimeMS() > m_Lifetime)
        m_ToDelete = true;

    // Check for stupid positions and velocities, but critical stuff can't go too fast
    if (!g_SceneMan.IsWithinBounds(m_Pos.m_X, m_Pos.m_Y, 100))
        m_ToDelete = true;

    // Fix speeds that are too high
    FixTooFast();

    // Never let mission critical stuff settle or delete
    if (m_MissionCritical)
        m_ToSettle = false;

    // Reset the terrain intersection warning
    m_CheckTerrIntersection = false;
}

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Pure v. method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates this MovableObject. Supposed to be done every frame. This also
//                  applies and clear the accumulated impulse forces (impulses), and the
//                  transferred forces of MOs attached to this.

void MovableObject::Update()
{
    return;
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::UpdateScripts() {
    if (m_AllLoadedScripts.empty() || m_ScriptPresetName.empty()) {
        return -1;
    }

    int status = !g_LuaMan.ExpressionIsTrue(m_ScriptPresetName, false) ? ReloadScripts() : 0;
    status = (status >= 0 && !ObjectScriptsInitialized()) ? InitializeObjectScripts() : status;
    status = (status >= 0) ? RunScriptedFunctionInAppropriateScripts("Update", false, true) : status;

    return status;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MovableObject::OnPieMenu(Actor *pieMenuActor) {
    if (!pieMenuActor || m_AllLoadedScripts.empty() || m_ScriptPresetName.empty() || !ObjectScriptsInitialized()) {
        return -1;
    }

    return RunScriptedFunctionInAppropriateScripts("OnPieMenu", false, false, {pieMenuActor});
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MovableObject::Update()
{
	if (m_RandomizeEffectRotAngleEveryFrame)
		m_EffectRotAngle = c_PI * 2 * NormalRand();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  UpdateMOID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates this' and its childrens MOID status. Supposed to be done every frame.

void MovableObject::UpdateMOID(vector<MovableObject *> &MOIDIndex, MOID rootMOID, bool makeNewMOID)
{
    // Register the own MOID
    RegMOID(MOIDIndex, rootMOID, makeNewMOID);

    // Register all the attachaed children of this, going through the class hierarchy
    UpdateChildMOIDs(MOIDIndex, rootMOID, makeNewMOID);

    // Figure out the total MOID footstep of this and all its children combined
    m_MOIDFootprint = MOIDIndex.size() - m_MOID;
}



//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMOIDs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Puts all MOIDs associated with this MO and all it's descendants into MOIDs vector

void MovableObject::GetMOIDs(std::vector<MOID> &MOIDs) const
{
	if (m_MOID != g_NoMOID)
		MOIDs.push_back(m_MOID);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  RegMOID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes this MO register itself in the MOID register and get ID:s for
//                  itself and its children for this frame.
//                  BITMAP of choice.

void MovableObject::RegMOID(vector<MovableObject *> &MOIDIndex,
                            MOID rootMOID,
                            bool makeNewMOID)
{
    // Make a new MOID for itself
    if (makeNewMOID)
    {
		// Skip g_NoMOID item
		if (MOIDIndex.size() == g_NoMOID)
			MOIDIndex.push_back(0);

		m_MOID = MOIDIndex.size();
		MOIDIndex.push_back(this);
    }
    // Use the parent's MOID instead (the two are considered the same MO)
    else
        m_MOID = MOIDIndex.size() - 1;

    // Assign the root MOID
    m_RootMOID = (rootMOID == g_NoMOID ? m_MOID : rootMOID);

}

} // namespace RTE