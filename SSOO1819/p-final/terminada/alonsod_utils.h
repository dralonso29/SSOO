int split_by(char *line, char *delim, char *tokens[]);
void set_null(char *array[], int len);
int isbuilt_in(char *command);
void exec_built_in(char *commands);
void create_pipes(int fd[][2], int numpipes);
void closeall(int fd[][2], int n, int extr);
int pos_on_string(char *str, int len, char c);
int the_lower(int n1, int n2);
int appear_ampersand(char *line);
int is_listed( int number, int list[], int len);
