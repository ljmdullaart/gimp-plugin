#include <libgimp/gimp.h>
#include <stdio.h>
#include <stdlib.h>

/*
#INSTALL_C@ ~/.config/GIMP/2.10/plug-ins/plugin_youcam_credtis
#MAKE gimptool-2.0 --install plugin_youcam_credtis.c

*/

static void query(void);
static void run(const gchar *name, gint nparams, const GimpParam *param,
                gint *nreturn_vals, GimpParam **return_vals);

GimpPlugInInfo PLUG_IN_INFO = {
    NULL,  /* init_proc  */
    NULL,  /* quit_proc  */
    query, /* query_proc */
    run,   /* run_proc   */
};

MAIN()

static void query(void) {
    static GimpParamDef args[] = {
        { GIMP_PDB_INT32, "run-mode", "Run mode" },
        { GIMP_PDB_IMAGE, "image", "Input image" },
        { GIMP_PDB_DRAWABLE, "drawable", "Input drawable" }
    };

    gimp_install_procedure(
        "plugin_youcam_credits",
        "Displays the credits in youcam",
        "Script that runs /usr/local/bin/gimp.youcam.credits and displays the credits",
        "ljm",
        "GPLv3 License",
        "2024",
        "_Check Youcam Credits",
        "RGB*, GRAY*",
        GIMP_PLUGIN,
        G_N_ELEMENTS(args),  /* number of parameters */
        0,                   /* number of return values */
        args,                /* parameters */
        NULL                 /* return values */
    );

	gimp_plugin_menu_register("plugin_youcam_credits", "<Image>/Filters/Misc");
}

static void run(const gchar *name, gint nparams, const GimpParam *param,
                gint *nreturn_vals, GimpParam **return_vals) {
    static GimpParam return_vals_local[1];
    GimpRunMode run_mode;

    /* Setting return values */
    *nreturn_vals = 1;
    *return_vals = return_vals_local;
    return_vals_local[0].type = GIMP_PDB_STATUS;
    return_vals_local[0].data.d_status = GIMP_PDB_SUCCESS;

    /* Extracting parameters */
    run_mode = param[0].data.d_int32;

    /* Command to run the external script */
    const char *command = "/usr/local/bin/gimp.youcam.credits";

    /* Run the command and capture the output */
    FILE *fp;
    char output[1024];

    /* Open the command for reading */
    fp = popen(command, "r");
    if (fp == NULL) {
        g_message("Failed to run script");
        return;
    }

    /* Read the first line of the output */
    if (fgets(output, sizeof(output), fp) != NULL) {
        /* Remove newline character if present */
        size_t len = strlen(output);
        if (len > 0 && output[len - 1] == '\n') {
            output[len - 1] = '\0';
        }
        /* Display the output in a message */
        g_message("%s", output);
    } else {
        g_message("No output from script");
    }

    /* Close the command stream */
    pclose(fp);
}

