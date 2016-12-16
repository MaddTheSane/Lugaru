//
//  NSURLAdditions.swift
//  Lugaru
//
//  Created by C.W. Betts on 4/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

import Foundation

extension URL {
	func URLByAppendingPathComponents(_ paths: [String]) -> URL {
		var newURL = self
		for path in paths {
			newURL = newURL.appendingPathComponent(path)
		}
		return newURL
	}
}
