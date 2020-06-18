use <_functions.scad>

/****************************************************************
 * File                : Configurations.scad
 *
 * Author              : Roel Drost
 * Date                : March 28th 2019
 * Version             : 1.0
 * License             : This file and artifacts created using
 *                       this file are for non comercial use
 *                       only.
 ***************************************************************/

module hex(size=undef, r=undef, d=undef) {
    if (is_number(d)) {
        hex(r=d/2);
    } else if (is_number(r)) {
        hex(size=2*cos(30)*r);
    } else {
        intersection_for(a=[0:120:240]) {
            rotate(a) square([size, size*2], true);
        }
    }
}

module rounded_square(dim, r = undef, center=undef, d=undef) {
    if (is_number(dim)) {
        rounded_square(new_vector(2, dim), r, center, d);
    } else if (is_number(d)) {
        rounded_square(new_vector(2, dim), d/2, center, undef);
    } else {
        if (r <= 0) {
            square(dim, center);
        } else {
            minkowski() {
                translate(new_vector(2, center?0:r)) circle(r=r);
                square([dim[0] - 2*r, dim[1] - 2*r], center);
            }
        }
    }
}
