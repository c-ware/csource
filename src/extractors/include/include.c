/*
 * C-Ware License
 * 
 * Copyright (c) 2022, C-Ware
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Redistributions of modified source code must append a copyright notice in
 *    the form of 'Copyright <YEAR> <NAME>' to each modified source file's
 *    copyright notice, and the standalone license file if one exists.
 * 
 * A "redistribution" can be constituted as any version of the source code
 * that is intended to comprise some other derivative work of this code. A
 * fork created for the purpose of contributing to any version of the source
 * does not constitute a truly "derivative work" and does not require listing.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * This is the component of csource which will extract all of the direct
 * local, and system inclusions in a source file.
*/

#include "include.h"
#include "../../csource.h"

/*
 * @docgen: function
 * @brief: determine if the cursor is on an inclusion
 * @name: is_inclusion
 *
 * @description
 * @Determine whether or not the cursor, which should be at the start
 * @of a line, is placed on a line that is an inclusion. It should match
 * @the pattern:
 * @
 * @\s*#\s*include\s*(<.+>|".+")\n
 * @description
 *
 * @notes
 * @This function does not advance the cursor.
 * @notes
 *
 * @param cursor: the cursor to use
 * @type: struct LibmatchCursor
 *
 * @return: 0 if its not an inclusion, 1 if it is
 * @type: int
*/
static int is_inclusion(struct LibmatchCursor cursor) {
    int character = -1;

    /* If there is no # on this line, well.. this is not a
     * directive of any kind. */
    if(libmatch_cond_before(&cursor, '#', "\n") == 0)
        return 0;

    /* Make sure we do not match something like
     * '^sdSFS#include <stdio.h>', since we will be reading spaces or
     * tabs until a '#' is found */
    while((character = libmatch_cursor_getch(&cursor)) != '#') {
        /* Character is an expected space or tab */
        if(strchr(" \t", character) != NULL) {
            continue;
        }

        /* Found a character between the start of the line and the #
         * which is not a tab or space */
        return 0;
    }

    /* OK, so we have found the '#', which indicates a compiler directive.
     * So far, we got the '\s*#' part matched so far. Now, there can also
     * be tabs or spaces between the '#' and the directive. */

    /* If there is a newline before any alphabetical character is found,
     * then this line could look something like '      #     \n', which
     * is certainly not an inclusion. */
    if(libmatch_cond_before(&cursor, '\n', LIBMATCH_ALPHA) == 1)
        return 0;

    /* Make sure we do not match something like
     * '#23aRb3$6include <stdio.h>', since we will be reading spaces
     * or tabs until an alphabetical character is found */
    while(strchr(LIBMATCH_ALPHA, (character = libmatch_cursor_getch(&cursor))) == NULL) {
        /* Character is an expected space or tab */
        if(strchr(" \t", character) != NULL)
            continue;

        /* Found a character between the start of the line and an alphabetical
         * character which is not a tab or space */
        return 0;
    }

    /* So now we have gotten to the second letter of the directive name.
     * Let's go back one character so we can read the whole directive, and
     * compare it against what it should be. */
    libmatch_cursor_ungetch(&cursor);

    if(libmatch_string_expect(&cursor, "include") == 0)
        return 0;

    /* Make sure we do not match something like
     * '#include$#sdsadsa<stdio.h>', since we will be reading spaces
     * or tabs until a printable character is found */
    while(strchr(LIBMATCH_PRINTABLE, (character = libmatch_cursor_getch(&cursor))) == NULL) {
        /* Character is an expected space or tab */
        if(strchr(" \t", character) != NULL)
            continue;

        /* Found a character between the start of the line and a
         * printable character */
        return 0;
    }

    /* First printable character must be a < or " */
    if(character != '<' && character != '"')
        return 0;

    /* If there is no > or " before the new line, then it is a heavily
     * malformed inclusion. Looks like '#include <foo/bar\n> */
    if(libmatch_cond_before(&cursor, '>', "\n") == 0)
        return 0;

    if(character == '<')
        libmatch_until(&cursor,  ">");
    else if(character == '"')
        libmatch_until(&cursor,  "\"");

    /* Finally, after the closing > or ", there should be a new line
     * or an EOF */
    character = libmatch_cursor_getch(&cursor);

    if(character != '\n' && character != LIBMATCH_EOF)
        return 0;

    return 1;
}

struct CSourceInclusions *csource_extract_inclusions(struct ExtractorSetup setup) {
    FILE *source_file = fopen(setup.source, "r");
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(source_file);
    struct CSourceInclusions *inclusions = carray_init(inclusions, INCLUSION);

    is_inclusion(cursor);

    fclose(source_file);
    libmatch_cursor_free(&cursor);

    return inclusions;
}
