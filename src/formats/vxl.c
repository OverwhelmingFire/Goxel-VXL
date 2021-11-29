/* Goxel 3D voxels editor
 *
 * copyright (c) 2016 Guillaume Chereau <guillaume@noctua-software.com>
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

// Support for Ace of Spades map files (vxl)

#include "goxel.h"
#include "file_format.h"

#define READ(type, file) \
    ({ type v; size_t r = fread(&v, sizeof(v), 1, file); (void)r; v;})

#define raise(msg) do { \
        LOG_E(msg); \
        ret = -1; \
        goto end; \
    } while (0)

static inline int AT(int x, int y, int z) {
    x = 511 - x;
    z = 63 - z;
    return x + y * 512 + z * 512 * 512;
}

static void swap_color(uint32_t v, uint8_t ret[4])
{
    uint8_t o[4];
    memcpy(o, &v, 4);
    ret[0] = o[2];
    ret[1] = o[1];
    ret[2] = o[0];
    ret[3] = 255;//o[3];
}

static int vxl_import(image_t *image, const char *path)
{
    // The algo is based on
    // https://silverspaceship.com/aosmap/aos_file_format.html
    // From Sean Barrett (the same person that wrote the code used in
    // ext_src/stb!).
    int ret = 0, size;
    int w = 512, h = 512, d = 64, x, y, z;
    uint8_t (*cube)[4] = NULL;
    uint8_t *data, *v;

    uint32_t *color;
    int i;
    int number_4byte_chunks;
    int top_color_start;
    int top_color_end;
    int bottom_color_start;
    int bottom_color_end; // exclusive
    int len_top;
    int len_bottom;

    if (!path) return -1;

    cube = calloc(w * h * d, sizeof(*cube));
    data = (void*)read_file(path, &size);
    v = data;

    for (y = 0; y < h; y++)
    for (x = 0; x < w; x++) {

        for (z = 0; z < 64; z++)
            cube[AT(x, y, z)][3] = 255;

        z = 0;
        while (true) {
            number_4byte_chunks = v[0];
            top_color_start = v[1];
            top_color_end = v[2];
            //if (top_color_start == 62) printf("(%d %d %d)", x, y, top_color_end);

            for (i = z; i < top_color_start; i++)
                cube[AT(x, y, i)][3] = 0;

            color = (uint32_t*)(v + 4);
            for (z = top_color_start; z <= top_color_end; z++) {
                CHECK(z >= 0 && z < d);
                swap_color(*color++, cube[AT(x, y, z)]);
            }

            len_bottom = top_color_end - top_color_start + 1;

            // check for end of data marker
            if (number_4byte_chunks == 0) {
                // infer ACTUAL number of 4-byte chunks from the length of the
                // color data
                v += 4 * (len_bottom + 1);
                break;
            }

            // infer the number of bottom colors in next span from chunk length
            len_top = (number_4byte_chunks-1) - len_bottom;

            // now skip the v pointer past the data to the beginning of the
            // next span
            v += v[0] * 4;

            bottom_color_end   = v[3]; // aka air start
            bottom_color_start = bottom_color_end - len_top;

            for(z = bottom_color_start; z < bottom_color_end; z++)
                swap_color(*color++, cube[AT(x, y, z)]);
        }
    }

    mesh_blit(image->active_layer->mesh, (uint8_t*)cube,
              -w / 2, -h / 2, -d / 2, w, h, d, NULL);
    if (box_is_null(image->box)) {
        bbox_from_extents(image->box, vec3_zero, w / 2, h / 2, d / 2);
    }
    free(cube);
    free(data);
    return ret;
}


static int is_surface(int x, int y, int z, uint8_t map[512][512][64])
{
    int r = 0;
   if (map[x][y][z]==0) r = 0;
   else if (z == 0) { 
       //printf("<c>");
        r = 1;
   }
   else if (x   >   0 && map[x-1][y][z]==0) { 
       //printf("<d>");
        r = 1;
   }
   else if (x+1 < 512 && map[x+1][y][z]==0) { 
       //printf("<e>");
        r = 1;
   }
   else if (y   >   0 && map[x][y-1][z]==0) { 
       //printf("<f>");
        r = 1;
   }
   else if (y+1 < 512 && map[x][y+1][z]==0) {
       //printf("<g>"); 
        r = 1;
   }
   else if (z   >   0 && map[x][y][z-1]==0) { 
       //printf("<h>");
        r = 1;
   }
   else if (z+1 <  64 && map[x][y][z+1]==0) { 
       //printf("<i>");
        r = 1;
   }
   else { 
       //printf("<j>");
        r = 0;
   }
   

    //printf("(%d,%d,%d):<%d> ", x,y,z, r);
    return r;
}


#define MAP_Z  64
static int write_map(const char *filename,
               uint8_t map[512][512][64],
               uint32_t color[512][512][64])
{        
    FILE *f = fopen(filename, "wb");
    int i,j,k;
    for (j = 0; j < 512; ++j) {
        for (i=0; i < 512; ++i) {
            /*if (!map[i][j][MAP_Z-1]) {
                    LOG_E("Cannot write to .vxl an object without water level: %d %d %d\n", i, j, MAP_Z);
                    fclose(f);
                    return ERR_EXPORT_VXL_NO_WATER;
                }8*/
            k = 0;
			
            while (k < MAP_Z) {
                int N = 0;
                int air = -1;
                int top_start = -1;
                int top_end = -1;
                int bottom_start = -1;
                int bottom_end = -1;
                

                // find the air region
                air = k;
                while (k < MAP_Z && !map[i][j][k])
                {
                    ++k;
                }
                
                // find top start&end and the next air
                
                
                
                
                top_start = k;
                
                
                if (!is_surface(i,j,k,map)) {
                    top_end = k-1;
                }
                else {                
                    if (k < MAP_Z) {
                        while (k < MAP_Z && map[i][j][k] && is_surface(i,j,k,map)) {
                            k++;
                            N++;
                        }
                        top_end = k-1;
                    }
                }
                    
                if (k < MAP_Z) {
                    while (k < MAP_Z && map[i][j][k] && !is_surface(i,j,k,map)) {
                        k++;
                    }
                    bottom_start = k;
                }
                
                if (k < MAP_Z) {
                    while (k < MAP_Z && map[i][j][k] && is_surface(i,j,k,map)) {
                        k++;
                        N++;
                    }
                    bottom_end = k-1;
                }
                
                
                if (k == MAP_Z) {
                    fputc(0,f);
                }
                else {
                    fputc(1+N,f);
                }

                fputc(top_start, f);
                fputc(top_end, f);
                fputc(air, f);

                if (top_start <= top_end) {
                    for (int z = top_start; z <= top_end; ++z) {
                        uint8_t c[4];
                        memcpy(c, &color[i][j][z], 4);
                        fputc(c[2],f);
                        fputc(c[1],f);
                        fputc(c[0],f);
                        fputc(c[3],f);
                    };
                }
                if (bottom_start != -1) {
                    for (int z = bottom_start; z <= bottom_end; ++z) {
                        uint8_t c[4];
                        memcpy(c, &color[i][j][z], 4);
                        fputc(c[2],f);
                        fputc(c[1],f);
                        fputc(c[0],f);
                        fputc(c[3],f);
                    };
                }
            }
            
        }
    }
    fclose(f);
    return 0;
}

static int export_as_vxl(const image_t *image, const char *path)
{
    uint8_t (*map)[512][512][64];
    uint32_t (*color)[512][512][64];
    const mesh_t *mesh = goxel_get_layers_mesh(image);
    mesh_iterator_t iter = {0};
    uint8_t c[4];
    int x, y, z, pos[3];
    assert(path);    
    /* Now we want to know the origin coordinates and the width-height-depth
    of the bounding box to determine which piece of model needs to be stored as map.
    The bounding box, a.k.a. bbox, one of which corners coincides with the origin
    of the coordinate system, can be seen in the editor as three blue cutting planes 
    joined together.
    
    The code of the origin of the coordinates was taken from the function gui_bbox()
    from the file \src\gui.cpp. 
    
    #TODO: throw exceptions when the bounding box has width and depth != 516 and 
    height > 64.*/
    
    int xbbox, ybbox, zbbox, w, h, d;
    image_t *image1 = goxel.image;
    float (*box)[4][4] = &image1->box;
    w = (*box)[0][0] * 2;
    h = (*box)[1][1] * 2;
    d = (*box)[2][2] * 2;
    
    if (w != 512) return ERR_EXPORT_VXL_WIDTH;
    if (h != 512) return ERR_EXPORT_VXL_HEIGHT;
    if (d > 64) return ERR_EXPORT_VXL_DEPTH;
    
    xbbox = round((*box)[3][0] - (*box)[0][0]);
    ybbox = round((*box)[3][1] - (*box)[1][1]);
    zbbox = round((*box)[3][2] - (*box)[2][2]);
    
    map = calloc(1, sizeof(*map));
    color = calloc(1, sizeof(*color));
    
    for (z = 0; z < 64; z++)
    for (y = 0; y < 512; y++)
    for (x = 0; x < 512; x++) {
        /* The original expression for the coordinates (pos[i]) of each voxel was 
        bound to the coordinates of the origin (256, 256, 32), which in fact forced
        users to put the origin of the coordinates (i.e. the bbox) at the 
        (-256,-256,-32):
        
        pos[0] = 255 - x;
        pos[1] = y - 256;
        pos[2] = 31 - z;
        */        
        pos[0] = xbbox + (512 - x);
        pos[1] = ybbox + y;
        pos[2] = zbbox + 64 -1 - z;
        
        mesh_get_at(mesh, &iter, pos, c);
        memcpy(&((*color)[x][y][z]), c, 4);
        
        if (c[3] == 0) continue;
        (*map)[x][y][z] = 1;
    }
    int res = write_map(path, *map, *color);
    
    free(map);
    free(color);
    
    if (res) return res;
    return 0;
}

FILE_FORMAT_REGISTER(vxl,
    .name = "vxl",
    .ext = "vxl\0*.vxl\0",
    .import_func = vxl_import,
    .export_func = export_as_vxl,
)
