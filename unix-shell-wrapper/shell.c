#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CMD_LINE_ARGS 128
// token types
#define EXECUTE 1
#define REDIRECT 2
#define PIPE 3
#define CHAIN 4
// type defintions and signatures
typedef struct cmd {
  int type;
} cmd;
typedef struct History {
  char **commands;
  int max_size;
  int length;
  int begin;
} history;
// incomplete list of function signature
int peek(char **, char *, char *);
int tokenize(char **, char *, char **, char **);
cmd *parse_pipe(char **, char *);
cmd *parse_line(char **, char *);
cmd *parse_exec(char **, char *);
cmd *parse_redirs(cmd *, char **, char *);
cmd *parse_expression(char **, char *);
// nodes for the parse tree
// derived token nodes and their constructors
struct exec_token {
  int type;
  char *argv[MAX_CMD_LINE_ARGS];
  char *oargv[10];
};
// execution program given name and options/arguments
cmd *exec_token(void) {
  struct exec_token *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = EXECUTE;
  return (struct cmd *)cmd;
}
// redirect output between processes
struct redir_token {
  int type;
  struct cmd *cmd;
  char *file;
  char *efile;
  int mode;
  int fd;
};
cmd *redir_token(cmd *subcommand, char *file, char *efile, int mode, int fd) {
  struct redir_token *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = REDIRECT;
  cmd->cmd = subcommand;
  cmd->file = file;
  cmd->efile = efile;
  cmd->mode = mode;
  cmd->fd = fd;
  return (struct cmd *)cmd;
}
// pip node for building subprocesses when a pipe character '|' is encountered
struct pipe_token {
  int type;
  struct cmd *left;
  struct cmd *right;
};
cmd *pipe_token(cmd *left, cmd *right) {
  struct pipe_token *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = PIPE;
  cmd->left = left;
  cmd->right = right;
  return (struct cmd *)cmd;
}
// for multiple commands
struct chain_token {
  int type;
  struct cmd *left;
  struct cmd *right;
};
cmd *chain_token(cmd *left_ptr, cmd *right_ptr) {
  struct chain_token *cmd;
  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = CHAIN;
  cmd->left = left_ptr;
  cmd->right = right_ptr;
  return (struct cmd *)cmd;
}
// parse_line will call these depending on what tokens it encounters
// parse the input from stdin given a reference to the end char
cmd *parse_line(char **start_ptr, char *end_ptr) {
  cmd *cmd = parse_pipe(start_ptr, end_ptr);
  if (peek(start_ptr, end_ptr, ";")) {
    tokenize(start_ptr, end_ptr, 0, 0);
    cmd = chain_token(cmd, parse_line(start_ptr, end_ptr));
  }
  return cmd;
}
// I spent so much time on the parser and the pipe that I couldn't quite get
// this working sorry for the messy code
history *initialize_history() {
  history *log;
  log = malloc(sizeof(history));
  memset(log, 0, sizeof(history));
  log->length = 0;
  log->max_size = BUFSIZ;
  log->commands = malloc(BUFSIZ * sizeof(char *));
  int i;
  for (i = 0; i < BUFSIZ; i++) {
    log->commands[i] = NULL;
  }
  return (history *)log;
}
// add new char pointers to the array of commands
void add_to_history(history *log, char *command) {
  if (log->commands[log->begin] != NULL) {
    free(log->commands[log->begin]);
  }
  log->commands[log->begin] = command;
  log->begin = (log->begin + 1) % log->max_size;
  log->length++;
}
// doesn't quite work like I thought
void print_history(history *log) {
  int i;
  for (i = 0; i < log->length; i++) {
    if (log->commands[i] != NULL) {
      printf("%s\n", log->commands[i]);
    }
  }
}
// at least there's no memory leaks? ...there probaly is :|
void free_history(history *log) {
  int i;
  for (i = 0; i < log->length; ++i) {
    if (log->commands[i] != NULL) {
      free(log->commands[i]);
    }
  }
  free(log->commands);
}
// parse pipe_tokens
cmd *parse_pipe(char **start_ptr, char *end_ptr) {
  cmd *cmd = parse_exec(start_ptr, end_ptr);
  if (peek(start_ptr, end_ptr, "|")) {
    // consume the token and buile a pipe node that
    // points to the previous command on the left and
    // anything right of the pipe gets parsed recursively
    tokenize(start_ptr, end_ptr, 0, 0);
    cmd = pipe_token(cmd, parse_pipe(start_ptr, end_ptr));
  }
  // returns a parse tree/branch
  return cmd;
}
// parse redirect_tokens
cmd *parse_redirs(cmd *cmd, char **start_ptr, char *end_ptr) {
  int token;
  char *q, *eq;
  while (peek(start_ptr, end_ptr, "<>")) {
    token = tokenize(start_ptr, end_ptr, 0, 0);
    if (tokenize(start_ptr, end_ptr, &q, &eq) != 't') {
      fprintf(stderr, "Missing file for redirection!");
    }
    switch (token) {
    case '<':
      cmd = redir_token(cmd, q, eq, O_RDONLY, 0);
      break;
    case '>':
      cmd = redir_token(cmd, q, eq, O_WRONLY | O_CREAT | O_TRUNC, 1);
    }
  }
  return cmd;
}
// parse expressions
cmd *parse_expression(char **start_ptr, char *end_ptr) {
  cmd *cmd;
  if (!peek(start_ptr, end_ptr, "("))
    fprintf(stderr, "Too many parentheses!");
  tokenize(start_ptr, end_ptr, 0, 0);
  cmd = parse_line(start_ptr, end_ptr);
  if (!peek(start_ptr, end_ptr, ")"))
    fprintf(stderr, "Missing matching right parenthese: \")\"");
  tokenize(start_ptr, end_ptr, 0, 0);
  cmd = parse_redirs(cmd, start_ptr, end_ptr);
  return cmd;
}
// parse executable commands
cmd *parse_exec(char **start_ptr, char *end_ptr) {
  char *q, *eq;
  int token, argc;
  struct exec_token *cmd;
  struct cmd *root;

  if (peek(start_ptr, end_ptr, "("))
    return parse_expression(start_ptr, end_ptr);
  root = exec_token();
  cmd = (struct exec_token *)root;

  argc = 0;
  root = parse_redirs(root, start_ptr, end_ptr);
  while (!peek(start_ptr, end_ptr, "|)&:")) {
    if ((token = tokenize(start_ptr, end_ptr, &q, &eq)) == 0) {
      break;
    }
    if (token != 't') {
      fprintf(stderr, "Invaid syntax!");
    }
    cmd->argv[argc] = q;
    cmd->oargv[argc] = eq;
    argc++;
    if (argc >= MAX_CMD_LINE_ARGS) {
      fprintf(stderr, "Too many arguments!");
    }
    root = parse_redirs(root, start_ptr, end_ptr);
  }
  cmd->argv[argc] = 0;
  cmd->oargv[argc] = 0;
  return root;
}
// add \0 character at the end of commands
cmd *null_terminate(cmd *cmd) {
  int index;
  struct back_cmd *bgcmd;
  struct exec_token *execcmd;
  struct chain_token *listcmd;
  struct pipe_token *pipecmd;
  struct redir_token *rcmd;
  if (cmd == 0) {
    return 0;
  }
  switch (cmd->type) {
  case EXECUTE:
    execcmd = (struct exec_token *)cmd;
    for (index = 0; execcmd->argv[index]; index++) {
      *execcmd->oargv[index] = 0;
    }
    break;
  case REDIRECT:
    rcmd = (struct redir_token *)cmd;
    null_terminate(rcmd->cmd);
    *rcmd->efile = 0;
    break;
  case PIPE:
    pipecmd = (struct pipe_token *)cmd;
    null_terminate(pipecmd->left);
    null_terminate(pipecmd->right);
    break;
  case CHAIN:
    listcmd = (struct chain_token *)cmd;
    null_terminate(listcmd->left);
    null_terminate(listcmd->right);
    break;
  }
  return cmd;
}
// characters of note
char whitespaces[] = " \t\n";
char token_symbols[] = "<!>;()|";
// tokenizer, returns the next token
int tokenize(char **start_ptr, char *end_ptr, char **token_start,
             char **token_end) {
  char *start = *start_ptr;
  // returns index of token in the buffer
  int ret;
  // advance past any whitespaces until we reach the end of the current
  // substring
  while (start < end_ptr && strchr(whitespaces, *start))
    start++;
  // set the token start if the argument isd not null
  if (token_start)
    *token_start = start;
  // set start as the return value
  ret = *start;
  // advance the start pointer past the current token
  switch (*start) {
  case 0:
    break;
  case '(':
  case ')':
  case ';':
  case '&':
  case '<':
  case '>':
  case '|':
    start++;
    // replace !! with a single ! character for the history command
  case '!':
    start++;
    if (*start == '!') {
      ret = '!';
      start++;
    }
    break;
  // advance until either the end of the line or the next whitespace
  default:
    ret = 't';
    while (start < end_ptr && !strchr(whitespaces, *start) &&
           !strchr(token_symbols, *start))
      start++;
    break;
  }
  // set the end token
  if (token_end)
    *token_end = start;
  // set the next address for start_ptr
  while (start < end_ptr && strchr(whitespaces, *start))
    start++;
  *start_ptr = start;
  return ret;
}
// look ahead and correct the parser if any whitespace was missed
int peek(char **start_ptr, char *end_ptr, char *tokens) {
  char *start = *start_ptr;
  while (start < end_ptr && strchr(whitespaces, *start))
    start++;
  *start_ptr = start;
  return *start && strchr(tokens, *start);
}
// return the root node of the command tree
cmd *parse(char *start) {
  char *end_ptr = start + strlen(start);
  cmd *cmd = parse_line(&start, end_ptr);
  peek(&start, end_ptr, "\0");
  if (start != end_ptr) {
    fprintf(stderr, "Leftovers: %s\n", start);
  }
  null_terminate(cmd);
  return cmd;
}
// executing functions
void execute(cmd *cmd) {
  int p[2];
  struct exec_token *execcmd;
  struct chain_token *listcmd;
  struct pipe_token *pipecmd;
  struct redir_token *redirectcmd;
  if (cmd == 0) {
    exit(1);
  }
  switch (cmd->type) {
  default:
    fprintf(stderr, "Must specify a command");
  case EXECUTE:
    execcmd = (struct exec_token *)cmd;
    if (execcmd->argv[0] == 0) {
      exit(1);
    }
    wait(NULL);
    execvp(execcmd->argv[0], execcmd->argv);
    fprintf(stderr, "execvp %s failed\n", execcmd->argv[0]);
    break;
  case REDIRECT:
    redirectcmd = (struct redir_token *)cmd;
    close(redirectcmd->fd);
    if (open(redirectcmd->file, redirectcmd->mode) < 0) {
      fprintf(stderr, "Open %s failed\n", redirectcmd->file);
      exit(1);
    }
    execute(redirectcmd->cmd);
    break;
  case CHAIN:
    listcmd = (struct chain_token *)cmd;
    if (fork() == 0) {
      execute(listcmd->left);
    }
    wait(NULL);
    execute(listcmd->right);
    break;
  case PIPE:
    pipecmd = (struct pipe_token *)cmd;
    if (pipe(p) < 0) {
      fprintf(stderr, "Get those pipes checked out!");
    }
    // first child executes left node
    int pid = fork();
    if (pid == 0) {
      // make p[1] stdout
      dup2(p[1], 1);
      close(p[0]);
      close(p[1]);
      execute(pipecmd->left);
    }
    // second child executes right node
    int pid2 = fork();
    if (pid2 == 0) {
      // make p[0] stdin
      dup2(p[0], 0);
      close(p[0]);
      close(p[1]);
      execute(pipecmd->left);
    }
    // close pipe in the parent as well as in both children
    close(p[0]);
    close(p[1]);
    // wait for both child processes to finish
    wait(NULL);
    wait(NULL);
    break;
  }
}
// get input from cmd line and put it in the buffer
void get_input(char *buffer) {
  memset(buffer, 0, BUFSIZ * sizeof(char));
  printf("osh > ");
  fgets(buffer, BUFSIZ, stdin);
  fflush(stdout);
}
// Implement osh shell as a REPL
int main() {
  char input[BUFSIZ];
  // initialize history
  history *log = initialize_history();
  // start the REPL
  while (true) {
    get_input(input);
    // check for null
    if (*input == '\0') {
      fprintf(stderr, "no command entered\n");
      exit(1);
    }
    // check for exit
    if (strncmp(input, "exit", 4) == 0) {
      printf("\t\t...exiting\n");
      exit(0);
    }
    // check for cd
    if (strncmp(input, "cd", 2) == 0) {
      input[strlen(input) - 1] = 0;
      if (chdir(input + 3) < 0) {
        fprintf(stderr, "cannot cd to nonexistent directory: %s\n", input + 3);
      }
      continue;
    }
    // print history- doesn't work :|
    if (strncmp(input, "!!", 2) == 0) {
      print_history(log);
      continue;
    }
    // maybe this one doesn't work, didn't have time to thoroughly debug
    add_to_history(log, input);
    int pid = fork();
    if (pid == 0) {
      execute(parse(input));
    }
    wait(NULL);
  }
  free_history(log);
  exit(0);
}
// 460 lines and still couldn't make it work :|