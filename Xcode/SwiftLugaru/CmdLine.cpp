//
//  CmdLine.cpp
//  Lugaru
//
//  Created by C.W. Betts on 4/21/16.
//  Copyright © 2016 Wolfire. All rights reserved.
//

#include <stdio.h>
#include <strings.h>

bool cmdline(const char *cmd)
{
	extern int NXArgc;
	extern char ** NXArgv;
	for (int i = 1; i < NXArgc; i++) {
		char *arg = NXArgv[i];
		while (*arg == '-')
			arg++;
		if (strcasecmp(arg, cmd) == 0)
			return true;
	}
	
	return false;
}

