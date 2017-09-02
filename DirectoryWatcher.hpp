///----------------------------------------------------
// Author:Karthik Hebbar M N
// Website http://fissionspark.weebly.com/
//----------------------------------------------------------

//----------------Info---------------------------
// Use ful for checking changes in a directory (Used in our engine to check 
//                      if a file was  moved or added to the project folder)
//-----------------------------------------------
//#define _SCL_SECURE_NO_WARNINGS



#if defined(_WIN32) 
#define PLATFORM_WINDOWS
#elif __APPLE__
#define PLATFORM_MAC
#elif __linux
#define PLATFORM_LINUX
#endif
//-------------------------------------------
// Code Begins Here
//---------------------------------
#ifndef DIRECTORY_WATCHER_H
#define DIRECTORY_WATCHER_H

#define USE_DIRENT



//------------------------
// Includes
//--------------
#include<boost/serialization/vector.hpp>
#include<iostream>
#include<fstream>

#include<sys\stat.h>
#include <tchar.h>


#ifdef PLATFORM_WINDOWS
#include<Windows.h>
#endif


#include<boost\archive\text_oarchive.hpp>
#include<boost\archive\text_iarchive.hpp>
#include<boost\serialization\string.hpp>
#include<boost\serialization\map.hpp>
#include <boost/foreach.hpp>

#include<boost/filesystem/detail/utf8_codecvt_facet.hpp>

#include<boost\serialization\serialization.hpp>
#include<boost\foreach.hpp>

#ifdef USE_BOOST
#include<boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/lexical_cast.hpp>
#endif

#include <stdio.h>

#ifdef USE_DIRENT

extern "C"
{
#include<sys/stat.h>
#include <dirent.h>
}
#endif
struct stat s;

using namespace std;
using namespace boost;

//----------------------------------------
///Static Methods 
//----------------------------------------


#ifndef STRING_CONTAINS
#define STRING_CONTAINS

#define TYPE_SEARCH_END 1
#define TYPE_SEARCH_BEGIN 2
#define TYPE_SEARCH_ANYWHERE 3


static bool StringContains(const string & Source,const string & Targetstring , int TypeOfSearch)
{
    if (TypeOfSearch == TYPE_SEARCH_ANYWHERE)
    {
        size_t Found = Source.find(Targetstring);

        if (Found != std::string::npos)
            return true;
        else
            return false;

    }

    if (TypeOfSearch == TYPE_SEARCH_END)
    {
        if (Source.length() >= Targetstring.length())
        {
            return (0 == 
                (Source.compare(Source.length() - Targetstring.length(), Targetstring.length(), Targetstring)
                ) );
        }else
        {
            return false;
        }
    }

    if (TypeOfSearch == TYPE_SEARCH_BEGIN)
        {
            if (Source.length() >= Targetstring.length())
            {
                return (0 ==
                    (Source.compare(0, Targetstring.length(), Targetstring)
                    ));
            }
            else
            {
                return false;
            }
       }
}

#endif
//Gets the Size of file in Bytes
static  string GetExtension(const string& Var // Path or fileName
    )
{
  
    string Extension = "";
    int FounDot = 0;
    for (auto itr = Var.begin(); itr < Var.end();++itr)
    {
        if (FounDot == 0)
        {
            if (*itr == '.')
            {
                FounDot = 1;

            }
        }
        else{
            if (*itr == '.')
            {
                Extension = "";
            }
        
        }

        if (FounDot == 1)
        {
        
            Extension.push_back(*itr);
        }
    }

    return Extension;

}
static long DW_GetFileSize(const string& Path // Path ot the files with extension Eg.C:/Test.txt
    )
{
    ifstream ifs(Path, ios::in);
    ifs.seekg(0, ios::end);
    auto Size = ifs.tellg();
    ifs.close();

    return Size;

}
// Checks if a filke exist in provieded path iwth extension 
// Eg. C:\\Program Files\\File.txt
 static bool CheckifFileExists(const string& Path)
{
    struct stat buffer;

    if (stat(Path.c_str(), &buffer) == 0)
        return true;
    else
        return false;

}

 static bool isUpDirecory(const char* directory) {
     if (strcmp(directory, "..") == 0 || strcmp(directory, ".") == 0)
         return true;
     else
         return false;
 }

//Meta File
template<typename T>
struct Meta
{
    string Name,Path;
    size_t GUID;
     double Size;

    // Typ 
    // typ =0 Folder
    //typ = -1 unknow file
    int Type;

    // Can be anything like list of name or list of animation files or list of folder with a folder
     vector<T> MetaInfo;

     friend class boost::serialization::access;

     template<typename Archive>
     void serialize(Archive & ar, const unsigned version) {
         ar & Name;
         ar & Path;
         ar & GUID;
         ar & Size;
         ar & Type;
         ar & MetaInfo;
     }


};


//---------------------------------------------
// TYPE DEFS
//-------------------------------------

 typedef  std::hash<string> HashFunction;
 typedef Meta<string> FOLDER_META;


struct ChildrenTree
{
    vector<Meta<string>> Children;

};

struct DirectoryTree
{

    string ParentPath;
    //map<Children Name,Related Info>
    map<string, FOLDER_META> Child;

};

struct Changed_Data
{
    FOLDER_META Old;
    FOLDER_META New;
};

//------------------------------------
//operators
//------------------------------------
static bool operator ==(const FOLDER_META& metaA,const FOLDER_META& metaB)
{
    if (metaA.GUID == metaB.GUID && metaA.Name == metaB.Name && metaA.Size == metaB.Size)
    {
        return true;
    }
    else{
        return false;
    }
}
/*
#ifdef USE_BOOST

class DirectoryWatcher
{
    //-----------------Type Defs------------------------

    typedef  boost::filesystem::path PATH;
    typedef boost::uuids::uuid UUID;

public:
    // Default Constructor
    DirectoryWatcher(); 

    // String as Path to the Directory
    // Example C;/ProgramFiles/Project
    DirectoryWatcher(const string&);

    ~DirectoryWatcher();

    // Obtains a list of files and folders in a directory
     void GetList(const string&,vector<string>* ReturnValue );

    // Searches and Returns the pathto the file
     bool FindFile(const string& FileName // File Name
        ,string* Ret //Path Returned 
        ); 

    //Update and check to see if a file as moved
     void Update();

    // Creates a GUID for a file 
     UUID CreateGUID(PATH fileName);

private:

    PATH Project_Path;




};

#endif

*/
#ifdef USE_DIRENT
class DirectoryWatcher
{
    //-----------------Type Defs------------------------

    //typedef Meta<string> FOLDER_META;
public:
    //-----------
    //ChangeTypeEnum
    //------------------
    enum class ChangeType
    {
        Size_Change,
        Path_Change,
        No_Change,


    };

    //Varaibles
    FOLDER_META defaultMeta;
    Meta<DirectoryTree> TreeMeta;

    // Default Constructor

    DirectoryWatcher(void);

   
    ~DirectoryWatcher(void){
        delete tempDir;
        delete ent;
        delete MainDir;
        delete dir;
    }

    // Obtains a list of files and folders in a directory
     void GetList(const string&, vector<FOLDER_META>* ReturnValue ,FOLDER_META* ThisDireMeta,bool recursive);

    // Searches and Returns the pathto the file
    
    bool FindFile(const string& Path
        ,const string& FileName // File Name
        , string* Ret //Path Returned 
        );

    //Update and check to see if a file as moved or added or changed 
    // Monitor(vector<FOLDER_META>* ChangedFiles,bool* FilesChanged -> return types); 
     void Monitor(vector<Changed_Data>* ChangedFiles,bool* FilesChanged);

    // Creates a GUID for a file 
     size_t CreateGUID(const string& fileName);

    //Export metadata
     void ExportMeta(const string& Path,FOLDER_META meta);
    // Get the meta data
      void GetFolderMeta(const string& Path,Meta<string> * ReturnValue );

    //InitalizeMethod 
    // False if path invalid
    // true if path correct
     bool Init(const string& Path //Path to the Project folder (The folder to watch)
        );

     bool Init(const string& Path //Path to the Project folder (The folder to watch)
        ,vector<FOLDER_META> * Returnmetas);

     void MakeDir(const string& path);



     void DoForceReimport(const string& FIlePath)
     {
         ForcedFilePath = FIlePath;
         ForceReimport = true;
     }


     // check if metas are equal 
     // if not tell type of change 
     ChangeType CheckMetas(FOLDER_META& A, FOLDER_META& B)
     {
     
         ChangeType typeofchange = ChangeType::No_Change;

         if (A.Size != B.Size)
         {
             if (typeofchange == ChangeType::No_Change)
                 typeofchange = ChangeType::Size_Change;
               
         }

         if (A.Path != B.Path)
         {
             if (typeofchange == ChangeType::No_Change)
                 typeofchange = ChangeType::Path_Change;

         }

         return typeofchange;

         
     }

private:

    string Project_Path;
    string Project_Path_Name;
    DIR* dir = nullptr;
    DIR* MainDir = nullptr;
    struct dirent* ent = nullptr;
    DIR* tempDir = nullptr;
    DirectoryTree Tree;
    HashFunction hashFunc;
    bool ForceReimport;

    string ForcedFilePath;

    //Dpeth Search 
    DirectoryTree UnVisited;
    

    //-------------------------------------
    // Windows Specifif cCode
    //---------------------------------
    HANDLE ChangeNotifications[2];
    TCHAR lpDrive[4];
    TCHAR lpFile[_MAX_FNAME];
    TCHAR lpExt[_MAX_EXT];
    DWORD Notifications; 
    

    // Platform Specific Code




    // Private methods
     long GetFolderSize( const string& Path);
     bool CheckPathValid(const string& Path);

     void RefreshFiles(vector<Changed_Data>* ChangedFiles,const string& path,bool OnlyThisFolder);
     void RefreshTree(vector<Changed_Data>* ChangedFiles, const string& path);
     void CreateTree(const string& Path );

     void SaveTree();
     void LoadTree();
     bool AreChildEqual(const FOLDER_META& metaA,const FOLDER_META& metaB );

     typedef std::map<std::string, FOLDER_META>::iterator _TreeItr;

     //import a file
     void ReImport(const string& FilePath)
     {



     }



};
#endif

#endif // !DIRECTORY_WATCHER_H
