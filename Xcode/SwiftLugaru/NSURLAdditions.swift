//
//  NSURLAdditions.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

extension NSURL {
	func URLByAppendingPathComponents(paths: [String]) -> NSURL {
		var newURL = self
		for path in paths {
			newURL = newURL.URLByAppendingPathComponent(path)
		}
		return newURL
	}
}
