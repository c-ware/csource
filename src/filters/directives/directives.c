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
 * This file deals with the removal / filtering of directives from a source
 * file. In order to properly strip macros, for example, we must also support
 * the preprocessor's backslash feature to continue onto the next line. This
 * can easily be done recursively, of course, though.
 *
 * Pretty much the only method we take to 'detect' preprocessor stuff is by 
 * checking if the first PRINTABLE character on a line is a hashtag (also
 * called pound) sign. If it is, all characters after that character including
 * the character itself is stripped. If a \ is found at the end of a line, then
 * the process continues onto the next line until the line AFTER a line without
 * a backslash at the end of it.
 *
 * We literally detect whether or not its the first printable character by
 * using libmatch_cond_before with a string of every printable without the
 * pound sign. High quality code for sure.
*/

#include "../../csource.h"

#include "directives.h"

/*
 * @docgen: function
 * @brief: determine if the cursor's line has a directive on it
 * @name: is_directive
 *
 * @description
 * @This function will determine whether or not the line that the cursor
 * @is on contains a directive. This function assumes that the cursor is
 * @currently on the START of the line.
 * @description
 *
 * @notes
 * @This function does not advance the cursor of the caller.
 * @notes
 *
 * @param cursor: the cursor to checK
 * @type: struct LibmatchCursor
 *
 * @return: returns 0 if the cursor is on a directive, and 1 if it is not
 * @type: int
*/
static int is_directive(struct LibmatchCursor cursor) {
    /* First printable character on this line must be a # */
    if(libmatch_cond_before(&cursor, '#', PRINTABLES_WITHOUT_POUND) == 0)
        return 0;

    return 1;
}

/*
 * @docgen: function
 * @brief: print the contents of a line until a character
 * @name: print_line_until
 *
 * @description
 * @This function will a line until a character sign is met, and when it
 * @is met, filter the rest of the line out. Either way, no matter what,
 * @it will always stop at the end of the line.
 * @description
 *
 * @error: cursor is NULL
 * @error: character is less than 0
 *
 * @param stop_at: the character to print until
 * @type: int
 *
 * @param cursor: the cursor to print from
 * @type: struct LibmatchCursor *
*/
static void print_line_until(struct LibmatchCursor *cursor, int stop_at) {
    int character = -1;

    liberror_is_null(print_line_until, cursor);
    liberror_is_negative(print_line_until, stop_at);

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(character == stop_at) {
            libmatch_next_line(cursor);

            break;
        }

        if(character == '\n')
            break;
        
        printf("%c", character);
    }
}

void csource_filter_directives(struct ModuleSetup setup) {
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(setup.file);

    while(cursor.cursor < cursor.length) {
        int cancel_next_line = 0;
        struct LibmatchCursor sub_cursor;
        char *line = libmatch_read_alloc_until(&cursor, "\n");

        sub_cursor = libmatch_cursor_init(line, strlen(line) + 1);

        if(is_directive(sub_cursor) == 0) {
            printf("%s\n", line);

            free(line);
            
            continue;
        }

        /* Keep reading lines until line has no more \ on it */
        while(cursor.cursor < cursor.length) {
            if(line[strlen(line) - 1] != '\\') {
                free(line);

                break;
            }
            
            free(line);

            line = libmatch_read_alloc_until(&cursor, "\n");
        }
    }
    

    libmatch_cursor_free(&cursor);
}
