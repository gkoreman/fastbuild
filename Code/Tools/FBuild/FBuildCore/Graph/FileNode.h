// FileNode.h - a node that tracks a single file
//------------------------------------------------------------------------------
#pragma once

// Includes
//------------------------------------------------------------------------------
#include "Node.h"

// FileNode
//------------------------------------------------------------------------------
class FileNode : public Node
{
public:
    explicit FileNode( const AString & fileName, uint32_t controlFlags );
    virtual bool Initialize( NodeGraph & nodeGraph, const BFFIterator & funcStartIter, const Function * function ) override;
    virtual ~FileNode();

    static inline Node::Type GetTypeS() { return Node::FILE_NODE; }

    virtual bool IsAFile() const override { return true; }

    static void HandleWarningsMSVC( Job * job, const AString & name, const char * data, uint32_t dataSize );

	void InitializeAsOutputFile( const BFFIterator & funcStartIter, const Function * function, Node & creator );

	inline Node* GetCreator() const { return m_StaticDependencies.GetSize() == 0 ? nullptr : m_StaticDependencies[0].GetNode(); }
protected:
	virtual void PostLoad( NodeGraph & nodeGraph ) override;
    virtual BuildResult DoBuild( Job * job ) override;

    static void DumpOutput( Job * job, const char * data, uint32_t dataSize, const AString & name, bool treatAsWarnings = false );

    friend class Client;
};

//------------------------------------------------------------------------------
