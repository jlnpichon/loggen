#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "names.c"

#include "markov.h"

#define ARRAY_SIZE(a) ((sizeof((a))) / (sizeof((a)[0])))

struct user {
    char *name;
    char gender;
    char ip[16];
};

struct action_type {
  char *name;
  int data;
} action_types[] = {
  { .name = "buy", .data = 0},
  { .name = "comment", .data = 1},
  { .name = "like", .data = 0},
  { .name = "dislike", .data = 0},
  { .name = "post", .data = 1}
};

struct action {
    char *name;
    char *data;
};

struct user users[1024];
struct action actions[1024];

static struct {
    struct user *user;
    struct action *action;
} users_actions[8192];

static void iprand(char *str, int len)
{
    int n;
    int written;

    n = (rand() % 255) + 1;
    written = sprintf(str, "%d.", n);

    str += written;
    n = (rand() % 255) + 1;
    written = sprintf(str, "%d.", n);

    str += written;
    n = (rand() % 255) + 1;
    written = sprintf(str, "%d.", n);

    str += written;
    n = (rand() % 255) + 1;
    written = sprintf(str, "%d", n);
}

char *strtolower(char *str, int len)
{
    int i;

    for (i = 0; str[i] != '\0' && i < len; i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

void emit_users(void)
{
    int i;
    int n;
    char *name;

    for (i = 0; i < 1024; i++) {
        if (rand() % 2 == 0) {
            n = rand() % ARRAY_SIZE(male_names);
            name = male_names[n];
            users[i].gender = 'm';
        } else {
            n = rand() % ARRAY_SIZE(female_names);
            name = female_names[n];
            users[i].gender = 'w';
        }
        users[i].name = name;
        iprand(users[i].ip, 16);
    }
}

void emit_actions(void)
{
    int i;
    int n;

    for (i = 0; i < 1024; i++) {
        n = (rand () % (ARRAY_SIZE(action_types) - 1)) + 1;
        actions[i].name = action_types[n].name;
        if (action_types[n].data) {
            char *sentence;

            sentence = calloc(4096, sizeof(char));
            generate_sentence(sentence, 4096, (rand() % 100) + 100);
            actions[i].data = sentence;
        }
    }
}

void add_name(char *line, int len, char **names, int *names_count)
{
    char *s;
    char *ptr;

    s = strtok_r(line, " ", &ptr);
    if (s == NULL) {
        fprintf(stderr, "Error parsing line '%s': %s\n",
                line, strerror(errno));
        return;
    }
    if (*names_count >= 8192) {
        fprintf(stderr, "Maximum name count is reached\n");
        return;
    }
    names[*names_count] = strdup(s);
    (*names_count)++;
}

void load_names_from_file(char *filename, char **names, int *names_count)
{
    FILE *f;
    char *line;
    int line_len;

    if (filename == NULL || names == NULL)
        return;

    f = fopen(filename, "r");
    if (f == NULL) {
        fprintf(stderr, "Error opening file '%s': %s\n",
                filename, strerror(errno));
        return;
    }

    line = calloc(sizeof(char), 1024);
    line_len = 1024;
    while (1) {
        fgets(line, line_len, f);
        if (feof(f))
            break;
        if (line[strlen(line) - 1] != '\n') {
            line_len *= 2;
            line = realloc(line, line_len);
            if (line == NULL) {
                fprintf(stderr, "realloc fail\n");
                return;
            }
            continue;
        }
        add_name(line, line_len, names, names_count);
    }
    free(line);
    if (ferror(f)) {
        fprintf(stderr, "Error reading file '%s': %s\n",
                filename, strerror(errno));
        return;
    }
    fclose(f);
}

static void dump(FILE *stream)
{
    int i;

    for (i = 0; i < 8192; i++) {
        char strtime[200];
        time_t t;
        struct tm *lt;

        t = time(NULL);
        /* add or susbtract */
        if ((rand() % 2) == 0)
            t = t - (rand() % 604800) /* one week */;
        else
            t = t + (rand() % 604800) /* one week */;
        lt = localtime(&t);

        fprintf(stream, "{");
        fprintf(stream, "\"action\": \"%s\", ", users_actions[i].action->name);
        if (users_actions[i].action->data != NULL)
            fprintf(stream, "\"data\": \"%s\", ", users_actions[i].action->data);
        fprintf(stream, "\"login\": \"%s\", ", users_actions[i].user->name);
        fprintf(stream, "\"gender\": \"%c\", ", users_actions[i].user->gender);
        fprintf(stream, "\"ip\": \"%s\", ", users_actions[i].user->ip);
        strftime(strtime, sizeof(strtime), "%s", lt);
        fprintf(stream, "\"timestamp\": \"%s\"", strtime);
        fprintf(stream, "}\n");
    }
}

static void emit_users_actions(void)
{
    int i;
    int user;
    int action;

    for (i = 0; i < 8192; i++) {
        user = (rand() % 1023) + 1;
        action = (rand() % 1023) + 1;
        users_actions[i].user = &users[user];
        users_actions[i].action = &actions[action];
    }
}

int main(int argc, char *argv[])
{
    int i;

    srand(time(NULL));
    markov_init();

    emit_users();
    emit_actions();
    emit_users_actions();

    dump(stdout);

    for (i = 0; i < 1024; i++) {
        if (actions[i].data != NULL)
            free(actions[i].data);
    }
    return 0;
}
