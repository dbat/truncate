#include <conio.h>
#include <tchar.h>
#include "windows.h"
#include <stdio.h>
#include <time.h>

#define POWERSHIFT 24
const unsigned long BLOCK = 1 << POWERSHIFT;

int showhelp(_TCHAR* arg)
{
	_TCHAR *s, *p = arg;

	while(p) {
		s = ++p;
		p = _tcschr(p, '\\');
	}

	printf(" \n");
	printf(" Version: 0.2.1\n");
	printf(" Created: 2005.05.10\n");
	printf(" Revised: 2009.09.04\n\n");
	printf(" Compiled with Borland's BCC version 5.5.1 (Freeware)\n\n");
	printf(" SYNOPSYS:\n");
	printf(" \tTruncate, create, extend or shift file content as specified size.\n\n");
	printf(" USAGE:\n");
	printf(" \t%s filename size[k|m|g|t][b] [skip[k|m|g|t][b]]\n\n", s);
	printf(" ARGUMENTS:\n");
	printf(" \tThis program expects at least 2 arguments.\n\n");
	printf(" \tfilename: file to be processed, will be created if not exist.\n\n");
	printf(" \tsize: target size (intended size).\n\n");
	printf(" \t      If positive, means final size, regardless of skipsize value.\n\n");
	printf(" \t      If negative, means counted from the end of file, and\n");
	printf(" \t      excluded (minus) skipsize. Example, given targetsize= -10g\n");
	printf(" \t      if original size is 100GB, targetsize calculated to: 90GB.\n");
	printf(" \t      If also given skipsize= 3gb, the targetsize become: 87GB\n\n");
	printf(" \tskip (optional): skip/discard bytes from beginning (default = 0).\n");
	printf(" \t                 skipsize value must be positive.\n\n");
	printf(" \tsize and skip may be suffixed by K/M/G/T(B), case insensitive.\n\n");
	//printf(" \tTrailing B (bytes), if any, will be simply ignored.\n\n");
	printf(" \tImportant:\n");
	printf(" \tskipsize might slowed down operation significantly, especially\n");
	printf(" \tif misaligned with pagesize (by given odd skip value).\n");
	//printf(" \t* Do not use it if not really necessary *\n\n");
	//printf(" \n");
	printf(" \n");
	printf(" \tIn all cases, depends heavily on drive speed and memory cache,\n");
	printf(" \tthough this program is smart enough to avoid unnecessary move.\n");
	printf(" \t(our test: approx. 100 seconds to shift 16GB data up from pos:2)\n\n");
	//printf(" \n");
	//printf(" Buffer size: %u MB\n\n", BLOCK >> 20);
	//fprintf(stderr, " Press any key to continue..\n"); getch();
	//printf(" Press any key to continue..\n"); getch();
	//printf(" \n");
	printf(" EXAMPLES:\n\n");
	printf("   - Empty a file name \"zero\", or create it if does not exist yet\n\n");
	printf(" \t%s zero 0\n\n", s);
	printf("   - Create, truncate or extend \"foo\" size to 2 terrabytes. fast!\n\n");
	printf(" \t%s foo 2TB\n\n", s);
	printf("   - Crop 19KB from beginning and end of \"bar\"\n\n");
	printf(" \t%s bar -19K 19k\n\n", s);
	printf("   - Shift 1TB of \"baz\" content from pos: 777-th byte\n\n");
	printf(" \t%s baz 1tB 777\n\n", s);
	printf(" \tIf the orginal size less than 1TB it will be extended (0-fill)\n\n");
	printf(" ====================================================\n");
	printf(" Copyright (c) 2003-2009\n");
	printf(" Adrian H, Ray AF & Raisa NF of PT SOFTINDO, Jakarta.\n");
	printf(" Email: aa _AT_ softindo.net\n");
	printf(" All rights reserved.\n");
	printf(" ====================================================\n");
	printf(" Press any key to continue..\n"); getch();
	return 1;
}

int showerr(const _TCHAR* msg)
{
	_TCHAR * lem;
	int err = GetLastError();

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lem, 0, NULL);

	//printf("ERROR: %s.\nLast error code:%d\n%s", msg, err, lem);
	printf("ERROR[%d]: %s.\n", err, msg); // no need to show last error string
	if (err != 183 && err != 0) printf("%s\n", lem);
	LocalFree(lem);
	return 0;
}

int closerr(HANDLE file, const _TCHAR* msg)
{
	CloseHandle(file);
	return showerr(msg);
}

__int64 argnvalue(int c, _TCHAR* args[], int n)
{
	__int64 result = 0;
	if (c > n)
	{
		result = _atoi64(args[n]);
		if (result)
		{
			int len = _tcslen(args[n]);
			if (n > 1)
			{
				_TCHAR c = _totupper(args[n][len-1]);
				if ((c == 'B') && (len > 2))
					c = _totupper(args[n][len-1-1]);
				switch (c)
				{
					case _T('K'): result *=1024; break;
					case _T('M'): result *=(1024*1024); break;
					case _T('G'): result *=(1024*1024*1024); break;
					case _T('T'): result *=(__int64)1024*(1024*1024*1024); break;
				}
			}
		}
	}
	return result;
}

int setEOF(HANDLE file, __int64 size)
{
	LARGE_INTEGER fp;
	fp.QuadPart = size;
	if (!SetFilePointerEx(file, fp, NULL, 0)) return closerr(file, "Seeking file");
	if (!SetEndOfFile(file)) return closerr(file, "Setting EOF");
	return 1;
}

// char confirm(char * msg) { printf(msg); return gtch(); }
char confirm_zerosize(const __int64 targetsize) {
	char ch;
	printf("This will truncate the file ");
	if (targetsize)	printf("and zero fill to %I64d bytes. ", targetsize);
	else printf("to zero size. ");
	printf("Are you sure?\n");
	printf("Press [y] to confirm, any other key will cancel. ");
	ch = (char)getche();
	printf("\n\n\n");
	return ch;
}

int _tmain(int c, _TCHAR* args[])
{
	if (c < 3 || c > 4) return showhelp(args[0]);
	else
	{
		HANDLE file;
		LARGE_INTEGER qsize;
		__int64 getsize = argnvalue(c, args, 2);
		__int64 skipsize = argnvalue(c, args, 3);
		__int64 filesize;
		__int64 targetsize;// = getsize;

        int ms;
		clock_t tic, tac, toe;

		tic = clock();

		if (skipsize < 0) return showerr("Skip size must not be negative");
		//if (getsize < -1) return showerr("Invalid target size");
		//if (getsize == -1 && skipsize == 0) return showerr("Nothing to do");

		file = CreateFile(args[1], GENERIC_WRITE|GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			return showerr("Open failed, maybe read-only environment");

		GetFileSizeEx(file, &qsize);
		filesize = qsize.QuadPart;

		//if (skipsize && skipsize >= filesize) skipsize = filesize;
		//if (getsize < 0) targetsize = filesize + getsize;

		targetsize = getsize >= 0 ? getsize : filesize + getsize;
		if (targetsize < 0) targetsize = 0;

			//printf("filesize: %I64d, getsize: %I64d -> ", filesize, getsize);
			//printf("targetsize: %I64d, skipsize: %I64d\n\n", targetsize, skipsize);

		if (!(filesize | targetsize)) {
			CloseHandle(file);
			printf("Done processing zero size file: \"%s\"\n", args[1]);
			return 1;// closerr(file, "Both filesize and targetsize are zero");
		}

		if (skipsize == 0)
		{
			if (targetsize == filesize)
				return closerr(file, "Target filesize doesn't change");
			else if (!setEOF(file, targetsize))
				return closerr(file, "An error occured on 1st stage");
			CloseHandle(file);
		}
		else // here, skipsize is > 0
		if (skipsize >= targetsize || skipsize >= filesize) {
			// negative/back-count targetsize larger than filesize itself
			// skipsize larger than filesize itself
			if (filesize)
				if ((confirm_zerosize(targetsize) | 0x20) != (char)'y')
					return closerr(file, "Operation cancelled");
			tic = clock();
			setEOF(file, 0);
			if (targetsize) setEOF(file, targetsize);
			CloseHandle(file);
		}
		else {
			HANDLE source, dest;
			LARGE_INTEGER qz;
			_TCHAR tmpfile[256];
			_TCHAR * Buf;
			unsigned long got;

			__int64 endsize;
			//chunk power size (20=1M to 30=1G)

			CloseHandle(file);

			dest = CreateFile(args[1], GENERIC_WRITE,
				FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (dest == INVALID_HANDLE_VALUE)
				return showerr("Open write failed, is file missing?");
			qz.QuadPart = 0;
			SetFilePointerEx(dest, qz, NULL, 0);

			source = CreateFile(args[1], GENERIC_READ,
				FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (source == INVALID_HANDLE_VALUE)
				return closerr(dest, "Open read failed, is file missing?");

			qz.QuadPart = skipsize;
			SetFilePointerEx(source, qz, NULL, 0);

			Buf = malloc(BLOCK);
			if (!Buf) {
				CloseHandle(dest); CloseHandle(source);
				return showerr("Not enough memory");
			}

			if (getsize < 0) targetsize -= skipsize;
			printf("Shifting data: %I64d by %I64d bytes (buffer: %dMB).", targetsize, skipsize, (BLOCK >> 20));
			printf(" Please wait..\n");

			ReadFile(source, Buf, BLOCK, &got, NULL);

			endsize = 1 + (targetsize -1) >> POWERSHIFT; 
			while (got && endsize--)
			{
				WriteFile(dest, Buf, got, &got, NULL);
				//if ((int)endsize & 3)
				printf(".");
				if (!got) return showerr("Writing file interrupted");
				ReadFile(source, Buf, BLOCK, &got, NULL);
			}
			printf("\n");

			free(Buf);
			CloseHandle(source);

			if (!SetEndOfFile(dest)) return closerr(dest, "Truncate");
			setEOF(dest, targetsize);
			CloseHandle(dest);
		}
		tac = clock() - tic;
		ms = tac * 1000 / CLOCKS_PER_SEC;
		printf("Done. New filesize: %I64d bytes.\n", targetsize);
		printf("timer: %d.%d seconds\n", ms/1000, ms%1000);
	}
	return 0;
}
