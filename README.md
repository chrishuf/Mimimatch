# Mimimatch
Matches two images of Macbeth charts to each other and generates a bmd fusion tree in the clipboard
Mimimatch

mimimatch is an experimental tool, to match two macbeth charts towards each other
and creating a fusion node setup ready to be copy pasted in blackmagic fusion

The idea behind this is, to skip any color normalisation and directly match two
sources directly to each other.

more info can be found at:
https://www.splotchdog.com/index.php?option=com_content&view=article&id=143&Itemid=171 
 


compile instructions

You need the following external sources:

The FreeImage library 
https://freeimage.sourceforge.io/

OpenCV with the contribution extensions compiled 
https://github.com/opencv/opencv    
https://github.com/opencv/opencv_contrib
(namely the mcc module)
wxwidgets
https://wxwidgets.org/

The files are released without project files but you can simply copy/overwrite the minimal example in wxwidgets
and replace / link the needed files. 
Didn't tested linux compile, but I didn't used any weird things, so it should be straight forward.
 
