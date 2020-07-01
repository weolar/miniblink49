# What is sfntly?

sfntly is a Java and C++ library for using, editing, and creating sfnt container based fonts (e.g. OpenType, TrueType). This library was initially created by Google's Font Team and the C++ port was done by the Chrome team. It has been made open source.

The basic features of sfntly are the reading, editing, and writing of an sfnt container font. Fonts that use an sfnt container include OpenType, TrueType, AAT/GX, and Graphite. sfntly isn't itself a tool that is usable by an end user - it is a library that allows software developers to build tools that manipulate fonts in ways that haven't been easily accessible to most developers. The sfntly library is available in Java with a partial C++ port. However, we have included some font tools that are built on top of sfntly: a font subsetter, font dumper, a font linter, some compression utilities.

The uses of sfntly are really anything that you can think of that involves reading and/or editing fonts. Right now, the Java version is the core library used to power Google's Web Fonts project. There it is used for all font manipulation - to read font data, to pull apart fonts, and to then reassemble them before they are streamed out to a user. Portions of the font that are not needed - specific glyph ranges or features - are stripped using sfntly to minimize the size of the streamed font. The C++ port is used somewhat similarly within Chrome to subset fonts for insertion into a PDF for viewing or printing. Though the features stripped in the font are different in Chrome than in Web Fonts because the end use is different.

Using sfntly you can read and extract any of the tables in a font. The tables are the individual data structures within the font for each of the features and functionality: glyph outlines, character maps, kerning, meta data, etc. If you look over the OpenType and TrueType specs, you will see a number of categories of tables. sfntly currently supports all of the required tables, the TrueType outline tables, bitmap glyph tables, and a couple of the other miscellaneous tables. This level of support provides for many of the needs developers have related to the informational reading of font data. It also covers a lot of the editing needs.

To get started with sfntly: clone the repository and follow the quickstart.txt guide in the Java directory

have fun

Stuart Gill - sfntly Architect and Lead Developer
