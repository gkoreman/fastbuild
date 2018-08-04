// TestMultipleOutput.cpp
//------------------------------------------------------------------------------

// Includes
//------------------------------------------------------------------------------
#include "FBuildTest.h"
#include "Tools/FBuild/FBuildCore/FBuild.h"
#include "Tools/FBuild/FBuildCore/Graph/NodeGraph.h"

#include "Core/FileIO/FileIO.h"
#include "Core/Strings/AStackString.h"

// TestCopy
//------------------------------------------------------------------------------
class TestMultipleOutputs : public FBuildTest
{
private:
    DECLARE_TESTS

    void SimpleMultiOut() const;
	void BuildSecondaryObjectList() const;
	void BuildSecondaryFile() const;
	void CheckConflictingCompilerOptions() const;
};

// Register Tests
//------------------------------------------------------------------------------
REGISTER_TESTS_BEGIN(TestMultipleOutputs)
    REGISTER_TEST(SimpleMultiOut)
	REGISTER_TEST( BuildSecondaryFile )
	REGISTER_TEST( BuildSecondaryObjectList )
	REGISTER_TEST( CheckConflictingCompilerOptions )
REGISTER_TESTS_END


//------------------------------------------------------------------------------
void TestMultipleOutputs::SimpleMultiOut() const
{
    FBuildTestOptions options;
    options.m_ConfigFile = "Tools/FBuild/FBuildTest/Data/TestMultipleOutputs/multipleoutputs.bff";
    FBuild fBuild( options );
    TEST_ASSERT( fBuild.Initialize() );

	AStackString<> dst0( "../tmp/Test/MultipleOutputs/a.obj" );
    AStackString<> dst1( "../tmp/Test/MultipleOutputs/a.aaa" );
	AStackString<> dst2( "../tmp/Test/MultipleOutputs/a.bbb" );

    // clean up anything left over from previous runs
	EnsureFileDoesNotExist( dst0 ); 
	EnsureFileDoesNotExist( dst1 );
	EnsureFileDoesNotExist( dst2 );

    // build (via alias)
    TEST_ASSERT( fBuild.Build( AStackString<>( "All" ) ) );
    TEST_ASSERT( fBuild.SaveDependencyGraph( "../tmp/Test/MultipleOutputs/multipleoutputs.fdb" ) );

    // make sure all output is where it is expected
	EnsureFileExists( dst0 ); 
	EnsureFileExists( dst1 );
	EnsureFileExists( dst2 );

    // Check stats
    //               Seen,  Built,  Type
    CheckStatsNode ( 1,     1,      Node::ALIAS_NODE );
	CheckStatsNode ( 4,     4,      Node::FILE_NODE );
	CheckStatsNode ( 1,     1,      Node::OBJECT_NODE );
	CheckStatsNode ( 3,     3,      Node::OBJECT_LIST_NODE );
	CheckStatsNode ( 1,     1,      Node::COMPILER_NODE );
    CheckStatsTotal( 10,   10 );
}


//------------------------------------------------------------------------------
void TestMultipleOutputs::BuildSecondaryFile() const
{
	FBuildTestOptions options;
	options.m_ConfigFile = "Tools/FBuild/FBuildTest/Data/TestMultipleOutputs/multipleoutputs.bff";
	FBuild fBuild( options );
	TEST_ASSERT( fBuild.Initialize( "../tmp/Test/MultipleOutputs/multipleoutputs.fdb" ) );

	AStackString<> dst0( "../tmp/Test/MultipleOutputs/a.obj" );
	AStackString<> dst1( "../tmp/Test/MultipleOutputs/a.aaa" );
	AStackString<> dst2( "../tmp/Test/MultipleOutputs/a.bbb" );

	// clean up anything left over from previous runs
	EnsureFileDoesNotExist( dst0 );
	EnsureFileDoesNotExist( dst1 );
	EnsureFileDoesNotExist( dst2 );

	// build one of the files that was listed in the last run
	//TEST_ASSERT( fBuild.Build( AStackString<>( "D:\\fastbuild\\tmp\\Test\\MultipleOutputs\\a.obj" ) ) );
	TEST_ASSERT( fBuild.Build( AStackString<>( "ObjectList-bbb" ) ) );

	// make sure all output is where it is expected
	EnsureFileExists( dst0 );
	EnsureFileExists( dst1 );
	EnsureFileExists( dst2 );

	// Check stats
	//               Seen,  Built,   Type
	CheckStatsNode(     0,      0,   Node::ALIAS_NODE );
	CheckStatsNode(     2,      2,   Node::FILE_NODE );			// See and build one .cpp file and one .bbb output file
	CheckStatsNode(     1,      1,   Node::OBJECT_NODE );
	CheckStatsNode(     1,      1,   Node::OBJECT_LIST_NODE );
	CheckStatsNode(     1,      0,   Node::COMPILER_NODE );
	CheckStatsTotal(    5,      4 );
}

//------------------------------------------------------------------------------
void TestMultipleOutputs::BuildSecondaryObjectList() const
{
	FBuildTestOptions options;
	options.m_ConfigFile = "Tools/FBuild/FBuildTest/Data/TestMultipleOutputs/multipleoutputs.bff";
	FBuild fBuild( options );
	TEST_ASSERT( fBuild.Initialize( "../tmp/Test/MultipleOutputs/multipleoutputs.fdb" ) );

	AStackString<> dst0( "../tmp/Test/MultipleOutputs/a.obj" );
	AStackString<> dst1( "../tmp/Test/MultipleOutputs/a.aaa" );
	AStackString<> dst2( "../tmp/Test/MultipleOutputs/a.bbb" );

	// clean up anything left over from previous runs
	EnsureFileDoesNotExist( dst0 );
	EnsureFileDoesNotExist( dst1 );
	EnsureFileDoesNotExist( dst2 );

	// build one of the files that was listed in the last run
	TEST_ASSERT( fBuild.Build( AStackString<>( "..\\tmp\\Test\\MultipleOutputs\\a.bbb" ) ) );
	
	// make sure all output is where it is expected
	EnsureFileExists( dst0 );
	EnsureFileExists( dst1 );
	EnsureFileExists( dst2 );

	// Check stats
	//               Seen,  Built,   Type
	CheckStatsNode(     0,      0,   Node::ALIAS_NODE );
	CheckStatsNode(     2,      2,   Node::FILE_NODE );			// See and build one .cpp file and one .bbb output file
	CheckStatsNode(     1,      1,   Node::OBJECT_NODE );
	CheckStatsNode(     0,      0,   Node::OBJECT_LIST_NODE );
	CheckStatsNode(     1,      0,   Node::COMPILER_NODE );
	CheckStatsTotal(    4,      3 );
}

//------------------------------------------------------------------------------
void TestMultipleOutputs::CheckConflictingCompilerOptions() const
{
	FBuildTestOptions options;
	options.m_ConfigFile = "Tools/FBuild/FBuildTest/Data/TestMultipleOutputs/multipleoutputs.bff";
	FBuild fBuild( options );
	TEST_ASSERT( fBuild.Initialize() );

	TEST_ASSERT( fBuild.Build( AStackString<>( "ConflictingCompilerOptions" ) ) == false );
	TEST_ASSERT( GetRecordedOutput().Find( "Conflicting CompilerOptions found:" ) ); 
}