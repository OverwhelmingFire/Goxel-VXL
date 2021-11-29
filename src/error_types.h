/* Goxel 3D voxels editor
 *
 * copyright (c) 2019 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Goxel is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * Goxel is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * You should have received a copy of the GNU General Public License along with
 * goxel.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ERROR_TYPES_H
#define ERROR_TYPES_H

enum {
    ERR_EXPORT_NO_PATH_PROVIDED = -1,
    ERR_EXPORT_VXL_NO_WATER = 1,
    ERR_EXPORT_VXL_WIDTH,
    ERR_EXPORT_VXL_HEIGHT,
    ERR_EXPORT_VXL_DEPTH,
};

extern char * ERR_DESCRIPTION[];

#endif // ERROR_TYPES_H