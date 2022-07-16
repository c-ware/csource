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

static int x = (1 + (2 + 3));

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "csource.h"
#include "extractors/include/include.h"
#include "extractors/functions/functions.h"

struct ArgparseParser setup_arguments(int argc, char **argv) {
    struct ArgparseParser parser = argparse_init("csource", argc, argv);

    /* Setup arguments */
    argparse_add_argument(&parser, "source");
    argparse_add_argument(&parser, "target");

    /* Options */
    argparse_add_option(&parser, "--help", "-h", 0);

    /* Display a help message */
    if(argparse_option_exists(parser, "--help") != 0 || argparse_option_exists(parser, "-h") != 0) {
        fprintf(HELP_MESSAGE_STREAM, "%s", HELP_MESSAGE);
        exit(EXIT_FAILURE);
    }

    argparse_error(parser);

    return parser;
}

int main(int argc, char **argv) {
    struct ArgparseParser parser = setup_arguments(argc, argv);
    struct ExtractorSetup setup;

    INIT_VARIABLE(setup);

    setup.source = argparse_get_argument(parser, "source");
    setup.target = argparse_get_argument(parser, "target");
    setup.file = NULL;


    /* The source file must exist before we go any further. Do not
     * attempt to find a file named '-', since that means stdin. */
    if(strcmp(setup.source, "-") != 0 && libpath_exists(setup.source) == 0) {
        fprintf(stderr, "csource: could not find file '%s'\n", setup.source);
        exit(EXIT_UNKNOWN_FILE);
    }

    if(strcmp(setup.source, "-") == 0)
        setup.file = stdin;
    else
        setup.file = fopen(setup.source, "r");

    /* Extract local and system inclusions */
    if(strcmp(setup.target, "include") == 0) {
        int i = 0;
        struct CSourceInclusions *inclusions = csource_extract_inclusions(setup);                

        /* Display resources */
        for(i = 0; i < carray_length(inclusions); i++)
            printf("%i\t\t%s\n", inclusions->contents[i].line, inclusions->contents[i].path.contents);

        carray_free(inclusions, INCLUSION);
    }

    if(strcmp(setup.target, "functions") == 0) {
        csource_extract_functions(setup);
    }

    argparse_free(parser);

    return 0;
}
