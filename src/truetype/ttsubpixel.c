/***************************************************************************/
/*                                                                         */
/*  ttsubpixel.c                                                           */
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

#include <ft2build.h>
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_CALC_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_SFNT_H
#include FT_TRUETYPE_TAGS_H
#include FT_OUTLINE_H

#include "ttsubpixel.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttgload



#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS
  static FT_Bool
  sph_test_tweak ( TT_Face       face,
                 FT_String*    family,
                 int           ppem,
                 FT_String*    style,
                 FT_UInt       glyph_index,
                 SPH_TweakRule* rule,
                 int           num_rules )
  {
    FT_UInt  i;

    /* rule checks may be able to be optimized further */
    for ( i = 0; i < num_rules; i++ )
    {
      if ( family && ( strcmp( rule[i].family, "" ) == 0
        || strcmp ( rule[i].family, family ) == 0 ) )
         if ( rule[i].ppem == 0
           || (rule[i].ppem == ppem) )
            if ( ( style && strcmp ( rule[i].style, "" ) == 0 )
              || strcmp ( rule[i].style, style ) == 0 )
               if ( rule[i].glyph == 0
                 || FT_Get_Char_Index( (FT_Face)face, rule[i].glyph )
                                         == glyph_index )
      {
        /* printf ("%s,%d,%s,%c ", family, ppem, style, rule[i].glyph); */
        return TRUE;
      }
    }
    return FALSE;
  }

  static void
  sph_set_tweaks( TT_Loader  loader,
                  FT_UInt    glyph_index )
  {
    TT_Face  face = (TT_Face)loader->face;
    FT_String* family = face->root.family_name;
    int ppem = loader->size->metrics.x_ppem;
    FT_String* style = face->root.style_name;

    /* loader->exec->sph_tweak_flags = 0x0000; */

    /*printf ("%s,%d,%s,%c ", family, ppem, style, glyph_index); */

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        NORMAL_ROUND_MIRP_Rules, NORMAL_ROUND_MIRP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_NORMAL_ROUND_MIRP;
   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        NORMAL_ROUND_MDRP_Rules, NORMAL_ROUND_MDRP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_NORMAL_ROUND_MDRP;
   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        NORMAL_ROUND_MDAP_Rules, NORMAL_ROUND_MDAP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_NORMAL_ROUND_MDAP;
   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        NORMAL_ROUND_MIAP_Rules, NORMAL_ROUND_MIAP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_NORMAL_ROUND_MIAP;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        SKIP_IUP_Rules, SKIP_IUP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_SKIP_IUP;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALWAYS_SKIP_DELTAP_Rules, ALWAYS_SKIP_DELTAP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_ALWAYS_SKIP_DELTAP;
   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALWAYS_DO_DELTAP_Rules, ALWAYS_DO_DELTAP_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_ALWAYS_DO_DELTAP;
   else if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        DELTAP_RTG_Rules, DELTAP_RTG_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_DELTAP_RTG;
   else if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        DELTAP_RUTG_Rules, DELTAP_RUTG_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_DELTAP_RUTG;
   else if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        DELTAP_RDTG_Rules, DELTAP_RDTG_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_DELTAP_RDTG;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALLOW_DMOVEX_FREEV_Rules, ALLOW_DMOVEX_FREEV_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_ALLOW_DMOVEX_FREEV;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALLOW_DMOVE_FREEV_Rules, ALLOW_DMOVE_FREEV_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_ALLOW_DMOVE_FREEV;


   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALLOW_DMOVEX_FREEV_Rules_Exceptions, ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE ) )
     loader->exec->sph_tweak_flags &= ~SPH_TWEAK_ALLOW_DMOVEX_FREEV;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALLOW_DMOVE_FREEV_Rules_Exceptions, ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE ) )
     loader->exec->sph_tweak_flags &= ~SPH_TWEAK_ALLOW_DMOVE_FREEV;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        RASTERIZER_35_Rules, RASTERIZER_35_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_RASTERIZER_35;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALLOW_MOVEZP2_FREEV_Rules, ALLOW_MOVEZP2_FREEV_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_ALLOW_MOVEZP2_FREEV;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        ALLOW_MOVEZP2_FREEV_Rules_Exceptions, ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE ) )
     loader->exec->sph_tweak_flags &= ~SPH_TWEAK_ALLOW_MOVEZP2_FREEV;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        DO_RS_Rules, DO_RS_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_DO_RS;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        DO_SHPIX_Rules, DO_SHPIX_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_DO_SHPIX;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        SKIP_NONPIXEL_INLINE_MOVES_Rules, SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_SKIP_NONPIXEL_INLINE_MOVES;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        SHPIX_CLASS_A_Rules, SHPIX_CLASS_A_RULES_SIZE ) )
     loader->exec->sph_tweak_flags |= SPH_TWEAK_SHPIX_CLASS_A;

   if ( sph_test_tweak( face, family, ppem, style, glyph_index,
        SHPIX_CLASS_A_Rules_Exceptions, SHPIX_CLASS_A_RULES_EXCEPTIONS_SIZE ) )
     loader->exec->sph_tweak_flags &= ~SPH_TWEAK_SHPIX_CLASS_A;

  }
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING_ADDITIONAL_TWEAKS */
#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
