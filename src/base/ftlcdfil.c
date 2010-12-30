/***************************************************************************/
/*                                                                         */
/*  ftlcdfil.c                                                             */
/*                                                                         */
/*    FreeType API for color filtering of subpixel bitmap glyphs (body).   */
/*                                                                         */
/*  Copyright 2006, 2008, 2009, 2010 by                                    */
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
#include FT_LCD_FILTER_H
#include FT_IMAGE_H
#include FT_INTERNAL_OBJECTS_H

#include <math.h>
#include <string.h>
#include <strings.h>

#ifdef FT_CONFIG_OPTION_SUBPIXEL_RENDERING

/*
int sinc( int val ) {
  if (val == 0.0)
  return(1.0);
  return 256.0 * (sin(3.14*(double) (val/256.0))/(3.14*(double) (val/256.0)));
}

int sigmoidal( int val ) {
   int val2;
   val2 = 256.0 /(1.0+exp(-(10.0*(val/256.0-.5))));
   return window(val2);
}

int window ( int val )
{
  if (val < 0 ) return 0;
  if (val > 255) return 255;
  else return val;
}
*/
int gamma2 ( int val, float value ) {
   return 256 * (1.0 - pow((1.0 - (float)val/ 256.0) , 1.0/value));
}
/*
int gamma3 ( int val, float value ) {
  1- (x-1)^10
}
*/
/*
int gamma ( int val ) {
   return 256 * (1.0 - pow((1.0 - (float)val/ 256.0) , 1.5));
}

int brick (int val) {
   if (val > 50) return val;
   else return 0;
}

int brick2 (int val) {
   if (val > 100) return val;
   if (val > 50) return 100;
   else return 0;
}

int none (int val) {
   return val;
}

float pixmoid (int val1, int val2, int val3, int testval)
{
  int avg;
  int result;

  avg = (val1 + val2 + val3)/3;

  if ( avg < 128 )
  {
    if (testval > 128) result = avg;
    else result = testval;
  }
  else
  {
    if (testval < 128) result = avg;
    else result = testval;
  }

  return none(result);
}

float pixmoid2 (int val1, int val2, int val3, int testval)
{
  int avg;
  int result;

  avg = (val1 + val2 + val3)/3;

  if ( avg > 160 ) return 255;
  else if  ( avg < 100 ) return 0;
  else return testval;

  return none(result);

}
*/
/*
bool Resample(FT_Byte*  line, int newWidth, int newHeight)
{

  unsigned char* newData = new unsigned char [newWidth * newHeight * 3];

  double scaleWidth =  (double)newWidth / (double)_width;
  double scaleHeight = (double)newHeight / (double)_height;

  for(int cy = 0; cy < newHeight; cy++)
  {
      for(int cx = 0; cx < newWidth; cx++)
      {
          int pixel = (cy * (newWidth *3)) + (cx*3);
          int nearestMatch =  (((int)(cy / scaleHeight)
          * (_width *3)) + ((int)(cx / scaleWidth) *3) );

          newData[pixel    ] =  _data[nearestMatch    ];
          newData[pixel + 1] =  _data[nearestMatch + 1];
          newData[pixel + 2] =  _data[nearestMatch + 2];
      }
  }

  delete[] _data;
  _data = newData;
  _width = newWidth;
  _height = newHeight;

  return true;
}*/


  /* Stem alignment for bitmaps;  A hack with very nice results */
  /* Ideally this could be implemented on the outline, prior to
   * rasterization */
  static void
  _lcd_stem_align ( FT_Bitmap*      bitmap,
                    FT_Render_Mode  mode,
                      FT_GlyphSlot   slot )
  {
    /*FT_Byte*  weights = library->lcd_weights;*/
    FT_UInt   width   = (FT_UInt)bitmap->width;
    FT_UInt   height  = (FT_UInt)bitmap->rows;

    FT_UInt   h;

    FT_UInt alignment_type = 0;
    FT_UInt checked_alignment_type = 0;
    float pseudo_gamma_value = 1;
    float pseudo_gamma_lt = 0;
    FT_UInt checked_pseudo_gamma_value = 0;

    if ( checked_alignment_type == 0)
    {
      char *alignment_type_env = getenv( "INFINALITY_FT_STEM_ALIGNMENT_TYPE" );
      if ( alignment_type_env != NULL )
      {
        /*sscanf ( alignment_type_env, "%d", &alignment_type );*/
        if      (strcasecmp(alignment_type_env, "full") == 0) alignment_type = 1;
        else if (strcasecmp(alignment_type_env, "medium") == 0) alignment_type = 2;
        else if (strcasecmp(alignment_type_env, "medium1") == 0) alignment_type = 2;
        else if (strcasecmp(alignment_type_env, "slight") == 0) alignment_type = 3;
        else if (strcasecmp(alignment_type_env, "slight1") == 0) alignment_type = 3;
        else if (strcasecmp(alignment_type_env, "medium2") == 0) alignment_type = 4;
        else if (strcasecmp(alignment_type_env, "slight2") == 0) alignment_type = 5;
        else if (strcasecmp(alignment_type_env, "infinality") == 0) alignment_type = 6;
        else if (strcasecmp(alignment_type_env, "infinality1") == 0) alignment_type = 6;
        else alignment_type = 0;

        if ( /*strstr(slot.metrics->root.scaler.face->style_name, "Regular")
          || strstr(slot.metrics->root.scaler.face->style_name, "Book")
          || strstr(slot.metrics->root.scaler.face->style_name, "Medium")
          ||*/ strcasestr(slot->face->style_name, "Italic")
          || strcasestr(slot->face->style_name, "Oblique") )
          alignment_type = 0;
        if ( strcasestr(slot->face->style_name, "Bold") )
          alignment_type = 0;
      }
      checked_alignment_type = 1;
    }

    if ( checked_pseudo_gamma_value == 0 )
    {
      char *pseudo_gamma_value_env = getenv( "INFINALITY_FT_PSEUDO_GAMMA" );
      if ( pseudo_gamma_value_env != NULL )
      {
        float f1, f2;

        if ( strcasecmp(pseudo_gamma_value_env, "default" ) != 0)
        {
          sscanf ( pseudo_gamma_value_env, "%f %f", &f1, &f2 );
          pseudo_gamma_lt = f1;
          pseudo_gamma_value = f2;
        }
        if ( pseudo_gamma_value < .1 ) pseudo_gamma_value = 1;
        if ( pseudo_gamma_lt < 0 ) pseudo_gamma_lt = 1;
      }
      checked_pseudo_gamma_value = 1;
    }

    /*printf("%s,%s ", slot->face->family_name, slot->face->style_name);*/
    /*printf("%d ", slot->face->size->metrics.x_ppem);*/

    /* set gamma value to 1 if out of range */
    if ( slot->face->size->metrics.x_ppem >= pseudo_gamma_lt )
    {
      pseudo_gamma_value = 1;
    }

    /* don't do alignment for < 10 */
    if ( slot->face->size->metrics.x_ppem < 10 )
    {
      alignment_type = 0;
    }

    if ( mode == FT_RENDER_MODE_LCD )
    {
      if (width >= 4 && alignment_type != 0 )
      {
        FT_Byte*  line = bitmap->buffer;
        FT_Byte*  lineabove = bitmap->buffer;
        FT_Byte*  linebelow = bitmap->buffer;

        FT_UInt vsums[width], vtotals[width], offsetruns[width / 2][width/2];
        FT_UInt offsetrank[width/2], offsetchosen, stemwidths[width], stemwidthsmax;
        FT_UInt stemwidth, vstems = 0;

        FT_UInt shift = 0;
        FT_UInt  xx;

        FT_ULong rtotal = 0, vweight = 0, wtotal = 0;
        FT_UInt lreached = 0, rreached = 0, rr = 0, ll = 0, testwidth;
        FT_UInt lwidth = 0, loffset = 0, offset = 0, htotal = 0, windowstart;
        FT_UInt windowelement;

        line = bitmap->buffer;
        rreached = 0; lreached = width, ll = width;

        lwidth = 0;
        loffset = 0;


        /* initialize variables - can this be done inline??? */
        for ( testwidth = 3; testwidth < 4; testwidth += 1 )
        {
          for ( offset = 0; offset < 3; offset +=1 )
          {
            offsetruns[offset][testwidth] = 0;
            offsetrank[offset] = 0;
          }
        }
        for ( xx = 0; xx < width; xx += 1 )
        {
          stemwidths[xx] = 0;
        }


        for ( h = (FT_UInt)bitmap->rows; h > 0; h--, line += bitmap->pitch )
        {

          if (rr > rreached) rreached = rr;
          if (ll < lreached) lreached = ll;

          rr = width;
          ll = 0;
          rtotal = 0;
          htotal = 0;

          /*stemwidthsmax = 0;*/
          stemwidth = 0;


          /* Calculate various sums and stem widths of glyph */
          for ( xx = 0; xx < width; xx += 1 )
          {
            if (line[xx] >= 128)
            {
              stemwidth += 1;
              /*if (stemwidth > stemwidthsmax) stemwidthsmax = stemwidth;*/ /* necessary ? */
            }
            else
            {
              if (xx > 0 && line[xx - 1] >= 128) stemwidths[stemwidth] += 1;
              stemwidth = 0;
            }

            if (h == (FT_UInt)bitmap->rows) vsums[xx] = 0;
            if ( h == (FT_UInt)bitmap->rows ) vtotals[xx] = 0;

            if (line[xx] == 0) vsums[xx] -= 255;
            else vsums[xx] += line[xx];
            if (vsums[xx] < 0) vsums[xx] = 0;

            if (ll == 0 && line[xx] != 0) ll = (xx);

            if (line[xx] != 0) rr = (xx);

            if (xx < width / 2) vweight -= line[xx];
            else vweight += line[xx];

            htotal += line [xx];
            vtotals[xx] += line[xx];
          }

          if ( h < (FT_UInt)bitmap->rows ) lineabove = line - bitmap->pitch;
          if ( h > 1 ) linebelow = line + bitmap->pitch;


          /* Determine the offset at which the most weight of the glyph exists */
          /* This is currently hard-coded at 3, but the code is here to adjust */
          for ( testwidth = 3; testwidth < 4; testwidth += 1 )
          {
            /* test the widths at each of these offsets */
            for ( offset = 0; offset < 3; offset +=1 )
            {
              /* test window of size testwidth, starting at offset */
              rtotal = 0;
              for ( windowstart = offset; windowstart < width;
                  windowstart += testwidth )
              {
                /* calculate total for this window */
                wtotal = 0;
                for ( windowelement = windowstart;
                    windowelement < windowstart + testwidth; windowelement += 1 )
                  if ( windowelement < width)
                  {
                    wtotal += line[windowelement];

                    /* Assign extra value to this subpixel under certain conditions */
                    if ( line[windowelement] == 255 )
                    {
                      /* favor if full pixels above and below */
                      if ( h < (FT_UInt)bitmap->rows
                        && lineabove[windowelement] == 255 )
                        wtotal += 10;
                      if ( h > 1 && linebelow[windowelement] == 255 )
                        wtotal += 10;

                      /* favor if full pixels next to them */
                      if ( windowelement > 0 && line[windowelement-1] == 255 )
                      {
                        wtotal += 10;
                        if ( windowelement > 1 &&  line[windowelement-2] == 255 )
                        {
                          wtotal += 10;
                          if ( windowelement > 2 && line[windowelement-3] == 255 )
                            wtotal += 10;
                        }
                      }
                      if ( windowelement < width - 1 && line[windowelement+1] == 255 )
                      {
                        wtotal += 10;
                        if ( windowelement < width - 2 && line[windowelement+2] == 255 )
                        {
                          wtotal += 10;
                          if ( windowelement < width - 3 && line[windowelement+3] == 255 )
                            wtotal += 10;
                        }
                      }
                    }
                  }
                /* divide window total by number of subpixel samples */
                /* add to total for this run */
                rtotal += (wtotal * wtotal) / testwidth;
              }
              /* dont count horizontal stems */
              /*if (rtotal < ( 255 * testwidth ) * (255 * testwidth) * (width / testwidth) / (testwidth * 2)  )*/
              if ( rtotal < ( 255 * 255 * width / 2 ) )
                offsetruns[offset][testwidth] += rtotal;
            }

            /* determine the best offset for this width and increment its counter */
            offsetchosen = 0;
            for ( offset = 0; offset < 2; offset +=1 )
            {
              if ( offsetruns[offset][testwidth] < offsetruns[offset + 1][testwidth] ){
                offsetrank[offset + 1] += 1;
                offsetchosen = offset;
              }
            }
            if (offsetchosen == 0) offsetrank[0] += 1;
          }
        }

        /* Use the best offset */
        loffset = 0;
        for ( offset = 0; offset < 2; offset +=1 )
        {
          if ( offsetrank[offset] < offsetrank[offset + 1] ){
            loffset = offset + 1;
          }
        }

        /* Use the best width */
        lwidth = 0;
        stemwidthsmax = 0;

        for ( xx = 0; xx < width - 1; xx +=1 )
        {
          if ( stemwidthsmax < stemwidths[xx + 1] ){
            lwidth = xx + 1;
            stemwidthsmax = stemwidths[xx + 1];
          }
        }

        /* currently unused */
        rreached = width - rreached;

        /* Set the number of vertical stem components */
        for ( xx = 0; xx < width; xx += 1 )
        {
          if ( height > 0 && vsums[xx] / height > 110 )
            vstems++;
        }


        /******************** CALCULATE GLYPH ALIGNMENT *********************/
        /*printf(" %d,%d,%d,%d,%d,%d,%d\n", width, height, lreached,
        rreached, lwidth,vstems,alignment_type );*/

        shift = 0;

        /* infinality1 alignment - combination of below */
        if ( alignment_type == 6 )
        {
          if ( lwidth < 5 ) alignment_type = 2;
          else alignment_type = 1;
        }

        /* strong alignment - shift glyph left or right one subpixel */
        if ( alignment_type == 1 /*&& vstems > 0*/ )
        {
          if ( lwidth < 5)
          {
          /* lower widths should use this */
            if (loffset % 3 == 0) shift = -1;
            if (loffset % 3 == 1) shift = 1;
            if (loffset % 3 == 2) shift = 0;
          }
          else if ( lwidth < 6 )
          {
          /* medium widths should use this */
            if (loffset % 3 == 0) shift = 1;
            if (loffset % 3 == 1) shift = 0;
            if (loffset % 3 == 2) shift = -1;
          }
          else if ( lwidth < 20 )
          {
          /* higher widths should use this */
            if (loffset % 3 == 0) shift = 1;
            if (loffset % 3 == 1) shift = -1;
            if (loffset % 3 == 2) shift = 0;
          }
        }

        /* medium alignment - shift glyph ONLY LEFT one subpixel
        * - a compromise to prevent spacing issues */
        else if ( alignment_type == 2 /*&& vstems > 0*/ ){

          if ( lwidth < 5 )
          {
            /* medium alignment - use next highest value instead */
            if ( loffset == 1 && offsetrank[0] > offsetrank[2] ) loffset = 0;

            if ( loffset % 3 == 0 ) shift = -1;
            /*if (loffset % 3 == 1 ) shift = 1;*/
            if ( loffset % 3 == 2 ) shift = 0;
          }
          else if (lwidth < 6 )
          {
            /* medium alignment - use next highest value instead */
            if ( loffset == 0 && offsetrank[2] > offsetrank[1] ) loffset = 2;
            /*if (loffset % 3 == 0 ) shift = 1;*/
            if ( loffset % 3 == 1 ) shift = 0;
            if ( loffset % 3 == 2 ) shift = -1;
          }
          else if ( lwidth < 20 )
          {
            /* medium alignment - use next highest value instead */
            if ( loffset == 0 && offsetrank[1] > offsetrank[2] ) loffset = 1;
            /*if (loffset % 3 == 0 ) shift = 1;*/
            if ( loffset % 3 == 1 ) shift = -1;
            if ( loffset % 3 == 2 ) shift = 0;
          }
        }

        /* medium alignment 2 - shift glyph ONLY RIGHT one subpixel
        * - a compromise to prevent spacing issues */
        else if ( alignment_type == 4 ){
          if ( lwidth < 5 )
          {
            /* medium alignment - use next highest value instead */
            if ( loffset == 0 && offsetrank[1] > offsetrank[2] ) loffset = 1;

            /*if ( loffset % 3 == 0 ) shift = -1;*/
            if ( loffset % 3 == 1 ) shift = 1;
            if ( loffset % 3 == 2 ) shift = 0;
          }
          else if ( lwidth < 6 )
          {
            /* medium alignment - use next highest value instead */
            if ( loffset == 2 && offsetrank[0] > offsetrank[1] ) loffset = 0;
            if ( loffset % 3 == 0 ) shift = 1;
            if ( loffset % 3 == 1 ) shift = 0;
            /*if ( loffset % 3 == 2 ) shift = -1;*/
          }
          else if ( lwidth < 20 )
          {
            /* medium alignment - use next highest value instead */
            if ( loffset == 1 && offsetrank[0] > offsetrank[2] ) loffset = 0;
            if ( loffset % 3 == 0 ) shift = 1;
            /*if (loffset % 3 == 1 ) shift = -1;*/
            if ( loffset % 3 == 2 ) shift = 0;
          }
        }

        /* light alignment - shift glyph ONLY LEFT one subpixel
        * - a compromise to prevent spacing issues */
        else if ( alignment_type == 3 ){
          if ( lwidth < 5 )
          {
            if ( loffset % 3 == 0 ) shift = -1;
            /*if ( loffset % 3 == 1 ) shift = 1;*/
            if ( loffset % 3 == 2 ) shift = 0;
          }
          else if ( lwidth < 6 )
          {
            /*if ( loffset % 3 == 0 ) shift = 1;*/
            if ( loffset % 3 == 1 ) shift = 0;
            if ( loffset % 3 == 2 ) shift = -1;
          }
          else if ( lwidth < 20 )
          {
            /*if ( loffset % 3 == 0 ) shift = 1;*/
            if ( loffset % 3 == 1 ) shift = -1;
            if ( loffset % 3 == 2 ) shift = 0;
          }
        }

        /* light alignment 2 - shift glyph ONLY RIGHT one subpixel
        * - a compromise to prevent spacing issues */
        else if ( alignment_type == 5 ){
          if ( lwidth < 5 )
          {
            /*if ( loffset % 3 == 0 ) shift = -1;*/
            if ( loffset % 3 == 1 ) shift = 1;
            if ( loffset % 3 == 2 ) shift = 0;
          }
          else if ( lwidth < 6 )
          {
            if ( loffset % 3 == 0 ) shift = 1;
            if ( loffset % 3 == 1 ) shift = 0;
            /*if ( loffset % 3 == 2 ) shift = -1;*/
          }
          else if ( lwidth < 20 )
          {
            if ( loffset % 3 == 0 ) shift = 1;
            /*if ( loffset % 3 == 1 ) shift = -1;*/
            if ( loffset % 3 == 2 ) shift = 0;
          }
        }

        /******************** ALIGN GLYPHS *********************/
        if (shift == -1)
        {
          line = bitmap->buffer;
          for ( height = (FT_UInt)bitmap->rows; height > 0; height--,
            line += bitmap->pitch )
          {
            FT_UInt  xx;

            for ( xx = 0; xx < width - 1; xx += 1 )
            {
              line[xx] = line[xx+1];
            }
            line[width - 1] = 1;
          }
        }

        else if (shift == -2)
        {
          line = bitmap->buffer;
          for ( height = (FT_UInt)bitmap->rows; height > 0; height--,
            line += bitmap->pitch )
          {
            FT_UInt  xx;

            for ( xx = 0; xx < width - 2; xx += 1 )
            {
              line[xx] = line[xx+2];
            }
            line[width - 2] = 1;
            line[width - 1] = 1;
          }
        }
        else if (shift == 1)
        {
          line = bitmap->buffer;
          for ( height = (FT_UInt)bitmap->rows; height > 0; height--,
            line += bitmap->pitch )
          {
            FT_UInt  xx;

            for ( xx = width - 1; xx > 0; xx -= 1 )
            {
              line[xx] = line[xx-1];
            }
            line[0] = 1;
          }
        }
        else if (shift == 2)
        {
          line = bitmap->buffer;
          for ( height = (FT_UInt)bitmap->rows; height > 0; height--,
            line += bitmap->pitch )
          {
            FT_UInt  xx;

            for ( xx = width; xx > 1; xx -= 1 )
            {
              line[xx] = line[xx-2];
            }
            line[0] = 1;
            line[1] = 1;
          }
        }
      }

      if ( pseudo_gamma_value != 1 )
      {
        FT_Byte*  line = bitmap->buffer;
        float ppem = (float)slot->face->size->metrics.x_ppem;

        if (ppem >= 5 )
        for (height = (FT_UInt)bitmap->rows; height > 0; height--, line += bitmap->pitch )
        {
          FT_UInt  xx;

          for ( xx = 0; xx < width; xx += 1 )
          {
            /*normal*/
            /*line[xx] = gamma2 ( line[xx], pseudo_gamma_value );*/

            /* sloped */
            /*line[xx] = gamma2 ( line[xx], pseudo_gamma_value - 5
            * (1-pseudo_gamma_value)/(pseudo_gamma_lt -5)
            + ((1-pseudo_gamma_value)/(pseudo_gamma_lt -5)) * ppem );*/

            /* 1/3-sloped */
            line[xx] = gamma2 ( line[xx], pseudo_gamma_value - 5
            * ((1-pseudo_gamma_value)/(3*(pseudo_gamma_lt -5)))
            * + ((1-pseudo_gamma_value)/(3*(pseudo_gamma_lt -5))) * ppem );
          }
        }
      }
    }
  }






/* define USE_LEGACY to implement the legacy filter */
#define  USE_LEGACY

  /* FIR filter used by the default and light filters */
  static void
  _ft_lcd_filter_fir( FT_Bitmap*      bitmap,
                      FT_Render_Mode  mode,
                      FT_Library      library )
  {
    FT_Byte*  weights = library->lcd_weights;
    FT_UInt   width   = (FT_UInt)bitmap->width;
    FT_UInt   height  = (FT_UInt)bitmap->rows;


    /* horizontal in-place FIR filter */
    if ( mode == FT_RENDER_MODE_LCD && width >= 4 )
    {
      FT_Byte*  line = bitmap->buffer;


      for ( ; height > 0; height--, line += bitmap->pitch )
      {
        FT_UInt  fir[5];
        FT_UInt  val1, xx;


        val1   = line[0];
        fir[0] = weights[2] * val1;
        fir[1] = weights[3] * val1;
        fir[2] = weights[4] * val1;
        fir[3] = 0;
        fir[4] = 0;

        val1    = line[1];
        fir[0] += weights[1] * val1;
        fir[1] += weights[2] * val1;
        fir[2] += weights[3] * val1;
        fir[3] += weights[4] * val1;

        for ( xx = 2; xx < width; xx++ )
        {
          FT_UInt  val, pix;


          val    = line[xx];
          pix    = fir[0] + weights[0] * val;
          fir[0] = fir[1] + weights[1] * val;
          fir[1] = fir[2] + weights[2] * val;
          fir[2] = fir[3] + weights[3] * val;
          fir[3] =          weights[4] * val;

          pix        >>= 8;
          pix         |= -( pix >> 8 );
          line[xx - 2] = (FT_Byte)pix;
        }

        {
          FT_UInt  pix;


          pix          = fir[0] >> 8;
          pix         |= -( pix >> 8 );
          line[xx - 2] = (FT_Byte)pix;

          pix          = fir[1] >> 8;
          pix         |= -( pix >> 8 );
          line[xx - 1] = (FT_Byte)pix;
        }
      }
    }

    /* vertical in-place FIR filter */
    else if ( mode == FT_RENDER_MODE_LCD_V && height >= 4 )
    {
      FT_Byte*  column = bitmap->buffer;
      FT_Int    pitch  = bitmap->pitch;


      for ( ; width > 0; width--, column++ )
      {
        FT_Byte*  col = column;
        FT_UInt   fir[5];
        FT_UInt   val1, yy;


        val1   = col[0];
        fir[0] = weights[2] * val1;
        fir[1] = weights[3] * val1;
        fir[2] = weights[4] * val1;
        fir[3] = 0;
        fir[4] = 0;
        col   += pitch;

        val1    = col[0];
        fir[0] += weights[1] * val1;
        fir[1] += weights[2] * val1;
        fir[2] += weights[3] * val1;
        fir[3] += weights[4] * val1;
        col    += pitch;

        for ( yy = 2; yy < height; yy++ )
        {
          FT_UInt  val, pix;


          val    = col[0];
          pix    = fir[0] + weights[0] * val;
          fir[0] = fir[1] + weights[1] * val;
          fir[1] = fir[2] + weights[2] * val;
          fir[2] = fir[3] + weights[3] * val;
          fir[3] =          weights[4] * val;

          pix           >>= 8;
          pix            |= -( pix >> 8 );
          col[-2 * pitch] = (FT_Byte)pix;
          col            += pitch;
        }

        {
          FT_UInt  pix;


          pix             = fir[0] >> 8;
          pix            |= -( pix >> 8 );
          col[-2 * pitch] = (FT_Byte)pix;

          pix         = fir[1] >> 8;
          pix        |= -( pix >> 8 );
          col[-pitch] = (FT_Byte)pix;
        }
      }
    }
  }


#ifdef USE_LEGACY

  /* intra-pixel filter used by the legacy filter */
  static void
  _ft_lcd_filter_legacy( FT_Bitmap*      bitmap,
                         FT_Render_Mode  mode,
                         FT_Library      library )
  {
    FT_UInt  width  = (FT_UInt)bitmap->width;
    FT_UInt  height = (FT_UInt)bitmap->rows;
    FT_Int   pitch  = bitmap->pitch;

    static const int  filters[3][3] =
    {
      { 65538 * 9/13, 65538 * 1/6, 65538 * 1/13 },
      { 65538 * 3/13, 65538 * 4/6, 65538 * 3/13 },
      { 65538 * 1/13, 65538 * 1/6, 65538 * 9/13 }
    };

    FT_UNUSED( library );


    /* horizontal in-place intra-pixel filter */
    if ( mode == FT_RENDER_MODE_LCD && width >= 3 )
    {
      FT_Byte*  line = bitmap->buffer;


      for ( ; height > 0; height--, line += pitch )
      {
        FT_UInt  xx;


        for ( xx = 0; xx < width; xx += 3 )
        {
          FT_UInt  r = 0;
          FT_UInt  g = 0;
          FT_UInt  b = 0;
          FT_UInt  p;


          p  = line[xx];
          r += filters[0][0] * p;
          g += filters[0][1] * p;
          b += filters[0][2] * p;

          p  = line[xx + 1];
          r += filters[1][0] * p;
          g += filters[1][1] * p;
          b += filters[1][2] * p;

          p  = line[xx + 2];
          r += filters[2][0] * p;
          g += filters[2][1] * p;
          b += filters[2][2] * p;

          line[xx]     = (FT_Byte)( r / 65536 );
          line[xx + 1] = (FT_Byte)( g / 65536 );
          line[xx + 2] = (FT_Byte)( b / 65536 );
        }
      }
    }
    else if ( mode == FT_RENDER_MODE_LCD_V && height >= 3 )
    {
      FT_Byte*  column = bitmap->buffer;


      for ( ; width > 0; width--, column++ )
      {
        FT_Byte*  col     = column;
        FT_Byte*  col_end = col + height * pitch;


        for ( ; col < col_end; col += 3 * pitch )
        {
          FT_UInt  r = 0;
          FT_UInt  g = 0;
          FT_UInt  b = 0;
          FT_UInt  p;


          p  = col[0];
          r += filters[0][0] * p;
          g += filters[0][1] * p;
          b += filters[0][2] * p;

          p  = col[pitch];
          r += filters[1][0] * p;
          g += filters[1][1] * p;
          b += filters[1][2] * p;

          p  = col[pitch * 2];
          r += filters[2][0] * p;
          g += filters[2][1] * p;
          b += filters[2][2] * p;

          col[0]         = (FT_Byte)( r / 65536 );
          col[pitch]     = (FT_Byte)( g / 65536 );
          col[2 * pitch] = (FT_Byte)( b / 65536 );
        }
      }
    }
  }

#endif /* USE_LEGACY */


  FT_EXPORT_DEF( FT_Error )
  FT_Library_SetLcdFilterWeights( FT_Library      library,
                                  unsigned char  *weights )
  {
    if ( !library || !weights )
      return FT_Err_Invalid_Argument;

    ft_memcpy( library->lcd_weights, weights, 5 );

    return FT_Err_Ok;
  }


  FT_EXPORT_DEF( FT_Error )
  FT_Library_SetLcdFilter( FT_Library    library,
                           FT_LcdFilter  filter )
  {
    static const FT_Byte  light_filter[5] =
                            { 0x00, 0x55, 0x56, 0x55, 0x00 };
    /* the values here sum up to a value larger than 256, */
    /* providing a cheap gamma correction                 */
    static FT_Byte  default_filter[5] =
                            { 0x10, 0x40, 0x70, 0x40, 0x10 };

    int checked_filter_params_env = 0;

    if ( checked_filter_params_env == 0 )
    {
      char *filter_params = getenv( "INFINALITY_FT_FILTER_PARAMS" );
      if ( filter_params != NULL && strcmp(filter_params, "") != 0 )
      {
        float f1, f2, f3, f4, f5;

        if ( strcasecmp(filter_params, "default" ) != 0)
        {
          int args_assigned = 0;
          args_assigned = sscanf ( filter_params, "%f %f %f %f %f", &f1, &f2, &f3, &f4, &f5 );

          if ( args_assigned == 5 )
          {
            default_filter[0] = (FT_Byte) (f1 * 255.0f + 0.5f);
            default_filter[1] = (FT_Byte) (f2 * 255.0f + 0.5f);
            default_filter[2] = (FT_Byte) (f3 * 255.0f + 0.5f);
            default_filter[3] = (FT_Byte) (f4 * 255.0f + 0.5f);
            default_filter[4] = (FT_Byte) (f5 * 255.0f + 0.5f);
          }
        }
      }
      checked_filter_params_env = 1;
    }

    if ( !library )
      return FT_Err_Invalid_Argument;

    switch ( filter )
    {
    case FT_LCD_FILTER_NONE:
      library->lcd_filter_func = NULL;
      library->lcd_extra       = 0;
      break;

    case FT_LCD_FILTER_DEFAULT:
#if defined( FT_FORCE_LEGACY_LCD_FILTER )

      library->lcd_stem_align_func = _lcd_stem_align;
      library->lcd_filter_func = _ft_lcd_filter_legacy;
      library->lcd_extra       = 0;

#elif defined( FT_FORCE_LIGHT_LCD_FILTER )

      library->lcd_stem_align_func = _lcd_stem_align;
      ft_memcpy( library->lcd_weights, light_filter, 5 );
      library->lcd_filter_func = _ft_lcd_filter_fir;
      library->lcd_extra       = 2;

#else

      library->lcd_stem_align_func = _lcd_stem_align;
      ft_memcpy( library->lcd_weights, default_filter, 5 );
      library->lcd_filter_func = _ft_lcd_filter_fir;
      library->lcd_extra       = 2;

#endif

      break;

    case FT_LCD_FILTER_LIGHT:
      ft_memcpy( library->lcd_weights, light_filter, 5 );
      library->lcd_stem_align_func = _lcd_stem_align;
      library->lcd_filter_func = _ft_lcd_filter_fir;
      library->lcd_extra       = 2;
      break;

#ifdef USE_LEGACY

    case FT_LCD_FILTER_LEGACY:
      library->lcd_stem_align_func = _lcd_stem_align;
      library->lcd_filter_func = _ft_lcd_filter_legacy;
      library->lcd_extra       = 0;
      break;

#endif

    default:
      return FT_Err_Invalid_Argument;
    }

    library->lcd_filter = filter;

    return FT_Err_Ok;
  }

#else /* !FT_CONFIG_OPTION_SUBPIXEL_RENDERING */

  FT_EXPORT_DEF( FT_Error )
  FT_Library_SetLcdFilterWeights( FT_Library      library,
                                  unsigned char  *weights )
  {
    FT_UNUSED( library );
    FT_UNUSED( weights );

    return FT_Err_Unimplemented_Feature;
  }


  FT_EXPORT_DEF( FT_Error )
  FT_Library_SetLcdFilter( FT_Library    library,
                           FT_LcdFilter  filter )
  {
    FT_UNUSED( library );
    FT_UNUSED( filter );

    return FT_Err_Unimplemented_Feature;
  }

#endif /* !FT_CONFIG_OPTION_SUBPIXEL_RENDERING */


/* END */
