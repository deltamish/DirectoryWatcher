#include "DirectoryWatcher.hpp"


//-------------------------------------------
//Author :Karthik Hebbar M N
//website http://fissionspark.weebly.com/
//------------------------------------------


/*
#ifdef USE_BOOST
DirectoryWatcher::DirectoryWatcher()
{
}

DirectoryWatcher::DirectoryWatcher(const string& _path) : Project_Path(_path )
{

}




DirectoryWatcher::~DirectoryWatcher()
{
}

void DirectoryWatcher:: GetList(const string& Path, vector<string>* ReturnValue)
{
    using namespace boost::filesystem;
    {

        // if the Target folder exists
        if (exists(Project_Path))
        {
          
            directory_iterator EndItr; // Default is at the end

            // Go through each file or folder
            for (directory_iterator Itr(Project_Path); Itr != EndItr; ++Itr)
            {
                if (is_directory(Itr->status() ))
                {

                  //  Meta<string> meta;

                    ReturnValue->push_back(Itr->path().string() );
                }
                else {

                    ReturnValue->push_back(Itr->path().string());
                }
               
                
            }

        }
    }

    
    
}

bool DirectoryWatcher::FindFile(const string& FileName, string* Ret)
{
    
}


void DirectoryWatcher::Update()
{}


boost::uuids::uuid  DirectoryWatcher::CreateGUID(PATH fileName)
{

}

#endif
*/


#ifdef USE_DIRENT
DirectoryWatcher::DirectoryWatcher()
{

  Project_Path = "";
  dir = nullptr;
  MainDir = nullptr;
  ent = nullptr;
  tempDir = nullptr;
}





// ----------------------------
// Initialization /methoids
//--------------------------------
bool DirectoryWatcher::Init(const string& Path)
{
    defaultMeta.GUID = 0;
    defaultMeta.Name = "";
    defaultMeta.Size = 0;
    defaultMeta.Type = -1;

    Project_Path = Path;
    if (CheckPathValid(Project_Path))
    {
        MainDir = opendir(Project_Path.c_str());

        return true;
    }
    else{
        return false;
    }
}


// Returnss a list of contnets immedeatly for displaying
bool DirectoryWatcher::Init(const string& Path,vector<FOLDER_META>* Ret)
{
    defaultMeta.GUID = 0;
    defaultMeta.Name = "";
    defaultMeta.Size = 0;
    defaultMeta.Type = -1;

    Project_Path_Name = Path;
    Project_Path = Path + "\\";
    if (CheckPathValid(Project_Path))
    {
        MainDir = nullptr;
        MainDir = opendir(Project_Path.c_str());
        GetList(Path,Ret,nullptr,true);
        CreateTree(Project_Path);

        //-------------------------------
        // WINDOWs Specific Code
        //---------------------------------
            #ifdef PLATFORM_WINDOWS
        wstring wsPath = wstring(Project_Path_Name.begin(),Project_Path_Name.end());

      //  const char * _Path = wsPath.c_str();
        const char * _Path = Project_Path_Name.c_str();

        _tsplitpath_s(_Path, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);
        lpDrive[2] = (TCHAR)'\\';
        lpDrive[3] = (TCHAR)'\0';


        // watch the project folder for change in file name or craetion of file
        ChangeNotifications[0] = FindFirstChangeNotification(_Path, TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE);

        if (ChangeNotifications[0] == INVALID_HANDLE_VALUE)

            return false;

        ChangeNotifications[1] = FindFirstChangeNotification(lpDrive,TRUE,FILE_NOTIFY_CHANGE_DIR_NAME);

        if (ChangeNotifications[1] == INVALID_HANDLE_VALUE)

            return false;

     //   ChangeNotifications[2] = FindFirstChangeNotification(_Path);

            #endif
        return true;
    }
    else{
        return false;
    }
}

// GE size of a folder
long DirectoryWatcher::GetFolderSize( const string& Path)
{
    string CurrentPath;
#ifdef PLATFORM_WINDOWS
    if (!CheckPathValid(Path))
    CurrentPath = Path + "\\"; // if C: then c:\\ or c:\\Assets theb c:\\Assets\\

#endif
    long Size = 0,tempSize;
   DIR* tempDir = nullptr;
    tempDir = opendir(CurrentPath.c_str());
    struct dirent * ent = nullptr;
       
    while ((ent = readdir(tempDir)) != NULL)
    {
    	stat(ent->d_name, &s);
        // If its a folder again go through all contents again
        if (s.st_mode & S_IFDIR)
        {
#ifdef PLATFORM_WINDOWS
            if (!isUpDirecory(ent->d_name))
            GetFolderSize(CurrentPath  + ent->d_name );
#endif
        } else{
        
            // if files then get their sizes
            
                tempSize = DW_GetFileSize((CurrentPath + ent->d_name));
            Size += tempSize;
        }

    }

    closedir(tempDir);
    return Size;

}
bool DirectoryWatcher::CheckPathValid(const string& Path)
{
     tempDir = opendir(Path.c_str());

    if (tempDir != NULL)
    {
        closedir(tempDir);
        return true;
    }
    else
    {
        closedir(tempDir);
        return false;
    }
   

}


void DirectoryWatcher::CreateTree(const string& Path)
{
 //Creatre tree for each folder
   DIR* tempDir = nullptr;
   struct dirent* ent = nullptr;
    string currentPath;
    currentPath = Path + "\\";
    FOLDER_META meta;
   
    tempDir = opendir(currentPath.c_str());

    //Go through contents and get metafiles
    while ((ent = readdir(tempDir)) != NULL)
    {
    	stat(ent->d_name, &s);
        if (s.st_mode & S_IFDIR)
        {  
            if (isUpDirecory(ent->d_name))
                continue;

            if (CheckifFileExists(Path + ".metaData"))
                GetFolderMeta(Path , &meta);

            GetList(Path, nullptr, &meta,false);
            
          
            Tree.Child[ent->d_name] = meta;

            CreateTree(currentPath +  ent->d_name);
        }

    }

    closedir(tempDir);

}
// GetList(string Path,vetor<FOLDER_META> * Ret)
void DirectoryWatcher::GetList(const string& Path, vector<FOLDER_META>*  ReturnValue,FOLDER_META* DirMeta,bool recursive)
{
    DIR * _dir = nullptr;
    struct dirent* ENT = nullptr;

    string currentPath;
   // if (!CheckPathValid(Path))
     currentPath = Path + "\\";
   _dir = opendir(currentPath.c_str());


  HashFunction Hash_func;           // Convert string to hash
  string tempPath;

    // Path Invalid Exit immedeatly
    if (_dir == NULL)
        return;
    // While contents not empty
    while ((ENT = readdir(_dir)) != NULL) 
    {
    	stat(ent->d_name, &s);
        if (s.st_mode & S_IFDIR)
        {
            if (isUpDirecory(ENT->d_name))
                continue;
            Meta<string> meta;

            if (CheckPathValid(tempPath = (Path + ENT->d_name)) == true) // If path is valid
                tempPath = Path + ENT->d_name;

            if (CheckPathValid(tempPath = (Path + "\\" + ENT->d_name)) == true) // If path is valid
                tempPath = (Path + "\\" + ENT->d_name);
            
            if (!isUpDirecory(ENT->d_name))
            {
                if (CheckifFileExists(tempPath + ".metaData"))
                {
                    GetFolderMeta(tempPath, &meta);
                    if (ReturnValue != nullptr)
                        ReturnValue->push_back(meta);
                }
                else
                {


                    meta.Path = tempPath;
                    meta.GUID = Hash_func(meta.Path);
                    meta.Name = ENT->d_name;
                    meta.Type = 0;
                  //  meta.Size = (float)GetFolderSize(Path) * 0.0000001; // Store in MB

                    meta.MetaInfo.push_back(ENT->d_name);
                    if (ReturnValue != nullptr)
                        ReturnValue->push_back(meta);

                    ExportMeta(meta.Path, meta);
                }

                if(recursive)
                GetList(meta.Path,ReturnValue,DirMeta,true);
            }

           
        }
        else{
            // If its a file then Do same stpes as above
            // with small changes

            Meta<string> meta;

            if (CheckPathValid(tempPath = (Path + ENT->d_name)) == true) // If path is valid
                tempPath = Path + ENT->d_name;

            if (CheckPathValid(tempPath = (Path + "\\" + ENT->d_name)) == true) // If path is valid
                tempPath = (Path + "\\" + ENT->d_name);

            if (GetExtension(tempPath) != ".metaData" )
            if (!isUpDirecory(ENT->d_name))
            {
                if (CheckifFileExists(tempPath + ".metaData"))
                {
                    GetFolderMeta(tempPath, &meta);
                    if (ReturnValue != nullptr)
                    ReturnValue->push_back(meta);
                }
                else
                {


                    meta.Path = tempPath;
                    meta.GUID = Hash_func(meta.Path);
                    meta.Name = ENT->d_name;
                    meta.Type = -1;
                    meta.Size = DW_GetFileSize(tempPath) * 0.0001; // Store in MB

                    if (ReturnValue != nullptr)
                    ReturnValue->push_back(meta);

                    ExportMeta(meta.Path, meta);
                }
            }

        }

        if (DirMeta != nullptr)
        {
            if (GetExtension(ENT->d_name) != ".metaData")
            DirMeta->MetaInfo.push_back(ENT->d_name);

        }
    }

    closedir(_dir);
    

}

bool DirectoryWatcher::FindFile(const string& Path,const string& FileName, string* Ret)
{
    DIR* tempDir = nullptr;
    struct dirent * ent = nullptr;
    tempDir = opendir(Path.c_str());

    if (tempDir == NULL)
    {
        cout << "Error Path is not correct " << "PAth " << Path << endl << endl;
        return false;
    }
    

    while ((ent = readdir(tempDir)) != NULL)
    {
    	stat(ent->d_name, &s);
        if (s.st_mode & S_IFDIR)
        {
            // cout << ent->d_name << endl;
            if (!isUpDirecory(ent->d_name))

                if (FindFile(Path + "\\" + ent->d_name + "\\", FileName, Ret))
                    return true;

        }
        else
        {
            cout << endl << "File Nae " << ent->d_name;
            if (FileName.compare(ent->d_name) == 0)
            {
                *Ret = (Path + ent->d_name);
                return true;
            }
        }

    }


    closedir(tempDir);

    return false;
}


bool DirectoryWatcher::AreChildEqual(const FOLDER_META& metaA, const FOLDER_META& metaB)
{
    int Counter = 0;
    BOOST_FOREACH (std::string varA , metaA.MetaInfo)
    {
    	BOOST_FOREACH(std::string varB , metaB.MetaInfo)
        {
            if (varA == varB)
            {
                ++Counter;
            }
        }
    }

    if ((metaA.MetaInfo.size() - Counter) < 3)
        return true;
    else 
        return false;
}

void DirectoryWatcher::RefreshTree(vector<Changed_Data>* ChangedFiles, const string& Paths)
{
    string CurrenmtPath;

    
        CurrenmtPath = Paths;
    

    FOLDER_META _old;
    FOLDER_META _new;


#ifdef PLATFORM_WINDOWS
    if (!StringContains(CurrenmtPath, "\\", TYPE_SEARCH_END))
        CurrenmtPath = CurrenmtPath + "\\";
#endif

   DIR* tempDir = nullptr;
    tempDir = opendir(CurrenmtPath.c_str());
    struct dirent* ent = nullptr;

    Changed_Data changedData;

    HashFunction hash_func;

   



    while ((ent = readdir(tempDir)) != NULL)
    {

    	stat(ent->d_name, &s);
        if (s.st_mode & S_IFDIR)
        {
            //Get the tree
            FOLDER_META treeMEta;
            FOLDER_META Meta;
            
            // Is the current folder in the list
            // if not its should be added or renamed
            if (!isUpDirecory(ent->d_name))
            if (UnVisited.Child.find(ent->d_name) == UnVisited.Child.end())
            { 
                //Create meta for curent folder
                Meta.Name = ent->d_name;
                Meta.Type = 0;
               // Meta.Size = GetFolderSize(CurrenmtPath + ent->d_name); // C:\\Assets\\ + Meshes -> C:\\Assest\\Meshes
                Meta.Path = CurrenmtPath + ent->d_name;
                GetList(CurrenmtPath + ent->d_name, nullptr, &Meta,false);

                Meta.GUID = hash_func(Meta.Path);


              // the current Folder dosnot exits
                for (_TreeItr Itr = UnVisited.Child.begin(); Itr != UnVisited.Child.end(); ++Itr)
                 
                {

                    
                    treeMEta = Itr->second;


                 
                   

                    
                    //Check if the Treemeat and folder meta match(like Size and its children not Path)
                    //If so then folder has been renamed 
                    //Send the cahnged values 
                    // And Refresthe children for path changes
                  //  if (((treeMEta.Size - Meta.Size) < 0.05))
                        if ( AreChildEqual(treeMEta,Meta) )
                    {
                        //Folder was Remaed

                        changedData.New = Meta;
                        changedData.Old = treeMEta;

                        ChangedFiles->push_back(changedData);

                        //replace old by new ones
                        Tree.Child.erase(treeMEta.Name);
                        Tree.Child[ent->d_name] = Meta;

                        ExportMeta(CurrenmtPath + ent->d_name, Meta);

                        //Folder was renamed .do chaned pathfor all subfolders
                        RefreshTree(ChangedFiles, (CurrenmtPath + ent->d_name));

                    

                    }
                   
                    
                        

                
                }

                {
                    // A new folder was added

                    Tree.Child[ent->d_name] = Meta;

                    changedData.New = Meta;
                    changedData.Old = defaultMeta;

                    ChangedFiles->push_back(changedData);

                    ExportMeta(CurrenmtPath + ent->d_name, Meta);

                }

            }else{

           

                if (CheckifFileExists(CurrenmtPath + ent->d_name + ".metaData"))
                {
                    GetFolderMeta(CurrenmtPath + ent->d_name, &treeMEta);

                    
                }

                // Update meta info again to cross verify
                Meta = UnVisited.Child[ent->d_name];
               // Meta.Size = GetFolderSize(CurrenmtPath + ent->d_name) * 0.0000001;// check for change in size so taht we can update if file was added
                Meta.Path = CurrenmtPath + ent->d_name;
                Meta.GUID = hash_func(Meta.Path);
                Meta.Type = 0;

                 
                if ( treeMEta.MetaInfo == Meta.MetaInfo  && treeMEta.Path == Meta.Path)
                {

                   

                    //check all folders  for new path
                    RefreshTree(ChangedFiles, CurrenmtPath + ent->d_name);

                }
                else{
                
                    // If parent folder was changed
                    changedData.New = Meta;
                    changedData.Old = Tree.Child[ent->d_name];
                    ChangedFiles->push_back(changedData);

                    Tree.Child[ent->d_name] = Meta;
                    //Save updated meta
                    ExportMeta(CurrenmtPath + ent->d_name, Meta);

                    //Remap all folders to their new path
                    RefreshTree(ChangedFiles, CurrenmtPath + ent->d_name);

                    //Remap all these files to  their new corresponding Path
                   // RefreshFiles(ChangedFiles, CurrenmtPath + ent->d_name,true);
                
                }
                
                //Remove visited node for faster search
                UnVisited.Child.erase(ent->d_name);

               


            }

            
          
        }
    }

    closedir(tempDir);
}

void DirectoryWatcher::RefreshFiles(vector<Changed_Data>* ChangedFiles, const string& Paths ,bool Onlythisfolder)
{
    string CurrenmtPath;

  
        CurrenmtPath = Paths;

    FOLDER_META _old;
    FOLDER_META _new;

#ifdef PLATFORM_WINDOWS
    if (!StringContains(CurrenmtPath,"\\",TYPE_SEARCH_END))
        CurrenmtPath = CurrenmtPath + "\\";
#endif

   DIR*  tempDir = nullptr;
    tempDir = opendir(CurrenmtPath.c_str());
  
    struct dirent * ent = nullptr;
    Changed_Data changedData;

    HashFunction hash_func;

   
    
    while ((ent = readdir(tempDir)) != NULL)
    {
    	stat(ent->d_name, &s);
        if (!isUpDirecory(ent->d_name))
        if (!(s.st_mode & S_IFDIR))
        {
            
            // Check if a file was added
           // Go through all files and check if .met existis if not then file was added
            //else File aws renamed or its path got changed
            // C:\\Data\\Test.fbx.metaData
            if (! StringContains(ent->d_name,".metaData", TYPE_SEARCH_ANYWHERE))
            if (CheckifFileExists(CurrenmtPath + ent->d_name +".metaData"))
            {
                GetFolderMeta(CurrenmtPath + ent->d_name, &_old);

                _new.GUID = hash_func(CurrenmtPath + ent->d_name);
                _new.Name = ent->d_name;
                _new.Type = _old.Type;
                _new.Size = DW_GetFileSize(CurrenmtPath + ent->d_name) * 0.0001;
                _new.Path = CurrenmtPath + ent->d_name;

                if (!(_new == _old))
                {
                    changedData.New = _new;
                    changedData.Old = _old;

                    ChangedFiles->push_back(changedData);

                  // Dont savemeta as the obtain meta maybe of different type let resource manager save it asits corresponding type
                }

            }
            else
            {
                // New file was added
                _new.GUID = hash_func(CurrenmtPath + ent->d_name);
                _new.Name = ent->d_name;
                _new.Type = -1;
                _new.Size = DW_GetFileSize(CurrenmtPath + ent->d_name) * 0.0001;
                _new.Path = CurrenmtPath + ent->d_name;
                
                ExportMeta(_new.Path,_new); // Save the file

                changedData.New = _new;
                changedData.Old = defaultMeta;
                ChangedFiles->push_back(changedData);
            }
            
        }
        else
        {
            if (!Onlythisfolder)
            RefreshFiles(ChangedFiles, (CurrenmtPath +ent->d_name),false); // Go through all folders for change in filess
        }
    }

    closedir(tempDir);
}
//Update and check to see if a file as moved or added or changed 
// Monitor(vector<FOLDER_META>* ChangedFiles,bool* FilesChanged -> return types);
void DirectoryWatcher::Monitor(vector<Changed_Data>* ChangedFiles,bool* FilesChanged )
{
    // See if the main folder has the Size stored in the file
    //if not do calculation 

    
    if (CheckifFileExists(Project_Path_Name + ".metaData"))
    {



        //----------------------------------------------------------------
        // Window Specific Code g
        //----------------------------------------------------------------
#ifdef PLATFORM_WINDOWS
        Notifications = WaitForMultipleObjects(2, ChangeNotifications, FALSE, INFINITY);

       

        switch (Notifications)
        {
        case WAIT_OBJECT_0:

            // File created or deleted

            //Refresh
            RefreshFiles(ChangedFiles, Project_Path, false);
            *FilesChanged = true;
         
            if (FindNextChangeNotification(ChangeNotifications[0]) == FALSE)
            {
                // printf("\n ERROR: FindNextChangeNotification function failed.\n");
                ExitProcess(GetLastError());
            }
            break;

        case WAIT_OBJECT_0 + 1:

            // Folder created or deleted
            // Done only when notification changes 
            UnVisited.Child.insert(Tree.Child.begin(), Tree.Child.end());
            RefreshTree(ChangedFiles,Project_Path );
            *FilesChanged = true;
     

            if (FindNextChangeNotification(ChangeNotifications[0]) == FALSE)
            {
                // printf("\n ERROR: FindNextChangeNotification function failed.\n");
                ExitProcess(GetLastError());
            }
            break;
        default:
            break;
        }
#endif // PLATFORM_WINDOWS

    
    } else{
    
        FOLDER_META _meta;
        HashFunction Hash_func;

        ent = readdir(MainDir);
        _meta.Path = Project_Path;
        _meta.GUID = Hash_func(_meta.Path);
        _meta.Name = ent->d_name;
        _meta.Type = 0;
        _meta.Size = 0;

        ExportMeta(Project_Path_Name + ".metaData", _meta);
    }


}


size_t  DirectoryWatcher::CreateGUID(const string& var)
{
    return hashFunc(var);
}


//Save Meta Data
void DirectoryWatcher::ExportMeta(const string& Path,FOLDER_META meta)
{
    ofstream Write_file((Path + ".metaData"));                // Path +.,metaDat -> Meshes(Folder).metaData
    boost::archive::text_oarchive oa(Write_file);

    oa & meta;


}


//-----------------
// Get the meta Data
//-----------------
void DirectoryWatcher::GetFolderMeta(const string& Path, FOLDER_META * meta)
{
    FOLDER_META _meta; string FileName = Path + ".metaData";
    ifstream Read_stream(FileName);
    boost::archive::text_iarchive ia(Read_stream);
    
    ia & _meta;

    *meta = _meta;
   
}


#endif
