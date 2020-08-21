#ifndef _RTEMOSROTATING_
#define _RTEMOSROTATING_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            MOSRotating.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Header file for the MOSRotating class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "MOSprite.h"
#include "Gib.h"
#include "PostProcessMan.h"
#include "SoundContainer.h"

namespace RTE
{

class AtomGroup;
struct HitData;
class AEmitter;
class Attachable;

//////////////////////////////////////////////////////////////////////////////////////////
// Class:           MOSRotating
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     A sprite movable object that can rotate.
// Parent(s):       MOSprite.
// Class history:   05/30/2002 MOSRotating created.

class MOSRotating:
    public MOSprite
{


//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:


friend class AtomGroup;
friend class SLTerrain;
friend class LuaMan;


// Concrete allocation and cloning definitions
EntityAllocation(MOSRotating)
SerializableOverrideMethods
ClassInfoGetters


//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     MOSRotating
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a MOSRotating object in system
//                  memory. Create() should be called before using the object.
// Arguments:       None.

    MOSRotating() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      ~MOSRotating
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a MOSRotating object before deletion
//                  from system memory.
// Arguments:       None.

	~MOSRotating() override { Destroy(true); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the MOSRotating object ready for use.
// Arguments:       None.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

   int Create() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the MOSRotating object ready for use.
// Arguments:       A pointer to ContentFile that represents the bitmap file that will be
//                  used to create the Sprite.
//                  The number of frames in the Sprite's animation.
//                  A float specifying the object's mass in Kilograms (kg).
//                  A Vector specifying the initial position.
//                  A Vector specifying the initial velocity.
//                  The amount of time in ms this MovableObject will exist. 0 means unlim.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

	int Create(ContentFile spriteFile, const int frameCount = 1, const float mass = 1, const Vector &position = Vector(0, 0), const Vector &velocity = Vector(0, 0), const unsigned long lifetime = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a MOSRotating to be identical to another, by deep copy.
// Arguments:       A reference to the MOSRotating to deep copy.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

	int Create(const MOSRotating &reference);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Reset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resets the entire MOSRotating, including its inherited members, to their
//                  default settings or values.
// Arguments:       None.
// Return value:    None.

    void Reset() override { Clear(); MOSprite::Reset(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the SceneLayer object.
// Arguments:       Whether to only destroy the members defined in this derived class, or
//                  to destroy all inherited members also.
// Return value:    None.

    void Destroy(bool notInherited = false) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMass
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the mass value of this ACDropShip, including the mass of its
//                  currently attached body parts and inventory.
// Arguments:       None.
// Return value:    A float describing the mass value in Kilograms (kg).

    float GetMass() const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetAtomGroup
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the current AtomGroup of this MOSRotating.
// Arguments:       None.
// Return value:    A const reference to the current AtomGroup.

    AtomGroup * GetAtomGroup() { return m_pAtomGroup; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMaterial
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the main Material  of this MOSRotating.
// Arguments:       None.
// Return value:    The the Material of this MOSRotating.

	Material const * GetMaterial() const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetDrawPriority
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the drawing priority of this MovableObject, if two things were
//                  overlap when copying to the terrain, the higher priority MO would
//                  end up getting drawn.
// Arguments:       None.
// Return value:    The the priority  of this MovableObject. Higher number, the higher
//                  priority.

	int GetDrawPriority() const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRecoilForce
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the current recoil impulse force Vector of this MOSprite.
// Arguments:       None.
// Return value:    A const reference to the current recoil impulse force in kg * m/s.

    const Vector & GetRecoilForce() const { return m_RecoilForce; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetRecoilOffset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the current recoil offset Vector of this MOSprite.
// Arguments:       None.
// Return value:    A const reference to the current recoil offset.

    const Vector & GetRecoilOffset() const { return m_RecoilOffset; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetGibList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets direct access to the list of object this is to generate upon gibbing.
// Arguments:       None.
// Return value:    A pointer to the list of gibs. Ownership is NOT transferred!

    std::list<Gib> * GetGibList() { return &m_Gibs; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddRecoil
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds graphical recoil offset to this MOSprite according to its angle.
// Arguments:       None.
// Return value:    None.

    void AddRecoil();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetRecoil
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adds recoil offset to this MOSprite.
// Arguments:       A vector with the recoil impulse force in kg * m/s.
//                  A vector with the recoil offset in pixels.
//                  Whether recoil should be activated or not for the next Draw().
// Return value:    None.

    void SetRecoil(const Vector &force, const Vector &offset, bool recoil = true)
    {
        m_RecoilForce = force;
        m_RecoilOffset = offset;
        m_Recoiled = recoil;
    }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsRecoiled
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns whether this MOSprite is currently under the effects of
//                  recoil.
// Arguments:       None.
// Return value:    None.

    bool IsRecoiled() { return m_Recoiled; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          EnableDeepCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets whether or not this MOSRotating should check for deep penetrations
//                  the terrain or not.
// Arguments:       Whether to enable deep penetration checking or not.
// Return value:    None.

    void EnableDeepCheck(const bool enable = true) { m_DeepCheck = enable; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ForceDeepCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets to force a deep checking of this' silhouette against the terrain
//                  and create an outline hole in the terrain, generating particles of the
//                  intersecting pixels in the terrain.
// Arguments:       Whether to force a deep penetration check for this sim frame or not..
// Return value:    None.

    void ForceDeepCheck(const bool enable = true) { m_ForceDeepCheck = enable; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  CollideAtPoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Calculates the collision response when another MO's Atom collides with
//                  this MO's physical representation. The effects will be applied
//                  directly to this MO, and also represented in the passed in HitData.
// Arguments:       Reference to the HitData struct which describes the collision. This
//                  will be modified to represent the results of the collision.
// Return value:    Whether the collision has been deemed valid. If false, then disregard
//                  any impulses in the Hitdata.

    bool CollideAtPoint(HitData &hitData) override;


    /// <summary>
    /// Defines what should happen when this MovableObject hits and then bounces off of something.
    /// This is called by the owned Atom/AtomGroup of this MovableObject during travel.
    /// </summary>
    /// <param name="hd">The HitData describing the collision in detail.</param>
    /// <return>Whether the MovableObject should immediately halt any travel going on after this bounce.</return>
	bool OnBounce(HitData &hd) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  OnSink
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Defines what should happen when this MovableObject hits and then
//                  sink into something. This is called by the owned Atom/AtomGroup
//                  of this MovableObject during travel.
// Arguments:       The HitData describing the collision in detail.
// Return value:    Wheter the MovableObject should immediately halt any travel going on
//                  after this sinkage.

	bool OnSink(HitData &hd) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ParticlePenetration
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Determines whether a particle which has hit this MO will penetrate,
//                  and if so, whether it gets lodged or exits on the other side of this
//                  MO. Appropriate effects will be determined and applied ONLY IF there
//                  was penetration! If not, nothing will be affected.
// Arguments:       The HitData describing the collision in detail, the impulses have to
//                  have been filled out!
// Return value:    Whether the particle managed to penetrate into this MO or not. If
//                  somehting but a MOPixel or MOSParticle is being passed in as hitor,
//                  false will trivially be returned here.

    virtual bool ParticlePenetration(HitData &hd);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GibThis
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gibs this, effectively destroying it and creating multiple gibs or
//                  pieces in its place.
// Arguments:       The impulse (kg * m/s) of the impact causing the gibbing to happen.
//					The internal blast impulse which will push the gibs away from the center.
//                  A pointer to an MO which the gibs shuold not be colliding with!
// Return value:    None.

    virtual void GibThis(Vector impactImpulse = Vector(), float internalBlast = 10, MovableObject *pIgnoreMO = 0);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  MoveOutOfTerrain
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks whether any of the Atom:s in this MovableObject are on top of
//                  terrain pixels, and if so, attempt to move this out so none of this'
//                  Atoms are on top of the terrain any more.
// Arguments:       Only consider materials stronger than this in the terrain for
//                  intersections.
// Return value:    Whether any intersection was successfully resolved. Will return true
//                  even if there wasn't any intersections to begin with.

	bool MoveOutOfTerrain(unsigned char strongerThan = g_MaterialAir) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyForces
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gathers and applies the global and accumulated forces. Then it clears
//                  out the force list.Note that this does NOT apply the accumulated
//                  impulses (impulse forces)!
// Arguments:       None.
// Return value:    None.

	void ApplyForces() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ApplyImpulses
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gathers and applies the accumulated impulse forces. Then it clears
//                  out the impulse list.Note that this does NOT apply the accumulated
//                  regular forces (non-impulse forces)!
// Arguments:       None.
// Return value:    None.

	void ApplyImpulses() override;


	void AddAttachable(Attachable *pAttachable);

	void AddAttachable(Attachable *pAttachable, const Vector& parentOffsetToSet);

	void AddAttachable(Attachable *pAttachable, bool isHardcodedAttachable);

	void AddAttachable(Attachable *pAttachable, const Vector& parentOffsetToSet, bool isHardcodedAttachable);

	bool RemoveAttachable(long attachableUniqueId);

	bool RemoveAttachable(Attachable *pAttachable);

	void DetachOrDestroyAll(bool destroy);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ResetAllTimers
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resest all the timers used by this. Can be emitters, etc. This is to
//                  prevent backed up emissions to come out all at once while this has been
//                  held dormant in an inventory.
// Arguments:       None.
// Return value:    None.

    void ResetAllTimers() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  RestDetection
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does the calculations necessary to detect whether this MO appears to
//                  have has settled in the world and is at rest or not. IsAtRest()
//                  retreves the answer.
// Arguments:       None.
// Return value:    None.

    void RestDetection() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  IsOnScenePoint
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this' current graphical representation overlaps
//                  a point in absolute scene coordinates.
// Arguments:       The point in absolute scene coordinates.
// Return value:    Whether this' graphical rep overlaps the scene point.

	bool IsOnScenePoint(Vector &scenePoint) const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  EraseFromTerrain
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Cuts this' silhouette out from the terrain's material and color layers.
// Arguments:       None.
// Return value:    None.

	void EraseFromTerrain();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  DeepCheck
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if any of this' deep group atmos are on top of the terrain, and
//                  if so, erases this' silhouette from the terrain.
// Arguments:       Whether to make any MOPixels from erased terrain pixels at all.
//                  The size of the gaps between MOPixels knocked loose by the terrain erasure.
//                  The max number of MOPixel:s to generate as dislodged particles from the
//                  erased terrain.
// Return value:    Whether deep penetration was detected and erasure was done.

	bool DeepCheck(bool makeMOPs = true, int skipMOP = 2, int maxMOP = 100);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  PreTravel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does stuff that needs to be done before Travel(). Always call before
//                  calling Travel.
// Arguments:       None.
// Return value:    None.

	void PreTravel() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Travel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Travels this MOSRotatin, using its physical representation.
// Arguments:       None.
// Return value:    None.

	void Travel() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  PostTravel
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does stuff that needs to be done after Update(). Always call after
//                  calling Update.
// Arguments:       None.
// Return value:    None.

	void PostTravel() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates this MovableObject. Supposed to be done every frame.
// Arguments:       None.
// Return value:    None.

	void Update() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  DrawMOIDIfOverlapping
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the MOID representation of this to the SceneMan's MOID layer if
//                  this is found to potentially overlap another MovableObject.
// Arguments:       The MovableObject to check this for overlap against.
// Return value:    Whether it was drawn or not.

	bool DrawMOIDIfOverlapping(MovableObject *pOverlapMO) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this MOSRotating's current graphical representation to a
//                  BITMAP of choice.
// Arguments:       A pointer to a BITMAP to draw on.
//                  The absolute position of the target bitmap's upper left corner in the Scene.
//                  In which mode to draw in. See the DrawMode enumeration for the modes.
//                  Whether to not draw any extra 'ghost' items of this MovableObject,
//                  indicator arrows or hovering HUD text and so on.
// Return value:    None.

    void Draw(BITMAP *pTargetBitmap, const Vector &targetPos = Vector(), DrawMode mode = g_DrawColor, bool onlyPhysical = false) const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  GetGibWoundLimit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return wound limit for this object.
// Arguments:       None.
// Return value:    Wound limit of the object.

	int GetGibWoundLimit() const { return m_GibWoundLimit; } 


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  SetGibImpulseLimit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Set new impulse limit.
// Arguments:       New impulse limit.
// Return value:    None.

	void SetGibImpulseLimit(int newLimit) { m_GibImpulseLimit = newLimit; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  GetGibImpulseLimit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Return impulse limit for this object.
// Arguments:       None.
// Return value:    Impulse limit of the object.

	int GetGibImpulseLimit() const { return m_GibImpulseLimit; } 


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  SetGibWoundLimit
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Set new wound limit, current wounds are not affected.
// Arguments:       New wound limit.
// Return value:    None.

	void SetGibWoundLimit(int newLimit) { m_GibWoundLimit = newLimit; }


	/// <summary>
	/// Attaches the passed in wound AEmitter and adds it to the list of wounds, changing its parent offset to the passed in Vector.
	/// </summary>
	/// <param name="pWound">The wound AEmitter to add</param>
	/// <param name="parentOffsetToSet">The vector to set as the wound AEmitter's parent offset</param>
	void AddWound(AEmitter *pWound, const Vector& parentOffsetToSet, bool checkGibWoundLimit = true);


	/// <summary>
	/// Removes a specified amount of wounds and returns damage caused by these wounds. Head multiplier is not used.				
	/// </summary>
	/// <param name="amount">Amount of wounds to remove.</param>
	/// <returns>Amount of damage caused by these wounds.</returns>
	virtual int RemoveWounds(int amount);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  GetWoundCount
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the amount of wound attached to this.
// Arguments:       Key to retrieve value.
// Return value:    Wound amount.

	int GetWoundCount() const { return m_Wounds.size(); }; 


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  GetStringValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the string value associated with the specified key or "" if it does not exist.
// Arguments:       Key to retrieve value.
// Return value:    String value.

	std::string GetStringValue(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:  GetNumberValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the number value associated with the specified key or 0 if it does not exist.
// Arguments:       Key to retrieve value.
// Return value:    Number (double) value.

	double GetNumberValue(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:  GetObjectValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the object value associated with the specified key or 0 if it does not exist.
// Arguments:       None.
// Return value:    Object (Entity *) value.

	Entity * GetObjectValue(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:  SetStringValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the string value associated with the specified key.
// Arguments:       String key and value to set.
// Return value:    None.

	void SetStringValue(std::string key, std::string value);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:  SetNumberValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the string value associated with the specified key.
// Arguments:       String key and value to set.
// Return value:    None.

	void SetNumberValue(std::string key, double value);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetObjectValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the string value associated with the specified key.
// Arguments:       String key and value to set.
// Return value:    None.

	void SetObjectValue(std::string key, Entity * value);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  RemoveStringValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Remove the string value associated with the specified key.
// Arguments:       String key to remove.
// Return value:    None.

	void RemoveStringValue(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  RemoveNumberValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Remove the number value associated with the specified key.
// Arguments:       String key to remove.
// Return value:    None.

	void RemoveNumberValue(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  RemoveObjectValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Remove the object value associated with the specified key.
// Arguments:       String key to remove.
// Return value:    None.

	void RemoveObjectValue(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  StringValueExists
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks whether the value associated with the specified key exists.
// Arguments:       String key to check.
// Return value:    True if value exists.

	bool StringValueExists(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  NumberValueExists
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks whether the value associated with the specified key exists.
// Arguments:       String key to check.
// Return value:    True if value exists.

	bool NumberValueExists(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ObjectValueExists
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks whether the value associated with the specified key exists.
// Arguments:       String key to check.
// Return value:    True if value exists.

	bool ObjectValueExists(std::string key);

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMOIDs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Puts all MOIDs associated with this MO and all it's descendants into MOIDs vector
// Arguments:       Vector to store MOIDs
// Return value:    None.

	void GetMOIDs(std::vector<MOID> &MOIDs) const override;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetDamageMultiplier
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets damage multiplier of this attachable.
// Arguments:       New multiplier value.
// Return value:    None.

	void SetDamageMultiplier(float newValue) { m_DamageMultiplier = newValue; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetDamageMultiplier
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns damage multiplier of this attachable.
// Arguments:       None.
// Return value:    Current multiplier value.

	float GetDamageMultiplier() const { return m_DamageMultiplier; }

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsDamageMultiplierRedefined
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether the damage multiplier was altered in the .INI definition. 
//					If not, CC will apply default values during actor construction.
// Arguments:       None.
// Return value:    Current multiplier value.

	bool IsDamageMultiplierRedefined() const { return m_DamageMultiplierRedefined; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetTravelImpulse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Retrurns the amount of impulse force exerted on this during the last frame.
// Arguments:       None.
// Return value:    The amount of impulse force exerted on this during the last frame.

	Vector GetTravelImpulse() const { return m_TravelImpulse; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTravelImpulse
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the amount of impulse force exerted on this during the last frame.
// Arguments:       New impulse value
// Return value:    None.

	void SetTravelImpulse(Vector impulse) { m_TravelImpulse = impulse; }


//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ApplyAttachableForces
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Does the joint force transfer stuff for an attachable. Convencinece
//                  method. If this returns false, it means the attachable has been knocked
//                  off and has been passed to MovableMan OR deleted. In either case,
//                  if false is returned just set the pointer to 0 and be done with it.
// Arguments:       A pointer to the attachable to mess with. Ownership isn't transferred,
//                  but if the return is false, then the object has been deleted!
//					If isCritical is true, then if attachable is gibbed created break wound
//					emits indefenitely to guarantee actor's death.
// Return value:    Whether or not the joint held up to the forces and impulses which had
//                  accumulated on the Attachable during this Update(). If false, the passed
//                  in instance is now deleted and invalid!

    bool ApplyAttachableForces(Attachable *pAttachable, bool isCritical = false);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  UpdateChildMOIDs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes this MO register itself and all its attached children in the
//                  MOID register and get ID:s for itself and its children for this frame.
// Arguments:       The MOID index to register itself and its children in.
//                  The MOID of the root MO of this MO, ie the highest parent of this MO.
//                  0 means that this MO is the root, ie it is owned by MovableMan.
//                  Whether this MO should make a new MOID to use for itself, or to use
//                  the same as the last one in the index (presumably its parent),
// Return value:    None.

    void UpdateChildMOIDs(std::vector<MovableObject *> &MOIDIndex, MOID rootMOID = g_NoMOID, bool makeNewMOID = true) override;

    // Member variables
    static Entity::ClassInfo m_sClass;
//    float m_Torque; // In kg * r/s^2 (Newtons).
//    float m_ImpulseTorque; // In kg * r/s.
    // The group of Atom:s that will be the physical reperesentation of this MOSRotating.
    AtomGroup *m_pAtomGroup;
    // The group of Atom:s that will serve as a means to detect deep terrain penetration.
    AtomGroup *m_pDeepGroup;
    // Whether or not to check for deep penetrations.
    bool m_DeepCheck;
    // A trigger for forcing a deep check to happen
    bool m_ForceDeepCheck;
    // Whether deep penetration happaned in the last frame or not, and how hard it was.
    float m_DeepHardness;
    // The amount of impulse force exerted on this during the last frame.
    Vector m_TravelImpulse;
    // The precomupted center location of the sprite relative to the MovableObject::m_Pos.
    Vector m_SpriteCenter;
    // How much to orient the rotation of this to match the velocity vector each frame 0 = none, 1.0 = immediately align with vel vector
    float m_OrientToVel;
    // Whether the SpriteMO is currently pushed back by recoil or not.
    bool m_Recoiled;
    // The impulse force in kg * m/s that represents the recoil.
    Vector m_RecoilForce;
    // The vector that the recoil offsets the sprite when m_Recoiled is true.
    Vector m_RecoilOffset;
    // The list of wound AEmitters currently attached to this MOSRotating, and owned here as well
    std::list<AEmitter *> m_Wounds;
    // The list of general Attachables currently attached and Owned by this.
    std::list<Attachable *> m_Attachables;
    // The list of all Attachables, including both hardcoded attachables and those added through ini or lua
    std::list<Attachable *> m_AllAttachables;
    // The list of Gib:s this will create when gibbed
    std::list<Gib> m_Gibs;
    // The amount of impulse force required to gib this, in kg * (m/s). 0 means no limit
    float m_GibImpulseLimit;
    // The number of wound emitters allowed before this gets gibbed. 0 means this can't get gibbed
    int m_GibWoundLimit;
    // Gib sound effect
    SoundContainer m_GibSound;
    // Whether to flash effect on gib
    bool m_EffectOnGib;
    // How far this is audiable (in screens) when gibbing
    float m_LoudnessOnGib;
	// Map to store any generic strings
	std::map<std::string, std::string> m_StringValueMap;
	// Map to store any generic numbers
	std::map<std::string, double> m_NumberValueMap;
	// Map to store any object pointers
	std::map<std::string, Entity *> m_ObjectValueMap;

	// Damage mutliplier for this attachable
	float m_DamageMultiplier;
	// Whether damage multiplier for this attachable was redefined in .ini
	bool m_DamageMultiplierRedefined;

    // Intermediary drawing bitmap used to flip rotating bitmaps. Owned!
    BITMAP *m_pFlipBitmap;
    BITMAP *m_pFlipBitmapS;
    // Intermediary drawing bitmap used to draw sihouettes and other effects. Not owned; points to the shared static bitmaps
    BITMAP *m_pTempBitmap;
    // Temp drawing bitmaps shared between all MOSRotatings
    static BITMAP *m_spTempBitmap16;
    static BITMAP *m_spTempBitmap32;
    static BITMAP *m_spTempBitmap64;
    static BITMAP *m_spTempBitmap128;
    static BITMAP *m_spTempBitmap256;
    static BITMAP *m_spTempBitmap512;

    // Intermediary drawing bitmap used to draw MO silhouettes. Not owned; points to the shared static bitmaps
    BITMAP *m_pTempBitmapS;
    // Temp drawing bitmaps shared between all MOSRotatings
    static BITMAP *m_spTempBitmapS16;
    static BITMAP *m_spTempBitmapS32;
    static BITMAP *m_spTempBitmapS64;
    static BITMAP *m_spTempBitmapS128;
    static BITMAP *m_spTempBitmapS256;
    static BITMAP *m_spTempBitmapS512;

//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

private:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this MOSRotating, effectively
//                  resetting the members of this abstraction level only.
// Arguments:       None.
// Return value:    None.

    void Clear();

    /// <summary>
    /// Handles reading for custom values, dealing with the various types of custom values.
    /// </summary>
    /// <param name="reader">A Reader lined up to the custom value type to be read.</param>
    void ReadCustomValueProperty(Reader &reader);


    // Disallow the use of some implicit methods.
	MOSRotating(const MOSRotating &reference) {}
	MOSRotating& operator=(const MOSRotating &rhs) {}

	static std::unordered_map<std::string, std::function<void(MOSRotating *, Reader &)>> RegisterPropertyMatchers();
	static std::unordered_map<std::string, std::function<void(MOSRotating *, Reader &)>> m_PropertyMatchers;
};

} // namespace RTE

#endif // File