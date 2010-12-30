/***************************************************************************/
/*                                                                         */
/*  ttsubpixel.h                                                           */
/*                                                                         */
/*    TrueType Subpixel Hinting.                                           */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009,   */
/*            2010 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#ifndef __TTSUBPIXEL_H__
#define __TTSUBPIXEL_H__

#include <ft2build.h>
#include "ttobjs.h"

#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
  /*************************************************************************/
  /*                                                                       */
  /* Tweak flags that are set for each glyph                               */
  /*                                                                       */
  /*                                                                       */
#define SPH_TWEAK_NORMAL_ROUND_MIRP           0x00001
#define SPH_TWEAK_NORMAL_ROUND_MDRP           0x00002
#define SPH_TWEAK_DELTAP_RDTG                 0x00004
#define SPH_TWEAK_DELTAP_RTG                  0x00008
#define SPH_TWEAK_DELTAP_RUTG                 0x00010
#define SPH_TWEAK_ALLOW_DMOVEX_FREEV          0x00020
#define SPH_TWEAK_ALLOW_DMOVE_FREEV           0x00040
#define SPH_TWEAK_ALLOW_MOVEZP2_FREEV         0x00080
#define SPH_TWEAK_ALWAYS_SKIP_DELTAP          0x00100
#define SPH_TWEAK_SKIP_IUP                    0x00200
#define SPH_TWEAK_NORMAL_ROUND_MIAP           0x00400
#define SPH_TWEAK_NORMAL_ROUND_MDAP           0x00800
#define SPH_TWEAK_DO_RS                       0x01000
#define SPH_TWEAK_DO_SHPIX                    0x02000
#define SPH_TWEAK_ALWAYS_DO_DELTAP            0x04000
#define SPH_TWEAK_SKIP_NONPIXEL_INLINE_MOVES  0x08000
#define SPH_TWEAK_SHPIX_CLASS_A               0x10000
#define SPH_TWEAK_RASTERIZER_35               0x20000


  static FT_Bool
  sph_test_tweak ( TT_Face       face,
                 FT_String*    family,
                 int           ppem,
                 FT_String*    style,
                 FT_UInt       glyph_index,
                 SPH_TweakRule* rule,
                 int           num_rules );
  static void
  sph_set_tweaks( TT_Loader  loader,
                  FT_UInt    glyph_index );


  /*************************************************************************/
  /*                                                                       */
  /* These are groups of rules that affect how the TT Interpreter does     */
  /* hinting.                                                              */
  /*                                                                       */
  /* "" string or 0 int/char indicates to apply to all.                    */
  /* "-" used as dummy placeholders, but any non-matching string works     */
  /*                                                                       */
  /* Remaining rules are tweaks for various fonts / glyphs                 */
  /* Some of this could arguably be implemented in fontconfig, however:    */
  /*                                                                       */
  /*  - Fontconfig can't set things on a glyph-by-glyph basis.             */
  /*  - The tweaks that happen here are very low-level, from an average    */
  /*      user's point of view and are best implemented in the hinter      */
  /*                                                                       */
  /* Ideally, some of these should be generalized across affected fonts,   */
  /* and enabled by default in the code. The rule structure is designed so */
  /* that entirely new rules can easily be added when a new compatibility  */
  /* feature is discovered.                                                */
  /*                                                                       */

  /*************************************************************************/
  /*                                                                       */
  /* TT_CONFIG_OPTION_SUBPIXEL_HINTING Rules                               */
  /*                                                                       */
  /* Simply, this attempts to duplicate the fuctionality described here    */
  /* and nothing more:                                                     */
  /*                                                                       */
  /* http://www.microsoft.com/typography/cleartype/truetypecleartype.aspx  */
  /*                                                                       */
  /* This mode is enabled when                                             */
  /* TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS                   */
  /* is undefined                                                          */
  /*                                                                       */


#ifndef TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS

#if FALSE /* THIS RULESET NOT CURRENTLY BEING USED */

#define BITMAP_WIDTHS          FALSE
#define SET_SUBPIXEL           FALSE
#define SET_GRAYSCALE          TRUE
#define SET_MONOCHROME         FALSE
#define SET_COMPATIBLE_WIDTHS  TRUE
#define SET_RASTERIZER_VERSION 35
#define Grids_Per_Pixel_X      1
#define Grids_Per_Pixel_Y      1


/********** MOVE RULES *************/

/* Allow a Direct_Move_X along freedom vector when matched */
#define ALLOW_DMOVEX_FREEV_RULES_SIZE  1
SPH_TweakRule ALLOW_DMOVEX_FREEV_Rules
[ALLOW_DMOVEX_FREEV_RULES_SIZE] = {
  { "", 0, "", 0 },
};

/* Allow a Direct_Move along freedom vector when matched */
#define ALLOW_DMOVE_FREEV_RULES_SIZE  1
SPH_TweakRule ALLOW_DMOVE_FREEV_Rules
[ALLOW_DMOVE_FREEV_RULES_SIZE] = {
  { "", 0, "", 0 },
};

/* Allow a ZP2 Move along freedom vector when matched */
#define ALLOW_MOVEZP2_FREEV_RULES_SIZE  1
SPH_TweakRule ALLOW_MOVEZP2_FREEV_Rules
[ALLOW_MOVEZP2_FREEV_RULES_SIZE] = {
  { "", 0, "", 0 },
};

/* Don't skip RS calls */
#define DO_RS_RULES_SIZE  1
SPH_TweakRule DO_RS_Rules
[DO_RS_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Force requested SHPIX operations when matched  */
#define DO_SHPIX_RULES_SIZE  1
SPH_TweakRule DO_SHPIX_Rules
[DO_SHPIX_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE  1
SPH_TweakRule SKIP_NONPIXEL_INLINE_MOVES_Rules
[SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define NORMAL_ROUND_MIRP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MIRP_Rules
[NORMAL_ROUND_MIRP_RULES_SIZE] = {
  { "", 0, "", 0 },
};

#define NORMAL_ROUND_MIAP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MIAP_Rules
[NORMAL_ROUND_MIAP_RULES_SIZE] = {
  { "", 0, "", 0 },
};

#define NORMAL_ROUND_MDRP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MDRP_Rules
[NORMAL_ROUND_MDRP_RULES_SIZE] = {
  { "", 0, "", 0 },
};

#define NORMAL_ROUND_MDAP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MDAP_Rules
[NORMAL_ROUND_MDAP_RULES_SIZE] = {
  { "", 0, "", 0 },
};

/* Indicates that SHPIX needs to match a touched point on x OR y */
#define SHPIX_CLASS_A_RULES_SIZE  1
SPH_TweakRule SHPIX_CLASS_A_Rules
[SHPIX_CLASS_A_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Mystery rules that make SHPIX work on certain fonts/glyphs. */
/* Indicates that SHPIX needs to match a touched point on x AND y */
/* This is dirty and needs to be generalized and incorporated. */
#define SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule SHPIX_CLASS_A_Rules_Exceptions
[SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

#define ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule ALLOW_DMOVEX_FREEV_Rules_Exceptions
[ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

#define ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule ALLOW_DMOVE_FREEV_Rules_Exceptions
[ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

#define ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule ALLOW_MOVEZP2_FREEV_Rules_Exceptions
[ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

/* Skip IUP instructions when matched */
#define SKIP_IUP_RULES_SIZE  1
SPH_TweakRule SKIP_IUP_Rules
[SKIP_IUP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Skip DELTAP instructions when matched */
#define ALWAYS_SKIP_DELTAP_RULES_SIZE  1
SPH_TweakRule ALWAYS_SKIP_DELTAP_Rules
[ALWAYS_SKIP_DELTAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Always do DELTAP instructions when matched */
#define ALWAYS_DO_DELTAP_RULES_SIZE  1
SPH_TweakRule ALWAYS_DO_DELTAP_Rules
[ALWAYS_DO_DELTAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Do an extra RTG instruction in DELTAP when matched */
#define DELTAP_RTG_RULES_SIZE  1
SPH_TweakRule DELTAP_RTG_Rules
[DELTAP_RTG_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Do an extra RUTG instruction in DELTAP when matched */
#define DELTAP_RUTG_RULES_SIZE  1
SPH_TweakRule DELTAP_RUTG_Rules
[DELTAP_RUTG_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Do an extra RDTG instruction in DELTAP when matched */
#define DELTAP_RDTG_RULES_SIZE 1
SPH_TweakRule DELTAP_RDTG_Rules
[DELTAP_RDTG_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Return MS rasterizer 35 when matched */
#define RASTERIZER_35_RULES_SIZE 1
SPH_TweakRule RASTERIZER_35_Rules
[RASTERIZER_35_RULES_SIZE] = {
  { "-", 0, "", 0 },
};
#endif


#define BITMAP_WIDTHS          FALSE
#define SET_SUBPIXEL           TRUE
#define SET_GRAYSCALE          FALSE
#define SET_MONOCHROME         FALSE
#define SET_COMPATIBLE_WIDTHS  FALSE
#define SET_RASTERIZER_VERSION 37
#define Grids_Per_Pixel_X      64
#define Grids_Per_Pixel_Y      1


/********** MOVE RULES *************/

/* Allow a Direct_Move_X along freedom vector when matched */
#define ALLOW_DMOVEX_FREEV_RULES_SIZE  1
SPH_TweakRule ALLOW_DMOVEX_FREEV_Rules
[ALLOW_DMOVEX_FREEV_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Allow a Direct_Move along freedom vector when matched */
#define ALLOW_DMOVE_FREEV_RULES_SIZE  1
SPH_TweakRule ALLOW_DMOVE_FREEV_Rules
[ALLOW_DMOVE_FREEV_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Allow a ZP2 Move along freedom vector when matched */
#define ALLOW_MOVEZP2_FREEV_RULES_SIZE  1
SPH_TweakRule ALLOW_MOVEZP2_FREEV_Rules
[ALLOW_MOVEZP2_FREEV_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Don't skip RS calls */
#define DO_RS_RULES_SIZE  1
SPH_TweakRule DO_RS_Rules
[DO_RS_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Force requested SHPIX operations when matched  */
#define DO_SHPIX_RULES_SIZE  1
SPH_TweakRule DO_SHPIX_Rules
[DO_SHPIX_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE  1
SPH_TweakRule SKIP_NONPIXEL_INLINE_MOVES_Rules
[SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define NORMAL_ROUND_MIRP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MIRP_Rules
[NORMAL_ROUND_MIRP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define NORMAL_ROUND_MIAP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MIAP_Rules
[NORMAL_ROUND_MIAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define NORMAL_ROUND_MDRP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MDRP_Rules
[NORMAL_ROUND_MDRP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

#define NORMAL_ROUND_MDAP_RULES_SIZE  1
SPH_TweakRule NORMAL_ROUND_MDAP_Rules
[NORMAL_ROUND_MDAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Indicates that SHPIX needs to match a touched point on x OR y */
#define SHPIX_CLASS_A_RULES_SIZE  1
SPH_TweakRule SHPIX_CLASS_A_Rules
[SHPIX_CLASS_A_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Mystery rules that make SHPIX work on certain fonts/glyphs. */
/* Indicates that SHPIX needs to match a touched point on x AND y */
/* This is dirty and needs to be generalized and incorporated. */
#define SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule SHPIX_CLASS_A_Rules_Exceptions
[SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

#define ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule ALLOW_DMOVEX_FREEV_Rules_Exceptions
[ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

#define ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule ALLOW_DMOVE_FREEV_Rules_Exceptions
[ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

#define ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE  1
SPH_TweakRule ALLOW_MOVEZP2_FREEV_Rules_Exceptions
[ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
};

/* Skip IUP instructions when matched */
#define SKIP_IUP_RULES_SIZE  1
SPH_TweakRule SKIP_IUP_Rules
[SKIP_IUP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Skip DELTAP instructions when matched */
#define ALWAYS_SKIP_DELTAP_RULES_SIZE  1
SPH_TweakRule ALWAYS_SKIP_DELTAP_Rules
[ALWAYS_SKIP_DELTAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Always do DELTAP instructions when matched */
#define ALWAYS_DO_DELTAP_RULES_SIZE  1
SPH_TweakRule ALWAYS_DO_DELTAP_Rules
[ALWAYS_DO_DELTAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Do an extra RTG instruction in DELTAP when matched */
#define DELTAP_RTG_RULES_SIZE  1
SPH_TweakRule DELTAP_RTG_Rules
[DELTAP_RTG_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Do an extra RUTG instruction in DELTAP when matched */
#define DELTAP_RUTG_RULES_SIZE  1
SPH_TweakRule DELTAP_RUTG_Rules
[DELTAP_RUTG_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Do an extra RDTG instruction in DELTAP when matched */
#define DELTAP_RDTG_RULES_SIZE 1
SPH_TweakRule DELTAP_RDTG_Rules
[DELTAP_RDTG_RULES_SIZE] = {
  { "-", 0, "", 0 },
};

/* Return MS rasterizer 35 when matched */
#define RASTERIZER_35_RULES_SIZE 1
SPH_TweakRule RASTERIZER_35_Rules
[RASTERIZER_35_RULES_SIZE] = {
  { "-", 0, "", 0 },
};


  /*************************************************************************/
  /*                                                                       */
  /* TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS Rules             */
  /*                                                                       */
  /* This set of rules is an attempt at enhancing the basic subpixel rules */
  /* defined above, to fix visual problems with individual fonts and       */
  /* glyphs.                                                               */
  /*                                                                       */
  /* This mode is enabled when                                             */
  /* TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS                   */
  /* is defined                                                            */
  /*                                                                       */
  /*     ****************** WORK IN PROGRESS *******************           */
  /*                                                                       */

#else

#define BITMAP_WIDTHS          FALSE
#define SET_SUBPIXEL           TRUE
#define SET_GRAYSCALE          FALSE
#define SET_MONOCHROME         FALSE
#define SET_COMPATIBLE_WIDTHS  FALSE
#define SET_RASTERIZER_VERSION 37
#define Grids_Per_Pixel_X      64
#define Grids_Per_Pixel_Y      1


/* Don't avoid RS Rules (as the basic subpixel hinting does) */
#define DO_RS_RULES_SIZE  1
SPH_TweakRule DO_RS_Rules[DO_RS_RULES_SIZE] = {
  { "-", 0, "", 0 },
};


/******************* DELTA RULES *********************/

/* Do requested SHPIX operations when matched.  This requires ZP2 moves */
/* to be enabled in order to get SHPIX moves in the X direction */
/* Do all "Optimized for Cleartype" fonts need to be here??.  */
/* the below doesn't work because the bit is not set in MS cleartype fonts */
/* CUR.face->header.Flags & 0x10000 */

#define DO_SHPIX_RULES_SIZE  7
SPH_TweakRule DO_SHPIX_Rules[DO_SHPIX_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-Verdana", 0, "Regular", 0 },
  { "-Verdana", 12, "Regular", 0 },
  { "Verdana", 13, "Regular", 0 },
  /* aligns to pixels nicely, but messes up some glyphs */
  { "-Times New Roman", 0, "Regular", 0 },
  { "+++Segoe UI", 0, "Regular", 0 },
  { "-Segoe UI", 0, "Semibold", 0 },
};

/* Indicates that SHPIX needs to match a touched point on x OR y */
#define SHPIX_CLASS_A_RULES_SIZE  1
SPH_TweakRule SHPIX_CLASS_A_Rules[SHPIX_CLASS_A_RULES_SIZE] = {
  { "", 0, "", 0 },
};

/* Mystery rules that make SHPIX work on certain fonts/glyphs. */
/* Indicates that SHPIX needs to match a touched point on x AND y */
/* This is dirty and needs to be generalized and incorporated. */
#define SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE  2
SPH_TweakRule SHPIX_CLASS_A_Rules_Exceptions
[SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE] = {
  { "---", 0, "", 0 },
  { "Arial", 11, "Regular", 's' },
};

/* Skip moves that don't align to a pixel in various functions. */
/* Fixes Tahoma, trebuchet oddities and some issues with $ */
#define SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE  4
SPH_TweakRule SKIP_NONPIXEL_INLINE_MOVES_Rules
[SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE] = {
  { "", 0, "Regular", 0 },
  /* keeps the weight in the center of the N */
  { "", 0, "Regular", 'N' },
  { "Tahoma", 0, "Regular", 0 },
  { "==Trebuchet MS", 0, "Regular", 0 },
};


/********** MOVE RULES *************/

/* Allow a Direct_Move_X along X freedom vector when matched */
#define ALLOW_DMOVEX_FREEV_RULES_SIZE  20
SPH_TweakRule ALLOW_DMOVEX_FREEV_Rules[ALLOW_DMOVEX_FREEV_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-", 0, "Regular", 0 },
  { "-", 0, "Italic", 0 },
  { "-", 0, "Regular", 0 },
  { "-Verdana", 12, "Regular", 0 },
  { "-Geneva", 0, "", 0 },
  { "-Courier New", 0, "Regular", 0 },
  { "-Courier New", 0, "", 0 },
  { "-Arial", 0, "Bold", 0 },
  { "Verdana", 13, "Regular", 0 },
  { "-Times New Roman", 0, "Regular", 0 },
  { "Arial", 13, "Regular", 0 },
  { "Arial", 14, "Regular", 0 },
  { "-Tahoma", 0, "Regular", 0 },
  { "+++Trebuchet MS", 0, "Regular", 0 },
  { "-Trebuchet MS", 0, "Bold", 0 },
  { "-Segoe UI", 0, "Semibold", 0 },
  { "-Segoe UI", 12, "Regular", 'H' },
  { "Arial Narrow", 0, "Regular", 0 },
  { "+++Andale Mono", 17, "Regular", 0 },
};

/* Allow a Direct_Move along X freedom vector when matched */
#define ALLOW_DMOVE_FREEV_RULES_SIZE  21
SPH_TweakRule ALLOW_DMOVE_FREEV_Rules
[ALLOW_DMOVE_FREEV_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-", 0, "Regular", 0 },
  { "-", 0, "Italic", 0 },
  { "-Verdana", 12, "Regular", 0 },
  { "Verdana", 13, "Regular", 0 },
  { "-Courier New", 0, "Bold", 0 },
  { "-Times New Roman", 0, "Regular", 0 },
  { "Arial", 13, "Regular", 0 },
  { "Arial", 14, "Regular", 0 },
  { "-Arial", 0, "Bold", 0 },
  { "-Tahoma", 0, "Regular", 0 },
  { "+++Trebuchet MS", 0, "Regular", 0 },
  { "-Trebuchet MS", 0, "Bold", 0 },
  { "-Geneva", 0, "", 0 },
  { "-------", 0, "Regular", 0 },
  { "-Segoe UI", 0, "Semibold", 0 },
  { "+++Segoe UI", 12, "Regular", 'H' },
  { "-----", 0, "Regular", 0 },
  { "Arial Narrow", 0, "Regular", 0 },
  { "+++Andale Mono", 17, "Regular", 0 },
  { "-Courier New", 0, "", 0 },
};


#define ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE  9
SPH_TweakRule ALLOW_DMOVEX_FREEV_Rules_Exceptions
  [ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
  { "-Times New Roman", 0, "Regular", 'a' },
  { "-Times New Roman", 0, "Regular", 'q' },
  { "-Times New Roman", 0, "Regular", 'P' },
  { "-Times New Roman", 0, "Regular", 'R' },
  { "-Times New Roman", 0, "Regular", 'B' },
  { "Arial", 0, "Regular", '4' },
  { "Arial", 0, "Regular", 's' },
  { "Arial", 0, "Regular", '^' },
};

#define ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE  9
SPH_TweakRule ALLOW_DMOVE_FREEV_Rules_Exceptions
[ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
  { "-Times New Roman", 0, "Regular", 'a' },
  { "-Times New Roman", 0, "Regular", 'q' },
  { "-Times New Roman", 0, "Regular", 'P' },
  { "-Times New Roman", 0, "Regular", 'R' },
  { "-Times New Roman", 0, "Regular", 'B' },
  { "Arial", 0, "Regular", '4' },
  { "Arial", 0, "Regular", 's' },
  { "Arial", 0, "Regular", '^' },
};


/* Allow a ZP2 Move along freedom vector when matched */
/* This is called from SHP, SHPIX, SHC, SHZ           */
#define ALLOW_MOVEZP2_FREEV_RULES_SIZE  14
SPH_TweakRule ALLOW_MOVEZP2_FREEV_Rules[ALLOW_MOVEZP2_FREEV_RULES_SIZE] = {
  { "-", 0, "Regular", 0 },
  { "-Verdana", 12, "Regular", 0 },
  { "Verdana", 13, "Regular", 0 },
  { "-Times New Roman", 0, "Regular", 0 },
  { "-Courier New", 0, "Bold", 0 },
  { "-Tahoma", 0, "Regular", 0 },
  { "-Courier New", 0, "", 0 },
  { "Arial", 13, "Regular", 0 },
  { "Arial", 14, "Regular", 0 },
  { "-Arial", 0, "Bold", 0 },
  { "+++Trebuchet MS", 0, "Regular", 0 },
  { "-Trebuchet MS", 0, "Bold", 0 },
  { "-Verdana", 13, "Regular", 0 },
  /* this needs a bit of work though */
  { "-Microsoft Sans Serif", 0, "Regular", 0 },
};

/* Return MS rasterizer 35 when matched */
#define RASTERIZER_35_RULES_SIZE 1
SPH_TweakRule RASTERIZER_35_Rules
[RASTERIZER_35_RULES_SIZE] = {
  { "Times New Roman", 0, "Regular", 'i' },
};

/************** DIRTY, DIRTY HACKS!!!!!!!! ***************/

#define ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE  11
SPH_TweakRule ALLOW_MOVEZP2_FREEV_Rules_Exceptions
[ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE] = {
  { "-", 0, "", 0 },
  { "Times New Roman", 0, "Regular", 'a' },
  { "Times New Roman", 0, "Regular", 'q' },
  { "Verdana", 13, "Regular", 'N' },
  { "Verdana", 13, "Regular", 'f' },
  { "Verdana", 13, "Regular", 'v' },
  { "-------", 13, "Regular", 'k' },
  { "Verdana", 13, "Regular", 'w' },
  { "Verdana", 13, "Regular", 'x' },
  { "Verdana", 13, "Regular", 'y' },
  { "Verdana", 13, "Regular", 'z' },
};

/*********** ROUNDING ***************/
/* These only have an effect on fonts that are allowed to move X (above) */
/* It appears all MS cleartype fonts may be OK using normal rounds */
#define NORMAL_ROUND_MIRP_RULES_SIZE  16
SPH_TweakRule NORMAL_ROUND_MIRP_Rules[NORMAL_ROUND_MIRP_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-Tahoma", 9, "Regular", 0 },
  { "-Courier New", 0, "Regular", 'W' },
  { "-Courier New", 0, "Regular", 'K' },
  { "-Courier New", 0, "Regular", 'k' },
  { "-Courier New", 0, "Regular", 'V' },
  { "-Courier New", 0, "Regular", 'O' },
  { "-Courier New", 0, "Regular", 'X' },
  { "-Courier New", 0, "Regular", 'Y' },
  { "-Courier New", 0, "Regular", 'A' },
  { "-Courier New", 0, "Regular", 'v' },
  { "-Courier New", 0, "Regular", 'z' },
  { "-Courier New", 0, "Regular", 'x' },
  { "-Courier New", 0, "Regular", 'y' },
  { "Calibri", 0, "Italic", 0 },
  { "Calibri", 0, "Bold Italic", 0 },
};

#define NORMAL_ROUND_MIAP_RULES_SIZE  16
SPH_TweakRule NORMAL_ROUND_MIAP_Rules[NORMAL_ROUND_MIAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-Tahoma", 9, "Regular", 0 },
  { "-Courier New", 0, "Regular", 'W' },
  { "-Courier New", 0, "Regular", 'K' },
  { "-Courier New", 0, "Regular", 'k' },
  { "-Courier New", 0, "Regular", 'V' },
  { "-Courier New", 0, "Regular", 'O' },
  { "-Courier New", 0, "Regular", 'X' },
  { "-Courier New", 0, "Regular", 'Y' },
  { "-Courier New", 0, "Regular", 'A' },
  { "-Courier New", 0, "Regular", 'v' },
  { "-Courier New", 0, "Regular", 'z' },
  { "-Courier New", 0, "Regular", 'x' },
  { "-Courier New", 0, "Regular", 'y' },
  { "Calibri", 0, "Italic", 0 },
  { "Calibri", 0, "Bold Italic", 0 },
};

#define NORMAL_ROUND_MDRP_RULES_SIZE  16
SPH_TweakRule NORMAL_ROUND_MDRP_Rules[NORMAL_ROUND_MDRP_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-Tahoma", 9, "Regular", 0 },
  { "-Courier New", 0, "Regular", 'W' },
  { "-Courier New", 0, "Regular", 'K' },
  { "-Courier New", 0, "Regular", 'k' },
  { "-Courier New", 0, "Regular", 'V' },
  { "-Courier New", 0, "Regular", 'O' },
  { "-Courier New", 0, "Regular", 'X' },
  { "-Courier New", 0, "Regular", 'Y' },
  { "-Courier New", 0, "Regular", 'A' },
  { "-Courier New", 0, "Regular", 'v' },
  { "-Courier New", 0, "Regular", 'z' },
  { "-Courier New", 0, "Regular", 'x' },
  { "-Courier New", 0, "Regular", 'y' },
  { "Calibri", 0, "Italic", 0 },
  { "Calibri", 0, "Bold Italic", 0 },
};

#define NORMAL_ROUND_MDAP_RULES_SIZE  16
SPH_TweakRule NORMAL_ROUND_MDAP_Rules[NORMAL_ROUND_MDAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "-Tahoma", 9, "Regular", 0 },
  { "-Courier New", 0, "Regular", 'W' },
  { "-Courier New", 0, "Regular", 'K' },
  { "-Courier New", 0, "Regular", 'k' },
  { "-Courier New", 0, "Regular", 'V' },
  { "-Courier New", 0, "Regular", 'O' },
  { "-Courier New", 0, "Regular", 'X' },
  { "-Courier New", 0, "Regular", 'Y' },
  { "-Courier New", 0, "Regular", 'A' },
  { "-Courier New", 0, "Regular", 'v' },
  { "-Courier New", 0, "Regular", 'z' },
  { "-Courier New", 0, "Regular", 'x' },
  { "-Courier New", 0, "Regular", 'y' },
  { "Calibri", 0, "Italic", 0 },
  { "Calibri", 0, "Bold Italic", 0 },
};


/* Skip IUP instructions when matched */
#define SKIP_IUP_RULES_SIZE  6
SPH_TweakRule SKIP_IUP_Rules[SKIP_IUP_RULES_SIZE] = {
  { "Arial", 13, "Regular", 'a' },
  { "-", 0, "Regular", '2' },
  { "-", 0, "", 0 },
  { "-", 0, "Regular", 'a' },
  { "-", 0, "Regular", 'V' },
  { "-", 0, "Light", 0 },
};

/* Skip DELTAP instructions when matched */
#define ALWAYS_SKIP_DELTAP_RULES_SIZE  19
SPH_TweakRule ALWAYS_SKIP_DELTAP_Rules[ALWAYS_SKIP_DELTAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "--Courier New", 0, "Regular", 'V' },
  { "Verdana", 10, "Regular", 0 },
  { "-Trebuchet MS", 0, "Regular", 'W' },
  { "-Trebuchet MS", 0, "Regular", 'w' },
  { "-Verdana", 0, "Italic", 'v' },
  { "-Verdana", 0, "Italic", 'w' },
  { "-Verdana", 0, "Italic", 'x' },
  { "-Verdana", 0, "Italic", 'y' },
  { "-Verdana", 0, "Italic", 'z' },
  { "-Verdana", 0, "Regular", 'v' },
  { "-Verdana", 10, "Regular", 'w' },
  { "-Verdana", 0, "Regular", 'y' },
  { "-Verdana", 0, "Regular", 'z' },
  { "-Arial Bold", 0, "Bold", 's' },
  { "Trebuchet MS", 14, "Regular", 'e' },
  { "Trebuchet MS", 0, "Italic", 0 },
  { "-Arial", 0, "Italic", 0 },
  { "-", 0, "Italic", 0 },
};

/* Always do DELTAP instructions when matched */
#define ALWAYS_DO_DELTAP_RULES_SIZE  3
SPH_TweakRule ALWAYS_DO_DELTAP_Rules[ALWAYS_DO_DELTAP_RULES_SIZE] = {
  { "-", 0, "", 0 },
  { "DejaVu Sans", 14, "Regular", 'k' },
  { "DejaVu Sans", 14, "Regular", 'K' },
};

/* Do an extra RTG instruction in DELTAP when matched */
#define DELTAP_RTG_RULES_SIZE  4
SPH_TweakRule DELTAP_RTG_Rules[DELTAP_RTG_RULES_SIZE] = {
  { "-Arial Unicode MS", 0, "Regular", 0 },
  { "-Microsoft Sans Serif", 0, "Regular", '0' },
  { "--Verdana", 0, "", 0 },
  { "-Trebuchet MS", 14, "Regular", 'e' },
};

/* Do an extra RUTG instruction in DELTAP when matched */
#define DELTAP_RUTG_RULES_SIZE  2
SPH_TweakRule DELTAP_RUTG_Rules[DELTAP_RUTG_RULES_SIZE] = {
  { "-", 14, "Regular", 'e' },
  { "-", 0, "", 0 },
};

/* Do an extra RDTG instruction in DELTAP when matched */
#define DELTAP_RDTG_RULES_SIZE 28
SPH_TweakRule DELTAP_RDTG_Rules[DELTAP_RDTG_RULES_SIZE] = {
  { "Calibri", 0, "Italic", 0 },
  { "Comic Sans MS", 0, "Regular", 0 },
  { "Lucida Grande", 0, "Regular", 'e' },
  { "Lucida Grande", 12, "Bold", 0 },
  { "Microsoft Sans Serif", 0, "Regular", '7' },
  { "Microsoft Sans Serif", 0, "Regular", 'O' },
  { "Microsoft Sans Serif", 0, "Regular", 'Q' },
  { "Microsoft Sans Serif", 0, "Regular", 'X' },
  { "Microsoft Sans Serif", 0, "Regular", 'e' },
  { "Microsoft Sans Serif", 0, "Regular", 'o' },
  { "-", 0, "", 0 },
  { "-", 0, "Regular", 'O' },
  { "-", 0, "Regular", 'U' },
  { "-", 0, "Regular", 'e' },
  { "-", 0, "Regular", 'g' },
  { "Tahoma", 0, "Bold", '0' },
  { "Tahoma", 16, "Bold", 'C' },
  { "Tahoma", 16, "Bold Italic", 'C' },
  { "Trebuchet MS", 0, "", '0' },
  { "-", 9, "", 'w' },
  { "Verdana", 0, "", '0' },
  { "Verdana", 0, "Bold Italic", '7' },
  { "Verdana", 0, "Bold Italic", 'v' },
  { "Verdana", 0, "Bold Italic", 'w' },
  { "Verdana", 0, "Bold", 0 },
  { "Verdana", 0, "Italic", 'o' },
  { "Verdana", 0, "Regular", 'x' },
  { "Trebuchet MS", 14, "Regular", 'e' },
};

#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS */
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */

#endif /* __TTSUBPIXEL_H__ */
