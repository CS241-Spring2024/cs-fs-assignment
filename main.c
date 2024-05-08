/*
 * Adopted from Miklos Szeredi <miklos@szeredi.hu> by Walter Neils, 2024
 * */

#include "impl.h"
#include <fcntl.h>

static const char *example_file_content = "Hello World!\n";
static const char *example_file_name = "hello";

static int fs_stat(fuse_ino_t ino, struct stat *stbuf) {
  stbuf->st_ino = ino;
  switch (ino) {
  case 1:
    // Yep that's our root directory
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    break;

  case 2:
    // Behold: our single example file.
    stbuf->st_mode = S_IFREG | 0444;
    stbuf->st_nlink = 1;
    stbuf->st_size = strlen(example_file_content);
    break;

  default:
    return -1;
  }
  return 0;
}
/*
 * Get attributes for a given record by inode
 * */
static void fs_getattr(fuse_req_t req, fuse_ino_t ino,
                       struct fuse_file_info *fi) {
  struct stat stbuf;

  memset(&stbuf, 0, sizeof(stbuf));
  if (fs_stat(ino, &stbuf) == -1) {
    // Yeah our stat function wasn't able to handle the request. Which means the
    // inode wasn't found. Enjoy the ENOENT.
    fuse_reply_err(req, ENOENT);
  } else {
    // Oh look, a result.
    fuse_reply_attr(req, &stbuf, 1.0);
  }
}

/*
 * Implement the FS lookup functionality (filename -> file attributes)
 * */
static void fs_lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
  if (parent != 1 || strcmp(name, example_file_name) != 0) {
    // Request was either for a record in a directory other than root (inode not
    // equal to 1) or for a file with a name other than `hello_name` or both.
    fuse_reply_err(req, ENOENT);
  } else {
    struct fuse_entry_param entry_parameter;
    memset(&entry_parameter, 0,
           sizeof(entry_parameter)); // Good 'ol C initialization
    entry_parameter.ino =
        2; // We're statically setting the file to have inode 2.
    entry_parameter.attr_timeout = 1.0;
    entry_parameter.entry_timeout = 1.0;
    fs_stat(entry_parameter.ino,
            &entry_parameter.attr); // Call our stat function to get entry
                                    // details for the given inode number

    fuse_reply_entry(req, &entry_parameter); // Send a reply back
  }
}

/*
 * Reads directory entries.
 * */
static void fs_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                       struct fuse_file_info *fi) {
  if (ino != 1) {
    // The only directory we have is hardcoded to use inode 1
    fuse_reply_err(req, ENOTDIR);
  } else {
    struct dirbuf dirbuf;
    memset(
        &dirbuf, 0,
        sizeof(
            dirbuf)); // The stack is NOT guaranteed to allocate zero'd memory
    dirbuf_add(req, &dirbuf, ".", 1);
    dirbuf_add(req, &dirbuf, "..", 1);
    dirbuf_add(req, &dirbuf, example_file_name,
               2); // Our example file, with a hardcoded inode value of 2
    reply_buf_limited(
        req, dirbuf.p, dirbuf.size, off,
        size); // Reply as much as possible.
               // If the requesting application needs more, it'll ask again.
    free(dirbuf.p); // Don't leak memory
  }
}

/*
 * Implements `open()` functionality for the FS.
 * */
static void fs_open(fuse_req_t request, fuse_ino_t inode,
                    struct fuse_file_info *file_info) {
  if (inode != 2) {
    // We have one hardcoded file at inode 2.
    // They requested to open a different inode.
    fuse_reply_err(request, EISDIR);
  } else if ((file_info->flags & O_ACCMODE) != O_RDONLY) {
    fuse_reply_err(request, EACCES);
  } else {
    // Ok fine, you can open our one (1) file
    fuse_reply_open(request, file_info);
  }
}

/*
 * Implements `read()` for the FS.
 * */
static void fs_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off,
                    struct fuse_file_info *fi) {
  assert(ino == 2); // We have one file. It's at inode 2.
  reply_buf_limited(req, example_file_content, strlen(example_file_content),
                    off, size);
}

// Collection of operations our little filesystem can do
static const struct fuse_lowlevel_ops fs_operations = {
    .lookup = fs_lookup,
    .getattr = fs_getattr,
    .readdir = fs_readdir,
    .open = fs_open,
    .read = fs_read,
};

int main(int argc, char *argv[]) {
  // DO NOT EDIT
  // Unless you're really sure of yourself.
  // Also you just shouldn't be editing this.
  // :)
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  struct fuse_session *fuse_session;
  struct fuse_cmdline_opts opts;
  struct fuse_loop_config *config;
  int ret = -1;

  if (fuse_parse_cmdline(&args, &opts) != 0)
    return 1;
  if (opts.show_help) {
    printf("usage: %s [options] <mountpoint>\n\n", argv[0]);
    fuse_cmdline_help();
    fuse_lowlevel_help();
    ret = 0;
    goto err_out1;
  } else if (opts.show_version) {
    printf("FUSE library version %s\n", fuse_pkgversion());
    fuse_lowlevel_version();
    ret = 0;
    goto err_out1;
  }

  if (opts.mountpoint == NULL) {
    printf("usage: %s [options] <mountpoint>\n", argv[0]);
    printf("       %s --help\n", argv[0]);
    ret = 1;
    goto err_out1;
  }

  fuse_session =
      fuse_session_new(&args, &fs_operations, sizeof(fs_operations), NULL);
  if (fuse_session == NULL)
    goto err_out1;

  if (fuse_set_signal_handlers(fuse_session) != 0)
    goto err_out2;

  if (fuse_session_mount(fuse_session, opts.mountpoint) != 0)
    goto err_out3;

  fuse_daemonize(1); // Always run foreground so unmounting isn't required

  /* Block until ctrl+c or fusermount -u */
  ret = fuse_session_loop(fuse_session);

  fuse_session_unmount(fuse_session);
err_out3:
  fuse_remove_signal_handlers(fuse_session);
err_out2:
  fuse_session_destroy(fuse_session);
err_out1:
  free(opts.mountpoint);
  fuse_opt_free_args(&args);

  return ret ? 1 : 0;
}
