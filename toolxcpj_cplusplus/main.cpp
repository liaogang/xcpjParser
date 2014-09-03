//
//  main.cpp
//  toolxcpj_cplusplus
//
//  Created by liaogang on 9/2/14.
//  Copyright (c) 2014 gang.liao. All rights reserved.
//

#include <iostream>
#include <map>
#include <vector>

using namespace std;

enum { xcUUIDLen = 24 };


static FILE *file = NULL;


char *sectionNames[]=
{
    "UnKnown",
    "PBXBuildFile",
    "PBXContainerItemProxy" ,
    "PBXFileReference" ,
    "PBXFrameworksBuildPhase" ,
    "PBXGroup" ,
    "PBXNativeTarget" ,
    "PBXProject" ,
    "PBXResourcesBuildPhase" ,
    "PBXSourcesBuildPhase" ,
    "PBXTargetDependency" ,
    "PBXVariantGroup" ,
    "XCBuildConfiguration" ,
    "XCConfigurationList"
};

enum PB_SECION
{
    UnKnown ,
    PBXBuildFile,
    PBXContainerItemProxy ,
    PBXFileReference ,
    PBXFrameworksBuildPhase ,
    PBXGroup ,
    PBXNativeTarget ,
    PBXProject ,
    PBXResourcesBuildPhase ,
    PBXSourcesBuildPhase ,
    PBXTargetDependency ,
    PBXVariantGroup ,
    XCBuildConfiguration ,
    XCConfigurationList,
    section_max = XCConfigurationList
};



static char bufLine[LINE_MAX];
fpos_t posLineBegin,posLineEnd;
char *readLine(FILE *f)
{
    char *buf1 = bufLine;
    char *pBuf = buf1;

#ifdef DEBUG
    memset(buf1, 0, LINE_MAX);
#endif
    
    
    size_t readed;
    
    
    fgetpos(f, &posLineBegin);
    posLineBegin++;
    
    while ( (readed = fread( pBuf , sizeof(char), 1 , f)))
    {
        if (pBuf[0] =='\n')
        {
            pBuf[0]='\0';
            break;
        }
        
        
        pBuf+=1;
    }
   
    
    if (readed == 0) {
        return nullptr;
    }
    
    return buf1;
}


/**
 * get a line of s
 * return the point of the new line.
 */
//char *getLine (char *s )
//{
//    char *p = strchr(s, '\n');
//    
//    return p + 1;
//}

/**
 * trim the right tab '\t'
 */
char * trimLine(char *s)
{
    int i;
    for (i=0; s[i] == '\t'; ++i);
    
    return s+i;
}




/**
 * buildFile
 * key: uuid
 * fileRef: uuid
 */
struct PB_BuildFile
{
    fpos_t posBegin;
    enum BuildFileType
    {
        Resources,
        Sources
    }type;
    char uuid [xcUUIDLen+1];
    char name [PATH_MAX];
    
    char uuidFileRef [xcUUIDLen +1];
    fpos_t posFileRef;
};


std::map<char*, PB_BuildFile*> mapBuildFile;
typedef pair<char*, PB_BuildFile*> pariBuidFile;
vector<char*> vecBuildFile;


void parseLine_BuildFile(char *s)
{
    PB_BuildFile *buildFile = new PB_BuildFile;
    
    buildFile->posBegin = posLineBegin;
    
    
    char *uuid = buildFile->uuid;
    char *name = buildFile->name;
    char *uuidFileRef = buildFile->uuidFileRef;
    
    /// find uuid
    strncpy(uuid, s,xcUUIDLen);
    uuid[xcUUIDLen] ='\0';
    printf("uuid: %s\n",uuid);
    
    
    
    /// find name
    char *pName = s + xcUUIDLen;
    for (;pName[0] == '/' || pName[0] == '*' || pName[0] == ' ';pName++){}
    
    char *pName2 = pName;
    for (; pName2[0] != ' ' && pName2[0] != '*';pName2++){}
    
    strncpy(name, pName, pName2 - pName);
    printf("name: %s\n",name);
    
    
    /// find fileRef uuid.
    const char fileRef[] ="fileRef";
    const size_t fileRefLen = sizeof(fileRef)/sizeof(fileRef[0]);
    char *p = strstr(s, fileRef);
    p += fileRefLen;
    
    for (;p[0] == '=' || p[0] == ' ';p++);
    
    strncpy(uuidFileRef, p, xcUUIDLen);
    uuidFileRef[xcUUIDLen]='\0';
    
    buildFile->posFileRef = p - bufLine;
    
    mapBuildFile.insert(make_pair(uuid, buildFile));
    //vecBuildFile.push_back(uuid);
}



map<char*,char*> mapFileRef;
void parseLine_FileRef(char *s)
{
    //32385E261966AB1600928312 /* cloud_ico.png */ = {isa = PBXFileReference; lastKnownFileType = image.png; path = cloud_ico.png; sourceTree = "<group>"; };
    
    char *name = (char*) malloc(PATH_MAX);
    char *uuid = (char*)malloc(xcUUIDLen+1);
    char *path = (char*) malloc(PATH_MAX);
    
    
    
    
    /// find uuid
    strncpy(uuid, s,xcUUIDLen);
    uuid[xcUUIDLen] ='\0';
    printf("uuid: %s\n",uuid);
    
    
    /// find name
    char *pName = s + xcUUIDLen;
    for (;pName[0] == '/' || pName[0] == '*' || pName[0] == ' ';pName++){}
    
    char *pName2 = pName;
    for (; pName2[0] != ' ' && pName2[0] != '*';pName2++){}
    
    strncpy(name, pName, pName2 - pName);
    printf("name: %s\n",name);
    
    
    
    
    /// find path
    const char fPath[]= "path";
    const size_t fPathLen = sizeof(fPath)/sizeof(fPath[0]) -1;
    
    char *p = strstr(s, fPath);
    if (p ) {
        p += fPathLen;
    }
    
    for (;p[0] == '=' || p[0] == ' ';p++){}
    
    char *p2 = p;
    for (;p2[0] != ';';p2++){}
    
    strncpy(path, p, p2 - p);
    
    printf("path: %s\n\n",path);
    
    
    
    mapFileRef.insert(pair<char*, char*>(name,uuid ));
}

void parseLine(char *s, PB_SECION c)
{
    
    if (c == PBXBuildFile)
    {
        parseLine_BuildFile(s);
    }
    else if (c == PBXContainerItemProxy )
    {
    }
    else if (c == PBXFileReference )
    {
        parseLine_FileRef(s);
    }
    else if (c == PBXFrameworksBuildPhase )
    {
        
    }
    else if (c == PBXGroup )
    {
        
    }
    else if (c == PBXNativeTarget )
    {
        
    }
    else if (c == PBXProject )
    {
        
    }
    else if (c == PBXResourcesBuildPhase )
    {
        
    }
    else if (c == PBXSourcesBuildPhase )
    {
    }
    else if (c == PBXTargetDependency )
    {
    }
    else if (c == PBXVariantGroup )
    {
    }
    else if (c == XCBuildConfiguration )
    {
    }
    else if (c == XCConfigurationList)
    {
    }
}


/**
 * @param section: output
 * @return the new line.
 */
PB_SECION parseSection(char *s )
{
    PB_SECION ret = UnKnown;
    
    if ( strncmp(s, "/* ", 3) != 0 )
        return ret;
    
    char strSearch[380];
    const char format[] = "/* Begin %s section */";
    
    for (int c = PBXBuildFile ; c <= section_max ; c++)
    {
        sprintf(strSearch, format , sectionNames[c] );
        
        char *p = strstr(s, strSearch);
        if (p)
        {
            ret = (PB_SECION) c;
            break;
        }
    }
    
    
    return ret;
}

bool isSecionEnd(char *s , PB_SECION currSection)
{
    if ( strncmp(s, "/* ", 3) != 0 )
       return false;
    
    char strSearch[380];
    const char format[] = "/* End %s section */";
    
   sprintf(strSearch, format , sectionNames[currSection] );
    
    
    if (strstr(s, strSearch))
    {
        currSection = UnKnown;
        return true;
    }
    
    return false;
}


int main(int argc, const char * argv[])
{
    
    char testProj[] =  "/Users/liaogang/toolxcpj/toolxcpj/test.pbxproj" ;
    

    file = fopen(testProj, "r");
    if (file)
    {

        PB_SECION currSection ;
       
        char * s ;

        /// read and parse the file
        while ( (s =  readLine(file)) )
        {
            s = trimLine(s);
            //printf("%s\n",s);
            
            currSection = parseSection(s);
            
            
            if (currSection != UnKnown)
            {
                printf("section: %s \n",sectionNames[currSection]);
                
               while(true)
                {
                    s = readLine(file);
                    
                    //printf("%s\n",s);
                    
                    if(isSecionEnd(s,currSection))
                        break;
                    else
                        parseLine(s, currSection);
                }
                
            }
        }
        
        
        /// change the file. update the buildFile's fileRefrence.
        auto e = mapBuildFile.end();
        for (auto i = mapBuildFile.begin(); i!= e; ++i)
        {
            pariBuidFile a = *i;
            PB_BuildFile *buildFile = a.second;
            if (buildFile)
            {
                mapFileRef[buildFile->name];
            }
            
        }
        
        
        fclose(file);
    }
    else
    {
        printf("can not open file: %s\n",testProj);
    }
    
    
    
    
    
    
    return 0;
}









