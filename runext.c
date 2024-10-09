#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    /* Command to run the external script */
    const char *command = "/usr/local/bin/youcam-credits";

    /* Run the command and capture the output */
    FILE *fp;
    char output[1024];

    /* Open the command for reading */
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run script");
        return (0);
    }

    /* Read the first line of the output */
    if (fgets(output, sizeof(output), fp) != NULL) {
        /* Remove newline character if present */
        size_t len = strlen(output);
        if (len > 0 && output[len - 1] == '\n') {
            output[len - 1] = '\0';
        }
        /* Display the output in a message */
        printf("Script output: %s", output);
    } else {
        printf("No output from script");
    }

    /* Close the command stream */
    pclose(fp);
    return (0);
}

