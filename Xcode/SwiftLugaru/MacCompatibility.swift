//
//  MacCompatibility.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation


//char* ConvertFileName( const char* orgfilename, const char *mode = "rb" );

private func getPrefPath() -> NSURL {
	var appSupURL = try! NSFileManager.defaultManager().URLForDirectory(.ApplicationSupportDirectory, inDomain: .UserDomainMask, appropriateForURL: nil, create: false)
	appSupURL = appSupURL.URLByAppendingPathComponent("Lugaru", isDirectory: true)
	
	return appSupURL
}

func ConvertFileName(orgfilename: UnsafePointer<Int8>, mode: String = "rb") -> [Int8] {
	return Array(UnsafeBufferPointer(start: orgfilename, count: Int(strlen(orgfilename) + 1)))
}
