//////////////////////////////////////////////////////////////////////////////////////////
// File:            GameActivity.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Source file for the GameActivity class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include <sol/sol.hpp>

#include "GameActivity.h"
#include "PresetMan.h"
#include "MovableMan.h"
#include "UInputMan.h"
#include "LicenseMan.h"
#include "AudioMan.h"
#include "MetaMan.h"
#include "ConsoleMan.h"
#include "PresetMan.h"
#include "AchievementMan.h"
#include "DataModule.h"
#include "Controller.h"
#include "Scene.h"
#include "Actor.h"
#include "AHuman.h"
#include "ACrab.h"
#include "ACRocket.h"
#include "HeldDevice.h"
#include "Loadout.h"

#include "GUI/GUI.h"
#include "GUI/GUIFont.h"
#include "GUI/AllegroBitmap.h"
#include "PieMenuGUI.h"
#include "BuyMenuGUI.h"
#include "SceneEditorGUI.h"
#include "GUIBanner.h"

extern bool g_ResetActivity;
extern bool g_InActivity;

// TODO: CHANGE BACK TO 600?
#define DEMOTIMESECS 360
#define BRAINLZWIDTHDEFAULT 640

using namespace std;

namespace RTE
{

ABSTRACTCLASSINFO(GameActivity, Activity)


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this GameActivity, effectively
//                  resetting the members of this abstraction level only.

void GameActivity::Clear()
{
    m_CPUTeam = -1;

    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        m_ObservationTarget[player].Reset();
        m_DeathViewTarget[player].Reset();
        m_DeathTimer[player].Reset();
        m_ActorSelectTimer[player].Reset();
        m_ActorCursor[player].Reset();
        m_pLastMarkedActor[player] = 0;
        m_LandingZone[player].Reset();
        m_AIReturnCraft[player] = true;
        m_pPieMenu[player] = 0;
        m_pBuyGUI[player] = 0;
        m_pEditorGUI[player] = 0;
        m_pBannerRed[player] = 0;
        m_pBannerYellow[player] = 0;
        m_BannerRepeats[player] = 0;
        m_ReadyToStart[player] = false;
        m_PurchaseOverride[player].clear();
        m_BrainLZWidth[player] = BRAINLZWIDTHDEFAULT;
		m_TeamTech[player] = "";
		m_NetworkPlayerNames[player] = "";
    }

	m_StartingGold = 0;
	m_FogOfWarEnabled = false;
	m_RequireClearPathToOrbit = true;

	m_DefaultFogOfWar = -1;
	m_DefaultRequireClearPathToOrbit = -1;
	m_DefaultDeployUnits = 1;
	m_DefaultGoldCake = -1;
	m_DefaultGoldEasy = -1;
	m_DefaultGoldMedium = -1;
	m_DefaultGoldHard = -1;
	m_DefaultGoldNuts = -1;
	m_FogOfWarSwitchEnabled = true;
	m_DeployUnitsSwitchEnabled = false;
	m_GoldSwitchEnabled = true;
	m_RequireClearPathToOrbitSwitchEnabled = true;
	m_BuyMenuEnabled = true;

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        m_Deliveries[team].clear();
        m_LandingZoneArea[team].Reset();
    }

    m_aLZCursor[FRIENDLYCURSOR] = 0;
    m_aLZCursor[ENEMYCURSOR] = 0;
    m_aObjCursor[FRIENDLYCURSOR] = 0;
    m_aObjCursor[ENEMYCURSOR] = 0;

    m_Objectives.clear();

    m_DeliveryDelay = 4500;
    m_CursorTimer.Reset();
    m_GameTimer.Reset();
    m_GameOverTimer.Reset();
    m_GameOverPeriod = 5000;
    m_WinnerTeam = -1;
    m_pOrbitedCraft = 0;
	m_pPieMenuActor = 0;
    m_DemoTimer.Reset();
    m_PausedDemoTime = 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the GameActivity object ready for use.

int GameActivity::Create()
{
    if (Activity::Create() < 0)
        return -1;

//    m_Description = "Define and edit Areas on this Scene.";
/*
    ////////////////////////////////
    // Set up teams

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        ;
    }

    ///////////////////////////////////////
    // Set up players

    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        ;
    }
*/
    // Load banners
    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        m_pBannerRed[player] = new GUIBanner();
        m_pBannerYellow[player] = new GUIBanner();
    }

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
		m_TeamIsCPU[team] = false;
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a GameActivity to be identical to another, by deep copy.

int GameActivity::Create(const GameActivity &reference)
{
    if (Activity::Create(reference) < 0)
        return -1;

    m_CPUTeam = reference.m_CPUTeam;

    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        m_ObservationTarget[player] = reference.m_ObservationTarget[player];
        m_DeathViewTarget[player] = reference.m_DeathViewTarget[player];
//        m_DeathTimer[player] = reference.m_DeathTimer[player];
        m_ActorCursor[player] = reference.m_ActorCursor[player];
        m_pLastMarkedActor[player] = reference.m_pLastMarkedActor[player];
        m_LandingZone[player] = reference.m_LandingZone[player];
        m_AIReturnCraft[player] = reference.m_AIReturnCraft[player];
        m_pPieMenu[player] = new PieMenuGUI;
        m_pBuyGUI[player] = new BuyMenuGUI;
        m_pEditorGUI[player] = new SceneEditorGUI;
        m_pBannerRed[player] = new GUIBanner();
        m_pBannerYellow[player] = new GUIBanner();
        m_ReadyToStart[player] = reference.m_ReadyToStart[player];
        m_BrainLZWidth[player] = reference.m_BrainLZWidth[player];

		m_NetworkPlayerNames[player] = reference.m_NetworkPlayerNames[player];
    }

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        m_LandingZoneArea[team] = reference.m_LandingZoneArea[team];
		m_TeamTech[team] = reference.m_TeamTech[team];
		m_TeamIsCPU[team] = reference.m_TeamIsCPU[team];
    }

	m_StartingGold = reference.m_StartingGold;
	m_FogOfWarEnabled = reference.m_FogOfWarEnabled;
	m_RequireClearPathToOrbit = reference.m_RequireClearPathToOrbit;

	m_DefaultFogOfWar = reference.m_DefaultFogOfWar;
	m_DefaultRequireClearPathToOrbit = reference.m_DefaultRequireClearPathToOrbit;
	m_DefaultDeployUnits = reference.m_DefaultDeployUnits;
	m_DefaultGoldCake = reference.m_DefaultGoldCake;
	m_DefaultGoldEasy = reference.m_DefaultGoldEasy;
	m_DefaultGoldMedium = reference.m_DefaultGoldMedium;
	m_DefaultGoldHard = reference.m_DefaultGoldHard;
	m_DefaultGoldNuts = reference.m_DefaultGoldNuts;
	m_FogOfWarSwitchEnabled = reference.m_FogOfWarSwitchEnabled;
	m_DeployUnitsSwitchEnabled = reference.m_DeployUnitsSwitchEnabled;
	m_GoldSwitchEnabled = reference.m_GoldSwitchEnabled;
	m_RequireClearPathToOrbitSwitchEnabled = reference.m_RequireClearPathToOrbitSwitchEnabled;
	m_BuyMenuEnabled = reference.m_BuyMenuEnabled;

    m_DeliveryDelay = reference.m_DeliveryDelay;
//    m_CursorTimer = reference.m_CursorTimer;
//    m_GameTimer = reference.m_GameTimer;
//    m_GameOverTimer = reference.m_GameOverTimer;
    m_GameOverPeriod = reference.m_GameOverPeriod;
    m_WinnerTeam = reference.m_WinnerTeam;
    m_PausedDemoTime = reference.m_PausedDemoTime;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the reader's position is untouched.

int GameActivity::ReadProperty(std::string propName, Reader &reader)
{
    if (propName == "CPUTeam")
    {
        reader >> m_CPUTeam;
        SetCPUTeam(m_CPUTeam);
    }
    else if (propName == "DeliveryDelay")
        reader >> m_DeliveryDelay;
    else if (propName == "DefaultFogOfWar")
	    reader >> m_DefaultFogOfWar;
    else if (propName == "DefaultRequireClearPathToOrbit")
        reader >> m_DefaultRequireClearPathToOrbit;
    else if (propName == "DefaultDeployUnits")
        reader >> m_DefaultDeployUnits;
    else if (propName == "DefaultGoldCake")
        reader >> m_DefaultGoldCake;
    else if (propName == "DefaultGoldEasy")
        reader >> m_DefaultGoldEasy;
    else if (propName == "DefaultGoldMedium")
        reader >> m_DefaultGoldMedium;
    else if (propName == "DefaultGoldHard")
        reader >> m_DefaultGoldHard;
    else if (propName == "DefaultGoldNuts")
        reader >> m_DefaultGoldNuts;
    else if (propName == "FogOfWarSwitchEnabled")
        reader >> m_FogOfWarSwitchEnabled;
    else if (propName == "DeployUnitsSwitchEnabled")
        reader >> m_DeployUnitsSwitchEnabled;
    else if (propName == "GoldSwitchEnabled")
        reader >> m_GoldSwitchEnabled;
	else if (propName == "RequireClearPathToOrbitSwitchEnabled")
        reader >> m_RequireClearPathToOrbitSwitchEnabled;
    else
        // See if the base class(es) can find a match instead
        return Activity::ReadProperty(propName, reader);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this GameActivity with a Writer for
//                  later recreation with Create(Reader &reader);

int GameActivity::Save(Writer &writer) const
{
    Activity::Save(writer);

    writer.NewProperty("CPUTeam");
    writer << m_CPUTeam;
    writer.NewProperty("DeliveryDelay");
    writer << m_DeliveryDelay;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the GameActivity object.

void GameActivity::Destroy(bool notInherited)
{
    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        delete m_pPieMenu[player];
        delete m_pBuyGUI[player];
        delete m_pEditorGUI[player];
        delete m_pBannerRed[player];
        delete m_pBannerYellow[player];
    }

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        for (deque<Delivery>::iterator itr = m_Deliveries[team].begin(); itr != m_Deliveries[team].end(); ++itr)
            delete itr->pCraft;
        m_Deliveries[team].clear();
    }

    if (!notInherited)
        Activity::Destroy();
    Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTeamTech
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets tech module name for specified team. Module must set must be loaded.
	void GameActivity::SetTeamTech(int team, std::string tech)
	{
		if (team >= 0 && team < MAXTEAMCOUNT)
		{
			if (tech == "-All-" || tech == "-Random-")
				m_TeamTech[team] = tech;
			else
			{
				int id = g_PresetMan.GetModuleID(tech);
				if (id != -1)
					m_TeamTech[team] = tech;
				else
					g_ConsoleMan.PrintString("ERR: " + tech + " not found. Can't set tech.");
			}
		}
	}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetCrabToHumanSpawnRatio
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns CrabToHumanSpawnRatio for specified module
	float GameActivity::GetCrabToHumanSpawnRatio(int moduleid) 
	{
		if (moduleid > -1)
		{
			const DataModule * pDataModule = g_PresetMan.GetDataModule(moduleid);
			if (pDataModule)
				return pDataModule->GetCrabToHumanSpawnRatio();
		}
		return 0.25;
	}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetCPUTeam
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the current CPU-assisted team, if any (NOTEAM) - LEGACY function

void GameActivity::SetCPUTeam(int team)
{
    // Set the legacy var
	if (team >= 0 && team < MAXTEAMCOUNT)
	{
	    m_CPUTeam = team;

		// Activate the CPU team
		m_TeamActive[team] = true;
		m_TeamIsCPU[team] = true;
	}

/* whaaaa?
    // Also set the newer human indicator flags
    for (int player = PLAYER_1; player < MAXPLAYERCOUNT; ++player)
        m_IsHuman[m_Team[player]] = m_IsActive[player] && m_Team[player] != team;
*/
}

#ifndef __OPEN_SOURCE_EDITION

/////////////////////////////
// TURN OPTIMIZATIONS OFF
// This is so the EXECryptor markers don't get mangled by the optimizer

#pragma optimize("", off)

#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  GetDemoTimeLeft
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Shows how many seconds of demo time is left, if indeed in demo mode.

long GameActivity::GetDemoTimeLeft()
{
#ifndef __OPEN_SOURCE_EDITION
    CRYPT_START
#endif

    // Not in demo mode
    if (g_LicenseMan.HasValidatedLicense())
        return -1;

    if (m_ActivityState != RUNNING)
        return m_ActivityState == DEMOEND ? 0 : DEMOTIMESECS;

    // Ten minutes
    long secsLeft = DEMOTIMESECS - m_DemoTimer.GetElapsedRealTimeS();

    // Cap at 0
    if (secsLeft < 0)
        secsLeft = 0;

    return secsLeft;

#ifndef __OPEN_SOURCE_EDITION
	CRYPT_END
#endif
}

#ifndef __OPEN_SOURCE_EDITION

/////////////////////////////
// TURN OPTIMIZATIONS ON
// This is so the EXECryptor markers don't get mangled by the optimizer

#pragma optimize("", on)

#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SwitchToActor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces the this to focus player control to a specific Actor for a
//                  specific team. OWNERSHIP IS NOT TRANSFERRED!

bool GameActivity::SwitchToActor(Actor *pActor, int player, int team)
{
    // Computer players don't focus on any Actor
    if (!m_IsHuman[player])
        return false;

    // Play the disabling animation when the actor swtiched, for easy ID of currently controlled actor
    m_pPieMenu[player]->DisableAnim();

    // Disable the AI command mode since it's connected to the current actor
    if (m_ViewState[player] == AISENTRYPOINT || m_ViewState[player] == AIPATROLPOINTS || m_ViewState[player] == AIGOLDDIGPOINT || m_ViewState[player] == AIGOTOPOINT || m_ViewState[player] == UNITSELECTCIRCLE)
        m_ViewState[player] = NORMAL;

    return Activity::SwitchToActor(pActor, player, team);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SwitchToNextActor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces the this to focus player control to the next Actor of a
//                  specific team, other than the current one focused on.

void GameActivity::SwitchToNextActor(int player, int team, Actor *pSkip)
{
    // Play the disabling animation when the actor swtiched, for easy ID of currently controlled actor
    m_pPieMenu[player]->DisableAnim();

    // Disable the AI command mode since it's connected to the current actor
    if (m_ViewState[player] == AISENTRYPOINT || m_ViewState[player] == AIPATROLPOINTS || m_ViewState[player] == AIGOLDDIGPOINT || m_ViewState[player] == AIGOTOPOINT || m_ViewState[player] == UNITSELECTCIRCLE)
        m_ViewState[player] = NORMAL;

    Activity::SwitchToNextActor(player, team, pSkip);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SwitchToPrevActor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces this to focus player control to the previous Actor of a
//                  specific team, other than the current one focused on.

void GameActivity::SwitchToPrevActor(int player, int team, Actor *pSkip)
{
    // Play the disabling animation when the actor swtiched, for easy ID of currently controlled actor
    m_pPieMenu[player]->DisableAnim();

    // Disable the AI command mode since it's connected to the current actor
    if (m_ViewState[player] == AISENTRYPOINT || m_ViewState[player] == AIPATROLPOINTS || m_ViewState[player] == AIGOLDDIGPOINT || m_ViewState[player] == AIGOTOPOINT  || m_ViewState[player] == UNITSELECTCIRCLE)
        m_ViewState[player] = NORMAL;

    Activity::SwitchToPrevActor(player, team, pSkip);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddObjectivePoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Created an objective point for one of the teams to show until cleared.

void GameActivity::AddObjectivePoint(string description, Vector objPos, int whichTeam, ObjectiveArrowDir arrowDir)
{
    m_Objectives.push_back(ObjectivePoint(description, objPos, whichTeam, arrowDir));
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          YSortObjectivePoints
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sorts all objective points according to their positions on the Y axis.

void GameActivity::YSortObjectivePoints()
{
    m_Objectives.sort(ObjPointYPosComparison());
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddOverridePurchase
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds somehting to the purchase list that will override what is set
//                  in the buy GUI next time CreateDelivery is called.

int GameActivity::AddOverridePurchase(const SceneObject *pPurchase, int player)
{
	if (player >= Activity::PLAYER_1 && player < Activity::MAXPLAYERCOUNT)
	{
		// Add to purchase list if valid item
		if (pPurchase)
		{
			// Get the preset of this instance passed in, so we make sure we are only storing non-owned instances
			const SceneObject *pPreset = dynamic_cast<const SceneObject *>(g_PresetMan.GetEntityPreset(pPurchase->GetClassName(), pPurchase->GetPresetName(), pPurchase->GetModuleID()));
			if (pPreset)
				m_PurchaseOverride[player].push_back(pPreset);
		}

		// Take metaplayer tech modifiers into account
		int nativeModule = 0;
		float foreignCostMult = 1.0;
		float nativeCostMult = 1.0;
		MetaPlayer *pMetaPlayer = g_MetaMan.GetMetaPlayerOfInGamePlayer(player);
		if (g_MetaMan.GameInProgress() && pMetaPlayer)
		{
			nativeModule = pMetaPlayer->GetNativeTechModule();
			foreignCostMult = pMetaPlayer->GetForeignCostMultiplier();
			nativeCostMult = pMetaPlayer->GetNativeCostMultiplier();
		}

		// Calculate the total list cost for this player
		int totalListCost = 0;
		for (list<const SceneObject *>::iterator itr = m_PurchaseOverride[player].begin(); itr != m_PurchaseOverride[player].end(); ++itr)
			totalListCost += (*itr)->GetGoldValue(nativeModule, foreignCostMult, nativeCostMult);

		return totalListCost;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetOverridePurchaseList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     First clears and then adds all the stuff in a Loadout to the override
//                  purchase list.

int GameActivity::SetOverridePurchaseList(const Loadout *pLoadout, int player)
{
    // First clear out the list
    ClearOverridePurchase(player);

    int finalListCost = 0;

    // Sanity check
    if (!pLoadout)
    {
        g_ConsoleMan.PrintString("ERROR: Tried to set an override purchase list based on a nonexistent Loadout!");
        return 0;
    }

    const ACraft *pCraftPreset = pLoadout->GetDeliveryCraft();

    // Check if we even have a craft and substitute a default if we don't
    if (!pCraftPreset)
    {
// Too verbose for a recoverable error
//        g_ConsoleMan.PrintString("ERROR: Tried to set an override purchase list with no delivery craft defined. Using a default instead.");
        const Loadout *pDefault = dynamic_cast<const Loadout *>(g_PresetMan.GetEntityPreset("Loadout", "Default", -1));
        if (pDefault)
            pCraftPreset = pDefault->GetDeliveryCraft();
        
        // If still no go, then fuck it
        if (!pCraftPreset)
        {
            g_ConsoleMan.PrintString("ERROR: Couldn't even find a \"Default\" Loadout in Base.rte! Aborting.");
            return 0;
        }
    }

    // Add the delivery Craft
    finalListCost = AddOverridePurchase(pCraftPreset, player);

    // Add the rest of the cargo list
    list<const SceneObject *> *pCargoList = const_cast<Loadout *>(pLoadout)->GetCargoList();
    for (list<const SceneObject *>::iterator itr = pCargoList->begin(); itr != pCargoList->end(); ++itr)
        finalListCost = AddOverridePurchase(*itr, player);

    return finalListCost;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetOverridePurchaseList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     First clears and then adds all the stuff in a Loadout to the override
//                  purchase list.

int GameActivity::SetOverridePurchaseList(string loadoutName, int player)
{
    // Find out the native module of this player
    int nativeModule = 0;
    MetaPlayer *pMetaPlayer = g_MetaMan.GetMetaPlayerOfInGamePlayer(player);
    if (g_MetaMan.GameInProgress() && pMetaPlayer)
        nativeModule = pMetaPlayer->GetNativeTechModule();

    // Find the Loadout that this Deployment is referring to
    const Loadout *pLoadout = dynamic_cast<const Loadout *>(g_PresetMan.GetEntityPreset("Loadout", loadoutName, nativeModule));
    if (pLoadout)
        return SetOverridePurchaseList(pLoadout, player);
    else
        g_ConsoleMan.PrintString("ERROR: Tried to set an override purchase list based on a nonexistent Loadout!");

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CreateDelivery
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Takes the current order out of a player's buy GUI, creates a Delivery
//                  based off it, and stuffs it into that player's delivery queue.

bool GameActivity::CreateDelivery(int player, int mode, Vector &waypoint, Actor * pTargetMO)
{
    int team = m_Team[player];
    if (team == NOTEAM)
        return false;

    // Prepare the Craft, stuff everything into it and add it to the queue
    // Retrieve the ordered craft and its inventory
    list<const SceneObject *> purchaseList;

    ACraft *pDeliveryCraft = 0;
    // If we have a list to purchase that overrides the buy GUI, then use it and clear it
    if (!m_PurchaseOverride[player].empty())
    {
        const ACraft *pCraftPreset = 0;
        for (list<const SceneObject *>::iterator itr = m_PurchaseOverride[player].begin(); itr != m_PurchaseOverride[player].end(); ++itr)
        {
            // Find the first craft to use as the delivery craft
            pCraftPreset = dynamic_cast<const ACraft *>(*itr);
            if (!pDeliveryCraft && pCraftPreset)
                pDeliveryCraft = dynamic_cast<ACraft *>((*itr)->Clone());
            else
                purchaseList.push_back(*itr);
        }
    }
    // Otherwise, use what's set in the buy GUI as usual
    else
    {
        m_pBuyGUI[player]->GetOrderList(purchaseList);
        pDeliveryCraft = dynamic_cast<ACraft *>(m_pBuyGUI[player]->GetDeliveryCraftPreset()->Clone());
        // If we don't have any loadout presets in the menu, save the current config for later
        if (m_pBuyGUI[player]->GetLoadoutPresets().empty())
            m_pBuyGUI[player]->SaveCurrentLoadout();
    }

    if (pDeliveryCraft && (!m_HadBrain[player] || m_pBrain[player]))
    {
        // Take metaplayer tech modifiers into account when calculating costs of this delivery
        int nativeModule = 0;
        float foreignCostMult = 1.0;
        float nativeCostMult = 1.0;
        MetaPlayer *pMetaPlayer = g_MetaMan.GetMetaPlayerOfInGamePlayer(player);
        if (g_MetaMan.GameInProgress() && pMetaPlayer)
        {
            nativeModule = pMetaPlayer->GetNativeTechModule();
            foreignCostMult = pMetaPlayer->GetForeignCostMultiplier();
            nativeCostMult = pMetaPlayer->GetNativeCostMultiplier();
        }
        // Start with counting the craft
		float totalCost = 0;

		if (m_pBuyGUI[player]->GetOnlyShowOwnedItems())
		{
			if (!m_pBuyGUI[player]->CommitPurchase(pDeliveryCraft->GetModuleAndPresetName()))
			{
				if (m_pBuyGUI[player]->IsAlwaysAllowedItem(pDeliveryCraft->GetModuleAndPresetName()))
					totalCost = pDeliveryCraft->GetGoldValue(nativeModule, foreignCostMult, nativeCostMult);
				else
					return false;
			}
		}
		else
		{
			if (!m_pBuyGUI[player]->CommitPurchase(pDeliveryCraft->GetModuleAndPresetName()))
				totalCost = pDeliveryCraft->GetGoldValue(nativeModule, foreignCostMult, nativeCostMult);
		}

        // Go through the list of things ordered, and give any actors all the items that is present after them,
        // until the next actor. Also, the first actor gets all stuff in the list above him.
        MovableObject *pInventoryObject = 0;
        Actor *pPassenger = 0;
        Actor *pLastPassenger = 0;
        list<MovableObject *> cargoItems;

        int crabCount = 0;

        for (list<const SceneObject *>::iterator itr = purchaseList.begin(); itr != purchaseList.end(); ++itr)
        {
            // Achievement: check if it's a crab
            if ((*itr)->GetPresetName() == "Crab" && (*itr)->GetGoldValue() == 0 && crabCount != -1)
            {
                crabCount++;
            }
            else
            {
                // Not a crab! Give up on the achievement.
                crabCount = -1;
            }

			bool purchaseItem = true;

            // Add to the total cost tally
			if (m_pBuyGUI[player]->GetOnlyShowOwnedItems())
			{
				if (!m_pBuyGUI[player]->CommitPurchase((*itr)->GetModuleAndPresetName()))
				{
					if (m_pBuyGUI[player]->IsAlwaysAllowedItem((*itr)->GetModuleAndPresetName()))
						totalCost += (*itr)->GetGoldValue(nativeModule, foreignCostMult, nativeCostMult);
					else
						purchaseItem = false;
				}
			}
			else
			{
				if (!m_pBuyGUI[player]->CommitPurchase((*itr)->GetModuleAndPresetName()))
					totalCost += (*itr)->GetGoldValue(nativeModule, foreignCostMult, nativeCostMult);
			}

			if (purchaseItem)
			{
				// Make copy of the preset instance in the list
				pInventoryObject = dynamic_cast<MovableObject *>((*itr)->Clone());
				// See if it's actually a passenger, as opposed to a regular item
				pPassenger = dynamic_cast<Actor *>(pInventoryObject);
				// If it's an actor, then set its team and add it to the Craft's inventory!
				if (pPassenger)
				{
					// If this is the first passenger, then give him all the shit found in the list before him
					if (!pLastPassenger)
					{
						for (list<MovableObject *>::iterator iItr = cargoItems.begin(); iItr != cargoItems.end(); ++iItr)
							pPassenger->AddInventoryItem(*iItr);
					}
					// This isn't the first passenger, so give the previous guy all the stuff that was found since processing him
					else
					{
						for (list<MovableObject *>::iterator iItr = cargoItems.begin(); iItr != cargoItems.end(); ++iItr)
							pLastPassenger->AddInventoryItem(*iItr);
					}
					// Clear out the temporary cargo list since we've assign all the stuff in it to a passenger
					cargoItems.clear();

					// Now set the current passenger as the 'last passenger' so he'll eventually get everything found after him.
					pLastPassenger = pPassenger;
					// Set the team etc for the current passenger and stuff him into the craft
					pPassenger->SetTeam(team);
					pPassenger->SetControllerMode(Controller::CIM_AI);
					pPassenger->SetAIMode((Actor::AIMode)mode);

					if (pTargetMO != NULL)
					{
						Actor * pTarget = dynamic_cast<Actor *>(pTargetMO);
						if (pTarget)
							pPassenger->AddAIMOWaypoint(pTarget);
					}
					else if (waypoint.m_X > 0 && waypoint.m_Y > 0)
					{
						pPassenger->AddAISceneWaypoint(waypoint);
					}

					pDeliveryCraft->AddInventoryItem(pPassenger);
				}
				// If not, then add it to the temp list of items which will be added to the last passenger's inventory
				else
					cargoItems.push_back(pInventoryObject);
			}
        }

        if (crabCount == 500)
            g_AchievementMan.UnlockAchievement("CC_MORECRABS");

        pPassenger = 0;

        // If there was a last passenger and things after him, stuff all the items into his inventory
        if (pLastPassenger)
        {
            for (list<MovableObject *>::iterator iItr = cargoItems.begin(); iItr != cargoItems.end(); ++iItr)
                pLastPassenger->AddInventoryItem(*iItr);
        }
        // Otherwise, stuff it all stuff directly into the craft instead
        else
        {
            for (list<MovableObject *>::iterator iItr = cargoItems.begin(); iItr != cargoItems.end(); ++iItr)
                pDeliveryCraft->AddInventoryItem(*iItr);
        }

        // Delivery craft appear straight over the selected LZ
        pDeliveryCraft->SetPos(Vector(m_LandingZone[player].m_X, 0));
//        pDeliveryCraft->SetPos(Vector(m_LandingZone[player].m_X, 300));

        pDeliveryCraft->SetTeam(team);
// TODO: The after-delivery AI mode needs to be set depending on what the user has set in teh LZ selection mode
        pDeliveryCraft->SetControllerMode(Controller::CIM_AI);
        pDeliveryCraft->SetAIMode(m_AIReturnCraft[player] ? Actor::AIMODE_DELIVER : Actor::AIMODE_STAY);

        // Prepare the Delivery struct and stuff the ready to go craft in there
        Delivery newDelivery;
        // Pass ownership of the craft to the new Delivery struct
        newDelivery.pCraft = pDeliveryCraft;
        newDelivery.orderedByPlayer = player;
        newDelivery.landingZone = m_LandingZone[player];
        newDelivery.delay = m_DeliveryDelay * pDeliveryCraft->GetDeliveryDelayMultiplier();
        newDelivery.timer.Reset();

        // Add the new Delivery to the queue
        m_Deliveries[team].push_back(newDelivery);

        pDeliveryCraft = 0;
        pLastPassenger = 0;

        // Deduct cost from team's funds
        m_TeamFunds[team] -= totalCost;

        // Go 'ding!', but only if player is human, or it may be confusing
		if (PlayerHuman(player))
			m_ConfirmSound.Play(0, player);

        // Clear out the override purchase list, whether anything was in there or not, it should not override twice.
        m_PurchaseOverride[player].clear();

        return true;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetupPlayers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Precalculates the player-to-screen index map, counts the number of
//                  active players, teams etc.

void GameActivity::SetupPlayers()
{
    Activity::SetupPlayers();

    // Add the locked cpu team that can't have any players
    if (m_CPUTeam != Activity::NOTEAM)
    {
        if (!m_TeamActive[m_CPUTeam])
            m_TeamCount++;
        // Also activate the CPU team
        m_TeamActive[m_CPUTeam] = true;
    }

    // Don't clear a CPU team's active status though
    for (int team = Activity::TEAM_1; team < MAXTEAMCOUNT; ++team)
	{
		if (m_TeamIsCPU[team])
			m_TeamActive[team] = true;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Start
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Officially starts this. Creates all the data etc necessary to start
//                  the activity.

int GameActivity::Start()
{
    // Set the split screen config before the Scene (and it SceneLayers, specifially) are loaded
    int humanCount = GetHumanCount();
    // Depending on the resolution aspect ratio, split first horizontally (if wide screen)
    if (((float)g_FrameMan.GetResX() / (float)g_FrameMan.GetResY()) >= 1.6)
        g_FrameMan.ResetSplitScreens(humanCount > 1, humanCount > 2);
    // or vertically (if 4:3-ish)
    else
        g_FrameMan.ResetSplitScreens(humanCount > 2, humanCount > 1);

    int error = Activity::Start();
    if (error < 0)
        return error;

    m_WinnerTeam = Activity::NOTEAM;

    ////////////////////////////////
    // Set up teams

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team])
            continue;

        m_Deliveries[team].clear();

        // Clear delivery queues
        for (deque<Delivery>::iterator itr = m_Deliveries[team].begin(); itr != m_Deliveries[team].end(); ++itr)
            delete itr->pCraft;
        m_Deliveries[team].clear();
/* This is taken care of by the individual Activity logic
        // See if there are specified landing zone areas defined in the scene
        char str[64];
        sprintf(str, "LZ Team %d", team + 1);
        Scene::Area *pArea = g_SceneMan.GetScene()->GetArea(str);
        pArea = pArea ? pArea : g_SceneMan.GetScene()->GetArea("Landing Zone");
        // If area is defined, save a copy so we can lock the LZ selection to within its boxes
        if (pArea && !pArea->HasNoArea())
            m_LandingZoneArea[team] = *pArea;
*/
    }

    ///////////////////////////////////////
    // Set up human players

    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;

        // Set the team associations with each screen displayed
        g_SceneMan.SetScreenTeam(ScreenOfPlayer(player), m_Team[player]);
        // And occlusion
        g_SceneMan.SetScreenOcclusion(Vector(), ScreenOfPlayer(player));

        // Allocate and (re)create the Pie Menus
        if (m_pPieMenu[player])
            m_pPieMenu[player]->Destroy();
        else
            m_pPieMenu[player] = new PieMenuGUI;
        m_pPieMenu[player]->Create(&m_PlayerController[player]);

        // Allocate and (re)create the Editor GUIs
        if (m_pEditorGUI[player])
            m_pEditorGUI[player]->Destroy();
        else
            m_pEditorGUI[player] = new SceneEditorGUI;
        m_pEditorGUI[player]->Create(&m_PlayerController[player]);
        m_ReadyToStart[player] = false;

        // Allocate and (re)create the Buy GUIs
        if (m_pBuyGUI[player])
            m_pBuyGUI[player]->Destroy();
        else
            m_pBuyGUI[player] = new BuyMenuGUI;
        m_pBuyGUI[player]->Create(&m_PlayerController[player]);

		// Load correct loadouts into buy menu if we're starting a non meta-game activity
		if (m_pBuyGUI[player]->GetMetaPlayer() == Activity::NOPLAYER)
		{
			m_pBuyGUI[player]->SetNativeTechModule(g_PresetMan.GetModuleID(GetTeamTech(GetTeamOfPlayer(player))));
			m_pBuyGUI[player]->SetForeignCostMultiplier(1.0);
			m_pBuyGUI[player]->LoadAllLoadoutsFromFile();

			// Change Editor GUI native tech module so it could load and show correct deployment prices
			m_pEditorGUI[player]->SetNativeTechModule(g_PresetMan.GetModuleID(GetTeamTech(GetTeamOfPlayer(player))));
		}

        ////////////////////////////////////
        // GUI split screen setup
        // If there are split screens, set up the GUIs to draw and their mouses to point correctly
		if (g_FrameMan.IsInMultiplayerMode())
		{
			m_pEditorGUI[player]->SetPosOnScreen(0, 0);
			m_pBuyGUI[player]->SetPosOnScreen(0, 0);
		}
		else
		{
			if (g_FrameMan.GetScreenCount() > 1)
			{
				// Screen 1 Always upper left corner
				if (ScreenOfPlayer(player) == 0)
				{
					m_pEditorGUI[player]->SetPosOnScreen(0, 0);
					m_pBuyGUI[player]->SetPosOnScreen(0, 0);
				}
				else if (ScreenOfPlayer(player) == 1)
				{
					// If both splits, or just Vsplit, then in upper right quadrant
					if ((g_FrameMan.GetVSplit() && !g_FrameMan.GetHSplit()) || (g_FrameMan.GetVSplit() && g_FrameMan.GetVSplit()))
					{
						m_pEditorGUI[player]->SetPosOnScreen(g_FrameMan.GetResX() / 2, 0);
						m_pBuyGUI[player]->SetPosOnScreen(g_FrameMan.GetResX() / 2, 0);
					}
					// If only hsplit, then lower left quadrant
					else
					{
						m_pEditorGUI[player]->SetPosOnScreen(0, g_FrameMan.GetResY() / 2);
						m_pBuyGUI[player]->SetPosOnScreen(0, g_FrameMan.GetResY() / 2);
					}
				}
				// Screen 3 is lower left quadrant
				else if (ScreenOfPlayer(player) == 2)
				{
					m_pEditorGUI[player]->SetPosOnScreen(0, g_FrameMan.GetResY() / 2);
					m_pBuyGUI[player]->SetPosOnScreen(0, g_FrameMan.GetResY() / 2);
				}
				// Screen 4 is lower right quadrant
				else if (ScreenOfPlayer(player) == 3)
				{
					m_pEditorGUI[player]->SetPosOnScreen(g_FrameMan.GetResX() / 2, g_FrameMan.GetResY() / 2);
					m_pBuyGUI[player]->SetPosOnScreen(g_FrameMan.GetResX() / 2, g_FrameMan.GetResY() / 2);
				}
			}
		}

        // Allocate and (re)create the banners
        if (m_pBannerRed[player])
            m_pBannerRed[player]->Destroy();
        else
            m_pBannerRed[player] = new GUIBanner;
        m_pBannerRed[player]->Create("Base.rte/GUIs/Fonts/BannerFontRedReg.bmp", "Base.rte/GUIs/Fonts/BannerFontRedBlur.bmp", 8);

        // Allocate and (re)create the banners
        if (m_pBannerYellow[player])
            m_pBannerYellow[player]->Destroy();
        else
            m_pBannerYellow[player] = new GUIBanner;
        m_pBannerYellow[player]->Create("Base.rte/GUIs/Fonts/BannerFontYellowReg.bmp", "Base.rte/GUIs/Fonts/BannerFontYellowBlur.bmp", 8);

        // Resetting the banner repeat counter
        m_BannerRepeats[player] = 0;

        // Draw GO! game start notification
        m_pBannerYellow[player]->ShowText("GO!", GUIBanner::FLYBYLEFTWARD, 1000, Vector(g_FrameMan.GetPlayerFrameBufferWidth(player), g_FrameMan.GetPlayerFrameBufferHeight(player)), 0.5, 1500, 500);

        m_ActorCursor[player].Reset();
        m_LandingZone[player].Reset();

        // Set the initial landing zones to be above the respective brains, but not for the observer player in a three player game
        if (m_pBrain[player] && !(m_PlayerCount == 3 && ScreenOfPlayer(player) == 3))
        {
            // Also set the brain to be the selected actor at start
            SwitchToActor(m_pBrain[player], player, m_Team[player]);
            m_ActorCursor[player] = m_pBrain[player]->GetPos();
            m_LandingZone[player].m_X = m_pBrain[player]->GetPos().m_X;
            // Set the observation target to the brain, so that if/when it dies, the view flies to it in observation mode
            m_ObservationTarget[player] = m_pBrain[player]->GetPos();
        }

        g_FrameMan.SetScreenText((player % 2 == 0) ? "Mine Gold and buy more firepower with the funds..." : "...then smash the competing brain to claim victory!", ScreenOfPlayer(player), 0);
    }

    // Set up the AI controllers for everyone
    InitAIs();

    // Start the game timer
    m_GameTimer.Reset();
    m_DemoTimer.Reset();

    if (!(m_aLZCursor[0]))
    {
        ContentFile cursorFile("Base.rte/GUIs/LZArrowRedL.bmp");
        m_aLZCursor[0] = cursorFile.GetAsAnimation(LZCURSORFRAMECOUNT);
        cursorFile.SetDataPath("Base.rte/GUIs/LZArrowGreenL.bmp");
        m_aLZCursor[1] = cursorFile.GetAsAnimation(LZCURSORFRAMECOUNT);
    }

    if (!(m_aObjCursor[0]))
    {
        ContentFile cursorFile("Base.rte/GUIs/ObjArrowRed.bmp");
        m_aObjCursor[0] = cursorFile.GetAsAnimation(OBJARROWFRAMECOUNT);
        cursorFile.SetDataPath("Base.rte/GUIs/ObjArrowGreen.bmp");
        m_aObjCursor[1] = cursorFile.GetAsAnimation(OBJARROWFRAMECOUNT);
    }

    // Start the in-game music
    g_AudioMan.ClearMusicQueue();
    g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/cc2g.ogg", 0);
    g_AudioMan.QueueSilence(30);
    g_AudioMan.QueueMusicStream("Base.rte/Music/Watts/Last Man.ogg");
    g_AudioMan.QueueSilence(30);
    g_AudioMan.QueueMusicStream("Base.rte/Music/dBSoundworks/cc2g.ogg");

    return error;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Pause
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Pauses and unpauses the game.

void GameActivity::Pause(bool pause)
{
    if (pause)
        m_PausedDemoTime = m_DemoTimer.GetElapsedRealTimeMS();
    else
        m_DemoTimer.SetElapsedRealTimeMS(m_PausedDemoTime);

    Activity::Pause(pause);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          End
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Forces the current game's end.

void GameActivity::End()
{
    Activity::End();

    bool playerWon = false;

    // Disable control of actors.. will be handed over to the observation targets instead
    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;

        g_SceneMan.SetScreenOcclusion(Vector(), ScreenOfPlayer(player));

        if (m_Team[player] == m_WinnerTeam)
        {
            playerWon = true;
            // Set the winner's observation view to his controlled actors instead of his brain
            if (m_pControlledActor[player] && g_MovableMan.IsActor(m_pControlledActor[player]))
                m_ObservationTarget[player] = m_pControlledActor[player]->GetPos();
        }
    }

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team])
            continue;
        for (deque<Delivery>::iterator itr = m_Deliveries[team].begin(); itr != m_Deliveries[team].end(); ++itr)
            delete itr->pCraft;
        m_Deliveries[team].clear();
    }

    if (GetWinnerTeam() >= 0)
    {
        g_AchievementMan.ProgressAchievement("CC_WARTORN", 1, 999);

        if (IsPlayerTeam(GetWinnerTeam()))
        {
            int playerLosses = GetTeamDeathCount(GetWinnerTeam());
            int highestCount = 0;

            for (int i = Activity::TEAM_1; i < GetTeamCount(); i++)
            {
                if (i != GetWinnerTeam() && GetTeamDeathCount(i) > highestCount)
                {
                    highestCount = GetTeamDeathCount(i);
                }
            }

            if (playerLosses > highestCount)
            {
                g_AchievementMan.UnlockAchievement("CC_PYRRHICVICTORY");
            }
        }
        else
        {
            int playerTeam = 0;
            for (int i = Activity::TEAM_1; i < GetTeamCount(); i++)
            {
                if (IsPlayerTeam(i))
                {
                    playerTeam = i;
                    break;
                }
            }

            int winnerLosses = GetTeamDeathCount(GetWinnerTeam());
            int playerLosses = GetTeamDeathCount(playerTeam);

            if (winnerLosses > playerLosses)
            {
                g_AchievementMan.UnlockAchievement("CC_TECHNICALWIN"); // the best kind of win
            }
        }
    }
/* Now controlled by the scripted activities
    // Play the approriate tune on player win/lose
    if (playerWon)
    {
// Didn't work well, has gap between intro and loop tracks
//        g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/uwinintro.ogg", 0);
//        g_AudioMan.QueueMusicStream("Base.rte/Music/dBSoundworks/uwinloop.ogg");
        g_AudioMan.ClearMusicQueue();
        // Loop it twice, nice tune!
        g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/uwinfinal.ogg", 2);
        g_AudioMan.QueueSilence(10);
        g_AudioMan.QueueMusicStream("Base.rte/Music/dBSoundworks/ccambient4.ogg");
    }
    else
    {
        g_AudioMan.ClearMusicQueue();
        g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/udiedfinal.ogg", 0);
        g_AudioMan.QueueSilence(10);
        g_AudioMan.QueueMusicStream("Base.rte/Music/dBSoundworks/ccambient4.ogg");
    }
*/

    m_ActivityState = OVER;
    m_GameOverTimer.Reset();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateEditing
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     This is a special update step for when any player is still editing the
//                  scene.

void GameActivity::UpdateEditing()
{
    // Editing the scene, just update the editor guis and see if players are ready to start or not
    if (m_ActivityState != EDITING)
        return;

    ///////////////////////////////////////////
    // Iterate through all human players

    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;

        // Update the player controllers which control the switching and editor gui
        m_PlayerController[player].Update();
        m_pEditorGUI[player]->Update();

        // Set the team associations with each screen displayed
        g_SceneMan.SetScreenTeam(ScreenOfPlayer(player), m_Team[player]);

        // Check if the player says he's done editing, and if so, make sure he really is good to go
        if (m_pEditorGUI[player]->GetEditorGUIMode() == SceneEditorGUI::DONEEDITING)
        {
            // See if a brain has been placed yet by this player - IN A VALID LOCATION
            if (!m_pEditorGUI[player]->TestBrainResidence())
            {
                // Hm not ready yet without resident brain in the right spot, so let user know
                m_ReadyToStart[player] = false;
                const Entity *pBrain = g_PresetMan.GetEntityPreset("Actor", "Brain Case");
                if (pBrain)
                    m_pEditorGUI[player]->SetCurrentObject(dynamic_cast<SceneObject *>(pBrain->Clone()));
                m_pEditorGUI[player]->SetEditorGUIMode(SceneEditorGUI::INSTALLINGBRAIN);
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
                g_FrameMan.SetScreenText("PLACE YOUR BRAIN IN A VALID SPOT FIRST!", ScreenOfPlayer(player), 250, 3500);
                m_MsgTimer[player].Reset();
            }
            // Ready to start
            else
            {
                m_ReadyToStart[player] = true;
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
                g_FrameMan.SetScreenText("READY to start - wait for others to finish...", ScreenOfPlayer(player), 333);
                m_pEditorGUI[player]->SetEditorGUIMode(SceneEditorGUI::ADDINGOBJECT);
            }
        }

        // Keep showing ready message
        if (m_ReadyToStart[player])
            g_FrameMan.SetScreenText("READY to start - wait for others to finish...", ScreenOfPlayer(player), 333);
    }

    // Have all players flagged themselves as ready to start the game?
    bool allReady = true;
    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;
        if (!m_ReadyToStart[player])
            allReady = false;
    }

    // YES, we are allegedly all ready to stop editing and start the game!
    if (allReady)
    {
        // Make sure any players haven't moved or entombed their brains in the period after flagging themselves "done"
        for (int player = 0; player < MAXPLAYERCOUNT; ++player)
        {
            if (!(m_IsActive[player] && m_IsHuman[player]))
                continue;
            // See if a brain has been placed yet by this player - IN A VALID LOCATION
            if (!m_pEditorGUI[player]->TestBrainResidence())
            {
                // Hm not ready yet without resident brain in the right spot, so let user know
                m_ReadyToStart[player] = false;
                allReady = false;
                const Entity *pBrain = g_PresetMan.GetEntityPreset("Actor", "Brain Case");
                if (pBrain)
                    m_pEditorGUI[player]->SetCurrentObject(dynamic_cast<SceneObject *>(pBrain->Clone()));
                m_pEditorGUI[player]->SetEditorGUIMode(SceneEditorGUI::INSTALLINGBRAIN);
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
                g_FrameMan.SetScreenText("PLACE YOUR BRAIN IN A VALID SPOT FIRST!", ScreenOfPlayer(player), 333, 3500);
                m_MsgTimer[player].Reset();
            }
        }

        // Still good to go??
        if (allReady)
        {
            // All resident brains are still in valid spots - place them into the simulation
            for (int player = 0; player < MAXPLAYERCOUNT; ++player)
            {
                if (!(m_IsActive[player] && m_IsHuman[player]))
                    continue;

                // Place this player's resident brain into the simulation and set it as the player's assigned brain
                g_SceneMan.GetScene()->PlaceResidentBrain(player, *this);

                // Still no brain of this player? Last ditch effort to find one and assign it to this player
                if (!m_pBrain[player])
                    m_pBrain[player] = g_MovableMan.GetUnassignedBrain(m_Team[player]);
                // Um, something went wrong.. we're not done placing brains after all??
                if (!m_pBrain[player])
                {
                    allReady = false;
                    // Get the brains back into residency so the players who are OK are still so
                    g_SceneMan.GetScene()->RetrieveResidentBrains(*this);
                    break;
                }

                // Set the brain to be the selected actor at start
                SwitchToActor(m_pBrain[player], player, m_Team[player]);
                m_ActorCursor[player] = m_pBrain[player]->GetPos();
                m_LandingZone[player].m_X = m_pBrain[player]->GetPos().m_X;
                // Set the observation target to the brain, so that if/when it dies, the view flies to it in observation mode
                m_ObservationTarget[player] = m_pBrain[player]->GetPos();
                // CLear the messages before starting the game
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
                // Reset the screen occlusion if any players are still in menus
                g_SceneMan.SetScreenOcclusion(Vector(), ScreenOfPlayer(player));
            }
        }

        // Still good to go?? then GO
        if (allReady)
        {
            // START the game!
            m_ActivityState = RUNNING;
            // Start the demo time
            m_DemoTimer.Reset();
            // Re-enable the AI's if we are done editing
            DisableAIs(false);
            InitAIs();
            // Reset the mouse value and pathfinding so it'll know about the newly placed stuff
            g_UInputMan.SetMouseValueMagnitude(0);
            g_SceneMan.GetScene()->ResetPathFinding();
            // Start the in-game track
            g_AudioMan.ClearMusicQueue();
            g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/cc2g.ogg", 0);
            g_AudioMan.QueueSilence(30);
            g_AudioMan.QueueMusicStream("Base.rte/Music/Watts/Last Man.ogg");
            g_AudioMan.QueueSilence(30);
            g_AudioMan.QueueMusicStream("Base.rte/Music/dBSoundworks/cc2g.ogg");
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this GameActivity. Supposed to be done every frame
//                  before drawing.

void GameActivity::Update()
{
    SLICK_PROFILE(0xFF446688);

    Activity::Update();

    // Avoid game logic when we're editing
    if (m_ActivityState == EDITING)
    {
        UpdateEditing();
        return;
    }

    ///////////////////////////////////////////
    // Iterate through all human players

    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;

        // The current player's team
        int team = m_Team[player];
        if (team == NOTEAM)
            continue;

        // Temporary hack to avoid teh buy menu buy button to be pressed immediately after selecting an LZ for a previous order
        bool skipBuyUpdate = false;

        // Set the team associations with each screen displayed
        g_SceneMan.SetScreenTeam(ScreenOfPlayer(player), team);

        //////////////////////////////////////////////////////
        // Assure that Controlled Actor is a safe pointer

        // Only allow this if player's brain is intact
        if (g_MovableMan.IsActor(m_pBrain[player]))
        {
            // Note that we have now had a brain
            m_HadBrain[player] = true;

            // Tracking normally
            if (m_ViewState[player] == NORMAL)
            {
                // Get a next actor if there isn't one
                if (!m_pControlledActor[player])
                    SwitchToNextActor(player, team);

                // Continually set the observation target to the brain during play, so that if/when it dies, the view flies to it in observation mode
                if (m_ActivityState != OVER && m_ViewState[player] != OBSERVE)
                    m_ObservationTarget[player] = m_pBrain[player]->GetPos();

                // Save the location of the currently controlled actor so we can know where to watch if he died on us
                if (g_MovableMan.IsActor(m_pControlledActor[player]))
                {
                    m_DeathViewTarget[player] = m_pControlledActor[player]->GetPos();
                }
                // Add delay after death before switching so the death comedy can be witnessed
                // Died, so enter death watch mode
                else
                {
                    m_pControlledActor[player] = 0;
                    m_ViewState[player] = DEATHWATCH;
                    m_DeathTimer[player].Reset();
                }
            }
            // Ok, done watching death comedy, now automatically switch
            else if (m_ViewState[player] == DEATHWATCH && m_DeathTimer[player].IsPastSimMS(1500))
            {
                // Get a next actor if there isn't one
                if (!m_pControlledActor[player])
                    SwitchToNextActor(player, team);

                // If currently focused actor died, get next one
                if (!g_MovableMan.IsActor(m_pControlledActor[player]))
                    SwitchToNextActor(player, team);

                if (m_ViewState[player] != ACTORSELECT)
                    m_ViewState[player] = NORMAL;
            }
            // Any other viewing mode and the actor died... go to deathwatch
            else if (m_pControlledActor[player] && !g_MovableMan.IsActor(m_pControlledActor[player]))
            {
                m_pControlledActor[player] = 0;
                m_ViewState[player] = DEATHWATCH;
                m_DeathTimer[player].Reset();
            }
        }
        // Player brain is now gone! Remove any control he may have had
        else if (m_HadBrain[player])
        {
            if (m_pControlledActor[player] && g_MovableMan.IsActor(m_pControlledActor[player]))
                m_pControlledActor[player]->SetControllerMode(Controller::CIM_AI);
            m_pControlledActor[player] = 0;
            m_ViewState[player] = OBSERVE;
        }
        // Never had a brain, and no actor is selected, so just select the first one we do have
        else if (m_ViewState[player] != OBSERVE && !g_MovableMan.IsActor(m_pControlledActor[player]))
        {
            // Only try to switch if there's somehting to switch to
            if (!g_MovableMan.GetTeamRoster(team)->empty())
                SwitchToNextActor(player, team);
            else
                m_pControlledActor[player] = 0;
        }

        ///////////////////////////////////////////
        // Player-commanded actor switching

        // Switch to brain actor directly if the player wants to
        if (m_PlayerController[player].IsState(ACTOR_BRAIN) && m_ViewState[player] != ACTORSELECT)
        {
            SwitchToActor(m_pBrain[player], player, team);
            m_ViewState[player] = NORMAL;
        }
        // Switch to next actor if the player wants to
        else if (m_PlayerController[player].IsState(ACTOR_NEXT) && m_ViewState[player] != ACTORSELECT)
        {
            SwitchToNextActor(player, team);
            m_ViewState[player] = NORMAL;
            g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
        }
        // Switch to prev actor if the player wants to
        else if (m_PlayerController[player].IsState(ACTOR_PREV) && m_ViewState[player] != ACTORSELECT)
        {
            SwitchToPrevActor(player, team);
            m_ViewState[player] = NORMAL;
            g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
        }
        // Go into manual actor select mode if either actor switch buttons are held for a duration
        else if (m_ViewState[player] != ACTORSELECT && !m_pBuyGUI[player]->IsVisible() && (m_PlayerController[player].IsState(ACTOR_NEXT_PREP) || m_PlayerController[player].IsState(ACTOR_PREV_PREP)))
        {
            if (m_ActorSelectTimer[player].IsPastRealMS(250))
            {
                // Set cursor to start at the head of controlled actor
                if (m_pControlledActor[player])
                {
                    // Give switched from actor an AI controller
                    m_pControlledActor[player]->SetControllerMode(Controller::CIM_AI);
                    m_pControlledActor[player]->GetController()->SetDisabled(false);
                    m_ActorCursor[player] = m_pControlledActor[player]->GetCPUPos();
                    m_CursorTimer.Reset();
                }
                m_ViewState[player] = ACTORSELECT;
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
			}
        }
        else
            m_ActorSelectTimer[player].Reset();

        ////////////////////////////////////
        // Update sceneman scroll targets

        if (m_ViewState[player] == OBSERVE)
        {
            // If we're observing game over state, freeze the view for a bit so the player's input doesn't ruin the focus
            if (!(m_ActivityState == OVER && !m_GameOverTimer.IsPastRealMS(1000)))
            {
                // Get cursor input
                m_PlayerController[player].RelativeCursorMovement(m_ObservationTarget[player], 1.2f);
            }
            // Set the view to the observation position
            g_SceneMan.SetScrollTarget(m_ObservationTarget[player], 0.1, g_SceneMan.ForceBounds(m_ObservationTarget[player]), ScreenOfPlayer(player));
        }

        ///////////////////////////////////////////////////
        // Manually selecting a new actor to switch to

        else if (m_ViewState[player] == ACTORSELECT)
        {
            // Continuously display message
            g_FrameMan.SetScreenText("Select a body to switch control to...", ScreenOfPlayer(player));
            // Get cursor input
            m_PlayerController[player].RelativeCursorMovement(m_ActorCursor[player]);

            // Find the actor closest to the cursor, if any within the radius
            float markedDistance = -1;
            Actor *pMarkedActor = g_MovableMan.GetClosestTeamActor(team, player, m_ActorCursor[player], g_SceneMan.GetSceneWidth(), markedDistance);
//            Actor *pMarkedActor = g_MovableMan.GetClosestTeamActor(team, player, m_ActorCursor[player], g_FrameMan.GetPlayerScreenWidth() / 4);

            // Show the pie menu if not already
//            if (!m_pPieMenu[player]->IsEnabled())
//                m_pPieMenu[player]->SetEnabled(true);

            // Player canceled selection of actor
            if (m_PlayerController[player].IsState(PRESS_SECONDARY))
            {
                // Reset the mouse so the actor doesn't change aim because mouse has been moved
                if (m_PlayerController[player].IsMouseControlled())
                    g_UInputMan.SetMouseValueMagnitude(0);
                // Switch back to normal view
                m_ViewState[player] = NORMAL;
                // Play err sound to indicate cancellation
                m_UserErrorSound.Play(0, player);
                // Flash the same actor, jsut to show the control went back to him
                m_pPieMenu[player]->DisableAnim();
            }
            // Player is done selecting new actor; switch to it if we have anything marked
            else if (m_PlayerController[player].IsState(ACTOR_NEXT) || m_PlayerController[player].IsState(ACTOR_PREV))// || m_PlayerController[player].IsState(PRESS_FACEBUTTON) || m_PlayerController[player].IsState(PRESS_PRIMARY))
            {
                // Reset the mouse so the actor doesn't change aim because mouse has been moved
                if (m_PlayerController[player].IsMouseControlled())
                    g_UInputMan.SetMouseValueMagnitude(0);

                // If we have something to switch to, then do so
                if (pMarkedActor)
                    SwitchToActor(pMarkedActor, player, team);
                // If not, boop
                else
                    m_UserErrorSound.Play(0, player);

                // Switch back to normal view
                m_ViewState[player] = NORMAL;
                // Stop displaying the message
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
                // Flash the same actor, jsut to show the control went back to him
                m_pPieMenu[player]->DisableAnim();
            }
            else
            {
                // If an actor is marked, move the pie menu over it to show
                if (pMarkedActor)
                {
                    // Show the pie menu switching animation over the highlighted Actor
                    m_pPieMenu[player]->SetPos(pMarkedActor->GetPos());

                    if (markedDistance > g_FrameMan.GetPlayerFrameBufferWidth(player) / 4)
                        m_pPieMenu[player]->WobbleAnim();
                    else
                        m_pPieMenu[player]->FreezeAtRadius(30);
                }
                else if (m_pPieMenu[player]->IsEnabled())
                    m_pPieMenu[player]->SetEnabled(false);
            }

            // Set the view to the cursor pos
            bool wrapped = g_SceneMan.ForceBounds(m_ActorCursor[player]);
            g_SceneMan.SetScrollTarget(m_ActorCursor[player], 0.1, wrapped, ScreenOfPlayer(player));
        }

        ///////////////////////////////////////////////////
        // Selecting points on the scene for the AI to go to

        else if (m_ViewState[player] == AIGOTOPOINT)
        {
            // Continuously display message
            g_FrameMan.SetScreenText("Set waypoints for the AI to go to...", ScreenOfPlayer(player));
            // Get cursor input
            m_PlayerController[player].RelativeCursorMovement(m_ActorCursor[player]);

            // If we are pointing to an actor to follow, then snap cursor to that actor's position
            Actor *pTargetActor = 0;
            float distance = 0;
            if (pTargetActor = g_MovableMan.GetClosestActor(m_ActorCursor[player], 40, distance, m_pControlledActor[player]))
            {
//                m_ActorCursor[player] = pTargetActor->GetPos();

                // Make pie menu wobble over hovered MO to follow
                m_pPieMenu[player]->SetEnabled(true);
                m_pPieMenu[player]->SetPos(pTargetActor->GetPos());
                m_pPieMenu[player]->FreezeAtRadius(15);
            }
            else
                m_pPieMenu[player]->SetEnabled(false);

            // Set the view to the cursor pos
            bool wrapped = g_SceneMan.ForceBounds(m_ActorCursor[player]);
            g_SceneMan.SetScrollTarget(m_ActorCursor[player], 0.1, wrapped, ScreenOfPlayer(player));

            // Draw the actor's waypoints
            m_pControlledActor[player]->DrawWaypoints(true);

            // Disable the actor's controller
            m_pControlledActor[player]->GetController()->SetDisabled(true);

            // Player is done setting waypoints
            if (m_PlayerController[player].IsState(PRESS_SECONDARY))
            {
                // Stop drawing the waypoints
//                m_pControlledActor[player]->DrawWaypoints(false);
                // Update the player's move path now to the first waypoint set
                m_pControlledActor[player]->UpdateMovePath();
                // Give player control back to actor
                m_pControlledActor[player]->GetController()->SetDisabled(false);
                // Switch back to normal view
                m_ViewState[player] = NORMAL;
                // Stop displaying the message
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
            }
            // Player set a new waypoint
            else if (m_pControlledActor[player] && m_PlayerController[player].IsState(PRESS_FACEBUTTON) || m_PlayerController[player].IsState(PRESS_PRIMARY))
            {
// TODO: Sound?
                // If we are pointing to an actor to follow, tehn give that kind of waypoint command
                if (pTargetActor)
                    m_pControlledActor[player]->AddAIMOWaypoint(pTargetActor);
                // Just pointing into somewhere in the scene, so give that command
                else
                    m_pControlledActor[player]->AddAISceneWaypoint(m_ActorCursor[player]);
                // Update the player's move path now to the first waypoint set
                m_pControlledActor[player]->UpdateMovePath();
            }
        }
        else if (m_ViewState[player] == UNITSELECTCIRCLE)
        {
            // Continuously display message
            g_FrameMan.SetScreenText("Select units to group...", ScreenOfPlayer(player));

			m_PlayerController[player].RelativeCursorMovement(m_ActorCursor[player]);

			Vector relativeToActor = m_ActorCursor[player] - m_pControlledActor[player]->GetPos();

			float sceneWidth = g_SceneMan.GetSceneWidth();

			//Check if we crossed the seam
			if (g_SceneMan.GetScene()->WrapsX())
				if (relativeToActor.GetMagnitude() > sceneWidth / 2 && relativeToActor.GetMagnitude() > 350)
					if (m_ActorCursor->m_X < sceneWidth / 2)
						relativeToActor = m_ActorCursor[player] + Vector(sceneWidth , 0) - m_pControlledActor[player]->GetPos();
					else
						relativeToActor = m_ActorCursor[player] - Vector(sceneWidth , 0) - m_pControlledActor[player]->GetPos();
	
			// Limit selection range
			relativeToActor = relativeToActor.CapMagnitude(350);
			m_ActorCursor[player] = m_pControlledActor[player]->GetPos() + relativeToActor;

            m_pPieMenu[player]->SetEnabled(false);

			bool wrapped;

            // Set the view to the cursor pos
            wrapped = g_SceneMan.ForceBounds(m_ActorCursor[player]);
            //g_SceneMan.SetScrollTarget(m_ActorCursor[player], 0.1, wrapped, ScreenOfPlayer(player));

            // Set the view to the actor pos
			Vector scrollPos = Vector(m_pControlledActor[player]->GetPos());
            wrapped = g_SceneMan.ForceBounds(scrollPos);
            g_SceneMan.SetScrollTarget(scrollPos, 0.1, wrapped, ScreenOfPlayer(player));

            // Disable the actor's controller
            m_pControlledActor[player]->GetController()->SetDisabled(true);

            // Player is done setting waypoints
            if (m_PlayerController[player].IsState(PRESS_SECONDARY))
            {
                // Give player control back to actor
                m_pControlledActor[player]->GetController()->SetDisabled(false);
                // Switch back to normal view
                m_ViewState[player] = NORMAL;
                // Stop displaying the message
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
            }
            // Player set a new waypoint
            else if (m_pControlledActor[player] && m_PlayerController[player].IsState(PRESS_FACEBUTTON) || m_PlayerController[player].IsState(PRESS_PRIMARY))
            {
                //    m_pControlledActor[player]->AddAISceneWaypoint(m_ActorCursor[player]);
                // Give player control back to actor
                m_pControlledActor[player]->GetController()->SetDisabled(false);
                // Switch back to normal view
                m_ViewState[player] = NORMAL;
                // Stop displaying the message
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));

				//Switch commander to sentry mode
				m_pControlledActor[player]->SetAIMode(Actor::AIMODE_SENTRY);

				// Detect nearby actors and attach them to commander
				float radius = g_SceneMan.ShortestDistance(m_ActorCursor[player],m_pControlledActor[player]->GetPos(), true).GetMagnitude();

				Actor *pActor = 0;
				Actor *pFirstActor = 0;

				// Get the first one
				pFirstActor = pActor = g_MovableMan.GetNextTeamActor(m_pControlledActor[player]->GetTeam());

				do
				{
					// Set up commander if actor is not player controlled and not brain
					if (pActor && !pActor->GetController()->IsPlayerControlled() && !pActor->IsInGroup("Brains"))
					{
						// If human, set appropriate AI mode
						if (dynamic_cast<AHuman *>(pActor) || dynamic_cast<ACrab *>(pActor))
							if (g_SceneMan.ShortestDistance(m_pControlledActor[player]->GetPos(), pActor->GetPos(),true).GetMagnitude() < radius)
							{
								pActor->FlashWhite();
								pActor->ClearAIWaypoints();
								pActor->SetAIMode(Actor::AIMODE_SQUAD);
								pActor->AddAIMOWaypoint(m_pControlledActor[player]);
                                pActor->UpdateMovePath();   // Make sure pActor has m_pControlledActor registered as an AIMOWaypoint
							}
					}

					// Next!
					pActor = g_MovableMan.GetNextTeamActor(team, pActor);
				}
				while (pActor && pActor != pFirstActor);
            }
        }
        ///////////////////////////////////////////////////
        // Selecting LZ, a place for the craft to land

        else if (m_ViewState[player] == LZSELECT)
        {
            // Continuously display LZ message
            g_FrameMan.SetScreenText("Choose your landing zone...", ScreenOfPlayer(player));

            // Save the x pos so we can see which direction the user is moving it
            float prevLZX = m_LandingZone[player].m_X;

            // See if there's analog input
            if (m_PlayerController[player].GetAnalogMove().m_X > 0.1)
                m_LandingZone[player].m_X += m_PlayerController[player].GetAnalogMove().m_X * 8;
            // Try the mouse
            else if (!m_PlayerController[player].GetMouseMovement().IsZero())
                m_LandingZone[player].m_X += m_PlayerController[player].GetMouseMovement().m_X;
            // Digital movement
            else
            {
                if (m_PlayerController[player].IsState(MOVE_RIGHT))
                    m_LandingZone[player].m_X += 8;
                else if (m_PlayerController[player].IsState(MOVE_LEFT))
                    m_LandingZone[player].m_X -= 8;
            }

            // Limit the LZ selection to the special LZ Area:s both specified in the derived Activity and moving with the brain
            if (!m_LandingZoneArea[m_Team[player]].HasNoArea())
            {
                // Add up the static LZ loaded from the Scene to the one(s) around the player's team's brains
                Scene::Area totalLZ(m_LandingZoneArea[m_Team[player]]);
/* This whole concept kinda sucks - defensive AA robots are more fun way to go to prevent bumrushing the brain with craft
                for (int p = 0; p < MAXPLAYERCOUNT; ++p)
                {
                    if (!(m_IsActive[p] && m_IsHuman[p] && m_BrainLZWidth[p] > 0))
                        continue;
                    // Same team as this player and has a brain
                    if (m_pBrain[p] && m_Team[p] == m_Team[player])
                    {
                        Box brainBox(Vector(0, 0), m_BrainLZWidth[p], 100);
                        // Center the brain LZ box aroud the player's brain
                        brainBox.SetCenter(m_pBrain[p]->GetPos());
                        // Add it to the total LZ
                        totalLZ.AddBox(brainBox);
                    }
                }
*/
                // Figure out the direction the player is moving the cursor, so we can jump it in the right direction if needed
                int direction = m_LandingZone[player].m_X - prevLZX;
                // Move the actual LZ cursor to within the valid LZ Area
                totalLZ.MovePointInsideX(m_LandingZone[player].m_X, direction);
            }

            // Interface for the craft AI post-delivery mode
            if (m_PlayerController[player].IsState(PRESS_DOWN))// || m_PlayerController[player].IsState(PRESS_SECONDARY))
            {
                if (m_AIReturnCraft[player])
                    m_SelectionChangeSound.Play(0, player);
                else
                    m_UserErrorSound.Play(0, player);

                m_AIReturnCraft[player] = false;
            }
            else if (m_PlayerController[player].IsState(PRESS_UP))// || m_PlayerController[player].IsState(PRESS_SECONDARY))
            {
                if (!m_AIReturnCraft[player])
                    m_SelectionChangeSound.Play(0, player);
                else
                    m_UserErrorSound.Play(0, player);

                m_AIReturnCraft[player] = true;
            }

            // Player canceled the order while selecting LZ - can't be done in pregame
            if (m_PlayerController[player].IsState(PRESS_SECONDARY) && m_ActivityState != PREGAME)
            {
                // Switch back to normal view
                m_ViewState[player] = NORMAL;
                // Play err sound to indicate cancellation
                g_FrameMan.SetScreenText("Order cancelled!", ScreenOfPlayer(player), 333);
                m_MsgTimer[player].Reset();
                m_UserErrorSound.Play(0, player);
                // Flash the same actor, jsut to show the control went back to him
                m_pPieMenu[player]->DisableAnim();
            }
            // Player is done selecting LZ,
            else if (m_PlayerController[player].IsState(PRESS_FACEBUTTON) || m_PlayerController[player].IsState(PRESS_PRIMARY))
            {
                // Set the LZ cursor to be just over terran, to avoid getting stuck at halfway interpolating there
                m_LandingZone[player].m_Y = 0;
                m_LandingZone[player].m_Y = g_SceneMan.FindAltitude(m_LandingZone[player], g_SceneMan.GetSceneHeight(), 10);
                // Complete the purchase by getting the order from the BuyGUI, build it, and stuff it into a delivery
                CreateDelivery(player);
                // If there are no other Actors on this team, just view the LZ we have selected
                if (!g_MovableMan.GetNextTeamActor(team))
                {
                    m_ObservationTarget[player] = m_LandingZone[player];
                    m_ViewState[player] = OBSERVE;
                }
                // If there are other guys around, switch back to normal view
                else
                    m_ViewState[player] = NORMAL;
                // Stop displaying the LZ message
                g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
                // Flash the same actor, jsut to show the control went back to him
                m_pPieMenu[player]->DisableAnim();
            }

            bool wrapped = g_SceneMan.ForceBounds(m_LandingZone[player]);

            // Interpolate the LZ altitude to the height of the highest terrain point at the player-chosen X
            float prevHeight = m_LandingZone[player].m_Y;
            m_LandingZone[player].m_Y = 0;
            m_LandingZone[player].m_Y = prevHeight + ((g_SceneMan.FindAltitude(m_LandingZone[player], g_SceneMan.GetSceneHeight(), 10) - prevHeight) * 0.2);

            // Set the view to a little above the LZ position
            Vector viewTarget(m_LandingZone[player].m_X, m_LandingZone[player].m_Y - (g_FrameMan.GetPlayerScreenHeight() / 4));
            g_SceneMan.SetScrollTarget(viewTarget, 0.1, wrapped, ScreenOfPlayer(player));
        }

        ////////////////////////////
        // Deathwatching

        else if (m_ViewState[player] == DEATHWATCH)
        {
            // Continuously deathwatch message
            g_FrameMan.SetScreenText("Lost control of remote body!", ScreenOfPlayer(player));
            // Don't move anything, just stay put watching the death funnies
            g_SceneMan.SetScrollTarget(m_DeathViewTarget[player], 0.1, false, ScreenOfPlayer(player));
        }

        ////////////////////////////////////////////////////
        // Normal scrolling to view the currently controlled Actor
		// But only if we're not editing something, because editor will scroll the screen himself
		// and double scrolling will cause CC gitch when we'll cross the seam
		else if (m_pControlledActor[player] && m_ActivityState != EDITING && m_ActivityState != PREGAME)
        {
            g_SceneMan.SetScrollTarget(m_pControlledActor[player]->GetViewPoint(), 0.1, m_pControlledActor[player]->DidWrap(), ScreenOfPlayer(player));
        }

        ////////////////////////////////////////
        // Update the Pie Menu

        // Set the valid actor (or 0) so it can be flashed by the piemenu when it activates/deactivates
        m_pPieMenu[player]->SetActor(m_pControlledActor[player]);

        if (m_pControlledActor[player] && m_ViewState[player] != DEATHWATCH && m_ViewState[player] != ACTORSELECT && m_ViewState[player] != AIGOTOPOINT && m_ViewState[player] != UNITSELECTCIRCLE)
        {
            if (m_PlayerController[player].IsState(PIE_MENU_ACTIVE))
            {
                if (!m_pPieMenu[player]->IsEnabled() || m_pControlledActor[player]->PieNeedsUpdate())
                {
                    // Remove all previous slices
                    m_pPieMenu[player]->ResetSlices();
                    // Add the slices that the actor needs
                    m_pControlledActor[player]->AddPieMenuSlices(m_pPieMenu[player]);
                    // Add some additional universal ones
					if (m_BuyMenuEnabled)
					{
						PieMenuGUI::Slice buySlice("Buy Menu", PieMenuGUI::PSI_BUYMENU, PieMenuGUI::Slice::LEFT);
						m_pPieMenu[player]->AddSlice(buySlice);
					}
                    // Brain-specific options
                    if (m_pControlledActor[player] == m_pBrain[player])
                    {
						//PieMenuGUI::Slice statsSlice("Statistics", PieMenuGUI::PSI_STATS, PieMenuGUI::Slice::RIGHT, false);
                        //m_pPieMenu[player]->AddSlice(statsSlice, true);
						//PieMenuGUI::Slice ceaseFireSlice("Propose Cease Fire", PieMenuGUI::PSI_CEASEFIRE, PieMenuGUI::Slice::RIGHT, false);
						//m_pPieMenu[player]->AddSlice(ceaseFireSlice, true);
                    }

					// Init the new slice positions and sizes, build the list of slices in menu
					m_pPieMenu[player]->RealignSlices();
					m_CurrentPieMenuPlayer = player;
					m_CurrentPieMenuSlices = GetCurrentPieMenuSlices(player);

					OnPieMenu(m_pControlledActor[player]);

                    // Realigns slices after possible external pie-menu changes
                    m_pPieMenu[player]->RealignSlices();
                    // Enable the pie menu
                    m_pPieMenu[player]->SetEnabled(true);
                }
            }
            else
                m_pPieMenu[player]->SetEnabled(false);
// TODO: FIX CRASH BUG HERE WHEN GAME OVER!
            // Set/save position of the menu
            m_pPieMenu[player]->SetPos(m_pControlledActor[player]->GetCPUPos());
        }

        // Update the Pie menu
        m_pPieMenu[player]->Update();

        // If it appears a slice has been activated, then let the controlled actor handle it
        int command = m_pPieMenu[player]->GetPieCommand();
        if (m_pControlledActor[player] && command != PieMenuGUI::PSI_NONE)
        {
            // AI mode commands that need extra points set in special view modes here
            if (command == PieMenuGUI::PSI_SENTRY)
                m_ViewState[player] = AISENTRYPOINT;
            else if (command == PieMenuGUI::PSI_PATROL)
                m_ViewState[player] = AIPATROLPOINTS;
            else if (command == PieMenuGUI::PSI_GOLDDIG)
                m_ViewState[player] = AIGOLDDIGPOINT;
            else if (command == PieMenuGUI::PSI_GOTO)
            {
                m_ViewState[player] = AIGOTOPOINT;
                // Clear out the waypoints
                m_pControlledActor[player]->ClearAIWaypoints();
                // Set cursor to the actor
                m_ActorCursor[player] = m_pControlledActor[player]->GetPos();
                // Disable Actor's controller while we set the waypoints
                m_pControlledActor[player]->GetController()->SetDisabled(true);
            }
			else if (command == PieMenuGUI::PSI_FORMSQUAD)
            {
				//Find out if we have any connected units, and disconnect them
				bool isCommander = false;

				Actor *pActor = 0;
				Actor *pFirstActor = 0;

				// Get the first one
				pFirstActor = pActor = g_MovableMan.GetNextTeamActor(m_pControlledActor[player]->GetTeam());

				// Reset commander if we have any subordinates
				do
				{
					if (pActor)
					{
						// Set appropriate AI mode
						if (dynamic_cast<AHuman *>(pActor) || dynamic_cast<ACrab *>(pActor))
							if (pActor->GetAIMOWaypointID() == m_pControlledActor[player]->GetID())
							{
								pActor->FlashWhite();
								pActor->ClearAIWaypoints();
                                pActor->SetAIMode((Actor::AIMode)m_pControlledActor[player]->GetAIMode()); // Inherit the leader's AI mode
								isCommander = true;
							}
					}

					// Next!
					pActor = g_MovableMan.GetNextTeamActor(team, pActor);
				}
				while (pActor && pActor != pFirstActor);

				//Now turn on selection UI, if we didn't disconnect anyone
				if (!isCommander)
				{
					m_ViewState[player] = UNITSELECTCIRCLE;
					// Set cursor to the actor
					m_ActorCursor[player] = m_pControlledActor[player]->GetPos() + Vector(50,-50);
					// Disable Actor's controller while we set the waypoints
					m_pControlledActor[player]->GetController()->SetDisabled(true);
				}
            }
			
			// TODO: More modes?

            // If the actor couldn't handle it, then it's probably a game specific one
            if (!m_pControlledActor[player]->HandlePieCommand(command))
            {
                if (command == PieMenuGUI::PSI_BUYMENU)
                {
                    m_pPieMenu[player]->SetEnabled(false);
                    m_pBuyGUI[player]->SetEnabled(true);
                    skipBuyUpdate = true;
                }
/*
                else if (command == PieMenuGUI::PSI_STATS)
                    ;
                else if (command == PieMenuGUI::PSI_MINIMAP)
                    ;
                else if (command == PieMenuGUI::PSI_CEASEFIRE)
                    ;
*/
            }
        }

        ///////////////////////////////////////
        // Update Buy Menu GUIs

        // Enable or disable the Buy Menus if the brain is selected, Skip if an LZ selection button press was just performed
        if (!skipBuyUpdate)
        {
//            m_pBuyGUI[player]->SetEnabled(m_pControlledActor[player] == m_pBrain[player] && m_ViewState[player] != LZSELECT && m_ActivityState != OVER);
            m_pBuyGUI[player]->Update();
        }

        // Trap the mouse if we're in gameplay and not in the buy menu
		g_UInputMan.TrapMousePos(!m_pBuyGUI[player]->IsEnabled(), player);

        // Start LZ picking mode if a purchase was made
        if (m_pBuyGUI[player]->PurchaseMade())
        {
            m_pBuyGUI[player]->SetEnabled(false);
//            SwitchToPrevActor(player, team, m_pBrain[player]);
            // Start selecting the landing zone
            m_ViewState[player] = LZSELECT;
            // Set this to zero so the cursor interpolates down from the sky
            m_LandingZone[player].m_Y = 0;
        }

        // After a while of game over, change messages to the final one for everyone
        if ((m_ActivityState == OVER || m_ActivityState == DEMOEND) && m_GameOverTimer.IsPastRealMS(m_GameOverPeriod))
        {
            g_FrameMan.ClearScreenText(ScreenOfPlayer(player));
            //g_FrameMan.SetScreenText("Press [Esc] to leave the battlefield", ScreenOfPlayer(player), 750);
			if (g_FrameMan.IsInMultiplayerMode())
				g_FrameMan.SetScreenText("All players must press and hold [BACKSPACE] to continue!", ScreenOfPlayer(player), 750);
			else
	            g_FrameMan.SetScreenText("Press [SPACE] or [START] to continue!", ScreenOfPlayer(player), 750);

            // Actually end on space
            if (m_GameOverTimer.IsPastSimMS(55000) || g_UInputMan.AnyStartPress())
            {
                g_ActivityMan.EndActivity();
                g_InActivity = false;
            }
        }

        ///////////////////////////////////
        // Enable/disable controlled actors' AI as appropriate when in menus

        if (m_pControlledActor[player])
        {
            // Don't disable when pie menu is active; it is done inside the Controller Update
            if (m_pBuyGUI[player]->IsVisible() || m_ViewState[player] == ACTORSELECT || m_ViewState[player] == LZSELECT || m_ViewState[player] == OBSERVE)
                m_pControlledActor[player]->GetController()->SetInputMode(Controller::CIM_AI);
            else
                m_pControlledActor[player]->GetController()->SetInputMode(Controller::CIM_PLAYER);
        }

        ///////////////////////////////////////
        // Configure banners to show when important things happen, like the game over or death of brain

        if (ActivityOver())
        {
            // Override previous messages
            if (m_pBannerRed[player]->IsVisible() && m_pBannerRed[player]->GetBannerText() != "FAIL")
                m_pBannerRed[player]->HideText(2500, 0);
            if (m_pBannerYellow[player]->IsVisible() && m_pBannerYellow[player]->GetBannerText() != "WIN")
                m_pBannerYellow[player]->HideText(2500, 0);

            // Player on a winning team
            if (GetWinnerTeam() == m_Team[player] && !m_pBannerYellow[player]->IsVisible())
                m_pBannerYellow[player]->ShowText("WIN", GUIBanner::FLYBYRIGHTWARD, 1000, Vector(g_FrameMan.GetPlayerFrameBufferWidth(player), g_FrameMan.GetPlayerFrameBufferHeight(player)), 0.5, 1500, 400);

            // Loser player
            if (GetWinnerTeam() != m_Team[player] && !m_pBannerRed[player]->IsVisible())
                m_pBannerRed[player]->ShowText("FAIL", GUIBanner::FLYBYLEFTWARD, 1000, Vector(g_FrameMan.GetPlayerFrameBufferWidth(player), g_FrameMan.GetPlayerFrameBufferHeight(player)), 0.5, 1500, 400);
        }
        // If a player had a brain that is now dead, but his team is not yet done, show the dead banner on his screen
        else if (m_ActivityState != EDITING && m_ActivityState != STARTING && m_HadBrain[player] && !m_pBrain[player] && !m_pBannerRed[player]->IsVisible())
        {
            // If repeated too many times, just let the banner stop at showing and not cycle
            if (m_BannerRepeats[player]++ < 6)
                m_pBannerRed[player]->ShowText("DEAD", GUIBanner::FLYBYLEFTWARD, 1000, Vector(g_FrameMan.GetPlayerFrameBufferWidth(player), g_FrameMan.GetPlayerFrameBufferHeight(player)), 0.5, 1500, 400);
            else
                m_pBannerRed[player]->ShowText("DEAD", GUIBanner::FLYBYLEFTWARD, -1, Vector(g_FrameMan.GetPlayerFrameBufferWidth(player), g_FrameMan.GetPlayerFrameBufferHeight(player)), 0.5, 1500, 400);
        }

        ///////////////////////////////////////
        // Update message banners

        m_pBannerRed[player]->Update();
        m_pBannerYellow[player]->Update();
    }

    ///////////////////////////////////////////
    // Iterate through all teams

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team])
            continue;

        // Pause deliveries if game hasn't started yet
        if (m_ActivityState == Activity::PREGAME)
        {
            for (deque<Delivery>::iterator itr = m_Deliveries[team].begin(); itr != m_Deliveries[team].end(); ++itr)
                (*itr).timer.Reset();
        }

        ////////////////////////////////
        // Delivery status update

        if (!m_Deliveries[team].empty())
        {
            int player = m_Deliveries[team].front().orderedByPlayer;
            if (m_MsgTimer[player].IsPastSimMS(1000))
            {
                char message[512];
    //                sprintf(message, "Delivery in %.2f secs", ((float)m_Deliveries[team].front().delay - (float)m_Deliveries[team].front().timer.GetElapsedSimTimeMS()) / 1000);
                sprintf(message, "Next delivery in %i secs", ((int)m_Deliveries[team].front().delay - (int)m_Deliveries[team].front().timer.GetElapsedSimTimeMS()) / 1000);
                g_FrameMan.SetScreenText(message, ScreenOfPlayer(player));
                m_MsgTimer[player].Reset();
            }
        }

        // Delivery has arrived! Unpack and put into the world
        if (!m_Deliveries[team].empty() && m_Deliveries[team].front().timer.IsPastSimMS(m_Deliveries[team].front().delay))
        {
            // This is transferring ownership of the craft instance from the Delivery struct
            ACraft *pDeliveryCraft = m_Deliveries[team].front().pCraft;
            int player = m_Deliveries[team].front().orderedByPlayer;
            if (pDeliveryCraft)
            {
                g_FrameMan.SetScreenText("Your order has arrived!", ScreenOfPlayer(player), 333);
                m_MsgTimer[player].Reset();

                pDeliveryCraft->ResetEmissionTimers();  // Reset the engine timers so they don't emit a massive burst after being added to the world
                pDeliveryCraft->Update();

                // Add the delivery craft to the world, TRANSFERRING OWNERSHIP
                g_MovableMan.AddActor(pDeliveryCraft);
/*
                // If the player who ordered this seems stuck int he manu waiting for the delivery, give him direct control
                if (m_pControlledActor[player] == m_pBrain[player] && m_ViewState[player] != LZSELECT)
                {
                    SwitchToActor(pDeliveryCraft, player, team);
                }
*/
            }
            m_Deliveries[team].pop_front();
        }
    }

    ///////////////////////////////////////////
    // Special observer mode for the FOURTH screen in a THREE player game
/* Problematic with new player scheme.. what if the fourth player is active??
    if (m_PlayerCount == 3)
    {
        // Update the controller of the observation view
        m_PlayerController[PLAYER_4].Update();

        // Observer user control override
        if (m_PlayerController[PLAYER_4].RelativeCursorMovement(m_ObservationTarget[PLAYER_4], 1.2))
            m_DeathTimer[PLAYER_4].Reset();

        // If no user input in a few seconds, start scrolling along the terrain
        if (m_DeathTimer[PLAYER_4].IsPastSimMS(5000))
        {
            // Make it scroll along
            m_ObservationTarget[PLAYER_4].m_X += 0.5;

            // Make view follow the terrain
            float prevHeight = m_ObservationTarget[PLAYER_4].m_Y;
            m_ObservationTarget[PLAYER_4].m_Y = 0;
            m_ObservationTarget[PLAYER_4].m_Y = prevHeight + ((g_SceneMan.FindAltitude(m_ObservationTarget[PLAYER_4], g_SceneMan.GetSceneHeight(), 20) - prevHeight) * 0.02);
        }

        // Set the view to the observation position
        g_SceneMan.SetScrollTarget(m_ObservationTarget[PLAYER_4], 0.1, g_SceneMan.ForceBounds(m_ObservationTarget[PLAYER_4]), ScreenOfPlayer(PLAYER_4));
    }
*/
    // Check if the demo time has run out, if not registered!
    if (m_ActivityState != DEMOEND && GetDemoTimeLeft() == 0)
    {
// TODO: Write out some good message telling that the demo is over
// ALSO MAKE BRAINS ASSPLODE
        for (int team = 0; team < MAXTEAMCOUNT; ++team)
        {
            if (!m_TeamActive[team])
                continue;
            // Kill all player teams members
            if (team != m_CPUTeam)
            {
                list<Actor *> *pActorList = g_MovableMan.GetTeamRoster(team);
                for (list<Actor *>::iterator itr = pActorList->begin(); itr != pActorList->end(); ++itr)
                {
                    // If brain, make it explode
                    if ((*itr)->HasObjectInGroup("Brains"))
                        (*itr)->GibThis();
                    // Otherwise, just kill the body
                    else
                        (*itr)->SetStatus(Actor::DYING);
                }
            }
        }

        // End the demo!
        End();
        // Override to Demo End for special handling of the demo end
        m_ActivityState = DEMOEND;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DrawGUI
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the currently active GUI of a screen to a BITMAP of choice.

void GameActivity::DrawGUI(BITMAP *pTargetBitmap, const Vector &targetPos, int which)
{
//    SLICK_PROFILE(0xFF789654);

    if (which < 0 || which >= MAXSCREENCOUNT)
        return;

    char str[512];
    int yTextPos = 0;
    int team = NOTEAM;
    int cursor = FRIENDLYCURSOR;
    int PoS = PlayerOfScreen(which);
    if (PoS < 0 || PoS >= Activity::MAXPLAYERCOUNT)
        return;
    Box screenBox(targetPos, pTargetBitmap->w, pTargetBitmap->h);
    GUIFont *pLargeFont = g_FrameMan.GetLargeFont();
    GUIFont *pSmallFont = g_FrameMan.GetSmallFont();
    AllegroBitmap pBitmapInt(pTargetBitmap);
    int frame = ((int)m_CursorTimer.GetElapsedSimTimeMS() % 1000) / 250;
    Vector landZone;

    // Iterate through all players, drawing each currently used LZ cursor.
    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;

        if (m_ViewState[player] == LZSELECT)
        {
            int halfWidth = 36;
            team = m_Team[player];
            if (team == NOTEAM)
                continue;
            cursor = g_SceneMan.GetScreenTeam(which) == team ? FRIENDLYCURSOR : ENEMYCURSOR;
            landZone = m_LandingZone[player] - targetPos;
            // Cursor
            draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X - halfWidth, landZone.m_Y - 48);
            draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
            // Text
            pSmallFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 42, m_AIReturnCraft[player] ? "Deliver here" : "Travel here", GUIFont::Centre);
            pSmallFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 36, "and then", GUIFont::Centre);
            pLargeFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 30, m_AIReturnCraft[player] ? "RETURN" : "STAY", GUIFont::Centre);
            // Draw wrap around the world if necessary, and only if this is being drawn directly to a scenewide target bitmap
            if (targetPos.IsZero() && (landZone.m_X < halfWidth || landZone.m_X > g_SceneMan.GetSceneWidth() - halfWidth))
            {
                // Wrap shit around and draw dupe on the other side
                int wrappedX = landZone.m_X + (landZone.m_X < halfWidth ? g_SceneMan.GetSceneWidth() : -g_SceneMan.GetSceneWidth());
                // Cursor
                draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX - halfWidth, landZone.m_Y - 48);
                draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
                // Text
                pSmallFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 42, m_AIReturnCraft[player] ? "Deliver here" : "Travel here", GUIFont::Centre);
                pSmallFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 36, "and then", GUIFont::Centre);
                pLargeFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 30, m_AIReturnCraft[player] ? "RETURN" : "STAY", GUIFont::Centre);
            }
        }
    }

    // Iterate through all teams, drawing all pending delivery cursors
    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team])
            continue;
        char str[64];
        cursor = g_SceneMan.GetScreenTeam(which) == team ? FRIENDLYCURSOR : ENEMYCURSOR;
        for (deque<Delivery>::iterator itr = m_Deliveries[team].begin(); itr != m_Deliveries[team].end(); ++itr)
        {
            int halfWidth = 24;
            landZone = itr->landingZone - targetPos;
            // Cursor
            draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X - halfWidth, landZone.m_Y - 48);
            draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
            // Text
            pSmallFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 38, "ETA:", GUIFont::Centre);
            if (m_ActivityState == Activity::PREGAME)
                sprintf(str, "???s");
            else
                sprintf(str, "%is", ((int)itr->delay - (int)itr->timer.GetElapsedSimTimeMS()) / 1000);
            pLargeFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 32, str, GUIFont::Centre);
            // Draw wrap around the world if necessary, and only if this is being drawn directly to a scenewide target bitmap
            if (targetPos.IsZero() && (landZone.m_X < halfWidth || landZone.m_X > g_SceneMan.GetSceneWidth() - halfWidth))
            {
                // Wrap shit around and draw dupe on the other side
                int wrappedX = landZone.m_X + (landZone.m_X < halfWidth ? g_SceneMan.GetSceneWidth() : -g_SceneMan.GetSceneWidth());
                // Cursor
                draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX - halfWidth, landZone.m_Y - 48);
                draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
                // Text
                pSmallFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 38, "ETA:", GUIFont::Centre);
                pLargeFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 32, str, GUIFont::Centre);
            }
        }
    }

    // The team of the screen
    team = m_Team[PoS];
    if (team == NOTEAM)
        return;

    // None of the following player-specific GUI elements apply if this isn't a played human actor
    if (!(m_IsActive[PoS] && m_IsHuman[PoS]))
        return;

    // Get all possible wrapped boxes of the screen
    list<Box> wrappedBoxes;
    g_SceneMan.WrapBox(screenBox, wrappedBoxes);
    Vector wrappingOffset, objScenePos, onScreenEdgePos;
    float distance, shortestDist;
    float sceneWidth = g_SceneMan.GetSceneWidth();
    float halfScreenWidth = pTargetBitmap->w / 2;
    float halfScreenHeight = pTargetBitmap->h / 2;
    // THis is the max distance that is possible between a point inside the scene, but outside the screen box, and the screen box's outer edge closest to the point (taking wrapping into account)
    float maxOffScreenSceneWidth = g_SceneMan.SceneWrapsX() ? ((sceneWidth / 2) - halfScreenWidth) : (sceneWidth - pTargetBitmap->w);
    // These handle arranging the left and right stacks of arrows, so they don't pile up on top of each other
    cursor = g_SceneMan.GetScreenTeam(which) == team ? FRIENDLYCURSOR : ENEMYCURSOR;
    float leftStackY = halfScreenHeight - m_aObjCursor[cursor][frame]->h * 2;
    float rightStackY = leftStackY;

    // Draw the objective points this player should care about
    for (list<ObjectivePoint>::iterator itr = m_Objectives.begin(); itr != m_Objectives.end(); ++itr)
    {
        // Only draw objectives of the same team as the current player
        if (itr->m_Team == team)
        {
            // Iterate through the wrapped screen boxes - will only be one if there's no wrapping
            // Try to the find one that contains the objective point
            bool withinAny = false;
            list<Box>::iterator nearestBoxItr = wrappedBoxes.begin();
            shortestDist = 1000000.0;
            for (list<Box>::iterator wItr = wrappedBoxes.begin(); wItr != wrappedBoxes.end(); ++wItr)
            {
                // See if we found the point to be within the screen or not
                if (wItr->WithinBox((*itr).m_ScenePos))
                {
                    nearestBoxItr = wItr;
                    withinAny = true;
                    break;
                }
                // Well, which wrapped screen box is closest to the point?
                distance = g_SceneMan.ShortestDistance(wItr->GetCenter(), (*itr).m_ScenePos).GetLargest();
                if (distance < shortestDist)
                {
                    shortestDist = distance;
                    nearestBoxItr = wItr;
                }
            }

            // Get the difference that the wrapped screen has from the actual one
            wrappingOffset = screenBox.GetCorner() - nearestBoxItr->GetCorner();
            // Apply that offet to the objective point's position
            objScenePos = itr->m_ScenePos + wrappingOffset;

            // Objective is within the screen, so draw the set arrow over it
            if (withinAny)
                itr->Draw(pTargetBitmap, m_aObjCursor[cursor][frame], objScenePos - targetPos, itr->m_ArrowDir);
            // Outside the screen, so draw it at the edge of it
            else
            {
                // Figure out which point is closest to the box, taking scene wrapping into account
                objScenePos = nearestBoxItr->GetCenter() + g_SceneMan.ShortestDistance(nearestBoxItr->GetCenter(), objScenePos);
                // Shortest distance from the edge of the screen box, not the center.
                shortestDist -= halfScreenWidth;

                // Make the arrow point toward the edge of the screen
                if (objScenePos.m_X >= nearestBoxItr->GetCorner().m_X + nearestBoxItr->GetWidth())
                {
                    // Make the edge position approach the center of the vertical edge of the screen the farther away the objective position is from the screen
                    onScreenEdgePos = nearestBoxItr->GetWithinBox(objScenePos) - targetPos;
                    // Double the EaseIn to make it even more exponential, want the arrow to stay close to the edge for a long time
                    onScreenEdgePos.m_Y = rightStackY + EaseIn(0, onScreenEdgePos.m_Y - rightStackY, EaseIn(0, 1.0, 1.0 - (shortestDist / maxOffScreenSceneWidth)));
                    itr->Draw(pTargetBitmap, m_aObjCursor[cursor][frame], onScreenEdgePos, ARROWRIGHT);
                    // Stack cursor moves down an arrowheight
                    rightStackY += m_aObjCursor[cursor][frame]->h;
                }
                else if (objScenePos.m_X < nearestBoxItr->GetCorner().m_X)
                {
                    // Make the edge position approach the center of the vertical edge of the screen the farther away the objective position is from the screen
                    onScreenEdgePos = nearestBoxItr->GetWithinBox(objScenePos) - targetPos;
                    // Double the EaseIn to make it even more exponential, want the arrow to stay close to the edge for a long time
                    onScreenEdgePos.m_Y = leftStackY + EaseIn(0, onScreenEdgePos.m_Y - leftStackY, EaseIn(0, 1.0, 1.0 - (shortestDist / maxOffScreenSceneWidth)));
                    itr->Draw(pTargetBitmap, m_aObjCursor[cursor][frame], onScreenEdgePos, ARROWLEFT);
                    // Stack cursor moves down an arrowheight
                    leftStackY += m_aObjCursor[cursor][frame]->h;
                }
                else if (objScenePos.m_Y < nearestBoxItr->GetCorner().m_Y)
                    itr->Draw(pTargetBitmap, m_aObjCursor[cursor][frame], nearestBoxItr->GetWithinBox(objScenePos) - targetPos, ARROWUP);
                else                        
                    itr->Draw(pTargetBitmap, m_aObjCursor[cursor][frame], nearestBoxItr->GetWithinBox(objScenePos) - targetPos, ARROWDOWN);
            }
        }
    }

    // Team Icon up in the top left corner
    const Icon *pIcon = GetTeamIcon(m_Team[PoS]);
    if (pIcon)
        draw_sprite(pTargetBitmap, pIcon->GetBitmaps8()[0], DMax(2, g_SceneMan.GetScreenOcclusion(which).m_X + 2), 2);
    // Gold
    sprintf(str, "%c Funds: %.0f oz", TeamFundsChanged(which) ? -57 : -58, GetTeamFunds(m_Team[PoS]));
    g_FrameMan.GetLargeFont()->DrawAligned(&pBitmapInt, DMax(16, g_SceneMan.GetScreenOcclusion(which).m_X + 16), yTextPos, str, GUIFont::Left);
/* Not applicable anymore to the 4-team games
    // Body losses
    sprintf(str, "%c Losses: %c%i %c%i", -39, -62, GetTeamDeathCount(Activity::TEAM_1), -59, GetTeamDeathCount(Activity::TEAM_2));
    g_FrameMan.GetLargeFont()->DrawAligned(&pBitmapInt, DMin(pTargetBitmap->w - 4, pTargetBitmap->w - 4 + g_SceneMan.GetScreenOcclusion(which).m_X), yTextPos, str, GUIFont::Right);
*/
    // Show the player's controller scheme icon in the upper right corner of his screen, but only for a minute
    if (m_GameTimer.GetElapsedRealTimeS() < 30)
    {
// TODO: Only blink if there hasn't been any input on a controller since start of game??
        // Blink them at first, but only if there's more than one human player
        if (m_GameTimer.GetElapsedRealTimeS() > 4 || m_GameTimer.AlternateReal(150) || GetHumanCount() < 2)
        {
            pIcon = g_UInputMan.GetSchemeIcon(PoS);
            if (pIcon)
            {
                draw_sprite(pTargetBitmap, pIcon->GetBitmaps8()[0], DMin(pTargetBitmap->w - pIcon->GetBitmaps8()[0]->w - 2, pTargetBitmap->w - pIcon->GetBitmaps8()[0]->w - 2 + g_SceneMan.GetScreenOcclusion(which).m_X), yTextPos);
// TODO: make a black Activity intro screen, saying "Player X, press any key/button to show that you are ready!, and display their controller icon, then fade into the scene"
//                stretch_sprite(pTargetBitmap, pIcon->GetBitmaps8()[0], 10, 10, pIcon->GetBitmaps8()[0]->w * 4, pIcon->GetBitmaps8()[0]->h * 4);
            }
        }
    }

    if (m_ActivityState == RUNNING)
    {
        // Pie menu may be visible if we're choosing actors
        if (/*m_pControlledActor[PoS] && */m_pPieMenu[PoS] && m_pPieMenu[PoS]->IsVisible())
            m_pPieMenu[PoS]->Draw(pTargetBitmap, targetPos);

        if (m_pBuyGUI[PoS] && m_pBuyGUI[PoS]->IsVisible())
            m_pBuyGUI[PoS]->Draw(pTargetBitmap);
    }

    // Draw actor picking crosshairs if applicable
    if (m_ViewState[PoS] == ACTORSELECT && m_IsActive[PoS] && m_IsHuman[PoS])
    {
        Vector center = m_ActorCursor[PoS] - targetPos;
        circle(pTargetBitmap, center.m_X, center.m_Y, m_CursorTimer.AlternateReal(150) ? 6 : 8, g_YellowGlowColor);
        // Add pixel glow area around it, in scene coordinates
        g_SceneMan.RegisterGlowArea(m_ActorCursor[PoS], 10);
/* Crosshairs
        putpixel(pTargetBitmap, center.m_X, center.m_Y, g_YellowGlowColor);
        hline(pTargetBitmap, center.m_X - 5, center.m_Y, center.m_X - 2, g_YellowGlowColor);
        hline(pTargetBitmap, center.m_X + 5, center.m_Y, center.m_X + 2, g_YellowGlowColor);
        vline(pTargetBitmap, center.m_X, center.m_Y - 5, center.m_Y - 2, g_YellowGlowColor);
        vline(pTargetBitmap, center.m_X, center.m_Y + 5, center.m_Y + 2, g_YellowGlowColor);
*/
    }
    // AI point commands cursor
    else if (m_ViewState[PoS] == AIGOTOPOINT)
    {
        Vector center = m_ActorCursor[PoS] - targetPos;
        circle(pTargetBitmap, center.m_X, center.m_Y, m_CursorTimer.AlternateReal(150) ? 6 : 8, g_YellowGlowColor);
        circlefill(pTargetBitmap, center.m_X, center.m_Y, 2, g_YellowGlowColor);
//            putpixel(pTargetBitmap, center.m_X, center.m_Y, g_YellowGlowColor);
        // Add pixel glow area around it, in scene coordinates
        g_SceneMan.RegisterGlowArea(m_ActorCursor[PoS], 10);

        // Draw a line from the last set waypoint to the cursor
        if (m_pControlledActor[PoS] && g_MovableMan.IsActor(m_pControlledActor[PoS]))
            g_FrameMan.DrawLine(pTargetBitmap, m_pControlledActor[PoS]->GetLastAIWaypoint() - targetPos, m_ActorCursor[PoS] - targetPos, g_YellowGlowColor, 0, AILINEDOTSPACING, 0, true);
    }
	// Group selection circle
	else if (m_ViewState[PoS] == UNITSELECTCIRCLE)
    {
		if (m_pControlledActor[PoS] && g_MovableMan.IsActor(m_pControlledActor[PoS]))
		{
			Vector cursorDrawPos = m_ActorCursor[PoS] - targetPos;
			Vector actorPos = m_pControlledActor[PoS]->GetPos();
			Vector drawPos = actorPos - targetPos;

			//Fix cursor coordinates
			if (!targetPos.IsZero())
			{
				// Spans vertical scene seam
				int sceneWidth = g_SceneMan.GetSceneWidth();
				if (g_SceneMan.SceneWrapsX() && pTargetBitmap->w < sceneWidth)
				{
					if ((targetPos.m_X < 0) && (m_ActorCursor[PoS].m_X > (sceneWidth - pTargetBitmap->w)))
						cursorDrawPos.m_X -= sceneWidth;
					else if (((targetPos.m_X + pTargetBitmap->w) > sceneWidth) && (m_ActorCursor[PoS].m_X < pTargetBitmap->w))
						cursorDrawPos.m_X += sceneWidth;
				}
				// Spans horizontal scene seam
				int sceneHeight = g_SceneMan.GetSceneHeight();
				if (g_SceneMan.SceneWrapsY() && pTargetBitmap->h < sceneHeight)
				{
					if ((targetPos.m_Y < 0) && (m_ActorCursor[PoS].m_Y > (sceneHeight - pTargetBitmap->h)))
						cursorDrawPos.m_Y -= sceneHeight;
					else if (((targetPos.m_Y + pTargetBitmap->h) > sceneHeight) && (m_ActorCursor[PoS].m_Y < pTargetBitmap->h))
						cursorDrawPos.m_Y += sceneHeight;
				}
			}


			// Fix circle center coordinates
			if (!targetPos.IsZero())
			{
				// Spans vertical scene seam
				int sceneWidth = g_SceneMan.GetSceneWidth();
				if (g_SceneMan.SceneWrapsX() && pTargetBitmap->w < sceneWidth)
				{
					if ((targetPos.m_X < 0) && (actorPos.m_X > (sceneWidth - pTargetBitmap->w)))
						drawPos.m_X -= sceneWidth;
					else if (((targetPos.m_X + pTargetBitmap->w) > sceneWidth) && (actorPos.m_X < pTargetBitmap->w))
						drawPos.m_X += sceneWidth;
				}
				// Spans horizontal scene seam
				int sceneHeight = g_SceneMan.GetSceneHeight();
				if (g_SceneMan.SceneWrapsY() && pTargetBitmap->h < sceneHeight)
				{
					if ((targetPos.m_Y < 0) && (actorPos.m_Y > (sceneHeight - pTargetBitmap->h)))
						drawPos.m_Y -= sceneHeight;
					else if (((targetPos.m_Y + pTargetBitmap->h) > sceneHeight) && (actorPos.m_Y < pTargetBitmap->h))
						drawPos.m_Y += sceneHeight;
				}
			}

			float radius = g_SceneMan.ShortestDistance(m_ActorCursor[PoS], m_pControlledActor[PoS]->GetPos(), true).GetMagnitude();

			circle(pTargetBitmap, cursorDrawPos.m_X, cursorDrawPos.m_Y, m_CursorTimer.AlternateReal(150) ? 6 : 8, g_YellowGlowColor);
			circlefill(pTargetBitmap, cursorDrawPos.m_X, cursorDrawPos.m_Y, 2, g_YellowGlowColor);

			Vector unwrappedPos;

			//Check if we crossed the seam
			if (g_SceneMan.GetScene()->WrapsX())
			{
				//Calculate unwrapped cursor position, or it won't glow
				unwrappedPos = m_ActorCursor[PoS] - m_pControlledActor[PoS]->GetPos();
				float sceneWidth = g_SceneMan.GetSceneWidth();
				
				if (unwrappedPos.GetMagnitude() > sceneWidth / 2 && unwrappedPos.GetMagnitude() > 350)
				{
					if (m_ActorCursor->m_X < sceneWidth / 2)
						unwrappedPos = m_ActorCursor[PoS] + Vector(sceneWidth , 0);
					else
						unwrappedPos = m_ActorCursor[PoS] - Vector(sceneWidth , 0);
					g_SceneMan.RegisterGlowArea(unwrappedPos, 10);
				}
			}
			else
				unwrappedPos = m_ActorCursor[PoS];

			g_SceneMan.RegisterGlowArea(m_ActorCursor[PoS], 10);

			//Glowing dotted circle version
			int dots = 2 * PI * radius / 25;//5 + (int)(radius / 10);
			float radsperdot = 2 * 3.14159265359 / dots;

			for (int i = 0; i < dots; i++)
			{
				Vector dotPos = Vector(actorPos.m_X + sin(i * radsperdot) * radius, actorPos.m_Y + cos(i * radsperdot) * radius);
				Vector dotDrawPos = dotPos - targetPos;

				if (!targetPos.IsZero())
				{
					// Spans vertical scene seam
					if (g_SceneMan.SceneWrapsX() && pTargetBitmap->w < sceneWidth)
					{
						if ((targetPos.m_X < 0) && (dotPos.m_X > (sceneWidth - pTargetBitmap->w)))
						{
							dotDrawPos.m_X -= sceneWidth;
						}
						else if (((targetPos.m_X + pTargetBitmap->w) > sceneWidth) && (actorPos.m_X < pTargetBitmap->w))
						{
							dotDrawPos.m_X += sceneWidth;
						}
					}
					// Spans horizontal scene seam
					int sceneHeight = g_SceneMan.GetSceneHeight();
					if (g_SceneMan.SceneWrapsY() && pTargetBitmap->h < sceneHeight)
					{
						if ((targetPos.m_Y < 0) && (dotPos.m_Y > (sceneHeight - pTargetBitmap->h)))
						{
							dotDrawPos.m_Y -= sceneHeight;
						}
						else if (((targetPos.m_Y + pTargetBitmap->h) > sceneHeight) && (actorPos.m_Y < pTargetBitmap->h))
						{
							dotDrawPos.m_Y += sceneHeight;
						}
					}

					circlefill(pTargetBitmap, dotDrawPos.m_X, dotDrawPos.m_Y, 1, g_YellowGlowColor);
					g_SceneMan.RegisterGlowArea(dotPos, 3);
				}
			}
		}
		else
		{
			// Cancel squad selection

		}
	}

    if ((m_ActivityState == EDITING || m_ActivityState == PREGAME) && m_pEditorGUI[PoS])
        m_pEditorGUI[PoS]->Draw(pTargetBitmap, targetPos);

    // Draw Banners
    m_pBannerRed[PoS]->Draw(pTargetBitmap);
    m_pBannerYellow[PoS]->Draw(pTargetBitmap);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this GameActivity's current graphical representation to a
//                  BITMAP of choice. This includes all game-related graphics.

void GameActivity::Draw(BITMAP *pTargetBitmap, const Vector &targetPos)
{
    SLICK_PROFILE(0xFF665432);

    GUIFont *pLargeFont = g_FrameMan.GetLargeFont();
    GUIFont *pSmallFont = g_FrameMan.GetSmallFont();
    AllegroBitmap pBitmapInt(pTargetBitmap);
    int frame = ((int)m_CursorTimer.GetElapsedSimTimeMS() % 1000) / 250;
    int cursor = FRIENDLYCURSOR;//= g_SceneMan.GetScreenTeam(which) == team ? FRIENDLYCURSOR : ENEMYCURSOR;
    Vector landZone;

    // Iterate through all players, drawing each currently used LZ cursor.
    for (int player = 0; player < MAXPLAYERCOUNT; ++player)
    {
        if (!(m_IsActive[player] && m_IsHuman[player]))
            continue;

        if (m_ViewState[player] == LZSELECT)
        {
            int halfWidth = 36;
            int team = m_Team[player];
            if (team == NOTEAM)
                continue;
            landZone = m_LandingZone[player] - targetPos;
            // Cursor
            draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X - halfWidth, landZone.m_Y - 48);
            draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
            // Text
            pSmallFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 42, m_AIReturnCraft[player] ? "Deliver here" : "Travel here", GUIFont::Centre);
            pSmallFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 36, "and then", GUIFont::Centre);
            pLargeFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 30, m_AIReturnCraft[player] ? "RETURN" : "STAY", GUIFont::Centre);
            // Draw wrap around the world if necessary, and only if this is being drawn directly to a scenewide target bitmap
            if (targetPos.IsZero() && (landZone.m_X < halfWidth || landZone.m_X > g_SceneMan.GetSceneWidth() - halfWidth))
            {
                // Wrap shit around and draw dupe on the other side
                int wrappedX = landZone.m_X + (landZone.m_X < halfWidth ? g_SceneMan.GetSceneWidth() : -g_SceneMan.GetSceneWidth());
                // Cursor
                draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX - halfWidth, landZone.m_Y - 48);
                draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
                // Text
                pSmallFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 42, m_AIReturnCraft[player] ? "Deliver here" : "Travel here", GUIFont::Centre);
                pSmallFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 36, "and then", GUIFont::Centre);
                pLargeFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 30, m_AIReturnCraft[player] ? "RETURN" : "STAY", GUIFont::Centre);
            }
        }
    }

    // Iterate through all teams, drawing all pending delivery cursors
    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team])
            continue;
        char str[64];
        for (deque<Delivery>::iterator itr = m_Deliveries[team].begin(); itr != m_Deliveries[team].end(); ++itr)
        {
            int halfWidth = 24;
            landZone = itr->landingZone - targetPos;
            // Cursor
            draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X - halfWidth, landZone.m_Y - 48);
            draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], landZone.m_X + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
            // Text
            pSmallFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 38, "ETA:", GUIFont::Centre);
            if (m_ActivityState == Activity::PREGAME)
                sprintf(str, "???s");
            else
                sprintf(str, "%is", ((int)itr->delay - (int)itr->timer.GetElapsedSimTimeMS()) / 1000);
            pLargeFont->DrawAligned(&pBitmapInt, landZone.m_X, landZone.m_Y - 32, str, GUIFont::Centre);
            // Draw wrap around the world if necessary, and only if this is being drawn directly to a scenewide target bitmap
            if (targetPos.IsZero() && (landZone.m_X < halfWidth || landZone.m_X > g_SceneMan.GetSceneWidth() - halfWidth))
            {
                // Wrap shit around and draw dupe on the other side
                int wrappedX = landZone.m_X + (landZone.m_X < halfWidth ? g_SceneMan.GetSceneWidth() : -g_SceneMan.GetSceneWidth());
                // Cursor
                draw_sprite(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX - halfWidth, landZone.m_Y - 48);
                draw_sprite_h_flip(pTargetBitmap, m_aLZCursor[cursor][frame], wrappedX + halfWidth - m_aLZCursor[cursor][frame]->w, landZone.m_Y - 48);
                // Text
                pSmallFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 38, "ETA:", GUIFont::Centre);
                pLargeFont->DrawAligned(&pBitmapInt, wrappedX, landZone.m_Y - 32, str, GUIFont::Centre);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetActiveCPUTeamCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns active CPU team count.
// Arguments:       None.
// Return value:    Returns active CPU team count.

int GameActivity::GetActiveCPUTeamCount() const
{
	int count = 0;

	for (int team = Activity::TEAM_1; team < Activity::MAXTEAMCOUNT; team++)
		if (TeamActive(team) && TeamIsCPU(team))
			count++;

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetActiveHumanTeamCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns active human team count.
// Arguments:       None.
// Return value:    Returns active human team count.

int GameActivity::GetActiveHumanTeamCount() const
{
	int count = 0;

	for (int team = Activity::TEAM_1; team < Activity::MAXTEAMCOUNT; team++)
		if (TeamActive(team) && !TeamIsCPU(team))
			count++;

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OtherTeam
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the next other team number from the one passed in, if any. If there
//                  are more than two teams in this game, then the next one in the series
//                  will be returned here.

int GameActivity::OtherTeam(int team)
{
    // Only one team in this game, so can't return another one
    if (m_TeamCount == 1)
        return Activity::NOTEAM;

    // Find another team that is active
    bool loopedOnce = false;
    for (int t = team + 1; ; t++)
    {
        // Loop
        if (t >= MAXTEAMCOUNT)
            t = Activity::TEAM_1;

        if (t == team)
            break;

        if (m_TeamActive[t])
            return t;
    }

    return Activity::NOTEAM;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OneOrNoneTeamsLeft
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether there is one and only one team left this game with
//                  a brain in its ranks.

bool GameActivity::OneOrNoneTeamsLeft()
{
    // See if only one team remains with any brains
    int brainTeamCount = 0;
    int brainTeam = Activity::NOTEAM;
    for (int t = Activity::TEAM_1; t < MAXTEAMCOUNT; ++t)
    {
        if (!m_TeamActive[t])
            continue;
        if (g_MovableMan.GetFirstBrainActor(t))
        {
            brainTeamCount++;
            brainTeam = t;
        }
    }

    // If less than two teams left with any brains, they get indicated
    // Also, if NO teams with brain are left, that is indicated with NOTEAM
    if (brainTeamCount <= 1)
        return true;

    return false;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WhichTeamLeft
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates which single team is left, if any.
// Arguments:       None.

int GameActivity::WhichTeamLeft()
{
    int whichTeam = Activity::NOTEAM;

    // See if only one team remains with any brains
    int brainTeamCount = 0;
    int brainTeam = Activity::NOTEAM;
    for (int t = Activity::TEAM_1; t < MAXTEAMCOUNT; ++t)
    {
        if (!m_TeamActive[t])
            continue;
        if (g_MovableMan.GetFirstBrainActor(t))
        {
            brainTeamCount++;
            brainTeam = t;
        }
    }

    // If exactly one team with brains, return that
    if (brainTeamCount == 1)
        return brainTeam;

    return Activity::NOTEAM;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          NoTeamLeft
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether there are NO teams left with any brains at all!

bool GameActivity::NoTeamLeft()
{
    for (int t = Activity::TEAM_1; t < MAXTEAMCOUNT; ++t)
    {
        if (!m_TeamActive[t])
            continue;
        if (g_MovableMan.GetFirstBrainActor(t))
            return false;
    }
    return true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  InitAIs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Goes through all Actor:s currently in the MovableMan and gives each
//                  one not controlled by a Controller a CAI and appropriate AIMode setting
//                  based on team and CPU team.

void GameActivity::InitAIs()
{
    Actor *pActor = 0;
    Actor *pFirstActor = 0;

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team])
            continue;
        // Get the first one
        pFirstActor = pActor = g_MovableMan.GetNextTeamActor(team);

        do
        {
            // Set up AI controller if currently not player controlled
            if (pActor && !pActor->GetController()->IsPlayerControlled())
            {
                pActor->SetControllerMode(Controller::CIM_AI);

                // If human, set appropriate AI mode
// TODO: IMPROVE
                if (dynamic_cast<AHuman *>(pActor) || dynamic_cast<ACrab *>(pActor))
                {
                    // Hunt if of CPU team, sentry default if of player team
                    if (team == m_CPUTeam)
                        pActor->SetAIMode(AHuman::AIMODE_BRAINHUNT);
                    else
                        pActor->SetAIMode(AHuman::AIMODE_SENTRY);
                }
            }

            // Next!
            pActor = g_MovableMan.GetNextTeamActor(team, pActor);
        }
        while (pActor && pActor != pFirstActor);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DisableAIs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Goes through all Actor:s currently in the MovableMan and gives each
//                  one not controlled by a Controller a CAI and appropriate AIMode setting
//                  based on team and CPU team.

void GameActivity::DisableAIs(bool disable, int whichTeam)
{
    Actor *pActor = 0;
    Actor *pFirstActor = 0;

    for (int team = 0; team < MAXTEAMCOUNT; ++team)
    {
        if (!m_TeamActive[team] || (whichTeam != Activity::NOTEAM && team != whichTeam))
            continue;
        // Get the first one
        pFirstActor = pActor = g_MovableMan.GetNextTeamActor(team);

        do
        {
            // Disable the AI controllers
            if (pActor && pActor->GetController()->GetInputMode() == Controller::CIM_AI)
                pActor->GetController()->SetDisabled(disable);

            // Next!
            pActor = g_MovableMan.GetNextTeamActor(team, pActor);
        }
        while (pActor && pActor != pFirstActor);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Simply draws this' arrow relative to a point on a bitmap.

void GameActivity::ObjectivePoint::Draw(BITMAP *pTargetBitmap, BITMAP *pArrowBitmap, const Vector &arrowPoint, ObjectiveArrowDir arrowDir)
{
    if (!pTargetBitmap || !pArrowBitmap)
        return;

    AllegroBitmap allegroBitmap(pTargetBitmap);
    int x = arrowPoint.GetFloorIntX();
    int y = arrowPoint.GetFloorIntY();
    int halfWidth = pArrowBitmap->w / 2;
    int halfHeight = pArrowBitmap->h / 2;
    int textSpace = 4;

    // Constrain the point within a gutter of the whole screen so the arrow is never right up agains teh edge of the screen.
/*
    Box constrainBox(halfWidth, pArrowBitmap->h, pTargetBitmap->w - halfWidth, pTargetBitmap->h - pArrowBitmap->h);
    x = constrainBox.GetWithinBoxX(x);
    y = constrainBox.GetWithinBoxY(y);
*/
    if (x < halfWidth || x > pTargetBitmap->w - halfWidth)
    {
        if (x < halfWidth)
            x = halfWidth;
        if (x > pTargetBitmap->w - halfWidth)
            x = pTargetBitmap->w - halfWidth - 1.0;

        if (y > pTargetBitmap->h - pArrowBitmap->h)
            y = pTargetBitmap->h - pArrowBitmap->h;
        else if (y < pArrowBitmap->h)
            y = pArrowBitmap->h;
    }
    else if (y < halfHeight || y > pTargetBitmap->h - halfHeight)
    {
        if (y < halfHeight)
            y = halfHeight;
        if (y > pTargetBitmap->h - halfHeight)
            y = pTargetBitmap->h - halfHeight - 1.0;

        if (x > pTargetBitmap->w - pArrowBitmap->w)
            x = pTargetBitmap->w - pArrowBitmap->w;
        else if (x < pArrowBitmap->w)
            x = pArrowBitmap->w;
    }

    // Draw the arrow and text descritpion of the Object so the point of the arrow ends up on the arrowPoint
    if (arrowDir == ARROWDOWN)
    {
        masked_blit(pArrowBitmap, pTargetBitmap, 0, 0, x - halfWidth, y - pArrowBitmap->h, pArrowBitmap->w, pArrowBitmap->h);
        g_FrameMan.GetLargeFont()->DrawAligned(&allegroBitmap, x, y - pArrowBitmap->h - textSpace, m_Description, GUIFont::Centre, GUIFont::Bottom);
    }
    else if (arrowDir == ARROWLEFT)
    {
        rotate_sprite(pTargetBitmap, pArrowBitmap, x, y - halfHeight, itofix(64));
        g_FrameMan.GetLargeFont()->DrawAligned(&allegroBitmap, x + pArrowBitmap->w + textSpace, y, m_Description, GUIFont::Left, GUIFont::Middle);
    }
    else if (arrowDir == ARROWRIGHT)
    {
        rotate_sprite(pTargetBitmap, pArrowBitmap, x - pArrowBitmap->w, y - halfHeight, itofix(-64));
        g_FrameMan.GetLargeFont()->DrawAligned(&allegroBitmap, x - pArrowBitmap->w - textSpace, y, m_Description, GUIFont::Right, GUIFont::Middle);
    }
    else if (arrowDir == ARROWUP)
    {
        rotate_sprite(pTargetBitmap, pArrowBitmap, x - halfWidth, y, itofix(-128));
        g_FrameMan.GetLargeFont()->DrawAligned(&allegroBitmap, x, y + pArrowBitmap->h + textSpace, m_Description, GUIFont::Centre, GUIFont::Top);
    }
}

std::string & GameActivity::GetNetworkPlayerName(int player)
{
	if (player >= 0 && player < MAXPLAYERCOUNT)
		return m_NetworkPlayerNames[player];
	else
		return m_NetworkPlayerNames[0];
}

void GameActivity::SetNetworkPlayerName(int player, std::string name)
{
	if (player >= 0 && player < MAXPLAYERCOUNT)
		m_NetworkPlayerNames[player] = name;
}

} // namespace RTE