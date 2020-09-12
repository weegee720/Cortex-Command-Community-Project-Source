#ifndef _RTEARM_
#define _RTEARM_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            Arm.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Header file for the Arm class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "Attachable.h"

namespace RTE
{

class HeldDevice;


//////////////////////////////////////////////////////////////////////////////////////////
// Class:           Arm
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     A detatchable arm that can hold HeldDevices (weapons, tools)
// Parent(s):       Attachable.
// Class history:   05/30/2002 Arm created.

class Arm:
    public Attachable
{


//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:


// Concrete allocation and cloning definitions
EntityAllocation(Arm)
SerializableOverrideMethods
ClassInfoGetters

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     Arm
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a Arm object in system
//                  memory. Create() should be called before using the object.
// Arguments:       None.

    Arm() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      ~Arm
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a Arm object before deletion
//                  from system memory.
// Arguments:       None.

	~Arm() override { Destroy(true); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Creates a Arm to be identical to another, by deep copy.
// Arguments:       A reference to the Arm to deep copy.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

    int Create(const Arm &reference);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Reset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resets the entire Arm, including its inherited members, to their
//                  default settings or values.
// Arguments:       None.
// Return value:    None.

    void Reset() override { Clear(); Attachable::Reset(); }


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
// Description:     Gets the mass value of this Arm, including the mass of any device it
//                  may be holding.
// Arguments:       None.
// Return value:    A float describing the mass value in Kilograms (kg).

    float GetMass() const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetHandPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the position of the hand of this Arm as an absolute scene coord.
// Arguments:       None.
// Return value:    Vector with the current absolute scene hand position.

    Vector GetHandPos() const { return m_JointPos + m_HandOffset; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetHeldDevice
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the HeldDevice currently held by this Arm, IF the thing held is
//                  a HeldDevice, that is. Ownership is NOT transferred.
// Arguments:       None.
// Return value:    A pointer to the currently held HeldDevice. 0 is returned if no
//                  HeldDevice is currently held (even though an MO may be held).

    HeldDevice * GetHeldDevice() const;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HoldsDevice
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether a device is being held or not. Faster than using
//                  GetHeldDevice() for the purpose.
// Arguments:       None.
// Return value:    A bool indicating whether any device is held by this Arm.

    bool HoldsDevice() const { return m_pHeldMO && m_pHeldMO->IsDevice(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HoldsHeldDevice
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether a HeldDevice is held or not. Faster than using
//                  GetHeldDevice() for the purpose.
// Arguments:       None.
// Return value:    A bool indicating whether a HeldDevice is held by this Arm.

    bool HoldsHeldDevice() const { return m_pHeldMO && m_pHeldMO->IsHeldDevice(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HoldsThrownDevice
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether a ThrownDevice is held or not. Faster than using
//                  GetHeldDevice() for the purpose.
// Arguments:       None.
// Return value:    A bool indicating whether a ThrownDevice is held by this Arm.

    bool HoldsThrownDevice() const { return m_pHeldMO && m_pHeldMO->IsThrownDevice(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetHeldMO
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the MovableObject currently held by this Arm. Ownership is NOT
//                  transferred.
// Arguments:       None.
// Return value:    A pointer to the currently held MovableObject. 0 is returned if no
//                  MovableObject is currently held.

    MovableObject * GetHeldMO() const { return m_pHeldMO; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMaxLength
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the max length this arm can reach from its socket to the hand.
// Arguments:       None.
// Return value:    The max length of reach, in pixels, of this arm.

    float GetMaxLength() const { return m_MaxLength; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  SetID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the MOID of this MovableObject for this frame.
// Arguments:       A moid specifying the MOID that this MovableObject is
//                  assigned for this frame.
// Return value:    None.

    void SetID(const MOID newID) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetHeldMO
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Replaces the MovableObject currently held by this Arm with a new
//                  one. Ownership IS transferred. The currently held MovableObject
//                  (if there is one) will be dropped and become a detached MovableObject,
// Arguments:       A pointer to the new MovableObject to hold. Ownership IS transferred.
// Return value:    None.

    void SetHeldMO(MovableObject *newHeldMO);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetHandPos
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the position of the hand of this Arm to an absolute scene coord.
//                  This position will be capped to the reach of the Arm before drawing.
// Arguments:       Vector with the new absolute scene hand position.
// Return value:    None.

    void SetHandPos(const Vector &newHandPos) { m_HandOffset = newHandPos - m_JointPos; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetIdleOffset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the offset from the joint position of this Arm to which its hand will
//                  go when not holding a device and not able to reach a certain target.
// Arguments:       None.
// Return value:    Vector with the new idle offset relative to the joint position of this Arm.

	Vector GetIdleOffset() const { return m_IdleOffset; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetIdleOffset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the offset from the joint position of this Arm to which its hand will
//                  go when not holding a device and not able to reach a certain target.
// Arguments:       Vector with the new idle offset relative to the joint position of this Arm.
// Return value:    None.

    void SetIdleOffset(const Vector &newIdleOffset) { m_IdleOffset = newIdleOffset; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReleaseHeldMO
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes this arm let go of the MovableObject currently held. Ownership
//                  IS transferred!
// Arguments:       None.
// Return value:    A pointer to the up to this point held MovableObject. 0 is returned
//                  if no MovableObject is currently held. Ownership IS transferred!

    MovableObject * ReleaseHeldMO();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DropEverything
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes this arm let go of anyhthing it holds and give it to the
//                  MovableMan. Ownership is transferred to MovableMan.
// Arguments:       None.
// Return value:    A pointer to the up to this point held anything. 0 is returned if
//                  nothing is currently held. Ownership is NOT transferred, but given
//                  to MovableMan.

    MovableObject * DropEverything();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SwapHeldMO
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Replaces the MovableObject currently held by this Arm with a new
//                  one, and returns the replaced one. Ownership IS transferred both ways.
// Arguments:       A pointer to the new MovableObject to hold. Ownership IS transferred.
// Return value:    A pointer to the previously held MO. Ownership IS transferred.

    MovableObject * SwapHeldMO(MovableObject *newMO);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Reach
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Rotates the arm so that it reaches after a point in scene coordinates.
//                  Must be called AFTER SetPos for this frame if the return value is to
//                  be accurate. If the target is not reached, the idle position of the
//                  Arm will be assumed.
// Arguments:       The point to reach after. If (0, 0), reaching is deactivated.
// Return value:    None.

    void Reach(const Vector &scenePoint);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReachToward
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Rotates the arm so that it reaches after a point in scene coordinates.
//                  Must be called AFTER SetPos for this frame if the return value is to
//                  be accurate. Arm will reach towards target regardless of wheter it
//                  is within this Arm's length or not.
// Arguments:       The point to reach after. If (0, 0), reaching is deactivated.
// Return value:    None.

    void ReachToward(const Vector &scenePoint);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          IsReaching
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this Arm is reaching toward somehting this frame
// Arguments:       None.
// Return value:    Whether this Arm is holding anyhting.

    bool IsReaching() { return !m_TargetPoint.IsZero(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DidReach
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this Arm actually reached the reach target last
//                  Update().
// Arguments:       None.
// Return value:    Whether.the Arm was able to actually reach the point or not.

    bool DidReach() { return m_DidReach; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          WillIdle
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this Arm will go to the idle position if it can't
//                  reach something.
// Arguments:       None.
// Return value:    Whether the Arm will idle upon failing to reach somehting.

    bool WillIdle() { return m_WillIdle; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HoldsDevice
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this Arm is holding a HeldDevice or not.
// Arguments:       None.
// Return value:    Whether this Arm is holding a HeldDevice or not.

    bool HoldsDevice() { return m_pHeldMO && m_pHeldMO->IsDevice(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          HoldsSomething
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Indicates whether this Arm is holding an MO or not.
// Arguments:       None.
// Return value:    Whether this Arm is holding anyhting.

    bool HoldsSomething() { return m_pHeldMO != 0; }


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GibThis
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gibs this, effectively destroying it and creating multiple gibs or
//                  pieces in its place.
// Arguments:       The impulse (kg * m/s) of the impact causing the gibbing to happen.
//					The internal blast impulse which will push the gibs away from the center.
//                  A pointer to an MO which the gibs shuold not be colliding with!
// Return value:    None.

    void GibThis(Vector impactImpulse = Vector(), float internalBlast = 10, MovableObject *pIgnoreMO = 0) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates this MovableObject. Supposed to be done every frame.
// Arguments:       None.
// Return value:    None.

	void Update() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this Arm's current graphical representation to a
//                  BITMAP of choice.
// Arguments:       A pointer to a BITMAP to draw on.
//                  The absolute position of the target bitmap's upper left corner in the Scene.
//                  In which mode to draw in. See the DrawMode enumeration for the modes.
//                  Whether to not draw any extra 'ghost' items of this MovableObject,
//                  indicator arrows or hovering HUD text and so on.
// Return value:    None.

    void Draw(BITMAP *pTargetBitmap, const Vector &targetPos = Vector(), DrawMode mode = g_DrawColor, bool onlyPhysical = false) const override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:  DrawHand
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws this Arm's hand's graphical representation to a BITMAP of
//                  choice.
// Arguments:       A pointer to a BITMAP to draw on.
//                  The absolute position of the target bitmap's upper left corner in the Scene.
//                  In which mode to draw in. See the DrawMode enumeration for the modes.
// Return value:    None.

	void DrawHand(BITMAP *pTargetBitmap, const Vector &targetPos = Vector(), DrawMode mode = g_DrawColor) const;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetMOIDs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Puts all MOIDs associated with this MO and all it's descendants into MOIDs vector
// Arguments:       Vector to store MOIDs
// Return value:    None.

	void GetMOIDs(std::vector<MOID> &MOIDs) const override;

//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ConstrainHand
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes sure the hand distance is constrained between the max length of
//                  this Arm and half the max length.
// Arguments:       None.
// Return value:    Whether.the Arm already was within the reach range of this Arm or not.

    bool ConstrainHand();


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
//    // The location of the 'hand' in relation to the MovableObject::m_Pos
//    Vector m_HandPos;
    // The MovableObject held, updated and held by this Arm, if any.
    MovableObject *m_pHeldMO;
    // Whether or not this arm is currently supporting something held in another hand
    bool m_Supporting;
    // The file containing the hand bitmap.
    ContentFile m_HandFile;
    // The small bitmap holding the hand bitmap.
    BITMAP *m_pHand;
	// The maximum reaching length of this Arm. Max distance between the joint position and the hand offset. Length of a straight arm sprite in pixels.
    float m_MaxLength;
    // Current offset position of the hand relative to m_JointPos.
    Vector m_HandOffset;
    // The target position that this Arm's hand is reaching after.
    // If (0, 0), the Arm is currently not reaching after anything.
    Vector m_TargetPoint;
    // The target offset relative to m_JointPos that this Arm's hand is moving to while not reaching for or doing anything else.
    Vector m_IdleOffset;
    // How fast the arm moves to a reach target,
    // on a scale from 0.0 (frozen) to 1.0 (instantly there).
    float m_MoveSpeed;
    // Wether this Arm will go to idle position if it didn't reach or not.
    bool m_WillIdle;
    // Whether this Arm reached the reach target last Update.
    bool m_DidReach;


//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

private:


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this Arm, effectively
//                  resetting the members of this abstraction level only.
// Arguments:       None.
// Return value:    None.

    void Clear();


    // Disallow the use of some implicit methods.
	Arm(const Arm &reference) {}
	Arm & operator=(const Arm &rhs) {}

};

} // namespace RTE

#endif // File