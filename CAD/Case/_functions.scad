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
 
function is_undef(var)  = (var == undef);
function is_vector(var) = (len(var) == 0) || (var[0] + 0 != undef);
function is_string(var) = (len(var) == 0) || ((var[0] != undef) 
                          && (var[0] + 0 == undef));
function is_number(var) = (var != undef) && (len(var) == undef) 
                          && (var[0] == undef);

function new_vector(dim, fill) = [ for (i = [1 : dim]) fill ];
