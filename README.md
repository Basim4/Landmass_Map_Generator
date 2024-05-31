# Landmass_Generator
USAGE: ./landmass_map.exe or ./landmass_map.exe -s (integer as a seed)

NOTE: termcolor.hpp is created by Ihor Kalnytskyi and is used under a BSD license. This file was provided to my class by our professor. 

C++ Program that allows the user to create a map of a Landmass. Based on the given parameters, a table of raw data values, 
normalized data values, and an ascii representation of the normalized data is displayed. A specific seed can be provided 
for the map generation, as shown in the second usage statement above.

This program is meant to be executed on the terminal for the colored text. However, it can be easily imported and ran on an IDE 
but the ascii text color would most likely be lost.

"#" indicates deep water

"~" indicates shallow water

"." indicates coast/beach

"-" indicates plains/grass

"*" indicates forests

"^" indicates mountains
