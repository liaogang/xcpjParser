//
//  main.swift
//  toolxcpj
//
//  Created by liaogang on 9/2/14.
//  Copyright (c) 2014 gang.liao. All rights reserved.
//

import Foundation

/**
 * get a line of a string.
 */
func getLine (var s:String) ->String
{
    let r :Range<String.Index> = s.rangeOfString("\n")!
    
    var ret = s.substringToIndex(r.startIndex)
    
    s = s.substringFromIndex(r.endIndex)
 
    /// del '\t' in string's begin.
    var r2 : Range<String.Index>
    while(true)
    {
        r2 = ret.rangeOfString("\t", options: NSStringCompareOptions.CaseInsensitiveSearch  , range: Range<String.Index>(start: ret.startIndex,end: ret.endIndex), locale: NSLocale())!
        
//        r2 = ret.rangeOfString ("\t")!
        
        
        
        if r2.startIndex != ret.startIndex
        {
            break
        }
        else
        {
            ret = ret.substringFromIndex(r2.endIndex)
        }
    }
    
    
    return ret
}


let testProj : String = "/Users/liaogang/toolxcpj/toolxcpj/test.pbxproj"


var errorPtr : NSErrorPointer = nil

var strFile : String = NSString.stringWithContentsOfFile(testProj, encoding: NSUTF8StringEncoding, error: errorPtr)


//print(strFile)


let strBuildFileBeg = "/* Begin PBXBuildFile section */\n"
let strBuildFileEnd = "/* End PBXBuildFile section */\n"

let rangeB : Range<String.Index> = strFile.rangeOfString(strBuildFileBeg)!
let rangeE : Range<String.Index>= strFile.rangeOfString(strBuildFileEnd)!

var rangeBuildFile = Range(start: rangeB.endIndex,end: rangeE.startIndex)
var strBuildFile = strFile.substringWithRange( rangeBuildFile )


var dicBuild : NSMutableDictionary = NSMutableDictionary.dictionary()


var strLine : String
do
{
    strLine = getLine(strBuildFile)
    print(strLine)
    
}while(!strLine.isEmpty)







