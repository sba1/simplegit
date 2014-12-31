[![Build Status](https://travis-ci.org/sba1/simplegit.svg?branch=master)](https://travis-ci.org/sba1/simplegit)

SimpleGit
=========

This is a very simple git client based on ```libgit2``` with AmigaOS being the
major target operating system. However, other platforms are equally important as
this eases testing.

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
 https://sonumina.de/jenkins/job/sgit.amiga/

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
https://github.com/Romain-Geissler/git2.git and
https://github.com/chris-y/git2.git. It contains sources from the exampled
included within ```libgit2```.

It is currently developed at https://github.com/sba1/simplegit.git with
https://git.gitorious.org/amiga-ports/git2-amiga.git being a mirror.

Note that the call forwarding to the original git for unknown options has been
removed as the goal is to implement all important commands natively.
