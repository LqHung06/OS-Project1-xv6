#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

// Keep the required recursive function void, but report errors to main().
static int had_error = 0;

static void
print_indent(int depth)
{
  for(int i = 0; i < depth; i++)
    printf("  ");
}

// Display only the last directory component, with a trailing slash.
// In particular, "." is printed as "./" and "/" is printed as "/".
static void
print_directory(char *path, int depth)
{
  char name[MAXPATH];
  int end = strlen(path);

  while(end > 1 && path[end - 1] == '/')
    end--;

  print_indent(depth);
  if(end == 1 && path[0] == '/'){
    printf("/\n");
    return;
  }

  int start = end;
  while(start > 0 && path[start - 1] != '/')
    start--;

  int len = end - start;
  memmove(name, path + start, len);
  name[len] = 0;
  printf("%s/\n", name);
}

// Depth-first traversal: print the current directory, then its entries.
void
tree(char *path, int depth)
{
  struct dirent de;
  struct stat st, entry_st;
  char name[DIRSIZ + 1];
  char child[MAXPATH];
  int fd, n;

  if(strlen(path) >= MAXPATH){
    fprintf(2, "tree: path too long: %s\n", path);
    had_error = 1;
    return;
  }

  fd = open(path, O_RDONLY);
  if(fd < 0){
    fprintf(2, "tree: cannot open %s\n", path);
    had_error = 1;
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "tree: cannot stat %s\n", path);
    had_error = 1;
    close(fd);
    return;
  }
  if(st.type != T_DIR){
    fprintf(2, "tree: not a directory: %s\n", path);
    had_error = 1;
    close(fd);
    return;
  }

  print_directory(path, depth);

  while((n = read(fd, &de, sizeof(de))) == sizeof(de)){
    if(de.inum == 0)
      continue; // An unused directory slot.

    // dirent.name has exactly DIRSIZ bytes; it may lack a '\0'.
    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = 0;
    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    int path_len = strlen(path);
    int name_len = strlen(name);
    int need_slash = path_len > 0 && path[path_len - 1] != '/';

    // Reserve one extra byte for the terminating '\0'.
    if(path_len + need_slash + name_len >= MAXPATH){
      fprintf(2, "tree: path too long: %s/%s\n", path, name);
      had_error = 1;
      continue;
    }

    memmove(child, path, path_len);
    int pos = path_len;
    if(need_slash)
      child[pos++] = '/';
    memmove(child + pos, name, name_len);
    child[pos + name_len] = 0;

    if(stat(child, &entry_st) < 0){
      fprintf(2, "tree: cannot stat %s\n", child);
      had_error = 1;
      continue; // Keep processing sibling entries.
    }

    if(entry_st.type == T_DIR){
      tree(child, depth + 1);
    } else {
      print_indent(depth + 1);
      printf("%s\n", name);
    }
  }

  if(n != 0){ // read() returned -1 or a partial directory entry.
    fprintf(2, "tree: cannot read directory %s\n", path);
    had_error = 1;
  }
  if(close(fd) < 0){
    fprintf(2, "tree: cannot close %s\n", path);
    had_error = 1;
  }
}

int
main(int argc, char *argv[])
{
  if(argc > 2){
    fprintf(2, "usage: tree [directory]\n");
    exit(1);
  }

  tree(argc == 2 ? argv[1] : ".", 0);
  exit(had_error ? 1 : 0);
}
