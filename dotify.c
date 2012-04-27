/*
 * dotify.c - Gimp plugin to dither an image.
 * Pretty redundant, as the effect can also be achieved
 * with existing plugins/filters.
 *
 * Andreas Textor <textor.andreas@googlemail.com>
 */
#include <libgimp/gimp.h>
#include <stdlib.h>

void run(const gchar *name,
		gint nparams,
		const GimpParam *param,
		gint *nreturn_vals,
		GimpParam **return_vals);
static void query();

GimpPlugInInfo PLUG_IN_INFO = {
	NULL, //init
	NULL, //quit
	query,
	run
};

MAIN()

static void blur (GimpDrawable *drawable) {
	gint i, j, k, channels;
	gint x1, y1, x2, y2;
	GimpPixelRgn rgn_in, rgn_out;
	guchar *row1, *row2, *row3;
	guchar *outrow;

	gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
	channels = gimp_drawable_bpp (drawable->drawable_id);

	gimp_pixel_rgn_init (&rgn_in, drawable, x1, y1, x2 - x1, y2 - y1, FALSE, FALSE);
	gimp_pixel_rgn_init (&rgn_out, drawable, x1, y1, x2 - x1, y2 - y1, TRUE, TRUE);

	/* Initialise enough memory for row1, row2, row3, outrow */
	row1 = g_new (guchar, channels * (x2 - x1));
	outrow = g_new (guchar, channels * (x2 - x1));

	for (i = y1; i < y2; i++) {
		/* Get row i-1, i, i+1 */
		gimp_pixel_rgn_get_row (&rgn_in, row1, x1, MAX (y1, i - 1), x2 - x1);

		for (j = x1; j < x2; j++) {
			/* For each layer, compute the average of the nine pixels */
			for (k = 0; k < channels; k++) {
				int in = row1[channels * (j - x1) + k];
				int a = (1.0 - (in / 255.0)) * 100.0;
				int r = rand() % 100;
				outrow[channels * (j - x1) + k] = r < a ? 0 : 255;
			}
		}

		gimp_pixel_rgn_set_row (&rgn_out, outrow, x1, i, x2 - x1);

		if (i % 10 == 0)
			gimp_progress_update ((gdouble) (i - y1) / (gdouble) (y2 - y1));
	}

	g_free (row1);
	g_free (outrow);

	gimp_drawable_flush (drawable);
	gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
	gimp_drawable_update (drawable->drawable_id, x1, y1, x2 - x1, y2 - y1);
}

void run(const gchar *name,
		gint nparams,
		const GimpParam *param,
		gint *nreturn_vals,
		GimpParam **return_vals) {
	static GimpParam values[1];
	GimpPDBStatusType status = GIMP_PDB_SUCCESS;
	GimpRunMode run_mode;
	GimpDrawable *drawable;

	/* Setting mandatory output values */
	*nreturn_vals = 1;
	*return_vals = values;

	values[0].type = GIMP_PDB_STATUS;
	values[0].data.d_status = status;

	/* Getting run_mode - we won't display a dialog if 
	* we are in NONINTERACTIVE mode */
	run_mode = param[0].data.d_int32;

	/*  Get the specified drawable  */
	drawable = gimp_drawable_get(param[2].data.d_drawable);

	gimp_progress_init("Dotify...");

	/* Let's time blur
	*
	*   GTimer timer = g_timer_new time();
	*/
	srand(time(NULL));

	blur(drawable);

	gimp_displays_flush();
	gimp_drawable_detach(drawable);
}


static void query() {
	static GimpParamDef args[] = {
		{ GIMP_PDB_INT32, "run-mode", "Run mode" },
		{ GIMP_PDB_IMAGE, "image", "Input image" },
		{ GIMP_PDB_DRAWABLE, "drawable", "Input drawable" }
	};

	gimp_install_procedure(
		"plug-in-dotify",
		"Dotify",
		"Creates the appearance of an image drawn by black dots only",
		"Andreas Textor",
		"Copyright Andreas Textor",
		"2009",
		"_Dotify...",
		"GRAY*",
		GIMP_PLUGIN,
		G_N_ELEMENTS(args), 0,
		args, NULL);

	gimp_plugin_menu_register("plug-in-dotify", "<Image>/Filters/Distorts"); 
}

