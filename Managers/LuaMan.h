#ifndef _RTELUAMAN_
#define _RTELUAMAN_

//////////////////////////////////////////////////////////////////////////////////////////
// File:            LuaMan.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Header file for the LuaMan class.
// Project:         Retro Terrain Engine
// Author(s):       Daniel Tabar
//                  data@datarealms.com
//                  http://www.datarealms.com


//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

#include "DDTTools.h"
#include "Singleton.h"
#define g_LuaMan LuaMan::Instance()

#include "Serializable.h"
#include "Entity.h"

// Forward declarations
struct lua_State;

namespace RTE
{

#define MAX_OPEN_FILES 10

//////////////////////////////////////////////////////////////////////////////////////////
// Class:           LuaMan
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     The singleton manager of the master lua script state.
// Parent(s):       Singleton, Serializable?
// Class history:   3/13/2008 LuaMan created.

class LuaMan:
    public Singleton<LuaMan>//,
//    public Serializable
{


//////////////////////////////////////////////////////////////////////////////////////////
// Public member variable, method and friend function declarations

public:

/*
enum ServerResult
{
    SUCCESS = 0,
    FAILEDCONNECTION,
    INVALIDXML,
    MAXCOUNT,
    INVALIDKEY,
    INVALIDPRODUCT,
    EXPIREDKEY,
    INVALIDMACHINE,
    UNKNOWNERROR
};
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Constructor:     LuaMan
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Constructor method used to instantiate a LuaMan object in system
//                  memory. Create() should be called before using the object.
// Arguments:       None.

    LuaMan() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Destructor:      ~LuaMan
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destructor method used to clean up a LuaMan object before deletion
//                  from system memory.
// Arguments:       None.

    virtual ~LuaMan() { Destroy(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Makes the LuaMan object ready for use.
// Arguments:       None.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Create();

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  ReadProperty
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a property value from a Reader stream. If the name isn't
//                  recognized by this class, then ReadProperty of the parent class
//                  is called. If the property isn't recognized by any of the base classes,
//                  false is returned, and the Reader's position is untouched.
// Arguments:       The name of the property to be read.
//                  A Reader lined up to the value of the property to be read.
// Return value:    An error return value signaling whether the property was successfully
//                  read or not. 0 means it was read successfully, and any nonzero indicates
//                  that a property of that name could not be found in this or base classes.

    virtual int ReadProperty(std::string propName, Reader &reader);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Save
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Saves the complete state of this LuaMan to an output stream for
//                  later recreation with Create(Reader &reader);
// Arguments:       A Writer that the LuaMan will save itself with.
// Return value:    An error return value signaling sucess or any particular failure.
//                  Anything below 0 is an error signal.

    virtual int Save(Writer &writer) const;
*/

//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  Reset
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Resets the entire LuaMan, including its inherited members, to
//                  their default settings or values.
// Arguments:       None.
// Return value:    None.

    virtual void Reset() { Clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Destroy
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Destroys and resets (through Clear()) the LuaMan object.
// Arguments:       None.
// Return value:    None.

    void Destroy();


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual method:  GetClassName
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the class name of this Entity.
// Arguments:       None.
// Return value:    A string with the friendly-formatted type name of this object.

    virtual const std::string & GetClassName() const { return m_ClassName; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetLastError
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns the last error message from executing scripts.
// Arguments:       None.
// Return value:    The error string with hopefully meaningful info about what went wrong.

    std::string GetLastError() const { return m_LastError; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearErrors
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears out all the error string to "". Will cause ErrorExists to return
//                  true again until RunScriptString is called.
// Arguments:       None.
// Return value:    None.

    void ClearErrors() { m_LastError.clear(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ErrorExists
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Tells whether there are any errors reported waiting to be read.
// Arguments:       None.
// Return value:    Whether errors exist.

    bool ErrorExists() const { return m_LastError.empty(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SavePointerAsGlobal
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Takes a pointer to an object and saves it in the Lua state as a global
//                  of a specified variable name.
// Arguments:       The pointer to the object to save. Ownership is NOT transferred!
//                  The name of the global var in the Lua state to save the pointer to.
// Return value:    Returns less than zero if any errors encountered when doing this.

    int SavePointerAsGlobal(void *pToSave, std::string globalName);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GlobalIsDefined
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if there is anything defined on a specific global var in Lua.
// Arguments:       The name of the global var in the Lua state to check.
// Return value:    Whether that global var has been defined yet in the Lua state.

    bool GlobalIsDefined(std::string globalName);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          TableEntryIsDefined
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if there is anything defined in a specific index of a table.
// Arguments:       The name of the table to look inside.
//                  The name of the index to check inside that table.
// Return value:    Whether that table var has been defined yet in the Lua state.

    bool TableEntryIsDefined(std::string tableName, std::string indexName);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ExpressionIsTrue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the result of an arbirary expression in lua as evaluating to
//                  true or false.
// Arguments:       The string with the expression to evaluate.
//                  Whether to report any errors to the console immediately.
// Return value:    Whether the expression was true.

    bool ExpressionIsTrue(std::string expression, bool consoleErrors);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RunScriptString
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Takes a string containing a script snippet and runs it on the master
//                  state.
// Arguments:       The string with the script snippet.
//                  Whether to report any errors to the console immediately.
// Return value:    Returns less than zero if any errors encountered when running this script.
//                  To get the actual error string, call GetLastError.

    int RunScriptString(std::string scriptString, bool consoleErrors = true);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          RunScriptFile
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Opens and loads a file containing a script and runs it on the master
//                  state.
// Arguments:       The path to the file to load and run.
//                  Whether to report any errors to the console immediately.
// Return value:    Returns less than zero if any errors encountered when running this script.
//                  To get the actual error string, call GetLastError.

    int RunScriptFile(std::string filePath, bool consoleErrors = true);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetNewPresetID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns an ID string unique to this runtime for use by original presets
//                  that have scripts associated with them.
// Arguments:       None.
// Return value:    Returns the unique ID as a string.

    std::string GetNewPresetID();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetNewObjectID
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns an ID string unique to this runtime for use by individual
//                  objects that are also tracked in the Lua state and have scripts
//                  associated with them.
// Arguments:       None.
// Return value:    Returns the unique ID as a string.

    std::string GetNewObjectID();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetTempEntity
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets a temporary Entity that can be accessed in the Lua state.
// Arguments:       The temporary entity. Ownership is NOT transferred.
// Return value:    None.

    void SetTempEntity(Entity *pEntity) { m_pTempEntity = pEntity; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetTempEntity
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets a temporary Entity that can be accessed in the Lua state.
// Arguments:       None.
// Return value:    The temporary entity. Ownership is NOT transferred.

    Entity * GetTempEntity() const { return m_pTempEntity; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Update
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Updates the state of this LuaMan. Supposed to be done every frame
//                  before drawing.
// Arguments:       None.
// Return value:    None.
 
	void Update();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FileOpen
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Opens file. You can open files only inside .rte folders of game directory.
//					you can open no more that MAX_OPEN_FILES file simultaneously.
// Arguments:       Path to file. All paths are made absoulte by adding current working directory
//					to specified path. File mode.
// Return value:    File number.

	int FileOpen(std::string filename, std::string mode);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FileClose
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Closes a previously opened file.
// Arguments:       File number.
// Return value:    None.

	void FileClose(int file);

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FileCloseAll
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Closes all previously opened files.
// Arguments:       None.
// Return value:    None.

	void FileCloseAll();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FileReadLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Reads a line from file.
// Arguments:       File number.
// Return value:    Line from file, or empty string on error.

	std::string FileReadLine(int file);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FileWriteLine
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Writes a text line to file.
// Arguments:       File number.
// Return value:    None.

	void FileWriteLine(int file, std::string line);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          FileEOF
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Returns true if end of file was reached
// Arguments:       File number.
// Return value:    Whether or not EOF was reached.

	bool FileEOF(int file);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearUserModuleCache
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears internal Lua package tables from all user-defined modules. Those must be reloaded with ReloadAllScripts()
// Arguments:       None.
// Return value:    None.

	void ClearUserModuleCache();

	void CreateSolState(lua_State * L);

//////////////////////////////////////////////////////////////////////////////////////////
// Protected member variable and method declarations

protected:

    // Member variables
    static const std::string m_ClassName;

    // The master parent script state
    lua_State *m_pMasterState;

    // Description of the last error that occurred in the script execution
    std::string m_LastError;
    // The next unique preset ID to hand out to the next Preset that wants to define some functions
    // This gets incremented each time a new one is requeste to give unique ID's to all original presets
    long m_NextPresetID;
    // The next unique object ID to hand out to the next scripted Entity instance that wants to run its preset's scripts
    // This gets incremented each time a new one is requested to give unique ID's to all scripted objects
    long m_NextObjectID;
    // Temporary holder for an Entity object that we want to pass into the Lua state without fuss
    Entity *m_pTempEntity;


//////////////////////////////////////////////////////////////////////////////////////////
// Private member variable and method declarations

private:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Clear
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears all the member variables of this LuaMan, effectively
//                  resetting the members of this abstraction level only.
// Arguments:       None.
// Return value:    None.

    void Clear();


    // Disallow the use of some implicit methods.
    LuaMan(const LuaMan &reference);
    LuaMan & operator=(const LuaMan &rhs);

	//Internal list of opened files used by File* functions 
	FILE * m_Files[MAX_OPEN_FILES];
};

} // namespace RTE

#endif // File