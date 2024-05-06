# Filesystems with `FUSE`
## What's `FUSE`?
It's a cool system for allowing userspace applications to create logical filesystems without elevation. The acronym itself stands for "**F**ilesystem in **USE**rspace".
Don't worry, most of the actual difficulty has already been wrangled, so all you've gotta deal with is what's left over. 


## Prerequisites
This project uses `fuse3`. Depending on your *nix distribution's package manager, you'll need to install the appropriate FUSE development libraries. Since I'm not entirely heartless, I've included a common list of them.

|Distro    |        Package Name(s)|
|---| ---|
|Arch (btw)    |`fuse3`,`fuse-common`|
|Ubuntu (&WSL) |`libfuse3-dev`, `libfuse3-3`, `fuse3`|

If you're running a different distro, I'm going to assume you're competent enough to get the dev dependencies installed. You might also consider adding your distro & required package set to this list so the next guy doesn't have to figure it out.

## Assignment Details

In this assignment, you'll be adding a directory with a single file. 
By default, the filesystem created should look like this:
```
mount
└── hello

1 directory, 1 file
```
Kinda...underwhelming, right? Not to worry, you'll be making it slightly less boring! The entire goal of this assignment is simple: add on to the existing logic to get a product in the form of the following directory tree:
```
mount
├── hello
└── sub-directory
    └── hi

2 directories, 2 files
```
Each file will need to have some amount of readable content. The first one is already implemented for you. 

You've been working with three-file structure. This is optional on this assignment as I don't see a real reason to separate the functionality into separate files. Enjoy it while it lasts, I suppose! :)

By this point, you've likely opened the `main.c` file and had a panic attack looking at the contents of `int main(int, char**)`. Don't sweat it. You'll be making a grand total of zero (0) edits to the main function. Everything I learned up to this point is the result of trying, failing, and trying again. With that in mind, get in there, break stuff, and remember: `git` is your friend. Use it. Frequent commits will save you from deleting progress as you work. 

The code is somewhat documented. This is purposeful. You're going to need to be able to infer usage details from implementation, and you're also going to need to be able to use the internet to your advantage. The real world's like that. Good luck!

## Grading
Run the `./validate.sh` script. If everything passes, you're good to submit. If it doesn't, get back in there. 

