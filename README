GIMP-SinXPI
===========

Modify an image's colors by mapping the channels to a curve defined by a
mathematical expression. (Similar to Gimp's built-in curves, only mathier.)

Developed for GIMP-2.8.6

Works for RGB*,GREY*,INDEXED* images.


Installation
------------

Try "make install". If that doesn't work, tell me why and I'll see what I
can do in the next release.


Known Issues
------------

* GimpZoomPreview. This widget needs to be banished to the moon until it
  learns to behave itself. Indexed color is the biggest problem, but
  why the hell is its shape locked to the image's aspect ratio?

* Translations. I really have to learn this gettext thing. The documentation
  and tutorials make it sound like yet another "do a shit-load of work and
  maybe we'll give you a feature that you wouldn't know how to test anyway"
  pile of shit. (GNU's Not Unix, you say?)

* 32-Bits and GEGL. Updating to Gimp-3 is going to be a pain.

* Reporting NaNs in expression evaluation. We silently convert to zero.
  In fact, any out-of-range [0..1] value should be reported.


Author
------

the other anonymous <ssinclair0073@gmail.com>


Acknowledgments
---------------

Many thanks to Leonardo Haddad Loureiro for making LViewP1b.exe (one
of the few Windows 3.1 programs which can still be found everywhere),
especially for the "Interactive RGB" feature that inspired this plugin.

Many unthanks to GTK for still not having any undo feature in its text buffer.


Copyright
---------

The file COPYING applies only to gundo.c and gundo.h, which trace their
roots back to GundoSequence by Nat Pryce.

The rest of this code is Public Domain (CC0).
http://creativecommons.org/publicdomain/zero/1.0/
