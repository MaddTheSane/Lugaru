//
//  MacCompatibility.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation


//char* ConvertFileName( const char* orgfilename, const char *mode = "rb" );

private func getPrefPath() -> URL {
	var appSupURL = try! FileManager.default.url(for: .applicationSupportDirectory, in: .userDomainMask, appropriateFor: nil, create: false)
	appSupURL = appSupURL.appendingPathComponent("Lugaru", isDirectory: true)
	
	return appSupURL
}

func ConvertFileName(_ orgfilename: String, mode: String = "rb") -> URL {
	return URL(fileURLWithPath: orgfilename)
}
