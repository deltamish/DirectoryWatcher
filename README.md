## DirectoryWatcher V 1.0

# Installation 
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
 ## Running 
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
 
 # The structs 
 
struct Changed_Data
{
    FOLDER_META Old;
    FOLDER_META New;
};

**Folder Meta **
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
 
 
 
You can use the [editor on GitHub](https://github.com/deltamish/DirectoryWatcher/edit/master/README.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for


Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `include>?` text

[Link](url) and ![Image](src)


For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/deltamish/DirectoryWatcher/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
