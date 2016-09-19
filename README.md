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

        size: target size (intended size).
              If positive, means final size, regardless of skipsize value.
              
              If negative, means counted from the end of file, and
              excluded (minus) skipsize. Example, given targetsize= -10g
              if original size is 100GB, targetsize calculated to: 90GB.
              This special value should be followed by skip value.
              If also given skipsize= 3gb, the targetsize become: 87GB.

        skip (optional): skip/discard bytes from beginning (default = 0).
                skip value must be positive

        size and skip may be suffixed by K/M/G/T(B), case insensitive.
        

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

   - Crop 19KB from beginning and end of "bar"

        truncate.exe bar -19K 19k

   - Shift 1TB of "baz" content from pos: 777-th byte

        truncate.exe baz 1tB 777

        If the orginal size less than 1TB it will be extended (0-fill)

==
