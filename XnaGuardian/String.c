/*
MIT License

Copyright (c) 2016 Benjamin "Nefarius" Höglinger

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <crtdefs.h>
#include <stddef.h>
#include "String.h"

#pragma warning(push)
#pragma warning( disable : 4706 )

//
// Stolen from here: https://github.com/lattera/glibc/blob/master/wcsmbs/wcsstr.c
// 
wchar_t *wcsstr(const wchar_t *haystack, const wchar_t *needle)
{
    register wchar_t b, c;

    if ((b = *needle) != L'\0')
    {
        haystack--;				/* possible ANSI violation */
        do
            if ((c = *++haystack) == L'\0')
                goto ret0;
        while (c != b);

        if (!(c = *++needle))
            goto foundneedle;
        ++needle;
        goto jin;

        for (;;)
        {
            register wchar_t a;
            register const wchar_t *rhaystack, *rneedle;

            do
            {
                if (!(a = *++haystack))
                    goto ret0;
                if (a == b)
                    break;
                if ((a = *++haystack) == L'\0')
                    goto ret0;
            shloop:;
            } while (a != b);

        jin:	  if (!(a = *++haystack))
            goto ret0;

                  if (a != c)
                      goto shloop;

                  if (*(rhaystack = haystack-- + 1) == (a = *(rneedle = needle)))
                      do
                      {
                          if (a == L'\0')
                              goto foundneedle;
                          if (*++rhaystack != (a = *++needle))
                              break;
                          if (a == L'\0')
                              goto foundneedle;
                      } while (*++rhaystack == (a = *++needle));

                      needle = rneedle;		  /* took the register-poor approach */

                      if (a == L'\0')
                          break;
        }
    }
foundneedle:
    return (wchar_t*)haystack;
ret0:
    return NULL;
}
#pragma warning(pop)
