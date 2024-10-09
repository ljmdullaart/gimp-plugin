/*
#INSTALL_C@ ~/.config/GIMP/2.10/plug-ins/plugin_youcam_enhance
#MAKE gimptool-2.0 --install plugin_youcam_enhance.c

*/


#include <libgimp/gimp.h>
#include <glib.h>  // For g_strdup_printf, g_spawn_command_line_sync
#include <unistd.h>  // For unlink()

// Declare a global variable for the JPEG file path
char *jpegfile;

static void query(void);
static void run(const gchar *name, gint nparams, const GimpParam *param, gint *nreturn_vals, GimpParam **return_vals);

GimpPlugInInfo PLUG_IN_INFO = {
	NULL,   // Init function (optional)
	NULL,   // Quit function (optional)
	query,  // Query function (required)
	run	 // Run function (required)
};

// Plugin query function
static void query(void) {
	static GimpParamDef args[] = {
		{ GIMP_PDB_INT32, "run_mode", "Run mode" },
		{ GIMP_PDB_IMAGE, "image", "Input image" },
		{ GIMP_PDB_DRAWABLE, "drawable", "Input drawable" }
	};

	gimp_install_procedure(
		"plug-in-youcam-enhance",		// Procedure name
		"Enhance the image using YouCam",  // Plugin description
		"Saves the current image as a JPEG file, runs the external YouCam enhancement script, and imports the result as a new layer", // Plugin help
		"Your Name",					 // Author
		"Your Name",					 // Copyright
		"2024",						  // Date
		"<Image>/Filters/Misc/Youcam Enhance", // Menu path
		"RGB*, GRAY*",				   // Image types this plugin works with
		GIMP_PLUGIN,					 // Plugin type
		G_N_ELEMENTS(args),			  // Number of input parameters
		0,							   // Number of output parameters
		args,							// Input parameter definitions
		NULL							 // No output parameters
	);
}

// Plugin run function
static void run(const gchar *name, gint nparams, const GimpParam *param, gint *nreturn_vals, GimpParam **return_vals) {
	gint32 image_id, drawable_id, new_layer_id;
	GError *error = NULL;
	gchar *stdout_output = NULL;
	gchar *stderr_output = NULL;
	gint exit_status;

	// Generate a unique temporary filename in /tmp
	jpegfile = g_strdup_printf("/tmp/tempfile-%d.jpg", g_random_int());

	// Get the image and drawable ID from the input parameters
	image_id = param[1].data.d_int32;
	drawable_id = param[2].data.d_int32;

	// Save the image to the specified file using gimp_file_save
	gboolean success = gimp_file_save(GIMP_RUN_NONINTERACTIVE, image_id, drawable_id, jpegfile, jpegfile);

	if (!success) {
		gimp_message("Error: Could not save the image as JPEG.");
		g_free(jpegfile);
		return;
	}

	// Run the external script with the JPEG file as an argument
	gchar *cmd = g_strdup_printf("/usr/local/bin/gimp.youcam.enhance %s", jpegfile);

	// Execute the command and capture stdout and stderr
	success = g_spawn_command_line_sync(cmd, &stdout_output, &stderr_output, &exit_status, &error);

	// Check if the command ran successfully
	if (!success) {
		gimp_message("Error: Could not run the enhancement script.");
	} else {
		// Display the output from stdout (if any) in the GIMP message bar
		if (stdout_output && *stdout_output) {
			gimp_message(stdout_output);
		}

		// Optionally, handle stderr output
		if (stderr_output && *stderr_output) {
			gimp_message(stderr_output);
		}

		// Load the enhanced JPEG as a new layer
		new_layer_id = gimp_file_load_layer(GIMP_RUN_NONINTERACTIVE, image_id, jpegfile);

		if (new_layer_id != -1) {
			// Add the new layer to the image
			gimp_image_insert_layer(image_id, new_layer_id, -1, -1);
		} else {
			gimp_message("Error: Could not load the enhanced image as a new layer.");
		}

		// Delete the temporary file after importing the layer
		unlink(jpegfile);  // Use standard POSIX unlink to remove the file
	}

	// Free resources
	g_free(jpegfile);
	g_free(cmd);
	g_free(stdout_output);
	g_free(stderr_output);

	// No return values to set
	*nreturn_vals = 0;
}

// Main entry point
MAIN();

