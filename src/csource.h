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

#ifndef CWARE_CSOURCE_H
#define CWARE_CSOURCE_H

/* Project dependencies */
#include "carray/carray.h"
#include "libpath/libpath.h"
#include "cstring/cstring.h"
#include "liberror/liberror.h"
#include "argparse/argparse.h"
#include "libmatch/libmatch.h"

/* Helpful macros */
#define INIT_VARIABLE(v) \
    memset(&(v), 0, sizeof((v)))

/* Program configuration */
#define HELP_MESSAGE_STREAM     stderr
#define ERROR_MESSAGE_STREAM    stderr
#define HELP_MESSAGE                                        \
    "csource SOURCE TARGET [ --help | -h ]\n"               \
    "Extract code from a C source file\n"                   \
    "\n"                                                    \
    "Arguments\n"                                           \
    "    command            the type of token to extract\n" \
    "    source             the file to use\n"              \
    "\n"                                                    \
    "Options\n"                                             \
    "    --help, -h         display this message\n"

/* Exit codes */
#define EXIT_HELP_MESSAGE   1
#define EXIT_UNKNOWN_FILE   2
#define EXIT_INTERNAL_ERROR 3
#define EXIT_UNKNOWN_MODULE 4

/*
 * @docgen: structure
 * @brief: parameters to configure a module's function
 * @name: ModuleSetup
 *
 * @param file: the file to read from
 * @type: FILE *
 *
 * @field source: the source file
 * @type: const char *
 *
 * @field command: the command to perform
 * @type: const char *
*/
struct ModuleSetup {
    FILE *file;
    const char *source;
    const char *command;
};

#endif
