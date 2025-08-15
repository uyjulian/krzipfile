krzipfile
=========

| This Kirikiri (2/Z) plugin allows mounting of UTF-8 encoded zip files.
| The compression used can be none, zlib/deflate, bz2, and lzma.

Building
--------

After cloning submodules and placing ``ncbind`` and ``tp_stub`` in the
parent directory, a simple ``make`` will generate ``krzipfile.dll``.

How to use
----------

After ``Plugins.link("krzipfile.dll");`` is used, the additional
functions will be exposed under the ``Storages`` class.

License
-------

This project is licensed under the MIT license. Please read the
``LICENSE`` file for more information.
