//////////////////////////////////////////////////////////////////////////////////////////
// File:            MainMenuGUI.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Implementation file for the MainMenuGUI class
// Project:         GUI Library
// Author(s):       Daniel Tabar
//                  dtabar@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include <sol/sol.hpp>

#include "MainMenuGUI.h"

#include "FrameMan.h"
#include "PresetMan.h"
#include "ActivityMan.h"
#include "GameActivity.h"
#include "AudioMan.h"
#include "UInputMan.h"
#include "SettingsMan.h"
#include "LicenseMan.h"
#include "ConsoleMan.h"
#include "MetaMan.h"
#ifdef STEAM_BUILD
#include "SteamUGCMan.h"
#endif
#include "GlobalScript.h"

#include "GUI/GUI.h"
#include "GUI/AllegroBitmap.h"
#include "GUI/AllegroScreen.h"
#include "GUI/AllegroInput.h"
#include "GUI/GUIControlManager.h"
#include "GUI/GUICollectionBox.h"
#include "GUI/GUIComboBox.h"
#include "GUI/GUITab.h"
#include "GUI/GUIListBox.h"
#include "GUI/GUITextBox.h"
#include "GUI/GUIButton.h"
#include "GUI/GUILabel.h"
#include "GUI/GUISlider.h"
#include "GUI/GUICheckbox.h"

#include "Controller.h"
#include "Entity.h"
#include "MOSprite.h"
#include "HeldDevice.h"
#include "AHuman.h"
#include "DataModule.h"
#include "GABrainMatch.h"
#include "GABaseDefense.h"
#include "GATutorial.h"
#include "SceneEditor.h"
#include "AreaEditor.h"
#include "GibEditor.h"
#include "ActorEditor.h"
#include "AssemblyEditor.h"
#include "MultiplayerGame.h"

extern int g_IntroState;

using namespace std;
using namespace RTE;

#define NAGSCREENCOUNT 7
#define NAGDURATIONMS 14000

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this MainMenuGUI, effectively
//                  resetting the members of this abstraction level only.

void MainMenuGUI::Clear()
{
    m_pController = 0;
    m_pGUIScreen = 0;
    m_pGUIInput = 0;
    m_pGUIController = 0;
    m_MenuEnabled = ENABLED;
    m_MenuScreen = MAINSCREEN;
    m_ScreenChange = false;
    m_MainMenuFocus = CAMPAIGN;
    m_FocusChange = 0;
    m_MenuSpeed = 0.3;
    m_ListItemIndex = 0;
    m_BlinkTimer.Reset();
    m_BlinkMode = NOBLINK;
    for (int screen = 0; screen < SCREENCOUNT; ++screen)
        m_apScreenBox[screen] = 0;
    for (int button = 0; button < MAINMENUBUTTONCOUNT; ++button)
        m_aMainMenuButton[button] = 0;
    m_pTeamBox = 0;
    m_pSceneSelector = 0;
    for (int box = 0; box < SKIRMISHPLAYERCOUNT; ++box)
        m_aSkirmishBox[box] = 0;
    for (int button = 0; button < SKIRMISHPLAYERCOUNT; ++button)
        m_aSkirmishButton[button] = 0;
    m_pCPUTeamLabel = 0;
    for (int button = 0; button < OPTIONSBUTTONCOUNT; ++button)
        m_aOptionButton[button] = 0;
    for (int label = 0; label < OPTIONSLABELCOUNT; ++label)
        m_aOptionsLabel[label] = 0;
    for (int checkbox = 0; checkbox < OPTIONSCHECKBOXCOUNT; ++checkbox)
        m_aOptionsCheckbox[checkbox] = 0;
    m_pResolutionCombo = 0;
    m_pResolutionNoticeLabel = 0;
    m_pSoundLabel = 0;
    m_pMusicLabel = 0;
    m_pSoundSlider = 0;
    m_pMusicSlider = 0;
    m_pEditorPanel = 0;
    m_pScrollPanel = 0;
    m_ScrollTimer.Reset();
    m_ScenarioStarted = false;
    m_CampaignStarted = false;
    m_ActivityRestarted = false;
    m_ActivityResumed = false;
    m_TutorialOffered = false;
    m_StartPlayers = 1;
    m_StartTeams = 2;
    m_StartFunds = 1600;
    for (int player = 0; player < SKIRMISHPLAYERCOUNT; ++player)
        m_aTeamAssignments[player] = Activity::TEAM_1;
    m_CPUTeam = -1;
    m_StartDifficulty = GameActivity::MEDIUMDIFFICULTY;
    m_Quit = false;

    // Config screen
    m_ConfiguringPlayer = UInputMan::PLAYER_ONE;
    m_ConfiguringDevice = UInputMan::DEVICE_KEYB_ONLY;
    m_ConfiguringGamepad = DPAD;
    m_ConfigureStep = 0;
    for (int label = 0; label < CONFIGLABELCOUNT; ++label)
        m_pConfigLabel[label] = 0;

    // Editor screen
    for (int button = 0; button < EDITORBUTTONCOUNT; ++button)
        m_aEditorButton[button] = 0;

    m_pLicenseCaptionLabel = 0;
    m_pLicenseInstructionLabel = 0;
    m_pLicenseEmailLabel = 0;
    m_pLicenseKeyLabel = 0;
    m_pLicenseEmailBox = 0;
    m_pLicenseKeyBox = 0;
    m_pRegistrationButton = 0;

    m_NagMode = QUITNAG;
    m_pNagExitButton = 0;
    m_pNagRegButton = 0;
    m_apNagSlides = 0;
    m_NagTimer.Reset();

    m_aDPadBitmaps = 0;
    m_aDualAnalogBitmaps = 0;
    m_pRecommendationBox = 0;
    m_pRecommendationDiagram = 0;
    m_pConfigSkipButton = 0;
    m_pConfigBackButton = 0;
    m_pDPadTypeBox = 0;
    m_pDAnalogTypeBox = 0;
    m_pXBox360TypeBox = 0;
    m_pDPadTypeDiagram = 0;
    m_pDAnalogTypeDiagram = 0;
    m_pXBox360TypeDiagram = 0;
    m_pDPadTypeButton = 0;
    m_pDAnalogTypeButton = 0;
    m_pXBox360TypeButton = 0;

    m_pPublishedListHeader = 0;
    m_pMainMenuButton = 0;
    m_pPublishNewButton = 0;
    m_pPublishedList = 0;
    m_pEditUpdateButton = 0;
    m_pPublishTitleText = 0;

    for (int step = 0; step < PUBLISHSTEPCOUNT; ++step)
        m_apPublishStepsControls[step] = 0;

    m_pPublishStepsHeader = 0;
    m_PublishStep = PUBLISHSTEPFIRST;
    m_UpdatingAlreadyPublished = false;
    m_PublishingComplete = false;
    m_PublishedAppIDString = "";
    m_pUnpublishedList = 0;
    m_pPreviewNoticeLabel = 0;
    m_pPublishBackButton = 0;
    m_pPublishNextButton = 0;
    m_pPublishCancelButton = 0;
    m_pLegalAgreementLabel = 0;
    m_pLegalAgreementLink = 0;
    m_pPublishTitleText = 0;
    m_pPublishDescText = 0;
    m_pPublishStatus = 0;
    m_pAppLinkButton = 0;
    m_PublishLogWriter.Reset();

    m_pPublishConfirmBox = 0;
    m_pPublishToolTipBox = 0;

    for (int tag = 0; tag < PUBLISHTAGCOUNT; ++tag)
        m_apPublishTags[tag] = 0;


    m_pModManagerBackButton = 0;

	m_MaxResX = 0;
	m_MaxResY = 0;

    // Sounds
    m_EnterMenuSound.Reset();
    m_ExitMenuSound.Reset();
    m_ExitMenuAltSound.Reset();
    m_FocusChangeSound.Reset();
    m_SelectionChangeSound.Reset();
    m_ButtonPressSound.Reset();
    m_BackButtonPressSound.Reset();
    m_ItemChangeSound.Reset();
    m_TestSound.Reset();
    m_UserErrorSound.Reset();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the MainMenuGUI object ready for use.

int MainMenuGUI::Create(Controller *pController)
{
    AAssert(pController, "No controller sent to MainMenuGUI on creation!");
    m_pController = pController;

    if (!m_pGUIScreen)
        m_pGUIScreen = new AllegroScreen(g_FrameMan.GetBackBuffer32());
    if (!m_pGUIInput)
        m_pGUIInput = new AllegroInput(-1, true); 
    if (!m_pGUIController)
        m_pGUIController = new GUIControlManager();
    if(!m_pGUIController->Create(m_pGUIScreen, m_pGUIInput, "Base.rte/GUIs/Skins/MainMenu"))
        DDTAbort("Failed to create GUI Control Manager and load it from Base.rte/GUIs/Skins/MainMenu");
    m_pGUIController->Load("Base.rte/GUIs/MainMenuGUI.ini");

    // Make sure we have convenient points to the containing GUI colleciton boxes that we will manipulate the positions of
    GUICollectionBox *pRootBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("root"));
    m_apScreenBox[MAINSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("MainScreen"));
    m_apScreenBox[PLAYERSSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PlayersScreen"));
    m_apScreenBox[SKIRMISHSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("SkirmishScreen"));
    m_apScreenBox[DIFFICULTYSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("DifficultyScreen"));
    m_apScreenBox[OPTIONSSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("OptionsScreen"));
    m_apScreenBox[CONFIGSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("ConfigScreen"));
    m_apScreenBox[LICENSESCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("LicenseScreen"));
    m_apScreenBox[EDITORSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("EditorScreen"));
    m_apScreenBox[METASCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("MetaScreen"));
    m_apScreenBox[CREDITSSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("CreditsScreen"));
    m_apScreenBox[NAGSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("LicenseNagScreen"));
    m_apScreenBox[QUITSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("QuitConfirmBox"));
    m_apScreenBox[MODMANAGERSCREEN] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("ModManagerScreen"));

    pRootBox->SetPositionAbs((g_FrameMan.GetResX() - pRootBox->GetWidth()) / 2, 0);// (g_FrameMan.GetResY() - pRootBox->GetHeight()) / 2);
// NO, this screws up the menu positioning!
//    pRootBox->Resize(pRootBox->GetWidth(), g_FrameMan.GetResY());


    // Also load into the steam workshop gui elements, keeping the main menu ones we just loaded above
    m_pGUIController->Load("Base.rte/GUIs/WorkshopGUI.ini", true);
    // Get handles to the workshop-related dialog box screens
    m_apScreenBox[WORKSHOPMANAGER] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishedListDialog"));
    m_apScreenBox[PUBLISHSTEPS] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishStepsDialog"));
    m_apScreenBox[WORKSHOPMANAGER]->CenterInParent(true, true);
    m_apScreenBox[PUBLISHSTEPS]->CenterInParent(true, true);

    // Attach the combo boxes with the publsihing steps to the proper dialog; they are first children of the root for layout purposes
    m_apPublishStepsControls[PUBLISHSTEPFIRST] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishStep1Box"));
    m_apPublishStepsControls[PUBLISHSTEPDESCRIPTION] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishStep2Box"));
    m_apPublishStepsControls[PUBLISHSTEPTAGS] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishStep3Box"));
    m_apPublishStepsControls[PUBLISHSTEPAGREEMENT] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishStep4Box"));
    m_apPublishStepsControls[PUBLISHSTEPLAST] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PublishStep5Box"));
    m_PublishLogWriter.Create("LogPublish.txt");
    // Set up all the step controls
    for (int step = PUBLISHSTEPDESCRIPTION; step < PUBLISHSTEPCOUNT; ++step)
    {
        // First remove any association with the root combobox since they are just placed there in the editor to make it easier to tweak them
        pRootBox->GUIPanel::RemoveChild(m_apPublishStepsControls[step]);
        pRootBox->GUIControl::RemoveChild(m_apPublishStepsControls[step]->GetName());
        // Now add the controls onto the proper publishing dialog box instead of root
        m_apScreenBox[PUBLISHSTEPS]->GUIPanel::AddChild(m_apPublishStepsControls[step], false);
        m_apScreenBox[PUBLISHSTEPS]->GUIControl::AddChild(m_apPublishStepsControls[step]);
        m_apPublishStepsControls[step]->SetPositionRel(5, 30);
        m_apPublishStepsControls[step]->SetVisible(false);
    }

    m_pPublishedListHeader = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("PublishedListHeader"));
    m_pMainMenuButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("PublishMainMenuButton"));
    m_pPublishNewButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("PublishNewDMButton"));
    m_pPublishedList = dynamic_cast<GUIComboBox *>(m_pGUIController->GetControl("PublishedList"));
    m_pEditUpdateButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("UpdateDMButton"));

    m_pPublishStepsHeader = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("PublishStepHeader"));
    m_pUnpublishedList = dynamic_cast<GUIComboBox *>(m_pGUIController->GetControl("UnpublishedCombo"));
    m_pPreviewNoticeLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("PreviewNoticeLabel"));
    m_pPublishBackButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("PrevPublishStepButton"));
    m_pPublishNextButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("NextPublishStepButton"));
    m_pPublishCancelButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("CancelPublishButton"));
    m_pLegalAgreementLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LegalAgreementLabel"));
    m_pLegalAgreementLink = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("LegalAgreementButton"));
    m_pPublishTitleText = dynamic_cast<GUITextBox *>(m_pGUIController->GetControl("DMTitleBox"));
    m_pPublishDescText = dynamic_cast<GUITextBox *>(m_pGUIController->GetControl("DMDescriptionBox"));
    m_apPublishTags[PUBLISHTAGACTORS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagActorCheck"));
    m_apPublishTags[PUBLISHTAGWEAPONS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagWeaponCheck"));
    m_apPublishTags[PUBLISHTAGTOOLS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagToolCheck"));
    m_apPublishTags[PUBLISHTAGCRAFT] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagCraftCheck"));
    m_apPublishTags[PUBLISHTAGBUNKER] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagBunkerCheck"));
    m_apPublishTags[PUBLISHTAGSCENES] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagSceneCheck"));
    m_apPublishTags[PUBLISHTACTIVITIES] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagActivityCheck"));
    m_apPublishTags[PUBLISHTAGTECH] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagTechCheck"));
    m_apPublishTags[PUBLISHTAGVANILLA] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagVanillaCheck"));
    m_apPublishTags[PUBLISHTAGFRIENDS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("DMTagFriendsCheck"));
// TODO: Add the two other tags
    m_pPublishStatus = dynamic_cast<GUIListBox *>(m_pGUIController->GetControl("PublishStatusList"));
    m_pAppLinkButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("AppLinkButton"));

    m_pPublishConfirmBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("ConfirmDialog"));
    m_pPublishToolTipBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("ToolTipBox"));
    m_pPublishConfirmBox->SetVisible(false);
    m_pPublishToolTipBox->SetVisible(false);

    // Set the font of the publish status display
    m_pPublishStatus->SetFont(m_pGUIController->GetSkin()->GetFont("smallfont.bmp"));
    m_pPublishStatus->EnableScrollbars(false, true);

    // Set up screens' initial positions and visibility
    m_apScreenBox[QUITSCREEN]->CenterInParent(true, true);
    // Hide all screens, the appropriate screen will reappear on next update
    HideAllScreens();

    // Panel behind editor menu to be resized depending on which editors are available
    m_pEditorPanel = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("EditorPanel"));
    // Credits scrolling panel
    m_pScrollPanel = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("CreditsPanel"));

    m_aMainMenuButton[CAMPAIGN] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToCampaign"));
    m_aMainMenuButton[SKIRMISH] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToSkirmish"));
	m_aMainMenuButton[MULTIPLAYER] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToMultiplayer"));
	m_aMainMenuButton[OPTIONS] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToOptions"));
    m_aMainMenuButton[MODMANAGER] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToModManager"));
    m_aMainMenuButton[EDITOR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToEditor"));
    m_aMainMenuButton[CREDITS] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonMainToCreds"));
    m_aMainMenuButton[QUIT] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonQuit"));
    m_aMainMenuButton[RESUME] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonResume"));
	m_aMainMenuButton[REGISTER] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonRegister"));
    m_aMainMenuButton[PLAYTUTORIAL] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonTutorial"));
    m_aMainMenuButton[METACONTINUE] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonContinue"));
    m_aMainMenuButton[BACKTOMAIN] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonBackToMain"));
    m_aMainMenuButton[QUITCONFIRM] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("QuitConfirmButton"));
    m_aMainMenuButton[QUITCANCEL] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("QuitCancelButton"));
    m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
    m_aMainMenuButton[PLAYTUTORIAL]->SetVisible(false);
    m_aMainMenuButton[METACONTINUE]->SetVisible(false);

    m_pSceneSelector = dynamic_cast<GUIComboBox *>(m_pGUIController->GetControl("ComboScene"));
    m_pTeamBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PanelTeams"));
    m_aSkirmishBox[P1TEAM] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PanelP1Team"));
    m_aSkirmishBox[P2TEAM] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PanelP2Team"));
    m_aSkirmishBox[P3TEAM] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PanelP3Team"));
    m_aSkirmishBox[P4TEAM] = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("PanelP4Team"));
    m_aSkirmishButton[P1TEAM] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP1Team"));
    m_aSkirmishButton[P2TEAM] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP2Team"));
    m_aSkirmishButton[P3TEAM] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP3Team"));
    m_aSkirmishButton[P4TEAM] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP4Team"));
    m_pCPUTeamLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelCPUTeam"));

    m_aOptionButton[FULLSCREEN] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonFullscreen"));
    m_aOptionButton[P1NEXT] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP1NextDevice"));
    m_aOptionButton[P2NEXT] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP2NextDevice"));
    m_aOptionButton[P3NEXT] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP3NextDevice"));
    m_aOptionButton[P4NEXT] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP4NextDevice"));
    m_aOptionButton[P1PREV] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP1PrevDevice"));
    m_aOptionButton[P2PREV] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP2PrevDevice"));
    m_aOptionButton[P3PREV] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP3PrevDevice"));
    m_aOptionButton[P4PREV] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP4PrevDevice"));
    m_aOptionButton[P1CONFIG] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP1Config"));
    m_aOptionButton[P2CONFIG] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP2Config"));
    m_aOptionButton[P3CONFIG] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP3Config"));
    m_aOptionButton[P4CONFIG] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP4Config"));
    m_aOptionButton[P1CLEAR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP1Clear"));
    m_aOptionButton[P2CLEAR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP2Clear"));
    m_aOptionButton[P3CLEAR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP3Clear"));
    m_aOptionButton[P4CLEAR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonP4Clear"));

	m_aOptionsLabel[P1DEVICE] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP1Device"));
    m_aOptionsLabel[P2DEVICE] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP2Device"));
    m_aOptionsLabel[P3DEVICE] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP3Device"));
    m_aOptionsLabel[P4DEVICE] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP4Device"));

	m_aDeadZoneLabel[P1DEADZONESLIDER] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP1DeadZoneValue"));
	m_aDeadZoneLabel[P2DEADZONESLIDER] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP2DeadZoneValue"));
	m_aDeadZoneLabel[P3DEADZONESLIDER] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP3DeadZoneValue"));
	m_aDeadZoneLabel[P4DEADZONESLIDER] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelP4DeadZoneValue"));

	// Set slider values
	m_aDeadZoneSlider[P1DEADZONESLIDER] = dynamic_cast<GUISlider *>(m_pGUIController->GetControl("SliderP1DeadZone"));
	m_aDeadZoneSlider[P2DEADZONESLIDER] = dynamic_cast<GUISlider *>(m_pGUIController->GetControl("SliderP2DeadZone"));
	m_aDeadZoneSlider[P3DEADZONESLIDER] = dynamic_cast<GUISlider *>(m_pGUIController->GetControl("SliderP3DeadZone"));
	m_aDeadZoneSlider[P4DEADZONESLIDER] = dynamic_cast<GUISlider *>(m_pGUIController->GetControl("SliderP4DeadZone"));

	m_aDeadZoneSlider[P1DEADZONESLIDER]->SetValue(g_UInputMan.GetControlScheme(0)->GetJoystickDeadzone() * 250);
	m_aDeadZoneSlider[P2DEADZONESLIDER]->SetValue(g_UInputMan.GetControlScheme(1)->GetJoystickDeadzone() * 250);
	m_aDeadZoneSlider[P3DEADZONESLIDER]->SetValue(g_UInputMan.GetControlScheme(2)->GetJoystickDeadzone() * 250);
	m_aDeadZoneSlider[P4DEADZONESLIDER]->SetValue(g_UInputMan.GetControlScheme(3)->GetJoystickDeadzone() * 250);

	// Set value labels
	char s[256];

	sprintf(s, "%d", m_aDeadZoneSlider[P1DEADZONESLIDER]->GetValue());
	m_aDeadZoneLabel[P1DEADZONESLIDER]->SetText(s);
	sprintf(s, "%d", m_aDeadZoneSlider[P2DEADZONESLIDER]->GetValue());
	m_aDeadZoneLabel[P2DEADZONESLIDER]->SetText(s);
	sprintf(s, "%d", m_aDeadZoneSlider[P3DEADZONESLIDER]->GetValue());
	m_aDeadZoneLabel[P3DEADZONESLIDER]->SetText(s);
	sprintf(s, "%d", m_aDeadZoneSlider[P4DEADZONESLIDER]->GetValue());
	m_aDeadZoneLabel[P4DEADZONESLIDER]->SetText(s);

	// Set deadzone checkboxes
	m_aDeadZoneCheckbox[P1DEADZONESLIDER] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("CheckboxP1DeadZoneType"));
	m_aDeadZoneCheckbox[P2DEADZONESLIDER] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("CheckboxP2DeadZoneType"));
	m_aDeadZoneCheckbox[P3DEADZONESLIDER] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("CheckboxP3DeadZoneType"));
	m_aDeadZoneCheckbox[P4DEADZONESLIDER] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("CheckboxP4DeadZoneType"));

	int dztype = 0;
	char str[2];
	str[0] = -2;
	str[1] = 0;

	dztype = g_UInputMan.GetControlScheme(0)->GetJoystickDeadzoneType();
	if (dztype == UInputMan::DeadZoneType::CIRCLE)
	{
		m_aDeadZoneCheckbox[P1DEADZONESLIDER]->SetCheck(1);
		m_aDeadZoneCheckbox[P1DEADZONESLIDER]->SetText("O");
	} else if(dztype == UInputMan::DeadZoneType::SQUARE) {
		m_aDeadZoneCheckbox[P1DEADZONESLIDER]->SetCheck(0);
		m_aDeadZoneCheckbox[P1DEADZONESLIDER]->SetText(str);
	}

	dztype = g_UInputMan.GetControlScheme(1)->GetJoystickDeadzoneType();
	if (dztype == UInputMan::DeadZoneType::CIRCLE)
	{
		m_aDeadZoneCheckbox[P2DEADZONESLIDER]->SetCheck(1);
		m_aDeadZoneCheckbox[P2DEADZONESLIDER]->SetText("O");
	}
	else if (dztype == UInputMan::DeadZoneType::SQUARE) {
		m_aDeadZoneCheckbox[P2DEADZONESLIDER]->SetCheck(0);
		m_aDeadZoneCheckbox[P2DEADZONESLIDER]->SetText(str);
	}

	dztype = g_UInputMan.GetControlScheme(2)->GetJoystickDeadzoneType();
	if (dztype == UInputMan::DeadZoneType::CIRCLE)
	{
		m_aDeadZoneCheckbox[P3DEADZONESLIDER]->SetCheck(1);
		m_aDeadZoneCheckbox[P3DEADZONESLIDER]->SetText("O");
	}
	else if (dztype == UInputMan::DeadZoneType::SQUARE) {
		m_aDeadZoneCheckbox[P3DEADZONESLIDER]->SetCheck(0);
		m_aDeadZoneCheckbox[P3DEADZONESLIDER]->SetText(str);
	}

	dztype = g_UInputMan.GetControlScheme(3)->GetJoystickDeadzoneType();
	if (dztype == UInputMan::DeadZoneType::CIRCLE)
	{
		m_aDeadZoneCheckbox[P4DEADZONESLIDER]->SetCheck(1);
		m_aDeadZoneCheckbox[P4DEADZONESLIDER]->SetText("O");
	}
	else if (dztype == UInputMan::DeadZoneType::SQUARE) {
		m_aDeadZoneCheckbox[P4DEADZONESLIDER]->SetCheck(0);
		m_aDeadZoneCheckbox[P4DEADZONESLIDER]->SetText(str);
	}

	m_aOptionsCheckbox[FLASHONBRAINDAMAGE] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("FlashOnBrainDamageCheckbox"));
	m_aOptionsCheckbox[FLASHONBRAINDAMAGE]->SetCheck(g_SettingsMan.FlashOnBrainDamage());
    m_aOptionsCheckbox[BLIPONREVEALUNSEEN] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("BlipOnRevealUnseenCheckbox"));
	m_aOptionsCheckbox[BLIPONREVEALUNSEEN]->SetCheck(g_SettingsMan.BlipOnRevealUnseen());
    m_aOptionsCheckbox[SHOWFOREIGNITEMS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("ShowForeignItemsCheckbox"));
	m_aOptionsCheckbox[SHOWFOREIGNITEMS]->SetCheck(g_SettingsMan.ShowForeignItems());
    m_aOptionsCheckbox[SHOWTOOLTIPS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("ShowToolTipsCheckbox"));
	m_aOptionsCheckbox[SHOWTOOLTIPS]->SetCheck(g_SettingsMan.ToolTips());
	m_aOptionsCheckbox[PRECISECOLLISIONS] = dynamic_cast<GUICheckbox *>(m_pGUIController->GetControl("PreciseCollisionsCheckbox"));
	m_aOptionsCheckbox[PRECISECOLLISIONS]->SetCheck(g_SettingsMan.PreciseCollisions());

    m_pResolutionCombo = dynamic_cast<GUIComboBox *>(m_pGUIController->GetControl("ComboResolution"));
    UpdateResolutionCombo();

    // Set labels only when we know max resolution, as it defines whether we can switch to 2X windowed mode or not
	if (g_FrameMan.IsFullscreen())
        m_aOptionButton[FULLSCREEN]->SetText("Go 1X Window");
    else
    {
        if (g_FrameMan.NxWindowed() == 1 && g_FrameMan.GetResX() <= m_MaxResX / 2 - 25 && g_FrameMan.GetResY() <= m_MaxResY / 2 - 25)
            m_aOptionButton[FULLSCREEN]->SetText("Go 2X Window");
        else
            m_aOptionButton[FULLSCREEN]->SetText("Go Fullscreen");
    }

    m_pResolutionNoticeLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelResolutionNotice"));
    // Only show when the new res doesn't mathc the current res
    m_pResolutionNoticeLabel->SetVisible(false);

    m_pSoundLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelSoundVolume"));
    m_pMusicLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelMusicVolume"));
    m_pSoundSlider = dynamic_cast<GUISlider *>(m_pGUIController->GetControl("SliderSoundVolume"));
    m_pMusicSlider = dynamic_cast<GUISlider *>(m_pGUIController->GetControl("SliderMusicVolume"));
    UpdateVolumeSliders();

    UpdateDeviceLabels();

    m_pBackToOptionsButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonBackToOptions"));
    m_pBackToOptionsButton->SetVisible(false);

    // Config screen controls
    m_pConfigLabel[CONFIGTITLE] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelConfigTitle"));
    m_pConfigLabel[CONFIGRECOMMENDATION] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelConfigRecKeyDesc"));
    m_pConfigLabel[CONFIGSTEPS] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelConfigStep"));
    m_pConfigLabel[CONFIGINSTRUCTION] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelConfigInstruction"));
    m_pConfigLabel[CONFIGINPUT] = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelConfigInput"));
    ContentFile diagramFile("Base.rte/GUIs/Controllers/D-Pad.bmp");
    m_aDPadBitmaps = diagramFile.GetAsAnimation(DPADSTEPS, COLORCONV_8_TO_32);
    diagramFile.SetDataPath("Base.rte/GUIs/Controllers/DualAnalog.bmp");
    m_aDualAnalogBitmaps = diagramFile.GetAsAnimation(DANALOGSTEPS, COLORCONV_8_TO_32);
    m_pRecommendationBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigRec"));
    m_pRecommendationDiagram = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigRecDiagram"));
    m_pConfigSkipButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonConfigSkip"));
    m_pConfigBackButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonConfigBack"));

    m_aEditorButton[SCENEEDITOR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonSceneEditor"));
    m_aEditorButton[AREAEDITOR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonAreaEditor"));
    m_aEditorButton[ASSEMBLYEDITOR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonAssemblyEditor"));
    m_aEditorButton[GIBEDITOR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonGibPlacement"));
    m_aEditorButton[ACTOREDITOR] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonActorEditor"));
    m_aEditorButton[WORKSHOPPUBLISH] = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonWorkshopPublish"));

    m_pMetaNoticeLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("MetaLabel"));
    m_pLicenseCaptionLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelLicenseCaption"));
    m_pLicenseInstructionLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelLicenseInstructions"));
    m_pLicenseEmailLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelLicenseEmail"));
    m_pLicenseKeyLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelLicenseKey"));
    m_pLicenseEmailBox = dynamic_cast<GUITextBox *>(m_pGUIController->GetControl("LicenseEmailTextBox"));
    m_pLicenseKeyBox = dynamic_cast<GUITextBox *>(m_pGUIController->GetControl("LicenseKeyTextBox"));
    m_pRegistrationButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonLicenseRegistration"));

    m_pNagExitButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonNagExit"));
    m_pNagRegButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonNagRegister"));

    m_pDPadTypeBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigDPadType"));
    m_pDAnalogTypeBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigDAnalogType"));
    m_pXBox360TypeBox = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigXBox360Type"));
    m_pDPadTypeDiagram = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigDPadTypeDiagram"));
    m_pDAnalogTypeDiagram = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigDAnalogTypeDiagram"));
    m_pXBox360TypeDiagram = dynamic_cast<GUICollectionBox *>(m_pGUIController->GetControl("BoxConfigXBox360TypeDiagram"));
    m_pDPadTypeButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonConfigDPadType"));
    m_pDAnalogTypeButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonConfigDAnalogType"));
    m_pXBox360TypeButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonConfigXBox360Type"));

    // Put the image in and resize appropriately
    m_pDPadTypeDiagram->Resize(m_aDPadBitmaps[0]->w, m_aDPadBitmaps[0]->h);
    m_pDPadTypeDiagram->CenterInParent(true, true);
    m_pDPadTypeDiagram->MoveRelative(0, -8);
    m_pDAnalogTypeDiagram->Resize(m_aDualAnalogBitmaps[0]->w, m_aDualAnalogBitmaps[0]->h);
    m_pDAnalogTypeDiagram->CenterInParent(true, true);
    m_pDAnalogTypeDiagram->MoveRelative(0, -10);
    m_pXBox360TypeDiagram->Resize(m_aDualAnalogBitmaps[0]->w, m_aDualAnalogBitmaps[0]->h);
    m_pXBox360TypeDiagram->CenterInParent(true, true);
    m_pXBox360TypeDiagram->MoveRelative(0, -10);

	// Mod manager controls
	m_pModManagerBackButton = dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonExitModManager"));
	m_pModManagerToggleModButton =  dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonToggleMod"));
	m_pModManagerToggleScriptButton =  dynamic_cast<GUIButton *>(m_pGUIController->GetControl("ButtonToggleScript"));
	m_pModManagerModsListBox = dynamic_cast<GUIListBox *>(m_pGUIController->GetControl("ModsLB"));
	m_pModManagerScriptsListBox = dynamic_cast<GUIListBox *>(m_pGUIController->GetControl("ScriptsLB"));
	m_pModManagerDescriptionLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelDescription"));

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Load mod data and fill the lists
    for (int i = 0; i < g_PresetMan.GetTotalModuleCount(); ++i)  
    {
		// Discard official modules
		if (i >= g_PresetMan.GetOfficialModuleCount() && i < g_PresetMan.GetTotalModuleCount() - 2)
		{
			const DataModule *pModule = g_PresetMan.GetDataModule(i);
			if (pModule)
			{
				ModRecord r;
				r.ModulePath = pModule->GetFileName();
				r.Description = pModule->GetDescription();
				r.ModuleName = pModule->GetFriendlyName();
				r.Disabled = g_SettingsMan.IsModDisabled(r.ModulePath);

				m_KnownMods.push_back(r);
			}
		}
    }

	// Now add missing data from disabled mods settings
	map<string, bool> disabledMods = g_SettingsMan.GetDisabledModsList();
	for(map<std::string, bool>::iterator itr = disabledMods.begin(); itr != disabledMods.end(); ++itr)
	{
		string modPath = itr->first;
		std::transform(modPath.begin(), modPath.end(), modPath.begin(), ::tolower);

		bool found = false;

		// Check if this mod is already in list
		for (vector<ModRecord>::iterator mItr = m_KnownMods.begin(); mItr != m_KnownMods.end(); ++mItr)
			if (modPath == (*mItr).ModulePath)
				found = true;

		if (!found)
		{
			ModRecord r;
			r.ModulePath = modPath;
			r.Description = "n/a, module not loaded";
			r.ModuleName = "n/a, module not loaded";
			r.Disabled = itr->second;

			m_KnownMods.push_back(r);
		}
	}

	// Sort the list
	std::sort(m_KnownMods.begin(), m_KnownMods.end());

	// Fill the GUI listbox with loaded mod data
	for (int i = 0; i < m_KnownMods.size(); i++)
	{
		ModRecord r = m_KnownMods.at(i);
		m_pModManagerModsListBox->AddItem(MakeModString(r), "", 0, 0, i);
	}


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Load script data and fill the lists
	std::list<Entity *> globalScripts;
	g_PresetMan.GetAllOfType(globalScripts, "GlobalScript");

	for (std::list<Entity *>::iterator sItr = globalScripts.begin(); sItr != globalScripts.end(); ++sItr )
	{
		GlobalScript * script = dynamic_cast<GlobalScript *>(*sItr);
		if (script)
		{
			ScriptRecord r;
			r.PresetName = script->GetModuleAndPresetName();
			r.Description = script->GetDescription();
			r.Enabled = g_SettingsMan.IsScriptEnabled(r.PresetName);
			m_KnownScripts.push_back(r);
		}
	}

	// Sort the list
	std::sort(m_KnownScripts.begin(), m_KnownScripts.end());

	// Fill the GUI listbox with loaded mod data
	for (int i = 0; i < m_KnownScripts.size(); i++)
	{
		ScriptRecord r = m_KnownScripts.at(i);
		m_pModManagerScriptsListBox->AddItem(MakeScriptString(r), "", 0, 0, i);
	}

    // Read all the credits from the file and set the credits label
    GUILabel *pCreditsLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("CreditsLabel"));
    Reader creditsReader("Credits.txt");
    string creditsText = creditsReader.ReadTo('#', true);

// TODO: Get Unicode going!
    // Hack here to change the special characters over 128 in the ansi ascii table to match our font files
    for (string::iterator sItr = creditsText.begin(); sItr != creditsText.end(); ++sItr)
    {
        if (*sItr == -60)//'Ä')
            (*sItr) = (char)142;
        if (*sItr == -42)//'Ö')
            (*sItr) = (char)153;
        if (*sItr == -87)//'©')
            (*sItr) = (char)221;
    }
    pCreditsLabel->SetText(creditsText);
    m_pScrollPanel->Resize(m_pScrollPanel->GetWidth(), pCreditsLabel->ResizeHeightToFit());

    // Set initial focus, category list, and label settings
    m_ScreenChange = true;
    m_FocusChange = 1;
//    CategoryChange();

    m_EnterMenuSound.Create("Base.rte/GUIs/Sounds/MenuEnter.wav", false);
    m_ExitMenuSound.Create("Base.rte/GUIs/Sounds/MenuExit.wav", false);
    m_ExitMenuSound.AddSample("Base.rte/GUIs/Sounds/MenuExitB.wav");
    m_ExitMenuAltSound.Create("Base.rte/GUIs/Sounds/MenuExitB.wav", false);
    m_FocusChangeSound.Create("Base.rte/GUIs/Sounds/Blip00.wav", false);
    m_SelectionChangeSound.Create("Base.rte/GUIs/Sounds/Blip02.wav", false);
    m_ButtonPressSound.Create("Base.rte/GUIs/Sounds/Splip01.wav", false);
    m_BackButtonPressSound.Create("Base.rte/GUIs/Sounds/Splip02.wav", false);
    m_ItemChangeSound.Create("Base.rte/GUIs/Sounds/Click00.wav", false);
    m_TestSound.Create("Base.rte/GUIs/Sounds/Click01.wav", false);
    m_UserErrorSound.Create("Base.rte/GUIs/Sounds/Error.wav", false);

	m_PioneerPromoVisible = false;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the MainMenuGUI object.

void MainMenuGUI::Destroy()
{
    delete m_pGUIController;
    delete m_pGUIInput;
    delete m_pGUIScreen;

    // Delete only the array, not the bitmaps themselves, we don't own them
    delete [] m_aDPadBitmaps;
    delete [] m_aDualAnalogBitmaps;

    if (m_apNagSlides)
    {
        for (int screen = 0; screen < NAGSCREENCOUNT; ++screen)
            destroy_bitmap(m_apNagSlides[screen]);
    }
    delete [] m_apNagSlides;

    Clear();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetGUIControlManager
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the GUIControlManager owned and used by this.

GUIControlManager * MainMenuGUI::GetGUIControlManager()
{
    return m_pGUIController;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetEnabled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Enables or disables the menu. This will animate it in and out of view.

void MainMenuGUI::SetEnabled(bool enable)
{
    if (enable && m_MenuEnabled != ENABLED && m_MenuEnabled != ENABLING)
    {
        m_MenuEnabled = ENABLING;
        m_EnterMenuSound.Play();
    }
    else if (!enable && m_MenuEnabled != DISABLED && m_MenuEnabled != DISABLING)
    {
        m_MenuEnabled = DISABLING;
        m_ExitMenuSound.Play();
    }

    m_ScreenChange = true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Static method:   PublishingProgressReport
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the workshop publishing status screen with a line of text that
//                  the user sees as reported progress.

void MainMenuGUI::PublishingProgressReport(std::string reportString, bool newItem)
{
    if (m_pPublishStatus)
    {
        if (newItem || m_pPublishStatus->GetItemList()->empty())
        {
            // Write out the last line to the log file before starting a new one
            if (m_PublishLogWriter.WriterOK() && !m_pPublishStatus->GetItemList()->empty())
                m_PublishLogWriter << m_pPublishStatus->GetItemList()->back()->m_Name << "\n";

            // Add the new report line
            m_pPublishStatus->AddItem(reportString);
            m_pPublishStatus->ScrollToBottom();
        }
        else
        {
            int lastItemIndex = m_pPublishStatus->GetItemList()->size() - 1;
            GUIListPanel::Item *pItem = m_pPublishStatus->GetItem(lastItemIndex);
            pItem->m_Name = reportString;
            m_pPublishStatus->SetItemValues(lastItemIndex, *pItem);
        }

        // Pump things so they update on the screen
        g_UInputMan.Update();
        g_TimerMan.Update();
        m_pGUIController->Update();
        m_pGUIController->Draw();
//        m_pGUIController->DrawMouse();
        g_FrameMan.FlipFrameBuffers();
	
#if defined(STEAM_BUILD)
		  // pump steam while loading game too
		  g_SteamUGCMan.Update();
#endif // 

        // Quit if we're commanded to during loading
//        if (g_Quit)
//            exit(0);
    }
}

#ifndef __OPEN_SOURCE_EDITION

/////////////////////////////
// TURN OPTIMIZATIONS OFF
// This is so the EXECryptor markers don't get mangled by the optimizer

#pragma optimize("", off)

#endif

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this Menu each frame

void MainMenuGUI::Update()
{
#ifndef __OPEN_SOURCE_EDITION
    CRYPT_START
#endif

    // Update the input controller
    m_pController->Update();

    // Reset the specific triggers
    m_ScenarioStarted = false;
    m_CampaignStarted = false;
    m_ActivityRestarted = false;
    m_ActivityResumed = false;
    m_StartDifficulty = 0;
    m_Quit = false;

    // Don't update the main menu if the console is open
    if (g_ConsoleMan.IsEnabled())
        return;

    // If esc pressed, show quit dialog if applicable
    if (g_UInputMan.KeyPressed(KEY_ESC))
        QuitLogic();

    ////////////////////////////////////////////////////////////////////////
    // Animate the menu into and out of view if enabled or disabled

	// Quit now if we aren't enabled
    if (m_MenuEnabled != ENABLED &&  m_MenuEnabled != ENABLING)
        return;

	//////////////////////////////////////
    // MAINSCREEN MENU SCREEN

    if (m_MenuScreen == MAINSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[MAINSCREEN]->SetVisible(true);
            // Replace the Register button with the Resume game button if there's a game to be resumed
            if (g_ActivityMan.GetActivity() && (g_ActivityMan.GetActivity()->GetActivityState() == Activity::RUNNING || g_ActivityMan.GetActivity()->GetActivityState() == Activity::EDITING))
            {
                m_apScreenBox[MAINSCREEN]->Resize(128, 220);
                m_aMainMenuButton[RESUME]->SetVisible(true);
                m_aMainMenuButton[REGISTER]->SetVisible(false);
            }
            else
            {
#ifdef NODRM
                m_apScreenBox[MAINSCREEN]->Resize(128, 196);
#else // NODRM
                m_apScreenBox[MAINSCREEN]->Resize(128, 220);
#endif // NODRM
                m_aMainMenuButton[RESUME]->SetVisible(false);
#ifndef NODRM
                m_aMainMenuButton[REGISTER]->SetVisible(true);
#endif // NODRM
            }
            // Set appropriate label on the registration button
            m_aMainMenuButton[REGISTER]->SetText(!g_LicenseMan.HasValidatedLicense() ? "Register" : "Un-Register");
            // Restore the label on the campaign button
            m_aMainMenuButton[CAMPAIGN]->SetText("Campaign");

            m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
            m_aMainMenuButton[PLAYTUTORIAL]->SetVisible(false);
            m_aMainMenuButton[METACONTINUE]->SetVisible(false);
            // Move main menu button back to center
            m_aMainMenuButton[BACKTOMAIN]->SetPositionRel(260, 280);
            m_ScreenChange = false;
        }
    
        // Blink the resume button to show the game is still going
        if (m_aMainMenuButton[RESUME]->GetVisible())
        {
            if (m_BlinkTimer.AlternateReal(500))
                m_aMainMenuButton[RESUME]->SetFocus();
            else
                m_pGUIController->GetManager()->SetFocus(0);
        }
        // Blink the register button if still unregistered
        else if (m_aMainMenuButton[REGISTER]->GetVisible() && m_aMainMenuButton[REGISTER]->GetText() == "Register")
        {
            if (m_BlinkTimer.AlternateReal(500))
                m_aMainMenuButton[REGISTER]->SetFocus();
            else
                m_pGUIController->GetManager()->SetFocus(0);
        }
		
		// Detect whether Planetoid Pioneers promo was clicked and open 
		if (m_PioneerPromoVisible)
		{
			bool buttonHeld = g_UInputMan.MouseButtonPressed(UInputMan::MOUSE_LEFT, -1);
			if (buttonHeld)
			{
				// Get mouse position
				int mouseX, mouseY;
				m_pGUIInput->GetMousePosition(&mouseX, &mouseY);
				Vector mouse(mouseX, mouseY);

				if (m_PioneerPromoBox.WithinBox(mouse))
				{
					OpenBrowserToURL("http://store.steampowered.com/app/300260/");
				}
			}
		}
    }

    //////////////////////////////////////
    // PLAYERS MENU SCREEN

    else if (m_MenuScreen == PLAYERSSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[PLAYERSSCREEN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
            m_ScreenChange = false;
        }

//        m_aMainMenuButton[BACKTOMAIN]->SetFocus();
    }

    //////////////////////////////////////
    // SKIRMISH SETUP MENU SCREEN

    else if (m_MenuScreen == SKIRMISHSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[SKIRMISHSCREEN]->SetVisible(true);
            // Set up the list of scenes to choose from
            UpdateScenesBox();
//            m_pGUIController->GetControl("ButtonStartSkirmish")->SetVisible(true);
            UpdateTeamBoxes();
            // Move main menu button over so the start button fits
            m_aMainMenuButton[BACKTOMAIN]->SetPositionRel(200, 280);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
            m_ScreenChange = false;
        }

//        for (int box = 0; box < SKIRMISHPLAYERCOUNT; ++box)
//            m_aSkirmishBox[box] = 0;

//        m_aMainMenuButton[BACKTOMAIN]->SetFocus();
    }

	//////////////////////////////////////
    // DIFFICULTY MENU SCREEN

    else if (m_MenuScreen == DIFFICULTYSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[DIFFICULTYSCREEN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
            m_ScreenChange = false;
        }

//        m_aMainMenuButton[BACKTOMAIN]->SetFocus();
    }

    //////////////////////////////////////
    // OPTIONS MENU SCREEN

    else if (m_MenuScreen == OPTIONSSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[OPTIONSSCREEN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
			m_aMainMenuButton[BACKTOMAIN]->SetPositionRel(260, 332);
            m_pBackToOptionsButton->SetVisible(false);
            UpdateDeviceLabels();
            m_ScreenChange = false;
        }
    }

    //////////////////////////////////////
    // CONFIGURATION SCREEN

    else if (m_MenuScreen == CONFIGSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[CONFIGSCREEN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
            m_pBackToOptionsButton->SetVisible(true);
            // Let this pass through, UpdateConfigScreen uses it
            //m_ScreenChange = false;
        }

        // Continuously update the contents through all the config steps
        UpdateConfigScreen();
    }

    //////////////////////////////////////
    // MOD MANAGER SCREEN

    else if (m_MenuScreen == MODMANAGERSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[MODMANAGERSCREEN]->SetVisible(true);
        }
    }


    //////////////////////////////////////
    // LICENSE REGISTRATION SCREEN

    else if (m_MenuScreen == LICENSESCREEN)
    {
        if (m_ScreenChange)
        {
            // Clear the key box, it will be set again in UpdateLicenseScreen if registered
            m_pLicenseEmailBox->SetText("");
            m_pLicenseKeyBox->SetText("");
            UpdateLicenseScreen();
            m_apScreenBox[LICENSESCREEN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
            m_aMainMenuButton[REGISTER]->SetVisible(false);
            m_pBackToOptionsButton->SetVisible(false);
            m_ScreenChange = false;
        }


    }

    //////////////////////////////////////
    // EDITOR MENU SCREEN

    else if (m_MenuScreen == EDITORSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[EDITORSCREEN]->SetVisible(true);
//            m_aEditorButton[ACTOREDITOR]->SetEnabled(false);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetPositionRel(260, 318);

            // If Steam Workshop is in play, show that button too, resizing the menu appropriately
#if defined(STEAM_BUILD)
            if (g_SteamUGCMan.IsCloudEnabled())
            {
                //m_pEditorPanel->SetPositionRel(0, 0);
                m_pEditorPanel->Resize(m_pEditorPanel->GetWidth(), 148);
                m_aEditorButton[WORKSHOPPUBLISH]->SetVisible(true);
            }
            else
            {
                //m_pEditorPanel->SetPositionRel(0, 0);
                m_pEditorPanel->Resize(m_pEditorPanel->GetWidth(), 124);
                m_aEditorButton[WORKSHOPPUBLISH]->SetVisible(false);
            }
#else // defined (STEAM_BUILD)
            m_aEditorButton[WORKSHOPPUBLISH]->SetVisible(false);
#endif // defined (STEAM_BUILD)

            m_ScreenChange = false;
        }

//        m_aMainMenuButton[BACKTOMAIN]->SetFocus();
    }

    //////////////////////////////////////
    // WORKSHOP MANAGER SCREEN

    else if (m_MenuScreen == WORKSHOPMANAGER)
    {
#if defined(STEAM_BUILD)
        if (m_ScreenChange)
        {
            m_apScreenBox[WORKSHOPMANAGER]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
            // Re-populate the dropdown list of already-published mods
            m_pPublishedList->ClearList();

            // Retrieve them first from the cloud
            deque<string> publishedNames;
            g_SteamUGCMan.GetPublishedNamesList(publishedNames);

            // Now put them into the interface
            if (!publishedNames.empty())
            {
                for (deque<string>::iterator itr = publishedNames.begin(); itr != publishedNames.end(); itr++)
                    m_pPublishedList->AddItem(*itr);

                // Select the first and show update controls
                m_pPublishedList->SetSelectedIndex(0);
                m_pPublishedList->SetVisible(true);
                m_pEditUpdateButton->SetVisible(true);
                m_pPublishedListHeader->SetText("Your published .rte Data Modules:");
            }
            else
            {
                m_pPublishedList->SetVisible(false);
                m_pEditUpdateButton->SetVisible(false);
                m_pPublishedListHeader->SetText("Your published .rte Data Modules:\n\n- No Data Modules published yet -");
            }

            m_ScreenChange = false;
        }

        // Keep pumping that Steam API - there are callback results that might happen while we are out here
        g_SteamUGCMan.Update();

        // Whether to show the edit/update button when something isn't selected
        m_pEditUpdateButton->SetVisible(m_pPublishedList->GetSelectedIndex() != -1);
#endif // defined (STEAM_BUILD)

    }

    //////////////////////////////////////
    // WORKSHOP PUBLISHING SCREEN

    else if (m_MenuScreen == PUBLISHSTEPS)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[PUBLISHSTEPS]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
// Do after the steps dialog has been updated
//            m_ScreenChange = false;
        }

        // Always be updatin'
        UpdatePublishingStepsDialog();

        // Now clear the screen change flag
        m_ScreenChange = false;
    }

    //////////////////////////////////////
    // CREDITS MENU SCREEN

    else if (m_MenuScreen == CREDITSSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[CREDITSSCREEN]->SetVisible(true);
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(true);
            // Set the scroll panel to be out of sight at the bottom of the credits screen box
            m_pScrollPanel->SetPositionRel(0, m_apScreenBox[CREDITSSCREEN]->GetHeight());
            m_ScrollTimer.Reset();
            m_ScreenChange = false;
        }

        long scrollTime = 36000;
        float scrollProgress = (float)m_ScrollTimer.GetElapsedRealTimeMS() / (float)scrollTime;
        int scrollDist = -m_apScreenBox[CREDITSSCREEN]->GetHeight() + (-m_pScrollPanel->GetHeight());
        // Scroll the scroll panel upwards, GetYPos returns absolute coordinates
        m_pScrollPanel->SetPositionRel(0, m_apScreenBox[CREDITSSCREEN]->GetHeight() + (scrollDist * scrollProgress));
        // If we've scrolled through the whole thing, reset to the bottom and restart scroll
        if (m_ScrollTimer.IsPastRealMS(scrollTime))
        {
            m_pScrollPanel->SetPositionRel(0, m_apScreenBox[CREDITSSCREEN]->GetHeight());
            m_ScrollTimer.Reset();
        }

//        m_aMainMenuButton[BACKTOMAIN]->SetFocus();
    }

    //////////////////////////////////////
    // METAGAME NOTICE SCREEN

    else if (m_MenuScreen == METASCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[METASCREEN]->SetVisible(true);
            m_aMainMenuButton[PLAYTUTORIAL]->SetVisible(true);
            m_aMainMenuButton[METACONTINUE]->SetVisible(true);
            m_pMetaNoticeLabel->SetText("- A T T E N T I O N -\n\nPlease note that the Campaign is currently in a COMPLETE, fully playable,\nyet still imperfect state! As such, it is lacking some polish, audio, and game balancing.\nThat said, you can absolutely enjoy fighting the A.I. and/or up to three friends in co-op, 2 vs 2, etc.\n\nAlso, if you have not yet played Cortex Command, we recommend you first try the tutorial:");
            m_pMetaNoticeLabel->SetVisible(true);
            // Flag that this notice has now been shown once, so no need to keep showing it
            m_TutorialOffered = true;
            m_ScreenChange = false;
        }

//        m_aMainMenuButton[BACKTOMAIN]->SetFocus();
    }

    //////////////////////////////////////
    // NAG SCREEN

    else if (m_MenuScreen == NAGSCREEN)
    {
        if (m_ScreenChange)
        {
            m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
            m_pBackToOptionsButton->SetVisible(false);

            // Set the background image of the screen
            m_apScreenBox[NAGSCREEN]->SetVisible(true);
            m_apScreenBox[NAGSCREEN]->SetDrawBackground(true);
            m_apScreenBox[NAGSCREEN]->SetDrawType(GUICollectionBox::Image);

            // Set the appropriate continue button text for the mode
            if (m_NagMode == EDITORNAG)
                m_pNagExitButton->SetText("Back to Main Menu");
            else// if (m_NagMode == QUITNAG)
                m_pNagExitButton->SetText("Quit");

            // Hide that continue button, only to show it on the last slide!
            m_pNagExitButton->SetVisible(false);

            // Start the timer to time the showing of different slides
            m_NagTimer.Reset();

            // Nag slides haven't been read yet, so read em in lazy style
            if (!m_apNagSlides)
            {
                ContentFile nagDisplay("Base.rte/GUIs/RegSplash/RegSplashQuit.bmp");
                // This transferrs ownership to this of both array and bitmaps
                m_apNagSlides = nagDisplay.LoadAndReleaseAnimation(NAGSCREENCOUNT, COLORCONV_8_TO_32);
            }
            m_ScreenChange = false;
        }

        // Determine which slide to be showing
        int nagSlide = 0;
        float nagTime = m_NagTimer.GetElapsedRealTimeMS();
        if (nagTime < NAGDURATIONMS)
            nagSlide = (nagTime / (float)NAGDURATIONMS) * NAGSCREENCOUNT;
        else
            nagSlide = NAGSCREENCOUNT - 1;

        // Show the appropriate slide
        // Ownership of the BITMAP doesn't transfer here, it stays with us
        m_apScreenBox[NAGSCREEN]->SetDrawImage(new AllegroBitmap(m_apNagSlides[nagSlide]));

        // Only show the continue button when on the last slide!
        if (nagSlide == (NAGSCREENCOUNT - 1))
            m_pNagExitButton->SetVisible(true);
    }

    //////////////////////////////////////
    // QUIT CONFIRM SCREEN

    else if (m_MenuScreen == QUITSCREEN)
    {
        if (m_ScreenChange)
        {
            m_apScreenBox[QUITSCREEN]->SetVisible(true);
            m_ScreenChange = false;
        }

//        m_aMainMenuButton[QUITCONFIRM]->SetFocus();
    }

    //////////////////////////////////////////
    // Update the ControlManager

    m_pGUIController->Update();

    ///////////////////////////////////////
    // Handle events

	GUIEvent anEvent;
	while(m_pGUIController->GetEvent(&anEvent))
    {
        // Commands
		if (anEvent.GetType() == GUIEvent::Command)
        {
			// Campaign button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[CAMPAIGN])
            {
/*
                // Disable the campaign button for now
                if (m_aMainMenuButton[CAMPAIGN]->GetText() == "Campaign")
                {
                    m_aMainMenuButton[CAMPAIGN]->SetText("COMING SOON!");
                    m_ExitMenuSound.Play();
                }
                else
                {
                    m_aMainMenuButton[CAMPAIGN]->SetText("Campaign");
                    m_ButtonPressSound.Play();
                }
*/
                // Show the metagame notice screen if it hasn't already been shown yet
                if (!m_TutorialOffered)
                    m_MenuScreen = METASCREEN;
                // Start the campaign right away!
                else
                {
                    m_CampaignStarted = true;
                    m_MenuScreen = MAINSCREEN;
                }

                HideAllScreens();
                m_ScreenChange = true;
                m_ButtonPressSound.Play();
            }

			// Skirmish button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[SKIRMISH])
            {
                m_ScenarioStarted = true;
                m_CampaignStarted = false;

                if (g_MetaMan.GameInProgress())
					g_MetaMan.EndGame();

                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
//                m_MenuScreen = PLAYERSSCREEN;
                m_MenuScreen = MAINSCREEN;
                m_ScreenChange = true;
                m_ButtonPressSound.Play();
//                m_ExitMenuSound.Play();
            }

			if (anEvent.GetControl() == m_aMainMenuButton[MULTIPLAYER])
			{
				m_ScenarioStarted = true;
				m_CampaignStarted = false;

				if (g_MetaMan.GameInProgress())
					g_MetaMan.EndGame();

				// Hide all screens, the appropriate screen will reappear on next update
				//HideAllScreens();
				//                m_MenuScreen = PLAYERSSCREEN;
				//m_MenuScreen = MAINSCREEN;
				//m_ScreenChange = true;
				m_ButtonPressSound.Play();
				//                m_ExitMenuSound.Play();

				HideAllScreens();
				m_MenuScreen = MAINSCREEN;
				m_ScreenChange = true;
				m_ActivityRestarted = true;
				m_ExitMenuSound.Play();

				g_SceneMan.SetSceneToLoad("Editor Scene");
				MultiplayerGame *pMultiplayerGame = new MultiplayerGame;
				pMultiplayerGame->Create();
				g_ActivityMan.SetStartActivity(pMultiplayerGame);
			}

			// Options button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[OPTIONS])
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = OPTIONSSCREEN;
                m_ScreenChange = true;

                m_ButtonPressSound.Play();
            }

			// Editor button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[EDITOR])
            {
                m_CampaignStarted = false;

                if (g_MetaMan.GameInProgress())
					g_MetaMan.EndGame();

                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();

                // If registered, go to the editor screen as normal, if not, go to the nag screen!
                // Nag here even if we have a last entered key - should deny access
                if (g_LicenseMan.HasValidatedLicense())
                    m_MenuScreen = EDITORSCREEN;
                else
                {
                    m_MenuScreen = NAGSCREEN;
                    m_NagMode = EDITORNAG;
                }
                m_ScreenChange = true;

                m_ButtonPressSound.Play();
//                m_UserErrorSound.Play();
            }

			// Editor button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[MODMANAGER])
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = MODMANAGERSCREEN;
                m_ScreenChange = true;

                m_ButtonPressSound.Play();
            }

			// Credits button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[CREDITS])
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = CREDITSSCREEN;
                m_ScreenChange = true;

                m_ButtonPressSound.Play();
            }

			// Quit button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[QUIT])
            {
                QuitLogic();
                m_ButtonPressSound.Play();
            }

			// Resume button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[RESUME])
            {
                m_ActivityResumed = true;

                m_ExitMenuSound.Play();
            }

			// Register button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[REGISTER])
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = LICENSESCREEN;
                m_ScreenChange = true;

                m_ButtonPressSound.Play();
            }

			// Fullscreen toggle button pressed
			if (anEvent.GetControl() == m_aOptionButton[FULLSCREEN])
            {
                m_ButtonPressSound.Play();

                // Was fullscreen, switch to 1x window
                if (g_FrameMan.IsFullscreen())
                {
                    // First set the multiplier back to 1 and then switch to fullscreen so we get the right multiplier
                    g_FrameMan.SwitchWindowMultiplier(1);
                    g_FrameMan.ToggleFullscreen();
                }
                // Was windowed
                else
                {
                    // Was at 1x, change to 2x, but only if resolution is reasonable
                    if (g_FrameMan.NxWindowed() == 1 && g_FrameMan.GetResX() <= m_MaxResX / 2 - 25 && g_FrameMan.GetResY() <= m_MaxResY / 2 - 25)
                        g_FrameMan.SwitchWindowMultiplier(2);
                    // Was at 2x, change to fullscreen
                    else
                        g_FrameMan.ToggleFullscreen();
                }

                // Update the label to whatever we ended up with
                if (g_FrameMan.IsFullscreen())
                    m_aOptionButton[FULLSCREEN]->SetText("Go 1X Window");
                else
                {
                    if (g_FrameMan.NxWindowed() == 1 && g_FrameMan.GetResX() <= m_MaxResX / 2 - 25 && g_FrameMan.GetResY() <= m_MaxResY / 2 - 25)
                        m_aOptionButton[FULLSCREEN]->SetText("Go 2X Window");
                    else
                        m_aOptionButton[FULLSCREEN]->SetText("Go Fullscreen");
                }
            }

			// Return to main menu button pressed
			if (anEvent.GetControl() == m_aMainMenuButton[BACKTOMAIN])
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_aMainMenuButton[BACKTOMAIN]->SetVisible(false);
                m_aMainMenuButton[BACKTOMAIN]->SetPositionRel(260, 280);
#ifndef NODRM
                m_aMainMenuButton[REGISTER]->SetVisible(true);
#endif // NODRM

                // If leaving the options screen, save the settings!
                if (m_MenuScreen == OPTIONSSCREEN)
				{
					g_SettingsMan.SetFlashOnBrainDamage(m_aOptionsCheckbox[FLASHONBRAINDAMAGE]->GetCheck());
					g_SettingsMan.SetBlipOnRevealUnseen(m_aOptionsCheckbox[BLIPONREVEALUNSEEN]->GetCheck());
					g_SettingsMan.SetShowForeignItems(m_aOptionsCheckbox[SHOWFOREIGNITEMS]->GetCheck());
					g_SettingsMan.SetShowToolTips(m_aOptionsCheckbox[SHOWTOOLTIPS]->GetCheck());
					g_SettingsMan.SetPreciseCollisions(m_aOptionsCheckbox[PRECISECOLLISIONS]->GetCheck());

					Writer writer("Base.rte/Settings.ini");
                    g_SettingsMan.Save(writer);
				}

                m_MenuScreen = MAINSCREEN;
                m_ScreenChange = true;

                m_BackButtonPressSound.Play();
            }

			// Return to options menu button pressed
			if (anEvent.GetControl() == m_pBackToOptionsButton)
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_pBackToOptionsButton->SetVisible(false);
                m_MenuScreen = OPTIONSSCREEN;
                m_ScreenChange = true;

                m_BackButtonPressSound.Play();
            }

            /////////////////////////////////////////////
            // PLAYER SCREEN BUTTONS
			// Player count setting button pressed

			if (m_MenuScreen == PLAYERSSCREEN && 
                (anEvent.GetControl()->GetName() == "ButtonOnePlayer" ||
                 anEvent.GetControl()->GetName() == "ButtonTwoPlayers" ||
                 anEvent.GetControl()->GetName() == "ButtonThreePlayers" ||
                 anEvent.GetControl()->GetName() == "ButtonFourPlayers"))
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = SKIRMISHSCREEN;
                m_ScreenChange = true;

                // Set desired player count
                if (anEvent.GetControl()->GetName() == "ButtonOnePlayer")
                    m_StartPlayers = 1;
                else if (anEvent.GetControl()->GetName() == "ButtonTwoPlayers")
                    m_StartPlayers = 2;
                else if (anEvent.GetControl()->GetName() == "ButtonThreePlayers")
                    m_StartPlayers = 3;
                else if (anEvent.GetControl()->GetName() == "ButtonFourPlayers")
                    m_StartPlayers = 4;
                else
                    m_StartPlayers = 0;

                m_ButtonPressSound.Play();
            }

            /////////////////////////////////////////////
            // SKIRMISH SETUP SCREEN BUTTONS

			if (m_MenuScreen == SKIRMISHSCREEN)
            {
                for (int player = 0; player < SKIRMISHPLAYERCOUNT; ++player)
                {
                    // Player team toggle button
			        if (anEvent.GetControl() == m_aSkirmishButton[player])
                    {
                        // Toggle
                        if (m_aTeamAssignments[player] == Activity::TEAM_1)
                            m_aTeamAssignments[player] = Activity::TEAM_2;
                        else
                            m_aTeamAssignments[player] = Activity::TEAM_1;

                        UpdateTeamBoxes();

                        m_ButtonPressSound.Play();
                    }
                }

			    // Start Skirmish menu button pressed
			    if (anEvent.GetControl()->GetName() == "ButtonStartSkirmish")
                {
                    // Hide all screens, the appropriate screen will reappear on next update
                    HideAllScreens();
                    
                    // No CPU team, so just start game
                    if (m_CPUTeam < 0)
                    {
                        m_MenuScreen = MAINSCREEN;
                        m_ScreenChange = true;
                        m_ActivityRestarted = true;
                        SetupSkirmishActivity();
                        m_ExitMenuSound.Play();
                    }
                    // CPU team present, so ask for the difficulty level of it before starting
                    else
                    {
                        // Move main menu button back to center
                        m_aMainMenuButton[BACKTOMAIN]->SetPositionRel(260, 280);
                        m_MenuScreen = DIFFICULTYSCREEN;
                        m_ScreenChange = true;
                        m_ButtonPressSound.Play();
                    }
                }
            }

            /////////////////////////////////////////////
            // DIFFICULTY SETUP SCREEN BUTTONS

			if (m_MenuScreen == DIFFICULTYSCREEN && 
                (anEvent.GetControl()->GetName() == "ButtonStartEasy" ||
                anEvent.GetControl()->GetName() == "ButtonStartMedium" ||
                anEvent.GetControl()->GetName() == "ButtonStartHard" ||
                anEvent.GetControl()->GetName() == "ButtonStartDeath"))
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = MAINSCREEN;
                m_ScreenChange = true;

                m_ActivityRestarted = true;

                // Set appropriate difficulty level
                if (anEvent.GetControl()->GetName() == "ButtonStartEasy")
                    m_StartDifficulty = GameActivity::EASYDIFFICULTY;
                else if (anEvent.GetControl()->GetName() == "ButtonStartMedium")
                    m_StartDifficulty = GameActivity::MEDIUMDIFFICULTY;
                else if (anEvent.GetControl()->GetName() == "ButtonStartHard")
                    m_StartDifficulty = GameActivity::HARDDIFFICULTY;
                else if (anEvent.GetControl()->GetName() == "ButtonStartDeath")
                    m_StartDifficulty = GameActivity::MAXDIFFICULTY;
                else
                    m_StartDifficulty = GameActivity::MEDIUMDIFFICULTY;

                SetupSkirmishActivity();

//                m_BackButtonPressSound.Play();
                m_ExitMenuSound.Play();
            }

            /////////////////////////////////////////////
            // CONTROL CONFIG UI

            // Control options
            if (m_MenuScreen == OPTIONSSCREEN)
            {
                int which = 0;
                int player = 0;

                // Handle all device select button pushes
                for (which = P1NEXT; which <= P4PREV; ++which)
                {
                    // Calculate the owner of the currently checked button, and if it's next/prev button
                    player = (which - P1NEXT) % UInputMan::MAX_PLAYERS;
                    bool nextButton = which < P1PREV;

                    // Handle the appropriate player's device setting
                    if (anEvent.GetControl() == m_aOptionButton[which])
                    {
                        // What's the current device
                        int currentDevice = g_UInputMan.GetControlScheme(player)->GetDevice();
                        // Next button pressed, so increment
                        if (nextButton)
                        {
                            // Loop around to first if we've gone around
                            if (++currentDevice >= UInputMan::DEVICE_COUNT)
                                currentDevice = 0;
                        }
                        // Prev button pressed, so decrement
                        else
                        {
                            // Loop around to last if we've gone around
                            if (--currentDevice < 0)
                                currentDevice = UInputMan::DEVICE_COUNT - 1;
                        }
                        // Set the device and update labels
                        g_UInputMan.GetControlScheme(player)->SetDevice(currentDevice);
                        UpdateDeviceLabels();

                        m_ButtonPressSound.Play();
                    }
                }

                // Handle all control config buttons
                for (which = P1CONFIG; which <= P4CONFIG; ++which)
                {
                    // Handle the appropriate player's device setting
                    if (anEvent.GetControl() == m_aOptionButton[which])
                    {
                        m_apScreenBox[OPTIONSSCREEN]->SetVisible(false);
                        m_ConfiguringPlayer = which - P1CONFIG;
                        m_ConfiguringDevice = g_UInputMan.GetControlScheme(m_ConfiguringPlayer)->GetDevice();
                        m_ConfigureStep = 0;
                        m_MenuScreen = CONFIGSCREEN;
                        m_ScreenChange = true;

						// Save joystick deadzones. Dead zone should work independently of the controller setup process
						// bacuse faulty sticks will ruin setup process
						float deadzone = g_UInputMan.GetControlScheme(m_ConfiguringPlayer)->GetJoystickDeadzone();
                        // Clear all the mappings when the user wants to config somehting
                        g_UInputMan.GetControlScheme(m_ConfiguringPlayer)->Reset();
						g_UInputMan.GetControlScheme(m_ConfiguringPlayer)->SetJoystickDeadzone(deadzone);
                        g_UInputMan.GetControlScheme(m_ConfiguringPlayer)->SetDevice(m_ConfiguringDevice);

                        m_ButtonPressSound.Play();
                    }
                }

                // Handle all control reset buttons
                for (which = P1CLEAR; which <= P4CLEAR; ++which)
                {
                    // Handle the appropriate player's clearing of mappings
                    if (anEvent.GetControl() == m_aOptionButton[which])
                    {
                        // Make user click twice to confirm
                        if (m_aOptionButton[which]->GetText() == "Reset")
                        {
                            // Ask to confirm!
                            m_aOptionButton[which]->SetText("CONFIRM?");
                            // And clear all other buttons of it
                            for (int otherButton = P1CLEAR; otherButton <= P4CLEAR; ++otherButton)
                                if (otherButton != which)
                                    m_aOptionButton[otherButton]->SetText("Reset");
                            m_ButtonPressSound.Play();
                        }
                        else
                        {
                            // Save the device type so we can re-set it after resetting - it's really annoying when the device changes
                            int device = g_UInputMan.GetControlScheme(which - P1CLEAR)->GetDevice();
                            g_UInputMan.GetControlScheme(which - P1CLEAR)->Reset();
                            g_UInputMan.GetControlScheme(which - P1CLEAR)->SetDevice(device);
                            // Set some default mappings so there's something at least
                            g_UInputMan.GetControlScheme(which - P1CLEAR)->SetPreset(UInputMan::PRESET_NONE);
                            UpdateDeviceLabels();

							// Set the dead zone slider value
							m_aDeadZoneSlider[which - P1CLEAR]->SetValue(g_UInputMan.GetControlScheme(which - P1CLEAR)->GetJoystickDeadzone() * 250);

//                            m_aOptionsLabel[P1DEVICE + (which - P1CLEAR)]->SetText("NEEDS CONFIG!");
//                            m_aOptionButton[P1CONFIG + (which - P1CLEAR)]->SetText("-> CONFIGURE <-");
                            m_ExitMenuSound.Play();
                        }
                    }
                }
            }

            //////////////////////////////////
            // Control config buttons

            if (m_MenuScreen == CONFIGSCREEN)
            {
			    // DPad Gamepad type selected
			    if (anEvent.GetControl() == m_pDPadTypeButton)
                {
                    m_ConfiguringGamepad = DPAD;
                    m_ConfigureStep++;
                    m_ScreenChange = true;

                    m_ButtonPressSound.Play();
                }

			    // DPad Gamepad type selected
			    if (anEvent.GetControl() == m_pDAnalogTypeButton)
                {
                    m_ConfiguringGamepad = DANALOG;
                    m_ConfigureStep++;
                    m_ScreenChange = true;

                    m_ButtonPressSound.Play();
                }

			    // XBox Gamepad type selected
			    if (anEvent.GetControl() == m_pXBox360TypeButton)
                {
/* Not allowing config, this is a complete preset
                    m_ConfiguringGamepad = XBOX360;
                    m_ConfigureStep++;
                    m_ScreenChange = true;
*/
                    // Set up the preset that will work well for a 360 controller
                    g_UInputMan.GetControlScheme(m_ConfiguringPlayer)->SetPreset(UInputMan::PRESET_XBOX360);

                    // Go back to the options screen immediately since the preset is all taken care of
                    m_apScreenBox[CONFIGSCREEN]->SetVisible(false);
                    m_MenuScreen = OPTIONSSCREEN;
                    m_ScreenChange = true;

                    m_ExitMenuSound.Play();
                }

			    // Skip ahead one config step button pressed
			    if (anEvent.GetControl() == m_pConfigSkipButton)
                {
// TODO: error checking so that we don't put configurestep out of bounds!
                    m_ConfigureStep++;
                    m_ScreenChange = true;

                    m_ButtonPressSound.Play();
                }

			    // Go back one config step button pressed
			    if (anEvent.GetControl() == m_pConfigBackButton)
                {
                    if (m_ConfigureStep > 0)
                    {
                        m_ConfigureStep--;
                        m_ScreenChange = true;
                        m_BackButtonPressSound.Play();
                    }
                    else
                        m_UserErrorSound.Play();
                }
            }

            /////////////////////////////////////////////
            // EDITOR SCREEN BUTTONS

			if (m_MenuScreen == EDITORSCREEN &&
                (anEvent.GetControl() == m_aEditorButton[SCENEEDITOR] ||
                 anEvent.GetControl() == m_aEditorButton[AREAEDITOR] ||
                 anEvent.GetControl() == m_aEditorButton[ASSEMBLYEDITOR] ||
                 anEvent.GetControl() == m_aEditorButton[GIBEDITOR] ||
                 anEvent.GetControl() == m_aEditorButton[ACTOREDITOR]))
            {
                // Hide all screens, the appropriate screen will reappear on next update
                HideAllScreens();
                m_MenuScreen = MAINSCREEN;
                m_ScreenChange = true;

                m_ActivityRestarted = true;
// TEMP ugly hack to get the damn main menu to hurry up when starting to edit
//                g_IntroState = 20;
//                set_palette(;

                // Create and start the appropriate editor Activity
                if (anEvent.GetControl() == m_aEditorButton[SCENEEDITOR])
                {
                    g_SceneMan.SetSceneToLoad("Editor Scene");
                    SceneEditor *pNewEditor = new SceneEditor;
                    pNewEditor->Create();
                    pNewEditor->SetEditorMode(EditorActivity::LOADDIALOG);
                    g_ActivityMan.SetStartActivity(pNewEditor);
                }
                else if (anEvent.GetControl() == m_aEditorButton[AREAEDITOR])
                {
                    g_SceneMan.SetSceneToLoad("Editor Scene");
                    AreaEditor *pNewEditor = new AreaEditor;
                    pNewEditor->Create();
                    pNewEditor->SetEditorMode(EditorActivity::LOADDIALOG);
                    g_ActivityMan.SetStartActivity(pNewEditor);
                }
                else if (anEvent.GetControl() == m_aEditorButton[ASSEMBLYEDITOR])
                {
                    g_SceneMan.SetSceneToLoad("Editor Scene");
                    AssemblyEditor *pNewEditor = new AssemblyEditor;
                    pNewEditor->Create();
                    pNewEditor->SetEditorMode(EditorActivity::LOADDIALOG);
                    g_ActivityMan.SetStartActivity(pNewEditor);
                }
                else if (anEvent.GetControl() == m_aEditorButton[GIBEDITOR])
                {
                    g_SceneMan.SetSceneToLoad("Editor Scene");
                    GibEditor *pNewEditor = new GibEditor;
                    pNewEditor->Create();
                    pNewEditor->SetEditorMode(EditorActivity::LOADDIALOG);
                    g_ActivityMan.SetStartActivity(pNewEditor);
                }
                else if (anEvent.GetControl() == m_aEditorButton[ACTOREDITOR])
                {
                    g_SceneMan.SetSceneToLoad("Editor Scene");
                    ActorEditor *pNewEditor = new ActorEditor;
                    pNewEditor->Create();
                    pNewEditor->SetEditorMode(EditorActivity::LOADDIALOG);
                    g_ActivityMan.SetStartActivity(pNewEditor);
                }

//                m_BackButtonPressSound.Play();
                m_ExitMenuSound.Play();
            }

            /////////////////////////////////////////////
            // STEAM WORKSHOP PUBLISHING SCREEN BUTTONS

            if (anEvent.GetControl() == m_aEditorButton[WORKSHOPPUBLISH])
            {
                // Hide all screens, the appropriate menu will reappear on next update
                HideAllScreens();
                m_MenuScreen = WORKSHOPMANAGER;
                m_ScreenChange = true;
            }

            if (m_MenuScreen == WORKSHOPMANAGER)
            {
                if (anEvent.GetControl() == m_pMainMenuButton)
                {
                    // Hide all screens, the appropriate menu will reappear on next update
                    HideAllScreens();
                    m_MenuScreen = MAINSCREEN;
                    m_ScreenChange = true;
                }
                else if (anEvent.GetControl() == m_pPublishNewButton)
                {
// TODO: Make a fresh publishing dialog, starting on step 1
                    m_PublishStep = PUBLISHSTEPFIRST;
                    m_UpdatingAlreadyPublished = false;
                    m_PublishingComplete = false;
                    // Hide all screens, the appropriate menu will reappear on next update
                    HideAllScreens();
                    m_MenuScreen = PUBLISHSTEPS;
                    m_ScreenChange = true;
                }
                else if (anEvent.GetControl() == m_pEditUpdateButton)
                {
// TODO: Take the selected item from the published list and start on step 2 of publishing
                    m_PublishStep = PUBLISHSTEPDESCRIPTION;
                    m_UpdatingAlreadyPublished = true;
                    m_PublishingComplete = false;
                    // Populate the fields with the previously published data
                    if (m_pPublishedList->GetSelectedItem())
                        PopulatePublishingWithPrevious(m_pPublishedList->GetSelectedItem()->m_Name);
                    // Hide all screens, the appropriate menu will reappear on next update
                    HideAllScreens();
                    m_MenuScreen = PUBLISHSTEPS;
                    m_ScreenChange = true;
                }
            }

            ///////////////////////////////////////////
            // PUBLISHING STEPS DIALOG

            if (m_MenuScreen == PUBLISHSTEPS)
            {
                // Cancel, going back to workshop manager
                if (anEvent.GetControl() == m_pPublishCancelButton)
                {
                    // Require re-click on cancel button to confirm
                    if (m_pPublishCancelButton->GetText() == "Cancel")
                        m_pPublishCancelButton->SetText("CONFIRM?");
                    // Ok, confirm clicked
                    else
                    {
                        // Hide all screens, the appropriate menu will reappear on next update
                        HideAllScreens();
                        m_MenuScreen = WORKSHOPMANAGER;
                        m_ScreenChange = true;
                    }
                }
                else if (anEvent.GetControl() == m_pPublishNextButton)
                {
                    // Proceed to the next step
                    if (m_PublishStep < PUBLISHSTEPLAST)
                    {
                        m_PublishStep++;
                        m_ScreenChange = true;
                    }

                    // If we progressed from the first step to the second, prepopulate if we have picked a module that was already published before
                    if (m_PublishStep == PUBLISHSTEPDESCRIPTION)
                    {
                        // Populate the fields with the previously published data
                        if (m_pUnpublishedList->GetSelectedItem())
                            PopulatePublishingWithPrevious(m_pUnpublishedList->GetSelectedItem()->m_Name);
                    }

                    // FIRST we must update the dialog to be on the last step, and then we can start displaying the publishing steps in there
                    m_PublishingComplete = false;
                    UpdatePublishingStepsDialog();
/*
                    // Done publishing, go back to workshop manager screen
                    if (m_PublishStep == PUBLISHSTEPLAST)
                    {
                        // Hide all screens, the appropriate menu will reappear on next update
                        HideAllScreens();
                        m_MenuScreen = WORKSHOPMANAGER;
                        m_ScreenChange = true;
                    }
*/
                    //////////////////////////////////////////////////////
                    // NOW PUBLISH, report into the publish status box
                    // If this is the actual real-deal publishing action, then collect all data from the various forms and get the upload started
                    if (m_PublishStep == PUBLISHSTEPLAST)
                        SetupAndSubmitPublishing();
                }
                else if (anEvent.GetControl() == m_pPublishBackButton)
                {
                    // Go back to previous step in publishing process
                    if (m_PublishStep > PUBLISHSTEPFIRST)
                    {
                        m_PublishStep--;
                        m_ScreenChange = true;
                    }
                    m_pPublishCancelButton->SetText("Cancel");
                    UpdatePublishingStepsDialog();
                }
                else if (anEvent.GetControl() == m_pLegalAgreementLink)
                {
                    // Open the license agreement page
#ifndef _DEBUG
                    OpenBrowserToURL("http://steamcommunity.com/sharedfiles/workshoplegalagreement");
#endif // _DEBUG
                    // Show the publish button now
                    m_pPublishNextButton->SetVisible(true);
                }
                else if (anEvent.GetControl() == m_pAppLinkButton)
                {
                    // Open the newly published-to workshop app page
                    OpenBrowserToURL("http://steamcommunity.com/sharedfiles/filedetails/?id=" + m_PublishedAppIDString);
                    // Hide all screens, the appropriate menu will reappear on next update
                    HideAllScreens();
                    m_MenuScreen = WORKSHOPMANAGER;
                    m_ScreenChange = true;
                }
            }

            /////////////////////////////////////////////
            // LICENSE REGISTRATION SCREEN BUTTONS

			if (m_MenuScreen == LICENSESCREEN)
            {
                // Register/Unregister button pressed
                if (anEvent.GetControl() == m_pRegistrationButton)
                {
                    // If no license yet, then try to register the one in the key field
                    if (!g_LicenseMan.HasValidatedLicense())
                    {
                        // Attempt the registration
                        LicenseMan::ServerResult result = g_LicenseMan.Register(m_pLicenseEmailBox->GetText(), m_pLicenseKeyBox->GetText());

                        // Registration was success!
                        if (result == LicenseMan::SUCCESS)
                        {
							Writer writer("Base.rte/Settings.ini");
                            // Write the ew key to the hidden data file right away
                            g_SettingsMan.WriteLicenseKey();
                            // Also write out the settings, because the new license email needs to be stored there
                            g_SettingsMan.Save(writer);
                            // Update to show the registration status
                            UpdateLicenseScreen();
                            // Show success message
                            m_pLicenseCaptionLabel->SetText("T H A N K   Y O U !");
                            m_pLicenseInstructionLabel->SetText("You have registered this copy of Cortex Command!\nYour support enables future development of Data Realms' games.\n\nWrite down and save your key - it is what you paid for:\n" + g_LicenseMan.GetLicenseKey() + "\nAlso remember which email address you used!");

// TODO: play some glorious sound to reward registration!
                            m_ButtonPressSound.Play();
                        }
                        // Registration was success, but with old key!
                        else if (result == LicenseMan::DEPRECATEDKEY)
                        {
							Writer writer("Base.rte/Settings.ini");
                            // Write the ew key to the hidden data file right away
                            g_SettingsMan.WriteLicenseKey();
                            // Also write out the settings, because the new license email needs to be stored there
                            g_SettingsMan.Save(writer);
                            // Update to show the registration status
                            UpdateLicenseScreen();
                            // Show success message
                            m_pLicenseCaptionLabel->SetText("T H A N K   Y O U   -   B U T   U P D A T E   Y O U R   K E Y !");
                            m_pLicenseInstructionLabel->SetText("You have successfully registered, but with an outdated key!\nIf you want to be able to play offline, update your key for FREE at:\nHTTP://LICENSING.DATAREALMS.COM\n\nWrite down your current key - you will need it to update:\n" + g_LicenseMan.GetLicenseKey());

// TODO: play some glorious sound to reward registration!
                            m_ButtonPressSound.Play();
                        }
                        // Error handling when registration fails
                        else
                        {
                            if (result == LicenseMan::INVALIDKEY)
                            {
                                m_pLicenseCaptionLabel->SetText("I N V A L I D   K E Y !");
                                m_pLicenseInstructionLabel->SetText("You seem to have entered an invalid registration key.\nPlease double-check it and try again.\nIf still trouble, email SUPPORT@DATAREALMS.COM for help!");
                            }
                            else if (result == LicenseMan::INVALIDEMAIL)
                            {
                                m_pLicenseCaptionLabel->SetText("I N V A L I D   E M A I L !");
                                m_pLicenseInstructionLabel->SetText("You have entered an invalid email address.\nPlease double-check it and try again.\nIf still trouble, email SUPPORT@DATAREALMS.COM for help!");
                            }
                            else if (result == LicenseMan::EMAILMISMATCH)
                            {
                                m_pLicenseCaptionLabel->SetText("E M A I L   M I S M A T C H !");
                                m_pLicenseInstructionLabel->SetText("You have entered an email address that doesn't match your key.\nWe need the billing email address used when buying your key.\nIf still trouble, email SUPPORT@DATAREALMS.COM for help!");
                            }
                            else if (result == LicenseMan::INVALIDMACHINE || result == LicenseMan::MAXCOUNT)
                            {
                                // Hide the register and key text fields, no point in actually try again anyway, and need the space for the explanation text
                                m_pLicenseEmailLabel->SetVisible(false);
                                m_pLicenseKeyLabel->SetVisible(false);
                                m_pLicenseEmailBox->SetVisible(false);
                                m_pLicenseKeyBox->SetVisible(false);
                                m_pRegistrationButton->SetVisible(false);
                                m_pLicenseCaptionLabel->SetText("T O O   M A N Y   R E G I S T R A T I O N S !");
                                m_pLicenseInstructionLabel->SetText("You can only have one copy registered at a time with each key.\nFirst un-register your old installation and then try again here,\nor buy another key to register both installations.\n\nIf you can't un-register or you haven't used this key before,\ngo to HTTP://LICENSING.DATAREALMS.COM for help!");
                            }
                            else if (result == LicenseMan::FAILEDCONNECTION || result == LicenseMan::INVALIDXML)
                            {
                                m_pLicenseCaptionLabel->SetText("F A I L E D   C O N N E C T I O N !");
                                m_pLicenseInstructionLabel->SetText("Could not contact the license server to register.\nMake sure you are connected to the internet and try again!\nIf still trouble, email SUPPORT@DATAREALMS.COM for help.");
                            }
                            else
                            {
                                m_pLicenseCaptionLabel->SetText("U N K N O W N   E R R O R !");
                                m_pLicenseInstructionLabel->SetText("An unknown error happened while registering.\nTry again later or contact SUPPORT@DATAREALMS.COM for help!");
                            }

                            m_UserErrorSound.Play();
                        }
                    }
                    // Already registered, so try to UN-register
                    else
                    {
                        // Attempt the un-registration
                        LicenseMan::ServerResult result = g_LicenseMan.Unregister();

                        // Unregistration succeeded
                        if (result == LicenseMan::SUCCESS)
                        {
							Writer writer("Base.rte/Settings.ini");
                            // Write the new null key to the hidden data file right away
                            g_SettingsMan.WriteLicenseKey();
                            // Also write out the settings, because the new license email needs to be stored there
                            g_SettingsMan.Save(writer);
                            // Update to show the registration status
                            UpdateLicenseScreen();
                            // Show success message
                            m_pLicenseCaptionLabel->SetText("K E Y   U N - R E G I S T E R E D !");
                            m_pLicenseInstructionLabel->SetText("You have released your license key from this game copy.\nThe key can now be used to register a copy on another computer.\nWrite it down and keep it safe!");

// TODO: play some more approriate locking sound
                            m_ButtonPressSound.Play();
                        }
                        // Error handle the failed un-registration attempt
                        else
                        {
                            if (result == LicenseMan::INVALIDMACHINE)
                            {
                                m_pLicenseCaptionLabel->SetText("D I F F E R E N T   C O M P U T E R !");
                                m_pLicenseInstructionLabel->SetText("You can only un-register your key on the same computer\nas it was originally registered.\nFirst un-register the older installation and try again here.\n\nIf you still have trouble, email SUPPORT@DATAREALMS.COM for help.");
                            }
                            else if (result == LicenseMan::FAILEDCONNECTION || result == LicenseMan::INVALIDXML)
                            {
                                m_pLicenseCaptionLabel->SetText("F A I L E D   C O N N E C T I O N !");
                                m_pLicenseInstructionLabel->SetText("Could not contact the license server to un-register.\nMake sure you are connected to the internet and try again!\n\nIf still trouble, email SUPPORT@DATAREALMS.COM for help.");
                            }
                            else
                            {
                                m_pLicenseCaptionLabel->SetText("U N K N O W N   E R R O R !");
                                m_pLicenseInstructionLabel->SetText("An unknown error happened while un-registering.\nTry again later or contact SUPPORT@DATAREALMS.COM for help!");
                            }

                            m_UserErrorSound.Play();
                        }
                    }
                }
            }

            /////////////////////////////////////////////
            // META NOTICE SCREEN BUTTONS

			if (m_MenuScreen == METASCREEN)
            {
                // Play tutorial button pressed
                if (anEvent.GetControl() == m_aMainMenuButton[PLAYTUTORIAL])
                {
                    // Hide all screens, the appropriate screen will reappear on next update
                    HideAllScreens();
                    m_MenuScreen = MAINSCREEN;
                    m_ScreenChange = true;

                    // Set up and start the tutorial!
                    g_ActivityMan.SetStartActivity(dynamic_cast<Activity *>(g_PresetMan.GetEntityPreset("GATutorial", "Tutorial Mission")->Clone()));
					GameActivity * pGameActivity = dynamic_cast<GameActivity *>(g_ActivityMan.GetStartActivity());
					if (pGameActivity)
						pGameActivity->SetStartingGold(10000);
                    g_SceneMan.SetSceneToLoad("Tutorial Bunker");
                    m_ActivityRestarted = true;

                    m_ButtonPressSound.Play();
                }
                // Go to registration dialog button
                else if (anEvent.GetControl() == m_aMainMenuButton[METACONTINUE])
                {
                    m_CampaignStarted = true;

                    // Hide all screens, and stay in main menu for if/when player comes back to the main menu
                    HideAllScreens();
                    m_MenuScreen = MAINSCREEN;
                    m_ScreenChange = true;
                    m_ButtonPressSound.Play();
                }
            }

            /////////////////////////////////////////////
            // NAG SCREEN BUTTONS

			if (m_MenuScreen == NAGSCREEN)
            {
                // Quit the nag screen button pressed
                if (anEvent.GetControl() == m_pNagExitButton)
                {
                    // Quit the app if this is the quitting nag screen
                    if (m_NagMode == QUITNAG)
                        m_Quit = true;
                    // Otherwise just go back to the main menu
                    else
                    {
                        // Hide all screens, the appropriate screen will reappear on next update
                        HideAllScreens();
    // TODO: Change this to be an exit state
                        m_MenuScreen = MAINSCREEN;
                        m_ScreenChange = true;
                    }
                    m_ButtonPressSound.Play();
                }
                // Go to registration dialog button
                else if (anEvent.GetControl() == m_pNagRegButton)
                {
                    // Hide all screens, the appropriate screen will reappear on next update
                    HideAllScreens();
// TODO: Change this to be an exit state
                    m_MenuScreen = LICENSESCREEN;
                    m_ScreenChange = true;

                    m_ButtonPressSound.Play();
                }
            }

			if (m_MenuScreen == MODMANAGERSCREEN)
            {
				// Return to main menu button pressed
				if (anEvent.GetControl() == m_pModManagerBackButton)
				{
					// Save settings
					Writer writer("Base.rte/Settings.ini");
					g_SettingsMan.Save(writer);

					// Hide all screens, the appropriate screen will reappear on next update
					HideAllScreens();
					m_MenuScreen = MAINSCREEN;
					m_ScreenChange = true;
					m_BackButtonPressSound.Play();
				}

				// Disable/Enable mod pressed
				if (anEvent.GetControl() == m_pModManagerToggleModButton)
				{
					ToggleMod();
				}

				// Disable/Enable script pressed
				if (anEvent.GetControl() == m_pModManagerToggleScriptButton)
				{
					ToggleScript();
				}
			}

            /////////////////////////////////////////////
            // QUIT SCREEN BUTTONS

			if (m_MenuScreen == QUITSCREEN)
            {
                // Confirm quitting of game
                if (anEvent.GetControl() == m_aMainMenuButton[QUITCONFIRM])
                {
                    // If registered, quit immediately, if not, go to the nag screen!
                    // Also, if we have a last entered key, don't nag anymore
				    std::string lastLicenseKey = g_LicenseMan.GetLastLicenseKey();
                    if (g_LicenseMan.HasValidatedLicense() || g_LicenseMan.CheckKeyFormatting(lastLicenseKey))
                        m_Quit = true;
                    else
                    {
                        // Hide all screens, the appropriate screen will reappear on next update
                        HideAllScreens();
                        m_NagMode = QUITNAG;
                        m_MenuScreen = NAGSCREEN;
                        m_ScreenChange = true;
                    }

                    m_ButtonPressSound.Play();
                }
                // Cancel quitting
                else if (anEvent.GetControl() == m_aMainMenuButton[QUITCANCEL])
                {
                    // Hide all screens, the appropriate screen will reappear on next update
                    HideAllScreens();
                    m_MenuScreen = MAINSCREEN;
                    m_ScreenChange = true;

                    m_ButtonPressSound.Play();
                }
            }
        }

		// Notifications
		else if (anEvent.GetType() == GUIEvent::Notification)
        {
            // Button focus notification that we can play a sound to
            if (dynamic_cast<GUIButton *>(anEvent.GetControl()))
            {
                if (anEvent.GetMsg() == GUIButton::Focused)
                    m_SelectionChangeSound.Play();
            }

			// Mod list pressed
			if (anEvent.GetControl() == m_pModManagerModsListBox)
			{
				if (anEvent.GetMsg() == GUIListBox::Select)
				{
					int index = m_pModManagerModsListBox->GetSelectedIndex();
					if (index > -1)
					{
						GUIListPanel::Item *selectedItem = m_pModManagerModsListBox->GetSelected();
						ModRecord r = m_KnownMods.at(selectedItem->m_ExtraIndex);
						m_pModManagerDescriptionLabel->SetText(r.Description);

						if (r.Disabled)
							m_pModManagerToggleModButton->SetText("Enable");
						else
							m_pModManagerToggleModButton->SetText("Disable");
					}
				}

				// Not reliable at all!!
				/*if (anEvent.GetMsg() == GUIListBox::DoubleClick)
				{
					ToggleMod();
				}*/
			}

			// Script list pressed
			if (anEvent.GetControl() == m_pModManagerScriptsListBox)
			{
				if (anEvent.GetMsg() == GUIListBox::Select)
				{
					int index = m_pModManagerScriptsListBox->GetSelectedIndex();
					if (index > -1)
					{
						GUIListPanel::Item *selectedItem = m_pModManagerScriptsListBox->GetSelected();
						ScriptRecord r = m_KnownScripts.at(selectedItem->m_ExtraIndex);
						m_pModManagerDescriptionLabel->SetText(r.Description);

						if (r.Enabled)
							m_pModManagerToggleScriptButton->SetText("Disable");
						else
							m_pModManagerToggleScriptButton->SetText("Enable");
					}
				}

				/*if (anEvent.GetMsg() == GUIListBox::DoubleClick)
				{
					ToggleScript();
				}*/
			}

            // Resolution combobox closed, something new selected
    		if (anEvent.GetControl() == m_pResolutionCombo)
            {
                // Closed it, IE selected somehting
                if(anEvent.GetMsg() == GUIComboBox::Closed)
                {
                    // Get and read the new resolution data from the item's label
                    GUIListPanel::Item *pResItem = m_pResolutionCombo->GetItem(m_pResolutionCombo->GetSelectedIndex());
                    if (pResItem && !pResItem->m_Name.empty())
                    {
                        int newResX = g_FrameMan.GetResX();
                        int newResY = g_FrameMan.GetResY();
                        sscanf(pResItem->m_Name.c_str(), "%4dx%4d", &newResX, &newResY);
                        // Sanity check the values and then set them as the new resolution to be switched to next time FrameMan is created
                        /*if (newResX >= 400 && newResX < 3000 && newResY >= 400 && newResY < 3000)*/
						if (g_FrameMan.IsValidResolution(newResX, newResY))
                        {
                            // Force double virtual fullscreen res if the res is too high
                            if (newResX >= 1280)
                                g_FrameMan.SetNewNxFullscreen(2);
                            // Not oversized resolution
                            else
                                g_FrameMan.SetNewNxFullscreen(1);

                            g_FrameMan.SetNewResX(newResX /= g_FrameMan.GetNewNxFullscreen());
                            g_FrameMan.SetNewResY(newResY /= g_FrameMan.GetNewNxFullscreen());
                        }
                    }

                    // Update the resolution restart notice
                    m_pResolutionNoticeLabel->SetVisible(g_FrameMan.IsNewResSet());
                }
            }

			// Sound Volume slider changed
			if(anEvent.GetControl() == m_pSoundSlider)
            {
                // See if we should play test sound after the volume has been set
                bool playTest = false;
                if (((double)m_pSoundSlider->GetValue() / 100) != g_AudioMan.GetSoundsVolume() && !g_AudioMan.IsPlaying(&m_TestSound))
                    playTest = true;

                g_AudioMan.SetSoundsVolume((double)m_pSoundSlider->GetValue() / 100);
                UpdateVolumeSliders();

                // Play test sound after new volume is set
                if (playTest)
                    m_TestSound.Play();
			}

			// Music Volume slider changed
			if(anEvent.GetControl() == m_pMusicSlider)
            {
                g_AudioMan.SetMusicVolume((double)m_pMusicSlider->GetValue() / 100);
                UpdateVolumeSliders();
			}

			// Dead zone sliders control
			for (int which = P1DEADZONESLIDER; which < DEADZONESLIDERCOUNT; ++which)
			{
				// Handle the appropriate player's clearing of mappings
				if (anEvent.GetControl() == m_aDeadZoneSlider[which])
				{
					// Display value
					char s[256];
					sprintf(s, "%d", m_aDeadZoneSlider[which]->GetValue());
					m_aDeadZoneLabel[which]->SetText(s);

					// Update control scheme
					g_UInputMan.GetControlScheme(which)->SetJoystickDeadzone((float)m_aDeadZoneSlider[which]->GetValue() / 250.0);
				}

				if (anEvent.GetControl() == m_aDeadZoneCheckbox[which])
				{
					if (m_aDeadZoneCheckbox[which]->GetCheck() == 1)
					{
						g_UInputMan.GetControlScheme(which)->SetJoystickDeadzoneType(UInputMan::DeadZoneType::CIRCLE);
						m_aDeadZoneCheckbox[which]->SetText("O");
					}
					else 
					{
						g_UInputMan.GetControlScheme(which)->SetJoystickDeadzoneType(UInputMan::DeadZoneType::SQUARE);
						char str[2];
						str[0] = -2;
						str[1] = 0;
						m_aDeadZoneCheckbox[which]->SetText(str);
					}
				}
			}
/*
			// Scrollbar changed
			if(anEvent.GetControl()->GetName() == "scroll1")
            {
				int Value = 0;
				GUIScrollbar *S = (GUIScrollbar *)anEvent.GetControl();
				Value = S->GetValue();
				GUILabel *L = (GUILabel *)m_pGUIController->GetControl("label1");
				char buf[64];
				L->SetText(itoa(Value, buf, 10));
			}

			// Double click on the listbox
			if(anEvent.GetControl()->GetName() == "list1")
            {
				if(anEvent.GetMsg() == GUIListBox::DoubleClicked)
                {
					GUILabel *L = (GUILabel *)m_pGUIController->GetControl("label1");
					GUIListBox *LB = (GUIListBox *)anEvent.GetControl();
					if(LB->GetSelected())
						L->SetText(LB->GetSelected()->m_Name);
				}

				if(anEvent.GetMsg() == GUIListBox::KeyDown)
                {
					// Delete
					if(anEvent.GetData() == GUIInput::Key_Delete)
                    {
						GUIListBox *LB = (GUIListBox *)anEvent.GetControl();
						LB->DeleteItem(LB->GetSelectedIndex());
					}
				}
			}

			if(anEvent.GetControl()->GetName() == "text1")
            {
				if(anEvent.GetMsg() == GUITextBox::Enter)
                {
					GUILabel *L = (GUILabel *)m_pGUIController->GetControl("label1");
					GUITextBox *T = (GUITextBox *)anEvent.GetControl();

					L->SetText(T->GetText());
				}
			}
*/
        }
    }

#ifndef __OPEN_SOURCE_EDITION
    CRYPT_END
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the menu

void MainMenuGUI::Draw(BITMAP *drawBitmap) const
{
    AllegroScreen drawScreen(drawBitmap);
    m_pGUIController->Draw(&drawScreen);
    m_pGUIController->DrawMouse();

	// Show who controls the cursor
	int device = g_UInputMan.GetLastDeviceWhichControlledGUICursor();

	if (device >= UInputMan::DEVICE_GAMEPAD_1)
	{
		int mouseX, mouseY;
		m_pGUIInput->GetMousePosition(&mouseX, &mouseY);

		const Icon * pIcon = g_UInputMan.GetDeviceIcon(device);
		if (pIcon)
			draw_sprite(drawBitmap, pIcon->GetBitmaps8()[0], mouseX + 16, mouseY - 4);

/*#ifdef _DEBUG
		if (g_UInputMan.JoystickActive(0))
		{
			Vector aim = g_UInputMan.AnalogAimValues(0);
			float axis00 = g_UInputMan.AnalogAxisValue(0, 0, 0);
			float axis01 = g_UInputMan.AnalogAxisValue(0, 0, 1);

			float axis10 = g_UInputMan.AnalogAxisValue(0, 1, 0);
			float axis11 = g_UInputMan.AnalogAxisValue(0, 1, 1);
			char s[256];
			sprintf(s, "Aim %.1f %.1f - Stick 0 %.1f %.1f - Stick 1 %.1f %.1f", aim.GetX(), aim.GetY(), axis00, axis01, axis10, axis11);

			GUILabel * debugLabel = dynamic_cast<GUILabel *>(m_pGUIController->GetControl("LabelDebug"));
			if (debugLabel)
				debugLabel->SetText(s);
		}
#endif*/

	}

	// Show which joysticks are detected by the game
	for (int joy = 0; joy < UInputMan::MAX_PLAYERS; joy++)
	{
		if (g_UInputMan.JoystickActive(joy))
		{
			int matchedDevice = UInputMan::DEVICE_GAMEPAD_1 + joy;

			if (matchedDevice != device)
			{
				const Icon * pIcon = g_UInputMan.GetDeviceIcon(matchedDevice);
				if (pIcon)
					draw_sprite(drawBitmap, pIcon->GetBitmaps8()[0], g_FrameMan.GetResX() - 30 * g_UInputMan.GetJoystickCount() + 30 * joy, g_FrameMan.GetResY() - 25);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HideAllScreens
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Hides all menu screens, so one can easily be unhidden and shown only.

void MainMenuGUI::HideAllScreens()
{
    for (int iscreen = 0; iscreen < SCREENCOUNT; ++iscreen)
    {
        if (m_apScreenBox[iscreen])
            m_apScreenBox[iscreen]->SetVisible(false);
    }
    m_ScreenChange = true;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          QuitLogic
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Handles quitting of the game, whether to show nag screen, etc.

void MainMenuGUI::QuitLogic()
{
    // If quit confirm dialog not already showing, or an activity is running, show it
    if (m_MenuScreen != QUITSCREEN && g_ActivityMan.GetActivity() && (g_ActivityMan.GetActivity()->GetActivityState() == Activity::RUNNING || g_ActivityMan.GetActivity()->GetActivityState() == Activity::EDITING))
    {
        HideAllScreens();
        m_MenuScreen = QUITSCREEN;
        m_ScreenChange = true;
    }
    // No activity, so just start quitting
    else
    {
        // If registered, quit immediately, if not, go to the nag screen!
        // Also, if we have a last entered key, don't nag anymore
	    std::string lastLicenseKey = g_LicenseMan.GetLastLicenseKey();
        if (g_LicenseMan.HasValidatedLicense() || g_LicenseMan.CheckKeyFormatting(lastLicenseKey))
            m_Quit = true;
        else
        {
            // Hide all screens, the appropriate screen will reappear on next update
            HideAllScreens();
            m_NagMode = QUITNAG;
            m_MenuScreen = NAGSCREEN;
            m_ScreenChange = true;
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetupSkirmishActivity
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the ActivityMan up with the current data for a skirmish game.

void MainMenuGUI::SetupSkirmishActivity()
{
#ifndef __OPEN_SOURCE_EDITION
    CRYPT_START
#endif

    // If activity restarted, stuff the ActivityMan with the selected data
    if (m_ActivityRestarted)
    {
// TODO: ******* add the game mode drop down and base the game mode selection off that instead

        // No CPU team, so Brain match
        if (m_CPUTeam == Activity::NOTEAM)
        {
            g_SceneMan.SetSceneToLoad(m_pSceneSelector->GetItem(m_pSceneSelector->GetSelectedIndex())->m_Name);
// TODO: Let player choose the GABrainMatch activity instance!
            GABrainMatch *pNewGame = new GABrainMatch;
            pNewGame->SetPlayerCount(m_StartPlayers);
            pNewGame->SetTeamCount(m_StartTeams);

            for (int player = 0; player < m_StartPlayers; ++player)
                pNewGame->SetTeamOfPlayer(player, m_aTeamAssignments[player]);

            pNewGame->SetCPUTeam(m_CPUTeam);
            pNewGame->Create();
            g_ActivityMan.SetStartActivity(pNewGame);
        }
        // CPU present, so base defense
        else
        {
            g_SceneMan.SetSceneToLoad(m_pSceneSelector->GetItem(m_pSceneSelector->GetSelectedIndex())->m_Name);
// TODO: Let player choose the GABaseDefense activity instance!
            GABaseDefense *pNewGame = dynamic_cast<GABaseDefense *>(g_PresetMan.GetEntityPreset("GABaseDefense", "Skirmish Defense")->Clone());
            AAssert(pNewGame, "Couldn't find the \"Skirmish Defense\" GABaseDefense Activity! Has it been defined?");
            pNewGame->SetPlayerCount(m_StartPlayers);
            pNewGame->SetTeamCount(m_StartTeams);

            for (int player = 0; player < m_StartPlayers; ++player)
                pNewGame->SetTeamOfPlayer(player, m_aTeamAssignments[player]);

            pNewGame->SetCPUTeam(m_CPUTeam);
            pNewGame->SetDifficulty(m_StartDifficulty);

            pNewGame->Create();
            g_ActivityMan.SetStartActivity(pNewGame);
        }

// TODO: Reenable and make GUI control for this!
/*
        g_ActivityMan.GetActivity()->SetStartingFunds(m_StartFunds);
        for (int team = 0; team < m_StartTeams; ++team)
        {
            g_ActivityMan.GetActivity()->SetTeamFunds(m_StartFunds, team);
        }
*/
    }

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
// Method:          UpdateScenesBox
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the contents of the scene selection box.

void MainMenuGUI::UpdateScenesBox()
{
    // Clear out the control
    m_pSceneSelector->ClearList();

    // Get the list of all read in scenes
    list<Entity *> sceneList;
    g_PresetMan.GetAllOfType(sceneList, "Scene");

    // Go through the list and add their names to the combo box
    for (list<Entity *>::iterator itr = sceneList.begin(); itr != sceneList.end(); ++itr)
    {
        m_pSceneSelector->AddItem((*itr)->GetPresetName());
    }

    // Select the first one
    m_pSceneSelector->SetSelectedIndex(0);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateTeamBoxes
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the size and contents of the team assignment boxes, according
//                  to the number of players chosen.

void MainMenuGUI::UpdateTeamBoxes()
{
    char str[128];

    // Hide all team assignment panels initially, and center their contents
    for (int box = 0; box < SKIRMISHPLAYERCOUNT; ++box)
    {
        m_aSkirmishBox[box]->SetVisible(false);
        m_aSkirmishButton[box]->CenterInParent(true, true);
    }

    // Total area size
    int areaWidth = m_pTeamBox->GetWidth();
    int areaHeight = m_pTeamBox->GetHeight();

    // Set up the team assignment boxes and contained labels and buttons
    // Single team to set up for single player
    if (m_StartPlayers == 1)
    {
        // Show and resize
        m_aSkirmishBox[P1TEAM]->Resize(areaWidth, areaHeight);
        m_aSkirmishButton[P1TEAM]->CenterInParent(true, true);
        m_aSkirmishBox[P1TEAM]->SetVisible(true);
    }
    // Two player split one above the other
    else if (m_StartPlayers == 2)
    {
        int boxHeight = (areaHeight - 4) / 2;

        // Player 1
        m_aSkirmishBox[P1TEAM]->Resize(areaWidth, boxHeight);
        m_aSkirmishButton[P1TEAM]->CenterInParent(true, true);
        m_aSkirmishBox[P1TEAM]->SetVisible(true);

        // Player 2
        m_aSkirmishBox[P2TEAM]->Resize(areaWidth, boxHeight);
        m_aSkirmishBox[P2TEAM]->SetPositionRel(0, boxHeight + 4);
        m_aSkirmishButton[P2TEAM]->CenterInParent(true, true);
        m_aSkirmishBox[P2TEAM]->SetVisible(true);
    }
    // Four-way split, either three or four players
    else if (m_StartPlayers >= 3)
    {
        int boxWidth = (areaWidth - 4) / 2;
        int boxHeight = (areaHeight - 4) / 2;

        // Player 1
        m_aSkirmishBox[P1TEAM]->Resize(boxWidth, boxHeight);
        m_aSkirmishButton[P1TEAM]->CenterInParent(true, true);
        m_aSkirmishBox[P1TEAM]->SetVisible(true);

        // Player 2
        m_aSkirmishBox[P2TEAM]->Resize(boxWidth, boxHeight);
        m_aSkirmishBox[P2TEAM]->SetPositionRel(boxWidth + 4, 0);
        m_aSkirmishButton[P2TEAM]->CenterInParent(true, true);
        m_aSkirmishBox[P2TEAM]->SetVisible(true);

        // Player 3
        m_aSkirmishBox[P3TEAM]->SetVisible(true);

        // Player 4
        if (m_StartPlayers == 4)
            m_aSkirmishBox[P4TEAM]->SetVisible(true);
    }

    // Update button labels
    for (int player = 0; player < SKIRMISHPLAYERCOUNT; ++player)
    {
        if (m_aTeamAssignments[player] == Activity::TEAM_1)
        {
            m_aSkirmishBox[player]->SetDrawColor(makecol(70, 27, 12));
            sprintf(str, "Player %i: %c", player + 1, -62);
        }
        else
        {
            m_aSkirmishBox[player]->SetDrawColor(makecol(47, 55, 40));
            sprintf(str, "Player %i: %c", player + 1, -59);
        }
        m_aSkirmishButton[player]->SetText(str);
    }

    ////////////////////////////////
    // Update CPU team label

    // Count how many players on each team
    int team0Count = 0;
    int team1Count = 0;
    for (int player = 0; player < m_StartPlayers; ++player)
    {
        if (m_aTeamAssignments[player] == 0)
            team0Count++;
        else if (m_aTeamAssignments[player] == 1)
            team1Count++;
    }

    // See if either team is empty of human players - that becomes the CPU team
    if (team0Count == 0 || team1Count == 0)
    {
        sprintf(str, "CPU Team: %c", team0Count == 0 ? -62 : -59);
        m_CPUTeam = team0Count == 0 ? 0 : 1;
    }
    else
    {
        sprintf(str, "No CPU Team (both have players)");
        m_CPUTeam = -1;
    }

    // Finally set the label
    m_pCPUTeamLabel->SetText(str);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateResolutionCombo
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the contents of the screen resolution combo box

void MainMenuGUI::UpdateResolutionCombo()
{
    // Refill possible resolutions
    m_pResolutionCombo->SetText("");
    m_pResolutionCombo->ClearList();
	
    // Only refill possible resolutions if empty
    if (m_pResolutionCombo->GetCount() <= 0)
    {
#if defined(__APPLE__)
		GFX_MODE_LIST* pList = get_gfx_mode_list(g_FrameMan.IsFullscreen() ? GFX_QUARTZ_FULLSCREEN : GFX_QUARTZ_WINDOW);
#elif defined(__unix__)
		GFX_MODE_LIST* pList = get_gfx_mode_list(g_FrameMan.IsFullscreen() ? GFX_XWINDOWS_FULLSCREEN : GFX_XWINDOWS);
#else
        GFX_MODE_LIST *pList = get_gfx_mode_list(GFX_DIRECTX_ACCEL);
#endif // defined(__APPLE__)

        int width = 0;
        int height = 0;
        char resString[256] = "";
        // Index of found useful resolution (32bit)
        int foundIndex = 0;
        // The saved index of the entry that has the current resolution setting
        int currentResIndex = -1;

        // Process and annotate the list
        for (int i = 0; pList && i < pList->num_modes; ++i)
        {
            // Only list 32 bpp modes
            if (pList->mode[i].bpp == 32)
            {
                width = pList->mode[i].width;
                height = pList->mode[i].height;

				// Resolutions must be multiples of 4 or we'll get 'Overlays not supported' during GFX mode init
				if (g_FrameMan.IsValidResolution(width, height) && width % 4 == 0)
				{
					// Fix wacky resolutions that are taller than wide
					if (height > width)
					{
						height = pList->mode[i].width;
						width = pList->mode[i].height;
					}

					// Try to figure the max available resotion
					if (width > m_MaxResX)
					{
						m_MaxResX = width;
						m_MaxResY = height;
					}

					// Construct and add the resolution string to the combobox
					sprintf(resString, "%ix%i", width, height);

					// Add useful notation to the standardized resolutions
					if (width == 320 && height == 200)
						strcat(resString, " CGA");
					if (width == 320 && height == 240)
						strcat(resString, " QVGA");
					if (width == 640 && height == 480)
						strcat(resString, " VGA");
					if (width == 720 && height == 480)
						strcat(resString, " NTSC");
					if (width == 768 && height == 576)
						strcat(resString, " PAL");
					if ((width == 800 || height == 854) && height == 480)
						strcat(resString, " WVGA");
					if (width == 800 && height == 600)
						strcat(resString, " SVGA");
					if (width == 1024 && height == 600)
						strcat(resString, " WSVGA");
					if (width == 1024 && height == 768)
						strcat(resString, " XGA");
					if (width == 1280 && height == 720)
						strcat(resString, " HD720");
					if (width == 1280 && (height == 768 || height == 800))
						strcat(resString, " WXGA");
	// These below are forced to be done in 2X pixels fullscreen
					if (width == 1280 && height == 1024)
						strcat(resString, " SXGA");
					if (width == 1400 && height == 1050)
						strcat(resString, " SXGA+");
					if (width == 1600 && height == 1200)
						strcat(resString, " UGA");
					if (width == 1680 && height == 1050)
						strcat(resString, " WSXGA+");
					if (width == 1920 && height == 1080)
						strcat(resString, " HD1080");
					if (width == 1920 && height == 1200)
						strcat(resString, " WUXGA");
					if (width == 2048 && height == 1080)
						strcat(resString, " 2K");

					m_pResolutionCombo->AddItem(resString);

					// If this is what we're currently set to have at next start, select it afterward
					if ((g_FrameMan.GetNewResX() * g_FrameMan.GetNewNxFullscreen()) == width && (g_FrameMan.GetNewResY() * g_FrameMan.GetNewNxFullscreen()) == height)
						currentResIndex = foundIndex;

					// Only increment this when we find a usable 32bit resolution
					foundIndex++;
				}
            }
        }

        // Get rid of the mode list, we're done with it
		if (pList)
		{
			destroy_gfx_mode_list(pList);
		}
		
        // If none of the listed matched our resolution set for next start, add a 'custom' one to display as the current res
        if (currentResIndex < 0)
        {
            sprintf(resString, "%ix%i Custom", g_FrameMan.GetNewResX() * g_FrameMan.GetNewNxFullscreen(), g_FrameMan.GetNewResY() * g_FrameMan.GetNewNxFullscreen());
            m_pResolutionCombo->AddItem(resString);
            currentResIndex = m_pResolutionCombo->GetCount() - 1;
        }

        // Show the current resolution item to be the selected one
        m_pResolutionCombo->SetSelectedIndex(currentResIndex);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateVolumeSliders
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the position of the volume sliders, based on what the AudioMan
//                  is currently set to.

void MainMenuGUI::UpdateVolumeSliders()
{
    char labelText[512];
    int volume = (int)(g_AudioMan.GetSoundsVolume() * 100);
    sprintf(labelText, "Sound Volume: %i", volume);
    m_pSoundLabel->SetText(labelText);
    m_pSoundSlider->SetValue(volume);

    volume = (int)(g_AudioMan.GetMusicVolume() * 100);
    sprintf(labelText, "Music Volume: %i", volume);
    m_pMusicLabel->SetText(labelText);
    m_pMusicSlider->SetValue(volume);
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateDeviceLabels
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the text on the config buttons, based on what they are mapped as.

void MainMenuGUI::UpdateDeviceLabels()
{
    int device = 0;
    string label;

    // Cycle through all players
    for (int player = 0; player < UInputMan::MAX_PLAYERS; ++player)
    {
        device = g_UInputMan.GetControlScheme(player)->GetDevice();

        if (device == UInputMan::DEVICE_KEYB_ONLY)
            label = "Classic Keyb";
        else if (device == UInputMan::DEVICE_MOUSE_KEYB)
            label = "Keyb + Mouse";
        else if (device == UInputMan::DEVICE_GAMEPAD_1)
            label = "Gamepad 1";
        else if (device == UInputMan::DEVICE_GAMEPAD_2)
            label = "Gamepad 2";
        else if (device == UInputMan::DEVICE_GAMEPAD_3)
            label = "Gamepad 3";
        else if (device == UInputMan::DEVICE_GAMEPAD_4)
            label = "Gamepad 4";

        // Set the label
        m_aOptionsLabel[P1DEVICE + player]->SetText(label);

        // Reset Config and Clear button labels
        m_aOptionButton[P1CONFIG + player]->SetText("Configure");
        m_aOptionButton[P1CLEAR + player]->SetText("Reset");
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateConfigScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the contents of the control configuarion screen.

void MainMenuGUI::UpdateConfigScreen()
{
    char str[256];

    if (m_ScreenChange)
    {
        // Hide most things first, enable as needed
        m_pConfigLabel[CONFIGINSTRUCTION]->SetVisible(false);
        m_pConfigLabel[CONFIGINPUT]->SetVisible(false);
        m_pRecommendationBox->SetVisible(false);
        m_pRecommendationDiagram->SetVisible(false);
        m_pConfigLabel[CONFIGSTEPS]->SetVisible(false);
        m_pConfigSkipButton->SetVisible(false);
        m_pConfigBackButton->SetVisible(false);
        m_pDPadTypeBox->SetVisible(false);
        m_pDAnalogTypeBox->SetVisible(false);
        m_pXBox360TypeBox->SetVisible(false);
    }

	// [CHRISK] Use GUI input class for better key detection
	g_UInputMan.SetInputClass(m_pGUIInput);

    // Keyboard screens
    if (m_ConfiguringDevice == UInputMan::DEVICE_KEYB_ONLY)
    {
        if (m_ScreenChange)
        {
            m_pConfigLabel[CONFIGINSTRUCTION]->SetVisible(true);
            m_pConfigLabel[CONFIGINPUT]->SetVisible(true);
            sprintf(str, "Keyboard Configuration - Player %i", m_ConfiguringPlayer + 1);
            m_pConfigLabel[CONFIGTITLE]->SetText(str);
            m_pConfigLabel[CONFIGINSTRUCTION]->SetText("Press the key for");
            m_pConfigLabel[CONFIGSTEPS]->SetVisible(true);
            m_pRecommendationBox->SetVisible(true);
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetVisible(true);
            m_pConfigSkipButton->SetVisible(true);
            m_pConfigBackButton->SetVisible(true);
            m_ScreenChange = false;
        }
		
        // Step label update
        sprintf(str, "Step %i / %i", m_ConfigureStep + 1, KEYBOARDSTEPS);
        m_pConfigLabel[CONFIGSTEPS]->SetText(str);

        // Move/Aim up
        if (m_ConfigureStep == 0)
        {
            // Hide the back button on this first step
            m_pConfigBackButton->SetVisible(false);

            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE or AIM UP");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Up Cursor]" : "[W]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_AIM_UP))
            {
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_UP);
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move/Aim down
        else if (m_ConfigureStep == 1)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE or AIM DOWN");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Down Cursor]" : "[S]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_AIM_DOWN))
            {
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_DOWN);
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move left
        else if (m_ConfigureStep == 2)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE LEFT");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Left Cursor]" : "[A]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_LEFT))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move right
        else if (m_ConfigureStep == 3)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE RIGHT");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Right Cursor]" : "[D]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_RIGHT))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Fire
        else if (m_ConfigureStep == 4)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("FIRE / ACTIVATE");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 1]" : "[H]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_FIRE))

            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Sharp aim
        else if (m_ConfigureStep == 5)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("SHARP AIM");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 2]" : "[J]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_AIM))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Inventory
        else if (m_ConfigureStep == 6)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("COMMAND MENU");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 3]" : "[K]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_PIEMENU))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Jump
        else if (m_ConfigureStep == 7)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("JUMP");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num Enter]" : "[L]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_JUMP))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Crouch
        else if (m_ConfigureStep == 8)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("CROUCH");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num Del]" : "[.]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_CROUCH))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Prev actor
        else if (m_ConfigureStep == 9)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("PREVIOUS BODY");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 4]" : "[Q]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_PREV))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Next actor
        else if (m_ConfigureStep == 10)
        {
            // Hide skip button on this last step
            m_pConfigSkipButton->SetVisible(false);

            m_pConfigLabel[CONFIGINPUT]->SetText("NEXT BODY");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 5]" : "[E]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_NEXT))
            {
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
        }
		// Reload
		else if (m_ConfigureStep == 11)
		{
			// Hide skip button on this last step
			m_pConfigSkipButton->SetVisible(false);

			m_pConfigLabel[CONFIGINPUT]->SetText("RELOAD");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 0]" : "[R]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_RELOAD))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 12)
		{
			// Hide skip button on this last step
			m_pConfigSkipButton->SetVisible(false);

			m_pConfigLabel[CONFIGINPUT]->SetText("PICK UP");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 9]" : "[F]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_PICKUP))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 13)
		{
			// Hide skip button on this last step
			m_pConfigSkipButton->SetVisible(false);

			m_pConfigLabel[CONFIGINPUT]->SetText("DROP");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 6]" : "[G]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_DROP))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 14)
		{
			// Hide skip button on this last step
			m_pConfigSkipButton->SetVisible(false);

			m_pConfigLabel[CONFIGINPUT]->SetText("PREVIOUS WEAPON");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 7]" : "[X]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_CHANGE_PREV))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 15)
		{
			// Hide skip button on this last step
			m_pConfigSkipButton->SetVisible(false);

			m_pConfigLabel[CONFIGINPUT]->SetText("NEXT WEAPON");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText(m_ConfiguringPlayer % 2 ? "[Num 8]" : "[C]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_CHANGE_NEXT))
			{
				m_apScreenBox[CONFIGSCREEN]->SetVisible(false);
				m_MenuScreen = OPTIONSSCREEN;
				m_ScreenChange = true;
			}
		}

/*
        // Start
        else if (m_ConfigureStep == 10)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("START BUTTON");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_START))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Back
        else if (m_ConfigureStep == 11)
        {
            // Hide the skip button on this last step
            m_pConfigSkipButton->SetVisible(false);
            m_pConfigLabel[CONFIGINPUT]->SetText("BACK BUTTON");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_BACK))
            {
                // Done, go back to options screen
                m_apScreenBox[CONFIGSCREEN]->SetVisible(false);
                m_MenuScreen = OPTIONSSCREEN;
                m_ScreenChange = true;
            }
        }
*/      
    }
    // Mouse+keyb
    else if (m_ConfiguringDevice == UInputMan::DEVICE_MOUSE_KEYB)
    {
        if (m_ScreenChange)
        {
            m_pConfigLabel[CONFIGINSTRUCTION]->SetVisible(true);
            m_pConfigLabel[CONFIGINPUT]->SetVisible(true);
            sprintf(str, "Mouse + Keyboard Configuration - Player %i", m_ConfiguringPlayer + 1);
            m_pConfigLabel[CONFIGTITLE]->SetText(str);
            m_pConfigLabel[CONFIGINSTRUCTION]->SetText("Press the key for");
            m_pConfigLabel[CONFIGSTEPS]->SetVisible(true);
            m_pRecommendationBox->SetVisible(true);
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetVisible(true);
            m_pConfigSkipButton->SetVisible(true);
            m_pConfigBackButton->SetVisible(true);
            m_ScreenChange = false;
        }

        // Step label update
        sprintf(str, "Step %i / %i", m_ConfigureStep + 1, MOUSESTEPS);
        m_pConfigLabel[CONFIGSTEPS]->SetText(str);

        // Move up
        if (m_ConfigureStep == 0)
        {
            // Hide the back button on this first step
            m_pConfigBackButton->SetVisible(false);

            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE UP or JUMP");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[W]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_UP))
            {
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_UP);
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_JUMP);
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move down
        else if (m_ConfigureStep == 1)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE DOWN or CROUCH");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[S]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_DOWN))
            {
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_DOWN);
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_CROUCH);
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move left
        else if (m_ConfigureStep == 2)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE LEFT");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[A]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_LEFT))
            {
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_LEFT);
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move right
        else if (m_ConfigureStep == 3)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("MOVE RIGHT");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[D]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_RIGHT))
            {
                g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_RIGHT);
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Move right
        else if (m_ConfigureStep == 4)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("RELOAD");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[R]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_RELOAD))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
		else if (m_ConfigureStep == 5)
		{
			m_pConfigLabel[CONFIGINPUT]->SetText("PICK UP");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[F]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_PICKUP))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 6)
		{
			m_pConfigLabel[CONFIGINPUT]->SetText("DROP");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[G]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_DROP))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 7)
		{
			m_pConfigLabel[CONFIGINPUT]->SetText("PREV WEAPON");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[X]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_CHANGE_PREV))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		else if (m_ConfigureStep == 8)
		{
			m_pConfigLabel[CONFIGINPUT]->SetText("NEXT WEAPON");
			m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[C]");
			if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_WEAPON_CHANGE_NEXT))
			{
				m_ConfigureStep++;
				m_ScreenChange = true;
			}
		}
		// Prev actor
        else if (m_ConfigureStep == 9)
        {
            m_pConfigLabel[CONFIGINPUT]->SetText("PREVIOUS BODY");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[Q]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_PREV))
            {
                m_ConfigureStep++;
                m_ScreenChange = true;
            }
        }
        // Next actor
        else if (m_ConfigureStep == 10)
        {
            // Hide skip button on this last step
            m_pConfigSkipButton->SetVisible(false);

            m_pConfigLabel[CONFIGINPUT]->SetText("NEXT BODY");
            m_pConfigLabel[CONFIGRECOMMENDATION]->SetText("[E]");
            if (g_UInputMan.CaptureKeyMapping(m_ConfiguringPlayer, UInputMan::INPUT_NEXT))
            {
//                m_ConfigureStep++;
//                m_ScreenChange = true;
                // Done, go back to options screen
                m_apScreenBox[CONFIGSCREEN]->SetVisible(false);
                m_MenuScreen = OPTIONSSCREEN;
                m_ScreenChange = true;
            }
        }
    }
    // Gamepad screens
    else if (m_ConfiguringDevice >= UInputMan::DEVICE_GAMEPAD_1 && m_ConfiguringDevice <= UInputMan::DEVICE_GAMEPAD_4)
    {
        int whichJoy = m_ConfiguringDevice - UInputMan::DEVICE_GAMEPAD_1;
        AllegroBitmap *pDiagramBitmap = 0;

        // Choose which gamepad type - special first step
        if (m_ConfigureStep == 0)
        {
            // Set title
            sprintf(str, "Choose Gamepad Type for Player %i:", m_ConfiguringPlayer + 1);
            m_pConfigLabel[CONFIGTITLE]->SetText(str);

            // Hide the back button on this first step
            m_pConfigBackButton->SetVisible(false);

            // Show the type option boxes
            m_pDPadTypeBox->SetVisible(true);
            m_pDAnalogTypeBox->SetVisible(true);
            m_pXBox360TypeBox->SetVisible(true);

            // Not passing in ownership of the BITMAP
            pDiagramBitmap = new AllegroBitmap(m_aDPadBitmaps[0]);
            // Passing in ownership of the AllegroBitmap, but again, not the BITMAP contained within
            m_pDPadTypeDiagram->SetDrawImage(pDiagramBitmap);
            pDiagramBitmap = 0;

            // Not passing in ownership of the BITMAP
            pDiagramBitmap = new AllegroBitmap(m_aDualAnalogBitmaps[0]);
            // Passing in ownership of the AllegroBitmap, but again, not the BITMAP contained within
            m_pDAnalogTypeDiagram->SetDrawImage(pDiagramBitmap);
            // Doing it again for the 360 one.. it's similar enough looking
            pDiagramBitmap = new AllegroBitmap(m_aDualAnalogBitmaps[0]);
            m_pXBox360TypeDiagram->SetDrawImage(pDiagramBitmap);
            pDiagramBitmap = 0;

            // The special selection buttons take care of advancing the step, so do nothing else
            m_ScreenChange = false;
        }
        // Configure selected gamepad type
        else
        {
            if (m_ScreenChange)
            {
                m_pConfigLabel[CONFIGINSTRUCTION]->SetVisible(true);
                m_pConfigLabel[CONFIGINPUT]->SetVisible(true);
                m_pConfigLabel[CONFIGSTEPS]->SetVisible(true);
                m_pRecommendationBox->SetVisible(true);
                m_pConfigSkipButton->SetVisible(true);
                m_pConfigBackButton->SetVisible(true);
                m_BlinkTimer.Reset();
            }

            // D-pad
            if (m_ConfiguringGamepad == DPAD)
            {
                if (m_ScreenChange)
                {
                    sprintf(str, "D-Pad Gamepad Configuration - Player %i", m_ConfiguringPlayer + 1);
                    m_pConfigLabel[CONFIGTITLE]->SetText(str);
                    m_pConfigLabel[CONFIGRECOMMENDATION]->SetVisible(false);
                    m_pConfigLabel[CONFIGINSTRUCTION]->SetText("Press the button or move the stick for");
                    m_pRecommendationDiagram->SetVisible(true);
                    m_pRecommendationDiagram->Resize(m_aDPadBitmaps[0]->w, m_aDPadBitmaps[0]->h);
                    m_pRecommendationDiagram->CenterInParent(true, true);
                    m_pRecommendationDiagram->MoveRelative(0, 4);
                    m_ScreenChange = false;
                }

                // Step label update
                sprintf(str, "Step %i / %i", m_ConfigureStep + 1, DPADSTEPS);
                m_pConfigLabel[CONFIGSTEPS]->SetText(str);

                // Diagram update
                // Not passing in ownership of the BITMAP
                pDiagramBitmap = new AllegroBitmap(m_aDPadBitmaps[m_BlinkTimer.AlternateReal(500) ? 0 : m_ConfigureStep]);
                // Passing in ownership of the AllegroBitmap, but again, not the BITMAP contained within
                m_pRecommendationDiagram->SetDrawImage(pDiagramBitmap);
                pDiagramBitmap = 0;

                // Move/Aim up
                if (m_ConfigureStep == 1)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE or AIM UP");

                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_AIM_UP))
                    {
//                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_UP);
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_UP);
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_UP);
//                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_UP);
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Move/Aim down
                else if (m_ConfigureStep == 2)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE or AIM DOWN");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_AIM_DOWN))
                    {
//                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_DOWN);
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_L_DOWN);
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_DOWN);
//                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_DOWN);
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Move left
                else if (m_ConfigureStep == 3)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE LEFT");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_LEFT))
                    {
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_LEFT);
//                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_LEFT);
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Move right
                else if (m_ConfigureStep == 4)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE RIGHT");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_RIGHT))
                    {
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_R_RIGHT);
//                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_RIGHT);
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Sharp aim
                else if (m_ConfigureStep == 5)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("SHARP AIM");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_AIM))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Fire
                else if (m_ConfigureStep == 6)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("FIRE / ACTIVATE");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_FIRE))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Jump
                else if (m_ConfigureStep == 7)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("JUMP");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_JUMP))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
// TODO: CROUCH???
                // Inventory
                else if (m_ConfigureStep == 8)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("COMMAND MENU");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_PIEMENU))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Next actor
                else if (m_ConfigureStep == 9)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("NEXT BODY");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_NEXT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Prev actor
                else if (m_ConfigureStep == 10)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("PREVIOUS BODY");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_PREV))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Start
                else if (m_ConfigureStep == 11)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("START BUTTON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_START))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Back
                else if (m_ConfigureStep == 12)
                {
                    // Hide the skip button on this last step
                    m_pConfigSkipButton->SetVisible(false);
                    m_pConfigLabel[CONFIGINPUT]->SetText("BACK BUTTON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_BACK))
                    {
                        // Done, go back to options screen
                        m_apScreenBox[CONFIGSCREEN]->SetVisible(false);
                        m_MenuScreen = OPTIONSSCREEN;
                        m_ScreenChange = true;
                    }
                }
            }
            // Dual analog OR XBox Controller
            else if (m_ConfiguringGamepad == DANALOG || m_ConfiguringGamepad == XBOX360)
            {
                if (m_ScreenChange)
                {
                    sprintf(str, "Dual Analog Gamepad Configuration - Player %i", m_ConfiguringPlayer + 1);
                    m_pConfigLabel[CONFIGTITLE]->SetText(str);
                    m_pConfigLabel[CONFIGRECOMMENDATION]->SetVisible(false);
                    m_pConfigLabel[CONFIGINSTRUCTION]->SetText("Press the button or move the stick for");
                    m_pRecommendationDiagram->SetVisible(true);
                    m_pRecommendationDiagram->Resize(m_aDualAnalogBitmaps[0]->w, m_aDualAnalogBitmaps[0]->h);
                    m_pRecommendationDiagram->CenterInParent(true, true);
                    m_pRecommendationDiagram->MoveRelative(0, 8);
                    m_ScreenChange = false;
                }

                // Step label update
                sprintf(str, "Step %i / %i", m_ConfigureStep + 1, DANALOGSTEPS);
                m_pConfigLabel[CONFIGSTEPS]->SetText(str);

                // Diagram update
                // Not passing in ownership of the BITMAP
                pDiagramBitmap = new AllegroBitmap(m_aDualAnalogBitmaps[m_BlinkTimer.AlternateReal(500) ? 0 : m_ConfigureStep]);
                // Passing in ownership of the AllegroBitmap, but again, not the BITMAP contained within
                m_pRecommendationDiagram->SetDrawImage(pDiagramBitmap);
                pDiagramBitmap = 0;

                // Move up
                if (m_ConfigureStep == 1)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE UP or JUMP");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_UP))
                    {
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_AIM_UP);
                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_JUMP);
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Move down
                else if (m_ConfigureStep == 2)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE DOWN or CROUCH");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_DOWN))
                    {
                        g_UInputMan.ClearMapping(m_ConfiguringPlayer, UInputMan::INPUT_AIM_DOWN);
                        g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_CROUCH);
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Move left
                else if (m_ConfigureStep == 3)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE LEFT");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_LEFT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Move right
                else if (m_ConfigureStep == 4)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("MOVE RIGHT");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_L_RIGHT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Aim up
                else if (m_ConfigureStep == 5)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("AIM UP");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_UP))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Aim down
                else if (m_ConfigureStep == 6)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("AIM DOWN");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_DOWN))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Aim left
                else if (m_ConfigureStep == 7)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("AIM LEFT");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_LEFT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Aim right
                else if (m_ConfigureStep == 8)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("AIM RIGHT");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_R_RIGHT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Fire
                else if (m_ConfigureStep == 9)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("FIRE / ACTIVATE");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_FIRE))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Inventory
                else if (m_ConfigureStep == 10)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("COMMAND MENU");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_PIEMENU))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Next actor
                else if (m_ConfigureStep == 11)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("NEXT BODY");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_NEXT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Prev actor
                else if (m_ConfigureStep == 12)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("PREVIOUS BODY");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_PREV))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }

                // Prev weapon
                else if (m_ConfigureStep == 13)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("PREVIOUS WEAPON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_WEAPON_CHANGE_PREV))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Next weapon
                else if (m_ConfigureStep == 14)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("NEXT WEAPON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_WEAPON_CHANGE_NEXT))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Pickup weapon
                else if (m_ConfigureStep == 15)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("PICKUP WEAPON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_WEAPON_PICKUP))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Reload weapon
                else if (m_ConfigureStep == 16)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("RELOAD WEAPON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_WEAPON_RELOAD))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Start
                else if (m_ConfigureStep == 17)
                {
                    m_pConfigLabel[CONFIGINPUT]->SetText("START BUTTON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_START))
                    {
                        m_ConfigureStep++;
                        m_ScreenChange = true;
                    }
                }
                // Back
                else if (m_ConfigureStep == 18)
                {
                    // Hide the skip button on this last step
                    m_pConfigSkipButton->SetVisible(false);
                    m_pConfigLabel[CONFIGINPUT]->SetText("BACK BUTTON");
                    if (g_UInputMan.CaptureJoystickMapping(m_ConfiguringPlayer, whichJoy, UInputMan::INPUT_BACK))
                    {
                        // If Xbox controller; if the A button has not been mapped to Activate/fire, then map it automatically
// These redundancies should apply to all custom analog setups, really
//                        if (m_ConfiguringGamepad == XBOX360)
                        {
                            // No button assigned to fire, so give it 'A' on the controller (in addition to any axis inputs)
                            if (g_UInputMan.GetButtonMapping(m_ConfiguringPlayer, UInputMan::INPUT_FIRE) == UInputMan::JOY_NONE)
                                g_UInputMan.SetButtonMapping(m_ConfiguringPlayer, UInputMan::INPUT_FIRE, UInputMan::JOY_1);
                            // No button assigned to pie menu, so give it 'B' on the controller (in addition to whatever axis it's assinged to)
                            if (g_UInputMan.GetButtonMapping(m_ConfiguringPlayer, UInputMan::INPUT_PIEMENU) == UInputMan::JOY_NONE)
                                g_UInputMan.SetButtonMapping(m_ConfiguringPlayer, UInputMan::INPUT_PIEMENU, UInputMan::JOY_2);
                        }

                        // Done, go back to options screen
                        m_apScreenBox[CONFIGSCREEN]->SetVisible(false);
                        m_MenuScreen = OPTIONSSCREEN;
                        m_ScreenChange = true;
                    }
                }
            }
        }
    }

	g_UInputMan.SetInputClass(NULL);
	
    if (m_ScreenChange)
        m_ExitMenuSound.Play();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          PopulatePublishingWithPrevious
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Populates the fields of the publishing steps with whatever might ahve
//                  been filled out before for the same-named mod.

bool MainMenuGUI::PopulatePublishingWithPrevious(string moduleName)
{
#if defined(STEAM_BUILD)
    // Title
    m_pPublishTitleText->SetText(g_SteamUGCMan.GetPublishedTitle(moduleName));
    // Description
    m_pPublishDescText->SetText(g_SteamUGCMan.GetPublishedDescription(moduleName));
    // Do the tags now
    bool moduleFound = false;
    m_apPublishTags[PUBLISHTAGACTORS]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Actors", moduleFound));
    m_apPublishTags[PUBLISHTAGWEAPONS]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Weapons", moduleFound));
    m_apPublishTags[PUBLISHTAGTOOLS]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Tools", moduleFound));
    m_apPublishTags[PUBLISHTAGCRAFT]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Craft", moduleFound));
    m_apPublishTags[PUBLISHTAGBUNKER]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Bunkers", moduleFound));
    m_apPublishTags[PUBLISHTAGSCENES]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Scenes", moduleFound));
    m_apPublishTags[PUBLISHTACTIVITIES]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Activities", moduleFound));
    m_apPublishTags[PUBLISHTAGTECH]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Complete Tech", moduleFound));
    m_apPublishTags[PUBLISHTAGVANILLA]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Vanilla-Balanced", moduleFound));
    m_apPublishTags[PUBLISHTAGFRIENDS]->SetCheck(g_SteamUGCMan.GetPublishedTagState(moduleName, "Friends-Only", moduleFound));

    return moduleFound;
#else
	return false;
#endif //STEAMBUILD
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdatePublishingStepsDialog
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the contents of the Workshop publishing steps dialog.

void MainMenuGUI::UpdatePublishingStepsDialog()
{
#if defined(STEAM_BUILD)
    if (m_PublishStep < PUBLISHSTEPFIRST)
        m_PublishStep = PUBLISHSTEPFIRST;
    if (m_PublishStep >= PUBLISHSTEPCOUNT)
        m_PublishStep = PUBLISHSTEPCOUNT - 1;

    // If switching to this dialog, reset some buttons etc
    if (m_ScreenChange)
    {
        m_pPublishCancelButton->SetText("Cancel");
        m_pPublishCancelButton->SetVisible(true);
        m_pAppLinkButton->SetVisible(false);

        // Populate the different unofficial and unpublished mods we have as candidates for new publishing
        if (m_PublishStep == PUBLISHSTEPFIRST)
        {
            string modName;
            deque<string> subscribedMods;
            deque<string> publishedMods;
            g_SteamUGCMan.GetSubscribedNamesList(subscribedMods);
            g_SteamUGCMan.GetPublishedNamesList(publishedMods);
            // Clear out the list so we start fresh
            m_pUnpublishedList->ClearList();
            bool skip = false;
            // Go through all unofficial modules and add them to the list of publishing candidates
            for (int i = g_PresetMan.GetOfficialModuleCount(); i < g_PresetMan.GetTotalModuleCount(); ++i)
            {
                skip = false;
                // The current mod name, all lowercase
                modName = g_PresetMan.GetDataModule(i)->GetFileName();
                std::transform(modName.begin(), modName.end(), modName.begin(), ::tolower);
                // Skip any mods that are subscribed to AND also not published by this user
                for (deque<string>::iterator subItr = subscribedMods.begin(); subItr != subscribedMods.end(); subItr++)
                {
                    if ((*subItr).find(modName) != string::npos)
                    {
                        skip = true;
                        // Now make sure we're also not the publisher of that file, then we should not skip it
                        for (deque<string>::iterator pubItr = publishedMods.begin(); pubItr != publishedMods.end(); pubItr++)
                        {
                            if ((*pubItr).find(modName) != string::npos)
                                skip = false;
                        }
                    }
                }

// TODO: cull already-published ones? Maybe not since they can simply become updates even though user pressed 'publish new'
                // HACK: Cull the special/semi-official modules 
                if (!skip && g_PresetMan.GetDataModule(i)->GetFileName().find("Scenes.rte") == string::npos &&
                    g_PresetMan.GetDataModule(i)->GetFileName().find("Metagames.rte") == string::npos)
                    m_pUnpublishedList->AddItem(g_PresetMan.GetDataModule(i)->GetFileName());
            }
            // Select the first one
// Actually, don't.. let user pick
//            m_pUnpublishedList->SetSelectedIndex(0);
        }
        // Starting to fill out forms
        else if (m_PublishStep == PUBLISHSTEPDESCRIPTION)
        {
/* This is bad because when doing step back from third step, it will wipe all changes made - these are now called from teh actual button press handlers
            // If we're about to start filling out forms, see if we can pre-fill them with data from previous publishing of the same-named mod
            // We are updating an already-published mod, so let's try to get the previously filled info
            if (m_UpdatingAlreadyPublished)
            {
                if (m_pPublishedList->GetSelectedItem())
                    PopulatePublishingWithPrevious(m_pPublishedList->GetSelectedItem()->m_Name);
            }
            // Setting up a new one, but still let's see if we can find previously-filled in info from earlier publishings
            else
            {
                if (m_pUnpublishedList->GetSelectedItem())
                    PopulatePublishingWithPrevious(m_pUnpublishedList->GetSelectedItem()->m_Name);
            }
*/

            // Only show the next step button if required forms are filled out
            m_pPublishNextButton->SetVisible(false);
        }
        // Only show the next step if at least one tag is checked
        else if (m_PublishStep == PUBLISHSTEPTAGS)
            m_pPublishNextButton->SetVisible(false);
        // Only show the final publish button when agreement button has been pressed
        else if (m_PublishStep == PUBLISHSTEPAGREEMENT)
            m_pPublishNextButton->SetVisible(false);
        // Clear the publishing status list
        else if (m_PublishStep == PUBLISHSTEPLAST)
            m_pPublishStatus->ClearList();
        else
            m_pPublishNextButton->SetVisible(true);

        m_ScreenChange = false;
    }

    // Update the header
    char str[256];
    sprintf(str, "P U B L I S H I N G   S T E P   %d / %d", m_PublishStep + 1, PUBLISHSTEPCOUNT);
    m_pPublishStepsHeader->SetText(string(str));

    // Turn off all step control collection boxes first so we can show the single, proper one
    for (int step = PUBLISHSTEPFIRST; step < PUBLISHSTEPCOUNT; ++step)
        m_apPublishStepsControls[step]->SetVisible(false);

    // Show the relevant step group
    m_apPublishStepsControls[m_PublishStep]->SetVisible(true);

    // If on last last step before comitting, show that on the 'next' button
    if (m_PublishStep == PUBLISHSTEPAGREEMENT)
        m_pPublishNextButton->SetText(m_UpdatingAlreadyPublished ? "UPDATE!" : "PUBLISH!");
    else if (m_PublishStep == PUBLISHSTEPLAST)
        m_pPublishNextButton->SetText("Done");
    else
        m_pPublishNextButton->SetText("Next >");

    // Hide backstep buttons as necessary
    m_pPublishBackButton->SetVisible(m_PublishStep != PUBLISHSTEPFIRST && !(m_PublishStep == PUBLISHSTEPDESCRIPTION && m_UpdatingAlreadyPublished));

    // Show the forward step button when ready at each step
    if (m_PublishStep == PUBLISHSTEPFIRST)
    {
        // See if we can find a preview image in the currently selected mod's folder root
        if (m_pUnpublishedList->GetSelectedIndex() != -1)
        {
            string selectedMod = m_pUnpublishedList->GetSelectedItem()->m_Name;
            string previewFilePath = "./" + selectedMod + "/preview.jpg";
            // If the preview file exists, then let proceed. If not, set up and blink notice label
            if (exists(previewFilePath.c_str()))
            {
                m_pPreviewNoticeLabel->SetText("Rememeber, you can update the 512x512px " + selectedMod + "/Preview.jpg to showcase your mod in the Workshop");
                m_pPublishNextButton->SetVisible(m_pUnpublishedList->GetSelectedIndex() != -1);
            }
            else
            {
                if (m_BlinkTimer.AlternateReal(333))
                    m_pPreviewNoticeLabel->SetText("To continue, put a 512x512px PREVIEW.JPG in the " + selectedMod + "/" + " dir to showcase your mod in the Workshop ");
                else
                    m_pPreviewNoticeLabel->SetText("To continue, put a 512x512px PREVIEW.JPG in the " + selectedMod + "/" + " dir to showcase your mod in the Workshop!");
                m_pPublishNextButton->SetVisible(false);
            }
        }
        else
            m_pPreviewNoticeLabel->SetText("Make sure there is a 512x512px PREVIEW.JPG in the .rte directory root to showcase your mod in the Workshop");
    }
    else if (m_PublishStep == PUBLISHSTEPDESCRIPTION)
    {
        m_pPublishNextButton->SetVisible(!m_pPublishTitleText->GetText().empty() && !m_pPublishDescText->GetText().empty());
    }
    else if (m_PublishStep == PUBLISHSTEPTAGS)
    {
        // See if at least one tag is checked
        bool anyChecked = false;
        // We are deliberately not checking the last two things; they aren't about the content
        for (int tag = 0; tag <= PUBLISHTAGTECH; ++tag)
            anyChecked = m_apPublishTags[tag]->GetCheck() || anyChecked;
        m_pPublishNextButton->SetVisible(anyChecked);
    }
    // If on last step where the publishing is happening but isn't done yet, show the cancel button but not the done button
    else if (m_PublishStep == PUBLISHSTEPLAST)
    {
        m_pPublishCancelButton->SetVisible(!m_PublishingComplete);
        // This is now overridden by the app link button that appears when things are done
        m_pPublishNextButton->SetVisible(false);
        // Make the link button appear when publishing is done
//        m_pPublishStatus->SetSize(m_pPublishStatus->GetWidth(), m_PublishingComplete ? 60 : 90);
//        m_pPublishStatus->SetFont(m_pGUIController->GetSkin()->GetFont("smallfont.bmp"));
        // Place the app link button over where the cancel button was
        m_pAppLinkButton->SetVisible(m_PublishingComplete);
        m_pAppLinkButton->SetPositionRel(m_pPublishCancelButton->GetRelXPos(), m_pPublishCancelButton->GetRelYPos());
    }

    // Keep pumping that Steam API
		g_SteamUGCMan.Update();
	#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetupAndSubmitPublishing
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Collects all the user-entered module metadata from all publishing
//                  forms and submits it all for publishing to the Steam Workshop.

int MainMenuGUI::SetupAndSubmitPublishing()
{
#if defined(STEAM_BUILD)
    // Name of the directory of the data module we're about to send off
    string dirName;

    // Wether we are updating or publishing a new thing, get the module name from the appropriate drop down list
    if (m_UpdatingAlreadyPublished)
    {
        if (m_pPublishedList->GetSelectedIndex() < 0)
            return -1;
        dirName = m_pPublishedList->GetSelectedItem()->m_Name;
    }
    else
    {
        if (m_pUnpublishedList->GetSelectedIndex() < 0)
            return -1;
        dirName = m_pUnpublishedList->GetSelectedItem()->m_Name;
    }

	// Harvest the rest of the user-entered metadata 
    string title = m_pPublishTitleText->GetText();
    string description = m_pPublishDescText->GetText();

    deque<string> tags;
    if (m_apPublishTags[PUBLISHTAGACTORS]->GetCheck())
        tags.push_back(string("Actors"));
    if (m_apPublishTags[PUBLISHTAGWEAPONS]->GetCheck())
        tags.push_back(string("Weapons"));
    if (m_apPublishTags[PUBLISHTAGTOOLS]->GetCheck())
        tags.push_back(string("Tools"));
    if (m_apPublishTags[PUBLISHTAGCRAFT]->GetCheck())
        tags.push_back(string("Craft"));
    if (m_apPublishTags[PUBLISHTAGBUNKER]->GetCheck())
        tags.push_back(string("Bunkers"));
    if (m_apPublishTags[PUBLISHTAGSCENES]->GetCheck())
        tags.push_back(string("Scenes"));
    if (m_apPublishTags[PUBLISHTACTIVITIES]->GetCheck())
        tags.push_back(string("Activities"));
    if (m_apPublishTags[PUBLISHTAGTECH]->GetCheck())
        tags.push_back(string("Complete Tech"));
    if (m_apPublishTags[PUBLISHTAGVANILLA]->GetCheck())
        tags.push_back(string("Vanilla-Balanced"));
    if (m_apPublishTags[PUBLISHTAGFRIENDS]->GetCheck())
        tags.push_back(string("Friends-Only"));

    // Sanity check
    if (dirName.empty() || title.empty() || description.empty() || tags.empty())
    {
        PublishingProgressReport("Something was wrong with the data entered in previous steps!?", true);
        return -1;
    }

    // Write log message
    PublishingProgressReport("--- Publishing log is saved to LogPublish.txt ---", true);

    // Actually do the publishing
    int result = g_SteamUGCMan.PublishDataModule(dirName, title, description, tags, this);

    // Write log message again
    PublishingProgressReport("--- Publishing log was saved to LogPublish.txt ---", true);

    return result;
#else
    return 0;
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          UpdateLicenseScreen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the contents of the license registration screen.

void MainMenuGUI::UpdateLicenseScreen()
{
    m_pLicenseEmailBox->SetVisible(false);
    m_pLicenseKeyBox->SetVisible(true);
    m_pRegistrationButton->SetVisible(true);

    if (!g_LicenseMan.HasValidatedLicense())
    {
        m_pLicenseCaptionLabel->SetText("R E G I S T E R");
        m_pLicenseInstructionLabel->SetText("Cortex Command is currently in locked demo mode.\nGo to HTTP://WWW.DATAREALMS.COM and buy a license key.\nEnter the key info below to unlock all the features of this game!");

		std::string lastLicenseEmail = g_LicenseMan.GetLastLicenseEmail();
		std::string enteredEmail = m_pLicenseEmailBox->GetText();
		std::string lastLicenseKey = g_LicenseMan.GetLastLicenseKey();
		std::string enteredKey = m_pLicenseKeyBox->GetText();
		
        // Clear any invalid email in the field
        // Pre-set the last valid email used, if available
        if (!g_LicenseMan.CheckEmailFormatting(enteredEmail))
		{
            m_pLicenseEmailBox->SetText(lastLicenseEmail);
		}
		
        // Clear any invalid key in the field
        // Pre-set the last valid key used, if available
        if (!g_LicenseMan.CheckKeyFormatting(enteredKey))
		{
            m_pLicenseKeyBox->SetText(lastLicenseKey);
		}

        m_pLicenseEmailLabel->SetVisible(true);
        m_pLicenseKeyLabel->SetVisible(true);
        m_pLicenseEmailBox->SetVisible(true);
        m_pLicenseEmailBox->SetFocus();
        m_pLicenseKeyBox->SetVisible(true);
//        m_pLicenseKeyBox->SetFocus();

        m_pRegistrationButton->SetText("Register Key");
    }
    else
    {
        m_pLicenseCaptionLabel->SetText("U N - R E G I S T E R");
        m_pLicenseInstructionLabel->SetText("This copy is currently registered to the email address:\n" + g_LicenseMan.GetLicenseEmail() + "\nusing the key:\n" + g_LicenseMan.GetLicenseKey() + "\n\nPress below if you want to use your key on a different computer:");

        // Hide the email/key box so user can't monkey with the valid registered key
        m_pLicenseEmailLabel->SetVisible(false);
        m_pLicenseKeyLabel->SetVisible(false);
        m_pLicenseEmailBox->SetText(g_LicenseMan.GetLicenseEmail());
        m_pLicenseEmailBox->SetVisible(false);
        m_pLicenseKeyBox->SetText(g_LicenseMan.GetLicenseKey());
        m_pLicenseKeyBox->SetVisible(false);

        m_pRegistrationButton->SetText("Un-Register Key");
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MakeModString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes UI displayable string with mod info

std::string MainMenuGUI::MakeModString(ModRecord r)
{
	string s;

	if (r.Disabled)
		s = "- ";
	else
		s = "+ ";
	s = s + r.ModulePath + " - " + r.ModuleName;

	return s;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          MakeScriptString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes UI-displayable string with script info

std::string MainMenuGUI::MakeScriptString(ScriptRecord r)
{
	string s;

	if (!r.Enabled)
		s = "- ";
	else 
		s = "+ ";
	s = s + r.PresetName;

	return s;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ToggleMod
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Turns currently selected mod on and aff and changes UI elements accordingly.

void MainMenuGUI::ToggleMod()
{
	int index = m_pModManagerModsListBox->GetSelectedIndex();
	if (index > -1)
	{
		GUIListPanel::Item *selectedItem = m_pModManagerModsListBox->GetSelected();
		ModRecord r = m_KnownMods.at(selectedItem->m_ExtraIndex);

		r.Disabled = !r.Disabled;

		if (r.Disabled)
		{
			m_pModManagerToggleModButton->SetText("Enable");
			g_SettingsMan.DisableMod(r.ModulePath);
		}
		else
		{
			m_pModManagerToggleModButton->SetText("Disable");
			g_SettingsMan.EnableMod(r.ModulePath);
		}
		selectedItem->m_Name = MakeModString(r);
		m_KnownMods[selectedItem->m_ExtraIndex] = r;
		m_pModManagerModsListBox->SetSelectedIndex(index);
		m_pModManagerModsListBox->Invalidate();
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ToggleScript
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Turns currently selected script on and aff and changes UI elements accordingly.

void MainMenuGUI::ToggleScript()
{
	int index = m_pModManagerScriptsListBox->GetSelectedIndex();
	if (index > -1)
	{
		GUIListPanel::Item *selectedItem = m_pModManagerScriptsListBox->GetSelected();
		ScriptRecord r = m_KnownScripts.at(selectedItem->m_ExtraIndex);

		r.Enabled = !r.Enabled;

		if (r.Enabled)
		{
			m_pModManagerToggleScriptButton->SetText("Disable");
			g_SettingsMan.EnableScript(r.PresetName);
		}
		else
		{
			m_pModManagerToggleScriptButton->SetText("Enable");
			g_SettingsMan.DisableScript(r.PresetName);
		}
		selectedItem->m_Name = MakeScriptString(r);
		m_KnownScripts[selectedItem->m_ExtraIndex] = r;
		m_pModManagerScriptsListBox->SetSelectedIndex(index);
		m_pModManagerScriptsListBox->Invalidate();
	}
}

