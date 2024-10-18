#include <time.h>
#include <sys/stat.h>
#include <stdio.h>

#include <config.h>
#include <filter.h>
#include <utils/utils.h>
#include <utils/linked_list.h>
#include <utils/types.h>
#include <utils/reads.h>

bool file_exists(const char *filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0;
}

char *timestamp2str(const struct tm *time_info)
{
    char *time_str = (char *)malloc(100);

    sprintf(time_str, "%02d.%02d.%02d_%02d.%02d.%02d",
            time_info->tm_year - 100, time_info->tm_mon + 1, time_info->tm_mday,
            time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    return time_str;
}

FILE *load_orbit_file(const struct electron_orbit *orbit, char *timestamp)
{
    const size_t len = strlen(timestamp);
    char *orbit_path = (char *)malloc(len + 100);

    if (orbit->magnetic == -1)
    {

        sprintf(orbit_path, "%s/%s/results_N%hi/results_K%hi.txt", RESULT_P,
                timestamp, orbit->principal, orbit->angular);
    }
    else
    {

        sprintf(orbit_path, "%s/%s/results_N%hi/results_K%hi/results_M%hi.txt",
                RESULT_P, timestamp, orbit->principal, orbit->angular, orbit->magnetic);
    }

    FILE *logFile = fopen(orbit_path, "w");

    free(orbit_path);
    return logFile;
}

bool store_metadata(const char *path)
{

    FILE *config_f = fopen(CONFIG_PATH, "r");
    if (config_f == NULL)
    {
        printf("ERROR opening Config file");
        return false;
    }

    FILE *metadata_file = fopen(path, "w");

    if (metadata_file == NULL)
    {
        printf("ERROR opening meta file");
        fclose(config_f);
        return false;
    }

    char *curr_line;

    while ((curr_line = readline(config_f)) != NULL)
    {
        fprintf(metadata_file, "%s\n", curr_line);
        free(curr_line);
    }

    fclose(config_f);
    fclose(metadata_file);
    return true;
}

int make_dir(const char *path)
{
#ifdef __linux__
    return mkdir(path, 0777);
#else
    return _mkdir(path);
#endif
}

char *construct_result_path(struct linked_list *filterList)
{
    time_t raw_time;
    struct tm time_info;
    char result_path[200];
    memset(result_path, 0, 200);

    time(&raw_time);
    localtime_r(&raw_time, &time_info);
    char *time_str = timestamp2str(&time_info);

    int last_n = 0;
    int last_k = 0;
    size_t list_size = linked_list_size(filterList);

    if (!file_exists(RESULT_P) && make_dir(RESULT_P) == -1)
    {
        printf("error creating file");
        free(time_str);
        return NULL;
        
    }

    sprintf(result_path, "%s/%s", RESULT_P, time_str);
    if (make_dir(result_path) == -1)
    {
        printf("error creating file");
        free(time_str);
        return NULL;
    }

    for (int i = 0; i < list_size; i++)
    {
        struct electron_orbit *orbit = (struct electron_orbit *)linked_list_pop(filterList);

        if (orbit->principal > last_n)
        {
            sprintf(result_path, "%s/%s/results_N%hi", RESULT_P, time_str, orbit->principal);

            if (make_dir(result_path) == -1)
            {
                printf("error creating file");
                free(time_str);
                return NULL;
            }

            last_k = 0;
            last_n = orbit->principal;
        }
        if (orbit->angular > last_k && orbit->magnetic != -1)
        {
            sprintf(result_path, "%s/%s/results_N%hi/results_K%hi", RESULT_P, time_str, orbit->principal, orbit->angular);

            if (make_dir(result_path) == -1)
            {
                printf("error creating file");
                free(time_str);
                return NULL;
            }

            last_k = orbit->angular;
        }

        linked_list_append(filterList, orbit);
    }

    return time_str;
}

struct config *load_config(const char *config_path)
{
    FILE *config_f = fopen(config_path, "r");
    if (config_f == NULL)
    {
        perror("ERROR opening Config file");
        exit(1);
    }

    int length = -1;
    char **configLines = read_lines(config_f, &length);

    struct config *config = (struct config *)malloc(sizeof(struct config));
    memset(config, 0, sizeof(struct config));

    const char *curr_line = readline(config_f);
    for (int i = 0; i < length; i++)
    {

        if (strstr(curr_line, "revolutions =") == curr_line)
        {
            config->revolutions = strtod(curr_line + 13, NULL);
        }
        else if (strstr(curr_line, "iterationMode =") == curr_line)
        {
            config->itr_mode = (bool)strtol(curr_line + 15, NULL, 10);
        }
        else if (strstr(curr_line, "deltaPsiMode =") == curr_line)
        {
            config->delta_psi_mode = (bool)strtol(curr_line + 14, NULL, 10);
        }
        else if (strstr(curr_line, "itrs =") == curr_line)
        {
            config->iters = atoi(curr_line + 6);
        }
        else if (strstr(curr_line, "Hbar =") == curr_line)
        {
            config->Hbar = strtod(curr_line + 6, NULL);
        }
        else if (strstr(curr_line, "charge =") == curr_line)
        {
            config->electron_charge = strtod(curr_line + 8, NULL);
        }
        else if (strstr(curr_line, "mass =") == curr_line)
        {
            config->electron_mass = strtod(curr_line + 6, NULL);
        }
        else if (strstr(curr_line, "t =") == curr_line)
        {
            config->time_interval = strtod(curr_line + 4, NULL);
        }
        else if (strstr(curr_line, "Type =") == curr_line)
        {
            config->type = (enum sim_type)strtol(curr_line + 6, NULL, 10);
        }
        else if (strstr(curr_line, "logPerod =") == curr_line)
        {
            config->log_p = atoi(curr_line + 10);
        }
    }

    config->filter_list = get_filter_list(config->type);

    config->timestamp = construct_result_path(config->filter_list);

    size_t list_size = linked_list_size(config->filter_list);

    config->log_files = linked_list_new();

    for (int i = 0; i < list_size; i++)
    {
        struct electron_orbit *orbit = (struct electron_orbit *)linked_list_pop(config->filter_list);

        FILE *log_file = load_orbit_file(orbit, config->timestamp);

        linked_list_append(config->log_files, log_file);
        linked_list_append(config->filter_list, orbit);
    }

    fclose(config_f);

    config_f = fopen(CONFIG_PATH, "w");
    if (config_f == NULL)
    {
        perror("ERROR opening Config file");
        exit(1);
    }

    for (int i = 0; i < length; i++)
    {
        if (strstr(configLines[i], "TimeStamp =") != NULL)
        {
            fprintf(config_f, "TimeStamp =%s\n", config->timestamp);
        }
        else
        {
            fprintf(config_f, "%s\n", configLines[i]);
        }
        free(configLines[i]);
    }

    free(configLines);
    fclose(config_f);
    char meta_filepath[100];
    sprintf(meta_filepath, "%s/%s/meta.txt", RESULT_P, config->timestamp);

    if (!store_metadata(meta_filepath))
    {
        printf("error saving meta data");
        free(config->timestamp);
        free(config);
        return NULL;
    }

    return config;
}