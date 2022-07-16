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
 * This file deals with filtering out comments from C source code. It
 * should filter out both multi-line ANSI comments, as well as C++
 * style single line comments.
*/

#include "../../csource.h"

#include "comments.h"

/*
 * @docgen: function
 * @brief: determine if the cursor is on a multi line comment
 * @name: is_multiline
 *
 * @description
 * @This function will determine if the cursor is positioned on
 * @the start delimiter of a multi line comment. It exists so the
 * @root cursor object is not advanced by the expect function. The
 * @root cursor object in this context should have full control
 * @over the position of the cursor.
 * @description
 *
 * @notes
 * @This function does not advance the cursor of the caller.
 * @notes
 *
 * @param cursor: the cursor to check
 * @type: struct LibmatchCursor
 *
 * @return: 1 if it is on a multiline comment, 0 if it is not
 * @type: int
*/
static int is_multiline(struct LibmatchCursor cursor) {
    if(libmatch_string_expect(&cursor, "/*") == 0)
        return 0;

    return 1;
}

/*
 * @docgen: function
 * @brief: determine if the cursor is on a multi line comment
 * @name: is_multiline
 *
 * @description
 * @This function will determine if the cursor is positioned on
 * @the end delimiter of a multi line comment. It exists so the
 * @root cursor object is not advanced by the expect function. The
 * @root cursor object in this context should have full control
 * @over the position of the cursor.
 * @description
 *
 * @notes
 * @This function does not advance the cursor of the caller.
 * @notes
 *
 * @param cursor: the cursor to check
 * @type: struct LibmatchCursor
 *
 * @return: 1 if it is on the end of a multiline comment, 0 if it is not
 * @type: int
*/
static int is_multiline_end(struct LibmatchCursor cursor) {
    if(libmatch_string_expect(&cursor, "*/") == 0)
        return 0;

    return 1;
}

/*
 * @docgen: function
 * @brief: determine if the cursor is on a single line comment
 * @name: is_singleline
 *
 * @description
 * @This function will determine if the cursor is positioned on
 * @the start of a single line comment. This exists so the root
 * @cursor object is not advanced by the expect function. The
 * @root cursor object in this context should have full control
 * @over the position of the cursor.
 * @description
 *
 * @notes
 * @This function does not advance the cursor of the caller.
 * @notes
 *
 * @param cursor: the cursor to check
 * @type: struct LibmatchCursor
 *
 * @return: 1 if it is on a single line comment, 0 if it is not
 * @type: int
*/
static int is_singleline(struct LibmatchCursor cursor) {
    if(libmatch_string_expect(&cursor, "//") == 0)
        return 0;

    return 1;
}

/*
 * @docgen: function
 * @brief: move the cursor until after the end of a multi line comment
 * @name: filter_multiline
 *
 * @description
 * @This function will displace the cursor given to it until it is no
 * @longer inside of the bounds of a single line comment.
 * @description
 *
 * @notes
 * @This function should be called when the cursor is on the first /
 * @character of the multiline comment. It will end with the cursor being
 * @placed at the first character after the / of the end delimiter.
 * @notes
 *
 * @error: cursor is NULL
 *
 * @param cursor: the cursot to displace
 * @type: struct LibmatchCursor *
*/
static void filter_multiline(struct LibmatchCursor *cursor) {
    liberror_is_null(filter_singleline, cursor);
    
    /* Keep traversing forward until a * / is found */
    while(is_multiline_end(*cursor) == 0)
        libmatch_cursor_getch(cursor);

    /* We only stop when we are ON the end delimiter--
     * we get rid of the end delimiter here. */
    libmatch_cursor_getch(cursor);
    libmatch_cursor_getch(cursor);
}

/*
 * @docgen: function
 * @brief: move the cursor until after the end of a single line comment
 * @name: filter_singleline
 *
 * @description
 * @This function will displace the cursor given to it until it is no
 * @longer on a single line comment. This function will respect backslashes
 * @at the end of a single line comment, as well, and will exhaust them up
 * @until and including the line after the last backslash.
 * @description
 *
 * @notes
 * @This function should be called when the cursor is on the first /
 * @character of the single line comment.
 * @notes
 *
 * @error: cursor is NULL
 *
 * @param cursor: the cursot to displace
 * @type: struct LibmatchCursor *
*/
static void filter_singleline(struct LibmatchCursor *cursor) {
    liberror_is_null(filter_singleline, cursor);

    /* Go to the end of the line for the comment. If a \ is found,
     * then that means we need to check the new line. Recursion is
     * nice here. Should be on the next line incase some idiot has
     * an extra space after their backslash or whatever, before
     * recursion starts. */
    while(cursor->cursor < cursor->length) {
        if(cursor->buffer[cursor->cursor] == '\\') {
            libmatch_next_line(cursor);
            
            filter_singleline(cursor);

            return;
        }

        /* Single line comments stop at new lines */
        if(cursor->buffer[cursor->cursor] != '\n') {
            libmatch_cursor_getch(cursor);

            continue;
        }

        break;
    }
}

/*
 * @docgen: function
 * @brief: move the cursor immediate after the end of a string
 * @name: filter_string
 *
 * @description
 * @This function will displace the cursor to the end of a string, which
 * @prevents a / and * or // inside of a string from causing it to consider
 * @that an actual comment. Since the strings themselves are not meant to be
 * @ignored, the characters are still printed inside this function.
 * @description
 *
 * @notes
 * @This function should be called with the cursor on the opening quote of
 * @string. When the function returns, we are left on the character after
 * @the closing quote.
 * @notes
 *
 * @param cursor: the cursor to displace
 * @type: struct LibmatchCursor *
*/
static void filter_string(struct LibmatchCursor *cursor) {
    int character = -1;
    int escaped = 0;
 
    printf("%c", '"');
    libmatch_cursor_getch(cursor);

    while((character = libmatch_cursor_getch(cursor)) != LIBMATCH_EOF) {
        if(character == '\\') {
            escaped = 1;

            printf("%c", '\\');

            continue;
        }

        if(character == '"' && escaped == 0) {
            printf("%c", '"');

            break;
        }

        printf("%c", character);

        escaped = 0;
    }
}

void csource_filter_comments(struct ModuleSetup setup) {
    struct LibmatchCursor cursor = libmatch_cursor_from_stream(setup.file);

    /* Enable pushback so we do not end up ignoring characters
     * because they failed a match. a / and * will mean a comment,
     * but a / then a / then a * will succeed on the first slash,
     * get the next character, see its not a *, then fail. This
     * is a problem because without pushback, the second /, which
     * was the actual comment start, is ignored. */
    libmatch_cursor_enable_pushback(&cursor);

    /* Check each character for a comment */
    while(cursor.cursor < cursor.length) { 
        if(is_multiline(cursor) == 1) {
            filter_multiline(&cursor);

            continue;
        } else if(is_singleline(cursor) == 1) {
            filter_singleline(&cursor);

            continue;
        } else if(cursor.buffer[cursor.cursor] == '"') {
            filter_string(&cursor);

            continue;
        }

        printf("%c", cursor.buffer[cursor.cursor]);
        libmatch_cursor_getch(&cursor);
    }

    libmatch_cursor_free(&cursor);
}
