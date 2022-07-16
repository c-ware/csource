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

#ifndef CWARE_CSOURCE_EXTRACT_INCLUDE_H
#define CWARE_CSOURCE_EXTRACT_INCLUDE_H

#define INCLUSION_TYPE_LOCAL    0
#define INCLUSION_TYPE_SYSTEM   1

/* Data structure properties */
#define INCLUSION_TYPE  struct CSourceInclusion
#define INCLUSION_HEAP  1
#define INCLUSION_FREE(value) cstring_free((value).path)

struct Cstring;
struct ModuleSetup;

/*
 * @docgen: structure
 * @brief: represents an inclusion in a source file
 * @name: CSourceInclusion
 *
 * @field line: the line the inclusion was on
 * @type: int
 *
 * @field type: the type of inclusion
 * @type: int
 *
 * @field path: the path of the inclusion
 * @type: char *
*/
struct CSourceInclusion {
    int line;
    int type;
    struct CString path;
};

/*
 * @docgen: structure
 * @brief: an array of inclusions
 * @name: CSourceInclusions
 *
 * @field length: the length of the array
 * @type: int
 *
 * @field capacity: the capacity of the array
 * @type: int
 *
 * @field contents: the inclusions in the array
 * @type: struct CSourceInclusion *
*/
struct CSourceInclusions {
    int length;
    int capacity;
    struct CSourceInclusion *contents;
};

struct CSourceInclusions *csource_extract_inclusions(struct ModuleSetup setup);









#endif
