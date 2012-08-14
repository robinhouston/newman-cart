/* Example program to calculate a grid of points for a Gastner-Newman
 * cartogram using the cartogram.c code
 *
 * Written by Mark Newman
 *
 * See http://www.umich.edu/~mejn/ for further details.
 */


#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cart.h"


/* The parameter OFFSET specifies a small amount to be added the density in
 * every grid square, as a Fraction of the mean density on the whole
 * lattice.  This prevents negative densities from being generated by
 * numerical errors in the FFTs which can cause problems for the
 * integrator.  If the program is giving weird behavior, particularly at
 * the beginning of a calculation, try increasing this quantity by a factor
 * of 10.
 */

#define OFFSET 0.005


/* Function to read population data into the array rho.  Returns 1 if there
 * was a problem, zero otherwise */

int readpop(FILE *stream, double **rho, int xsize, int ysize)
{
  int ix,iy;
  int n;
  double mean;
  double sum=0.0;

  for (iy=0; iy<ysize; iy++) {
    for (ix=0; ix<xsize; ix++) {
      n = fscanf(stream,"%lf",&rho[ix][iy]);
      if (n!=1) return 1;
      sum += rho[ix][iy];
    }
  }

  mean = sum/(xsize*ysize);
  for (iy=0; iy<ysize; iy++) {
    for (ix=0; ix<xsize; ix++) {
      rho[ix][iy] += OFFSET*mean;
    }
  }

  return 0;
}


/* Function to make the grid of points */

void creategrid(double *gridx, double *gridy, int xsize, int ysize)
{
  int ix,iy;
  int i;

  for (iy=0,i=0; iy<=ysize; iy++) {
    for (ix=0; ix<=xsize; ix++) {
      gridx[i] = ix;
      gridy[i] = iy;
      i++;
    }
  }
}


/* Function to write out the grid points */

void writepoints(FILE *stream, double *gridx, double *gridy, int npoints)
{
  int i;

  for (i=0; i<npoints; i++) fprintf(stream,"%g %g\n",gridx[i],gridy[i]);
}

static void usage(char *program_name)
{
    fprintf(stderr,"Usage: %s xsize ysize inputfile outputfile\n", program_name);
    exit(1);
}

main(int argc, char *argv[])
{
  int xsize,ysize;
  double *gridx,*gridy;  // Array for grid points
  double **rho;          // Initial population density
  FILE *infp;
  FILE *outfp;
  char *end_ptr;

  /* Read the command-line parameters and open the input and output files */
  /* options descriptor */
  static struct option longopts[] = {
         { "progress", required_argument, NULL, 'p' }, /* How to display progress */
         { "intermediate", no_argument, NULL, 'i' },   /* Store intermediate density grids */
         { "max-h", required_argument, NULL, 'm' },   /* Cap on step size */
         { NULL, 0, NULL, 0 }
  };
  options_t options = DEFAULT_OPTIONS;
  char ch;

  while ((ch = getopt_long(argc, argv, "p:m:i", longopts, NULL)) != -1)
    switch (ch) {
      case 'p':
        if (0 == strcmp("none", optarg))
          options.progress_mode = NONE;
        else if (0 == strcmp("normal", optarg))
          options.progress_mode = NORMAL;
        else if (0 == strcmp("percent", optarg))
          options.progress_mode = PERCENT;
        else if (0 == strcmp("detailed", optarg))
          options.progress_mode = DETAILED;
        else {
          fprintf(stderr, "Unrecognised option --progress=%s\n", optarg);
          usage(argv[0]);
        }
        break;
      case 'i':
        options.intermediate = TRUE;
        break;
      case 'm':
        options.max_h = strtod(optarg, &end_ptr);
        if (*end_ptr != '\0') {
          fprintf(stderr, "Failed to parse number --max-h=%s\n", optarg);
          usage(argv[0]);
        }
        break;
      case 0:
        break;
      default:
        usage(argv[0]);
    }
  
  if ( !isatty(1) )
    setbuf(stdout,NULL);
  
  if (argc != optind+4) usage(argv[0]);
  
  xsize = atoi(argv[optind+0]);
  if (xsize<1) usage(argv[0]);
  
  ysize = atoi(argv[optind+1]);
  if (ysize<1) usage(argv[0]);
  
  infp = fopen(argv[optind+2],"r");
  if (infp==NULL) {
    fprintf(stderr,"%s: unable to open file '%s': ",argv[0], argv[optind+3]);
    perror(NULL);
    exit(4);
  }
  
  options.output_filename = argv[optind+3];
  outfp = fopen(options.output_filename,"w");
  if (outfp==NULL) {
    fprintf(stderr,"%s: unable to open file '%s': ",argv[0],argv[optind+4]);
    perror(NULL);
    exit(5);
  }

  /* Allocate space for the cartogram code to use */

  cart_makews(xsize,ysize);

  /* Read in the population data, transform it, then destroy it again */

  rho = cart_dmalloc(xsize,ysize);
  if (readpop(infp,rho,xsize,ysize)) {
    fprintf(stderr,"%s: density file contains too few or incorrect data\n",
	    argv[0]);
    exit(6);
  }
  cart_transform(rho,xsize,ysize);
  cart_dfree(rho);

  /* Create the grid of points */

  gridx = malloc((xsize+1)*(ysize+1)*sizeof(double));
  gridy = malloc((xsize+1)*(ysize+1)*sizeof(double));
  creategrid(gridx,gridy,xsize,ysize);

  /* Make the cartogram */

  cart_makecart(gridx,gridy,(xsize+1)*(ysize+1),xsize,ysize,0.0,&options);

  /* Write out the final positions of the grid points */

  writepoints(outfp,gridx,gridy,(xsize+1)*(ysize+1));

  /* Free up the allocated space */

  cart_freews(xsize,ysize);
  free(gridx);
  free(gridy);

  /* Close the input and output files */

  fclose(infp);
  fclose(outfp);
}
