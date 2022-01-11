#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>


//        Kronos String Format
//
//  n      : Number type (bYTE/sHORT/iNTEGER/lONG)
//  nu     : Number
//  ch     : Character
//  %      : Format expression
//
//  %c     -> Print ASCII character
//  %l     -> Print signed number in decimal
//  %u     -> Print unsigned number in decimal
//  %xn    -> Print unsigned number in hexadecimal
//  %bn    -> Print unsigned number in binary
//  %s     -> Print null-terminated string
//  %%     -> Print %
//  
//  %{chnu% -> Print `nu` of `ch` back padded format expression
//  %{chn%  -> Print `n`  of `ch` back padded format expression
//  %{cn%   -> Print `n`  of `c`  back padded format expression
//
//  %}chnu% -> Print `nu` of `ch` front padded format expression
//  %}chn%  -> Print `n`  of `ch` front padded format expression
//  %}cn%   -> Print `n`  of `c`  front padded format expression



struct FormatCtx
{
	char       *str;
	size_t     size;
	const char *fmt;
	size_t     spos;
};

struct FormatSpec
{
	size_t    pad;
	char    padch;
	uint8_t front : 1;
	uint8_t  type : 2;
};


size_t Format(char *str, size_t size, const char *fmt, va_list ap);
