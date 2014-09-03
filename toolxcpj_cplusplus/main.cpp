//
//  main.cpp
//  toolxcpj_cplusplus
//
//  Created by liaogang on 9/2/14.
//  Copyright (c) 2014 gang.liao. All rights reserved.
//

#include <iostream>
#include <map>

using namespace std;

enum { xcUUIDLen = 24 };




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


char *readLine(FILE *f)
{
    enum {bufLen = 800  };
    static char buf1[bufLen];
    char *pBuf = buf1;

#ifdef DEBUG
    memset(buf1, 0, bufLen);
#endif
    
    
    size_t readed;
    
    
    /// trim '\t' in begin.
   while( (readed = fread( pBuf , sizeof(char), 1 , f)) )
   {
       if ( pBuf[0] != '\t')
       {
           if (pBuf[0] =='\n')
           {
               pBuf[0]='\0';
           
               return buf1;
           }
           
           pBuf+=1;
           break;
       }
   }
    
    if (readed == 0) {
        return nullptr;
    }

    while ( (readed = fread( pBuf , sizeof(char), 1 , f)))
    {
        if (pBuf[0] =='\n')
        {
            pBuf[0]='\0';
            break;
        }
        
        
        pBuf+=1;
    }
    
    
    return buf1;
}


/**
 * get a line of s
 * return the point of the new line.
 */
char *getLine (char *s )
{
    char *p = strchr(s, '\n');
    
    return p + 1;
}

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
std::map<char*, char*> mapBuildFile;
void parseLine_BuildFile(char *s)
{
    char *uuid = (char*)malloc(xcUUIDLen+1);
    char *uuidFileRef = (char*)malloc(xcUUIDLen +1);
    
    
    strncpy(uuid, s,xcUUIDLen);
    
    uuid[xcUUIDLen] ='\0';
    
    printf("uuid: %s\n",uuid);
    
    const char fileRef[] ="fileRef";
    const size_t fileRefLen = sizeof(fileRef)/sizeof(fileRef[0]);
    char *p = strstr(s, "fileRef");
    p += fileRefLen;
    
    for (;p[0] == '=' || p[0] == ' ';p++)
    {
    }
    
    
    strncpy(uuidFileRef, p, xcUUIDLen);
    uuidFileRef[xcUUIDLen]='\0';
    
    
    mapBuildFile.insert(std::pair<char*,char*>(uuid,uuidFileRef));
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
    

    FILE *file = fopen(testProj, "r");
    if (file)
    {
        
//        char buf[2000];
//        fread(buf, 1, 2000, file);
        
        
        
        PB_SECION currSection ;
       
        char * s ;

        
        while ( (s = readLine(file)) )
        {
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
                    
                    parseLine(s, currSection);
                }
                
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