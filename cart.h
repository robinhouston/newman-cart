/* Header file for cartogram.c
 *
 * Written by Mark Newman
 *
 * See http://www.umich.edu/~mejn/ for further details.
 */

#ifndef _CART_H
#define _CART_H

#define FALSE 0
#define TRUE 1

typedef enum {NONE, NORMAL, PERCENT, DETAILED} progress_mode_t;
typedef struct {
  progress_mode_t progress_mode;
  int intermediate;
  char *output_filename;
  double max_h;
} options_t;
#define DEFAULT_OPTIONS { NORMAL, FALSE, NULL, INFINITY }

double** cart_dmalloc(int xsize, int ysize);
void cart_dfree(double **userrho);
void cart_makews(int xsize, int ysize);
void cart_freews(int xsize, int ysize);
void cart_transform(double **userrho, int xsize, int ysize);
void cart_makecart(double *pointx, double *pointy, int npoints,
		   int xsize, int ysize, double blur, options_t *options);

#endif
