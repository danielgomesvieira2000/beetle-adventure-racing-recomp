#ifndef MATRIX_TYPES_H
#define MATRIX_TYPES_H
typedef union {
    float m[4][4];
    struct {
        float xx, yx, zx, wx,
              xy, yy, zy, wy,
              xz, yz, zz, wz,
              xw, yw, zw, ww;
    };
} Mtx4F;

typedef struct Mtx_u {
    s16 i[4][4];
    s16 f[4][4];
} Mtx_u;

typedef union {
    Mtx_t m;
    Mtx_u u;
    long long int force_structure_alignment;
} uvMtx;

#define MTX_TO_PART(mtx) (*(Mtx_u*)&(mtx))
#define L2F(mtx, i1, i2) ((((s16)MTX_TO_PART(mtx).i[(i1)][(i2)] << 0x10) | ((s16)MTX_TO_PART(mtx).f[(i1)][(i2)] & 0xFFFF)) / 65536.0f)

#endif /* MATRIX_TYPES_H */
