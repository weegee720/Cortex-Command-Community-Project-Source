/*          ______   ______   ______  ______  ______  __  __       ______   ______   __    __   __    __   ______   __   __   _____
           /\  ___\ /\  __ \ /\  == \/\__  _\/\  ___\/\_\_\_\     /\  ___\ /\  __ \ /\ "-./  \ /\ "-./  \ /\  __ \ /\ "-.\ \ /\  __-.
           \ \ \____\ \ \/\ \\ \  __<\/_/\ \/\ \  __\\/_/\_\/_    \ \ \____\ \ \/\ \\ \ \-./\ \\ \ \-./\ \\ \  __ \\ \ \-.  \\ \ \/\ \
            \ \_____\\ \_____\\ \_\ \_\ \ \_\ \ \_____\/\_\/\_\    \ \_____\\ \_____\\ \_\ \ \_\\ \_\ \ \_\\ \_\ \_\\ \_\\"\_\\ \____-
  	         \/_____/ \/_____/ \/_/ /_/  \/_/  \/_____/\/_/\/_/     \/_____/ \/_____/ \/_/  \/_/ \/_/  \/_/ \/_/\/_/ \/_/ \/_/ \/____/
   ______   ______   __    __   __    __   __  __   __   __   __   ______  __  __       ______  ______   ______      __   ______   ______   ______
  /\  ___\ /\  __ \ /\ "-./  \ /\ "-./  \ /\ \/\ \ /\ "-.\ \ /\ \ /\__  _\/\ \_\ \     /\  == \/\  == \ /\  __ \    /\ \ /\  ___\ /\  ___\ /\__  _\
  \ \ \____\ \ \/\ \\ \ \-./\ \\ \ \-./\ \\ \ \_\ \\ \ \-.  \\ \ \\/_/\ \/\ \____ \    \ \  _-/\ \  __< \ \ \/\ \  _\_\ \\ \  __\ \ \ \____\/_/\ \/
   \ \_____\\ \_____\\ \_\ \ \_\\ \_\ \ \_\\ \_____\\ \_\\"\_\\ \_\  \ \_\ \/\_____\    \ \_\   \ \_\ \_\\ \_____\/\_____\\ \_____\\ \_____\  \ \_\
    \/_____/ \/_____/ \/_/  \/_/ \/_/  \/_/ \/_____/ \/_/ \/_/ \/_/   \/_/  \/_____/     \/_/    \/_/ /_/ \/_____/\/_____/ \/_____/ \/_____/   \/_/

/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\/////\\\\\*/

/// <summary>
/// Main driver implementation of the Retro Terrain Engine.
/// Data Realms, LLC - http://www.datarealms.com
/// Cortex Command Center - https://discord.gg/SdNnKJN
/// Cortex Command Community Project - https://github.com/cortex-command-community
/// </summary>

#include "System.h"

#include "MetaMan.h"
#include "SettingsMan.h"
#include "ConsoleMan.h"
#include "PresetMan.h"
#include "PerformanceMan.h"
#include "PrimitiveMan.h"
#include "UInputMan.h"

#include "GUI/GUI.h"
#include "GUI/AllegroBitmap.h"
#include "LoadingGUI.h"
#include "MainMenuGUI.h"
#include "ScenarioGUI.h"
#include "MetagameGUI.h"

#include "DataModule.h"
#include "SceneLayer.h"
#include "MOSParticle.h"
#include "MOSRotating.h"
#include "Controller.h"

#include "MultiplayerServerLobby.h"
#include "NetworkServer.h"

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }

using namespace RTE;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Global variables.
/// </summary>

enum TITLESEQUENCE {
    START = 0,
    // DRL Logo
    LOGOFADEIN,
    LOGODISPLAY,
    LOGOFADEOUT,
	FMODLOGOFADEIN,
	FMODLOGODISPLAY,
	FMODLOGOFADEOUT,
    // Game notice
    NOTICEFADEIN,
    NOTICEDISPLAY,
    NOTICEFADEOUT,
    // Intro
    FADEIN,
    SPACEPAUSE1,
    SHOWSLIDE1,
    SHOWSLIDE2,
    SHOWSLIDE3,
    SHOWSLIDE4,
    SHOWSLIDE5,
    SHOWSLIDE6,
    SHOWSLIDE7,
    SHOWSLIDE8,
    PRETITLE,
    TITLEAPPEAR,
    PLANETSCROLL,
    PREMENU,
    MENUAPPEAR,
    // Main menu is active and operational
    MENUACTIVE,
    // Scenario mode views and transitions
    MAINTOSCENARIO,
    // Back from a scenario game to the scenario selection menu
    SCENARIOFADEIN,
    SCENARIOMENU,
    // Campaign mode views and transitions
    MAINTOCAMPAIGN,
    // Back from a battle to the campaign view
    CAMPAIGNFADEIN,
    CAMPAIGNPLAY,
    // Going back to the main menu view from a planet-centered view
    PLANETTOMAIN,
    FADESCROLLOUT,
    FADEOUT,
    END
};

// Intro slides
enum SLIDES {
    SLIDEPAST = 0,
    SLIDENOW,
    SLIDEVR,
    SLIDETRAVEL,
    SLIDEALIENS,
    SLIDETRADE,
    SLIDEPEACE,
    SLIDEFRONTIER,
    SLIDECOUNT
};

volatile bool g_Quit = false;
bool g_ResetRTE = false; //!< Signals to reset the entire RTE next iteration.
bool g_LaunchIntoEditor = false; //!< Flag for launching directly into editor activity.
const char *g_EditorToLaunch = ""; //!< String with editor activity name to launch.
bool g_InActivity = false;
bool g_ResetActivity = false;
bool g_ResumeActivity = false;
bool g_MeasureModuleLoadTime = false;
int g_IntroState = START;
int g_StationOffsetX;
int g_StationOffsetY;

MainMenuGUI *g_pMainMenuGUI = 0;
ScenarioGUI *g_pScenarioGUI = 0;
Controller *g_pMainMenuController = 0;

enum StarSize {
    StarSmall = 0,
    StarLarge,
    StarHuge,
};

struct Star {
    // Bitmap representation
    BITMAP *m_pBitmap;
    // Center location on screen
    Vector m_Pos;
    // Bitmap offset
//    int m_Offset;
    // Scrolling ratio
    float m_ScrollRatio;
    // Normalized intensity 0-1.0
    float m_Intensity;
    // Type
    StarSize m_Size;

    Star() { m_pBitmap = 0; m_Pos.Reset(); m_ScrollRatio = 1.0; m_Intensity = 1.0; m_Size = StarSmall; }
    Star(BITMAP *pBitmap, Vector &pos, float scrollRatio, float intensity) { m_pBitmap = pBitmap; m_Pos = pos; m_ScrollRatio = scrollRatio; m_Intensity = intensity; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// This handles when the quit or exit button is pressed on the window.
/// </summary>
void QuitHandler(void) { g_Quit = true; }
END_OF_FUNCTION(QuitHandler)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Load and initialize the Main Menu.
/// </summary>
/// <returns></returns>
bool InitMainMenu() {
    g_FrameMan.LoadPalette("Base.rte/palette.bmp");

    // Create the main menu interface
	g_pMainMenuGUI = new MainMenuGUI();
    g_pMainMenuController = new Controller(Controller::CIM_PLAYER, 0);
    g_pMainMenuController->SetTeam(0);
    g_pMainMenuGUI->Create(g_pMainMenuController);
    // As well as the Scenario setup menu interface
	g_pScenarioGUI = new ScenarioGUI();
    g_pScenarioGUI->Create(g_pMainMenuController);
    // And the Metagame GUI too
    g_MetaMan.GetGUI()->Create(g_pMainMenuController);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Reset the current activity.
/// </summary>
/// <returns></returns>
bool ResetActivity() {
	g_ConsoleMan.PrintString("SYSTEM: Activity was reset!");
    g_ResetActivity = false;

    // Clear and reset out things
    g_FrameMan.ClearBackBuffer8();
    g_FrameMan.FlipFrameBuffers();
    g_AudioMan.StopAll();

    // Quit if we should
	if (g_Quit) {
		return false;
	}

	// TODO: Deal with GUI resetting here!$@#")
    // Clear out all MO's
    g_MovableMan.PurgeAllMOs();
    // Have to reset TimerMan before creating anything else because all timers are reset against it
    g_TimerMan.ResetTime();

    g_FrameMan.LoadPalette("Base.rte/palette.bmp");
    g_FrameMan.FlipFrameBuffers();

    // Reset TimerMan again after loading so there's no residual delay
    g_TimerMan.ResetTime();
    // Enable time averaging since it helps with animation jerkiness
    g_TimerMan.EnableAveraging(true);
    // Unpause
    g_TimerMan.PauseSim(false);

    int error = g_ActivityMan.RestartActivity();
	if (error >= 0) {
		g_InActivity = true;
	} else {
        // Something went wrong when restarting, so drop out to scenario menu and open the console to show the error messages
		g_InActivity = false;
		g_ActivityMan.PauseActivity();
		g_ConsoleMan.SetEnabled(true);
		g_IntroState = MAINTOSCENARIO;
		return false;
	}
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Start the simulation back up after being paused.
/// </summary>
void ResumeActivity() {
	if (g_ActivityMan.GetActivity()->GetActivityState() != Activity::NotStarted) {
		g_Quit = false;
		g_InActivity = true;
		g_ResumeActivity = false;

		g_FrameMan.ClearBackBuffer8();
		g_FrameMan.FlipFrameBuffers();
		g_FrameMan.LoadPalette("Base.rte/palette.bmp");

		g_PerformanceMan.ResetFrameTimer();
        // Enable time averaging since it helps with animation jerkiness
		g_TimerMan.EnableAveraging(true);
		g_TimerMan.PauseSim(false);
		g_ActivityMan.PauseActivity(false);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Launch multiplayer lobby activity.
/// </summary>
void EnterMultiplayerLobby() {
	g_SceneMan.SetSceneToLoad("Multiplayer Scene");
	MultiplayerServerLobby *pMultiplayerServerLobby = new MultiplayerServerLobby;
	pMultiplayerServerLobby->Create();

	pMultiplayerServerLobby->ClearPlayers(true);
	pMultiplayerServerLobby->AddPlayer(0, true, 0, 0);
	pMultiplayerServerLobby->AddPlayer(1, true, 0, 1);
	pMultiplayerServerLobby->AddPlayer(2, true, 0, 2);
	pMultiplayerServerLobby->AddPlayer(3, true, 0, 3);

	//g_FrameMan.ResetSplitScreens(true, true);
	g_ActivityMan.SetStartActivity(pMultiplayerServerLobby);
	g_ResetActivity = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Launch editor activity specified in command-line argument.
/// </summary>
void EnterEditorActivity(const char *editorToEnter) {
	if (std::strcmp(editorToEnter, "ActorEditor") == 0) { 
		g_pMainMenuGUI->StartActorEditor(); 
	} else if (std::strcmp(editorToEnter, "GibEditor") == 0) {
		g_pMainMenuGUI->StartGibEditor();
	} else if (std::strcmp(editorToEnter, "SceneEditor") == 0) {
		g_pMainMenuGUI->StartSceneEditor();
	} else if (std::strcmp(editorToEnter, "AreaEditor") == 0) {
		g_pMainMenuGUI->StartAreaEditor();
	} else if (std::strcmp(editorToEnter, "AssemblyEditor") == 0) {
		g_pMainMenuGUI->StartAssemblyEditor();
	} else {
		g_LaunchIntoEditor = false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Load and display the into, title and menu sequence.
/// </summary>
/// <returns></returns>
bool PlayIntroTitle() {
    // Disable time averaging since it can make the music timing creep off target.
    g_TimerMan.EnableAveraging(false);
    
    // Untrap the mouse and keyboard
    g_UInputMan.DisableKeys(false);
    g_UInputMan.TrapMousePos(false);

    // Stop all audio
    g_AudioMan.StopAll();

    g_FrameMan.ClearBackBuffer32();
    g_FrameMan.FlipFrameBuffers();
    int resX = g_FrameMan.GetResX();
    int resY = g_FrameMan.GetResY();

    // The fade-in/out screens
    BITMAP *pFadeScreen = create_bitmap_ex(32, resX, resY);
    clear_to_color(pFadeScreen, 0);
    int fadePos = 0;

    // Load the Intro slides
    BITMAP **apIntroSlides = new BITMAP *[SLIDECOUNT];
    ContentFile introSlideFile("Base.rte/GUIs/Title/Intro/IntroSlideA.png");
    apIntroSlides[SLIDEPAST] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideB.png");
    apIntroSlides[SLIDENOW] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideC.png");
    apIntroSlides[SLIDEVR] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideD.png");
    apIntroSlides[SLIDETRAVEL] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideE.png");
    apIntroSlides[SLIDEALIENS] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideF.png");
    apIntroSlides[SLIDETRADE] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideG.png");
    apIntroSlides[SLIDEPEACE] = introSlideFile.LoadAndReleaseBitmap();
    introSlideFile.SetDataPath("Base.rte/GUIs/Title/Intro/IntroSlideH.png");
    apIntroSlides[SLIDEFRONTIER] = introSlideFile.LoadAndReleaseBitmap();

    ContentFile alphaFile;
    BITMAP *pAlpha = 0;

    MOSParticle *pDRLogo = new MOSParticle();
    pDRLogo->Create(ContentFile("Base.rte/GUIs/Title/Intro/DRLogo5x.png"));
    pDRLogo->SetWrapDoubleDrawing(false);

	MOSParticle *pFMODLogo = new MOSParticle();
	pFMODLogo->Create(ContentFile("Base.rte/GUIs/Title/Intro/FMODLogo.png"));
	pFMODLogo->SetWrapDoubleDrawing(false);

    SceneLayer *pBackdrop = new SceneLayer();
    pBackdrop->Create(ContentFile("Base.rte/GUIs/Title/Nebula.png"), false, Vector(), false, false, Vector(0, -1.0));//startYOffset + resY));
    float backdropScrollRatio = 1.0F / 3.0F;

    MOSParticle *pTitle = new MOSParticle();
    pTitle->Create(ContentFile("Base.rte/GUIs/Title/Title.png"));
    pTitle->SetWrapDoubleDrawing(false);
    // Logo glow effect
    MOSParticle *pTitleGlow = new MOSParticle();
    pTitleGlow->Create(ContentFile("Base.rte/GUIs/Title/TitleGlow.png"));
    pTitleGlow->SetWrapDoubleDrawing(false);
    // Add alpha
    alphaFile.SetDataPath("Base.rte/GUIs/Title/TitleAlpha.png");
    set_write_alpha_blender();
    draw_trans_sprite(pTitle->GetSpriteFrame(0), alphaFile.GetAsBitmap(), 0, 0);

    MOSParticle *pPlanet = new MOSParticle();
    pPlanet->Create(ContentFile("Base.rte/GUIs/Title/Planet.png"));
    pPlanet->SetWrapDoubleDrawing(false);
    // Add alpha
    alphaFile.SetDataPath("Base.rte/GUIs/Title/PlanetAlpha.png");
    set_write_alpha_blender();
    draw_trans_sprite(pPlanet->GetSpriteFrame(0), alphaFile.GetAsBitmap(), 0, 0);

    MOSParticle *pMoon = new MOSParticle();
    pMoon->Create(ContentFile("Base.rte/GUIs/Title/Moon.png"));
    pMoon->SetWrapDoubleDrawing(false);
    // Add alpha
    alphaFile.SetDataPath("Base.rte/GUIs/Title/MoonAlpha.png");
    set_write_alpha_blender();
    draw_trans_sprite(pMoon->GetSpriteFrame(0), alphaFile.GetAsBitmap(), 0, 0);

    MOSRotating *pStation = new MOSRotating();
    pStation->Create(ContentFile("Base.rte/GUIs/Title/Station.png"));
    pStation->SetWrapDoubleDrawing(false);

	MOSRotating *pPioneerCapsule = new MOSRotating();
	pPioneerCapsule->Create(ContentFile("Base.rte/GUIs/Title/Promo/PioneerCapsule.png"));
	pPioneerCapsule->SetWrapDoubleDrawing(false);

	MOSRotating *pPioneerScreaming = new MOSRotating();
	pPioneerScreaming->Create(ContentFile("Base.rte/GUIs/Title/Promo/PioneerScreaming.png"));
	pPioneerScreaming->SetWrapDoubleDrawing(false);

	MOSParticle * pFirePuffLarge = dynamic_cast<MOSParticle *>(g_PresetMan.GetEntityPreset("MOSParticle", "Fire Puff Large", "Base.rte")->Clone());
	MOSParticle * pFirePuffMedium = dynamic_cast<MOSParticle *>(g_PresetMan.GetEntityPreset("MOSParticle", "Fire Puff Medium", "Base.rte")->Clone());

	long long lastShake = 0;
	long long lastPuffFrame = 0;
	long long lastPuff = 0;
	bool puffActive = false;
	int puffFrame = 0;
	int puffCount = 0;

	Vector shakeOffset(0, 0);

    // Generate stars!
    int starArea = resX * pBackdrop->GetBitmap()->h;
    int starCount = starArea / 1000;
    ContentFile starSmallFile("Base.rte/GUIs/Title/Stars/StarSmall.png");
    ContentFile starLargeFile("Base.rte/GUIs/Title/Stars/StarLarge.png");
    ContentFile starHugeFile("Base.rte/GUIs/Title/Stars/StarHuge.png");
    int starSmallBitmapCount = 4;
    int starLargeBitmapCount = 1;
    int starHugeBitmapCount = 2;
    BITMAP **apStarSmallBitmaps = starSmallFile.GetAsAnimation(starSmallBitmapCount);
    BITMAP **apStarLargeBitmaps = starLargeFile.GetAsAnimation(starLargeBitmapCount);
    BITMAP **apStarHugeBitmaps = starHugeFile.GetAsAnimation(starHugeBitmapCount);
    Star *aStars = new Star[starCount];
    StarSize size;

    for (int star = 0; star < starCount; ++star) {
        if (PosRand() < 0.95) {
            aStars[star].m_Size = StarSmall;
            aStars[star].m_pBitmap = apStarSmallBitmaps[SelectRand(0, starSmallBitmapCount - 1)];
            aStars[star].m_Intensity = RangeRand(0.001, 0.5);
        }
        else if (PosRand() < 0.85) {
            aStars[star].m_Size = StarLarge;
            aStars[star].m_pBitmap = apStarLargeBitmaps[SelectRand(0, starLargeBitmapCount - 1)];
            aStars[star].m_Intensity = RangeRand(0.6, 1.0);
        }
        else {
            aStars[star].m_Size = StarHuge;
            aStars[star].m_pBitmap = apStarHugeBitmaps[SelectRand(0, starLargeBitmapCount - 1)];
            aStars[star].m_Intensity = RangeRand(0.9, 1.0);
        }
        aStars[star].m_Pos.SetXY(resX * PosRand(), pBackdrop->GetBitmap()->h * PosRand());//resY * PosRand());
        aStars[star].m_Pos.Floor();
        // To match the nebula scroll
        aStars[star].m_ScrollRatio = backdropScrollRatio;
    }

    // Font stuff
    GUISkin *pSkin = g_pMainMenuGUI->GetGUIControlManager()->GetSkin();
    GUIFont *pFont = pSkin->GetFont("fatfont.png");
    AllegroBitmap backBuffer(g_FrameMan.GetBackBuffer32());
    int yTextPos = 0;
    // Timers
    Timer totalTimer, songTimer, sectionTimer;
    // Convenience for how many seconds have elapsed on each section
    double elapsed = 0;
    // How long each section is, in s
    double duration = 0, scrollDuration = 0, scrollStart = 0, slideFadeInDuration = 0.5, slideFadeOutDuration = 0.5;
    // Progress made on a section, from 0.0 to 1.0
    double sectionProgress = 0, scrollProgress = 0;
    // When a section is supposed to end, relative to the song timer
    long sectionSongEnd = 0;

    // Scrolling data
	bool keyPressed = false;
	bool sectionSwitch = true;
    float planetRadius = 240;
    float orbitRadius = 274;
    float orbitRotation = c_HalfPI - c_EighthPI;
    // Set the start so that the nebula is fully scrolled up
    int startYOffset = pBackdrop->GetBitmap()->h / backdropScrollRatio - (resY / backdropScrollRatio);
    int titleAppearYOffset = 900;
    int preMenuYOffset = 100;
    int topMenuYOffset = 0;
    // So planet is centered on the screen regardless of resolution
    int planetViewYOffset = 325 + planetRadius - (resY / 2);
    // Set Y to title offset so there's no jump when entering the main menu
    Vector scrollOffset(0, preMenuYOffset), planetPos, stationOffset, capsuleOffset, slidePos;

    totalTimer.Reset();
    sectionTimer.Reset();
    while (!g_Quit && g_IntroState != END && !g_ResumeActivity) {
        keyPressed = g_UInputMan.AnyStartPress();
//        g_Quit = key[KEY_ESC];
        // Reset the key press states
        g_UInputMan.Update();
        g_TimerMan.Update();
        g_TimerMan.UpdateSim();
        g_ConsoleMan.Update();

		g_AudioMan.Update();

		if (sectionSwitch) { sectionTimer.Reset(); }
        elapsed = sectionTimer.GetElapsedRealTimeS();
        // Calculate the normalized sectionProgress scalar
        sectionProgress = duration <= 0 ? 0 : (elapsed / duration);
        // Clamp the sectionProgress scalar
        sectionProgress = min(sectionProgress, 0.9999);

		if (g_NetworkServer.IsServerModeEnabled()) { g_NetworkServer.Update(); }
			
        ////////////////////////////////
        // Scrolling logic

        if (g_IntroState >= FADEIN && g_IntroState <= PRETITLE)
        {
            if (g_IntroState == FADEIN && sectionSwitch)
            {
                songTimer.SetElapsedRealTimeS(0.05);
                scrollStart = songTimer.GetElapsedRealTimeS();
                // 66.6s This is the end of PRETITLE
                scrollDuration = 66.6 - scrollStart;
            }
            scrollProgress = (double)(songTimer.GetElapsedRealTimeS() - scrollStart) / (double)scrollDuration;
            scrollOffset.m_Y = LERP(0, 1.0, startYOffset, titleAppearYOffset,  scrollProgress);
        }
        // Scroll after the slide-show
        else if (g_IntroState >= TITLEAPPEAR && g_IntroState <= PLANETSCROLL)
        {
            if (g_IntroState == TITLEAPPEAR && sectionSwitch)
            {
                scrollStart = songTimer.GetElapsedRealTimeS();
                // This is the end of PLANETSCROLL
                scrollDuration = 92.4 - scrollStart;
            }
            scrollProgress = (double)(songTimer.GetElapsedRealTimeS() - scrollStart) / (double)scrollDuration;
//            scrollOffset.m_Y = LERP(scrollStart, 92.4, titleAppearYOffset, preMenuYOffset, songTimer.GetElapsedRealTimeS());
            scrollOffset.m_Y = EaseOut(titleAppearYOffset, preMenuYOffset, scrollProgress);
        }
        // Scroll the last bit to reveal the menu appears
        else if (g_IntroState == MENUAPPEAR)
        {
            scrollOffset.m_Y = EaseOut(preMenuYOffset, topMenuYOffset, sectionProgress);
        }
        // Scroll down to the planet screen
        else if (g_IntroState == MAINTOSCENARIO || g_IntroState == MAINTOCAMPAIGN)
        {
            scrollOffset.m_Y = EaseOut(topMenuYOffset, planetViewYOffset, sectionProgress);
        }
        // Scroll back up to the main screen from campaign
        else if (g_IntroState == PLANETTOMAIN)
        {
            scrollOffset.m_Y = EaseOut(planetViewYOffset, topMenuYOffset, sectionProgress);
        }

        ///////////////////////////////////////////////////////
        // DRL Logo drawing

        if (g_IntroState >= LOGOFADEIN && g_IntroState <= LOGOFADEOUT)
        {
            // Draw the early build notice
            g_FrameMan.ClearBackBuffer32();
            pDRLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
            pDRLogo->Draw(g_FrameMan.GetBackBuffer32());
        }

		///////////////////////////////////////////////////////
		// FMOD Logo drawing

		if (g_IntroState >= FMODLOGOFADEIN && g_IntroState <= FMODLOGOFADEOUT) {
			g_FrameMan.ClearBackBuffer32();
			pFMODLogo->SetPos(Vector(g_FrameMan.GetResX() / 2, (g_FrameMan.GetResY() / 2) - 35));
			pFMODLogo->Draw(g_FrameMan.GetBackBuffer32());
		}

        ///////////////////////////////////////////////////////
        // Notice drawing

        if (g_IntroState >= NOTICEFADEIN && g_IntroState <= NOTICEFADEOUT)
        {
            // Draw the early build notice
            g_FrameMan.ClearBackBuffer32();
            yTextPos = g_FrameMan.GetResY() / 3;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("N O T E :"), GUIFont::Centre);
            yTextPos += pFont->GetFontHeight() * 2;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("This game plays great with up to FOUR people on a BIG-SCREEN TV!"), GUIFont::Centre);
            yTextPos += pFont->GetFontHeight() * 2;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("So invite some friends/enemies over, plug in those USB controllers, and have a blast -"), GUIFont::Centre);
            yTextPos += pFont->GetFontHeight() * 4;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, string("Press ALT+ENTER to toggle FULLSCREEN mode"), GUIFont::Centre);

            // Draw the copyright notice
            yTextPos = g_FrameMan.GetResY() - pFont->GetFontHeight();
            char copyRight[512];
            sprintf_s(copyRight, sizeof(copyRight), "Cortex Command is TM and %c 2017 Data Realms, LLC", -35);
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, copyRight, GUIFont::Centre);
        }

        //////////////////////////////////////////////////////////
        // Scene drawing

        if (g_IntroState >= FADEIN)
        {
            g_FrameMan.ClearBackBuffer32();

			Box backdropBox;
            pBackdrop->Draw(g_FrameMan.GetBackBuffer32(), backdropBox, scrollOffset * backdropScrollRatio);

            Vector starDrawPos;
            for (int star = 0; star < starCount; ++star)
            {
                size = aStars[star].m_Size;
                int intensity = 185 * aStars[star].m_Intensity + (size == StarSmall ? 35 : 70) * PosRand();
                set_screen_blender(intensity, intensity, intensity, intensity);
                starDrawPos.SetXY(aStars[star].m_Pos.m_X, aStars[star].m_Pos.m_Y - scrollOffset.m_Y * aStars[star].m_ScrollRatio);
                draw_trans_sprite(g_FrameMan.GetBackBuffer32(), aStars[star].m_pBitmap, starDrawPos.GetFloorIntX(), starDrawPos.GetFloorIntY());
            }

            planetPos.SetXY(g_FrameMan.GetResX() / 2, 567 - scrollOffset.GetFloorIntY());
            pMoon->SetPos(Vector(planetPos.m_X + 200, 364 - scrollOffset.GetFloorIntY() * 0.60));
            pPlanet->SetPos(planetPos);

            pMoon->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);
            pPlanet->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);

			// Manually shake our shakeOffset to randomize some effects
			if (g_TimerMan.GetAbsoluteTime() > lastShake + 50000)
			{
				shakeOffset.m_X = RangeRand(-3, 3);
				shakeOffset.m_Y = RangeRand(-3, 3);
				lastShake = g_TimerMan.GetAbsoluteTime();
			}

			// Tell the menu that PP promo is off
			g_pMainMenuGUI->DisablePioneerPromoButton();


			// Draw pioneer promo capsule
			if (g_IntroState < MAINTOCAMPAIGN && orbitRotation < -c_PI * 1.27 && orbitRotation > -c_PI * 1.85)
			{
				// Start drawing pioneer capsule
				// Slowly decrease radius to show that the capsule is falling
				float radiusperc = 1 - ((fabs(orbitRotation) - (1.27 * c_PI)) / (0.35 * c_PI) / 4);
				// Slowly decrease size to make the capsule disappear after a while
				float sizeperc = 1 - ((fabs(orbitRotation) - (1.27 * c_PI)) / (0.35 * c_PI) / 1.5);

				// Rotate, place and draw capsule
				capsuleOffset.SetXY(orbitRadius * radiusperc, 0);
				capsuleOffset.RadRotate(orbitRotation);
				pPioneerCapsule->SetScale(sizeperc);
				pPioneerCapsule->SetPos(planetPos + capsuleOffset);
				pPioneerCapsule->SetRotAngle(orbitRotation);
				pPioneerCapsule->Draw(g_FrameMan.GetBackBuffer32());
			}

			// Enable promo clickables only if we're in main menu and the station is at the required location (under the menu)
			if (g_IntroState == MENUACTIVE && g_pMainMenuGUI->AllowPioneerPromo() &&  orbitRotation < -c_PI * 1.25 && orbitRotation > -c_PI * 1.95)
			{
				// After capsule flew some time, start showing angry pioneer
				if (orbitRotation < -c_PI * 1.32 && orbitRotation > -c_PI * 1.65)
				{
					Vector pioneerScreamPos = planetPos - Vector(320 - 130, 320 + 44);

					// Draw line to indicate that the screaming guy is the one in the drop pod
					drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
					g_pScenarioGUI->SetPlanetInfo(Vector(0,0), planetRadius);
					g_pScenarioGUI->DrawScreenLineToSitePoint(g_FrameMan.GetBackBuffer32(), pioneerScreamPos, pPioneerCapsule->GetPos(), makecol(255, 255, 255), -1, -1, 40, 0.20);
					drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);

					// Draw pioneer
					pPioneerScreaming->SetPos(pioneerScreamPos + shakeOffset);
					pPioneerScreaming->Draw(g_FrameMan.GetBackBuffer32());

					// Enable the promo banner and tell the menu where it can be clicked
					g_pMainMenuGUI->EnablePioneerPromoButton();

					Box promoBox(pioneerScreamPos.m_X - 125, pioneerScreamPos.m_Y - 70, pioneerScreamPos.m_X + 125, pioneerScreamPos.m_Y + 70);
					g_pMainMenuGUI->SetPioneerPromoBox(promoBox);
				} 
			}
				
			// Place, rotate and draw station
			stationOffset.SetXY(orbitRadius, 0);
			stationOffset.RadRotate(orbitRotation);
			pStation->SetPos(planetPos + stationOffset);
			pStation->SetRotAngle(-c_HalfPI + orbitRotation);
			pStation->Draw(g_FrameMan.GetBackBuffer32());

			// Start explosion effects to show that there's something wrong with the station
			// but only if we're not in campaign
			if (g_IntroState < MAINTOCAMPAIGN && orbitRotation < -c_PI * 1.25 && orbitRotation > -c_TwoPI)
			{
				// Add explosions delay and count them
				if (g_TimerMan.GetAbsoluteTime() > lastPuff + 1000000)
				{
					lastPuff = g_TimerMan.GetAbsoluteTime();
					puffActive = true;
					puffCount++;
				}

				// If explosion was authorized
				if (puffActive)
				{
					// First explosion is big while other are smaller
					if (puffCount == 1)
					{
						pFirePuffLarge->SetPos(planetPos + stationOffset);
						if (g_TimerMan.GetAbsoluteTime() > lastPuffFrame + 50000)
						{
							lastPuffFrame = g_TimerMan.GetAbsoluteTime();
							puffFrame++;

							if (puffFrame >= pFirePuffLarge->GetFrameCount())
							{
								// Manually reset frame counters and disable other explosions until it's time
								puffFrame = 0;
								puffActive = 0;
							}

							pFirePuffLarge->SetFrame(puffFrame);
						}
						pFirePuffLarge->Draw(g_FrameMan.GetBackBuffer32());
					} else {
						pFirePuffMedium->SetPos(planetPos + stationOffset + shakeOffset);
						if (g_TimerMan.GetAbsoluteTime() > lastPuffFrame + 50000)
						{
							lastPuffFrame = g_TimerMan.GetAbsoluteTime();
							puffFrame++;

							if (puffFrame >= pFirePuffLarge->GetFrameCount())
							{
								// Manually reset frame counters and disable other explosions until it's time
								puffFrame = 0;
								puffActive = 0;
							}

							pFirePuffMedium->SetFrame(puffFrame);
						}
						pFirePuffMedium->Draw(g_FrameMan.GetBackBuffer32());
					}
				}
			} else {
				//Reset explosions counter
				puffCount = 0;
			}

			orbitRotation -= 0.0020; //0.0015

            // Keep the rotation angle from getting too large
            if (orbitRotation < -c_TwoPI)
                orbitRotation += c_TwoPI;
            g_StationOffsetX = stationOffset.m_X;
            g_StationOffsetY = stationOffset.m_Y;
        }

        /////////////////////////////
        // Game Logo drawing

        if ((g_IntroState >= TITLEAPPEAR && g_IntroState < SCENARIOFADEIN) || g_IntroState == MAINTOCAMPAIGN)
        {
            if (g_IntroState == TITLEAPPEAR)
                pTitle->SetPos(Vector(resX / 2, (resY / 2) - 20));
            else if (g_IntroState == PLANETSCROLL && sectionProgress > 0.5)
                pTitle->SetPos(Vector(resX / 2, EaseIn((resY / 2) - 20, 120, (sectionProgress - 0.5) / 0.5)));//LERP(0.5, 1.0, (resY / 2) - 20, 120, sectionProgress)));
            else if (g_IntroState == MENUAPPEAR)
                pTitle->SetPos(Vector(resX / 2, EaseOut(120, 64, sectionProgress)));
            else if (g_IntroState == MAINTOSCENARIO || g_IntroState == MAINTOCAMPAIGN)
                pTitle->SetPos(Vector(resX / 2, EaseOut(64, -150, sectionProgress)));
            else if (g_IntroState >= MENUAPPEAR)
                pTitle->SetPos(Vector(resX / 2, 64));

            pTitleGlow->SetPos(pTitle->GetPos());

            pTitle->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawAlpha);
            // Screen blend the title glow on top, with some flickering in its intensity
            int blendAmount = 220 + 35 * NormalRand();
            set_screen_blender(blendAmount, blendAmount, blendAmount, blendAmount);
            pTitleGlow->Draw(g_FrameMan.GetBackBuffer32(), Vector(), g_DrawTrans);
        }

        /////////////////////////////
        // Menu drawing

        // Main Menu updating and drawing, behind title
        if (g_IntroState >= MENUAPPEAR)
        {
            if (g_IntroState == MENUAPPEAR)
            {
				// TODO: some fancy transparency effect here
/*
                g_pMainMenuGUI->Update();
                clear_to_color(pFadeScreen, 0xFFFF00FF);
                g_pMainMenuGUI->Draw(pFadeScreen);
                fadePos = 255 * sectionProgress;
                set_trans_blender(fadePos, fadePos, fadePos, fadePos);
                draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
*/
            }
            else if (g_IntroState == MENUACTIVE)
            {
                g_pMainMenuGUI->Update();
                g_pMainMenuGUI->Draw(g_FrameMan.GetBackBuffer32());
            }
        }

        // Scenario setup menu update and drawing
        if (g_IntroState == SCENARIOMENU)
        {
            g_pScenarioGUI->SetPlanetInfo(planetPos, planetRadius);
            g_pScenarioGUI->Update();
            g_pScenarioGUI->Draw(g_FrameMan.GetBackBuffer32());
        }

        // Metagame menu update and drawing
        if (g_IntroState == CAMPAIGNPLAY)
        {
            g_MetaMan.GetGUI()->SetPlanetInfo(planetPos, planetRadius);
            g_MetaMan.Update();
            g_MetaMan.Draw(g_FrameMan.GetBackBuffer32());
        }

        ////////////////////////////////////
        // Slides drawing

        if (g_IntroState >= SHOWSLIDE1 && g_IntroState <= SHOWSLIDE8)
        {
            int slide = g_IntroState - SHOWSLIDE1;
            Vector slideCenteredPos((resX / 2) - (apIntroSlides[slide]->w / 2), (resY / 2) - (apIntroSlides[slide]->h / 2));

            // Screen wide slide
            if (apIntroSlides[slide]->w <= resX)
                slidePos.m_X = (resX / 2) - (apIntroSlides[slide]->w / 2);
            // The slides wider than the screen, pan sideways
            else
            {
                if (elapsed < slideFadeInDuration)
                    slidePos.m_X = 0;
                else if (elapsed < duration - slideFadeOutDuration)
                    slidePos.m_X = EaseInOut(0, resX - apIntroSlides[slide]->w, (elapsed - slideFadeInDuration) / (duration - slideFadeInDuration - slideFadeOutDuration));
                else
                    slidePos.m_X = resX - apIntroSlides[slide]->w;
            }

            // TEMP?
            slidePos.m_Y = slideCenteredPos.m_Y;
            // TEMP?
            if (elapsed < slideFadeInDuration)
            {
                fadePos = EaseOut(0, 255, elapsed / slideFadeInDuration);
//                slidePos.m_Y = EaseOut(slideCenteredPos.m_Y - slideFadeDistance, slideCenteredPos.m_Y, elapsed / slideFadeInDuration);
            }
            else if (elapsed < duration - slideFadeOutDuration)
            {
                fadePos = 255;
                slidePos.m_Y = slideCenteredPos.m_Y;
            }
            else
            {
                fadePos = EaseIn(255, 0, (elapsed - duration + slideFadeOutDuration) / slideFadeOutDuration);
//                slidePos.m_Y = EaseIn(slideCenteredPos.m_Y, slideCenteredPos.m_Y + slideFadeDistance, (elapsed - duration + slideFadeOutDuration) / slideFadeOutDuration);
            }

            if (fadePos > 0)
            {
                set_trans_blender(fadePos, fadePos, fadePos, fadePos);
                draw_trans_sprite(g_FrameMan.GetBackBuffer32(), apIntroSlides[slide], slidePos.m_X, slidePos.m_Y);
            }
        }

        //////////////////////////////////////////////////////////
        // Intro sequence logic

        if (g_IntroState == START)
        {
            g_IntroState = LOGOFADEIN;
            sectionSwitch = true;
        }
        else if (g_IntroState == LOGOFADEIN)
        {
            if (sectionSwitch)
            {
                // Play juicy logo signature jingle/sound
				g_GUISound.SplashSound()->Play();
                // Black fade
                clear_to_color(pFadeScreen, 0);
                duration = 0.25;
                sectionSwitch = false;
            }

            fadePos = 255 - (255 * sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration)
            {
                g_IntroState = LOGODISPLAY;
                sectionSwitch = true;
            }
            else if (keyPressed)
            {
                g_IntroState = LOGOFADEOUT;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == LOGODISPLAY)
        {
            if (sectionSwitch)
            {
                duration = 2.0;
                sectionSwitch = false;
            }
            if (elapsed > duration || keyPressed)
            {
                g_IntroState = LOGOFADEOUT;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == LOGOFADEOUT)
        {
            if (sectionSwitch)
            {
                // Black fade
                clear_to_color(pFadeScreen, 0);
                duration = 0.25;
                sectionSwitch = false;
            }

            fadePos = 255 * sectionProgress;
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration || keyPressed)
            {
                g_IntroState = FMODLOGOFADEIN;
                sectionSwitch = true;
            }
        }
		else if (g_IntroState == FMODLOGOFADEIN) {
			if (sectionSwitch) {
				// Black fade
				clear_to_color(pFadeScreen, 0);
				duration = 0.25;
				sectionSwitch = false;
			}

			fadePos = 255 - (255 * sectionProgress);
			set_trans_blender(fadePos, fadePos, fadePos, fadePos);
			draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

			if (elapsed >= duration) {
				g_IntroState = FMODLOGODISPLAY;
				sectionSwitch = true;
			} else if (keyPressed) {
				g_IntroState = FMODLOGOFADEOUT;
				sectionSwitch = true;
			}
		} else if (g_IntroState == FMODLOGODISPLAY) {
			if (sectionSwitch) {
				duration = 2.0;
				sectionSwitch = false;
			}
			if (elapsed > duration || keyPressed) {
				g_IntroState = FMODLOGOFADEOUT;
				sectionSwitch = true;
			}
		} else if (g_IntroState == FMODLOGOFADEOUT) {
			if (sectionSwitch) {
				// Black fade
				clear_to_color(pFadeScreen, 0);
				duration = 0.25;
				sectionSwitch = false;
			}
			fadePos = 255 * sectionProgress;
			set_trans_blender(fadePos, fadePos, fadePos, fadePos);
			draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

			if (elapsed >= duration || keyPressed) {
				g_IntroState = NOTICEFADEIN;
				sectionSwitch = true;
			}
		}
        else if (g_IntroState == NOTICEFADEIN)
        {
            if (sectionSwitch)
            {
                // Black fade
                clear_to_color(pFadeScreen, 0);
                duration = 0.5;
                sectionSwitch = false;
            }

            fadePos = 255 - (255 * sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration)
            {
                g_IntroState = NOTICEDISPLAY;
                sectionSwitch = true;
            }
            else if (keyPressed)
            {
                g_IntroState = NOTICEFADEOUT;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == NOTICEDISPLAY)
        {
            if (sectionSwitch)
            {
                duration = 7.0;
                sectionSwitch = false;
            }
            if (elapsed > duration || keyPressed)
            {
                g_IntroState = NOTICEFADEOUT;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == NOTICEFADEOUT)
        {
            if (sectionSwitch)
            {
                // Black fade
                clear_to_color(pFadeScreen, 0);
                duration = 0.5;
                sectionSwitch = false;
            }

            fadePos = 255 * sectionProgress;
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration || keyPressed)
            {
                g_IntroState = FADEIN;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == FADEIN)
        {
            if (sectionSwitch)
            {
                // Start scroll at the bottom
                scrollOffset.m_Y = startYOffset;
                // Black fade
                clear_to_color(pFadeScreen, 0);

                duration = 1.0;
                sectionSwitch = false;

                // Play intro music
                g_AudioMan.PlayMusic("Base.rte/Music/Hubnester/ccintro.ogg", 0);
                g_AudioMan.SetMusicPosition(0.05);
                // Override music volume setting for the intro if it's set to anything
                if (g_AudioMan.GetMusicVolume() > 0.1)
                    g_AudioMan.SetTempMusicVolume(1.0);
//                songTimer.Reset();
                songTimer.SetElapsedRealTimeS(0.05);
            }

            fadePos = 255 - (255 * sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration)
            {
                g_IntroState = SPACEPAUSE1;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SPACEPAUSE1)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 3.7;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE1;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE1)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 11.4;
                slideFadeInDuration = 2.0;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (elapsed > 1.25)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "At the end of humanity's darkest century...", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE2;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE2)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 17.3;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 2.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (elapsed < duration - 1.75)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...a curious symbiosis between man and machine emerged.", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE3;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE3)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 25.1;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (/*elapsed > 0.75 && */sectionProgress < 0.49)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "This eventually enabled humans to leave their natural bodies...", GUIFont::Centre);
            else if (sectionProgress > 0.51)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...and to free their minds from obsolete constraints.", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE4;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE4)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 31.3;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "With their brains sustained by artificial means, space travel also became feasible.", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE5;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE5)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 38.0;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Other civilizations were encountered...", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE6;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE6)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 44.1;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...and peaceful intragalactic trade soon established.", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE7;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE7)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 51.5;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Now, the growing civilizations create a huge demand for resources...", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = SHOWSLIDE8;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SHOWSLIDE8)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 64.5;
                slideFadeInDuration = 0.5;
                slideFadeOutDuration = 0.5;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2) + (apIntroSlides[g_IntroState - SHOWSLIDE1]->h / 2) + 12;
            if (sectionProgress < 0.30)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "...which can only be satisfied by the ever-expanding frontier.", GUIFont::Centre);
            else if (sectionProgress > 0.33 && sectionProgress < 0.64)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Competition is brutal and anything goes in this galactic gold rush.", GUIFont::Centre);
            else if (sectionProgress > 0.67)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "You will now join it on a venture to an untapped planet...", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = PRETITLE;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == PRETITLE)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 66.6;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            yTextPos = (g_FrameMan.GetResY() / 2);
            if (elapsed > 0.05)
                pFont->DrawAligned(&backBuffer, g_FrameMan.GetResX() / 2, yTextPos, "Prepare to assume...", GUIFont::Centre);

            if (elapsed >= duration)
            {
                g_IntroState = TITLEAPPEAR;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == TITLEAPPEAR)
        {
            if (sectionSwitch)
            {
                // White fade
                clear_to_color(pFadeScreen, 0xFFFFFFFF);
                sectionSongEnd = 68.2;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            fadePos = LERP(0, 0.5, 255, 0, sectionProgress);
            if (fadePos >= 0)
            {
                set_trans_blender(fadePos, fadePos, fadePos, fadePos);
                draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);
            }

            if (elapsed >= duration)
            {
                g_IntroState = PLANETSCROLL;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == PLANETSCROLL)
        {
            if (sectionSwitch)
            {
                sectionSongEnd = 92.4;
                duration = sectionSongEnd - songTimer.GetElapsedRealTimeS();
                sectionSwitch = false;
            }

            if (elapsed >= duration)
            {
                g_IntroState = PREMENU;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == PREMENU)
        {
            if (sectionSwitch)
            {
                duration = 3.0;
                sectionSwitch = false;
                scrollOffset.m_Y = preMenuYOffset;
            }

            if (elapsed >= duration || keyPressed)
            {
                g_IntroState = MENUAPPEAR;

                sectionSwitch = true;
            }
        }
        else if (g_IntroState == MENUAPPEAR)
        {
            if (sectionSwitch)
            {
				duration = 1.0F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;
                scrollOffset.m_Y = preMenuYOffset;

                // Play the main menu ambient
                g_AudioMan.PlayMusic("Base.rte/Music/Hubnester/ccmenu.ogg", -1);
            }

            if (elapsed >= duration || g_NetworkServer.IsServerModeEnabled())
            {
                g_IntroState = MENUACTIVE;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == MENUACTIVE)
        {
            if (sectionSwitch)
            {
                scrollOffset.m_Y = topMenuYOffset;
                // Fire up the menu
                g_pMainMenuGUI->SetEnabled(true);
                // Indicate that we're now in the main menu
                g_InActivity = false;

                sectionSwitch = false;
            }

            // Detect quitting of the program from the menu button
            g_Quit = g_Quit || g_pMainMenuGUI->QuitProgram();

            // Detect if a scenario mode has been commanded to start
            if (g_pMainMenuGUI->ScenarioStarted())
            {
                g_IntroState = MAINTOSCENARIO;
                sectionSwitch = true;
            }

            // Detect if a campaign mode has been commanded to start
            if (g_pMainMenuGUI->CampaignStarted())
            {
                g_IntroState = MAINTOCAMPAIGN;
                sectionSwitch = true;
            }

            // Detect if the current game has been commanded to resume
            if (g_pMainMenuGUI->ActivityResumed())
                g_ResumeActivity = true;

            // Detect if a game has been commanded to restart
            if (g_pMainMenuGUI->ActivityRestarted())
            {
                // Make sure the scene is going to be reset with the new parameters
                g_ResetActivity = true;

                g_IntroState = FADESCROLLOUT;
                sectionSwitch = true;
            }

			if (g_NetworkServer.IsServerModeEnabled())
			{
				EnterMultiplayerLobby();
				g_IntroState = FADESCROLLOUT;
				sectionSwitch = true;
			}
        }
        else if (g_IntroState == MAINTOSCENARIO)
        {
            if (sectionSwitch)
            {
                duration = 2.0F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;

                // Reset the Scenario menu
                g_pScenarioGUI->SetEnabled(true);

                // Play the scenario music with juicy start sound
                g_GUISound.SplashSound()->Play();
                g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/thisworld5.ogg", -1);
            }

            if (elapsed >= duration || g_NetworkServer.IsServerModeEnabled())// || keyPressed)
            {
                g_IntroState = SCENARIOMENU;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SCENARIOFADEIN)
        {
            if (sectionSwitch)
            {
                // Scroll to planet pos
                scrollOffset.m_Y = planetViewYOffset;
                // Black fade
                clear_to_color(pFadeScreen, 0);

				duration = 1.0F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;
            }

            fadePos = 255 - (255 * sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration)
            {
                g_IntroState = SCENARIOMENU;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == SCENARIOMENU)
        {
            if (sectionSwitch)
            {
                scrollOffset.m_Y = planetViewYOffset;
                sectionSwitch = false;
            }

            // Detect quitting of the program from the menu button
            g_Quit = g_Quit || g_pScenarioGUI->QuitProgram();

            // Detect if user wants to go back to main menu
            if (g_pScenarioGUI->BackToMain())
            {
                g_IntroState = PLANETTOMAIN;
                sectionSwitch = true;
            }

            // Detect if the current game has been commanded to resume
            if (g_pScenarioGUI->ActivityResumed())
                g_ResumeActivity = true;

            // Detect if a game has been commanded to restart
            if (g_pScenarioGUI->ActivityRestarted())
            {
                // Make sure the scene is going to be reset with the new parameters
                g_ResetActivity = true;

                g_IntroState = FADEOUT;
                sectionSwitch = true;
            }

			// In server mode once we exited to main or scenario menu we need to start Lobby activity 
			if (g_NetworkServer.IsServerModeEnabled())
			{
				EnterMultiplayerLobby();
				g_IntroState = FADEOUT;
				sectionSwitch = true;
			}
        }
        else if (g_IntroState == MAINTOCAMPAIGN)
        {
            if (sectionSwitch)
            {
				duration = 2.0F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;

                // Play the campaign music with Meta sound start
				g_GUISound.SplashSound()->Play();
                g_AudioMan.PlayMusic("Base.rte/Music/dBSoundworks/thisworld5.ogg", -1);
            }

            if (elapsed >= duration)// || keyPressed)
            {
                g_IntroState = CAMPAIGNPLAY;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == CAMPAIGNFADEIN)
        {
            if (sectionSwitch)
            {
                // Scroll to campaign pos
                scrollOffset.m_Y = planetViewYOffset;
                // Black fade
                clear_to_color(pFadeScreen, 0);

				duration = 1.0F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;
            }

            fadePos = 255 - (255 * sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            if (elapsed >= duration)
            {
                g_IntroState = CAMPAIGNPLAY;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == CAMPAIGNPLAY)
        {
            if (sectionSwitch)
            {
                scrollOffset.m_Y = planetViewYOffset;
                sectionSwitch = false;
            }

            // Detect quitting of the program from the menu button
            g_Quit = g_Quit || g_MetaMan.GetGUI()->QuitProgram();

            // Detect if user wants to go back to main menu
            if (g_MetaMan.GetGUI()->BackToMain())
            {
                g_IntroState = PLANETTOMAIN;
                sectionSwitch = true;
            }

            // Detect if a game has been commanded to restart
            if (g_MetaMan.GetGUI()->ActivityRestarted())
            {
                // Make sure the scene is going to be reset with the new parameters
                g_ResetActivity = true;

                g_IntroState = FADEOUT;
                sectionSwitch = true;
            }
            // Detect if the current game has been commanded to resume
            if (g_MetaMan.GetGUI()->ActivityResumed())
                g_ResumeActivity = true;
        }
        else if (g_IntroState == PLANETTOMAIN)
        {
            if (sectionSwitch)
            {
				duration = 2.0F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;
            }

            if (elapsed >= duration)// || keyPressed)
            {
                g_IntroState = MENUACTIVE;
                sectionSwitch = true;
            }
        }
        else if (g_IntroState == FADESCROLLOUT)
        {
            if (sectionSwitch)
            {
                // Black fade
                clear_to_color(pFadeScreen, 0x00000000);
				duration = 1.5F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;
            }

            scrollOffset.m_Y = EaseIn(topMenuYOffset, 250, sectionProgress);

            fadePos = EaseIn(0, 255, sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            // Fade out the music as well
            g_AudioMan.SetTempMusicVolume(EaseIn(g_AudioMan.GetMusicVolume(), 0, sectionProgress));

            if (elapsed >= duration)
            {
                g_IntroState = END;
                sectionSwitch = true;
                g_FrameMan.ClearBackBuffer32();
            }
        }
        else if (g_IntroState == FADEOUT)
        {
            if (sectionSwitch)
            {
                // White fade
                clear_to_color(pFadeScreen, 0x00000000);
				duration = 1.5F * g_SettingsMan.GetMenuTransitionDurationMultiplier();
                sectionSwitch = false;
            }

//            scrollOffset.m_Y = EaseIn(topMenuYOffset, 250, sectionProgress);

            fadePos = EaseIn(0, 255, sectionProgress);
            set_trans_blender(fadePos, fadePos, fadePos, fadePos);
            draw_trans_sprite(g_FrameMan.GetBackBuffer32(), pFadeScreen, 0, 0);

            // Fade out the music as well
//            g_AudioMan.SetTempMusicVolume(g_AudioMan.GetMusicVolume() * 1.0 - sectionProgress);
            g_AudioMan.SetTempMusicVolume(EaseIn(g_AudioMan.GetMusicVolume(), 0, sectionProgress));

            if (elapsed >= duration)
            {
                g_IntroState = END;
                sectionSwitch = true;
                g_FrameMan.ClearBackBuffer32();
            }
        }

        ////////////////////////////////
        // Additional user input and skipping handling
        
        if (g_IntroState >= FADEIN && g_IntroState <= SHOWSLIDE8 && keyPressed)
        {
            g_IntroState = MENUAPPEAR;
            sectionSwitch = true;

            scrollOffset.m_Y = preMenuYOffset;
            orbitRotation = c_HalfPI - c_EighthPI;

			orbitRotation = -c_PI * 1.20;
        }

        // Draw the console in the menu
        g_ConsoleMan.Draw(g_FrameMan.GetBackBuffer32());

        // Wait for vertical sync before flipping frames
        vsync();
        g_FrameMan.FlipFrameBuffers();
    }

    // Clean up heap data
    destroy_bitmap(pFadeScreen); pFadeScreen = 0;
    for (int slide = 0; slide < SLIDECOUNT; ++slide)
    {
        destroy_bitmap(apIntroSlides[slide]);
        apIntroSlides[slide] = 0;
    }
    delete [] apIntroSlides; apIntroSlides = 0;
    delete pBackdrop; pBackdrop = 0;
    delete pTitle; pTitle = 0;
    delete pPlanet; pPlanet = 0;
    delete pMoon; pMoon = 0;
    delete pStation; pStation = 0;
	delete pPioneerCapsule; pPioneerCapsule = 0;
	delete pPioneerScreaming; pPioneerScreaming = 0;
	delete pFirePuffLarge; pFirePuffLarge = 0;
	delete pFirePuffMedium; pFirePuffMedium = 0;
    delete [] aStars; aStars = 0;

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Orders to reset the entire Retro Terrain Engine system next iteration.
/// </summary>
void ResetRTE() { g_ResetRTE = true; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Indicates whether the system is about to be reset before the next loop starts.
/// </summary>
/// <returns>Whether the RTE is about to reset next iteration of the loop or not.</returns>
bool IsResettingRTE() { return g_ResetRTE; }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Game simulation loop.
/// </summary>
bool RunGameLoop() {
	if (g_Quit) {
		return true;
	}
	g_PerformanceMan.ResetFrameTimer();
	g_TimerMan.EnableAveraging(true);
	g_TimerMan.PauseSim(false);

	if (g_ResetActivity) { ResetActivity(); }

	while (!g_Quit) {
		// Need to clear this out; sometimes background layers don't cover the whole back
		g_FrameMan.ClearBackBuffer8();

		// Update the real time measurement and increment
		g_TimerMan.Update();

		bool serverUpdated = false;

		// Simulation update, as many times as the fixed update step allows in the span since last frame draw
		while (g_TimerMan.TimeForSimUpdate()) {
			serverUpdated = false;
			g_PerformanceMan.NewPerformanceSample();

			// Advance the simulation time by the fixed amount
			g_TimerMan.UpdateSim();

			g_PerformanceMan.StartPerformanceMeasurement(PerformanceMan::PERF_SIM_TOTAL);

			g_UInputMan.Update();

			// It is vital that server is updated after input manager but before activity because input manager will clear received pressed and released events on next update.
			if (g_NetworkServer.IsServerModeEnabled()) {
				g_NetworkServer.Update(true);
				serverUpdated = true;
			}
			g_FrameMan.Update();
			g_AudioMan.Update();
			g_LuaMan.Update();
			g_PerformanceMan.StartPerformanceMeasurement(PerformanceMan::PERF_ACTIVITY);
			g_ActivityMan.Update();
			g_PerformanceMan.StopPerformanceMeasurement(PerformanceMan::PERF_ACTIVITY);
			g_MovableMan.Update();

			g_ActivityMan.LateUpdateGlobalScripts();

			g_ConsoleMan.Update();
			g_PerformanceMan.StopPerformanceMeasurement(PerformanceMan::PERF_SIM_TOTAL);

			if (!g_InActivity) {
				g_TimerMan.PauseSim(true);
				// If we're not in a metagame, then show main menu
				if (g_MetaMan.GameInProgress()) {
					g_IntroState = CAMPAIGNFADEIN;
				} else {
					const Activity *activity = g_ActivityMan.GetActivity();
					// If we edited something then return to main menu instead of scenario menu player will probably switch to area/scene editor.
					if (activity && activity->GetPresetName() == "None") {
						g_IntroState = MENUAPPEAR;
					} else {
						g_IntroState = MAINTOSCENARIO;
					}
				}
				PlayIntroTitle();
			}
			// Resetting the simulation
			if (g_ResetActivity) {
				// Reset and quit if user quit during reset loading
				if (!ResetActivity()) { break; }
			}
			// Resuming the simulation
			if (g_ResumeActivity) { ResumeActivity(); }
		}

		if (g_NetworkServer.IsServerModeEnabled()) {
			// Pause sim while we're waiting for scene transmission or scene will start changing before clients receive them and those changes will be lost.
			if (!g_NetworkServer.ReadyForSimulation()) {
				g_TimerMan.PauseSim(true);
			} else {
				if (g_InActivity) { g_TimerMan.PauseSim(false); }
			}
			if (!serverUpdated) {
				g_NetworkServer.Update();
				serverUpdated = true;
			}
			if (g_SettingsMan.GetServerSimSleepWhenIdle()) {
				long long ticksToSleep = g_TimerMan.GetTimeToSleep();
				if (ticksToSleep > 0) {
					double secsToSleep = static_cast<double>(ticksToSleep) / static_cast<double>(g_TimerMan.GetTicksPerSecond());
					long long milisToSleep = static_cast<long long>(secsToSleep) * 1000;
					std::this_thread::sleep_for(std::chrono::milliseconds(milisToSleep));
				}
			}
		}
		g_FrameMan.Draw();
		g_FrameMan.FlipFrameBuffers();
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Command-line argument handling.
/// </summary>
/// <param name="argc">Argument name.</param>
/// <param name="argv">Argument value.</param>
/// <param name="appExitVar">The appExitVar is what the program should exit with if this returns false.</param>
/// <returns>False if app should quit right after this.</returns>
bool HandleMainArgs(int argc, char *argv[], int &appExitVar) {

    // If no additional arguments passed, just continue (first argument is the program path)
    if (argc == 1) {
		return true;
	}
    // Default program return var if fail
    appExitVar = 2;

    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            // Print loading screen console to cout
			if (std::strcmp(argv[i], "-cout") == 0) {
				g_System.SetLogToCLI(true);
			} else if (std::strcmp(argv[i], "-bench") == 0) {
				// Measure load time
				g_MeasureModuleLoadTime = true;
			} else if (i + 1 < argc) {
				// Launch game in server mode
                if (std::strcmp(argv[i], "-server") == 0 && i + 1 < argc) {
                    std::string port = argv[++i];
                    g_NetworkServer.EnableServerMode();
                    g_NetworkServer.SetServerPort(port);
				
				// Load a single module right after the official modules
                } else if (std::strcmp(argv[i], "-module") == 0 && i + 1 < argc) {
					g_PresetMan.SetSingleModuleToLoad(argv[++i]);

				// Launch game directly into editor activity
				} else if (std::strcmp(argv[i], "-editor") == 0 && i + 1 < argc) {
					const char *editorName = argv[++i];
					if (std::strcmp(editorName, "") == 1) {
						g_EditorToLaunch = editorName;
						g_LaunchIntoEditor = true;
					}
				}
            }
        }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Implementation of the main function.
/// </summary>
int main(int argc, char *argv[]) {

	///////////////////////////////////////////////////////////////////
    // Initialize Allegro

    set_config_file("Base.rte/AllegroConfig.txt");
    allegro_init();
	loadpng_init();

    // Enable the exit button on the window
    LOCK_FUNCTION(QuitHandler);
    set_close_button_callback(QuitHandler);

    // Seed the random number generator
    SeedRand();

    ///////////////////////////////////////////////////////////////////
    // Instantiate all the managers

    new ConsoleMan();
    new LuaMan();
    new SettingsMan();
    new TimerMan();
	new PerformanceMan();
    new PresetMan();
    new FrameMan();
	new PostProcessMan();
	new PrimitiveMan();
    new AudioMan();
    new GUISound();
    new UInputMan();
    new ActivityMan();
    new MovableMan();
    new SceneMan();
    new MetaMan();

	new NetworkServer();
	new NetworkClient();

    ///////////////////////////////////////////////////////////////////
    // Create the essential managers

    g_LuaMan.Create();
	
	Reader settingsReader("Base.rte/Settings.ini", false, 0, true);
    g_SettingsMan.Create(settingsReader);

	g_NetworkServer.Create();
	g_NetworkClient.Create();

    int exitVar = 0;
    if (!HandleMainArgs(argc, argv, exitVar)) {
		return exitVar;
	}
    g_TimerMan.Create();
	g_PerformanceMan.Create();
    g_PresetMan.Create();
    g_FrameMan.Create();
    g_PostProcessMan.Create();
    if (g_AudioMan.Create() >= 0) {
        g_GUISound.Create();
    }
    g_UInputMan.Create();
    g_ConsoleMan.Create();
    g_ActivityMan.Create();
    g_MovableMan.Create();
    g_MetaMan.Create();

    ///////////////////////////////////////////////////////////////////
    // Main game driver

	if (g_NetworkServer.IsServerModeEnabled()) {
		g_NetworkServer.Start();
		g_UInputMan.SetMultiplayerMode(true);
		g_FrameMan.SetMultiplayerMode(true);
		g_AudioMan.SetMultiplayerMode(true);
		g_AudioMan.SetSoundsVolume(0);
		g_AudioMan.SetMusicVolume(0);
	}

    new LoadingGUI();
	g_LoadingGUI.InitLoadingScreen(g_MeasureModuleLoadTime);
	InitMainMenu();

	std::string screenshotSaveDir = g_System.GetWorkingDirectory() + "/" + c_ScreenshotDirectory;
	if (!std::experimental::filesystem::exists(screenshotSaveDir)) { g_System.MakeDirectory(screenshotSaveDir); }

	if (g_ConsoleMan.LoadWarningsExist()) {
		g_ConsoleMan.PrintString("WARNING: References to files that could not be located or failed to load detected during module loading!\nSee \"LoadWarningLog.txt\" for a list of bad references.");
		g_ConsoleMan.SaveLoadWarningLog("LogLoadingWarning.txt");
		// Open the console so the user is aware there are loading warnings.
		g_ConsoleMan.SetEnabled(true);
	} else {
		// Delete an existing log if there are no warnings so there's less junk in the root folder.
		if (std::experimental::filesystem::exists(g_System.GetWorkingDirectory() + "/LogLoadingWarning.txt")) { std::remove("LogLoadingWarning.txt"); }
	}

    if (!g_NetworkServer.IsServerModeEnabled()) {
		if (g_LaunchIntoEditor) {
			// Force mouse + keyboard with default mapping so we won't need to change manually if player 1 is set to keyboard only or gamepad.
			g_UInputMan.GetControlScheme(Players::PlayerOne)->SetDevice(InputDevice::DEVICE_MOUSE_KEYB);
			g_UInputMan.GetControlScheme(Players::PlayerOne)->SetPreset(InputPreset::PRESET_WASDKEYS);
			// Start the specified editor activity.
			EnterEditorActivity(g_EditorToLaunch);
		} else if (!g_SettingsMan.LaunchIntoActivity()) {
			g_IntroState = g_SettingsMan.SkipIntro() ? MENUAPPEAR : START;
			PlayIntroTitle();
		}
	} else {
		// NETWORK Create multiplayer lobby activity to start as default if server is running
		EnterMultiplayerLobby();
	}

    // If we fail to start/reset the activity, then revert to the intro/menu
    if (!ResetActivity()) { PlayIntroTitle(); }
	
    RunGameLoop();

    ///////////////////////////////////////////////////////////////////
    // Clean up

	g_NetworkClient.Destroy();
	g_NetworkServer.Destroy();

    g_MetaMan.Destroy();
    g_MovableMan.Destroy();
    g_SceneMan.Destroy();
    g_ActivityMan.Destroy();
	g_GUISound.Destroy();
    g_AudioMan.Destroy();
    g_PresetMan.Destroy();
    g_UInputMan.Destroy();
	g_PerformanceMan.Destroy();
    g_FrameMan.Destroy();
    g_TimerMan.Destroy();
    g_SettingsMan.Destroy();
    g_LuaMan.Destroy();
    ContentFile::FreeAllLoaded();
    g_ConsoleMan.Destroy();

#ifdef DEBUG_BUILD
    // Dump out the info about how well memory cleanup went
    Entity::ClassInfo::DumpPoolMemoryInfo(Writer("MemCleanupInfo.txt"));
#endif
	
    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) { return main(__argc, __argv); }