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
 * This file contains logic for extracting functions from a C source file.
*/

#include <string.h>

#include "../../csource.h"

static void ignore_string(struct LibmatchCursor *cursor) {

}

void csource_extract_functions(struct ExtractorSetup setup) {
    int depth = 0;
    FILE *source_file = setup.file;
    struct LibmatchCursor sub_cursor;
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(source_file);

    INIT_VARIABLE(sub_cursor);

    /* Read multiple statements, or scope starters until the file
     * is exhausted. */
    while(cursor.cursor < cursor.length) {
        char *line = NULL;
        int character = libmatch_cursor_getch(&cursor);

        if(character == '"') {
            ignore_string(&cursor);

            continue;
        }

        if(character == '{') {
            depth++;

            continue;
        }

        if(character == '}') {
            depth--;

            continue;
        }

        /* We only care about lines in the ""global"" scope (for lack of a
         * better word) */
        if(depth != 0)
            continue;

        /* Read until a '{' or ';', and then go back a character
         * so we can examine whether or not its a declaration or body, so
         * we know whether or not to increase scope */
        libmatch_cursor_enable_pushback(&cursor);
        libmatch_until(&cursor, LIBMATCH_ALPHA "_");

        line = libmatch_read_alloc_until(&cursor, ";{");
        character = libmatch_cursor_getch(&cursor);
        sub_cursor = libmatch_cursor_init(line, strlen(line));

        /* If the line has an equals sign (this is a possibility, because
         * 'static int x = (1 + (2 + 3));' will still be matched by this
         * algorithm so far), then we dispose of this line. */
        if(strchr(line, '=') != NULL)
            continue;

        /* Next, let's rip other non-function declarations out. */
        if(strchr(line, '(') == NULL)
            continue;

        /* By this point, typedefs can also get caught in our net, so
         * we must be rid of them. */
        if(libmatch_string_expect(&sub_cursor, "typedef") == 1)
            continue;

        /* Get rid of arrays declared in the global scope, like:
         * int x[123 + (53 + 1)]; */
        if(strchr(line, '[') != NULL && libmatch_cond_before(&sub_cursor, '(', "[") == 0)
            continue;

        /* We only need to increase the depth if we are about to enter a
         * function body next. We still may consider this a */
        if(character == '{')
            depth++;

        printf("Line: %i\n", cursor.line + 1);
        printf("Line: '%s'\n", line);
    }


    fclose(source_file);
    libmatch_cursor_free(&cursor);
}











