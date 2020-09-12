#include "PostProcessMan.h"
#include "FrameMan.h"
#include "Scene.h"
#include "ContentFile.h"
#include "Matrix.h"

namespace RTE {

	const std::string PostProcessMan::c_ClassName = "PostProcessMan";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::Clear() {
		m_PostScreenEffects.clear();
		m_PostSceneEffects.clear();
		m_YellowGlow = 0;
		m_YellowGlowHash = 0;
		m_RedGlow = 0;
		m_RedGlowHash = 0;
		m_BlueGlow = 0;
		m_BlueGlowHash = 0;
		for (short i = 0; i < c_MaxScreenCount; ++i) {
			m_ScreenRelativeEffects->clear();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int PostProcessMan::Create() {
		// TODO: Make more robust and load more glows!
		ContentFile glowFile("Base.rte/Effects/Glows/YellowTiny.bmp");
		m_YellowGlow = glowFile.GetAsBitmap();
		m_YellowGlowHash = glowFile.GetHash();
		glowFile.SetDataPath("Base.rte/Effects/Glows/RedTiny.bmp");
		m_RedGlow = glowFile.GetAsBitmap();
		m_RedGlowHash = glowFile.GetHash();
		glowFile.SetDataPath("Base.rte/Effects/Glows/BlueTiny.bmp");
		m_BlueGlow = glowFile.GetAsBitmap();
		m_BlueGlowHash = glowFile.GetHash();

		// Create temporary bitmaps to rotate post effects in.
		m_TempEffectBitmaps = {
			{16, create_bitmap(16, 16)},
			{32, create_bitmap(32, 32)},
			{64, create_bitmap(64, 64)},
			{128, create_bitmap(128, 128)},
			{256, create_bitmap(256, 256)},
			{512, create_bitmap(512, 512)}
		};

		return 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::Destroy() {
		ClearScreenPostEffects();
		ClearScenePostEffects();
		Clear();
		for (std::pair<unsigned short, BITMAP *> tempBitmapEntry : m_TempEffectBitmaps) {
			destroy_bitmap(tempBitmapEntry.second);
		}
		m_TempEffectBitmaps.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::AdjustEffectsPosToPlayerScreen(short playerScreen, BITMAP *targetBitmap, Vector targetBitmapOffset, std::list<PostEffect> &screenRelativeEffectsList, std::list<Box> &screenRelativeGlowBoxesList) {
		int screenOcclusionOffsetX = g_SceneMan.GetScreenOcclusion(playerScreen).GetFloorIntX();
		int screenOcclusionOffsetY = g_SceneMan.GetScreenOcclusion(playerScreen).GetFloorIntY();
		int occludedOffsetX = targetBitmap->w + screenOcclusionOffsetX;
		int occludedOffsetY = targetBitmap->h + screenOcclusionOffsetY;

		// Copy post effects received by client if in network mode
		if (g_FrameMan.GetDrawNetworkBackBuffer()) { g_PostProcessMan.GetNetworkPostEffectsList(0, screenRelativeEffectsList); }

		// Adjust for the player screen's position on the final buffer
		for (const PostEffect &postEffect : screenRelativeEffectsList) {
			// Make sure we won't be adding any effects to a part of the screen that is occluded by menus and such
			if (postEffect.m_Pos.m_X > screenOcclusionOffsetX && postEffect.m_Pos.m_Y > screenOcclusionOffsetY && postEffect.m_Pos.m_X < occludedOffsetX && postEffect.m_Pos.m_Y < occludedOffsetY) {
				g_PostProcessMan.GetPostScreenEffectsList()->push_back(PostEffect(postEffect.m_Pos + targetBitmapOffset, postEffect.m_Bitmap, postEffect.m_BitmapHash, postEffect.m_Strength, postEffect.m_Angle));
			}
		}
		// Adjust glow areas for the player screen's position on the final buffer
		for (const Box &glowBox : screenRelativeGlowBoxesList) {
			g_PostProcessMan.GetPostScreenGlowBoxesList()->push_back(glowBox);
			// Adjust each added glow area for the player screen's position on the final buffer
			g_PostProcessMan.GetPostScreenGlowBoxesList()->back().m_Corner += targetBitmapOffset;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::RegisterPostEffect(const Vector &effectPos, BITMAP *effect, size_t hash, unsigned char strength, float angle) {
		// These effects get applied when there's a drawn frame that followed one or more sim updates.
		// They are not only registered on drawn sim updates; flashes and stuff could be missed otherwise if they occur on undrawn sim updates.
		if (effect && g_TimerMan.SimUpdatesSinceDrawn() >= 0) { m_PostSceneEffects.push_back(PostEffect(effectPos, effect, hash, strength, angle)); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool PostProcessMan::GetPostScreenEffectsWrapped(const Vector &boxPos, int boxWidth, int boxHeight, std::list<PostEffect> &effectsList, short team) {
		bool found = false;

		// Do the first unwrapped rect
		found = GetPostScreenEffects(boxPos, boxWidth, boxHeight, effectsList, team);

		int left = boxPos.m_X;
		int top = boxPos.m_Y;
		int right = left + boxWidth;
		int bottom = top + boxHeight;

		if (g_SceneMan.SceneWrapsX()) {
			int sceneWidth = g_SceneMan.GetScene()->GetWidth();
			if (left < 0) { found = GetPostScreenEffects(left + sceneWidth, top, right + sceneWidth, bottom, effectsList, team) || found; }
			if (right >= sceneWidth) { found = GetPostScreenEffects(left - sceneWidth, top, right - sceneWidth, bottom, effectsList, team) || found; }
		}
		if (g_SceneMan.SceneWrapsY()) {
			int sceneHeight = g_SceneMan.GetScene()->GetHeight();
			if (top < 0) { found = GetPostScreenEffects(left, top + sceneHeight, right, bottom + sceneHeight, effectsList, team) || found; }
			if (bottom >= sceneHeight) { found = GetPostScreenEffects(left, top - sceneHeight, right, bottom - sceneHeight, effectsList, team) || found; }
		}
		return found;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP *PostProcessMan::GetTempEffectBitmap(BITMAP *bitmap) const {
		// Get the largest dimension of the bitmap and convert it to a multiple of 16, i.e. 16, 32, etc
		unsigned short bitmapSizeNeeded = std::ceil(static_cast<float>(std::max(bitmap->w, bitmap->h)) / 16) * 16;
		std::unordered_map<unsigned short, BITMAP *>::const_iterator correspondingBitmapSizeEntry = m_TempEffectBitmaps.find(bitmapSizeNeeded);

		// If we didn't find a match then the bitmap size is greater than 512 but that's the biggest we've got, so return it
		if (correspondingBitmapSizeEntry == m_TempEffectBitmaps.end()) { correspondingBitmapSizeEntry = m_TempEffectBitmaps.find(512); }

		return correspondingBitmapSizeEntry->second;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::RegisterGlowDotEffect(const Vector &effectPos, DotGlowColor color, unsigned char strength) {
		// These effects only apply only once per drawn sim update, and only on the first frame drawn after one or more sim updates
		if (color != NoDot && g_TimerMan.DrawnSimUpdate() && g_TimerMan.SimUpdatesSinceDrawn() >= 0) {
			RegisterPostEffect(effectPos, GetDotGlowEffect(color), GetDotGlowEffectHash(color), strength);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool PostProcessMan::GetGlowAreasWrapped(const Vector &boxPos, int boxWidth, int boxHeight, std::list<Box> &areaList) {
		bool foundAny = false;
		Vector intRectPosRelativeToBox;

		// Account for wrapping in any registered glow IntRects, as well as on the box we're testing against
		std::list<IntRect> wrappedGlowRects;

		for(const IntRect &glowArea : m_GlowAreas) {
			g_SceneMan.WrapRect(glowArea, wrappedGlowRects);
		}
		std::list<IntRect> wrappedTestRects;
		g_SceneMan.WrapRect(IntRect(boxPos.m_X, boxPos.m_Y, boxPos.m_X + boxWidth, boxPos.m_Y + boxHeight), wrappedTestRects);

		// Check for intersections. If any are found, cut down the intersecting IntRect to the bounds of the IntRect we're testing against, then make and store a Box out of it
		for (IntRect &wrappedTestRect : wrappedTestRects) {
			for (const IntRect &wrappedGlowRect : wrappedGlowRects) {
				if (wrappedTestRect.Intersects(wrappedGlowRect)) {
					IntRect cutRect(wrappedGlowRect);
					cutRect.IntersectionCut(wrappedTestRect);
					intRectPosRelativeToBox = Vector(cutRect.m_Left - boxPos.m_X, cutRect.m_Top - boxPos.m_Y);
					areaList.push_back(Box(intRectPosRelativeToBox, cutRect.m_Right - cutRect.m_Left, cutRect.m_Bottom - cutRect.m_Top));
					foundAny = true;
				}
			}
		}
		return foundAny;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::GetNetworkPostEffectsList(short whichScreen, std::list<PostEffect> & outputList) {
		ScreenRelativeEffectsMutex[whichScreen].lock();
		outputList.clear();
		for (const PostEffect &postEffect : m_ScreenRelativeEffects[whichScreen]) {
			outputList.push_back(PostEffect(postEffect.m_Pos, postEffect.m_Bitmap, postEffect.m_BitmapHash, postEffect.m_Strength, postEffect.m_Angle));
		}
		ScreenRelativeEffectsMutex[whichScreen].unlock();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::SetNetworkPostEffectsList(short whichScreen, std::list<PostEffect> & inputList) {
		ScreenRelativeEffectsMutex[whichScreen].lock();
		m_ScreenRelativeEffects[whichScreen].clear();
		for (const PostEffect &postEffect : inputList) {
			m_ScreenRelativeEffects[whichScreen].push_back(PostEffect(postEffect.m_Pos, postEffect.m_Bitmap, postEffect.m_BitmapHash, postEffect.m_Strength, postEffect.m_Angle));
		}
		ScreenRelativeEffectsMutex[whichScreen].unlock();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool PostProcessMan::GetPostScreenEffects(Vector boxPos, int boxWidth, int boxHeight, std::list<PostEffect> &effectsList, short team) {
		bool found = false;
		bool unseen = false;
		Vector postEffectPosRelativeToBox;

		for (PostEffect &scenePostEffect : m_PostSceneEffects) {
			if (team != Activity::NoTeam) { unseen = g_SceneMan.IsUnseen(scenePostEffect.m_Pos.m_X, scenePostEffect.m_Pos.m_Y, team); }

			if (WithinBox(scenePostEffect.m_Pos, boxPos, boxWidth, boxHeight) && !unseen) {
				found = true;
				postEffectPosRelativeToBox = scenePostEffect.m_Pos - boxPos;
				effectsList.push_back(PostEffect(postEffectPosRelativeToBox, scenePostEffect.m_Bitmap, scenePostEffect.m_BitmapHash, scenePostEffect.m_Strength, scenePostEffect.m_Angle));
			}
		}
		return found;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool PostProcessMan::GetPostScreenEffects(int left, int top, int right, int bottom, std::list<PostEffect> &effectsList, short team) {
		bool found = false;
		bool unseen = false;
		Vector postEffectPosRelativeToBox;

		for (PostEffect &scenePostEffect : m_PostSceneEffects) {
			if (team != Activity::NoTeam) { unseen = g_SceneMan.IsUnseen(scenePostEffect.m_Pos.m_X, scenePostEffect.m_Pos.m_Y, team); }
				
			if (WithinBox(scenePostEffect.m_Pos, left, top, right, bottom) && !unseen) {
				found = true;
				postEffectPosRelativeToBox = Vector(scenePostEffect.m_Pos.m_X - left, scenePostEffect.m_Pos.m_Y - top);
				effectsList.push_back(PostEffect(postEffectPosRelativeToBox, scenePostEffect.m_Bitmap, scenePostEffect.m_BitmapHash, scenePostEffect.m_Strength, scenePostEffect.m_Angle));
			}
		}
		return found;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BITMAP * PostProcessMan::GetDotGlowEffect(DotGlowColor whichColor) const {
		switch (whichColor) {
			case NoDot:
				return 0;
			case YellowDot:
				return m_YellowGlow;
			case RedDot:
				return m_RedGlow;
			case BlueDot:
				return m_BlueGlow;
			default:
				RTEAbort("Undefined glow dot color value passed in. See DotGlowColor enumeration for defined values.");
				return 0;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	size_t PostProcessMan::GetDotGlowEffectHash(DotGlowColor whichColor) const {
		switch (whichColor) {
			case NoDot:
				return 0;
			case YellowDot:
				return m_YellowGlowHash;
			case RedDot:
				return m_RedGlowHash;
			case BlueDot:
				return m_BlueGlowHash;
			default:
				RTEAbort("Undefined glow dot color value passed in. See DotGlowColor enumeration for defined values.");
				return 0;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::PostProcess() {
		// First copy the current 8bpp backbuffer to the 32bpp buffer; we'll add effects to it
		blit(g_FrameMan.GetBackBuffer8(), g_FrameMan.GetBackBuffer32(), 0, 0, 0, 0, g_FrameMan.GetBackBuffer8()->w, g_FrameMan.GetBackBuffer8()->h);

		// Set the screen blender mode for glows
		set_screen_blender(128, 128, 128, 128);

		// Reference. Do not remove.
		//acquire_bitmap(m_BackBuffer8);
		//acquire_bitmap(m_BackBuffer32);

		DrawDotGlowEffects();
		DrawPostScreenEffects();

		// Reference. Do not remove.
		//release_bitmap(m_BackBuffer32);
		//release_bitmap(m_BackBuffer8);
		// Set blender mode back??
		//set_trans_blender(128, 128, 128, 128);

		// Clear the effects list for this frame
		m_PostScreenEffects.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::DrawDotGlowEffects() {
		int x = 0;
		int y = 0;
		int startX = 0;
		int startY = 0;
		int endX = 0;
		int endY = 0;
		unsigned short testpixel = 0;

		// Randomly sample the entire backbuffer, looking for pixels to put a glow on.
		// NOTE THIS IS SLOW, especially on higher resolutions!
		for (const Box &glowBox : m_PostScreenGlowBoxes) {
			startX = glowBox.m_Corner.m_X;
			startY = glowBox.m_Corner.m_Y;
			endX = startX + glowBox.m_Width;
			endY = startY + glowBox.m_Height;

			// Sanity check a little at least
			if (startX < 0 || startX >= g_FrameMan.GetBackBuffer8()->w || startY < 0 || startY >= g_FrameMan.GetBackBuffer8()->h ||
				endX < 0 || endX >= g_FrameMan.GetBackBuffer8()->w || endY < 0 || endY >= g_FrameMan.GetBackBuffer8()->h) {
				continue;
			}

#ifdef DEBUG_BUILD
			// Draw a rectangle around the glow box so we see it's position and size
			rect(g_FrameMan.GetBackBuffer32(), startX, startY, endX, endY, g_RedColor);
#endif
			
			for (y = startY; y < endY; ++y) {
				for (x = startX; x < endX; ++x) {
					testpixel = _getpixel(g_FrameMan.GetBackBuffer8(), x, y);

					// YELLOW
					if ((testpixel == g_YellowGlowColor && RandomNum() < 0.9F) || testpixel == 98 || (testpixel == 120 && RandomNum() < 0.7F)) {
						draw_trans_sprite(g_FrameMan.GetBackBuffer32(), m_YellowGlow, x - 2, y - 2);
					}
					// TODO: Enable and add more colors once we actually have something that needs these.
					// RED
					/*
					if (testpixel == 13) {
						draw_trans_sprite(m_BackBuffer32, m_RedGlow, x - 2, y - 2);
					}
					// BLUE
					if (testpixel == 166) {
						draw_trans_sprite(g_FrameMan.GetBackBuffer32(), m_BlueGlow, x - 2, y - 2);
					}
					*/
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void PostProcessMan::DrawPostScreenEffects() {
		BITMAP *effectBitmap = 0;
		int effectPosX = 0;
		int effectPosY = 0;
		unsigned char effectStrength = 0;

		for (const PostEffect &postEffect : m_PostScreenEffects) {
			if (postEffect.m_Bitmap) {
				effectBitmap = postEffect.m_Bitmap;
				effectStrength = postEffect.m_Strength;
				effectPosX = postEffect.m_Pos.GetFloorIntX() - (effectBitmap->w / 2);
				effectPosY = postEffect.m_Pos.GetFloorIntY() - (effectBitmap->h / 2);
				set_screen_blender(effectStrength, effectStrength, effectStrength, effectStrength);

				// Draw all the scene screen effects accumulated this frame
				if (postEffect.m_Angle == 0) {
					draw_trans_sprite(g_FrameMan.GetBackBuffer32(), effectBitmap, effectPosX, effectPosY);
				} else {
					BITMAP *targetBitmap = GetTempEffectBitmap(effectBitmap);
					clear_to_color(targetBitmap, 0);

					Matrix newAngle;
					newAngle.SetRadAngle(postEffect.m_Angle);

					rotate_sprite(targetBitmap, effectBitmap, 0, 0, ftofix(newAngle.GetAllegroAngle()));
					draw_trans_sprite(g_FrameMan.GetBackBuffer32(), targetBitmap, effectPosX, effectPosY);
				}
			}
		}
	}
}