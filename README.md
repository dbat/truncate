# truncate
Truncate, create, extend or shift file content as specified size

    Copyright (c) 2003-2009
    Adrian H, Ray AF & Raisa NF of PT SOFTINDO, Jakarta.
    Email: aa _AT_ softindo.net
    All rights reserved.

      Version: 0.2.0
      Created: 2005.05.10
      Revised: 2009.07.11
      
    Compiled with Borland's BCC version 5.5.1 (Freeware)

 SYNOPSYS:
 
        Truncate, create, extend or shift file content as specified size.

 USAGE:
 
        truncate.exe filename size[k|m|g|t][b] [skip[k|m|g|t][b]]

 ARGUMENTS:
 
         This program expects at least 2 arguments.
        
        filename: file to be processed, will be created if not exist.

        size: target size (except for special value: -1).
              size value -1 means up to end of file, excl. skipsize if any
              (eg. if originalsize=1000GB and skip=100GB, -1 means 900GB).
              This special value should be followed by skip value.

        skip (optional): skip/discard bytes from beginning (default = 0).
              skip value must be positive, negative value counted as 0.

        size and skip may be suffixed by K/M/G/T(B), case insensitive.
        Trailing B (bytes), if any, will be simply ignored.

        Important:
        skipsize might slowed down operation significantly, especially
        if misaligned with pagesize (by given odd skip value).

        In all cases, depends heavily on drive speed and memory cache,
        though this program is smart enough to avoid unnecessary move.
        (our test: approx. 100 seconds to shift 16GB data up from pos:2)

 EXAMPLES:

   - Empty a file name "zero", or create it if does not exist yet

        truncate.exe zero 0

   - Create, truncate or extend "foo" size to 2 terrabytes. fast!

        truncate.exe foo 2TB

   - Delete 19KB from beginning of "bar" (shifted up, slow)

        truncate.exe bar -1 19k

   - Crop 1TB of "baz" content from pos: 777th byte (dog slow)

        truncate.exe baz 1tB 777

        If the orginal size less than 1TB it will be extended (0-fill)

==
