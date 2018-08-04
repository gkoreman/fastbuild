// FileNode.cpp
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "Tools/FBuild/FBuildCore/PrecompiledHeader.h"

#include "FileNode.h"
#include "ObjectNode.h"
#include "Tools/FBuild/FBuildCore/Error.h"
#include "Tools/FBuild/FBuildCore/FBuild.h"
#include "Tools/FBuild/FBuildCore/FLog.h"
#include "Tools/FBuild/FBuildCore/Graph/NodeGraph.h"

#include "Core/Containers/AutoPtr.h"
#include "Core/FileIO/FileIO.h"
#include "Core/FileIO/FileStream.h"
#include "Core/Strings/AStackString.h"

// CONSTRUCTOR
//------------------------------------------------------------------------------
FileNode::FileNode( const AString & fileName, uint32_t controlFlags )
: Node( fileName, Node::FILE_NODE, controlFlags )
{
    ASSERT( fileName.EndsWith( "\\" ) == false );
    #if defined( __WINDOWS__ )
        ASSERT( ( fileName.FindLast( ':' ) == nullptr ) ||
                ( fileName.FindLast( ':' ) == ( fileName.Get() + 1 ) ) );
    #endif

    m_LastBuildTimeMs = 1; // very little work required
}

// Initialize
//------------------------------------------------------------------------------
/*virtual*/ bool FileNode::Initialize( NodeGraph & /*nodeGraph*/, const BFFIterator & /*funcStartIter*/, const Function * /*function*/ )
{
    ASSERT( false ); // Should never get here
    return false;
}

// DESTRUCTOR
//------------------------------------------------------------------------------
FileNode::~FileNode() = default;


// PostLoad
//------------------------------------------------------------------------------
/*virtual*/ void FileNode::PostLoad( NodeGraph & /*nodeGraph*/ )
{
	if( GetType() == Node::FILE_NODE )
	{
		// Since this node depends on it's creator (as a static dependency)
		// we know that the creator is serialized and deserialized before 
		// this node is loaded and it is safe to link this node up with it now.
		Node* creator = GetCreator();
		if( creator != nullptr )
		{
			ObjectNode* objectNode = creator->CastTo<ObjectNode>();
			objectNode->AppendOutputFile( this );
		}
	}
}

void FileNode::InitializeAsOutputFile( const BFFIterator & funcStartIter, const Function * function, Node & creator)
{
	if( m_StaticDependencies.IsEmpty() )
	{
		m_StaticDependencies.SetCapacity( 1 );
		m_StaticDependencies.Append( Dependency( &creator ) );
	}
	else
	{
		ASSERT( m_StaticDependencies.GetSize() == 1 );
		if( m_StaticDependencies[0].GetNode() != &creator )
		{
			// TODO: Consider creating another error type for this?
			Error::Error_1100_AlreadyDefined( funcStartIter, function, GetName() );
		}
	}
}

// DoBuild
//------------------------------------------------------------------------------
/*virtual*/ Node::BuildResult FileNode::DoBuild( Job * UNUSED( job ) )
{
    m_Stamp = FileIO::GetFileLastWriteTime( m_Name );
    return NODE_RESULT_OK;
}

// HandleWarningsMSVC
//------------------------------------------------------------------------------
void FileNode::HandleWarningsMSVC( Job * job, const AString & name, const char * data, uint32_t dataSize )
{
    if ( ( data == nullptr ) || ( dataSize == 0 ) )
    {
        return;
    }

    // Are there any warnings? (string is ok even in non-English)
    if ( strstr( data, ": warning " ) )
    {
        const bool treatAsWarnings = true;
        DumpOutput( job, data, dataSize, name, treatAsWarnings );
    }
}

// DumpOutput
//------------------------------------------------------------------------------
void FileNode::DumpOutput( Job * job, const char * data, uint32_t dataSize, const AString & name, bool treatAsWarnings )
{
    if ( ( data != nullptr ) && ( dataSize > 0 ) )
    {
        Array< AString > exclusions( 2, false );
        exclusions.Append( AString( "Note: including file:" ) );
        exclusions.Append( AString( "#line" ) );

        AStackString<> msg;
        msg.Format( "%s: %s\n", treatAsWarnings ? "WARNING" : "PROBLEM", name.Get() );

        AutoPtr< char > mem( (char *)Alloc( dataSize + msg.GetLength() ) );
        memcpy( mem.Get(), msg.Get(), msg.GetLength() );
        memcpy( mem.Get() + msg.GetLength(), data, dataSize );

        Node::DumpOutput( job, mem.Get(), dataSize + msg.GetLength(), &exclusions );
    }
}

//------------------------------------------------------------------------------
