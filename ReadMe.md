[![Build Status](https://travis-ci.org/sba1/simplegit.svg?branch=master)](https://travis-ci.org/sba1/simplegit)
[![PayPal Donate](https://img.shields.io/badge/donate-paypal-yellow.svg?style=flat)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GMK9426WMR4NW)

SimpleGit
=========

This is a very simple git client based on ```libgit2``` with AmigaOS being the
major target operating system. However, other platforms are equally important as
this eases testing.

Usage
-----

The program is currently called ```sgit``` to differentiate it from the standard
```git``` command. If you start the ```sgit``` executable without any arguments,
a list of all supported commands will be printed. Note that even if a command is
listed there, it it likely a very crippled verison of the original version.

The AmigaOS version of SimpleGit uses a statically linked OpenSSL version. Even if
it is not recommended in the general case, you can use the ```GIT_SSL_NO_VERIFY```
environment variable to skip the SSL verification. E.g., enter
```
 1.> setenv GIT_SSL_NO_VERIFY 1
 1.> sgit clone https://github.com/sba1/simplemail simplemail
```
to clone and the SimpleMail repository into the ```simplemail``` folder ignoring
any SSL relatated problems.

Generally note that this is software is in a very preliminary state. It may be not
usable at all in terms of functions it provides but also in terms of bugs it
contains. Consider it as a prototype and use it at your own risk.

To display diff in color the normal ```sgit diff --color``` command can be used.
But if the output does not look good for you in the AmigaDOS console, you can use
the ```gitdiff``` script instead. Make sure it has the Executable and Script bit
set, and exists in the path. Edit the E[47m and E[45m entries to your liking.
Ref: http://wiki.amigaos.net/wiki/AmigaOS_Manual:_AmigaDOS_Advanced_Features
The script uses ```sed``` which is part of the SDK.

Commands
--------

```
Usage: sgit <command>
Following plumbing commands are supported:
  add             Add contents of file to the index
  branch          Create, delete, or delete branches
  checkout        Copy a branch or paths to the working directory
  cherry-pick     Redo commits
  clone           Fetch and checkout a repository into a new directory
  commit          Create a new commit from the index
  config          Get and set repository or global options
  diff            Show differences between objects
  fetch           Fetch objects from another repository
  init            Create or reinitialize a git repository
  log             Show the commit logs
  merge           Join histories defined by commit together
  push            Update the remote references and push associated objects
  rebase          Reapply changes
  reset           Reset the HEAD or the index to a specific state
  rm              Remove files
  show            Show objects
  stash           Manage a separate stash of changes
  status          Summarize working directory changes
  submodule       Manage submodules
  tag             Manage tags
  version         Show version information
Following porcelaine commands are supported:
  cat-file        Show content of repository objects
  checkout-index  Copy files from the index
  commit-tree     Create a new commit object from a tree object
  diff-files      Show differences between files in the working directory and the index
  help            Show this help
  index-pack      Build the index file for an existing pack file
  ls-files        Show information about files in the index and the working directory
  ls-remote       Show information about remote references
  ls-tree         Show information about a tree object
  mktag           Create a tag object
  read-tree       Read a tree object into the index
  rev-list        Show information about commit objects
  update-index    Update the index with files from the working directory
  write-tree      Create a write object from the current index

```

Bug reports
-----------

Please use the GitHub Issue form for filing bug reports or enhancement requests:
https://github.com/sba1/simplegit/issues

Changes
-------

Refer to the git log for changes.

Building SimpleGit for AmigaOS
------------------------------

Currently, it only is possible to compile SimpleGit via a cross compiler and
CMake. See http://sourceforge.net/projects/adtools/ or
https://github.com/sba1/adtools/ how to get the cross compiler environment.

Once everything is set up, building SimpleGit is as easy as typing
```
 $ make -f makefile.aos4
```
in the shell command prompt. Snapshot builds are available from
 https://sonumina.de/jenkins/job/sgit.amiga/.
That site's SHA1 fingerprint is ```E5 7A 7E 48 FE 4C BC 36 1D A8 D9 EE 1B 43 04 C6
10 B8 FF 81```.

Building SimpleGit for other platforms
--------------------------------------

Among other stuff, CMake is needed to build the tool. Then type

```
 $ git submodule init
 $ git submodule update
 $ make
```
Testing on other platforms
--------------------------

If you want to test git2 with the tests of the original git, run:
```
 $ make test
```
on a POSIX-compatible operating system. This can take a lot of time (it compiles
git and then launch thousands of tests). As we are using the git test framework,
nearly all what can be done with git test can be done with our test base (i.e.
set ```GIT_SKIP_TESTS``` or ```GIT_TEST_OPTS```).

You can run a particular test by giving its name:
```
 $ make -C tests t0000-basic.sh
```

Origins
-------

Its code base derived from https://github.com/vfr-nl/git2/ (thanks to Vincent
van Ravesteijn and Motiejus Jakštys), from which it was forked to
https://github.com/Romain-Geissler/git2.git. It was then forked by Chris
Young to https://github.com/chris-y/git2.git, which is the direct parent
of SimpleGit.

It is currently developed at https://github.com/sba1/simplegit.git with
https://git.gitorious.org/amiga-ports/git2-amiga.git being a mirror. The
sources contains work from the examples included within ```libgit2```.

Thanks for testing
------------------

  Hubert Maier
