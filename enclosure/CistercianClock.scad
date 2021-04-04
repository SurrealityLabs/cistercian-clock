/*
 * This file is part of Cistercian Clock.
 *
 * Cistercian Clock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * Cistercian Clock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Cistercian Clock. If not, see <https://www.gnu.org/licenses/>.
 */

/* roundedcube.scad was not included due to licensing.
 * download from https://gist.github.com/groovenectar/92174cb1c98c1089347e
 */
use <roundedcube.scad>

numColumns = 5;
numRows = 7;
ledPitch = 1000 / 60;
outerWallWidth = 8;
outerSpaceWidth = 2;
ledWallWidth = 1.66667;
ledChamberDepth = 10;
diffuserThickness = 1;
faceThickness = 1;
interfaceDepth = 4;
threadedInsertDiameter = 5;
threadedInsertDepth = 12;
threadedInsertInset = 6;
screwDiameter = 3.2;
screwHeadDiameter = 7;
sigma = 0.01;
fitFactor = 0.1;
ledChamberLength = ledPitch - ledWallWidth;
fillet = 2;

electronicsBayDepth = 11;
ledStripWall = 2;
ledStripDepth = 2;
ledStripWidth = 12;
ledWiringExposure = 5;

usbWidth = 18;
usbHeight = 10;
usbOffset = 2;

backWall = 5;

midlayerDepth = electronicsBayDepth + 2 + ledStripWall + ledStripDepth + interfaceDepth;

overallWidth = (numColumns * ledPitch) + ledWallWidth + (2 * (outerSpaceWidth + outerWallWidth));
overallHeight = (numRows * ledPitch) + ledWallWidth + (2 * (outerSpaceWidth + outerWallWidth));

interfaceWidth = (numColumns * ledPitch) + ledWallWidth + (2 * outerSpaceWidth);
interfaceHeight = (numRows * ledPitch) + ledWallWidth + (2 * outerSpaceWidth);

displayShellDepth = faceThickness + ledChamberDepth + interfaceDepth;

cornerScrewX = (overallWidth - (threadedInsertInset + (threadedInsertDiameter/2))) / 2;
cornerScrewY = (overallHeight - (threadedInsertInset + (threadedInsertDiameter/2))) / 2;

screwLocations = [[    cornerScrewX,  cornerScrewY, 0],
                  [    cornerScrewX, -cornerScrewY, 0],
                  [   -cornerScrewX,  cornerScrewY, 0],
                  [   -cornerScrewX, -cornerScrewY, 0],
                  [   -cornerScrewX, 0, 0],
                  [    cornerScrewX, 0, 0]];
ledLocations = [[ 0 * ledPitch,  0 * ledPitch, 0],
                [-1 * ledPitch,  0 * ledPitch, 0],
                [-2 * ledPitch,  0 * ledPitch, 0],
                [ 1 * ledPitch,  0 * ledPitch, 0],
                [ 2 * ledPitch,  0 * ledPitch, 0],
                [ 0 * ledPitch,  1 * ledPitch, 0],
                [-1 * ledPitch,  1 * ledPitch, 0],
                [-2 * ledPitch,  1 * ledPitch, 0],
                [ 1 * ledPitch,  1 * ledPitch, 0],
                [ 2 * ledPitch,  1 * ledPitch, 0],
                [ 0 * ledPitch, -1 * ledPitch, 0],
                [-1 * ledPitch, -1 * ledPitch, 0],
                [-2 * ledPitch, -1 * ledPitch, 0],
                [ 1 * ledPitch, -1 * ledPitch, 0],
                [ 2 * ledPitch, -1 * ledPitch, 0],
                [ 0 * ledPitch,  2 * ledPitch, 0],
                [-1 * ledPitch,  2 * ledPitch, 0],
                [-2 * ledPitch,  2 * ledPitch, 0],
                [ 1 * ledPitch,  2 * ledPitch, 0],
                [ 2 * ledPitch,  2 * ledPitch, 0],
                [ 0 * ledPitch, -2 * ledPitch, 0],
                [-1 * ledPitch, -2 * ledPitch, 0],
                [-2 * ledPitch, -2 * ledPitch, 0],
                [ 1 * ledPitch, -2 * ledPitch, 0],
                [ 2 * ledPitch, -2 * ledPitch, 0],
                [ 0 * ledPitch,  3 * ledPitch, 0],
                [-1 * ledPitch,  3 * ledPitch, 0],
                [-2 * ledPitch,  3 * ledPitch, 0],
                [ 1 * ledPitch,  3 * ledPitch, 0],
                [ 2 * ledPitch,  3 * ledPitch, 0],
                [ 0 * ledPitch, -3 * ledPitch, 0],
                [-1 * ledPitch, -3 * ledPitch, 0],
                [-2 * ledPitch, -3 * ledPitch, 0],
                [ 1 * ledPitch, -3 * ledPitch, 0],
                [ 2 * ledPitch, -3 * ledPitch, 0]];
                
ledRows = [[0,  3 * ledPitch, 0],
           [0,  2 * ledPitch, 0],
           [0,  1 * ledPitch, 0],
           [0,  0 * ledPitch, 0],
           [0, -1 * ledPitch, 0],
           [0, -2 * ledPitch, 0],
           [0, -3 * ledPitch, 0]];

//displayShell();

diffuser();

//midlayer();

//backlid(); 

/*union()
{
    color("brown", 0.9) displayShell();
    translate([0,0,-((displayShellDepth-(2*faceThickness)-diffuserThickness)/2) + sigma])
    {
        for(location = ledLocations)
        {
            translate(location)
            {
                color("white", 0.75) diffuser();
            }
        }
    }
}

color("blue", 0.8) translate([0,0,18.75]) midlayer();

color("orange", 0.8) translate([0,0,32]) backlid();*/

module backlid()
{
    difference()
    {
        union()
        {
            roundedcube([overallWidth, overallHeight, backWall], center=true, radius = fillet, apply_to = "zmax");
            translate([0,0,-((backWall - (2*fitFactor))/2)-(interfaceDepth/2)+sigma])
            {
                cube([interfaceWidth - (2 * fitFactor), interfaceHeight - (2 * fitFactor), interfaceDepth - (2*fitFactor) + (2 * sigma)], center=true);
            }
        };
        union()
        {
            translate([0,0,(((displayShellDepth-threadedInsertDepth)/2))+sigma])
            {
                for(insert = screwLocations)
                {
                    translate(insert)
                    {
                        cylinder(r=screwDiameter/2, h=midlayerDepth + 4 + 4 * sigma, center=true, $fn=32);
                    }
                }
            };
        }
    }
}


module midlayer()
{
    difference()
    {
        union()
        {
            roundedcube([overallWidth, overallHeight, midlayerDepth - interfaceDepth], center=true, radius = fillet, apply_to = "z");
            translate([0,0,-((midlayerDepth - interfaceDepth - (2*fitFactor))/2)-(interfaceDepth/2)+sigma])
            {
                cube([interfaceWidth - (2 * fitFactor), interfaceHeight - (2 * fitFactor), interfaceDepth - (2*fitFactor) + (2 * sigma)], center=true);
            }
        };
        union()
        {
            translate([0,0,((((midlayerDepth - interfaceDepth)-(electronicsBayDepth+interfaceDepth))/2)-fitFactor)+sigma])
            {
                cube([interfaceWidth + (2 * fitFactor), interfaceHeight + (2 * fitFactor), electronicsBayDepth + interfaceDepth + (2*fitFactor) + (2 * sigma)], center=true);
            };
            translate([0,0,-((midlayerDepth - interfaceDepth)/2)-(interfaceDepth)+(ledStripDepth/2)])
            {
                for(row = ledRows)
                {
                    translate(row)
                    {
                        cube([interfaceWidth, ledStripWidth, ledStripDepth + 2 * sigma], center=true);
                        translate([-(interfaceWidth - ledWiringExposure)/2, 0, ledStripWall-(sigma)]) cube([ledWiringExposure, ledStripWidth, 2 + ledStripDepth + ledStripWall + 2 * sigma], center=true);
                        translate([+(interfaceWidth - ledWiringExposure)/2, 0, ledStripWall-(sigma)]) cube([ledWiringExposure, ledStripWidth, 2 + ledStripDepth + ledStripWall + 2 * sigma], center=true);
                    }
                }
            };
            translate([0,0,(((displayShellDepth-threadedInsertDepth)/2))+sigma])
            {
                for(insert = screwLocations)
                {
                    translate(insert)
                    {
                        cylinder(r=screwDiameter/2, h=midlayerDepth + 4 + 4 * sigma, center=true, $fn=32);
                    }
                }
            };
            
            translate([sigma+(overallWidth-outerWallWidth)/2,-42,-((midlayerDepth - interfaceDepth - (2*fitFactor))/2)+usbOffset+ledStripWall+(usbHeight/2)])
            {
                cube([outerWallWidth + (2 * sigma), usbWidth, usbHeight], center=true);
            }
        }
    }
}

module diffuser()
{
    cube([ledChamberLength - 6*fitFactor, ledChamberLength - 6*fitFactor, diffuserThickness], center=true);
}

module displayShell()
{
    difference()
    {
        roundedcube([overallWidth, overallHeight, displayShellDepth], center=true, radius = fillet, apply_to = "zmin");
        union()
        {
            translate([0,0,(((displayShellDepth-interfaceDepth)/2)-fitFactor)+sigma])
            {
                cube([interfaceWidth + (2 * fitFactor), interfaceHeight + (2 * fitFactor), interfaceDepth + (2*fitFactor) + (2 * sigma)], center=true);
            };
            translate([0,0,(((displayShellDepth-threadedInsertDepth)/2))+sigma])
            {
                for(insert = screwLocations)
                {
                    translate(insert)
                    {
                        cylinder(r=threadedInsertDiameter/2, h=threadedInsertDepth + 2 * sigma, center=true, $fn=32);
                    }
                }
            };
            translate([0,0,(((displayShellDepth-(2*interfaceDepth)-ledChamberDepth)/2)-fitFactor)+sigma])
            {
                for(location = ledLocations)
                {
                    translate(location)
                    {
                        cube([ledChamberLength, ledChamberLength, ledChamberDepth], center=true);
                    }
                }
            };
            /* letters */
            translate([0,0,-((displayShellDepth-faceThickness)/2) - sigma])
            {
                linear_extrude(height = faceThickness + (2 * sigma), center = true, convexity = 10)
   mirror([1,0,0]) import (file = "CistercianClockHoles.svg");
            }
        }
    }
}

$fs = 0.15;
