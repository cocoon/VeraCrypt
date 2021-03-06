/*
 Legal Notice: Some portions of the source code contained in this file were
 derived from the source code of TrueCrypt 7.1a, which is 
 Copyright (c) 2003-2012 TrueCrypt Developers Association and which is 
 governed by the TrueCrypt License 3.0, also from the source code of
 Encryption for the Masses 2.02a, which is Copyright (c) 1998-2000 Paul Le Roux
 and which is governed by the 'License Agreement for Encryption for the Masses' 
 Modifications and additions to the original source code (contained in this file) 
 and all other portions of this file are Copyright (c) 2013-2015 IDRIX
 and are governed by the Apache License 2.0 the full text of which is
 contained in the file License.txt included in VeraCrypt binary and source
 code distribution packages. */

#include "Tcdefs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <Strsafe.h>

#include "Dir.h"

/* create full directory tree. returns 0 for success, -1 if failure */
int
mkfulldir (char *oriPath, BOOL bCheckonly)
{
	struct _stat st;
	char *uniq_file;
	char path [TC_MAX_PATH];

	StringCbCopyA (path, TC_MAX_PATH, oriPath);

	if (strlen (path) == 3 && path[1] == ':')
		goto is_root;	/* keep final slash in root if present */

	/* strip final forward or backslash if we have one! */
	uniq_file = strrchr (path, '\\');
	if (uniq_file && uniq_file[1] == '\0')
		uniq_file[0] = '\0';
	else
	{
		uniq_file = strrchr (path, '/');
		if (uniq_file && uniq_file[1] == '\0')
			uniq_file[0] = '\0';
	}

      is_root:
	if (bCheckonly)
		return _stat (path, &st);

	if (_stat (path, &st))
		return mkfulldir_internal (path);
	else
		return 0;
}


int
mkfulldir_internal (char *path)
{
	char *token;
	struct _stat st;
	static char tokpath[_MAX_PATH];
	static char trail[_MAX_PATH];

	StringCbCopyA (tokpath, _MAX_PATH, path);
	trail[0] = '\0';

	token = strtok (tokpath, "\\/");

	if (tokpath[0] == '\\' && tokpath[1] == '\\')
	{			/* unc */
		trail[0] = tokpath[0];
		trail[1] = tokpath[1];
		trail[2] = '\0';
		if (token)
		{
			StringCbCatA (trail, _MAX_PATH, token);
			StringCbCatA (trail, _MAX_PATH, "\\");
			token = strtok (NULL, "\\/");
			if (token)
			{		/* get share name */
				StringCbCatA (trail, _MAX_PATH, token);
				StringCbCatA (trail, _MAX_PATH, "\\");
			}
			token = strtok (NULL, "\\/");
		}
	}

	if (tokpath[1] == ':')
	{			/* drive letter */
		StringCbCatA (trail, _MAX_PATH, tokpath);
		StringCbCatA (trail, _MAX_PATH, "\\");
		token = strtok (NULL, "\\/");
	}

	while (token != NULL)
	{
		int x;
		StringCbCatA (trail, _MAX_PATH, token);
		x = _mkdir (trail);
		StringCbCatA (trail, _MAX_PATH, "\\");
		token = strtok (NULL, "\\/");
	}

	return _stat (path, &st);
}
