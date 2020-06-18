/****************************************************************
 * File                : Configurations.scad
 *
 * Author              : Roel Drost
 * Date                : March 28th 2019
 * Version             : 1.0
 * License             : This file and artifacts created using
 *                       this file are for non comercial use
 *                       only.
 *
 * Configurion author  : < Your name here >
 * Configurion date    : < Your date here >
 * Configurion version : < Your version here >
 *
 * ***************************************************************
 * 
 * Intended use:
 * - Change the configuration to your needs.
 * - Save this file.
 * - Refresh or render the files:
 *   - ./bottom.scad
 *   - ./top.scad
 *
 ***************************************************************/

/****************************************************************
 * Constants (DO NOT CHANGE)
 ***************************************************************/

// Compatibility version number [major, minor]
CONFIGURATION_COMPAT_VERSION = [1, 0];

// Directions
NONE   = 0;
NORTH  = 1;
EAST   = 2;
SOUTH  = 4;
WEST   = 8;

// Possible values for: nut_type
NUT_TYPE_NONE   = "none";
NUT_TYPE_HEX    = "hex";
NUT_TYPE_INSERT = "insert";

/****************************************************************
 * PCB
 ***************************************************************/

// PCB board dimentions [width, height, thickness]
pcb_board_dim        = [53, 73, 1.5];

// Clearance at the bottom of the PCB board
pcb_clearance_bottom =  3.0;

// Clearance at the top of the PCB board
pcb_clearance_top    = 35.5;

// Height of the seam relative to the top surface of the pcb. If
// you have your seam through all holes in the side of the case,
// you don't need to print support for the holes.
// Negative numbers are allowed.
pcb_seam_height      =  4;

// Location of the holes (see also: components_alignment)
// x          x-coordinate relative to the component alignment
// y          y-coordinate relative to the component alignment
// tough-wall where the strud should tough the wall.
//            Possible values are NONE or the sum of one or
//            more of the following constants:
//              NORTH, EAST, SOUTH and WEST or
// bottom     'true' of a hole for the bottom must be created,
//            'false' if not
// top        'true' of a hole for the top must be created,
//            'false' if not
pcb_holes = [
// [ [     x,      y], touch-wall  , bottom, top ],
   [ [ 22.5,  33.5], NORTH + EAST, true  , true],
   [ [-31,  37.05], NORTH + WEST, true  , true],
   [ [ 31, -37.05], SOUTH + EAST, true  , true],
   [ [-22.5, -33.5], SOUTH + WEST, true  , true]
];

/****************************************************************
 * Components
 ***************************************************************/

// Alignment of the components [hor, ver].
// hor:
//   -1 is left alignment
//    0 is center alignment
//    1 is right alignment
// ver:
//   -1 is bottom alignment
//    0 is middle alignment
//    1 is top alignment
components_alignment     = [0, 0];

// Components to be cut out of the case like connectors and
// LED's.
// See also 'components_alignment'
module components_negative() {
    INCH = 25.4;
    rotate(90) {
        translate([30,-25.6/2-0.05*INCH]) {
            cube([12,25.6,8.4]);
        }
        translate([-30-12,-10.5/2+0.35*INCH]) {
            cube([12,10.5,8.4]);
        }
        translate([-30-12,
                   -0.55*INCH/1-0.25*INCH, 
                   4-0.15*INCH/2])
        {
            cube([14,0.5*INCH,0.7*INCH]);
        }
    }
}

// Extra material for the top part, like walls and LED holders
module components_positive_top() {
}

// Extra material for the bottom part, like extra struds and
// inner walls
module components_positive_bottom() {
}

/****************************************************************
 * Nuts and bolts 
 ***************************************************************/

// Type of nut. Possible values:
// - NUT_TYPE_NONE
//   'nut_size' will be the diameter of round hole where the nut
//   had been.
// - NUT_TYPE_HEX
//   'nut_size' will be the size of the hex nut measured from
//   face to face
// - NUT_TYPE_INSERT  
//   'nut_size' will be the diameter of the insert.
nut_type        = NUT_TYPE_HEX;

// Size of the nut
nut_size        = 5.4;

// Height of the hex-nut
nut_height      = 2.25;

// The depth of the nut beneath the surface of the bottom of the
// case
nut_depth       = 0.0;

// The diameter of the head of the screw
bolt_head_size  = 5.3;

// Height of the head of the screw from its top till the shaft
bolt_head_height = 1.9;

// The diameter of the shaft of the screw
bolt_shaft_size = 3.0;

// The depth of the head beneath the surface of the case
bolt_depth      = 0.5;

/****************************************************************
 * Montage screw
 ***************************************************************/

// Size of the diameter of the hole
screw_shaft_size  = 3.5;

// Size of the diameter of the head of the screw
screw_head_size   = 8.0;

/****************************************************************
 * Wall thickness
 ***************************************************************/

// Thickness for the bottom
case_thickness_bottom    = 1.0;

// Thickness for the top
case_thickness_top       = 1.0;

// Height of the seam
case_wall_seam_height    = 0.5;

// Clearance between the seam of the top part of the case and
// bottom
case_wall_seam_clearance = 0.25;

// The thickness of the seam
case_wall_seam_thickness  = 0.5;

// The minimum size of the wall of the pilar
pilar_wall_thickness     = 0.8;

// The thickness of the wall of the strut
strut_wall_thickness     = 1.5;

// The thickness of the bottom of the strut
strut_bottom_thickness   = 2.0;

// The clearance for the head of the screw of to the strut walls
// and case [horizontal, vertical]
strut_clearance          = [0.0, 1.0];

/****************************************************************
 * Tolerance
 ***************************************************************/

// Formula to correct the diameter of a inner circle (i.e. hole)
// hint: see OpenCAD it's 'lookup' function
// parameter 'd' : original diameter
// output        : corrected diameter as to be printed
function inner_circle(d) = 0.4487428/d+1.02564288*d;

// Number of millimeters to add to the size of the nut for
// (nut_type == NUT_TYPE_NONE)
// Use positive numbers to make it wider
nut_size_tolerance_nut_type_none   = -0.1;

// Number of millimeters to add to the size of the nut for
// (nut_type == NUT_TYPE_HEX)
// Use positive number to make it wider.
nut_size_tolerance_nut_type_hex    = 0.2;

// Number of millimeters to add to the size of the nut for
// (nut_type == NUT_TYPE_INSERT)
// Use positive number to make it wider.
nut_size_tolerance_nut_type_insert = 0.0;

// Number of millimeters to add to the size of the shaft of
// the bolt
// Use positive number to make it wider.
bolt_shaft_size_tolerance          = 0.4;

// Number of millimeters to add to the size of the head of
// the bolt
// Use positive number to make it wider.
bolt_head_size_toterance           = 0.5;