typedef struct {
    float m, v, x;
} Punkt;

typedef struct {
    float x, y, z, nx, ny, nz;
} PunktNormal;

__kernel void obliczWspolrzedne(__global Punkt* aa, __global Punkt* bb, float dt, float w, int N_X, int N_Y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i > 0 && i < N_X-1 && j > 0 && j < N_Y-1) {
        float F = w * (4 * aa[i * N_Y + j].x
                     - aa[i * N_Y + (j + 1)].x - aa[i * N_Y + (j - 1)].x
                     - aa[(i + 1) * N_Y + j].x - aa[(i - 1) * N_Y + j].x);
        bb[i * N_Y + j].v = aa[i * N_Y + j].v + F / aa[i * N_Y + j].m * dt;
        bb[i * N_Y + j].x = aa[i * N_Y + j].x + bb[i * N_Y + j].v * dt;
    }

    if (i == 0 || i == N_X-1 || j == 0 || j == N_Y-1) {
        bb[i * N_Y + j].x = 0.0;
    }
}

__kernel void obliczNormalne(__global Punkt* bb, __global PunktNormal* punorm, int N_X, int N_Y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if (i > 0 && i < N_X-1 && j > 0 && j < N_Y-1) {
        float3 p0 = (float3)(i, j, bb[i * N_Y + j].x);
        float3 px1 = (float3)(i + 1, j, bb[(i + 1) * N_Y + j].x);
        float3 px2 = (float3)(i - 1, j, bb[(i - 1) * N_Y + j].x);
        float3 py1 = (float3)(i, j + 1, bb[i * N_Y + (j + 1)].x);
        float3 py2 = (float3)(i, j - 1, bb[i * N_Y + (j - 1)].x);

        float3 dx = px1 - px2;
        float3 dy = py1 - py2;

        float3 normal = cross(dx, dy);
        normal = normalize(normal);

        punorm[i * N_Y + j].x = i;
        punorm[i * N_Y + j].y = j;
        punorm[i * N_Y + j].z = bb[i * N_Y + j].x;
        punorm[i * N_Y + j].nx = normal.x;
        punorm[i * N_Y + j].ny = normal.y;
        punorm[i * N_Y + j].nz = normal.z;
    }

    if (i == 0 || i == N_X-1 || j == 0 || j == N_Y-1) {
        punorm[i * N_Y + j].x = i;
        punorm[i * N_Y + j].y = j;
        punorm[i * N_Y + j].z = bb[i * N_Y + j].x;
        punorm[i * N_Y + j].nx = 0.0;
        punorm[i * N_Y + j].ny = 0.0;
        punorm[i * N_Y + j].nz = 1.0;
    }
}