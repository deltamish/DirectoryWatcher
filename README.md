## DirectoryWatcher V 1.0

# Installation 
# Preq
 - Install Boost Library 
 - Install Dirent
 # Steps 
- Complete Above steps first  
- Clone the Repo in a new Folder 
- Create a new Exlipse/Visual Studio Project Select this as Source and Mark Project as DLL or Shared Library

## How to Use in your application ? 

```markdown

 #include<DirectoryWatcher.hpp> 
 
 int main()
 {
 DirectoryWatcher * watcher = new DirectoryWatcher();
 wathcer->Init("C://Program Files//TestFolder");
 return 0;
 }
 
 ```
 ### Running 
 ```code
// preferabbly to be used inside a Tick method  that calls after ervery 0.2 seconds 
 while(true)
 {
 vector<Changed_Data>* ChangedFiles;
 bool hasChanged = false;
   watcher->Monitor(ChangedFiles,hasChanged);
   
   if(hasChanged)
   {
     // Do something with ChanngedFiles
   }
 }
 
 ```
 
  ### The structs 
 ```code
struct Changed_Data
{
    FOLDER_META Old;
    FOLDER_META New;
};
```
### Folder Meta
```
typedef Meta<string> FOLDER_META;


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

 ```
 
 
Eager to see what you guys will do with the Project 

